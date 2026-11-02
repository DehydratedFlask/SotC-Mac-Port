#!/usr/bin/env python3
"""ELF tie for the teardown and tail-free slots added by the vtable-closure sweep.

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
        "rva": 0xE52CB0,
        "size": 0xFE,
        "body": "554889e5415741565350488d05dfa579004989fe4883c010498906418b467085c07e414531ffbb080000000f1f440000498b4e60803c1900741b488b7419f84885f67411498b7e08488b07ff90d0000000418b467049ffc74883c3104863c84939cf7ccc418b465885c07e2631db6690498b4e48488b3cd94885ff740a488b07ff5010418b465848ffc34863c84839cb7cde41807e50007416498b76484885f6740d498b7e08488b07ff90c000000041807e68007416498b76604885f6740d498b7e08488b07ff90c00000004180be80000000007416498b76784885f6740d498b7e08488b07ff90c00000004c89f74883c4085b415e415f5de932c0fcff",
        "sha": "99300965a5ac345fbbf571daf47c1f6fbcc9e5729a4eb213c07e96918e558d81",
        "vtable": 0x15ED2A0,
        "slot": 0x10,
        "reloc": 0x15ED2B0,
        "header": ROOT / "native/core/vtable_15ed2a0_slot10_e52cb0.hpp",
        "needles": [
            "kVtable15ed2a0Slot10_e52cb0Rva = 0xe52cb0",
            "kVtable15ed2a0Slot10_e52cb0SlotOffset = 0x10",
            "kVtable15ed2a0Slot10_e52cb0RelocAddress = 0x15ed2b0",
        ],
    },
    {
        "rva": 0xED4860,
        "size": 0x5,
        "body": "e94b2814ff",
        "sha": "746c2303b7641ab14ef73c55f24913eeb85ba520ca9b96e585aaf5f1e8315df0",
        "vtable": 0x15F2F08,
        "slot": 0x88,
        "reloc": 0x15F2F90,
        "header": ROOT / "native/core/vtable_15f2f08_slot88_ed4860.hpp",
        "needles": [
            "kVtable15f2f08Slot88_ed4860Rva = 0xed4860",
            "kVtable15f2f08Slot88_ed4860SlotOffset = 0x88",
            "kVtable15f2f08Slot88_ed4860RelocAddress = 0x15f2f90",
        ],
    },
    {
        "rva": 0x488D0,
        "size": 0x1C,
        "body": "554889e553504889fbe8b2feffff4889df4883c4085b5de9c4e7fcff",
        "sha": "c8c3976159ccf2a7443bb4f45efe4c43d4fce7f6701056985ef39bd2185973db",
        "vtable": 0x1595B60,
        "slot": 0x18,
        "reloc": 0x1595B78,
        "header": ROOT / "native/core/vtable_1595b60_slot18_488d0.hpp",
        "needles": [
            "kVtable1595b60Slot18_488d0Rva = 0x488d0",
            "kVtable1595b60Slot18_488d0SlotOffset = 0x18",
            "kVtable1595b60Slot18_488d0RelocAddress = 0x1595b78",
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
        'core/vtable_15ed2a0_slot10_e52cb0.cpp',
        'core/vtable_15f2f08_slot88_ed4860.cpp',
        'core/vtable_1595b60_slot18_488d0.cpp',
        'test_vtable_teardown_tail',
    ):
        require(needle in cmake, f"cmake missing {needle}")

    print(f"OK check_vtable_teardown_tail: {len(UNITS)} bodies, slots, RELA rows and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
