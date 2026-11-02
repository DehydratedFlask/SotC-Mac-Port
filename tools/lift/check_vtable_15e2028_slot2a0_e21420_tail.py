#!/usr/bin/env python3
"""Pin the two terminal continuations of the 0xe21420 decision contract.

Window digests, the constants they imply (missing-path vtable 0x15e2d70, slot 0xb0/0xc0/0x148),
and that both windows still live inside the 0xe21420 FDE range. Skips cleanly without the image.
"""

from __future__ import annotations

import hashlib
import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
HEADER = ROOT / "native/core/vtable_15e2028_slot2a0_e21420_tail.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

FOUND = (0xE2167E, 0x4B, "9e214c763f22348f3ee8ad05825172e9983b2385a6bff2c15613bf39866ea669")
MISSING = (0xE214F7, 0xBF, "74f92e9e4f173752e0790372dc62b5e3cf4b0f349a27efa6e62761e2e30e800e")
NULL_NAME = (0xE215CE, 0x66, "9c5d5cf69f890d817eb2fa0cf7882c1dcbcb4fea15f022279206fcb2de2bcead")
MATCH = (0xE21634, 0x20, "f8459e0501df9773b62614895ba0b27f44a6d539a4d243e88f394cbc608ae1d1")
EXIT = (0xE21654, 0x2A, "0554df070f09faffaf0645351e9f9b33e500210d9cf15be0d23e6199a9cb034e")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    header = HEADER.read_text()
    cmake = CMAKE.read_text()
    for needle in ("kE21420Found18000Rva = 0xe2167e", "kE21420Missing18000Rva = 0xe214f7",
                   "kE21420MissingVtable = 0x15e2d70", "kE21420TailSlot = 0x148",
                   "kE21420AllocationSize = 0x48", "kE21420RecordFlags = 0x40"):
        require(needle in header, f"header missing {needle}")
    for needle in ("core/vtable_15e2028_slot2a0_e21420_tail.cpp",
                   "tests/test_vtable_15e2028_slot2a0_e21420_tail.cpp",
                   "test_vtable_15e2028_slot2a0_tail"):
        require(needle in cmake, f"cmake missing {needle}")

    if not ELF.is_file() or not FDE.is_file():
        print("SKIP check_vtable_15e2028_slot2a0_e21420_tail: private image not present")
        return 0

    elf = ELF.read_bytes()
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)

    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    fdes, _duplicates = census.load_fde_ranges(FDE)

    owner = fdes.find(0xE21420)
    require(owner is not None and owner[0] == 0xE21420, "0xe21420 is not an FDE function start")
    for rva, size, sha in (MISSING, FOUND, NULL_NAME, MATCH, EXIT):
        body = image.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"0x{rva:x} window changed")
        require(owner[0] <= rva < owner[0] + owner[1],
                f"0x{rva:x} is no longer inside the 0xe21420 range")

    missing = image.read(*MISSING[:2])
    found = image.read(*FOUND[:2])
    # Missing path: allocate slot 0xb0, the vtable lea, the flag bytes and the release slot 0xc0.
    require(bytes.fromhex("ff90b0000000") in missing, "missing path lost the 0xb0 allocation call")
    require(bytes.fromhex("488d0d") in missing, "missing path lost the vtable lea")
    # Three explicit `movb $0x1` stores at +0x40, +0x42 and +0x41 (that order in the body).
    require(bytes.fromhex("41c6454001") in missing, "flag store at +0x40 missing")
    require(bytes.fromhex("41c6454201") in missing, "flag store at +0x42 missing")
    require(bytes.fromhex("41c6454101") in missing, "flag store at +0x41 missing")
    # The record's vtable and owner stores.
    require(bytes.fromhex("49894d00") in missing and bytes.fromhex("49894508") in missing,
            "record vtable/owner stores missing")
    require(bytes.fromhex("ff90c0000000") in missing, "missing path lost the 0xc0 release call")
    require(bytes.fromhex("be48000000") in missing, "missing path lost the 0x48 size immediate")
    # Found path: lookup slot 0x38, status 0x1b, tail slot 0x148 and the indirect tail jump.
    require(bytes.fromhex("ff5038") in found, "found path lost the 0x38 lookup call")
    require(bytes.fromhex("41bf1b000000") in found, "found path lost the 0x1b status")
    require(bytes.fromhex("4c8b8148010000") in found, "found path lost the 0x148 slot load")
    require(found.endswith(bytes.fromhex("41ffe0")), "found path no longer ends in the tail jmp")

    null_name = image.read(*NULL_NAME[:2])
    match = image.read(*MATCH[:2])
    exit_window = image.read(*EXIT[:2])
    # Null-name sub-tree: primary slot 0x88, the 0x38 held-value load, notify 0xd0, compare 0xc8.
    require(bytes.fromhex("ff9088000000") in null_name, "null-name lost the 0x88 primary call")
    require(bytes.fromhex("4d8b6538") in null_name, "null-name lost the +0x38 held-value load")
    require(bytes.fromhex("ff90c8000000") in null_name, "null-name lost the 0xc8 compare call")
    require(bytes.fromhex("ff90d0000000") in null_name, "null-name lost the 0xd0 notify call")
    # Match path: the record's vtable slot 0x30 plus provider slot 0x88.
    require(bytes.fromhex("ff9088000000") in match, "match lost the provider 0x88 call")
    require(bytes.fromhex("ffd0") in match or bytes.fromhex("41ffd7") in match,
            "match lost the indirect record call")
    # Shared exit: slot 0x148 and the closing indirect jump.
    require(bytes.fromhex("488b8048010000") in exit_window, "exit lost the 0x148 slot load")
    require(exit_window.endswith(bytes.fromhex("ffe0")), "exit no longer ends in the tail jmp")

    print("OK check_vtable_15e2028_slot2a0_e21420_tail: all four continuations pinned inside the "
          "0xe21420 range, with the 0xb0/0x38/0xc0/0x148 slots and the 0x48 allocation size")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
