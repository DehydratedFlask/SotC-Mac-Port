#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x56dcb0.

Private: needs local/re/eboot.elf. Pins 5 B fragment after post-base 0x499f80.
E1426 executes the native 132 B body in init_post_bodies.cpp; later continues stay separate.
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
FRAG = 0x531DF4
FRAG_SIZE = 5
FRAG_SHA256 = "f05f5ca1438a59b2280791b6df9e40b6e8c8a45ee1a1cdcca2e7c678e53f3cd6"
END = 0x531DF9
TARGET = 0x56DCB0
BODY_SIZE = 126
BODY_SHA256 = "195d7ee2edf3d17815d43f5fea45b7233b8354fade52f9ac562463edf4e564ea"
OUTER_946 = 0x946880
OUTER_946_SIZE = 353
OUTER_946_SHA256 = "f99d118f75cbcd85f5e0f2406be2459e30246dc3972473d19d40d8f32d7847c0"
INNER_946 = 0x9469F0
INNER_946_SIZE = 360
INNER_946_SHA256 = "45f573c4a2dff259f7fb81fa95e281d26cd92c7520bec6c7c51b2e37fcea7379"
NATIVE = ROOT / "native/core/init_531d60_post_base_56dcb0.hpp"
NATIVE_946 = ROOT / "native/core/init_946880_body.hpp"
RUNTIME = ROOT / "native/core/init_window_runtime.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


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

    require(call_target(0x531DEF, img) == 0x499F80, "prev 0x499f80")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x56dcb0")
    require(call_target(END, img) == 0x56DCA0, "next 0x56dca0 (E406 ret-nop)")

    body = img.read(TARGET, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha")
    require(body[-1:] == b"\xc3", "body ret")
    require(call_target(0x56DCFC, img) == OUTER_946, "first 0x946880 call")
    require(call_target(0x56DD03, img) == OUTER_946, "second 0x946880 call")

    outer = img.read(OUTER_946, OUTER_946_SIZE)
    require(hashlib.sha256(outer).hexdigest() == OUTER_946_SHA256, "0x946880 body sha")
    for site, target in {
        0x946909: 0x98A450,
        0x946924: 0x944DE0,
        0x946930: INNER_946,
        0x9469BB: 0x984700,
        0x9469DB: 0xF8F3A0,  # compiler stack-check trap; pinned, never called
    }.items():
        require(call_target(site, img) == target, f"0x946880 call {site:#x}")

    inner = img.read(INNER_946, INNER_946_SIZE)
    require(hashlib.sha256(inner).hexdigest() == INNER_946_SHA256, "0x9469f0 body sha")
    for site, target in {
        0x946A36: 0x986F30,
        0x946A49: 0x987720,
        0x946A5A: 0x9882E0,
        0x946A78: 0x988400,
        0x946AA8: 0x988400,
        0x946ADC: 0x9556A0,
        0x946B03: 0x9556A0,
        0x946B1E: 0x947E30,
        0x946B29: 0x9878D0,
        0x946B32: 0x987260,
        0x946B52: 0xF8F3A0,  # compiler stack-check trap; pinned, never called
    }.items():
        require(call_target(site, img) == target, f"0x9469f0 call {site:#x}")
    # stack-chk cell used inside body (not claimed)
    lea = img.read(TARGET + 0x0B, 7)
    require(lea[0:3] == bytes.fromhex("4c8b35"), "mov r14,[rip]")
    cell = TARGET + 0x0B + 7 + struct.unpack("<i", lea[3:7])[0]
    require(cell == 0x15FE518, "guard cell in body")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_56dcb0" in nh, "API")
    require("132 B body is native" in nh and "E1426" in nh, "E1426 native body claim")
    require("post_base_56dcb0" in SLICE.read_text() or "PostBase56dcb0" in SLICE.read_text(), "wire")

    native_946 = NATIVE_946.read_text()
    require("run_init_946880_body" in native_946 and "run_init_9469f0_body" in native_946,
            "0x946880/0x9469f0 native APIs")
    require("kInit946880BodyRva = 0x946880" in native_946, "0x946880 RVA")
    require("kInit9469f0BodyRva = 0x9469f0" in native_946, "0x9469f0 RVA")
    require("kInit944de0BodyRva = 0x944de0" in native_946, "0x944de0 contract RVA")
    runtime = RUNTIME.read_text()
    require("run_init_946880_body" in runtime and "hook_56_946880" in runtime,
            "0x946880 runtime wire")
    require("body absent: 353 B" not in runtime, "stale 0x946880 blocker removed")

    print(
        f"OK check_f531d60_post_base_56dcb0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x}; end={END:#x}; 56dcb0 body native (E1426); "
        f"0x946880 native (353 B) + 0x9469f0 native (360 B); "
        f"0x944de0 remains a named 399 B contract; 5c64c0/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
