#!/usr/bin/env python3
"""ELF-tie checker for the shared-release twin 0xf7b0 (CPU-01-SLOT408-PREREQS).

Pins the 73 B SharedStr dtor 0xf7b0 per instruction, proves it is byte-identical to the shared release 0xf590 except the low displacement bytes of its two allocator transfers, and re-derives the slot 454 reference (0x48432a) and slot 309's (0xa508f6).

Usage: python3 -I tools/lift/check_helper_f7b0.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_f as pins_f  # noqa: E402
from helper_f_util import slot_ties  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_f.PINS)

SLOTS = [
    ("slot 454 (0x483e40)", 0x483e40, 1431, "88217ac442e8f020453863f84a4be06faa7f6590a4820cf25222603949ba2fd5"),
    ("slot 309 (0xa505e0)", 0xa505e0, 1892, "a559b2f9a26e6ef95c647bad80f2695a6c662da6276224c908e10b6dfd1b11a5"),
]
SITES = [
    ("slot 454 dtor 0xf7b0", 0x483e40, 1431, 0x48432a, 0xf7b0),
    ("slot 309 dtor 0xf7b0", 0xa505e0, 1892, 0xa508f6, 0xf7b0),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)
    release, twin = e.raw(0xF590, 73), e.raw(0xF7B0, 73)
    differ = [i for i in range(73) if release[i] != twin[i]]
    # call 0x12af0 (E8 at +0x29) and jmp 0x170b0 (E9 at +0x3d): only the low two rel32 bytes differ
    require(release[0x29] == 0xE8 and twin[0x29] == 0xE8 and release[0x3D] == 0xE9 and twin[0x3D] == 0xE9, "0xf590 / 0xf7b0 call and jmp opcodes")
    require(differ == [0x2A, 0x2B, 0x3E, 0x3F], f"0xf7b0 differs from 0xf590 only in the rel32 low bytes, got {differ}")


SPEC = {
    "name": "check_helper_f7b0",
    "summary": ('0xf7b0 (73 B SharedStr dtor) pinned per instruction and proved a byte-identical twin of the shared release 0xf590 (only the rel32 low bytes of its call 0x12af0 and jmp 0x170b0 differ); slot 454 (0x48432a) and slot 309 (0xa508f6) references re-derived'),
    "families": [],
    "bodies": [
        {"name": "0xf7b0 shared release twin", "rva": 0xf7b0, "size": 73, "fde": 73, "pins": "shared_release_twin_f7b0",
         "sha": "1e7da61ccdc2bc47c92c291047e615af7bbdc1b131ed4e0f5ff1836119dbc676"},
    ],
    "rips": [
    ],
    "calls": [
        ('allocator destroy dispatch', 0xf7d9, 0x12af0),
    ],
    "jmps": [
        ('tail free of the control block', 0xf7ed, 0x170b0),
    ],
    "relocs": [],
    "bytes": [
        ('`lock xadd [rax], rcx`', 0xf7c9, 'f0480fc108'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_f7b0_shared_release_twin.hpp", "helper_c": "native/core/helper_f7b0_shared_release_twin.cpp",
        "test": "native/tests/test_helper_f7b0.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kSharedReleaseTwinF7b0Rva': 63408, 'kSharedReleaseTwinF7b0Size': 73},
    },
    "tokens": {
        "helper_c": ['return shared_release_f590(holder, hooks);'],
        "test": ['CHECK(ra.ok == rb.ok && ra.was_last == rb.was_last'],
        "cmake": ['core/helper_f7b0_shared_release_twin.cpp', 'add_test(NAME helper_f7b0 COMMAND test_helper_f7b0)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
