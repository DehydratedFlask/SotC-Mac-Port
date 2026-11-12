#!/usr/bin/env python3
"""ELF-tie checker for the strncpy import thunk 0xf8f630 and its startup-slot-503 consumer (CPU-01-HELPER-CALLEE-MODELS-03).

0xf8f630 is a PLT thunk (`ff 25 d2 0a 67 00`, jmp [rip+0x670ad2] -> GOT 0x1600108); the GOT slot has exactly one JMPREL row (R_X86_64_JUMP_SLOT, addend 0)
whose NID is `6sJWiWSRuqk` = the NID hash of `strncpy`, recomputed here with the same function that proves the sibling thunks 0xf8f390 (__cxa_atexit) and
0xf8f480 (memmove). So the callee is the C library function, not game or Sony SDK code, and the native contract is ISO C strncpy. The consumer, startup slot
503 (0x247f00), is pinned: its only call to the thunk is at 0x247f47 with rdi = cell+4 (cell 0x2eea480), rsi = the literal `NULL\\0` at 0xfc15f3, edx = 15, followed by
`movb $0, 0x13(%rbx)` and preceded by `movl $0, (%rbx)`.

Usage: python3 -I tools/lift/check_helper_f8f630.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_d as pins_d  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_d.PINS)

SLOT_BASE, SLOT_SIZE = 0x247F00, 3185
SLOT_SHA = "a5defd5e18330647c362a0b6cc4791a4a9a4394b4a246cae4882f3b3e5fb5ec9"
CELL = 0x2EEA480
LITERAL = 0xFC15F3


def extra(e, fams) -> None:
    # the thunk is the strncpy import, proven by NID; the siblings show the NID function is the one the other checkers use
    bp._bp_check_imports(e, ((0xF8F630, 0x1600108, "strncpy"), (0xF8F390, 0x15FFFB8, "__cxa_atexit"),
                             (0xF8F480, 0x1600030, "memmove")))
    require(e.read(0xF8F630, 6) == bytes.fromhex("ff25d20a6700"), "0xf8f630 is `jmp qword [rip + 0x670ad2]`")
    e.body_hash("slot 503 (0x247f00)", SLOT_BASE, SLOT_SIZE, SLOT_SHA, always=True)
    require(e.call(0x247F47) == 0xF8F630, "slot 503: call at 0x247f47 reaches the strncpy thunk")
    require(e.rip(0x247F20, 6, 2) == CELL, "slot 503: rbx (ebx) = the cell 0x2eea480")
    require(e.read(0x247F20, 2) == b"\x8d\x1d", "slot 503: `lea ebx, [rip + disp]` opcode")
    require(e.lea(0x247F26) == LITERAL, "slot 503: rsi = the source literal at 0xfc15f3")
    e.cstring(LITERAL, b"NULL")
    e.bytes_at(0x247F2D, "ba0f000000", "slot 503: edx = 15 (copy length)")
    e.bytes_at(0x247F35, "488d7b04", "slot 503: rdi = cell + 4 (destination)")
    e.bytes_at(0x247F41, "c703" + "00000000", "slot 503: movl $0, (%rbx) before the call")
    e.bytes_at(0x247F53, "c6431300", "slot 503: movb $0, 0x13(%rbx) after the call")
    body = e.raw(SLOT_BASE, SLOT_SIZE)
    found = [SLOT_BASE + i for i in range(SLOT_SIZE - 4) if body[i] == 0xE8
             and SLOT_BASE + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0xF8F630]
    require(found == [0x247F47], f"slot 503: exactly one call to the strncpy thunk, got {[hex(x) for x in found]}")


SPEC = {
    "name": "check_helper_f8f630",
    "summary": ("0xf8f630 is the PLT thunk of the strncpy import (GOT 0x1600108, NID 6sJWiWSRuqk recomputed); slot 503's only "
                "call (0x247f47) copies 15 bytes of \"NULL\" to cell+4"),
    "families": [],
    "bodies": [
        {"name": "slot 503 (0x247f00)", "rva": SLOT_BASE, "size": SLOT_SIZE, "fde": SLOT_SIZE, "sha": SLOT_SHA},
    ],
    "rips": [],
    "calls": [("slot 503 strncpy call", 0x247F47, 0xF8F630)],
    "jmps": [],
    "relocs": [],
    "bytes": [("0xf8f630 PLT thunk", 0xF8F630, "ff25d20a6700")],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_f8f630_strncpy.hpp", "helper_c": "native/core/helper_f8f630_strncpy.cpp",
        "test": "native/tests/test_helper_f8f630.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kStrncpyPltRva": 0xF8F630, "kStrncpyGotRva": 0x1600108, "kNullNameCellCopyLength": 15,
                     "kNullNameCellTerminatorOffset": 0x13},
    },
    "tokens": {
        "helper_h": ["char* strncpy_f8f630(char* dst, const char* src, std::size_t n) noexcept;",
                     "void store_null_name_cell_247f47(NullNameCell247f47& cell) noexcept;",
                     "static_assert(sizeof(NullNameCell247f47) == 0x14"],
        "helper_c": ["for (; i < n && src[i] != '\\0'; ++i) dst[i] = src[i];", "for (; i < n; ++i) dst[i] = '\\0';",
                     "cell.id = 0;", "strncpy_f8f630(cell.name, \"NULL\", kNullNameCellCopyLength)",
                     "cell.name[kNullNameCellTerminatorOffset - offsetof(NullNameCell247f47, name)] = '\\0';"],
        "test": ["CHECK(bytes[0x13] == 0 && bytes[4] == 'N' && bytes[7] == 'L');",
                 "CHECK(std::memcmp(buf, \"abcd\", 4) == 0 && buf[4] == 0x7f && buf[5] == 0x7f);"],
        "cmake": ["core/helper_f8f630_strncpy.cpp", "add_test(NAME helper_f8f630 COMMAND test_helper_f8f630)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
