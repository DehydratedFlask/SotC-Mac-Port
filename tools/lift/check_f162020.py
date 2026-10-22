#!/usr/bin/env python3
"""ELF-tie path-normalizing FNV helpers 0xa2ddc0 -> 0x162020.

Private: needs local/re/eboot.elf. Pins:
  - wrapper 0xa2ddc0 = mov esi,0x811c9dc5; jmp 0x162020
  - body 0x162020 (98 B) shape: null->0, movzbl, cmpq $0xff shortcut, call PLT _Getptoupper,
    '\\'->'/' via cmove, imul 0x01000193
  - PLT 0xf90180 GOT NID == _Getptoupper (rcQCUr0EaRU)
  - native headers name the helpers and document the ASCII-toupper policy

Does NOT claim locale-faithful _Getptoupper for non-ASCII; does NOT lift registry insert loops.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
WRAPPER = 0xA2DDC0
BODY = 0x162020
BODY_SIZE = 98
WRAPPER_SHA = "c526d089a4b2d7d1c1acf93955168456e042e061b9f42ec2cfc7fd160fb10765"
BODY_SHA = "a7d19297baf11957f065c2631731c010abc4d382f344a2125d59a09f65684d57"
PLT_TOUPPER = 0xF90180
GOT_TOUPPER = 0x16006B0
NATIVE = ROOT / "native/core/fnv_path_cstr_162020.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def jmp_target(rva: int, body: bytes, e9_at: int) -> int:
    require(body[e9_at] == 0xE9, f"{rva:#x}: missing jmp at +{e9_at}")
    return rva + e9_at + 5 + struct.unpack("<i", body[e9_at + 1 : e9_at + 5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    parsed = d.parse(elf)
    img = d.Image(elf, parsed["ph"])

    wrap = img.read(WRAPPER, 10)
    require(hashlib.sha256(wrap).hexdigest() == WRAPPER_SHA, "0xa2ddc0 sha changed")
    require(wrap[:5] == bytes.fromhex("bec59d1c81"), "0xa2ddc0 not mov esi,0x811c9dc5")
    require(jmp_target(WRAPPER, wrap, 5) == BODY, "0xa2ddc0 does not jmp to 0x162020")

    body = img.read(BODY, BODY_SIZE)
    require(len(body) == BODY_SIZE, "0x162020 size")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA, "0x162020 sha changed")
    require(body[0] == 0x55 and body[-1] == 0xC3, "0x162020 prologue/ret")
    require(b"\x41\xbf\x2f\x00\x00\x00" in body, "missing '/' immediate 0x2f")
    require(b"\x83\xf8\x5c" in body, "missing cmp with '\\\\' (0x5c)")
    require(b"\x69\xf0\x93\x01\x00\x01" in body or b"\x44\x69\xf0\x93\x01\x00\x01" in body,
            "missing FNV prime imul 0x01000193")
    require(b"\x48\x81\xfb\xff\x00\x00\x00" in body, "missing cmpq $0xff shortcut")

    # call rel32 to PLT _Getptoupper
    call_at = body.find(bytes.fromhex("e8"))
    require(call_at >= 0, "no call in 0x162020")
    # find the call that targets PLT_TOUPPER
    found_call = False
    for i in range(len(body) - 4):
        if body[i] != 0xE8:
            continue
        rel = struct.unpack("<i", body[i + 1 : i + 5])[0]
        if BODY + i + 5 + rel == PLT_TOUPPER:
            found_call = True
            break
    require(found_call, "0x162020 does not call PLT 0xf90180")

    thunk = img.read(PLT_TOUPPER, 6)
    require(thunk[:2] == b"\xff\x25", "0xf90180 not jmp [rip]")
    got = PLT_TOUPPER + 6 + struct.unpack("<i", thunk[2:])[0]
    require(got == GOT_TOUPPER, f"GOT {got:#x} != {GOT_TOUPPER:#x}")
    sym = next(
        parsed["symbols"][r["symbol"]]
        for r in parsed["relocs"]
        if r["target"] == got and r["type"] in (1, 6, 7)
    )
    require(sym["nid"] == d.nid("_Getptoupper"), f"nid {sym['nid']} != _Getptoupper")

    native = NATIVE.read_text()
    require("fnv_path_cstr_162020" in native and "fnv_cstr_a2ddc0" in native, "native API missing")
    require(re.search(r"kFnvOffsetBasisA2ddc0\s*=\s*0x811c9dc5", native), "seed constant")
    require("_Getptoupper" in native, "native must document the import")

    print(
        "OK check_f162020: wrapper 0xa2ddc0 -> 0x162020; "
        f"PLT _Getptoupper NID {sym['nid']}; body sha {BODY_SHA[:12]}…"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
