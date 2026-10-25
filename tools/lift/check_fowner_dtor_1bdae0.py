#!/usr/bin/env python3
"""ELF-tie the owner destructor 0x1bdae0 (destroy -> delete -> clear slot).

Private: needs local/re/eboot.elf.
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
RVA = 0x1BDAE0
SIZE = 46
FN_SHA256 = "9c9bbed48a956febc8255e8719b48203e603d425022783bb626ede6f129d40d5"
END = 0x1BDB0E
DESTROY = 0x1BEDE0
FREE = 0x170B0
NATIVE = ROOT / "native/core/owner_dtor_1bdae0.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bdae0 sha256")
    require(img.read(END - 1, 1) == b"\xc3", "ret at 0x1bdb0d")
    require(body[0:4] == bytes.fromhex("554889e5"), "push rbp; mov rbp,rsp")
    require(body[4:7] == bytes.fromhex("415653"), "push r14; push rbx")
    require(body[7:10] == bytes.fromhex("4989fe"), "mov r14,rdi")
    require(body[10:13] == bytes.fromhex("498b1e"), "mov rbx,[r14]")
    require(body[13:16] == bytes.fromhex("4885db"), "test rbx,rbx")

    d1 = img.read(0x1BDAF5, 5)
    require(d1[0] == 0xE8 and 0x1BDAF5 + 5 + struct.unpack("<i", d1[1:5])[0] == DESTROY,
            "destroy call -> 0x1bede0")
    d2 = img.read(0x1BDAFD, 5)
    require(d2[0] == 0xE8 and 0x1BDAFD + 5 + struct.unpack("<i", d2[1:5])[0] == FREE,
            "free call -> 0x170b0")
    # [r14] = 0 then epilogue
    require(bytes.fromhex("49c70600000000") in body, "clear slot")

    header = NATIVE.read_text().lower()
    for token in (
        "kownerdtor1bdae0rva = 0x1bdae0",
        "kownerdtor1bdae0size = 46",
        "kownerdtor1bdae0endrva = 0x1bdb0e",
        "kownerdtor1bdae0destroyrva = 0x1bede0",
        "kownerdtor1bdae0freerva = 0x170b0",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_fowner_dtor_1bdae0: 0x1bdae0+{SIZE}B sha={FN_SHA256[:12]}…; "
        "null path; destroy 0x1bede0 -> free 0x170b0 -> clear slot; "
        "callee bodies/atexit registration NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
