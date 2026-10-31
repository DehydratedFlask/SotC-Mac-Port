#!/usr/bin/env python3
"""Byte-oracle for CritSec helpers 0x3b310 (106 B) and 0x3b380 (5 B). RE-02-CRITSEC-3B310-3B380-NATIVE-01.

Offline only: reads local/re/eboot.elf. Pins body digests and import/settype/CritSec string facts
(shared with tools/lift/lane_m5_check.critsec_extra). Verifies native/core/critsec_3b310.{hpp,cpp}
documents the RVAs and sidecar inference.
"""
import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf  # noqa: E402
import lane_m5_check as m5  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_SHA = "262dbcd4b170dd06ab8fa9f8c1d30839f93abe91f4c9825dbe65699901cbf1c1"
DTOR_SHA = "de79f6373ce5a48f02b6ea56b3fb54c88d603d639900604b1b51ee6f78e3e565"


def require(cond, msg):
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main():
    elf_path = ROOT / "local/re/eboot.elf"
    raw = elf_path.read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")

    e = Elf()
    require(hashlib.sha256(e.read(0x3B310, 106)).hexdigest() == CTOR_SHA, "0x3b310 body sha256")
    require(hashlib.sha256(e.read(0x3B380, 5)).hexdigest() == DTOR_SHA, "0x3b380 body sha256")
    m5.critsec_extra(e)

    hdr = (ROOT / "native/core/critsec_3b310.hpp").read_text()
    src = (ROOT / "native/core/critsec_3b310.cpp").read_text()
    require("0x3b310" in hdr and "0x3b380" in hdr, "header missing RVA constants")
    require("PTHREAD_MUTEX_RECURSIVE" in src, "critsec must use plain recursive mutex")
    require("PTHREAD_PRIO_INHERIT" not in src, "critsec must not set priority inheritance")
    require("sidecar" in hdr.lower() or "sidecar" in src.lower(), "sidecar layout documented")
    require(re.search(r"kCritSecHandleBytes\s*=\s*8", hdr), "handle size must be 8")
    require("default_critsec_slot414_hooks" in src, "default Slot414 hooks missing")

    print("OK: critsec 0x3b310/0x3b380 bodies pinned; import sequence verified; native module contract present")


if __name__ == "__main__":
    main()
