#!/usr/bin/env python3
"""ELF-tie prologue debug gates 0x2552d4..0x255318 + empty-path 0xba40.

Private: needs local/re/eboot.elf. Pins gate cells, ba40 call, a30 vslot+0x10.
Does NOT claim stack-chk PLT, non-empty ba40 listener invoke, or vslot bodies.
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
GATES = 0x2552D4
GATES_SIZE = 68
GATES_SHA256 = "d599061b5cabe4171a82295c9cf2e04a97d17fa3094235479e195eb0c0a1c531"
BA40 = 0xBA40
BA40_SIZE = 173
BA40_SHA256 = "3615371ed46905ef509b37f78c55744ab84802fa55eb651d0e4bfe4d988b3c1c"
FLAG_OBJ = 0x2DFCE70
VTBL_OBJ = 0x2DFCF40
DEBUG_OBJ = 0x2EEC840
A30_CELL = 0x2DFD4B8
NATIVE = ROOT / "native/core/prologue_gates_2552d4.hpp"
PROL = ROOT / "native/core/prologue_255180.hpp"


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


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(GATES, GATES_SIZE)).hexdigest() == GATES_SHA256, "gates sha")
    require(hashlib.sha256(img.read(BA40, BA40_SIZE)).hexdigest() == BA40_SHA256, "ba40 sha")
    require(img.read(BA40 + BA40_SIZE - 1, 1) == b"\xc3", "ba40 ret")

    require(lea_abs(0x2552D4, img) == FLAG_OBJ, "flag obj 0x2dfce70")
    require(img.read(0x2552DB, 4) == bytes.fromhex("488b4060"), "mov rax,[rax+0x60]")
    require(img.read(0x2552DF, 3) == bytes.fromhex("803800"), "cmp byte [rax],0")
    require(lea_abs(0x2552E8, img) == VTBL_OBJ, "vtbl obj 0x2dfcf40")
    require(img.read(0x2552F2, 3) == bytes.fromhex("ff5048"), "vslot+0x48")
    require(img.read(0x2552F5, 2) == bytes.fromhex("34 01".replace(" ", "")), "xor al,1")
    require(lea_abs(0x2552F7, img) == DEBUG_OBJ, "debug obj 0x2eec840")
    require(img.read(0x255305, 4) == bytes.fromhex("488b7760"), "mov rsi,[rdi+0x60]")
    require(call_target(0x255309, img) == BA40, "call 0xba40")
    require(img.read(0x255315, 3) == bytes.fromhex("ff5010"), "a30 vslot+0x10")
    require(img.read(0x255320, 1) == b"\x75", "jne canary fail")
    require(call_target(0x255331, img) == 0xF8F3A0, "stack-chk fail PLT")
    require(img.read(0xF8F3A0, 2) == bytes.fromhex("ff25"), "stack-chk PLT")

    # a30 cell used by early + gates
    require(lea_abs(0x2551B8, img) == A30_CELL, "r13 a30 cell")

    # ba40 empty-path pins
    require(img.read(0xBA5F, 4) == bytes.fromhex("80796a00"), "cmp [rcx+0x6a],0")
    require(img.read(0xBA9A, 2) == bytes.fromhex("8a06"), "mov al,[rsi]")
    require(img.read(0xBAA3, 2) == bytes.fromhex("8816"), "mov [rsi],dl")

    nh = NATIVE.read_text()
    require("run_prologue_gates_2552d4" in nh, "API")
    require("init_ba40_empty" in nh, "ba40 empty API")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("run_prologue_gates_2552d4" in PROL.read_text() or "prologue_gates_2552d4" in PROL.read_text(),
            "prologue wires")

    print(
        f"OK check_f2552d4_gates: gates={GATES_SHA256[:12]}… ba40={BA40_SHA256[:12]}…; "
        f"cells {FLAG_OBJ:#x}/{VTBL_OBJ:#x}/{DEBUG_OBJ:#x}; "
        f"stack-chk/listener invoke NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
