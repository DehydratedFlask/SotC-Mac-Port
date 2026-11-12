#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x5c64c0 and its 0x5d5410 chain.

Private: needs local/re/eboot.elf. Pins the 5 B fragment after 0x56dcb0/0x56dca0 and
the full 0x5c64c0 prologue. E1429 executes the native 0x5d5410/0x8be60/0x8f520/0x8f1d0
bodies; the 0x12820 allocator and 0x87f3e0 -> 0xf906d0 platform tail stay named host
contracts, and the 0x8f628/0x8f276 stack-check calls stay pinned but are never called.
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
FRAG = 0x531DFE
FRAG_SIZE = 5
FRAG_SHA256 = "11e4ea9d33f244a619eaacfaa0633b319b9360b6257f97f71aeca57b9bf67b24"
END = 0x531E03
TARGET = 0x5C64C0
PROLOGUE_SIZE = 0x70
PROLOGUE_SHA256 = "a4744b2a2dc8cfdc56611e67d88aae91f2f68a0e652fb509290e33794e92831a"
STORE_CELL = 0x2FDAA60

# Chain bodies and their exact FDE spans.
BODY_5D5410 = (0x5D5410, 0x1A4, "f23ba6181ca405a95206e5cb601d8195686062c795a8a7168f8007b34cf015e2")
BODY_8BE60 = (0x8BE60, 0x223, "95be6ad805ce8fb75beb079e6a1c4689a0886b58702094d8ab38dd2510544d3e")
BODY_8F520 = (0x8F520, 0x10D, "a3514fb484fca6ada6135a5a319995a7aebde47eabc91d4d332dc6ead50ea084")
BODY_8F1D0 = (0x8F1D0, 0xAB, "59cb70a1bd9032706345a1868427a6d2c659290f09daa3b8a2d3e12e8463b6e3")

CALLS_5D5410 = {
    0x5D543B: 0x8BE60,
    0x5D5465: 0x8BE60,
    0x5D54A2: 0x8F520,
    0x5D54CB: 0x8F1D0,
    0x5D5510: 0x8F520,
    0x5D5536: 0x8F1D0,
    0x5D5572: 0x8F520,
    0x5D559E: 0x8F1D0,
}
CALLS_8BE60 = {
    0x8BED2: 0x12820,
    0x8BF06: 0x12820,
    0x8BF3A: 0x12820,
    0x8BF4B: 0xF8F470,
    0x8BF86: 0x8655D0,
    0x8BF9C: 0x865600,
    0x8BFB5: 0x865500,
    0x8BFF0: 0x8652D0,
    0x8C013: 0x865360,
    0x8C06F: 0x87F3E0,
}
CALLS_8F520 = {
    0x8F555: 0x87CB90,
    0x8F55F: 0x87CC40,
    0x8F56C: 0x87CC00,
    0x8F579: 0x87CBA0,
    0x8F586: 0x87CC10,
    0x8F593: 0x87CC20,
    0x8F59E: 0x87CBC0,
    0x8F5A9: 0x87CBE0,
    0x8F5B5: 0x87CD70,
    0x8F5C6: 0x87CD80,
    0x8F5D7: 0x87CDA0,
    0x8F628: 0xF8F3A0,  # compiler stack-check trap; pinned, never called
}
CALLS_8F1D0 = {
    0x8F201: 0x87C380,
    0x8F21D: 0x87C390,
    0x8F22A: 0x87C400,
    0x8F23A: 0x87C3B0,
    0x8F24A: 0x87C3D0,
    0x8F276: 0xF8F3A0,  # compiler stack-check trap; pinned, never called
}

NATIVE_H = ROOT / "native/core/init_5d5410_body.hpp"
NATIVE_CPP = ROOT / "native/core/init_5d5410_body.cpp"
RUNTIME = ROOT / "native/core/init_window_runtime.cpp"
MAIN = ROOT / "native/app/sotc_init_window_main.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_target(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0:2] == bytes.fromhex("488d"), f"missing lea at {site:#x}")
    return site + 7 + struct.unpack("<i", b[3:7])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(call_target(0x531DF4, img) == 0x56DCB0, "prev 0x56dcb0")
    require(call_target(0x531DF9, img) == 0x56DCA0, "intervening 0x56dca0 ret-nop")
    require(img.read(0x56DCA0, 1) == b"\xc3", "0x56dca0 is ret")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x5c64c0")
    require(call_target(END, img) == 0x1290, "next 0x1290 (not claimed)")

    pro = img.read(TARGET, PROLOGUE_SIZE)
    require(hashlib.sha256(pro).hexdigest() == PROLOGUE_SHA256, "prologue sha")
    require(lea_target(TARGET + 0x0C, img) == STORE_CELL, "store cell")

    # The three chain bodies and the two record builders.
    for rva, size, sha in (BODY_5D5410, BODY_8BE60, BODY_8F520, BODY_8F1D0):
        body = img.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"body sha {rva:#x}")
    require(img.read(0x5D5410 + BODY_5D5410[1] - 1, 1) == b"\xc3", "0x5d5410 ret")

    for site, target in CALLS_5D5410.items():
        require(call_target(site, img) == target, f"0x5d5410 call {site:#x}")
    for site, target in CALLS_8BE60.items():
        require(call_target(site, img) == target, f"0x8be60 call {site:#x}")
    for site, target in CALLS_8F520.items():
        require(call_target(site, img) == target, f"0x8f520 call {site:#x}")
    for site, target in CALLS_8F1D0.items():
        require(call_target(site, img) == target, f"0x8f1d0 call {site:#x}")
    require(call_target(0x86554E, img) == 0x8666D0, "0x865500 -> 0x8666d0")

    # State/owner addressing at the call sites.
    require(lea_target(0x5D541A, img) == 0x30015C8, "object A rva")
    require(lea_target(0x5D5444, img) == 0x30015F0, "object B rva")
    require(lea_target(0x5D546E, img) == 0x2ED33F0, "owner cell rva")
    require(lea_target(0x5D54AB, img) == 0x3001618, "state rva")
    require(img.read(0x5D548E, 10) == bytes.fromhex("68ff00000068ff000000"), "stack ff ff")

    # 0x8be9e jumps before r15 is initialized at 0x8bea4; the model must refuse that path.
    require(img.read(0x8BE9E, 2) == bytes.fromhex("0f85"), "0x8be9e jne")
    jne_target = 0x8BE9E + 6 + struct.unpack("<i", img.read(0x8BEA0, 4))[0]
    require(jne_target == 0x8BF5E, "0x8be9e target")
    require(jne_target > 0x8BEA4, "jump skips r15 clear")
    require(img.read(0x8BEA4, 3) == bytes.fromhex("4531ff"), "r15 clear at 0x8bea4")

    # 0x87f3e0 is a thunk to 0xf906d0, which jumps through the import slot at 0x1600958.
    thunk = img.read(0x87F3E0, 5)
    require(thunk[0] == 0xE9, "0x87f3e0 thunk")
    require(0x87F3E0 + 5 + struct.unpack("<i", thunk[1:5])[0] == 0xF906D0, "thunk target")
    slot = img.read(0xF906D0, 6)
    require(slot[0:2] == bytes.fromhex("ff25"), "0xf906d0 import jump")
    slot_rva = 0xF906D0 + 6 + struct.unpack("<i", slot[2:6])[0]
    require(slot_rva == 0x1600958, "import slot rva")

    nh = NATIVE_H.read_text()
    nc = NATIVE_CPP.read_text()
    require("kInit5d5410BodyRva = 0x5d5410" in nh and "kInit5d5410BodySize = 0x1a4" in nh,
            "0x5d5410 constants")
    require("kInit8be60BodyRva = 0x8be60" in nh and "kInit8be60BodySize = 0x223" in nh,
            "0x8be60 constants")
    require("kInit8f520BodyRva = 0x8f520" in nh and "kInit8f520BodySize = 0x10d" in nh,
            "0x8f520 constants")
    require("kInit8f1d0BodyRva = 0x8f1d0" in nh and "kInit8f1d0BodySize = 0xab" in nh,
            "0x8f1d0 constants")
    require("kInit8f520StackChkSiteRva = 0x8f628" in nh, "0x8f520 trap site")
    require("kInit8f1d0StackChkSiteRva = 0x8f276" in nh, "0x8f1d0 trap site")
    require("never called" in nh, "trap sites labeled never called")
    require("run_init_5d5410_body" in nh and "run_init_8be60_body" in nh, "native APIs")
    require("run_init_8f520_body" in nh and "run_init_8f1d0_body" in nh, "builder APIs")
    require("0x12820" in nh and "0x87f3e0" in nh, "host contracts named")
    require("0xf8f3a0" not in nc, "trap target not called by the model")

    runtime = RUNTIME.read_text()
    require('#include "init_5d5410_body.hpp"' in runtime, "runtime include")
    require("run_init_5d5410_body" in runtime and "hook_5d5410" in runtime,
            "runtime hook")
    require("p.hooks_5c.call_5d5410 = hook_5d5410;" in runtime, "0x5c64c0 hook wire")
    require("host_5d_alloc" in runtime and "host_5d_tail" in runtime, "host contracts")
    require("body absent: 0x5d5410" not in runtime, "stale 0x5d5410 blocker removed")
    require("post_5d5410" in MAIN.read_text(), "E1427 JSON object")

    require("init_531d60_post_base_5c64c0" in SLICE.read_text() or
            "PostBase5c64c0" in SLICE.read_text(), "call-site wire")

    print(
        f"OK check_f531d60_post_base_5c64c0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x}; end={END:#x}; 0x5d5410/0x8be60/0x8f520/0x8f1d0 native (E1429); "
        f"{len(CALLS_5D5410)} + {len(CALLS_8BE60)} + {len(CALLS_8F520)} + "
        f"{len(CALLS_8F1D0)} direct calls pinned; 0x12820 and 0x87f3e0 host contracts; "
        f"0xf8f3a0 trap sites never called"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
