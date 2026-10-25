"""Shared helpers for the single-slot startup-constructor ELF checkers (303, 302, 305, ...).

Offline and read-only: bytes of the pinned private ELF are read through `ps4_dynlink.Image`; nothing
is loaded, relocated or executed. `Elf` records every byte range that a check reads (`pins`) and can
apply an in-memory overlay, which the checkers use for their mutation self-test: flipping the first
and last byte of every pin must make the check fail (the whole-body hash is disabled for that run).
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE_START = 0x2DD70E0
DSO = 0x1604218
ATEXIT = 0xF8F390
RELEASE = 0xF590


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def const_value(text: str, name: str) -> int:
    match = re.search(rf"\b{re.escape(name)}\s*=\s*(0x[0-9a-fA-F]+|\d+)u?\s*;", text)
    require(match is not None, f"missing native constant {name}")
    return int(match.group(1), 0)


class Elf:
    def __init__(self) -> None:
        elf = (ROOT / "local/re/eboot.elf").read_bytes()
        require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
        _, ph = d.program_headers(elf)
        self.img = d.Image(elf, ph)
        self.relocs = {r["target"]: r for r in d.parse(elf)["relocs"]}
        self.overlay: dict[int, int] = {}
        self.reloc_flip: set[int] = set()    # mutation: relocation slots whose addend is perturbed
        self.skip_hash = False
        self.pins: set[tuple[int, int]] = set()
        self.reloc_pins: set[int] = set()
        self.recording = False

    def raw(self, addr: int, size: int) -> bytes:
        data = bytearray(self.img.read(addr, size))
        for off in range(size):
            if addr + off in self.overlay:
                data[off] = self.overlay[addr + off]
        return bytes(data)

    def read(self, addr: int, size: int) -> bytes:
        if self.recording:
            self.pins.add((addr, size))
        return self.raw(addr, size)

    def u64(self, addr: int) -> int:
        return struct.unpack("<Q", self.read(addr, 8))[0]

    def rip(self, site: int, length: int, disp_off: int) -> int:
        raw = self.read(site + disp_off, 4)
        return site + length + struct.unpack("<i", raw)[0]

    def lea(self, site: int) -> int:
        b = self.read(site, 7)
        require(b[0] in (0x48, 0x4C) and b[1] == 0x8D and (b[2] & 0xC7) == 0x05, f"no lea at {site:#x}")
        return self.rip(site, 7, 3)

    def call(self, site: int) -> int:
        b = self.read(site, 5)
        require(b[0] == 0xE8, f"no call at {site:#x}")
        return site + 5 + struct.unpack("<i", b[1:5])[0]

    def jmp(self, site: int) -> int:
        b = self.read(site, 5)
        require(b[0] == 0xE9, f"no jmp at {site:#x}")
        return site + 5 + struct.unpack("<i", b[1:5])[0]

    def reloc_target(self, slot_addr: int) -> int:
        r = self.relocs.get(slot_addr)
        require(r is not None and r["type"] == 8, f"no RELATIVE relocation at {slot_addr:#x}")
        if self.recording:
            self.reloc_pins.add(slot_addr)
        return r["addend"] ^ (1 if slot_addr in self.reloc_flip else 0)

    def bytes_at(self, site: int, hexstr: str, what: str) -> None:
        want = bytes.fromhex(hexstr)
        require(self.read(site, len(want)) == want, f"{what} at {site:#x}")

    def cstring(self, addr: int, text: bytes) -> None:
        require(self.read(addr, len(text) + 1) == text + b"\0", f"string {text!r} at {addr:#x}")

    def body_hash(self, name: str, rva: int, size: int, sha: str, always: bool = False) -> None:
        # The hash of the slot body itself is skipped during the mutation run so that the per-
        # instruction pins, not the whole-body hash, have to catch each mutant. Shared callees
        # (`always=True`) are pinned by their hash only, so the hash stays active for them.
        body = self.read(rva, size)
        if always or not self.skip_hash:
            require(digest(body) == sha, f"{name} ({rva:#x}) sha256")
