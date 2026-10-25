#!/usr/bin/env python3
"""ELF-tie 0xa473b0 (capture/streaming enable-state setter, 134 B).

Private: needs local/re/eboot.elf.

Pins the region extent/sha256, every rel32 call target, the RIP-relative stack-guard
cell, and the opaque set-info constants, and checks the native header carries them.
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
RVA = 0xA473B0
SIZE = 134
END = 0xA47436
REGION_SHA256 = "e755e3f053753a4ee69d42c710ce84323abf96bf9f43366891b2f3f495c8f7a9"

GUARD_CELL = 0x15FE518
GUARD_NID = "f7uOxY9mM1U"          # __stack_chk_guard (libkernel OBJECT)
HEADER = ROOT / "native/core/capture_a473b0.hpp"

# call-site RVA -> (thunk RVA, NID)
IMPORTS = [
    (0xA473CF, 0xF91380, "dWM80AX39o4"),  # sceGameLiveStreamingEnableLiveStreaming
    (0xA473D8, 0xF913A0, "2xxUtuC-RzE"),  # sceScreenShotEnable
    (0xA473E6, 0xF91390, "tIYf0W5VTi8"),  # sceScreenShotDisable
    (0xA47400, 0xF913B0, "Fc8qxlKINYQ"),  # sceVideoRecordingSetInfo (0x0d)
    (0xA4741A, 0xF913B0, "Fc8qxlKINYQ"),  # sceVideoRecordingSetInfo (0xa007)
    (0xA47431, 0xF8F3A0, "Ou3iL1abvng"),  # __stack_chk_fail
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def rel32_target(code: bytes, at: int) -> int:
    """Return the absolute target of a `call`/`jmp` rel32 at `at` (= RVA + 1 + disp32)."""
    return at + 5 + struct.unpack("<i", code[at - RVA + 1:at - RVA + 5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    parsed = d.parse(elf)
    img = d.Image(elf, parsed["ph"])

    body = img.read(RVA, SIZE)
    require(len(body) == SIZE and RVA + SIZE == END, "span arithmetic")
    require(hashlib.sha256(body).hexdigest() == REGION_SHA256, "region sha256")

    # prologue pin.
    require(body[0:11] == bytes.fromhex("554889e54156534883ec10"), "prologue bytes")

    # RIP-relative guard cell: 0xa473bb `mov r14,[rip+disp32]` -> 0x15fe518.
    require(body[0x0B:0x0E] == bytes.fromhex("4c8b35"), "mov r14,[rip+..]")
    guard_disp = RVA + 0x0B + 7 + struct.unpack("<i", body[0x0E:0x12])[0]
    require(guard_disp == GUARD_CELL, f"guard cell {guard_disp:#x} != {GUARD_CELL:#x}")

    # movzx ebx,sil (the bool arg) and the canary save.
    require(body[0x12:0x16] == bytes.fromhex("400fb6de"), "movzx ebx,sil")
    require(body[0x16:0x18] == bytes.fromhex("89df"), "mov edi,ebx")
    require(body[0x18:0x1B] == bytes.fromhex("498b06"), "mov rax,[r14]")
    require(body[0x1B:0x1F] == bytes.fromhex("488945e8"), "mov [rbp-0x18],rax")

    # every rel32 call target.
    for site, thunk, nid in IMPORTS:
        off = site - RVA
        require(body[off] == 0xE8, f"call opcode at {site:#x}")
        tgt = rel32_target(body, site)
        require(tgt == thunk, f"call at {site:#x} -> {tgt:#x}, want {thunk:#x}")

    # the enable/disable branch and the state dword writes to [rbp-0x1c].
    require(body[0x24:0x26] == bytes.fromhex("84db"), "test bl,bl")
    require(body[0x26:0x28] == bytes.fromhex("740e"), "je +0x0e")
    require(body[0x2D:0x34] == bytes.fromhex("c745e400000000"), "dword -0x1c = 0 (enable)")
    require(body[0x3B:0x42] == bytes.fromhex("c745e401000000"), "dword -0x1c = 1 (disable)")
    require(body[0x63:0x6A] == bytes.fromhex("c745e400000000"), "dword -0x1c = 0 (reset before 0xa007)")

    # opaque set-info type ids (imm32 follows the `bf` opcode).
    require(struct.unpack_from("<I", body, 0x47)[0] == 0x0D, "edi=0x0d")
    require(struct.unpack_from("<I", body, 0x5A)[0] == 0xA007, "edi=0xa007")

    # canary compare `cmp rax,[rbp-0x18]` and `jne`.
    require(body[0x72:0x76] == bytes.fromhex("483b45e8"), "cmp rax,[rbp-0x18]")
    require(body[0x76:0x78] == bytes.fromhex("7509"), "jne fail")

    # guard cell is an R_X86_64_64 slot for libkernel OBJECT __stack_chk_guard.
    guard_slot = None
    for r in parsed["relocs"]:
        if r["target"] == GUARD_CELL:
            guard_slot = r
            break
    require(guard_slot is not None, "no relocation at guard cell")
    require(guard_slot["type"] == 1, "guard cell is not R_X86_64_64")
    gsym = parsed["symbols"][guard_slot["symbol"]]
    require(gsym["nid"] == GUARD_NID, f"guard symbol nid {gsym['nid']} != {GUARD_NID}")

    header = HEADER.read_text()
    tokens = (
        "kCaptureA473b0Rva = 0xa473b0",
        "kCaptureA473b0Size = 134",
        "kCaptureA473b0EndRva = 0xa47436",
        "kCaptureA473b0GuardCellRva = 0x15fe518",
        "kCaptureVideoInfoTypeD = 0x0d",
        "kCaptureVideoInfoTypeA007 = 0xa007",
        "sceGameLiveStreamingEnableLiveStreaming",
        "sceScreenShotEnable",
        "sceScreenShotDisable",
        "sceVideoRecordingSetInfo",
        "__stack_chk_fail",
        "dWM80AX39o4",
        "2xxUtuC-RzE",
        "tIYf0W5VTi8",
        "Fc8qxlKINYQ",
        "Ou3iL1abvng",
        "NOT modelled",
    )
    for token in tokens:
        require(token in header, f"header token {token!r}")

    n_calls = sum(1 for _ in IMPORTS)
    print(
        f"OK check_fa473b0: 0x{RVA:x}+{SIZE}B sha {REGION_SHA256[:12]}…; "
        f"guard 0x{GUARD_CELL:x} (__stack_chk_guard); {n_calls} rel32 calls "
        f"(→ {', '.join(hex(t) for _, t, _ in IMPORTS)}); state dword 0/1; set-info 0x0d/0xa007"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())