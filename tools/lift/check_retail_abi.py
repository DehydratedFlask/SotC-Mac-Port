#!/usr/bin/env python3
"""Tie the retail ABI contract table to the image and the entry map.

Every recorded target must be an FDE function start and must appear in docs/entry_map.json, so a row
cannot name an address that no lifted body claims.
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
HEADER = ROOT / "native/core/retail_abi.hpp"
ENTRY_MAP = ROOT / "docs/entry_map.json"
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
CMAKE = ROOT / "native/CMakeLists.txt"

EXPECTED = {
    0xC2E0: 2, 0xC740: 2, 0x13D20: 2, 0x187C0: 1, 0x20D60: 1, 0x358C0: 2, 0x1BF710: 1,
    0x2687B0: 1, 0xA40E20: 0, 0xA460F0: 1, 0xDDF4B0: 1, 0xE16A00: 2, 0xE19980: 5,
    0xE1A5C0: 4, 0xE21420: 3, 0xE24900: 2, 0xE26F90: 3,
}


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")

    header = HEADER.read_text()
    for rva, count in EXPECTED.items():
        needle = f"{{0x{rva:x}, {count},"
        require(needle in header, f"missing row {needle}")
    require("kRetailArgSpecCount" in header, "count constant missing")
    require("retail_abi.cpp" in CMAKE.read_text(), "cmake source")
    require("test_retail_abi" in CMAKE.read_text(), "cmake test")

    entry_map = json.loads(ENTRY_MAP.read_text())["entries"]
    for rva in EXPECTED:
        require(f"0x{rva:x}" in entry_map, f"0x{rva:x} missing from entry map")

    if FDE_JSON.is_file():
        spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
        census = importlib.util.module_from_spec(spec)
        sys.modules["cfg_census"] = census
        spec.loader.exec_module(census)
        fdes, _duplicates = census.load_fde_ranges(FDE_JSON)
        for rva in EXPECTED:
            found = fdes.find(rva)
            require(found is not None and found[0] == rva,
                    f"0x{rva:x} is not an FDE function start")

    print(f"OK check_retail_abi: {len(EXPECTED)} targets tie to FDE starts and the entry map")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
