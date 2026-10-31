#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E first 0xa54920 call-site after batch-D entry 0x5329ae."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x5329AE
PREP_SIZE = 89
PREP_SHA256 = "4337e92cffdb7eb3b3ea0a0e818bbe53264bcfef77d5ecfca7deb5a49d4e5c00"
FRAG = 0x532A07
FRAG_SIZE = 5
FRAG_SHA256 = "df161f5138a59e161cdd7c34d3ab0d4dc4e32a45d84713a79da30caaa7792df1"
END = 0x532A0C
TARGET = 0xA54920
NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_continue_d.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        "kInit531d60PostBaseContinueDEndRva = 0x5329ae" in PREV_NATIVE.read_text(),
        "batch D end == E entry",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[0:3] == bytes.fromhex("488d05"), "E entry lea head")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3)[0:3] == bytes.fromhex("488b84"), "post-call mov rax head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a54920_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532a1a" in ce, "shrunk continue_e start")
    require("merge_a54920_u1_ran" in ce, "u1 merge hook")
    require("core/init_531d60_post_base_a54920_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a54920_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa54920@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
