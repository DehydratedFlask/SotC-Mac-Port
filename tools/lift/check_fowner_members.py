#!/usr/bin/env python3
"""ELF-tie the 0x2569d0 member-install driver (E579).

Private: needs local/re/eboot.elf. Pins the 20 new-site call targets (19 x 0x17070, 1 factory),
the three replace/destroy idioms, row 7's unconditional store, and the native row table.
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
NEW_SITES = [
    0x256A15, 0x256A49, 0x256B70, 0x256BAE, 0x256BC7, 0x256BFB, 0x256C2D,
    0x256C4C, 0x256C65, 0x256CC8, 0x256D01, 0x256D36, 0x256D83, 0x256DC1,
    0x256DFF, 0x256E3D, 0x256E77, 0x256F5C, 0x256F9D, 0x257029, 0x257070,
]
HEADER = ROOT / "native/core/owner_members.hpp"
SOURCE = ROOT / "native/core/owner_members.cpp"


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

    for site in NEW_SITES:
        expected = 0xA2B960 if site == 0x256D01 else 0x17070
        require(call_target(img, site) == expected, f"allocation call at {site:#x}")

    # Idiom (a): test old / je / load vptr / call [vptr+8] / store.
    a = bytes.fromhex("4885ff7406488b07ff50084d893c24")
    require(img.read(0x256A35, len(a)) == a, "virtual-dtor replace idiom")
    # Idiom (b): test / je / direct dtor / free / store.
    b = bytes.fromhex("4885db74104889dfe8fec9ebff4889dfe81604dcff4d897d")
    require(img.read(0x256C85, len(b)) == b, "dtor-then-free replace idiom")
    # Row 7: no replace check; the store follows the ctor directly.
    require(img.read(0x256C49, 3) == bytes.fromhex("488918"), "row 7 unconditional store")

    header = HEADER.read_text()
    for token in (
        "kOwnerMemberRowCount = 21",
        "kOwnerMembersStartRva = 0x256A15",
        "kOwnerMembersEndRva = 0x2570A9",
        "kVirtualDtor",
        "kDtorThenFree",
        "kSelfRegistered",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in ("0x350", "0x82028", "0x2203F0", "0x178", "0x4028", "kOwnerMemberRows"):
        require(token in source, f"table token {token!r}")
    require(source.count("Dest::kSelfRegistered") >= 2, "two self-registered rows")

    print(
        f"OK check_fowner_members: {len(NEW_SITES)} new sites (20 x 0x17070 + 1 factory), "
        "virtual-dtor and dtor+free idioms, row-7 unconditional store, 21-row native table; "
        "ctor bodies remain hooks"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
