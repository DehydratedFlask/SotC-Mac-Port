#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 tail 0x140d90 call-site after csp attach.

Private: needs local/re/eboot.elf. Pins 22 B fragment, slot+arg, f590 end.
Does NOT claim 0x140d90 body, stack-check/ret, or helper bodies.
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
TAIL = 0x53403C
TAIL_SIZE = 22
TAIL_SHA256 = "cee89da3f08e0912de77476e119a73ef7e672a02b249ee15ba2b6c543201cffb"
END = 0x534052
NATIVE = ROOT / "native/core/vslot_40_533100_tail_140d90.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(TAIL, TAIL_SIZE)).hexdigest() == TAIL_SHA256, "sha")
    require(img.read(0x53403C, 5) == bytes.fromhex("be09000000"), "mov esi,9")
    require(img.read(0x534041, 3) == bytes.fromhex("4c89ff"), "mov rdi,r15")
    require(call_target(0x534044, img) == 0x140D90, "140d90")
    require(img.read(0x534049, 4) == bytes.fromhex("488d7d90"), "lea path standin")
    require(call_target(0x53404D, img) == 0xF590, "f590")
    require(img.read(0x534052, 3) == bytes.fromhex("488b05"), "stack-check start")
    # body size pin (not claimed): first ret
    require(img.read(0x140E1E, 1) == b"\xc3", "140d90 ret")

    nh = NATIVE.read_text()
    require("vslot_40_533100_tail_140d90" in nh, "API")
    require("kVslot40Tail140d90Arg" in nh, "arg")
    require("NOT modelled" in nh, "omission")
    require("tail_140d90" in BASE.read_text() or "Tail140d90" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_tail_140d90: tail={TAIL_SHA256[:12]}…; "
        f"140d90(self+0x3cc8,9)+f590; end={END:#x}; "
        f"140d90_body/stackchk/ret/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
