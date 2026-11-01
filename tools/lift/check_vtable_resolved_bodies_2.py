#!/usr/bin/env python3
"""ELF tie for the three remaining resolved vtable bodies.

For each unit: body bytes/hash, the tail/padding, the slot qword, the type-8 RELA row with matching
addend, the RIP-relative table address the body installs, the header constants, CMake targets and the
entry map.
"""

from __future__ import annotations

import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
ENTRY_MAP = ROOT / "docs/entry_map.json"
CMAKE = ROOT / "native/CMakeLists.txt"
PADDING = bytes.fromhex("90909090")

UNITS = [
    {
        "name": "13d20",
        "rva": 0x13D20,
        "size": 0x4,
        "side": None,
        "body": bytes.fromhex("89771cc3"),
        "sha": "71ce06d0aa5b8a12c09d86a62c9d2fd4f4d293dcd4e2ef33f188dad1835fbf3c",
        "vtable": 0x1594AE8,
        "slot": 0x28,
        "reloc": 0x1594B10,
        "header": ROOT / "native/core/vtable_1594ae8_slot28_13d20.hpp",
        "needles": [
            "kVtable1594ae8Slot28_13d20Rva = 0x13d20",
            "kVtable1594ae8Slot28_13d20VtableAddress = 0x1594ae8",
            "kVtable1594ae8Slot28_13d20SlotOffset = 0x28",
            "kVtable1594ae8Slot28_13d20RelocAddress = 0x1594b10",
            "kVtable1594ae8Slot28_13d20FieldOffset = 0x1c",
        ],
    },
    {
        "name": "358c0",
        "rva": 0x358C0,
        "size": 0x14,
        "side": 0x15955C0,
        "body": bytes.fromhex("488d05f9fc550148894720488b461048894710c3"),
        "sha": "17c50fadc05424b4d07f70ab3a53b2a9b0f263701c9bfb1d282165555a02bb3f",
        "vtable": 0x15955C0,
        "slot": 0x08,
        "reloc": 0x15955C8,
        "header": ROOT / "native/core/vtable_15955c0_slot08_358c0.hpp",
        "needles": [
            "kVtable15955c0Slot08_358c0Rva = 0x358c0",
            "kVtable15955c0Slot08_358c0VtableAddress = 0x15955c0",
            "kVtable15955c0Slot08_358c0SlotOffset = 0x08",
            "kVtable15955c0Slot08_358c0RelocAddress = 0x15955c8",
            "kVtable15955c0Slot08_358c0InstalledTable = 0x15955c0",
        ],
    },
    {
        "name": "c740",
        "rva": 0xC740,
        "size": 0x14,
        "side": 0x15944A0,
        "body": bytes.fromhex("488d05597d580148894720488b461048894710c3"),
        "sha": "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593",
        "vtable": 0x15D1D80,
        "slot": 0x08,
        "reloc": 0x15D1D88,
        "header": ROOT / "native/core/vtable_15d1d80_slot08_c740.hpp",
        "needles": [
            "kVtable15d1d80Slot08_c740Rva = 0xc740",
            "kVtable15d1d80Slot08_c740VtableAddress = 0x15d1d80",
            "kVtable15d1d80Slot08_c740SlotOffset = 0x08",
            "kVtable15d1d80Slot08_c740RelocAddress = 0x15d1d88",
            "kVtable15d1d80Slot08_c740InstalledTable = 0x15944a0",
        ],
    },
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    relocs = d.parse(elf)["relocs"]
    entry_map = json.loads(ENTRY_MAP.read_text())["entries"]
    cmake = CMAKE.read_text()

    for unit in UNITS:
        body = img.read(unit["rva"], unit["size"])
        require(hashlib.sha256(body).hexdigest() == unit["sha"], f"{unit['name']} body sha")
        require(body == unit["body"], f"{unit['name']} body bytes")
        require(img.read(unit["rva"] + unit["size"], 4) == PADDING, f"{unit['name']} padding")

        slot_value = struct.unpack("<Q", img.read(unit["vtable"] + unit["slot"], 8))[0]
        require(slot_value == unit["rva"], f"{unit['name']} slot qword")

        rows = [row for row in relocs if row["target"] == unit["reloc"]]
        require(len(rows) == 1, f"{unit['name']} expects one RELA row at {unit['reloc']:#x}")
        require(rows[0]["addend"] == unit["rva"], f"{unit['name']} RELA addend")

        if unit["side"] is not None:
            # The body installs its own table through a RIP-relative lea; recompute the target.
            require(body[0:2] == bytes.fromhex("488d"), f"{unit['name']} expects a lea prologue")
            displacement = struct.unpack("<i", body[3:7])[0]
            require(unit["rva"] + 7 + displacement == unit["side"], f"{unit['name']} lea target")

        header = unit["header"].read_text()
        for needle in unit["needles"]:
            require(needle in header, f"{unit['name']} header constant missing: {needle}")
        require(f"0x{unit['rva']:x}" in entry_map, f"{unit['name']} missing from entry map")

    for needle in ("vtable_1594ae8_slot28_13d20.cpp", "vtable_15955c0_slot08_358c0.cpp",
                   "vtable_15d1d80_slot08_c740.cpp", "test_vtable_resolved_bodies_2"):
        require(needle in cmake, f"cmake missing {needle}")

    print("OK check_vtable_resolved_bodies_2: 3 bodies, slots, RELA rows, lea targets and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
