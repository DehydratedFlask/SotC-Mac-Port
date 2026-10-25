#!/usr/bin/env python3
"""ELF-tie the 18-byte remarshal 0x1bde30 that tail-calls the list-3 insert.

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
RVA = 0x1BDE30
SIZE = 18
FN_SHA256 = "4a838c0c26ef815c262b36a44a16bc02459bb184ee6c4d45c99866bdf44ef2c9"
TAIL = 0x1BEEB0
NATIVE = ROOT / "native/core/insert_1bde30.hpp"
IMPL = ROOT / "native/core/insert_1bde30.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bde30 sha256")
    # mov rax,rsi ; add rdi,0x40 ; mov rsi,rdi ; mov rdx,rax ; jmp rel32
    require(body[0:3] == bytes.fromhex("4889f0"), "mov rax,rsi")
    require(body[3:7] == bytes.fromhex("4883c740"), "add rdi,0x40")
    require(body[7:10] == bytes.fromhex("4889fe"), "mov rsi,rdi")
    require(body[10:13] == bytes.fromhex("4889c2"), "mov rdx,rax")
    require(body[13] == 0xE9, "jmp rel32")
    require(RVA + 13 + 5 + struct.unpack("<i", body[14:18])[0] == TAIL, "tail target 0x1beeb0")

    header = NATIVE.read_text().lower()
    for token in (
        "kInsert1bde30Rva = 0x1bde30",
        "kInsert1bde30Size = 18",
        "kInsert1bde30EndRva = 0x1bde42",
        "kInsert1bde30TailTargetRva = 0x1beeb0",
        "not modelled",
    ):
        require(token.lower() in header, f"header token {token!r}")
    require("insert_1bde30" in IMPL.read_text(), "impl present")

    print(
        f"OK check_finsert_1bde30: 0x1bde30+{SIZE}B sha={FN_SHA256[:12]}…; "
        "mov rax,rsi / add rdi,0x40 / mov rsi,rdi / mov rdx,rax / jmp 0x1beeb0; "
        "insert body/owner NOT claimed here"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
