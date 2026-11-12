#!/usr/bin/env python3
"""ELF-tie checker for the perspective-projection builder 0x1580a0 (192 B) and its slot 408 call site (CPU-01-HELPER-CALLEE-MODELS-04).

Pins instruction by instruction the whole body [0x1580a0, 0x158160): prologue, the three argument spills, `*fov * 0.5f` (dword 0xfaad0c), the tanf PLT call
(thunk 0xf8f410, GOT 0x15ffff8, NID ZE6RNL+eLbk proven through the JMPREL row), `1.0f / t` (dword 0xfaad10), the `test r14b, r14b` branch and its two
arms (near / (near - far) against far / (far - near)), the sign-mask xor (0xfaad30) and the sixteen stores of the output matrix. The three rodata constants
are read from the image. The seven direct callers are re-derived: a scan of the whole text window proves there are exactly those `call rel32` sites to
0x1580a0, no `jmp rel32` tail call to it and no relocation addend equal to 0x1580a0. The slot 408 call site 0x5c8082 is pinned argument by argument (out =
0x2fda41c, fov cell = 0x3f490fdb at [rsp+0x60], xmm0 = [0x126633c] = 16/9, xmm1 = [0x1266340] = 0.5, xmm2 = [0x1266344] = 4200.0, edx = 1) and the whole slot
body (0x5c7f50, 4750 B) is hash-pinned and contains exactly one call to 0x1580a0.

NOT established: which of near / far / reversed-depth the original authors meant (names are INFERENCE from the observed values); the behaviour of the six
other callers beyond "they call it"; that no instruction between the argument setup and the call at 0x5c8082 changes xmm1 / xmm2 / edx beyond the straight-line
reading of the pinned slot body (the slot body is hash-pinned, not re-executed).

Usage: python3 -I tools/lift/check_helper_1580a0.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_e as pins_e  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_e.PINS)

TARGET = 0x1580A0
CALLERS = (0xA7038, 0xA8978, 0xB1110, 0x1198C7, 0x119F79, 0x20E79A, 0x5C8082)
SLOT_408 = ("slot 408 (0x5c7f50)", 0x5C7F50, 4750, "73c66ded4a5facf4c6ab4df025f86bc043455dad9e4a3dcd9f25ca50982201eb")
TEXT_END = 0xF8F000


def x_callers(e) -> None:
    for site in CALLERS:
        require(e.call(site) == TARGET, f"caller at {site:#x} reaches 0x1580a0")
    if e.skip_hash:                         # the global scans are whole-image facts, like the body hashes: skipped in the byte-mutation run
        return
    text = e.raw(0, TEXT_END)
    calls, jmps = [], []
    for opcode, found in ((0xE8, calls), (0xE9, jmps)):
        pos = text.find(bytes([opcode]))
        while pos != -1:
            if pos + 5 <= len(text) and pos + 5 + struct.unpack("<i", text[pos + 1:pos + 5])[0] == TARGET:
                found.append(pos)
            pos = text.find(bytes([opcode]), pos + 1)
    require(calls == sorted(CALLERS), f"exactly the listed direct callers of 0x1580a0, got {[hex(c) for c in calls]}")
    require(jmps == [], f"no jmp rel32 tail call to 0x1580a0, got {[hex(c) for c in jmps]}")
    require(not [a for a, r in e.relocs.items() if r["type"] == 8 and r["addend"] in (TARGET, TARGET ^ 1)],
            "no relocation addend names 0x1580a0 (never address-taken)")


def x_slot408(e) -> None:
    name, base, size, sha = SLOT_408
    fde = lib.fde_ranges()
    if fde is not None:
        require(fde.get(hex(base)) == size, f"{name}: FDE length")
    e.body_hash(name, base, size, sha, always=True)
    body = e.raw(base, size)
    found = [base + i for i in range(size - 4) if body[i] == 0xE8
             and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == TARGET]
    require(found == [0x5C8082], f"{name}: exactly one call to 0x1580a0")
    require(e.lea(0x5C8060) == 0x2FDA41C, "slot 408: rdi = the output cell 0x2fda41c")
    e.bytes_at(0x5C8067, "488d742460", "slot 408: rsi = rsp + 0x60 (the fov cell)")
    e.bytes_at(0x5C8014, "c744246 0db0f493f".replace(" ", ""), "slot 408: fov cell = 0x3f490fdb")
    e.bytes_at(0x5C8045, "ba01000000", "slot 408: edx = 1")
    require(e.rip(0x5C807A, 8, 4) == 0x126633C, "slot 408: xmm0 (aspect) loaded from 0x126633c")
    e.bytes_at(0x5C807A, "c5fa1005", "slot 408: vmovss xmm0, [rip+...]")
    require(e.rip(0x5C7F8B, 8, 4) == 0x1266340, "slot 408: xmm1 (near) loaded from 0x1266340")
    e.bytes_at(0x5C7F8B, "c5fa100d", "slot 408: vmovss xmm1, [rip+...]")
    require(e.rip(0x5C7F93, 8, 4) == 0x1266344, "slot 408: xmm2 (far) loaded from 0x1266344")
    e.bytes_at(0x5C7F93, "c5fa1015", "slot 408: vmovss xmm2, [rip+...]")
    e.bytes_at(0x126633C, "398ee33f", "aspect 16/9 = 0x3fe38e39")
    e.bytes_at(0x1266340, "0000003f", "near 0.5 = 0x3f000000")
    e.bytes_at(0x1266344, "00408345", "far 4200.0 = 0x45834000")


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F410, 0x15FFFF8, "tanf"),))
    e.bytes_at(0xFAAD0C, "0000003f", "constant 0.5f")
    e.bytes_at(0xFAAD10, "0000803f", "constant 1.0f")
    e.bytes_at(0xFAAD30, "00000080" * 4, "sign mask (16 B)")
    x_callers(e)
    x_slot408(e)


SPEC = {
    "name": "check_helper_1580a0",
    "summary": ("0x1580a0 (192 B perspective builder) pinned per instruction, tanf NID-proven, rodata constants read, seven direct callers re-derived by a "
                "whole-text scan, slot 408 call site (out, fov, aspect, near, far, flag) pinned argument by argument"),
    "families": [],
    "bodies": [
        {"name": "0x1580a0 perspective builder", "rva": TARGET, "size": 192, "fde": 192, "pins": "perspective_1580a0",
         "sha": "66cffb30018e1d6210dc52ab66859df9b96ea140ec58f75525f529bdbce645c0"},
    ],
    "rips": [
        ("fov * 0.5f", 0x1580C4, 8, 4, 0xFAAD0C), ("1.0f / tan", 0x1580D1, 8, 4, 0xFAAD10), ("sign mask", 0x158111, 8, 4, 0xFAAD30),
    ],
    "calls": [("tanf PLT", 0x1580CC, 0xF8F410)],
    "jmps": [],
    "relocs": [],
    "bytes": [
        ("flag test: test r14b, r14b", 0x1580D9, "4584f6"),
        ("flag branch: je +0x14", 0x1580E5, "7414"),
        ("m[11] = 1.0f, m[12] = 0 (one qword store)", 0x158138, "48c743 2c0000803f".replace(" ", "")),
        ("m[13] = 0", 0x158140, "c743340000 0000".replace(" ", "")),
        ("m[15] = 0", 0x158150, "c7433c00000000"),
        ("m[10] store (xmm3)", 0x158133, "c5fa115b28"),
        ("m[14] store (xmm1)", 0x15814B, "c5fa114b38"),
        ("m[5] store (xmm0)", 0x158129, "c5fa114314"),
        ("m[0] store (xmm2)", 0x158119, "c5fa1113"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_1580a0_perspective.hpp", "helper_c": "native/core/helper_1580a0_perspective.cpp",
        "test": "native/tests/test_helper_1580a0.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kPerspective1580a0Rva": TARGET, "kPerspective1580a0Size": 192, "kPerspectiveTanfPltRva": 0xF8F410,
                     "kPerspectiveTanfGotRva": 0x15FFFF8, "kPerspectiveHalfBits": 0x3F000000, "kPerspectiveOneBits": 0x3F800000,
                     "kPerspectiveSignBits": 0x80000000, "kPerspectiveMatrixFloats": 16,
                     "kSlot408PerspectiveCallRva": 0x5C8082, "kSlot408PerspectiveOutRva": 0x2FDA41C, "kSlot408FovBits": 0x3F490FDB,
                     "kSlot408AspectBits": 0x3FE38E39, "kSlot408NearBits": 0x3F000000, "kSlot408FarBits": 0x45834000, "kSlot408Flag": 1},
    },
    "tokens": {
        "helper_h": ["// 0x1580a0. Writes all 16 elements of `out` on success; nothing on refusal.", "enum class PerspectiveRefusal"],
        "helper_c": ["result.tan_argument = *fov * bits_to_float(kPerspectiveHalfBits);", "const float t = tanf_hook(result.tan_argument);",
                     "const float cot = bits_to_float(kPerspectiveOneBits) / t;", "const float x_scale = cot / aspect;",
                     "if ((flag & 0xffu) != 0) {", "denominator = near_plane - far_plane;", "depth_scale = near_plane / denominator;",
                     "denominator = far_plane - near_plane;", "depth_scale = far_plane / denominator;",
                     "const float depth_offset = negate(near_plane * far_plane) / denominator;",
                     "out[0] = x_scale;", "out[5] = cot;", "out[10] = depth_scale;", "out[11] = bits_to_float(kPerspectiveOneBits);",
                     "out[14] = depth_offset;", "out[15] = 0.0f;"],
        "test": ["CHECK(m[10] == 101.0f / 100.0f);", "CHECK(m[10] == 1.0f / (1.0f - 101.0f));", "CHECK(c[10] != a[10]);",
                 "PerspectiveRefusal::no_tanf_hook"],
        "cmake": ["core/helper_1580a0_perspective.cpp", "add_test(NAME helper_1580a0 COMMAND test_helper_1580a0)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
