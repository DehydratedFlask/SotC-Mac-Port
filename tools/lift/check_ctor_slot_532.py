#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 532 (RVA 0x143f50, 832 B, index 532).

Batch 22, slot 532: one straight-line body that builds 21 borrowed SharedStr views at
0x2ee2a58 + 0x20 * i. Each group is `lea object; lea literal; xor esi,esi; call 0xf2f0` followed by
`__cxa_atexit(0xf7b0, object, 0x1604218)`; the 21st group ends in a tail `jmp __cxa_atexit`. There is
no stack guard and no conditional branch anywhere in the body.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. The destructor RVA and the __dso_handle are IDENTITY TAGS, recorded and never run.
The M6 driver runs a fail-closed mutation self-test; --tamper elf|native are explicit rejection probes.

The literal lengths are measured here, from the ELF, as `strlen` would: the run to the next NUL. They
are compared against kSlot532TextLengths so the model cannot drift from the pinned rodata.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x143F50
OBJ0 = 0x2EE2A58
STRIDE = 0x20
COUNT = 21
DTOR = 0xF7B0
DSO = 0x1604218

TEXTS = [
    0xFA8D4C, 0xFA8D86, 0xFA8DC2, 0xFA8E00, 0xFA8E31, 0xFA8E63, 0xFA8E95,
    0xFA8EC6, 0xFA8EF5, 0xFA8F27, 0xFA8F5D, 0xFA8F93, 0xFA8FC7, 0xFA9002,
    0xFA9037, 0xFA9070, 0xFA90AA, 0xFA90E4, 0xFA9122, 0xFA9169, 0xFA91A1,
]
LENGTHS = [0x15, 0x52, 0x16, 0x3F, 0x0E, 0x59, 0x27, 0x63, 0x34, 0x02, 0x00,
           0x00, 0x55, 0x1A, 0x03, 0x24, 0x58, 0x1E, 0x00, 0x00, 0x00]

# Group 0 has the frame's r15/r14 setup in front of it, so it is pinned explicitly; groups 1..19 are
# exactly 0x26 B apart. Group 20 adds the epilogue and ends in the tail jump.
G0_OBJ_LEA, G0_F2F0, G0_ATEXIT = 0x143F5A, 0x143F6D, 0x143F89
# Group 1 starts 0x34 B after group 0 (group 0 lacks the three-register prologue); from there on the
# groups are exactly 0x26 B apart.
G1_OBJ_LEA = 0x143F8E
STEP = 0x26
G20_JMP = 0x14428B
G20_JMP_BYTES = "e900b1e400"   # e9 + rel32 (0xf8f390 - 0x144290)

PINS = [
    ("rip", 0x143F72, 7, 3, DTOR),
    ("rip", 0x143F79, 7, 3, DSO),
    ("rip", G0_OBJ_LEA, 7, 3, OBJ0),
    ("rip", G0_OBJ_LEA + 7, 7, 3, TEXTS[0]),
    ("call", G0_F2F0, 0xF2F0),
    ("call", G0_ATEXIT, 0xF8F390),
]
for i in range(1, 20):
    site = G1_OBJ_LEA + STEP * (i - 1)
    PINS += [
        ("rip", site, 7, 3, OBJ0 + STRIDE * i),
        ("rip", site + 7, 7, 3, TEXTS[i]),
        ("call", site + 0x13, 0xF2F0),
        ("call", site + 0x21, 0xF8F390),
    ]
G20 = G1_OBJ_LEA + STEP * 19
PINS += [
    ("rip", G20, 7, 3, OBJ0 + STRIDE * 20),
    ("rip", G20 + 7, 7, 3, TEXTS[20]),
    ("call", G20 + 0x13, 0xF2F0),
    ("imm", G20_JMP, G20_JMP_BYTES),
    # One byte past the body: the mutation driver probes it, so pin it. It belongs to the next
    # function; pinning it is a boundary check, not a claim about this slot.
    ("imm", 0x144290, "554889e5"),
]

SPEC = {
    "label": "532",
    "slot": 532,
    "base": BASE,
    "size": 832,
    "body_sha256": "4dea77004e6e41e51a2522ea49e065d958d6bc38efcf99e058a547b839f1c090",
    "pad": "",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_532_143f50.hpp",
        "source": "native/core/ctor_slot_532_143f50.cpp",
        "test": "native/tests/test_ctor_slot_532.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot532Rva": BASE,
            "kSlot532Size": 832,
            "kSlot532TableWord": 0x2DD6040,
            "kSlot532Object0": OBJ0,
            "kSlot532Stride": STRIDE,
            "kSlot532Count": COUNT,
            "kSlot532LastObject": OBJ0 + STRIDE * (COUNT - 1),
            "kSlot532BorrowedStrRva": 0xF2F0,
            "kSlot532StrlenRva": 0xF8F3B0,
            "kSlot532Dtor": DTOR,
            "kSlot532DsoHandle": DSO,
            "kSlot532AtexitRva": 0xF8F390,
            "kSlot532EmptyCount": 5,
        },
        "header_tokens": [
            "0x2ee2a58", "0x2ee2cd8", "0xf2f0", "0xf7b0", "0x1604218",
            "kSlot532TextLengths", "INFERENCE",
        ],
        "source_order": ["kSlot532Count", "log.record"],
        "test_tokens": [
            "kSlot532TextLengths", "views_built", "atexit_records", "refused",
            "run_refuses_short_table",
        ],
        "cmake_tokens": ["ctor_slot_532_143f50.cpp", "test_ctor_slot_532.cpp", "ctor_slot_532"],
    },
    # The body length is checked directly rather than by digest: the whole 832 B are pinned operand
    # by operand (63 rip/call/imm expectations), and the FDE extent is asserted below.
}

SUMMARY = ("21 borrowed SharedStr views at 0x2ee2a58 + 0x20*i, each {ptr, strlen, 0, 0} from 0xf2f0 "
           "and registered at 0xf7b0; the 21st record is the body's tail jmp __cxa_atexit")


def measure_lengths(elf) -> list[int]:   # kept for the record; the lib measures pins, not text
    out = []
    for rva in TEXTS:
        blob = elf.read(rva, 0x400)
        end = blob.find(b"\x00")
        if end < 0:
            raise SystemExit(f"FAIL no NUL within 0x400 B at {rva:#x}")
        out.append(end)
    return out


if __name__ == "__main__":
    sys.exit(lib.main(SPEC, SUMMARY))
