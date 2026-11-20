#!/usr/bin/env python3
"""Interrupt-injection regression for the connected-map graphics transaction.

Compile the actual CallCallbacks, VBlankIntr and GPU-register functions, then
interrupt each preparation stage. Only the hardware and non-graphics services
are replaced with a small deterministic model. This verifies publication
ordering; the real emulator test covers transfer timing and the DMA budget.
"""

from __future__ import annotations

import argparse
import ctypes
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from validate_ponte_laricia_transition import functions_from, mask_comments_and_strings


SHIM = r'''
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8, bool8;
typedef uint16_t u16;
typedef uint32_t u32, bool32;
typedef int32_t s32;
typedef int8_t s8;
typedef volatile uint8_t vbool8;
typedef void (*MainCallback)(void);
#define TRUE 1
#define FALSE 0
#define GPU_REG_BUF_SIZE 0x60
#define EMPTY_SLOT 0xFF
#define REG_OFFSET_DISPCNT 0
#define REG_OFFSET_DISPSTAT 4
#define REG_OFFSET_VCOUNT 6
#define DISPSTAT_HBLANK_INTR 0x10
#define DISPSTAT_VBLANK_INTR 8
#define DISPCNT_FORCED_BLANK 0x80
#define INTR_FLAG_VBLANK 1
#define BATTLE_TYPE_LINK 1
#define BATTLE_TYPE_FRONTIER 2
#define BATTLE_TYPE_RECORDED 4
static u16 hardwareRegs[GPU_REG_BUF_SIZE/2];
#define REG_DISPCNT hardwareRegs[0]
#define REG_DISPSTAT hardwareRegs[2]
#define REG_VCOUNT hardwareRegs[3]
#define GPU_REG_BUF(offset) (*(u16 *)(&sGpuRegBuffer[offset]))
#define GPU_REG(offset) hardwareRegs[(offset)/2]
static u8 sGpuRegBuffer[GPU_REG_BUF_SIZE],sGpuRegWaitingList[GPU_REG_BUF_SIZE];
static volatile bool8 sGpuRegBufferLocked,sShouldSyncRegIE;
static volatile u16 sRegIE;
static vbool8 sVBlankGraphicsDeferred;
static struct {
    MainCallback callback1,callback2,vblankCallback;
    u32 vblankCounter1,vblankCounter2;
    bool8 inBattle;
    u16 intrCheck;
} gMain;
static u8 gWirelessCommType,gLinkVSyncDisabled,gTestRunnerEnabled;
static u32 gBattleTypeFlags,trainerCounter,*gTrainerHillVBlankCounter=&trainerCounter;
static struct { s8 pcmDmaCounter; } gSoundInfo;
static s8 gPcmDmaCounter;
static u16 INTR_CHECK;
static int audioCalls,linkCalls,receiveCalls,randomCalls,wirelessCalls,graphicsCalls,dmaCalls;
static int sourceTiles,sourceBG[3],paletteBuffer,oamBuffer,scrollBuffer;
static int hardwareTiles,hardwareBG[3],hardwarePalette,hardwareOam;
static const int *queueSources[8];
static int *queueDestinations[8],queueCount;
static int errors,checks,stage,interruptStage,allStages,callbackMode;
static void RfuVSync(void) { linkCalls++; }
static void LinkVSync(void) { linkCalls++; }
static void m4aSoundMain(void) { audioCalls++; }
static void TryReceiveLinkBattleData(void) { receiveCalls++; }
static void AdvanceRandom(void) { randomCalls++; }
static void UpdateWirelessStatusIndicatorSprite(void) { wirelessCalls++; }
static void ProcessDma3Requests(void)
{
    dmaCalls++;
    for(int i=0;i<queueCount;i++) *queueDestinations[i]=*queueSources[i];
    queueCount=0;
}
'''


TESTS = r'''
static void check(int condition,const char *message)
{
    checks++;
    if(!condition) { errors++; fprintf(stderr,"FAIL stage %d mode %d: %s\n",stage,callbackMode,message); }
}
static void enqueue(const int *src,int *dest)
{
    queueSources[queueCount]=src; queueDestinations[queueCount++]=dest;
}
static void fieldVBlank(void)
{
    graphicsCalls++;
    hardwarePalette=paletteBuffer; hardwareOam=oamBuffer;
    SetGpuReg(0x14,scrollBuffer);
}
static void checkpoint(void)
{
    int thisStage=stage++;
    if(!allStages && thisStage!=interruptStage) return;
    int previousAudio=audioCalls,previousLink=linkCalls;
    u32 previousCounter=gMain.vblankCounter1;
    REG_VCOUNT=160;
    VBlankIntr();
    check(graphicsCalls==0 && dmaCalls==0,"interrupt published incomplete graphics");
    check(!hardwareTiles && !hardwareBG[0] && !hardwareBG[1] && !hardwareBG[2]
          && !hardwarePalette && !hardwareOam && !hardwareRegs[0x14/2],"visible state changed while deferred");
    check(audioCalls==previousAudio+1 && linkCalls==previousLink+1,"audio or link service stopped");
    check(gMain.vblankCounter1==previousCounter+1 && gMain.vblankCounter2==gMain.vblankCounter1
          && trainerCounter==gMain.vblankCounter1,"VBlank counters stopped or diverged");
    check(INTR_CHECK==INTR_FLAG_VBLANK && gMain.intrCheck==INTR_FLAG_VBLANK,"interrupt acknowledgement lost");
    REG_VCOUNT=170; // main callback resumes while the screen is still in VBlank
}
static void prepareFirstHalf(void)
{
    DeferVBlankGraphicsUntilCallbacksComplete();
    checkpoint();
    sourceTiles=1; enqueue(&sourceTiles,&hardwareTiles);
    checkpoint();
    paletteBuffer=1;
    checkpoint();
    sourceBG[0]=1;
    checkpoint();
    sourceBG[1]=1;
    checkpoint();
    sourceBG[2]=1;
    checkpoint();
}
static void prepareSecondHalf(void)
{
    check(IsVBlankGraphicsDeferred(),"deferral ended between callback1 and callback2");
    enqueue(&sourceBG[0],&hardwareBG[0]);
    checkpoint();
    enqueue(&sourceBG[1],&hardwareBG[1]);
    checkpoint();
    enqueue(&sourceBG[2],&hardwareBG[2]);
    checkpoint();
    oamBuffer=1; scrollBuffer=1;
    // The fast SetGpuReg path must not leak writes when main code resumes
    // during VBlank. The buffered value will be published with the frame.
    REG_VCOUNT=170;
    SetGpuReg(0x14,1);
    check(!hardwareRegs[0x14/2],"SetGpuReg bypassed the transaction during VBlank");
    checkpoint();
}
static void prepareWholeFrame(void) { prepareFirstHalf(); prepareSecondHalf(); }
static void reset(void)
{
    memset(&gMain,0,sizeof(gMain));
    memset(hardwareRegs,0,sizeof(hardwareRegs));
    InitGpuRegManager();
    sVBlankGraphicsDeferred=FALSE;
    gMain.vblankCallback=fieldVBlank;
    gWirelessCommType=gLinkVSyncDisabled=gTestRunnerEnabled=0;
    gBattleTypeFlags=trainerCounter=0;
    audioCalls=linkCalls=receiveCalls=randomCalls=wirelessCalls=graphicsCalls=dmaCalls=0;
    sourceTiles=paletteBuffer=oamBuffer=scrollBuffer=0;
    hardwareTiles=hardwarePalette=hardwareOam=0;
    memset(sourceBG,0,sizeof(sourceBG)); memset(hardwareBG,0,sizeof(hardwareBG));
    queueCount=stage=0; INTR_CHECK=0;
}
static void runPreparedFrame(int mode,int atStage,int injectEveryStage)
{
    reset(); callbackMode=mode; interruptStage=atStage; allStages=injectEveryStage;
    if(mode==0) { gMain.callback1=prepareFirstHalf; gMain.callback2=prepareSecondHalf; }
    else if(mode==1) gMain.callback2=prepareWholeFrame; // ordinary overworld/credits producer
    else gMain.callback1=prepareWholeFrame;
    CallCallbacks();
    check(!IsVBlankGraphicsDeferred(),"deferral stuck after callbacks completed");
    check(queueCount==4,"graphics requests lost before commit");
    REG_VCOUNT=160; VBlankIntr();
    check(graphicsCalls==1 && dmaCalls==1,"complete frame not committed exactly once");
    check(hardwareTiles==1 && hardwareBG[0]==1 && hardwareBG[1]==1 && hardwareBG[2]==1
          && hardwarePalette==1 && hardwareOam==1 && hardwareRegs[0x14/2]==1,"incomplete frame after commit");
    check(queueCount==0,"completed requests left queued");
}
int run_tests(void)
{
    errors=checks=0;
    for(int mode=0;mode<3;mode++)
    {
        for(int i=0;i<10;i++) runPreparedFrame(mode,i,0);
        runPreparedFrame(mode,0,1); // work spanning several VBlanks
    }

    reset(); callbackMode=3;
    sourceTiles=paletteBuffer=oamBuffer=scrollBuffer=1;
    enqueue(&sourceTiles,&hardwareTiles);
    REG_VCOUNT=160; VBlankIntr();
    check(graphicsCalls==1 && dmaCalls==1 && hardwareTiles==1 && hardwarePalette==1,
          "ordinary non-deferred VBlank behavior changed");
    REG_VCOUNT=170; SetGpuReg(0x14,27);
    check(hardwareRegs[0x14/2]==27,"ordinary direct VBlank register update changed");

    for(int forcedAPI=0;forcedAPI<2;forcedAPI++)
    {
        reset(); DeferVBlankGraphicsUntilCallbacksComplete();
        REG_DISPCNT=DISPCNT_FORCED_BLANK; REG_VCOUNT=80;
        if(forcedAPI) SetGpuReg_ForcedBlank(0x14,31); else SetGpuReg(0x14,31);
        check(!hardwareRegs[0x14/2],"forced-blank register path bypassed deferral");
        CallCallbacks(); // also proves null callbacks cannot leave the flag stuck
        REG_DISPCNT=0; REG_VCOUNT=160; VBlankIntr();
        check(!IsVBlankGraphicsDeferred(),"null callback completion left deferral stuck");
        // fieldVBlank legitimately replaces scroll with zero; use another register
        // for the direct-path publication assertion below.
    }
    reset(); DeferVBlankGraphicsUntilCallbacksComplete();
    REG_DISPCNT=DISPCNT_FORCED_BLANK; SetGpuReg_ForcedBlank(0x18,31);
    check(!hardwareRegs[0x18/2],"forced-blank write escaped before completion");
    CallCallbacks(); REG_DISPCNT=0; REG_VCOUNT=160; VBlankIntr();
    check(hardwareRegs[0x18/2]==31,"deferred register value was discarded");
    return errors;
}
int get_check_count(void) { return checks; }
'''


def run(root: Path, compiler: str) -> dict:
    definitions = {}
    for path in ['src/main.c','src/gpu_regs.c']:
        definitions.update(functions_from((root/path).read_text()))
    seeds = {'CallCallbacks','VBlankIntr','DeferVBlankGraphicsUntilCallbacksComplete',
             'IsVBlankGraphicsDeferred','InitGpuRegManager','SetGpuReg','SetGpuReg_ForcedBlank',
             'CopyBufferedValuesToGpuRegs'}
    selected=set()
    pending=list(seeds)
    while pending:
        name=pending.pop()
        if name in selected:
            continue
        if name not in definitions:
            raise RuntimeError(f'Missing production function: {name}')
        selected.add(name)
        body=mask_comments_and_strings(definitions[name][1])
        pending.extend(n for n in re.findall(r'\b(\w+)\s*\(',body) if n in definitions and n not in selected)
    source=SHIM+'\n'+'\n'.join(definitions[n][0]+';' for n in sorted(selected))+'\n'
    source+='\n'.join(definitions[n][1] for n in sorted(selected))+'\n'+TESTS
    with tempfile.TemporaryDirectory(prefix='alba-vblank-') as directory:
        c_path=Path(directory)/'vblank.c'; library_path=Path(directory)/'vblank.so'
        c_path.write_text(source)
        build=subprocess.run([compiler,'-std=c99','-O1','-shared','-fPIC','-o',str(library_path),str(c_path)],
                             capture_output=True,text=True)
        if build.returncode:
            raise RuntimeError('Host C compilation failed:\n'+build.stderr)
        library=ctypes.CDLL(str(library_path))
        errors=library.run_tests(); checks=library.get_check_count()
    return {'status':'FAIL' if errors else 'PASS','assertions':checks,'failures':errors,
            'interrupt_scenarios':33,'source_functions':sorted(selected),
            'scope':'Publication order, direct GPU-register paths, callback completion, non-graphics services; hardware timing tested separately.'}


def main() -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repository-root',type=Path,default=Path(__file__).resolve().parents[1])
    parser.add_argument('--cc',default=os.environ.get('CC','cc'))
    parser.add_argument('--json',type=Path)
    args=parser.parse_args()
    compiler=shutil.which(args.cc)
    if compiler is None:
        parser.error(f'Host C compiler not found: {args.cc}')
    result=run(args.repository_root,compiler)
    if args.json:
        args.json.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
    return int(result['status']!='PASS')


if __name__=='__main__':
    raise SystemExit(main())
