#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 streams attach (production model, E595).

Pins the site's byte extent, its key literal, the slot offset and the recovered call targets.
The earlier success-flag/notify reading is withdrawn: `[out]`/`[out+8]` are the GetResource
`{ptr, owned}` result and `owned` is always 0, so the RequestState/release branches are dead.
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
SITE = 0x533a4c
SIZE = 377
SITE_SHA256 = "2aeb2284b38994a08dff19ce4d31ac132aede335285b2cba979c945c56d0214f"
END = 0x533bc5
LITERAL = "$/sound/shadow/****/shadowstreams.crmp"
SLOT_OFF = 0x50
SELF_BASED = False
NATIVE = ROOT / "native/core/vslot_40_533100_streams_attach.hpp"
COMMON = ROOT / "native/core/vslot_40_533100_attach_common.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def calls_in(img: d.Image, start: int, end: int) -> dict[int, int]:
    counts: dict[int, int] = {}
    pc = start
    while pc + 5 <= end:
        if img.read(pc, 1)[0] == 0xE8:
            rel = struct.unpack("<i", img.read(pc + 1, 4))[0]
            target = pc + 5 + rel
            counts[target] = counts.get(target, 0) + 1
            pc += 5
        else:
            pc += 1
    return counts


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(SITE, SIZE)).hexdigest() == SITE_SHA256, "site bytes")
    require(LITERAL.encode() in elf, "literal bytes in ELF")
    require(img.read(SITE, 1)[0] in (0x80, 0x41, 0x4C, 0x4D, 0x48, 0x49),
            "site does not start at the recovered join")

    counts = calls_in(img, SITE - 0x30, END + 0x20)
    require(counts.get(0x14C2F0, 0) >= 1, "0x14c2f0 key")
    require(counts.get(0x14CCB0, 0) >= 2, "0x14ccb0 AddReference x2")
    require(counts.get(0x14CCC0, 0) >= 2, "0x14ccc0 RemoveReference x2")
    require(counts.get(0x14D880, 0) >= 1, "0x14d880 null wrapper")
    require(counts.get(0xF590, 0) >= 1, "0xf590 key release")
    require(counts.get(0x95580, 0) + counts.get(0x140D90, 0) >= 1, "slot Lock")

    text = NATIVE.read_text()
    require("Vslot40AttachCommonHooks" in text, "common hooks")
    require(LITERAL in text, "literal constant")
    require(f"SlotOff = {SLOT_OFF:#X}".lower() in text.lower(), "slot offset")
    for bad in ("took_fail_path", "success_flag", "notify_95580", "resource_pool_miss",
                "release_14ccc0", "resource_pool_miss"):
        require(bad not in text, f"withdrawn token {bad}")
    common = COMMON.read_text()
    require(common.count("remove_reference(") >= 2, "two RemoveReference calls")
    require("slot->assign(" in common, "slot assign")
    require("slot->lock(" in common, "slot lock")
    require("release_site_key(" in common, "key release")
    base = (ROOT / "native/core/base_initialize_255130.cpp").read_text()
    require("vslot_40_533100_streams_attach(" in base or "vslot_40_533100_streams_attach(cp3_detail)" in base
            or "vslot_40_533100_streams_attach(csp_detail)" in base, "wired in base initialize")

    print(f"OK check_f533100_vslot40_streams_attach: {SIZE} B {SITE_SHA256[:12]}…; "
          f"key/addref/remove×2/null/f590/lock; slot={SLOT_OFF:#x}; literal pinned")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
