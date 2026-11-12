#!/usr/bin/env python3
"""ELF-tie checker for the 27 byte ring-header leaf 0x1f220 (CPU-01-HELPER-CALLEE-MODELS-04).

Pins 0x1f220 per instruction (FDE 27), re-derives its nine direct callers with a whole-text scan of [0, 0xf90000), hash-pins startup
slot 473 (0x355a00) and ties its only call (0x3571ae) argument by argument: rdi = the object 0x2ef2ba8 (`lea rbx, [rip+...]` at
0x3571a4, `mov rdi, rbx`), followed by `__cxa_atexit(0x354470, object, ...)`. Nothing is executed.

Usage: python3 -I tools/lift/check_helper_1f220.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_e as pins_e  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_e.PINS)

SLOT_BASE, SLOT_SIZE, SLOT_FDE = 0x355A00, 6487, 6487
SLOT_SHA = "2ec7a932dd8bed1a4b7cfc24091e926e6068bcd295a6124e9aaa3a86dd11fcf8"
CALLERS = [0x23DA4, 0x24438, 0x29C5C, 0x3C795, 0x4B75B, 0x2563B2, 0x3571AE, 0x3C9D07, 0x710752]
OBJECT = 0x2EF2BA8


def extra(e, fams) -> None:
    e.body_hash("slot 473 (0x355a00)", SLOT_BASE, SLOT_SIZE, SLOT_SHA, always=True)
    if not e.skip_hash:   # whole-image scans are hash-style work: skipped in the byte-mutation run
        data = e.raw(0, 0xF90000)
        found = [i for i in range(len(data) - 5) if data[i] == 0xE8
                 and i + 5 + struct.unpack("<i", data[i + 1:i + 5])[0] == 0x1F220]
        require(found == CALLERS, f"direct callers of 0x1f220: {[hex(x) for x in found]}")
        body = e.raw(SLOT_BASE, SLOT_SIZE)
        in_slot = [SLOT_BASE + i for i in range(SLOT_SIZE - 4) if body[i] == 0xE8
                   and SLOT_BASE + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0x1F220]
        require(in_slot == [0x3571AE], f"slot 473 calls 0x1f220 exactly once, got {[hex(x) for x in in_slot]}")
    require(e.call(0x3571AE) == 0x1F220, "slot 473: call at 0x3571ae reaches 0x1f220")
    require(e.read(0x3571A4, 3) == b"\x48\x8d\x1d", "slot 473: `lea rbx, [rip + disp]` opcode")
    require(e.rip(0x3571A4, 7, 3) == OBJECT, "slot 473: rbx = the object 0x2ef2ba8")
    require(e.read(0x3571AB, 3) == b"\x48\x89\xdf", "slot 473: `mov rdi, rbx` before the call")
    require(e.rip(0x3571B3, 7, 3) == 0x354470, "slot 473: atexit callback 0x354470 loaded into rdi")
    require(e.read(0x3571BA, 6) == b"\x48\x89\xde\x4c\x89\xe2", "slot 473: `mov rsi, rbx ; mov rdx, r12` (object, dso handle)")
    require(e.call(0x3571C0) == 0xF8F390, "slot 473: __cxa_atexit call at 0x3571c0")


SPEC = {
    "name": "check_helper_1f220",
    "summary": ("0x1f220 (27 B ring-header leaf) pinned per instruction; nine direct callers re-derived; slot 473's only call "
                "(0x3571ae) passes the object 0x2ef2ba8 and is followed by atexit(0x354470)"),
    "families": [],
    "bodies": [
        {"name": "slot 473 (0x355a00)", "rva": SLOT_BASE, "size": SLOT_SIZE, "fde": SLOT_FDE, "sha": SLOT_SHA},
        {"name": "0x1f220 ring header", "rva": 0x1F220, "size": 27, "fde": 27, "pins": "ring_header_1f220",
         "sha": "e1020c61248d9826439112dbf8e6011bca15209efe45990f88133469bec7a1cc"},
    ],
    "rips": [],
    "calls": [("slot 473 call", 0x3571AE, 0x1F220), ("slot 473 atexit", 0x3571C0, 0xF8F390)],
    "jmps": [],
    "relocs": [],
    "bytes": [],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_1f220_ring_header.hpp", "helper_c": "native/core/helper_1f220_ring_header.cpp",
        "test": "native/tests/test_helper_1f220.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kRingHeader1f220Rva": 0x1F220, "kRingHeader1f220Size": 27, "kRingHeader1f220Slot473Site": 0x3571AE,
                     "kRingHeader1f220Slot473Object": OBJECT, "kRingHeader1f220StorageOffset": 0x28,
                     "kRingHeader1f220EndOffset": 0x2628},
    },
    "tokens": {
        "helper_c": ["base + kRingHeader1f220StorageOffset;", "base + kRingHeader1f220EndOffset;", "store(0x20, begin);",
                     "store(0x08, begin);", "store(0x00, begin);", "store(0x10, end);"],
        "test": ["CHECK(qword(0x10) == block.data() + 0x2628);", "CHECK(qword(0x20) == block.data() + 0x28);"],
        "cmake": ["core/helper_1f220_ring_header.cpp", "add_test(NAME helper_1f220 COMMAND test_helper_1f220)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
