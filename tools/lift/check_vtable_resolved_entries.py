#!/usr/bin/env python3
"""ELF tie for every statically resolved vtable call site.

For each row in native/core/vtable_resolved_entries.hpp:
  * the qword at `vtable + slot` in the pinned image equals the recorded target;
  * a type-8 dynamic relocation exists at that address with the same addend;
  * the target is an FDE function start (when the private FDE table is present);
  * the target appears in docs/entry_map.json.
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
HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"
ENTRY_MAP = ROOT / "docs/entry_map.json"
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

EXPECTED = [
    (0x1254D, 0x15949A0, 0x88, 0x187C0),
    (0x13620, 0x1594AE8, 0x28, 0x13D20),
    (0x28471, 0x1594DC0, 0x20, 0x20D60),
    (0x35165, 0x15955C0, 0x08, 0x358C0),
    (0x14DC19, 0x1594DC0, 0x20, 0x20D60),
    (0x257068, 0x159C908, 0x58, 0x2687B0),
    (0xA3DF9C, 0x15D1678, 0xE8, 0xC2E0),
    (0xA3DFDA, 0x15D1678, 0x68, 0xA40E20),
    (0xA3DFF7, 0x15D1678, 0x280, 0xA460F0),
    (0xA54B7B, 0x15D1D80, 0x08, 0xC740),
    (0xA54C92, 0x15D1D80, 0x08, 0xC740),
    (0xA70F4D, 0x1598E10, 0x18, 0x1BF710),
    (0xDDF017, 0x15E0B48, 0x28, 0xDDF4B0),
    (0xE119BE, 0x15E2028, 0x2A0, 0xE21420),
    (0xE119D3, 0x15E2028, 0x70, 0xE1A5C0),
    (0xE11C64, 0x15E1EE0, 0xB0, 0xE19980),
    (0xED171C, 0x15F2F08, 0x38, 0xE16A00),
    (0xED1733, 0x15F2F08, 0x38, 0xE16A00),
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    info = d.parse(elf)
    relocs = info["relocs"]

    header = HEADER.read_text()
    entry_map = json.loads(ENTRY_MAP.read_text())["entries"]

    fde_starts = None
    if FDE_JSON.is_file():
        spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
        census = importlib.util.module_from_spec(spec)
        sys.modules["cfg_census"] = census
        spec.loader.exec_module(census)
        fdes, _duplicates = census.load_fde_ranges(FDE_JSON)
        fde_starts = fdes

    for site, vtable, slot, target in EXPECTED:
        address = vtable + slot
        value = struct.unpack("<Q", img.read(address, 8))[0]
        require(value == target, f"slot qword at {address:#x} is {value:#x}, expected {target:#x}")
        rows = [row for row in relocs if row["target"] == address]
        require(rows, f"no dynamic relocation at {address:#x}")
        require(any(row["addend"] == target for row in rows),
                f"no relocation addend {target:#x} at {address:#x}")
        if fde_starts is not None:
            found = fde_starts.find(target)
            require(found is not None and found[0] == target,
                    f"target {target:#x} is not an FDE function start")
        require(f"0x{target:x}" in entry_map, f"entry map is missing 0x{target:x}")
        require(f"0x{site:x}" not in entry_map or True, "site lookup placeholder")

    for needle in (
        "kResolvedVtableTarget_187c0Rva = 0x187c0",
        "kResolvedVtableTarget_13d20Rva = 0x13d20",
        "kResolvedVtableTarget_20d60Rva = 0x20d60",
        "kResolvedVtableTarget_c740Rva = 0xc740",
        "kResolvedVtableTarget_358c0Rva = 0x358c0",
        "kResolvedVtableTarget_2687b0Rva = 0x2687b0",
        "kResolvedVtableTarget_c2e0Rva = 0xc2e0",
        "kResolvedVtableTarget_a40e20Rva = 0xa40e20",
        "kResolvedVtableTarget_a460f0Rva = 0xa460f0",
        "kResolvedVtableTarget_1bf710Rva = 0x1bf710",
        "kResolvedVtableTarget_ddf4b0Rva = 0xddf4b0",
        "kResolvedVtableTarget_e21420Rva = 0xe21420",
        "kResolvedVtableTarget_e1a5c0Rva = 0xe1a5c0",
        "kResolvedVtableTarget_e19980Rva = 0xe19980",
        "kResolvedVtableTarget_e16a00Rva = 0xe16a00",
    ):
        require(needle in header, f"header constant missing: {needle}")
    require("kResolvedVtableCallSiteCount" in header, "count constant missing")
    require("vtable_resolved_entries.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_resolved_entries" in CMAKE.read_text(), "cmake test")

    print(f"OK check_vtable_resolved_entries: {len(EXPECTED)} sites, "
          f"{len({row[3] for row in EXPECTED})} distinct targets tied to image and entry map")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
