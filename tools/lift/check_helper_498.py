#!/usr/bin/env python3
"""ELF-tie checker for the 48 six byte getters and 24 hash records of startup slot 498 (CPU-01-HELPER-CALLEE-MODELS-04).

Every getter is pinned as `b8 imm32 ; c3` with FDE length 6. Slot 498 (0x252ae0, body hash-pinned) is walked pair by pair:
`call lo ; mov ebx, eax ; call hi ; mov [cell], ebx ; mov [cell+4], eax ; mov dword [cell+8], size` (the last pair has the
stack-canary load `mov rcx, [rip+...]` after the second call). Each instruction is read through the recorded reader, so a flipped
byte in any of them fails the check. The 24 cells are contiguous (stride 12 from 0x2eec3b0), the pairs are consecutive in the body,
and the native table must carry every row as one token. Nothing is executed.

Usage: python3 -I tools/lift/check_helper_498.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
from slot_pin_util import require  # noqa: E402

SLOT_BASE, SLOT_SIZE, SLOT_FDE = 0x252AE0, 2379, 2379
SLOT_SHA = "e1e90af27d9697fafa2c2617abb24e8945eb05119d60b56ea6f6873db4e26eea"
CELLS = 0x2EEC3B0

# (lo getter, hi getter, call site of the lo getter, lo, hi, size)
TABLE = [
    (0x3295d0, 0x3295e0, 0x2530d2, 0x3afc714, 0x30e40eda, 0x28),
    (0x311530, 0x311540, 0x2530f4, 0x39d9cae8, 0x355373d, 0x8),
    (0x309b20, 0x309b30, 0x253116, 0xd1edf694, 0x51b85a57, 0x24),
    (0x373e70, 0x373e80, 0x253138, 0xaadb73f3, 0x8cacdb54, 0x28),
    (0x297310, 0x297320, 0x25315a, 0xaadb73f3, 0xc19bcbce, 0x28),
    (0x250060, 0x250070, 0x25317c, 0xaadb73f3, 0xfa0e9428, 0x28),
    (0x297b40, 0x297b50, 0x25319e, 0xaadb73f3, 0x7664dc20, 0x28),
    (0x36aca0, 0x36acb0, 0x2531c0, 0xaadb73f3, 0xb6429662, 0x28),
    (0x381840, 0x381850, 0x2531e2, 0xaadb73f3, 0x69f98817, 0x28),
    (0x298390, 0x2983a0, 0x253204, 0xaadb73f3, 0x1900d54d, 0x28),
    (0x36c920, 0x36c930, 0x253226, 0xaadb73f3, 0x17a73db4, 0x28),
    (0x31fe70, 0x31fe80, 0x253248, 0x3d2f4f61, 0xbdff2f49, 0x8),
    (0x320730, 0x320740, 0x25326a, 0x3d2f4f61, 0xf28988bd, 0x8),
    (0x3b9440, 0x3b9450, 0x25328c, 0x3d2f4f61, 0x9f0d08fd, 0x8),
    (0x249bf0, 0x249c00, 0x2532ae, 0x2a0f60ad, 0x525563b1, 0x4),
    (0x3a8570, 0x3a8580, 0x2532d0, 0x51a4998d, 0x30e40eda, 0x4),
    (0x3a93f0, 0x3a9400, 0x2532f2, 0x51a4998d, 0xdd5981f, 0x4),
    (0x3a8cb0, 0x3a8cc0, 0x253314, 0x51a4998d, 0x525563b1, 0x4),
    (0x2c0690, 0x2c06a0, 0x253336, 0xd5edd0d2, 0x30e40eda, 0x4),
    (0x2c1130, 0x2c1140, 0x253358, 0xd5edd0d2, 0xdd5981f, 0x4),
    (0x2c1ae0, 0x2c1af0, 0x25337a, 0xd5edd0d2, 0x525563b1, 0x4),
    (0x3425e0, 0x3425f0, 0x25339c, 0xec9e749b, 0xf05050d6, 0x2),
    (0x2a23c0, 0x2a23d0, 0x2533be, 0xec9e749b, 0x852af535, 0x2),
    (0x2a2c60, 0x2a2c70, 0x2533e0, 0xec9e749b, 0x413ce7a5, 0x2),
]


def extra(e, fams) -> None:
    e.body_hash("slot 498 (0x252ae0)", SLOT_BASE, SLOT_SIZE, SLOT_SHA, always=True)
    fde = lib.fde_ranges()
    require(len(TABLE) == 24, "24 records")
    expected_site = TABLE[0][2]
    for i, (lo_g, hi_g, site, lo, hi, size) in enumerate(TABLE):
        cell = CELLS + 12 * i
        require(hi_g == lo_g + 0x10, f"record {i}: hi getter is 0x10 after the lo getter")
        require(site == expected_site, f"record {i}: pair {site:#x} follows the previous one at {expected_site:#x}")
        require(e.call(site) == lo_g, f"record {i}: call lo getter {lo_g:#x}")
        require(e.read(site + 5, 2) == b"\x89\xc3", f"record {i}: mov ebx, eax")
        require(e.call(site + 7) == hi_g, f"record {i}: call hi getter {hi_g:#x}")
        j = site + 12
        if i == 23:   # stack-canary load interleaved after the second call
            require(e.read(j, 3) == b"\x48\x8b\x0d", "last record: canary load mov rcx, [rip+...]")
            j += 7
        require(e.read(j, 2) == b"\x89\x1d" and e.rip(j, 6, 2) == cell, f"record {i}: mov [{cell:#x}], ebx")
        require(e.read(j + 6, 2) == b"\x89\x05" and e.rip(j + 6, 6, 2) == cell + 4, f"record {i}: mov [{cell + 4:#x}], eax")
        require(e.read(j + 12, 2) == b"\xc7\x05" and e.rip(j + 12, 10, 2) == cell + 8, f"record {i}: mov dword [{cell + 8:#x}], size")
        require(struct.unpack("<I", e.read(j + 18, 4))[0] == size, f"record {i}: size immediate {size:#x}")
        expected_site = j + 22
        if fde is not None:
            require(fde.get(hex(lo_g)) == 6 and fde.get(hex(hi_g)) == 6, f"record {i}: getter FDE lengths are 6")
        for g, value in ((lo_g, lo), (hi_g, hi)):
            require(e.read(g, 1) == b"\xb8" and struct.unpack("<I", e.read(g + 1, 4))[0] == value and e.read(g + 5, 1) == b"\xc3",
                    f"getter {g:#x} is mov eax, {value:#x} ; ret")
    require(len({r[0] for r in TABLE} | {r[1] for r in TABLE}) == 48, "48 distinct getters")


def getter_bytes() -> list:
    rows = []
    for lo_g, hi_g, _site, lo, hi, _size in TABLE:
        for g, v in ((lo_g, lo), (hi_g, hi)):
            rows.append((f"getter {g:#x} mov eax, imm ; ret", g, "b8" + struct.pack("<I", v).hex() + "c3"))
    return rows


def row_token(r) -> str:
    lo_g, hi_g, site, lo, hi, size = r
    return f"{{{lo_g:#x}, {hi_g:#x}, {site:#x}, {lo:#x}u, {hi:#x}u, {size:#x}u}},"


SPEC = {
    "name": "check_helper_498",
    "summary": ("slot 498's 48 six byte getters (`mov eax, imm ; ret`, FDE 6) pinned; its 24 call pairs and 12 byte record stores "
                "(cells 0x2eec3b0..0x2eec4d0) walked instruction by instruction and tied to the native table row by row"),
    "families": [],
    "bodies": [
        {"name": "slot 498 (0x252ae0)", "rva": SLOT_BASE, "size": SLOT_SIZE, "fde": SLOT_FDE, "sha": SLOT_SHA},
    ],
    "rips": [],
    "calls": [(f"slot 498 call lo {i}", r[2], r[0]) for i, r in enumerate(TABLE)],
    "jmps": [],
    "relocs": [],
    "bytes": getter_bytes(),
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_498_member_hashes.hpp", "helper_c": "native/core/helper_498_member_hashes.cpp",
        "test": "native/tests/test_helper_498.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kMemberHashRecordCount498": 24, "kMemberHashRecordStride498": 12, "kMemberHashCellsRva498": CELLS,
                     "kMemberHashCellsBytes498": 0x120, "kMemberHashSlot498Rva": SLOT_BASE, "kMemberHashGetterSize498": 6,
                     "kMemberHashGetterSpacing498": 0x10},
    },
    "tokens": {
        "helper_c": [row_token(r) for r in TABLE] + ["std::memcpy(cell + 0, &r.lo, 4);", "std::memcpy(cell + 4, &r.hi, 4);",
                                                    "std::memcpy(cell + 8, &r.size, 4);"],
        "test": ["CHECK(getters.size() == 48);", "CHECK(kMemberHashRecords498[23].lo_getter_rva == 0x2a2c60"],
        "cmake": ["core/helper_498_member_hashes.cpp", "add_test(NAME helper_498 COMMAND test_helper_498)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
