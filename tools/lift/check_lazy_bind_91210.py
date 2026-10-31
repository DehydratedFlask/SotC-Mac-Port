#!/usr/bin/env python3
"""Byte-oracle for 0x91210 (0x58 B). Pins the vtable+0x30 polarity: eax==0 keeps the bind."""
import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_SHA256 = "46be9cad276c1d24bd7a7b6b23ab45d4616346774a4d9aee77ba63b53166b802"
# test eax,eax; je +8; mov qword [rbx+0x28], 0
ROLLBACK_IF_NONZERO = bytes.fromhex("85c0740848c7432800000000")


def require(cond, msg):
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main():
    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as d  # noqa: E402

    _, ph = d.program_headers(raw)
    img = d.Image(raw, ph)
    body = img.read(0x91210, 0x58)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "0x91210 body sha256")
    require(ROLLBACK_IF_NONZERO in body, "vtable+0x30 nonzero path clears +0x28")

    hdr = (ROOT / "native/core/lazy_bind_91210.hpp").read_text()
    cpp = (ROOT / "native/core/lazy_bind_91210.cpp").read_text()
    require("kLazyBind91210Size = 0x58" in hdr, "size pin")
    require(BODY_SHA256[:16] in hdr, "sha pin")
    require("vcall_30" in cpp and "rolled_back" in cpp, "rollback path")
    require("run_lazy_bind_91210" in cpp, "runner")
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/lazy_bind_91210.cpp" in cmake, "cmake source")
    require("add_test(NAME lazy_bind_91210" in cmake, "cmake test")
    print(f"OK check_lazy_bind_91210: 0x58 B sha {BODY_SHA256[:12]}…; eax==0 keeps +0x28")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
