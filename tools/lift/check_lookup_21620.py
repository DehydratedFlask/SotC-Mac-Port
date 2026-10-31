#!/usr/bin/env python3
"""Byte-oracle for app-main registry lookup 0x21620 (464 B full body). Luna E1042."""
import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
HOT_SHA = "787bf4ada236589505be9d753be8317445d253e8f43a2db55f102b0cf77b5185"
BODY_SHA = "a7ba043b102574823a8381f36c654b1bf30f6ef812b64e209e9b438d670413bd"
RVA = 0x21620
HOT_SIZE = 449
BODY_SIZE = 0x1D0
NEXT_FN = 0x217F0


def require(cond, msg):
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img, site):
    raw = img.read(site, 5)
    require(raw[0] == 0xE8, f"expected call at {site:#x}")
    rel = struct.unpack("<i", raw[1:5])[0]
    return site + 5 + rel


def main():
    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as d  # noqa: E402

    _, ph = d.program_headers(raw)
    img = d.Image(raw, ph)
    hot = img.read(RVA, HOT_SIZE)
    body = img.read(RVA, BODY_SIZE)
    require(hashlib.sha256(hot).hexdigest() == HOT_SHA, "449 B hot sha256")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA, "464 B body sha256")
    require(body[HOT_SIZE - 1] == 0xC3, "ret @ end of 449 B hot span (0x217e0)")
    require(body[HOT_SIZE + 3] == 0xE9, "cold jmp rel32 @ 0x217e4")
    require(img.read(NEXT_FN, 1) == b"\x55", "next function prologue @ 0x217f0")

    require(call_target(img, 0x21689) == 0x3B390, "lock begin call")
    require(call_target(img, 0x21701) == 0x3B3C0, "lock end call (site 1)")
    require(call_target(img, 0x2170E) == 0x3B3C0, "lock end call (site 2)")

    hdr = (ROOT / "native/core/lookup_21620.hpp").read_text()
    require("NOT IMPLEMENTED" in hdr and "787bf4ada2365895" in hdr, "contract header")
    require("a7ba043b10257482" in hdr, "full body sha pin")
    require(str(BODY_SIZE) in hdr or "0x1d0" in hdr.lower(), "body size pin")
    memo = ROOT / "docs/checkpoints/2026-10-30-init-21620-luna-spec-e1042.md"
    require(memo.is_file(), "Luna spec memo")

    cpp = ROOT / "native/core/lookup_21620.cpp"
    require(not cpp.is_file(), "native body must not exist until contract pass (Luna stop rules)")

    print(
        f"OK check_lookup_21620: hot {HOT_SIZE} B {HOT_SHA[:12]}…; "
        f"body {BODY_SIZE} B {BODY_SHA[:12]}…; 3b390/3b3c0 calls pinned; "
        "lookup semantics NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
