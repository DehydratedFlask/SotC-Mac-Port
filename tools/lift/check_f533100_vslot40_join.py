#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 join+ after miss/hit converge.

Private: needs local/re/eboot.elf. Pins join fragment, Position, callees, slot.
Does NOT claim 0x2779a0 / 0x42390 bodies or shadowmodels cluster past 0x533411.
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
JOIN = 0x533305
JOIN_SIZE = 268
JOIN_SHA256 = "f4142065a26bf7b8cea1a59349621f5c11701cbf1acb471612097f0b25b74ca6"
END = 0x533411
NATIVE = ROOT / "native/core/vslot_40_533100_join.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_abs(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0] in (0x48, 0x4C) and b[1] == 0x8D, f"missing lea at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def cstr(img: d.Image, addr: int) -> bytes:
    out = bytearray()
    i = 0
    while True:
        b = img.read(addr + i, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
        i += 1
        if i > 80:
            raise SystemExit(f"FAIL unterminated at {addr:#x}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(JOIN, JOIN_SIZE)).hexdigest() == JOIN_SHA256, "join sha")
    # miss/hit converge: jne from post lands on JOIN
    jne = img.read(0x533201, 6)
    require(jne[:2] == bytes.fromhex("0f85"), "jne")
    hit = 0x533207 + struct.unpack("<i", jne[2:6])[0]
    require(hit == JOIN, "hit → join")

    require(call_target(0x53332E, img) == 0x42390, "42390")
    require(cstr(img, lea_abs(0x533333, img)) == b"Position", "Position")
    require(call_target(0x533399, img) == 0xF590, "f590")
    require(call_target(0x5333A3, img) == 0x17070, "17070")
    require(call_target(0x5333B7, img) == 0x2779A0, "2779a0")
    # mov qword ptr [r13+0x3b60], r15 → 4d 89 bd 60 3b 00 00
    store = img.read(0x53340A, 7)
    require(store == bytes.fromhex("4d89bd603b0000"), f"slot store {store.hex()}")

    nh = NATIVE.read_text()
    require("vslot_40_533100_join" in nh, "API")
    require("Position" in nh, "name")
    require("NOT modelled" in nh, "omission")
    require("vslot_40_join" in BASE.read_text() or "vslot_40_533100_join" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_join: join={JOIN_SHA256[:12]}…; "
        f"Position; 42390/17070/2779a0/f590; slot+0x3b60; end={END:#x}; "
        f"2779a0/42390/shadowmodels NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
