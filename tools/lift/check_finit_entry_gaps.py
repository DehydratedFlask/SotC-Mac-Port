#!/usr/bin/env python3
"""ELF-tie Init 0x531d60's four remaining spans (entry frame, base-init site, ret-nop, jump).

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
SPANS = {
    "A": (0x531D60, 36, "18abd8d4dcc10a77429d0585b28d127b7dec25dc043f2fd9ff18fca2ab9f76f1"),
    "B": (0x531DCF, 16, "ee6d49088d551b5348688a309dac631cc09958453417a37fef45e17bdc0f5010"),
    "C": (0x531DF9, 5, "3bcaa2dea7fb7d461781762618a60af241993eed7c0851f67f808eb45dfac673"),
    "D": (0x5327B1, 5, "fb89143aa6398bfc034e4b05d911ebfd8f0056e5c01669513eb3f7a603b56598"),
}
NATIVE = ROOT / "native/core/init_531d60_entry_gaps.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    sha = {}
    for name, (rva, size, expected) in SPANS.items():
        body = img.read(rva, size)
        got = hashlib.sha256(body).hexdigest()
        require(got == expected, f"span {name} sha256 at 0x{rva:x}: {got[:16]}…")
        sha[name] = got

    # Span A: entry frame — seven pushes, and/sub rsp, guard mov, self save.
    a = img.read(0x531D60, 36)
    require(a[0:1] == b"\x55", "push rbp")
    require(a[1:4] == bytes.fromhex("4889e5"), "mov rbp,rsp")
    require(a[4:13] == bytes.fromhex("415741564155415453"), "push r15/r14/r13/r12/rbx")
    require(a[13:17] == bytes.fromhex("4883e4e0"), "and rsp,-0x20")
    require(a[17:24] == bytes.fromhex("4881ec400c0000"), "sub rsp,0xc40")
    require(a[24:27] == bytes.fromhex("4c8b2d"), "guard mov r13,[rip+..]")
    require(0x531D60 + 24 + 7 + struct.unpack("<i", a[27:31])[0] == 0x15FE518,
            "guard cell 0x15fe518")
    require(a[31:36] == bytes.fromhex("48897c2420"), "mov [rsp+0x20],rdi")

    # Span B: reload self + base Initialize call + test al.
    b = img.read(0x531DCF, 16)
    require(b[0:5] == bytes.fromhex("4c8b642420"), "mov r12,[rsp+0x20]")
    require(b[5:8] == bytes.fromhex("4c89fe"), "mov rsi,r15")
    require(b[8:11] == bytes.fromhex("4c89e7"), "mov rdi,r12")
    require(b[11] == 0xE8, "call base Initialize")
    require(0x531DCF + 11 + 5 + struct.unpack("<i", b[12:16])[0] == 0x255130,
            "call target 0x255130")

    # Span C: call 0x56dca0.
    c = img.read(0x531DF9, 5)
    require(c[0] == 0xE8, "call ret-nop")
    require(0x531DF9 + 5 + struct.unpack("<i", c[1:5])[0] == 0x56DCA0,
            "call target 0x56dca0")

    # Span D: jmp 0x532904.
    dd = img.read(0x5327B1, 5)
    require(dd[0] == 0xE9, "jmp rel32")
    require(0x5327B1 + 5 + struct.unpack("<i", dd[1:5])[0] == 0x532904,
            "jump target 0x532904")

    header = NATIVE.read_text()
    for token in (
        "kInitEntrySpanARva = 0x531D60",
        "kInitEntrySpanASize = 36",
        "kInitEntrySpanBRva = 0x531DCF",
        "kInitEntrySpanBRva = 0x531DCF",
        "kInitEntrySpanCRva = 0x531DF9",
        "kInitEntrySpanDRva = 0x5327B1",
        "kInitEntryGuardCellRva = 0x15FE518",
        "kInitEntryBaseInitRva = 0x255130",
        "kInitEntryRetNopRva = 0x56DCA0",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        "OK check_finit_entry_gaps: A=0x531d60+36B sha=" + sha["A"][:12] + "…; "
        "B=0x531dcf+16B sha=" + sha["B"][:12] + "…; C=0x531df9+5B sha=" + sha["C"][:12] + "…; "
        "D=0x5327b1+5B sha=" + sha["D"][:12] + "…; "
        "guard 0x15fe518; base 0x255130; ret-nop 0x56dca0; jmp 0x532904; "
        "0xc40 frame/r15 provenance/callee bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
