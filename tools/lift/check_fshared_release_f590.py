#!/usr/bin/env python3
"""ELF-tie the shared-ownership release at 0xf590.

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
RVA = 0xF590
SIZE = 73
FN_SHA256 = "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47"
END = 0xF5D9
DESTROY = 0x12AF0
FREE = 0x170B0
NATIVE = ROOT / "native/core/shared_release_f590.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0xf590 sha256")
    require(body[-1:] == b"\xc3", "ret at 0xf5d8")
    require(body[0:4] == bytes.fromhex("554889e5"), "push rbp; mov rbp,rsp")
    require(body[0x06:0x09] == bytes.fromhex("4889fb"), "mov rbx,rdi")
    require(body[0x09:0x0D] == bytes.fromhex("488b4318"), "mov rax,[rbx+0x18]")
    require(body[0x0D:0x10] == bytes.fromhex("4885c0"), "test rax,rax")
    # rcx = -1
    require(body[0x12:0x15] == bytes.fromhex("48c7c1"), "mov rcx,-1")
    require(body[0x15:0x19] == bytes.fromhex("ffffffff"), "imm -1")
    # lock xadd (%rax),rcx
    require(body[0x19:0x1E] == bytes.fromhex("f0480fc108"), "lock xadd")
    require(body[0x1E:0x22] == bytes.fromhex("4883f901"), "cmp rcx,1")

    call = img.read(0xF5B9, 5)
    require(call[0] == 0xE8, "destroy call opcode")
    require(0xF5B9 + 5 + struct.unpack("<i", call[1:5])[0] == DESTROY, "destroy 0x12af0")
    tj = img.read(0xF5CD, 5)
    require(tj[0] == 0xE9, "tail jmp opcode")
    require(0xF5CD + 5 + struct.unpack("<i", tj[1:5])[0] == FREE, "free 0x170b0")

    header = NATIVE.read_text().lower()
    for token in (
        "ksharedreleasef590rva = 0xf590",
        "ksharedreleasef590size = 73",
        "ksharedreleasef590endrva = 0xf5d9",
        "ksharedreleasef590destroyrva = 0x12af0",
        "ksharedreleasef590freerva = 0x170b0",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_fshared_release_f590: 0xf590+{SIZE}B sha={FN_SHA256[:12]}…; "
        "control at +0x18; lock xadd decomp; destroy 0x12af0 on last ref; tail jmp 0x170b0; "
        "callee bodies/object layout NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
