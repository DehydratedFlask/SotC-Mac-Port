#!/usr/bin/env python3
"""ELF-tie checker for the 7 byte store-one leaf 0x8d650 (CPU-01-HELPER-CALLEE-MODELS-03).

Pins 0x8d650 (`c7 07 01 00 00 00 ; c3`, FDE 7) instruction by instruction and ties it to its only caller found by a whole-image direct-call scan, startup
slot 497 (0x262a80): the call at 0x262ab1 passes rdi = the cell 0x2eec4d8 (`lea edi, [rip+...]` at 0x262aa0), exactly one call to 0x8d650 occurs in
the slot body, and the two stores that follow (0x780 / 0x438 at 0x2eec4e0 / 0x2eec4e4, 0 at 0x2eec4e8) are pinned. Nothing is executed.

Usage: python3 -I tools/lift/check_helper_8d650.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_d as pins_d  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_d.PINS)

SLOT_BASE, SLOT_SIZE, SLOT_FDE = 0x262A80, 9164, 9164
SLOT_SHA = "629685642a828585b97e375f91d0a01551f1262125230769f85937a92ef8a2df"
CELL = 0x2EEC4D8


def extra(e, fams) -> None:
    e.body_hash("slot 497 (0x262a80)", SLOT_BASE, SLOT_SIZE, SLOT_SHA, always=True)
    require(e.call(0x262AB1) == 0x8D650, "slot 497: call at 0x262ab1 reaches 0x8d650")
    require(e.rip(0x262AA0, 6, 2) == CELL, "slot 497: rdi (edi) = the cell 0x2eec4d8 at the call")
    require(e.read(0x262AA0, 2) == b"\x8d\x3d", "slot 497: `lea edi, [rip + disp]` opcode")
    body = e.raw(SLOT_BASE, SLOT_SIZE)
    found = [SLOT_BASE + i for i in range(SLOT_SIZE - 4) if body[i] == 0xE8
             and SLOT_BASE + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0x8D650]
    require(found == [0x262AB1], f"slot 497: exactly one call to 0x8d650, got {[hex(x) for x in found]}")
    # the three dword stores that follow the call (rip operands + immediates)
    for site, length, imm, target in ((0x262AC6, 10, 0x780, 0x2EEC4E0), (0x262AD0, 10, 0x438, 0x2EEC4E4),
                                       (0x262ADA, 10, 0x0, 0x2EEC4E8)):
        require(e.read(site, 2) == b"\xc7\x05", f"slot 497: movl $imm, [rip+disp] at {site:#x}")
        require(e.rip(site, length, 2) == target, f"slot 497: store at {site:#x} -> {target:#x}")
        require(struct.unpack("<I", e.read(site + 6, 4))[0] == imm, f"slot 497: store immediate at {site:#x} is {imm:#x}")


SPEC = {
    "name": "check_helper_8d650",
    "summary": ("0x8d650 (7 B, sha bf063ac8...a8fc) pinned per instruction; slot 497's only call (0x262ab1) passes the cell "
                "0x2eec4d8 and is the only call in the body"),
    "families": [],
    "bodies": [
        # first so that `--tamper elf` (which flips a byte at bodies[0].rva + 0x10) lands inside a body larger than the 7 byte leaf
        {"name": "slot 497 (0x262a80)", "rva": SLOT_BASE, "size": SLOT_SIZE, "fde": SLOT_FDE, "sha": SLOT_SHA},
        {"name": "0x8d650 store-one leaf", "rva": 0x8D650, "size": 7, "fde": 7, "pins": "store_one_8d650",
         "sha": "bf063ac89841f517f4b1d340f03cf0cd2ca2afe1d5b52f2d6740c4c0475ea8fc"},
    ],
    "rips": [],
    "calls": [("slot 497 call", 0x262AB1, 0x8D650)],
    "jmps": [],
    "relocs": [],
    "bytes": [("0x8d650 `mov dword [rdi], 1`", 0x8D650, "c70701000000"), ("0x8d650 `ret`", 0x8D656, "c3")],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_8d650_store_one.hpp", "helper_c": "native/core/helper_8d650_store_one.cpp",
        "test": "native/tests/test_helper_8d650.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kStoreOne8d650Rva": 0x8D650, "kStoreOne8d650Size": 7, "kStoreOne8d650Value": 1},
    },
    "tokens": {
        "helper_h": ["void store_one_8d650(std::uint32_t& cell) noexcept;"],
        "helper_c": ["cell = kStoreOne8d650Value;"],
        "test": ["store_one_8d650(d.cell);", "CHECK(d.cell == 1 && d.w == 0x780 && d.h == 0x438 && d.tail == 0xcafef00du);"],
        "cmake": ["core/helper_8d650_store_one.cpp", "add_test(NAME helper_8d650 COMMAND test_helper_8d650)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
