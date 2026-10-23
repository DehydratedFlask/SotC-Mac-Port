#!/usr/bin/env python3
"""ELF-tie the 0x2569d0 gate/mode dispatch and the generated property table.

Private: needs local/re/eboot.elf. Pins the gate byte test, the provable mode-1 write, the map A/B
record copy instructions, and the generated table hash/counts.
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE = ROOT / "native/core/owner_properties_2569d0_table.inc.hpp"
TABLE_SHA256 = "ca62ba66bba2b51ce4a181ba971894497003bbf4dc2fb6b8b0f99c84d2474f5a"
NATIVE_H = ROOT / "native/core/owner_properties_2569d0.hpp"
NATIVE_C = ROOT / "native/core/owner_properties_2569d0.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    # Gate: lea rbx,[0x2dfce70]; mov rax,[rbx+0x60]; cmp byte [rax],0; jne skip.
    require(img.read(0x2570A9, 20) == bytes.fromhex("488d1dc05dba02488b4360803800751d488d05c8"),
            "RetailBuild gate byte sequence")
    # Mode read: mov eax,[r15+0x10] at 0x256d27.
    require(img.read(0x256D27, 4) == bytes.fromhex("418b4710"), "mode read [r15+0x10]")
    # The only map ctor writes mode 1: mov dword [r15+0x10],1.
    require(img.read(0xA2B143, 8) == bytes.fromhex("41c7471001000000"), "mode write 1 at 0xa2b143")

    # Map A record copy: key -> +0x1c, tag -> +0x28, payload -> +0x20.
    require(img.read(0xAC52B9, 3) == bytes.fromhex("89581c"), "map A key store +0x1c")
    require(img.read(0xAC52BC, 3) == bytes.fromhex("895028"), "map A tag store +0x28")
    require(img.read(0xAC52C3, 4) == bytes.fromhex("48895020"), "map A payload store +0x20")
    # Map B record copy: vmovups [rdx] then vmovups [rax+0x20].
    require(img.read(0xAC5345, 4) == bytes.fromhex("c5f81002"), "map B 16-byte record load")
    require(img.read(0xAC5379, 3) == bytes.fromhex("89581c"), "map B key store +0x1c")
    require(img.read(0xAC537C, 5) == bytes.fromhex("c5f8114020"), "map B 16-byte value store")

    table = TABLE.read_text()
    require(hashlib.sha256(TABLE.read_bytes()).hexdigest() == TABLE_SHA256,
            "generated table sha256")
    paths = re.findall(r"\{0x[0-9a-f]+, \d, (\d),", table)
    require(len(paths) == 260, f"table rows {len(paths)}")
    counts = {i: paths.count(str(i)) for i in range(7)}
    require(counts == {0: 2, 1: 5, 2: 130, 3: 1, 4: 120, 5: 1, 6: 1},
            f"table path counts {counts}")

    header = NATIVE_H.read_text().lower()
    for token in (
        "classify_owner_mode",
        "ownerpropertysink",
        "apply_owner_properties",
        "mode2_unreachable",
        "not lifted",
    ):
        require(token in header, f"header token {token!r}")
    source = NATIVE_C.read_text()
    require("retail_gate1" in source and "retail_gate0" in source, "mode dispatch")

    print(
        "OK check_fowner_properties_2569d0: RetailBuild gate, mode==1 proof, map A/B record copies, "
        "260-row table sha=%s…, paths {2,5,130,1,120,1,1}; map inserts NOT executed"
        % TABLE_SHA256[:12]
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
