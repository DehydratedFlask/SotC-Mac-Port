#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 stack-check + success epilogue after tail 140d90.

Private: needs local/re/eboot.elf. Pins 34 B fragment, canary cell, ret end.
Does NOT claim __stack_chk_fail body, 140d90 body, or helper bodies.
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
CHK = 0x534052
CHK_SIZE = 34
CHK_SHA256 = "8d7ee744885d8de1313fb0390c5bad95c2979091ced01c033f59b0e4e87b0d8c"
END = 0x534074
NATIVE = ROOT / "native/core/vslot_40_533100_stackchk.hpp"
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
    require(b[0] == 0x48 and b[1] == 0x8B and b[2] == 0x05, f"missing mov rax,[rip] at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(CHK, CHK_SIZE)).hexdigest() == CHK_SHA256, "sha")
    require(lea_abs(0x534052, img) == 0x15FE518, "guard cell")
    require(img.read(0x534059, 3) == bytes.fromhex("488b00"), "deref guard")
    require(img.read(0x53405C, 4) == bytes.fromhex("483b45d0"), "cmp [rbp-0x30]")
    require(img.read(0x534060, 2) == bytes.fromhex("7512"), "jne fail")
    require(img.read(0x534062, 7) == bytes.fromhex("4881c4d8020000"), "add rsp,0x2d8")
    require(img.read(0x534073, 1) == b"\xc3", "ret")
    require(call_target(0x534074, img) == 0xF8F3A0, "stack_chk_fail")
    # prologue stores same cell into [rbp-0x30]
    require(lea_abs(0x533114, img) == 0x15FE518, "prologue guard cell")
    require(img.read(0x53312D, 4) == bytes.fromhex("488945d0"), "store [rbp-0x30]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_stackchk" in nh, "API")
    require("kVslot40StackchkGuardCellRva" in nh, "cell")
    require("NOT modelled" in nh, "omission")
    require("stackchk" in BASE.read_text() or "Stackchk" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_stackchk: chk={CHK_SHA256[:12]}…; "
        f"cell=0x15fe518; success ret; end={END:#x}; "
        f"f8f3a0_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
