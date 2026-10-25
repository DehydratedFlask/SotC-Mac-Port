#!/usr/bin/env python3
"""ELF-tie the bag derived ctor and the 1..4 input-device cap (E591).

Private: needs local/re/eboot.elf. Pins 0xa2afb0, its call sequence, the +0x10=1 write, the
r13<=2 cap checks and the 0xc88 allocation.
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
CTOR_SHA = "252b37a339c17ed52afe7e5cb219ff92be7b0ce71222ad377b4b1c5dfaba1d61"
HEADER = ROOT / "native/core/owner_prop_bag.hpp"
SOURCE = ROOT / "native/core/owner_prop_bag.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img, addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(0xA2AFB0, 441)).hexdigest() == CTOR_SHA, "0xa2afb0 sha256")
    for site, target, what in (
        (0xA2AFD3, 0xAC4DE0, "base ctor"),
        (0xA2B00D, 0xF90D70, "scePadInit"),
        (0xA2B023, 0xF901E0, "GetInitialUser"),
        (0xA2B02D, 0xF91300, "gesture init"),
        (0xA2B037, 0x17070, "new(0xc88)"),
        (0xA2B045, 0xA2BA50, "pad ctor"),
        (0xA2B055, 0xF90DF0, "GetLoginUserIdList"),
    ):
        require(call_target(img, site) == target, what)
    cap = bytes.fromhex("4183fd027f")
    require(img.read(0xA2B0DD, len(cap)) == cap, "cap check D")
    require(img.read(0xA2B11A, len(cap)) == cap, "cap check E")
    require(img.read(0xA2B143, 8) == bytes.fromhex("41c7471001000000"), "+0x10 = 1")
    require(img.read(0xA2B032, 5) == bytes.fromhex("bf880c0000"), "0xc88 allocation")

    header = HEADER.read_text()
    for token in ("BagUserSelection", "select_bag_users", "login_list_ok", "input",
                  "OwnerBagConstructHooks", "owner_bag_construct", "mode_10"):
        require(token in header, f"header token {token!r}")
    source = SOURCE.read_text()
    for token in ("initial_user", "i >= 2", "count >= 4", "user_ids", "set_mode_10",
                  "hooks.publish"):
        require(token in source, f"source token {token!r}")

    print(
        "OK check_fowner_bag_ctor: 0xa2afb0 hash, base/PadInit/user-service/gesture/pad calls, "
        "r13<=2 cap, +0x10=1, 0xc88 allocation; input-device bodies NOT lifted"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
