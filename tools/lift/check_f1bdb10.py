#!/usr/bin/env python3
"""ELF-tie the native lift of ctor RVA 0x1bdb10 (0x60 B three-list container).

Pins the 56 B body and its eleven stores, the absence of writes to +0x18/+0x38/+0x58,
and the native module's constants. Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
RVA = 0x1BDB10
SIZE = 56
FN_SHA256 = "056d75148454754896928518e09b1a3fd81320f2d2a8db08aca9b489b327580a"
END = 0x1BDB48
NATIVE = ROOT / "native/core/container_1bdb10.hpp"
IMPL = ROOT / "native/core/container_1bdb10.cpp"
TEST = ROOT / "native/tests/test_container_1bdb10.cpp"

# (offset, bytes, meaning)
STORES = [
    (0x08, "48c7471000000000", "movq $0,[rdi+0x10]  list-1 count"),
    (0x10, "48893f", "mov [rdi],rdi          list-1 prev"),
    (0x13, "48897f08", "mov [rdi+0x08],rdi     list-1 next"),
    (0x17, "48c7473000000000", "movq $0,[rdi+0x30]  list-2 count"),
    (0x1F, "48894720", "mov [rdi+0x20],rax     list-2 prev"),
    (0x23, "48894728", "mov [rdi+0x28],rax     list-2 next"),
    (0x27, "48c7475000000000", "movq $0,[rdi+0x50]  list-3 count"),
    (0x2F, "48894f40", "mov [rdi+0x40],rcx     list-3 prev"),
    (0x33, "48894f48", "mov [rdi+0x48],rcx     list-3 next"),
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bdb10 sha256")
    require(img.read(END - 1, 1) == b"\xc3", "ret at 0x1bdb47")
    require(body[:4] == bytes.fromhex("488d4720"), "lea rax,[rdi+0x20] first")
    require(body[4:8] == bytes.fromhex("488d4f40"), "lea rcx,[rdi+0x40] second")

    for off, hexbytes, meaning in STORES:
        got = img.read(RVA + off, len(hexbytes) // 2)
        require(got == bytes.fromhex(hexbytes), f"{meaning} at +{off:#x}")

    # No store anywhere in the body targets +0x18/+0x38/+0x58 (those qwords are untouched).
    for qword_off in (0x18, 0x38, 0x58):
        for pattern in (
            bytes([0x48, 0xC7, 0x47, qword_off]),          # movq $imm32,[rdi+off]
            bytes([0x48, 0x89, 0x47, qword_off]),          # mov [rdi+off],rax
            bytes([0x48, 0x89, 0x4F, qword_off]),          # mov [rdi+off],rcx
        ):
            require(pattern not in body, f"unexpected store to +{qword_off:#x}")

    header = NATIVE.read_text()
    for token in (
        "kContainer1bdb10Rva = 0x1bdb10",
        "kContainer1bdb10Size = 56",
        "kContainer1bdb10EndRva = 0x1bdb48",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")
    require("init_container_1bdb10" in IMPL.read_text(), "impl function")
    require("0x1bdb10" in TEST.read_text(), "test pins the RVA")

    print(
        "OK check_f1bdb10: extent=0x1bdb10+56B sha=" + FN_SHA256[:12] + "…; "
        "three self-referential list sentinels, counts 0 at +0x10/+0x30/+0x50, "
        "+0x18/+0x38/+0x58 untouched; owner construction NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
