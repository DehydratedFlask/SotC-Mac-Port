#!/usr/bin/env python3
"""ELF-tie the composed 0x1bdf30 report path (E578).

Private: needs local/re/eboot.elf. Pins the four phase entry bytes, their order, the
"FieldEntry"/"Attachment"/"name" literals, and the native composition order.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PHASES = [
    (0x1BE14D, "498b5d00", "list-A serialize"),
    (0x1BE2F7, "498d4520", "list-B serialize"),
    (0x1BE3F4, "4d8b3424", "list-A drain"),
    (0x1BE458, "498b5d00", "list-B clear"),
]
HEADER = ROOT / "native/core/app_info_snapshot.hpp"
SOURCE = ROOT / "native/core/app_info_snapshot.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for addr, pattern, what in PHASES:
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, f"{what} at {addr:#x}")
    require([p[0] for p in PHASES] == sorted(p[0] for p in PHASES), "phase order")

    require(img.read(0xFB3E29, 11) == b"FieldEntry\x00", '"FieldEntry" literal')
    require(img.read(0xFB3E4C, 11) == b"Attachment\x00", '"Attachment" literal')
    require(img.read(0xFB3E34, 5) == b"name\x00", '"name" literal')

    header = HEADER.read_text().lower()
    for token in (
        "kappinfolistaserializerva = 0x1be14d",
        "kappinfolistaserializeendrva = 0x1be222",
        "fieldentry",
        "app_info_snapshot",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    order = ["serialize_list_a", "appinfo_listb_serialize", "drain_app_info",
             "appinfo_listb_clear"]
    positions = [source.find(name) for name in order]
    require(all(pos >= 0 for pos in positions), "composition calls present")
    require(positions == sorted(positions), "composition order")

    print(
        "OK check_fapp_info_snapshot: phases 0x1be14d < 0x1be2f7 < 0x1be3f4 < 0x1be458; "
        '"FieldEntry"/"Attachment"/"name" literals; native order A-serialize, B-serialize, '
        "A-drain, B-clear; screenshot query and root adoption NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
