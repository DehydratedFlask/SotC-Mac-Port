#!/usr/bin/env python3
"""ELF-tie checker for the slot 451 holder dtors 0x3b380 / 0x499e50 / 0x499e60 / 0x499e90 / 0x49c9e0 (CPU-01-SLOT408-PREREQS).

Pins the five holder-dtor bodies per instruction, ties every free (0x170b0 / 0x170c0) and the critical-section destroy thunk, and re-derives the three slot 451 consumers (0x49e454, 0x49e4da, 0x49e50e).

Usage: python3 -I tools/lift/check_helper_499e90.py [--no-mutate] [--tamper elf|native]
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
    ("slot 451 (0x49d230)", 0x49d230, 5126, "f6655116c4174f7d116b24f8f54f4beb69727ef64b920df63eaf59e45c504b59"),
]
SITES = [
    ("slot 451 dtor 0x499e50", 0x49d230, 5126, 0x49e454, 0x499e50),
    ("slot 451 dtor 0x499e60", 0x49d230, 5126, 0x49e4da, 0x499e60),
    ("slot 451 dtor 0x499e90", 0x49d230, 5126, 0x49e50e, 0x499e90),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)


SPEC = {
    "name": "check_helper_499e90",
    "summary": ('five slot 451 holder dtors (0x3b380 critsec thunk, 0x499e50 thunk, 0x499e60, 0x499e90, 0x49c9e0) pinned per instruction; every 0x170b0 / 0x170c0 free and the 0x3b380 critsec destroy tied; slot 451 consumers 0x49e454 / 0x49e4da / 0x49e50e re-derived'),
    "families": [],
    "bodies": [
        {"name": "0x499e90 node dtor", "rva": 0x499e90, "size": 74, "fde": 74, "pins": "node_dtor_499e90",
         "sha": "d84e37bc1bc1bb2d977b21b1c47508a41bc9a43e0c929bf8382f1e830ab0149a"},
        {"name": "0x3b380 critsec destroy thunk", "rva": 0x3b380, "size": 5, "fde": 5, "pins": "critsec_dtor_3b380",
         "sha": "de79f6373ce5a48f02b6ea56b3fb54c88d603d639900604b1b51ee6f78e3e565"},
        {"name": "0x499e50 thunk", "rva": 0x499e50, "size": 5, "fde": 5, "pins": "tail_dtor_499e50",
         "sha": "60bcb9ef6aa5e01eab850dddbe1d82fa1d9bc26bc13555042ed80445dba01cec"},
        {"name": "0x499e60 buffer dtor", "rva": 0x499e60, "size": 37, "fde": 37, "pins": "buffer_dtor_499e60",
         "sha": "efacf02b1973bde85152e33ed19977ff06b8682b27f694919691973b3c5f47f4"},
        {"name": "0x49c9e0 six-buffer dtor", "rva": 0x49c9e0, "size": 113, "fde": 113, "pins": "six_buffer_dtor_49c9e0",
         "sha": "8c3915d35c9dd914e87795d861dee536187538e38ffe2472ead40a387416c9de"},
    ],
    "rips": [
    ],
    "calls": [
        ('0x499e60 free type 2', 0x499e72, 0x170c0),
        ('0x499e90 free type 2 outer', 0x499eab, 0x170c0),
        ('0x499e90 free type 2 inner', 0x499eb9, 0x170c0),
        ('0x499e90 critsec', 0x499ec1, 0x3b380),
        ('0x499e90 free type 1', 0x499ec9, 0x170b0),
        ('0x49c9e0 free type 2 #0', 0x49c9f5, 0x170c0),
        ('0x49c9e0 free type 2 #1', 0x49ca06, 0x170c0),
        ('0x49c9e0 free type 2 #2', 0x49ca14, 0x170c0),
        ('0x49c9e0 free type 2 #3', 0x49ca22, 0x170c0),
        ('0x49c9e0 free type 2 #4', 0x49ca30, 0x170c0),
        ('0x49c9e0 free type 2 #5', 0x49ca3e, 0x170c0),
    ],
    "jmps": [
        ('0x3b380 jmp critsec destroy', 0x3b380, 0xf8f8c0),
        ('0x499e50 jmp 0x49c9e0', 0x499e50, 0x49c9e0),
        ('0x499e60 jmp 0x3b380', 0x499e80, 0x3b380),
        ('0x49c9e0 jmp 0x3b380', 0x49ca4c, 0x3b380),
        ('0x170b0 jmp allocator dispatch', 0x170b5, 0x12af0),
        ('0x170c0 jmp allocator dispatch', 0x170c5, 0x12af0),
    ],
    "relocs": [],
    "bytes": [
        ('0x170b0 `mov esi,1`', 0x170b0, 'be01000000'),
        ('0x170c0 `mov esi,2`', 0x170c0, 'be02000000'),
        ('0x499e90 `mov qword [r14],0`', 0x499ece, '49c70600000000'),
        ('0x49c9e0 offset 0xb8', 0x49c9e9, '488bbbb8000000'),
        ('0x49c9e0 offset 0x98', 0x49c9fa, '488bbb98000000'),
        ('0x49c9e0 offset 0x78', 0x49ca0b, '488b7b78'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_499e90_slot451_dtors.hpp", "helper_c": "native/core/helper_499e90_slot451_dtors.cpp",
        "test": "native/tests/test_helper_499e90.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kCritsecDtor3b380Rva': 242560, 'kTailDtor499e50Rva': 4824656, 'kBufferDtor499e60Rva': 4824672, 'kNodeDtor499e90Rva': 4824720, 'kSixBufferDtor49c9e0Rva': 4835808, 'kNodeDtor499e90OuterOffset': 40, 'kNodeDtor499e90InnerOffset': 8, 'kBufferDtor499e60BufferOffset': 8},
    },
    "tokens": {
        "helper_h": ['kSixBufferOffsets49c9e0 = {0xb8, 0x98, 0x78, 0x58, 0x38, 0x18}'],
        "helper_c": ['hooks.free_type1(node);', '*slot = nullptr;', 'hooks.free_type2(outer);', 'hooks.free_type2(inner);', 'hooks.free_type2(buffer);'],
        "test": ['g_log == want', 'tag("f2", buf[1]) + tag("f2", buf[2]) + tag("cs", node) + tag("f1", node)'],
        "cmake": ['core/helper_499e90_slot451_dtors.cpp', 'add_test(NAME helper_499e90 COMMAND test_helper_499e90)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
