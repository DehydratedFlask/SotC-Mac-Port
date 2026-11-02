#!/usr/bin/env python3
"""ELF tie for the container adapters added by the vtable-closure sweep.

Each unit pins, from the pinned image: the body bytes and their sha256, the FDE length, the slot
qword at `vtable + slot`, the single type-8 dynamic relocation with a matching addend, and the
entry-map record. The CMake sources and test target are checked too.
"""

from __future__ import annotations

import hashlib
import importlib.util
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
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

UNITS = [
    {
        "rva": 0xE1A040,
        "size": 0x81,
        "body": "554889e5415741564154534989f74189d64989fc4d85ff745d31db41837c241000b80000000074500f1f840000000000498b44240889db488b3cd8488b07ff50104885c074274889c74c89fee8ff53170085c07518498b442408488b3cd8488b07ff501889c1b0014439f1740bffc3413b5c241072ba31c05b415c415e415f5dc3",
        "sha": "c870f61d7b6cbdcfddc02b108cc34e07132635db5883de7a24e1dc3a83c85499",
        "vtable": 0x15E2028,
        "slot": 0x40,
        "reloc": 0x15E2068,
        "header": ROOT / "native/core/vtable_15e2028_slot40_e1a040.hpp",
        "needles": [
            "kVtable15e2028Slot40_e1a040Rva = 0xe1a040",
            "kVtable15e2028Slot40_e1a040SlotOffset = 0x40",
            "kVtable15e2028Slot40_e1a040RelocAddress = 0x15e2068",
        ],
    },
    {
        "rva": 0xE19F70,
        "size": 0xC5,
        "body": "554889e5415741564155415453504989f64989fc4d85f60f8493000000c745d40000000041bf0000000041837c24100074784531ed4531ff0f1f840000000000498b4424084489eb488b3cd8488b07ff50104885c074494889c74c89f6e8be54170085c0753a41f6c701742a498b442408488b3cd8488b07ff501889c38b45d4498b4c2408488b3cc1488b07ff501841b70139c3760a41b70144896dd40f1f0041ffc5453b6c241072964180e701eb034531ff4489f84883c4085b415c415d415e415f5dc3",
        "sha": "57f4254e8c30ca40cf5662af6c6ce7771b2dee153b475d263e21036abb1f6a54",
        "vtable": 0x15E2028,
        "slot": 0x38,
        "reloc": 0x15E2060,
        "header": ROOT / "native/core/vtable_15e2028_slot38_e19f70.hpp",
        "needles": [
            "kVtable15e2028Slot38_e19f70Rva = 0xe19f70",
            "kVtable15e2028Slot38_e19f70SlotOffset = 0x38",
            "kVtable15e2028Slot38_e19f70RelocAddress = 0x15e2060",
        ],
    },
    {
        "rva": 0xE19CB0,
        "size": 0x1C,
        "body": "554889e553504889fbe8f21400004889df4883c4085b5de9e4d31fff",
        "sha": "0c2290446901e5ef194505a95c1763bccd925d89aa14d5d870fc2ab96a2bdcbc",
        "vtable": 0x15E2028,
        "slot": 0x18,
        "reloc": 0x15E2040,
        "header": ROOT / "native/core/vtable_15e2028_slot18_e19cb0.hpp",
        "needles": [
            "kVtable15e2028Slot18_e19cb0Rva = 0xe19cb0",
            "kVtable15e2028Slot18_e19cb0SlotOffset = 0x18",
            "kVtable15e2028Slot18_e19cb0RelocAddress = 0x15e2040",
        ],
    },
    {
        "rva": 0xE19440,
        "size": 0x8F,
        "body": "554889e541574156415453bf080000004889f3e818dc1fff488d0dc18a7c00bf080000004989c64883c11049890ee8fddb1fff488d0dfe8a7c00bf080000004989c74883c11049890fe8e2db1fff488d0d3b8b7c004989c44889df4c89f64883c11049890c24488b03ff5010488b034889df4c89feff5010488b034889df4c89e6488b40105b415c415e415f5dffe0",
        "sha": "75f782b7955ebc62ac044dc31178a83225febe5ac4a451d5f8c1f3b0deefe84b",
        "vtable": 0x15E1EE0,
        "slot": 0x10,
        "reloc": 0x15E1EF0,
        "header": ROOT / "native/core/vtable_15e1ee0_slot10_e19440.hpp",
        "needles": [
            "kVtable15e1ee0Slot10_e19440Rva = 0xe19440",
            "kVtable15e1ee0Slot10_e19440SlotOffset = 0x10",
            "kVtable15e1ee0Slot10_e19440RelocAddress = 0x15e1ef0",
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

    fdes = None
    if FDE_JSON.is_file():
        spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
        census = importlib.util.module_from_spec(spec)
        sys.modules["cfg_census"] = census
        spec.loader.exec_module(census)
        fdes, _duplicates = census.load_fde_ranges(FDE_JSON)

    for unit in UNITS:
        rva = unit["rva"]
        body = img.read(rva, unit["size"])
        require(hashlib.sha256(body).hexdigest() == unit["sha"], f"{rva:#x} body sha")
        require(body == bytes.fromhex(unit["body"]), f"{rva:#x} body bytes")

        slot_value = struct.unpack("<Q", img.read(unit["vtable"] + unit["slot"], 8))[0]
        require(slot_value == rva, f"{rva:#x} slot qword is {slot_value:#x}")
        rows = [row for row in relocs if row["target"] == unit["reloc"]]
        require(len(rows) == 1, f"{rva:#x} expects one RELA row at {unit['reloc']:#x}")
        require(rows[0]["type"] == 8, f"{rva:#x} expects a type-8 relocation")
        require(rows[0]["addend"] == rva, f"{rva:#x} RELA addend")

        if fdes is not None:
            found = fdes.find(rva)
            require(found is not None and found[0] == rva, f"{rva:#x} is not an FDE start")
            require(found[1] == unit["size"], f"{rva:#x} FDE length {found[1]:#x}")

        header = (ROOT / unit["header"]).read_text()
        for needle in unit["needles"]:
            require(needle in header, f"{rva:#x} header constant missing: {needle}")
        require(f"0x{rva:x}" in entry_map, f"{rva:#x} missing from entry map")

    for needle in (
        'core/vtable_15e2028_slot40_e1a040.cpp',
        'core/vtable_15e2028_slot38_e19f70.cpp',
        'core/vtable_15e2028_slot18_e19cb0.cpp',
        'core/vtable_15e1ee0_slot10_e19440.cpp',
        'test_vtable_containers_e1',
    ):
        require(needle in cmake, f"cmake missing {needle}")

    print(f"OK check_vtable_containers_e1: {len(UNITS)} bodies, slots, RELA rows and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
