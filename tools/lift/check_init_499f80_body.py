#!/usr/bin/env python3
"""Byte-oracle for app-main helper body 0x499f80 (193 B). CPU-01-499F80-BODY-NATIVE-01."""
import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_SHA256 = "9ea23d248ce29318034c8f929f5cc435bbf7f92f57113ebf93b5b4c8e853f8ce"


def require(cond, msg):
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main():
    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    body = raw  # use ps4 image read via objdump pin from E1031
    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as d  # noqa: E402

    _, ph = d.program_headers(raw)
    img = d.Image(raw, ph)
    got = img.read(0x499F80, 193)
    require(hashlib.sha256(got).hexdigest() == BODY_SHA256, "0x499f80 body sha256")

    hdr = (ROOT / "native/core/init_499f80_body.hpp").read_text()
    require("0x499f80" in hdr and "alloc_wrappers_17070" in hdr and "0x3b310" in hdr,
            "header contract incomplete")
    require(re.search(r"kInit499f80BodySize\s*=\s*193", hdr), "body size pin")
    require("run_init_499f80_body" in (ROOT / "native/core/init_499f80_body.cpp").read_text(),
            "missing body runner")

    print("OK: 0x499f80 body hash pinned; native init_499f80_body module present")


if __name__ == "__main__":
    main()
