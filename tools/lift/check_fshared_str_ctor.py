#!/usr/bin/env python3
"""ELF-tie the 0xf1a0 SharedStr constructor and the 0xf3c0 resize contract (E581).

Private: needs local/re/eboot.elf. Pins both body hashes, the align4/capacity sequence, the two
allocation call targets and tags, the xchg refcount store, and the empty-string path.
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
    ("0xf1a0 ctor", 0xF1A0, 151,
     "954762abfefbff590b8ad52dddf7e1008cadd7944d828ef1159f004553d08946"),
    ("0xf3c0 resize", 0xF3C0, 459,
     "980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb"),
]
HEADER = ROOT / "native/core/shared_str_ctor_f1a0.hpp"
SOURCE = ROOT / "native/core/shared_str_ctor_f1a0.cpp"


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

    for name, rva, size, sha in FUNCTIONS:
        require(hashlib.sha256(img.read(rva, size)).hexdigest() == sha, f"{name} sha256")

    for addr, pattern, what in (
        (0xF1A0, "554889e5415653", "prologue"),
        (0xF1BA, "48894308", "size store +0x08"),
        (0xF1BE, "4883c004", "add 4"),
        (0xF1CC, "4883e0fc", "align4"),
        (0xF1D0, "488d48ff", "capacity = alloc-1"),
        (0xF1D7, "48894b10", "capacity store +0x10"),
        (0xF1E2, "488903", "data store +0x00"),
        (0xF20E, "488708", "xchg [rax],rcx refcount init"),
        (0xF211, "48894318", "control store +0x18"),
        (0xF217, "488d055aaddc02", "empty-string lea"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)

    require(call_target(img, 0xF1DD) == 0x12820, "data allocation target")
    require(call_target(img, 0xF204) == 0x17070, "control allocation target")

    header = HEADER.read_text()
    for token in (
        "kSharedStrCtorF1a0Size = 151",
        "kSharedStrCtorF1a0EndRva = 0xF237",
        "kSharedStrResizeF3c0Size = 459",
        "empty_string_f730",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in ("(size + 4)", "/*tag*/ 0", "/*tag*/ 1", "std::atomic_ref", "empty_string_f730()"):
        require(token in source, f"source token {token!r}")

    print(
        "OK check_fshared_str_ctor: 0xf1a0 151B and 0xf3c0 459B hashes, align4 capacity, "
        "data tag 0 / control tag 1, xchg *control=1, shared empty string; guest allocator NOT executed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
