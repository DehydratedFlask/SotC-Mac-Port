#!/usr/bin/env python3
"""ELF-tie the native owner property bag (E577).

Private: needs local/re/eboot.elf. Pins the bag factory, base ctor, four tree sentinels, the
`*(0x3305568)` republish, and the native constants.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FUNCTIONS = [
    ("bag factory", 0xA2B960, 37, "f7bcfbead2ea4c8077c5689b4d163352db520007042c729b8c5b8adc2a04b0ea"),
    ("bag derived ctor", 0xA2AFB0, 441, "252b37a339c17ed52afe7e5cb219ff92be7b0ce71222ad377b4b1c5dfaba1d61"),
    ("bag base ctor", 0xAC4DE0, 380, "73936f4546efed7d5d023d9d17b0f571858ac281a1461a412903820abf89dfee"),
]
NATIVE = ROOT / "native/core/owner_prop_bag.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for name, rva, size, sha in FUNCTIONS:
        require(hashlib.sha256(img.read(rva, size)).hexdigest() == sha, f"{name} sha256")

    require(img.read(0xA2B966, 5) == bytes.fromhex("bf78010000"), "bag allocation 0x178")
    require(img.read(0xAC4E2D, 5) == bytes.fromhex("bf30000000"), "tree node allocation 0x30")
    # Four sentinel pointers at +0x20/+0x38/+0x50/+0x68 (handles +0x18/+0x30/+0x48/+0x60).
    for off, what in ((0x69, "tree 0"), (0x93, "tree 1"), (0xBD, "tree 2"), (0xE9, "tree 3")):
        raw = img.read(0xAC4DE0 + off, 4)
        require(raw[0] == 0x48 and raw[1] == 0x89, f"{what} sentinel store")
    # lea rcx,[rip+disp32] at 0xac4f1c -> 0x3305568, then mov [rcx],rbx at 0xac4f33.
    lea = img.read(0xAC4F1C, 7)
    require(lea[0:3] == bytes.fromhex("488d0d"), "global alias lea")
    target = 0xAC4F1C + 7 + struct.unpack("<i", lea[3:7])[0]
    require(target == 0x3305568, f"global alias target {target:#x}")
    require(img.read(0xAC4F33, 3) == bytes.fromhex("488919"), "global alias store")

    header = NATIVE.read_text().lower()
    for token in (
        "kownerpropbagsize = 0x178",
        "kownerpropbagtreeaoffset = 0x48",
        "kownerpropbagtreeboffset = 0x60",
        "std::multimap",
        "insert_equal",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        "OK check_fowner_prop_bag: factory/base ctor hashes, 0x178 bag, four sentinels at "
        "+0x20/+0x38/+0x50/+0x68, *(0x3305568) republish, multimap model; RB bodies NOT lifted"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
