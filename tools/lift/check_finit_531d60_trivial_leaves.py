#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 trivial remainder leaves (appmain triage).

Private: needs local/re/eboot.elf. Pins:
  0x4c4950 (21 B) → cells 0x2efa890/0x2efa891;
  setters 0xa52b70/+0x01, 0xa599e0/+0x29, 0xa431e0/+0x51;
  one-byte ret stubs; splash PLT 0xf90310 = jmp [GOT].
Does NOT claim base Initialize 0x255130 or full Init.
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
NATIVE = ROOT / "native/core/init_531d60_trivial_leaves.hpp"
SLICE = ROOT / "native/core/appmain_init_remainder_slice.hpp"

LEAVES = {
    0x4C4950: ("291bec4aa6e219a6", 21),  # prefix of sha256
    0xA52B70: ("92c88ac2c035e95f", 5),
    0xA599E0: ("be6f381037cb22f8", 5),
    0xA431E0: ("d88a6ab36c9b8916", 5),
}
RETS = (0x56DCA0, 0x56DB70, 0x5B3D00, 0x5B3CF0, 0x56FAD0)
SPLASH_PLT = 0xF90310


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for addr, (sha_prefix, size) in LEAVES.items():
        body = img.read(addr, size)
        digest = hashlib.sha256(body).hexdigest()
        require(digest.startswith(sha_prefix), f"{addr:#x} sha {digest[:16]}…")

    # 0x4c4950 lea targets
    b = img.read(0x4C4950, 21)
    require(b[0:3] == bytes.fromhex("488d05"), "lea rax")
    disp = struct.unpack("<i", b[3:7])[0]
    require(0x4C4950 + 7 + disp == 0x2EFA890, "cell 0x2efa890")
    disp2 = struct.unpack("<i", b[10:14])[0]
    require(0x4C4957 + 7 + disp2 == 0x2EFA891, "cell 0x2efa891")
    require(b[14:21] == bytes.fromhex("408838c60101c3"), "store dil + set 1 + ret")

    require(img.read(0xA52B70, 5) == bytes.fromhex("40887701c3"), "setter +0x01")
    require(img.read(0xA599E0, 5) == bytes.fromhex("40887729c3"), "setter +0x29")
    require(img.read(0xA431E0, 5) == bytes.fromhex("40887751c3"), "setter +0x51")

    for addr in RETS:
        require(img.read(addr, 1) == b"\xc3", f"ret stub {addr:#x}")

    # Splash PLT: ff 25 xx xx xx xx
    plt = img.read(SPLASH_PLT, 6)
    require(plt[0:2] == bytes.fromhex("ff25"), "splash PLT jmp [rip]")
    got_disp = struct.unpack("<i", plt[2:6])[0]
    got = SPLASH_PLT + 6 + got_disp
    require(got == 0x1600778, f"splash GOT {got:#x}")

    # Init call sites still present
    require(img.read(0x5327A0, 5)[0] == 0xE8, "call 0x4c4950 site")
    require(img.read(0x53290B, 5)[0] == 0xE8, "call setter site")
    require(img.read(0x531E46, 5)[0] == 0xE8, "call splash site")

    hdr = NATIVE.read_text()
    require("set_init_flag_pair_4c4950" in hdr, "flag API")
    require("set_byte_a52b70" in hdr and "set_byte_a431e0" in hdr, "setter API")
    require("hide_splash_screen_f90310" in hdr, "splash API")
    require("NOT modelled" in hdr or "not modelled" in hdr.lower(), "omission")
    require("0x255130" in hdr, "base Init omission cite")

    slice_hdr = SLICE.read_text()
    require("run_appmain_init_remainder_slice" in slice_hdr, "remainder slice API")
    require("run_appmain_init_registry_slice" in slice_hdr or "registry" in slice_hdr.lower(),
            "wires registry")

    print(
        "OK check_finit_531d60_trivial_leaves: flag/setters/rets/splash PLT pinned; "
        "base Initialize NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
