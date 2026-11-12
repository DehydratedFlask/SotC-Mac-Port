"""Shared helpers for tools/lift/check_helper_{8ef00,534580,499e90,498b20,354470}.py (CPU-01-SLOT408-PREREQS).

Offline and read-only (bytes of the pinned private ELF through slot_pin_util.Elf; nothing is loaded or executed). Provides
the startup-slot tie: a slot body is hash-pinned, a rip-relative `lea` scan of that body lists the exact instruction
addresses that load a destructor RVA (the atexit callback argument), and each listed site is re-read with e.rip.
"""
from __future__ import annotations

import re
import struct
from pathlib import Path

from slot_pin_util import require

ROOT = Path(__file__).resolve().parents[2]


def lea_scan(data: bytes, base: int, target: int) -> list[int]:
    """Addresses of 7 byte `REX.W lea r64, [rip + disp32]` instructions in `data` (loaded at `base`) that target `target`."""
    out = []
    for i in range(len(data) - 7):
        if data[i] & 0xF8 == 0x48 and data[i + 1] == 0x8D and data[i + 2] & 0xC7 == 0x05:
            if base + i + 7 + struct.unpack("<i", data[i + 3:i + 7])[0] == target:
                out.append(base + i)
    return out


def slot_ties(e, slots, sites) -> None:
    """`slots`: [(name, base, size, sha)] hash-pinned (always); `sites`: [(what, slot_base, slot_size, lea_site, target)].

    For each site: opcode byte 0x8d at site+1, rip operand == target; and (outside the byte-mutation run) the slot body holds
    exactly the listed lea sites for that target."""
    for name, base, size, sha in slots:
        e.body_hash(name, base, size, sha, always=True)
    by_target: dict[tuple[int, int, int], list[int]] = {}
    for what, base, size, site, target in sites:
        require(base <= site < base + size, f"{what}: site {site:#x} lies in the slot body")
        require(e.read(site + 1, 1) == b"\x8d", f"{what}: `lea` opcode at {site + 1:#x}")
        require(e.rip(site, 7, 3) == target, f"{what}: rip operand at {site:#x} is {target:#x}")
        by_target.setdefault((base, size, target), []).append(site)
    if not e.skip_hash:
        for (base, size, target), listed in by_target.items():
            found = lea_scan(e.raw(base, size), base, target)
            require(found == sorted(listed), f"slot {base:#x}: lea sites of {target:#x} are {[hex(x) for x in found]}")


def header_hex_list(rel: str, name: str) -> list[int]:
    """The 0x... values of the `inline constexpr std::array ... name = {...};` initialiser in a native header."""
    text = (ROOT / rel).read_text()
    match = re.search(rf"\b{re.escape(name)}\s*=\s*\{{(.*?)\}};", text, re.S)
    require(match is not None, f"missing native array {name}")
    return [int(v, 16) for v in re.findall(r"0x[0-9a-fA-F]+", match.group(1))]
