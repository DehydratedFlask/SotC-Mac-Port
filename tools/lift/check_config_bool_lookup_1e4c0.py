#!/usr/bin/env python3
"""Byte-oracle for app-main 0x1e4c0 (0x140 B, E524). CPU-01-INIT-1E4C0-SHARED-LOOKUP-01."""
import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_SHA256 = "1188f8517326e086907a871b96482be50005651deed17f9b55fb22c51c4afbc1"
BODY_RVA = 0x1E4C0
BODY_SIZE = 0x140
TOKENS = {
    0xF95A1E: "true",
    0xF95A23: "yes",
    0xF95A27: "false",
    0xF95A2D: "no",
}
NID_STRCASE = "AV6ipCNa4Rw"
NID_STOUL = "zlfEH8FmyUA"


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
    got = img.read(BODY_RVA, BODY_SIZE)
    require(hashlib.sha256(got).hexdigest() == BODY_SHA256, "0x1e4c0 body sha256 (E524 0x140 B)")

    for addr, token in TOKENS.items():
        blob = img.read(addr, len(token) + 1)
        require(blob == token.encode() + b"\0", f"token {token!r} at {addr:#x}")

    require(d.nid("strcasecmp") == NID_STRCASE, "strcasecmp NID")
    require(d.nid("_Stoul") == NID_STOUL, "_Stoul NID")

    hdr = (ROOT / "native/core/config_bool_lookup_1e4c0.hpp").read_text()
    cpp = (ROOT / "native/core/config_bool_lookup_1e4c0.cpp").read_text()
    require(str(BODY_SIZE) in hdr or "0x140" in hdr, "body size pin in header")
    require(BODY_SHA256[:16] in hdr, "body sha pin in header")
    require("kConfigBoolTableBeginOffset" in hdr and "0x30" in hdr, "table begin offset")
    require("kConfigBoolEntrySize" in hdr and "16" in hdr, "16-byte entry pin")
    require("config_bool_lookup_object" in cpp and "config_bool_lookup_args_span" in cpp,
            "both entry paths implemented")
    require("strcasecmp" in cpp and "strtoul" in cpp, "parse imports")
    require((ROOT / "native/tests/test_config_bool_lookup_1e4c0.cpp").is_file(), "missing test")
    sndx = (ROOT / "native/core/sndx_command_handlers.cpp").read_text()
    require("config_bool_lookup_args_span" in sndx, "sndx wired to span lookup")

    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/config_bool_lookup_1e4c0.cpp" in cmake, "CMake source")
    require("add_test(NAME config_bool_lookup_1e4c0" in cmake, "CMake test")

    print(
        f"OK check_config_bool_lookup_1e4c0: 0x1e4c0 {BODY_SIZE} B sha {BODY_SHA256[:12]}…; "
        "tokens + table/span native module"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
