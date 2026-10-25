#!/usr/bin/env python3
"""ELF-tie the composed 0x1bdf30 run path (E584).

Private: needs local/re/eboot.elf. Pins the list-C dispatch entry bytes and the native order
(dispatch before the report snapshot).
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
HEADER = ROOT / "native/core/app_info_run.hpp"
SOURCE = ROOT / "native/core/app_info_run.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for addr, pattern, what in (
        (0x1BDF30, "554889e54157415641554154", "0x1bdf30 prologue"),
        (0x1BDF66, "488b5f40", "list-C dispatch loads container+0x40"),
        (0x1BDF79, "488b1b", "list-C dispatch follows node+0x00"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)

    header = HEADER.read_text()
    for token in (
        "kAppInfoRunDispatchStartRva = 0x1BDF66",
        "kAppInfoRunDispatchNextRva = 0x1BDF79",
        "RegistryOwner::dispatch",
        "app_info_snapshot",
        "G1 PARTIAL",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    require(source.find("owner.dispatch()") < source.find("app_info_snapshot("),
            "dispatch before snapshot")

    print(
        "OK check_fapp_info_run: 0x1bdf30 prologue, list-C dispatch bytes at 0x1bdf66/0x1bdf79, "
        "native dispatch-before-snapshot order; gate/screenshot/adoption NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
