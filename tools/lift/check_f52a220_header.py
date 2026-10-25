#!/usr/bin/env python3
"""ELF-tie the prologue stores of registry ctor 0x52a220 (object behind 0x2efd160).

Private: needs local/re/eboot.elf. Pins the early straight-line header stores only:
  zero vector heads at +0x00/+0x20/+0x40, input_table at +0x60, float 5.0f @ +0x6c,
  float 60.0f @ +0x80. Does NOT interpret the insert loops or pool resizes.
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR = 0x52A220
CTOR_SIZE = 1890
CTOR_SHA256 = "8420efe0e42f20a6a0e3e642bdd913030d4416ee59d978ec579287be2ae4c1eb"
NATIVE = ROOT / "native/core/registry_object_2efd160.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(CTOR, CTOR_SIZE)
    require(len(body) == CTOR_SIZE, "0x52a220 size")
    require(hashlib.sha256(body).hexdigest() == CTOR_SHA256, "0x52a220 sha changed")

    # Prologue immediates as raw encodings.
    require(bytes.fromhex("c7476c0000a040") in body, "missing movl $0x40a00000,0x6c(%rdi)")
    require(bytes.fromhex("c7878000000000007042") in body, "missing movl $0x42700000,0x80(%rdi)")
    require(bytes.fromhex("4c896760") in body, "missing movq %r12,0x60(%rdi)")
    require(bytes.fromhex("c5f81107") in body, "missing vmovups %xmm0,(%rdi)")
    require(bytes.fromhex("c5f8114720") in body, "missing vmovups %xmm0,0x20(%rdi)")
    require(bytes.fromhex("c5f8114740") in body, "missing vmovups %xmm0,0x40(%rdi)")

    native = NATIVE.read_text()
    require("0x98" in native and "init_registry_object_header_52a220" in native, "native API")
    require(re.search(r"kRegistryObjectFloat5\s*=\s*0x40a00000", native), "float 5 constant")
    require(re.search(r"kRegistryObjectFloat60\s*=\s*0x42700000", native), "float 60 constant")
    require("NOT modelled" in native or "NOT modelled" in native.upper() or "not modelled" in native.lower(),
            "must document insert omission")

    print("OK check_f52a220_header: prologue stores + native header constants pinned")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
