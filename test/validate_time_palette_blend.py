#!/usr/bin/env python3
"""Compare production time-palette optimizations with the weighted C path.

Python 3 and a host C compiler are sufficient; no ROM or generated assets are
required. The functions, blend structure and color macros are extracted from
the repository at test time. Exhaustive source-color cases cover both high-bit
states, alternate palette lighting and coefficient extremes. Additional cases
exercise sparse palette masks, in-place buffers and mixed-weight dispatch.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


def extract_function(source: str, name: str) -> str:
    masked = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/',
                    lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), source)
    match = re.search(r'^(?:static )?void\s+' + re.escape(name)
                      + r'\s*\([^;{}]*\)\s*\n\{', masked, re.M)
    if match is None:
        raise ValueError(f'Production function not found: {name}')
    brace = masked.index('{', match.start())
    depth, end = 1, brace + 1
    while depth:
        depth += (masked[end] == '{') - (masked[end] == '}')
        end += 1
    return source[match.start():end]


SHIM = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8, bool8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
static unsigned singleCalls, weightedCalls, lookupCalls;
'''


TEST = r'''
#define GUARD 8
#define WORDS 512
static u16 input[GUARD + WORDS + GUARD];
static u16 reference[GUARD + WORDS + GUARD];
static u16 actual[GUARD + WORDS + GUARD];
static unsigned long long cases, comparedWords, endpointCases, equalCases, mixedCases, maskCases, lookupCases;

static void compare(u32 mask, unsigned palettes, struct BlendSettings b0,
                    struct BlendSettings b1, u16 weight, unsigned expectedPath)
{
    unsigned words = palettes * 16, total = words + GUARD * 2;
    unsigned inPlace, i;

    for (inPlace = 0; inPlace < 2; inPlace++)
    {
        for (i = 0; i < total; i++)
            reference[i] = actual[i] = (u16)(0xA53C ^ (i * 73));
        if (inPlace)
        {
            memcpy(reference + GUARD, input + GUARD, words * sizeof(u16));
            memcpy(actual + GUARD, input + GUARD, words * sizeof(u16));
        }
        TimeMixPalettesWeighted(mask, inPlace ? reference + GUARD : input + GUARD,
                               reference + GUARD, &b0, &b1, weight);
        singleCalls = weightedCalls = lookupCalls = 0;
        TimeMixPalettes(mask, inPlace ? actual + GUARD : input + GUARD,
                        actual + GUARD, &b0, &b1, weight);
        if (singleCalls != (expectedPath == 1) || weightedCalls != (expectedPath == 2)
         || lookupCalls != (expectedPath == 3))
        {
            fprintf(stderr, "Dispatch mismatch: case %llu mask %08x weight %u path %u calls %u/%u/%u\n",
                    cases, mask, weight, expectedPath, singleCalls, weightedCalls, lookupCalls);
            exit(1);
        }
        if (memcmp(reference, actual, total * sizeof(u16)))
        {
            for (i = 0; i < total; i++)
                if (reference[i] != actual[i])
                {
                    fprintf(stderr, "Color mismatch: case %llu mask %08x weight %u inplace %u word %u expected %04x actual %04x\n",
                            cases, mask, weight, inPlace, i, reference[i], actual[i]);
                    exit(1);
                }
        }
        for (i = 0; i < GUARD; i++)
            if (actual[i] != (u16)(0xA53C ^ (i * 73))
             || actual[GUARD + words + i] != (u16)(0xA53C ^ ((GUARD + words + i) * 73)))
            {
                fprintf(stderr, "Destination guard modified\n");
                exit(1);
            }
        cases++;
        comparedWords += words;
    }
}

int main(void)
{
    const struct BlendSettings settings[] = {
        {.blendColor = 0x9D7474, .isTint = 1, .coeff = 10},
        {.blendColor = 0xA8B0E0, .isTint = 1, .coeff = 4},
        {.blendColor = 0x000000, .isTint = 1, .coeff = 0},
        {.blendColor = 0xFFFFFF, .isTint = 1, .coeff = 31},
        {.blendColor = 0x00FF80, .isTint = 1, .coeff = 16},
        {.blendColor = 0,        .isTint = 0, .coeff = 0},
        {.blendColor = 0,        .isTint = 0, .coeff = 1},
        {.blendColor = 0x7FFF,   .isTint = 0, .coeff = 16},
        {.blendColor = 0x3F9F,   .isTint = 0, .coeff = 8},
        {.blendColor = 0,        .isTint = 0, .coeff = 31},
        {.blendColor = 0x7FFF,   .isTint = 0, .coeff = 31},
        {.blendColor = 0x991234, .isTint = 0, .coeff = 31},
    };
    const u16 colorZeros[] = {0, 0x1234, 0x8000, 0xFFFF, 0x803F};
    const u16 weights[] = {1, 32, 64, 127, 128, 192, 255};
    const u32 masks[] = {0, 1, 0x1FC0, 0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA, 0x80000001, 0x80000003};
    const unsigned manyBanks[] = {0, 0, 1, 1, 0, 1, 1, 0, 1};
    const unsigned settingCount = sizeof(settings) / sizeof(settings[0]);
    unsigned t, z, base, j, w, m, coeff, coeff1;

    // A zero mask must return before dereferencing any caller buffer/settings.
    TimeMixPalettes(0, NULL, NULL, NULL, NULL, 128);

    // Every u16 source word, including every RGB555 value with and without its
    // high bit, under both endpoints and both default/alternate palette lights.
    for (t = 0; t < settingCount; t++)
        for (z = 0; z < sizeof(colorZeros) / sizeof(colorZeros[0]); z++)
            for (base = 0; base < 65536; base += 15)
            {
                struct BlendSettings selected = settings[t];
                struct BlendSettings other = settings[(t + 1) % settingCount];
                input[GUARD] = colorZeros[z];
                for (j = 1; j < 16; j++)
                    input[GUARD + j] = (u16)(base + j - 1);
                compare(1, 1, selected, other, 256, 1);
                compare(1, 1, other, selected, 0, 1);
                endpointCases += 4;
            }

    // Equal effective settings must stay exact at intermediate weights, even
    // when unused fields or fields irrelevant to this blend type differ.
    for (t = 0; t < settingCount; t++)
        for (z = 0; z < 2; z++)
            for (base = 0; base < 65536; base += 15)
            {
                struct BlendSettings first = settings[t], second = settings[t];
                second.unused = 3;
                if (second.isTint)
                    second.coeff ^= 31;
                else
                    second.blendColor ^= 0xFF8000;
                input[GUARD] = z ? 0xFFFF : 0x1234;
                for (j = 1; j < 16; j++)
                    input[GUARD + j] = (u16)(base + j - 1);
                compare(1, 1, first, second, 1, 1);
                compare(1, 1, first, second, 128, 1);
                compare(1, 1, first, second, 255, 1);
                equalCases += 6;
            }

    // Every source word also goes through the lookup path at mixed weights.
    // Three banks cover 45 nonzero-index source entries in each comparison.
    for (t = 0; t < settingCount; t++)
        for (z = 0; z < sizeof(colorZeros) / sizeof(colorZeros[0]); z++)
            for (base = 0; base < 65536; base += 45)
            {
                struct BlendSettings first = settings[t];
                struct BlendSettings second = settings[(t + 1) % settingCount];
                for (j = 0; j < 48; j++)
                    input[GUARD + j] = j % 16 ? (u16)(base + (j / 16) * 15 + j % 16 - 1) : colorZeros[z];
                for (w = 0; w < sizeof(weights) / sizeof(weights[0]); w++)
                {
                    compare(7, 3, first, second, weights[w], 3);
                    lookupCases += 2;
                }
            }

    // Sparse masks, including palette 31, retain unselected destination words.
    // Masks on both sides of the two/three-bank threshold check dispatch.
    for (t = 0; t < settingCount; t++)
        for (z = 0; z < sizeof(colorZeros) / sizeof(colorZeros[0]); z++)
            for (m = 0; m < sizeof(masks) / sizeof(masks[0]); m++)
            {
                struct BlendSettings first = settings[t];
                struct BlendSettings second = settings[(t + 1) % settingCount];
                for (j = 0; j < WORDS; j++)
                    input[GUARD + j] = j % 16 ? (u16)(j * 257 + t * 991 + z * 17) : colorZeros[z];
                compare(masks[m], 32, first, second, 0, masks[m] ? (manyBanks[m] ? 3 : 1) : 0);
                compare(masks[m], 32, first, second, 256, masks[m] ? (manyBanks[m] ? 3 : 1) : 0);
                maskCases += 4;
                for (w = 0; w < sizeof(weights) / sizeof(weights[0]); w++)
                {
                    compare(masks[m], 32, first, second, weights[w], masks[m] ? (manyBanks[m] ? 3 : 2) : 0);
                    mixedCases += 2;
                }
            }

    // Exercise every representable coefficient, including extrapolation at31.
    for (coeff = 0; coeff < 32; coeff++)
    {
        struct BlendSettings first = {.blendColor = 0x21AD, .coeff = coeff};
        struct BlendSettings second = {.blendColor = 0x6B52, .coeff = 31 - coeff};
        for (j = 0; j < WORDS; j++)
            input[GUARD + j] = (u16)(j * 131 + coeff * 337);
        compare(0xFFFFFFFF, 32, first, second, 256, 3);
        compare(0xFFFFFFFF, 32, first, second, 0, 3);
        compare(0xFFFFFFFF, 32, first, second, 127, 3);
        maskCases += 4;
        mixedCases += 2;
    }

    // All weight values and coefficient pairs exercise the lookup's rounding.
    for (w = 0; w <= 256; w++)
    {
        struct BlendSettings first = settings[0], second = settings[8];
        for (j = 0; j < WORDS; j++)
            input[GUARD + j] = (u16)(j * 131 + w * 337);
        compare(0x80000003, 32, first, second, w, 3);
        lookupCases += 2;
    }
    for (coeff = 0; coeff < 32; coeff++)
        for (coeff1 = 0; coeff1 < 32; coeff1++)
        {
            struct BlendSettings first = {.blendColor = 0x21AD, .coeff = coeff};
            struct BlendSettings second = {.blendColor = 0x6B52, .coeff = coeff1};
            for (j = 0; j < 48; j++)
                input[GUARD + j] = (u16)(j * 991 + coeff * 337 + coeff1 * 131);
            compare(7, 3, first, second, 0, 3);
            compare(7, 3, first, second, 127, 3);
            compare(7, 3, first, second, 256, 3);
            lookupCases += 6;
        }
    // Out-of-range weights retain the original weighted behavior.
    for (w = 257; w <= 65535; w += 65278)
    {
        struct BlendSettings first = settings[0], second = settings[8];
        for (j = 0; j < WORDS; j++)
            input[GUARD + j] = (u16)(j * 257 + w);
        compare(0x1FC0, 32, first, second, w, 2);
        mixedCases += 2;
    }
    printf("{\"pass\":true,\"buffer_comparisons\":%llu,\"words_compared\":%llu,"
           "\"endpoint_comparisons\":%llu,\"equal_setting_comparisons\":%llu,"
           "\"mixed_weight_comparisons\":%llu,\"mask_and_coefficient_comparisons\":%llu,"
           "\"lookup_comparisons\":%llu,"
           "\"source_u16_values\":65536,\"destination_guards_intact\":true}\n",
           cases, comparedWords, endpointCases, equalCases, mixedCases, maskCases, lookupCases);
    return 0;
}
'''


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repository-root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--cc', default=os.environ.get('CC', 'cc'))
    parser.add_argument('--json', type=Path, help='Optional JSON result path')
    args = parser.parse_args()
    compiler = shutil.which(args.cc)
    if compiler is None:
        parser.error(f'Host C compiler not found: {args.cc}')
    root = args.repository_root
    palette = (root / 'src/palette.c').read_text()
    header = (root / 'include/palette.h').read_text()
    rgb = (root / 'include/constants/rgb.h').read_text()
    structure = re.search(r'struct BlendSettings\s*\{[^}]*\};', header)[0]
    macros = '\n'.join(line for line in rgb.splitlines()
                       if line.startswith(('#define RGB2(', '#define RGB2GBA(')))
    default = re.search(r'^#define DEFAULT_LIGHT_COLOR[^\n]*', palette, re.M)[0]
    functions = [extract_function(palette, name) for name in
                 ('TimeMixPalettesSingle', 'TimeMixPalettesWeighted', 'TimeMixPalettesLookup', 'TimeMixPalettes')]
    original_hashes = {name: hashlib.sha256(text.encode()).hexdigest()
                       for name, text in zip(('single', 'weighted', 'lookup', 'public'), functions)}
    functions[0] = functions[0].replace('{', '{\n    singleCalls++;', 1)
    functions[1] = functions[1].replace('{', '{\n    weightedCalls++;', 1)
    functions[2] = functions[2].replace('{', '{\n    lookupCalls++;', 1)
    source = '\n'.join([SHIM, structure, macros, default, *functions, TEST])
    with tempfile.TemporaryDirectory(prefix='alba-time-palette-') as folder:
        directory = Path(folder)
        harness = directory / 'harness.c'
        executable = directory / 'harness'
        harness.write_text(source)
        subprocess.run([compiler, '-std=c99', '-O2', str(harness), '-o', str(executable)], check=True)
        run = subprocess.run([str(executable)], text=True, capture_output=True)
        if run.returncode:
            raise RuntimeError(run.stderr.strip() or run.stdout.strip())
        result = json.loads(run.stdout)
    result['production_function_sha256'] = original_hashes
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
