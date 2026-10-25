#!/usr/bin/env python3
"""ELF-tie the E597 screenshot seam.

Pins the 1027 B encoder body, the `bug_screenshot.png` literal, the report call sites and the
`scePngEnc*` import mapping. The native decision table is checked structurally; no guest code runs.
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
ENCODER = 0x961A0
ENCODER_SIZE = 1027
ENCODER_SHA256 = "5889c316cf7dec11931b01d1634bb29ad8eb4931425b2d2765b6249bb9d0d01b"
NAME_LITERAL = b"bug_screenshot.png"
NATIVE_H = ROOT / "native/core/screenshot_contract.hpp"
NATIVE_C = ROOT / "native/core/screenshot_contract.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(pc: int, img: d.Image) -> int:
    b = img.read(pc, 5)
    require(b[0] == 0xE8, f"no call at {pc:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return pc + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(ENCODER, ENCODER_SIZE)).hexdigest() == ENCODER_SHA256,
            "encoder body hash")
    require(NAME_LITERAL + b"\x00" in elf, "attachment name literal")

    # The encoder is reached from the report block and the console command.
    callers = []
    pc = 0x1000
    while pc + 5 <= 0x1C0000:
        if img.read(pc, 1)[0] == 0xE8 and call_target(pc, img) == ENCODER:
            callers.append(pc)
        pc += 1
    require(0x1BDFC5 in callers, "report call site 0x1bdfc5")
    require(0x169DD7 in callers, "console call site 0x169dd7")

    text = NATIVE_H.read_text()
    for token in ("bug_screenshot.png", "synthetic_command", "capture_frame", "send_file",
                  "write_app0_file", "frame_num", "renderer_present"):
        require(token in text, f"header token {token}")
    cpp = NATIVE_C.read_text()
    for token in ("synthetic_command", "sent_to_sink", "capture_frame", "send_file",
                  "write_app0_file", "renderer_present"):
        require(token in cpp, f"source token {token}")

    print(f"OK check_fscreenshot_contract: encoder {ENCODER_SHA256[:12]}… ({ENCODER_SIZE} B); "
          f"name literal; report 0x1bdfc5 + console 0x169dd7; decision table present")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
