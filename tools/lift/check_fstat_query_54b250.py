#!/usr/bin/env python3
"""ELF-tie the native lift of stat-query wrapper 0x54b250.

Private: needs local/re/eboot.elf.
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
RVA = 0x54B250
SIZE = 119
FN_SHA256 = "40814086ed16f3968b83c8a049541ddbf8bcee557ebb39d0e657a00f07b0d39f"
END = 0x54B2C7
ENGINE = 0xA56560
TRAITS = 0x15C9920
NOOP = 0x538200
GUARD = 0x15FE518
NATIVE = ROOT / "native/core/stat_query_54b250.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    got = hashlib.sha256(body).hexdigest()
    require(got == FN_SHA256, f"fn 0x54b250 sha256: {got}")
    require(img.read(END - 1, 1) == b"\xc3", "ret at 0x54b2c6")

    # frame + guard load
    require(body[0:4] == bytes.fromhex("554889e5"), "push rbp; mov rbp,rsp")
    require(body[0x0F:0x12] == bytes.fromhex("488b1d"), "guard load")
    require(RVA + 0x0F + 7 + struct.unpack("<i", body[0x12:0x16])[0] == GUARD, "guard cell")
    # traits lea
    require(body[0x1A:0x1D] == bytes.fromhex("488d0d"), "traits lea")
    require(RVA + 0x1A + 7 + struct.unpack("<i", body[0x1D:0x21])[0] == TRAITS, "traits 0x15c9920")
    # engine call
    call_off = 0x54
    require(body[call_off] == 0xE8, "engine call opcode")
    require(RVA + call_off + 5 + struct.unpack("<i", body[call_off + 1:call_off + 5])[0] == ENGINE,
            "engine 0xa56560")
    # no-op dispatch encoding: call *0x18(%rax) at 0x54b2b1 (offset 0x61)
    require(body[0x61:0x64] == bytes.fromhex("ff5018"), "call *0x18(%rax)")

    # traits table slot 3 must be the known bare-ret no-op.
    slot3 = struct.unpack("<Q", img.read(TRAITS + 0x18, 8))[0]
    require(slot3 == NOOP, f"traits slot 3 = 0x{slot3:x} (expect 0x538200)")
    require(img.read(NOOP, 1) == b"\xc3", "0x538200 is a bare ret")

    header = NATIVE.read_text().lower()
    for token in (
        "kStatQuery54b250Rva = 0x54b250",
        "kStatQuery54b250Size = 119",
        "kStatQuery54b250EngineRva = 0xa56560",
        "kStatQuery54b250TraitsRva = 0x15c9920",
        "kStatQuery54b250NoopSlotRva = 0x538200",
        "kStatQuery54b250GuardCellRva = 0x15fe518",
        "not modelled",
    ):
        require(token.lower() in header, f"header token {token!r}")

    print(
        f"OK check_fstat_query_54b250: 0x54b250+{SIZE}B sha={FN_SHA256[:12]}…; "
        f"engine 0x{ENGINE:x}; traits 0x{TRAITS:x} slot3=0x{NOOP:x} (bare ret); "
        f"guard 0x{GUARD:x}; engine body/descriptor layout/return value NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
