#!/usr/bin/env python3
"""ELF-tie checker for the ring dtor 0x354470 (CPU-01-SLOT408-PREREQS).

Pins the 116 B ring destructor per instruction (0x98 B elements: buffer +0x30 vs inline +0x50, holder release at +8, ring buffer vs inline storage +0x20) and re-derives slot 473's only reference (0x3571b3), the atexit callback for the object that 0x1f220 initialises.

Usage: python3 -I tools/lift/check_helper_354470.py [--no-mutate] [--tamper elf|native]
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
    ("slot 473 (0x355a00)", 0x355a00, 6487, "2ec7a932dd8bed1a4b7cfc24091e926e6068bcd295a6124e9aaa3a86dd11fcf8"),
]
SITES = [
    ("slot 473 dtor 0x354470", 0x355a00, 6487, 0x3571b3, 0x354470),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)


SPEC = {
    "name": "check_helper_354470",
    "summary": ("0x354470 (116 B ring dtor over 0x98 B elements) pinned per instruction; its frees (0x170c0), holder release (0xf590) and slot 473's atexit reference (0x3571b3) tied"),
    "families": [],
    "bodies": [
        {"name": "0x354470 ring dtor", "rva": 0x354470, "size": 116, "fde": 116, "pins": "ring_dtor_354470",
         "sha": "cf14d6ba9e4a4a8e4fd1c0dbb04ef11bc79d3fb9a5d8991db8d1775c3bb3a6d4"},
    ],
    "rips": [
    ],
    "calls": [
        ('element buffer free', 0x35449f, 0x170c0),
        ('element holder release', 0x3544a8, 0xf590),
    ],
    "jmps": [
        ('ring buffer free (tail)', 0x3544d4, 0x170c0),
    ],
    "relocs": [],
    "bytes": [
        ('`cmp rdi,[rbx+0x50]`', 0x354499, '48397b50'),
        ('`add rbx,0x98`', 0x3544ad, '4881c398000000'),
        ('`cmp [r14+0x20],rbx`', 0x3544c1, '49395e20'),
        ('`mov rdi,[rbx+0x30]`', 0x354490, '488b7b30'),
        ('`lea rdi,[rbx+8]`', 0x3544a4, '488d7b08'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_354470_ring_dtor.hpp", "helper_c": "native/core/helper_354470_ring_dtor.cpp",
        "test": "native/tests/test_helper_354470.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kRingDtor354470Rva': 3490928, 'kRingDtor354470Size': 116, 'kRingDtorElementSize': 152, 'kRingDtorElementHolderOffset': 8, 'kRingDtorElementBufferOffset': 48, 'kRingDtorElementInlineOffset': 80, 'kRingDtorInlineStorageOffset': 32},
    },
    "tokens": {
        "helper_c": ['hooks.free_type2(buffer);', 'shared_release_f590(holder, hooks.release)', 'hooks.free_type2(ring);', 'kRingDtorElementInlineOffset'],
        "test": ['CHECK(r.ok && r.elements == 3 && r.buffers_freed == 1 && r.releases_last == 1 && !r.ring_buffer_freed);', 'ring_header_init_1f220(ring);'],
        "cmake": ['core/helper_354470_ring_dtor.cpp', 'add_test(NAME helper_354470 COMMAND test_helper_354470)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
