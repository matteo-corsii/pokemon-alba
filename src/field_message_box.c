#include "global.h"
#include "menu.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "match_call.h"
#include "field_message_box.h"
#include "text_window.h"
#include "script.h"
#include "field_name_box.h"
#include "palette.h"

// Temporary, audio-only instrumentation for Lago interior field messages.
#define LAGO_FIELD_MESSAGE_DIAGNOSTIC 1

static EWRAM_DATA u8 sFieldMessageBoxMode = 0;
EWRAM_DATA u8 gWalkAwayFromSignpostTimer = 0;

static void ExpandStringAndStartDrawFieldMessage(const u8 *, bool32);
static void StartDrawFieldMessage(void);

#if LAGO_FIELD_MESSAGE_DIAGNOSTIC
static bool8 IsLagoInteriorForMessageDiagnostic(void)
{
    return (gMapHeader.mapType == MAP_TYPE_INDOOR
         && gMapHeader.regionMapSectionId == MAPSEC_ALBERA_STORICA);
}

static bool8 IsPaletteZero(const u16 *palette)
{
    u8 i;
    for (i = 0; i < 16; i++)
        if (palette[i] != 0)
            return FALSE;
    return TRUE;
}

static bool8 ArePalettesEqual(const u16 *a, const u16 *b)
{
    u8 i;
    for (i = 0; i < 16; i++)
        if (a[i] != b[i])
            return FALSE;
    return TRUE;
}

static void DiagnosticMessageBoxPaletteCheckpoint(void)
{
    const u16 *unfaded14 = &gPlttBufferUnfaded[BG_PLTT_ID(14) / sizeof(u16)];
    const u16 *unfaded15 = &gPlttBufferUnfaded[BG_PLTT_ID(15) / sizeof(u16)];
    const u16 *faded14 = &gPlttBufferFaded[BG_PLTT_ID(14) / sizeof(u16)];
    const u16 *faded15 = &gPlttBufferFaded[BG_PLTT_ID(15) / sizeof(u16)];

    // D: base tone, then PAL14/PAL15 zero-state and faded/unfaded comparison.
    PlaySE(SE_PIN);
    PlaySE(IsPaletteZero(faded14) ? SE_BOO : SE_SUCCESS);
    PlaySE(IsPaletteZero(faded15) ? SE_WALL_HIT : SE_BIKE_BELL);
    PlaySE(ArePalettesEqual(faded14, unfaded14) ? SE_SELECT : SE_SWITCH);
    PlaySE(ArePalettesEqual(faded15, unfaded15) ? SE_SELECT : SE_SWITCH);
    if (gPaletteFade.bufferTransferDisabled)
        PlaySE(SE_FAILURE);
}
#endif

void InitFieldMessageBox(void)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
    gTextFlags.canABSpeedUpPrint = FALSE;
    gTextFlags.useAlternateDownArrow = FALSE;
    gTextFlags.autoScroll = FALSE;
    gTextFlags.forceMidTextSpeed = FALSE;
}

#define tState data[0]

static void Task_DrawFieldMessage(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
#if LAGO_FIELD_MESSAGE_DIAGNOSTIC
        if (IsLagoInteriorForMessageDiagnostic())
            PlaySE(SE_PC_LOGIN); // C: state 0, before loading message-box graphics.
#endif
        if (gMsgIsSignPost)
            LoadSignPostWindowFrameGfx();
        else
            LoadMessageBoxAndBorderGfx();
#if LAGO_FIELD_MESSAGE_DIAGNOSTIC
        if (IsLagoInteriorForMessageDiagnostic())
            DiagnosticMessageBoxPaletteCheckpoint(); // D: immediately after UI load.
#endif
        task->tState++;
        break;
    case 1:
    {
        u32 nameboxWinId = GetNameboxWindowId();
        DrawDialogueFrame(0, TRUE);
        if (nameboxWinId != WINDOW_NONE)
            DrawNamebox(nameboxWinId, NAME_BOX_BASE_TILE_NUM - NAME_BOX_BASE_TILES_TOTAL, TRUE);
        task->tState++;
        break;
    }
    case 2:
        if (RunTextPrintersAndIsPrinter0Active() != TRUE)
        {
            sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
            DestroyTask(taskId);
        }
    }
}

#undef tState

static void CreateTask_DrawFieldMessage(void)
{
    u8 taskId = CreateTask(Task_DrawFieldMessage, 0x50);
#if LAGO_FIELD_MESSAGE_DIAGNOSTIC
    if (taskId != TASK_NONE && IsLagoInteriorForMessageDiagnostic())
        PlaySE(SE_WIN_OPEN); // B: Task_DrawFieldMessage created; task id is available here.
#endif
}

static void DestroyTask_DrawFieldMessage(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DrawFieldMessage);
    if (taskId != TASK_NONE)
        DestroyTask(taskId);
}

bool8 ShowFieldMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
#if LAGO_FIELD_MESSAGE_DIAGNOSTIC
    if (IsLagoInteriorForMessageDiagnostic())
        PlaySE(SE_CLICK); // A: ShowFieldMessage reached.
#endif
    ExpandStringAndStartDrawFieldMessage(str, TRUE);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    return TRUE;
}

static void Task_HidePokenavMessageWhenDone(u8 taskId)
{
    if (!IsMatchCallTaskActive())
    {
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
        DestroyTask(taskId);
    }
}

bool8 ShowPokenavFieldMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    StringExpandPlaceholders(gStringVar4, str);
    CreateTask(Task_HidePokenavMessageWhenDone, 0);
    StartMatchCallFromScript(str);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    return TRUE;
}

bool8 ShowFieldAutoScrollMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessage(str, FALSE);
    return TRUE;
}

static bool8 UNUSED ForceShowFieldAutoScrollMessage(const u8 *str)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessage(str, TRUE);
    return TRUE;
}

// Same as ShowFieldMessage, but instead of accepting a
// string arg it just prints whats already in gStringVar4
bool8 ShowFieldMessageFromBuffer(void)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    StartDrawFieldMessage();
    return TRUE;
}

static void ExpandStringAndStartDrawFieldMessage(const u8 *str, bool32 allowSkippingDelayWithButtonPress)
{
    StringExpandPlaceholders(gStringVar4, str);
    TrySpawnNamebox(gStringVar4, NAME_BOX_BASE_TILE_NUM);
    AddTextPrinterForMessage(allowSkippingDelayWithButtonPress);
    CreateTask_DrawFieldMessage();
}

static void StartDrawFieldMessage(void)
{
    AddTextPrinterForMessage(TRUE);
    CreateTask_DrawFieldMessage();
}

void HideFieldMessageBox(void)
{
    DestroyTask_DrawFieldMessage();
    ClearDialogWindowAndFrame(0, TRUE);
    DestroyNamebox();
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}

u8 GetFieldMessageBoxMode(void)
{
    return sFieldMessageBoxMode;
}

bool8 IsFieldMessageBoxHidden(void)
{
    if (sFieldMessageBoxMode == FIELD_MESSAGE_BOX_HIDDEN)
        return TRUE;
    return FALSE;
}

static void UNUSED ReplaceFieldMessageWithFrame(void)
{
    DestroyTask_DrawFieldMessage();
    DrawStdWindowFrame(0, TRUE);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}

void StopFieldMessage(void)
{
    DestroyTask_DrawFieldMessage();
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}
