#!/usr/bin/env python3
"""ELF-tie the list-A serialization and flag==1 drain inside 0x1bdf30.

Private: needs local/re/eboot.elf. Pins the reader start/end, the node field reads, the drain's
flag test, the two 0xf590 calls, the 0x170c0 free and the 64-bit count decrement.
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
READER = 0x1BE14D
READER_END = 0x1BE222
DRAIN = 0x1BE3F4
DRAIN_END = 0x1BE456
RELEASE = 0xF590
FREE = 0x170C0
NATIVE_H = ROOT / "native/core/app_info_collect.hpp"


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

    reader = img.read(READER, READER_END - READER)
    # 49 8b 5d 00      mov rbx,[r13]        (container base, list-A sentinel)
    require(reader[0:4] == bytes.fromhex("498b5d00"), "reader head load [r13]")
    # 4c 39 eb          cmp rbx,r13
    require(reader[4:7] == bytes.fromhex("4c39eb"), "reader sentinel compare")
    # 4c 8b 63 30       mov r12,[rbx+0x30]   (value holder)
    require(b"\x4c\x8b\x63\x30" in reader, "reader value load +0x30")
    # 48 8b 53 10       mov rdx,[rbx+0x10]   (name holder)
    require(b"\x48\x8b\x53\x10" in reader, "reader name load +0x10")
    # 48 8b 1b          mov rbx,[rbx]        (advance)
    require(b"\x48\x8b\x1b" in reader, "reader advance [rbx]")
    # 4c 39 eb          cmp rbx,r13          (loop end)
    require(reader.endswith(bytes.fromhex("4c39eb")), "reader loop compare")

    drain = img.read(DRAIN, DRAIN_END - DRAIN)
    # 4d 8b 34 24       mov r14,[r12]        (first list-A node)
    require(drain[0:4] == bytes.fromhex("4d8b3424"), "drain head load [r12]")
    # 41 83 7e 50 01    cmp dword [r14+0x50],1
    require(b"\x41\x83\x7e\x50\x01" in drain, "drain flag==1 test")
    require(call_target(img, 0x1BE424) == RELEASE, "drain release +0x30 -> 0xf590")
    require(call_target(img, 0x1BE42D) == RELEASE, "drain release +0x10 -> 0xf590")
    require(call_target(img, 0x1BE43A) == FREE, "drain free -> 0x170c0")
    require(b"\x49\xff\x4c\x24\x10" in drain, "64-bit count decrement [r12+0x10]")

    header = NATIVE_H.read_text().lower()
    for token in (
        "kappinfocollectreaderrva = 0x1be14d",
        "kappinfocollectreaderendrva = 0x1be222",
        "kappinfodrainrva = 0x1be3f4",
        "kappinfodrainendrva = 0x1be456",
        "kappinfodrainflag = 1",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_fapp_info_collect: reader {READER:#x}..{READER_END:#x}; "
        f"drain {DRAIN:#x}..{DRAIN_END:#x}; flag==1; f590 +0x30/+0x10; free 0x170c0; "
        "list-B/report/gate NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
