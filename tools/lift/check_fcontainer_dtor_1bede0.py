#!/usr/bin/env python3
"""ELF-tie the three-list container destructor 0x1bede0.

Private: needs local/re/eboot.elf. Pins the three loops, the list-C vtable slot 3 (bare ret), the
four 0xf590 calls in lists B/A, the three 0x170c0 frees, and the sentinel/count offsets.
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
RVA = 0x1BEDE0
SIZE = 208
FN_SHA256 = "632451ffbb53e39cf2c54782fc40657079103d99ab6057d212dc68c84d2358f6"
END = 0x1BEEB0
FREE = 0x170C0
RELEASE = 0xF590
LIST_C_VTABLE = 0x1598CE0
LIST_C_SLOT3 = 0x19F0
NATIVE = ROOT / "native/core/container_dtor_1bede0.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img: "d.Image", addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bede0 sha256")
    require(img.read(END - 5, 1) == b"\xc3", "ret at 0x1beeab")
    require(body[0:4] == bytes.fromhex("554889e5"), "push rbp; mov rbp,rsp")

    # Three sentinel loads: +0x40 (list C), +0x20 (list B), +0x00 (list A).
    require(body[0x11:0x15] == bytes.fromhex("498b5e40"), "list C load [r14+0x40]")
    require(body[0x43:0x47] == bytes.fromhex("498b5e20"), "list B load [r14+0x20]")
    require(body[0x7D:0x80] == bytes.fromhex("498b1e"), "list A load [r14]")

    # List C: vtable load, payload call, free.
    require(body[0x20:0x24] == bytes.fromhex("488b4350"), "list C vtable [rbx+0x50]")
    require(body[0x2B:0x2E] == bytes.fromhex("ff5018"), "list C call [rax+0x18]")
    require(call_target(img, 0x1BEE16) == FREE, "list C free -> 0x170c0")

    # Lists B and A: two releases each then free.
    for site in (0x1BEE3B, 0x1BEE43, 0x1BEE7B, 0x1BEE83):
        require(call_target(img, site) == RELEASE, f"release at {site:#x} -> 0xf590")
    for site in (0x1BEE50, 0x1BEE90):
        require(call_target(img, site) == FREE, f"free at {site:#x} -> 0x170c0")

    # List-C vtable slot 3 resolves to the bare-ret helper (RELRO mapping).
    relro_off = 0x1598000 + (LIST_C_VTABLE + 0x18 - 0x1594000)
    require(struct.unpack_from("<Q", elf, relro_off)[0] == LIST_C_SLOT3,
            "list-C vtable slot 3 == 0x19f0")

    header = NATIVE.read_text().lower()
    for token in (
        "kcontainerdtor1bede0rva = 0x1bede0",
        "kcontainerdtor1bede0size = 208",
        "kcontainerdtor1bede0endrva = 0x1beeb0",
        "kcontainerdtor1bede0freerva = 0x170c0",
        "kcontainerdtor1bede0releaserva = 0xf590",
        "kcontainerdtor1bede0listcslot3 = 0x19f0",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_fcontainer_dtor_1bede0: 0x1bede0+{SIZE}B sha={FN_SHA256[:12]}…; "
        "C: vtable slot3 (0x19f0 ret)+free; B/A: f590 x2+free; sentinels +0x40/+0x20/+0x00; "
        "allocator/release bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
