#!/usr/bin/env python3
"""ELF-tie the snapshotBugData block (E585).

Private: needs local/re/eboot.elf. Pins the 0x25d8b0 block, the gate clear/call targets, the
screenshot readback body, the Frame-root helpers, the synthetic literals and the native order.
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
FUNCTIONS = [
    ("0x25d8b0 BugData block", 0x25D8B0, 1086,
     "e3eeb54982d51f409a2113e2c9ce9c084c04cd635bac8272104def9bdde84e27"),
    ("0x961a0 screenshot readback", 0x961A0, 1022,
     "3690d267ca56a8946f74c80ac25b06bfc1026463dcec32fa3e91c87e1df0d219"),
    ("0xa70c30 Frame get", 0xA70C30, 220,
     "33074135a3486645057378fb6041f670fe5d9318eb24781fbeec6b0a417f4e24"),
]
HEADER = ROOT / "native/core/snapshot_bug_data.hpp"
SOURCE = ROOT / "native/core/snapshot_bug_data.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img, addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for name, rva, size, sha in FUNCTIONS:
        require(hashlib.sha256(img.read(rva, size)).hexdigest() == sha, f"{name} sha256")

    require(call_target(img, 0x25DB7B) == 0xBA40, "gate clear target")
    require(call_target(img, 0x25DB9E) == 0x1BDF30, "report build target")
    require(img.read(0xFB3E39, 8) == b"Command\x00", '"Command" literal')
    require(img.read(0xFB3E41, 11) == b"screenshot\x00", '"screenshot" literal')
    require(img.read(0x1BE236, 10) == bytes.fromhex("48b865656e73686f7400"),
            "stack-built screenshot text")

    header = HEADER.read_text()
    for token in (
        "kSnapshotBugDataBodySize = 1086",
        "kSnapshotBugDataGateClearRva = 0x25DB7B",
        "kSnapshotBugDataCallRva = 0x25DB9E",
        "kScreenshotQueryRva = 0x961A0",
        "bug_screenshot",
        "G1 PARTIAL",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    order = ["query_screenshot", "insert_1bdc80", "run_app_info_1bdf30", "adopt_root"]
    positions = [source.find(name) for name in order]
    require(all(pos >= 0 for pos in positions), "composition calls present")
    require(positions == sorted(positions), "query -> insert -> build -> adopt order")

    print(
        "OK check_fsnapshot_bug_data: 0x25d8b0/0x961a0/0xa70c30 hashes, gate clear -> 0xba40, "
        "build -> 0x1bdf30, synthetic literals, native query/insert/build/adopt order; "
        "GPU readback and file sink NOT lifted"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
