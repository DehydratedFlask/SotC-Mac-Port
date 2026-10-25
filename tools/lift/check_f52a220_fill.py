#!/usr/bin/env python3
"""ELF-tie the hash→insert fill loop of registry ctor 0x52a220.

Private: needs local/re/eboot.elf. Pins body digest, fill-loop callees
(0x161fb0, 0xa2ddc0, 0x52d530, 0x52d7e0), FNV seed immediate, and that native
headers document the fill + private-name policy. Does NOT claim pool/listener
loops after 0x52a680.
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
CTOR = 0x52A220
CTOR_SIZE = 1890
CTOR_SHA256 = "8420efe0e42f20a6a0e3e642bdd913030d4416ee59d978ec579287be2ae4c1eb"
HASHPAIR_SHA256 = "a1aa059e47b80f1d450d46f47d0885858d04323299c8e01b6a3bd2425f329258"
NATIVE = ROOT / "native/core/registry_fill_52a220.hpp"
FIXTURE_HDR = ROOT / "native/core/inputtable_names_fixture.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_targets(rva: int, body: bytes) -> list[int]:
    out = []
    i = 0
    while i + 5 <= len(body):
        if body[i] == 0xE8:
            rel = struct.unpack("<i", body[i + 1 : i + 5])[0]
            out.append(rva + i + 5 + rel)
            i += 5
            continue
        i += 1
    return out


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(CTOR, CTOR_SIZE)
    require(len(body) == CTOR_SIZE, "0x52a220 size")
    require(hashlib.sha256(body).hexdigest() == CTOR_SHA256, "0x52a220 sha changed")

    calls = call_targets(CTOR, body)
    require(0x161FB0 in calls, "missing call 0x161fb0 (name_a FNV)")
    require(0xA2DDC0 in calls, "missing call 0xa2ddc0 (name_b path-FNV)")
    require(0x52D530 in calls, "missing call 0x52d530 (insert A)")
    require(0x52D7E0 in calls, "missing call 0x52d7e0 (insert B)")
    require(b"\xc5\x9d\x1c\x81" in body, "missing FNV seed imm 0x811c9dc5")

    # Fill-loop entry: xor r14d / mov esi, seed around 0x52a50e..0x52a524
    require(body[0x52A50E - CTOR : 0x52A511 - CTOR] == b"\x45\x31\xf6", "fill idx xor")
    require(body[0x52A524 - CTOR : 0x52A529 - CTOR] == b"\xbe\xc5\x9d\x1c\x81", "seed mov")

    native = NATIVE.read_text()
    require("fill_registry_object_52a220" in native, "API")
    require("0x52a220" in native and "NOT modelled" in native, "RVA + omission")
    require("private" in native.lower() or "fixture" in native.lower(), "fixture policy")

    fix = FIXTURE_HDR.read_text()
    require(HASHPAIR_SHA256 in fix, "hashpair digest constant")
    require("I275" in fix or "kInputtableNamesFixtureMagic" in fix, "magic")

    print(
        "OK check_f52a220_fill: fill-loop callees + seed pinned; "
        f"hashpair={HASHPAIR_SHA256[:12]}…"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
