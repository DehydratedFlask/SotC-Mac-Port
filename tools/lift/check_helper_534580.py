#!/usr/bin/env python3
"""ELF-tie checker for the cell dtors 0x534580 / 0x13a0d0 (CPU-01-SLOT408-PREREQS).

Pins both single-pointer cell dtors per instruction and re-derives their startup-slot consumers (427 and 534).

Usage: python3 -I tools/lift/check_helper_534580.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_f as pins_f  # noqa: E402
from helper_f_util import slot_ties  # noqa: E402

lib.pins.PINS.update(pins_f.PINS)

SLOTS = [
    ("slot 427 (0x5388f0)", 0x5388f0, 4362, "aa202ea2138463351225cbf3bbf3a4b3367ada23597fd87b87bcbeb7b586747e"),
    ("slot 534 (0x1418d0)", 0x1418d0, 6207, "4eedf09f4d802ac8cbbd255206623722c2bea3f484ed3081cfc622c5581b968c"),
]
SITES = [
    ("slot 427 dtor 0x534580", 0x5388f0, 4362, 0x539016, 0x534580),
    ("slot 534 dtor 0x13a0d0", 0x1418d0, 6207, 0x142ef5, 0x13a0d0),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)


SPEC = {
    "name": "check_helper_534580",
    "summary": ('0x534580 (14 B, free type 2 when non-null, cell not cleared) and 0x13a0d0 (37 B, virtual delete via [[p]+8] then clear) pinned per instruction; consumers are slot 427 (0x539016) and slot 534 (0x142ef5)'),
    "families": [],
    "bodies": [
        {"name": "0x13a0d0 virtual-delete cell dtor", "rva": 0x13a0d0, "size": 37, "fde": 37, "pins": "cell_dtor_13a0d0",
         "sha": "527056b5d040ccceb3ac1a2fe69496b640c6e89dfadb0d5383e651af6d14fae3"},
        {"name": "0x534580 cell dtor", "rva": 0x534580, "size": 14, "fde": 14, "pins": "cell_dtor_534580",
         "sha": "0da19f8783212337c45e796a3cd67e7a0b5f231dd12c47a5ac7d5d892762add9"},
    ],
    "rips": [
    ],
    "calls": [
    ],
    "jmps": [
        ('0x534580 tail jmp free type 2', 0x534588, 0x170c0),
    ],
    "relocs": [],
    "bytes": [
        ('0x534580 `mov rdi,[rdi] ; test rdi,rdi ; je +5`', 0x534580, '488b3f4885ff7405'),
        ('0x13a0d0 `mov rdi,[rbx] ; test ; je`', 0x13a0d9, '488b3b4885ff740d'),
        ('0x13a0d0 `mov rax,[rdi] ; call [rax+8]`', 0x13a0e1, '488b07ff5008'),
        ('0x13a0d0 `mov qword [rbx],0`', 0x13a0e7, '48c70300000000'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_534580_cell_dtors.hpp", "helper_c": "native/core/helper_534580_cell_dtors.cpp",
        "test": "native/tests/test_helper_534580.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kCellDtor534580Rva': 5457280, 'kCellDtor534580Size': 14, 'kCellDtor13a0d0Rva': 1286352, 'kCellDtor13a0d0Size': 37, 'kCellDtorFreeType2Rva': 94400, 'kCellDtor13a0d0VtableSlotOffset': 8},
    },
    "tokens": {
        "helper_c": ['hooks.free_type2(pointer);', 'hooks.virtual_delete(pointer);', '*cell = nullptr;'],
        "test": ['CHECK(cell == &a);', 'cell == nullptr);'],
        "cmake": ['core/helper_534580_cell_dtors.cpp', 'add_test(NAME helper_534580 COMMAND test_helper_534580)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
