#!/usr/bin/env python3
"""Check the source identity and decoded save-body checksum invariants (stdlib only)."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_SHA256 = "0e10cab6ef5646275365017d20e8baaf90f26bc0ac33edeb88cb97c1d105c981"
BODY_ADDRESS = 0x161F50
BODY_SIZE = 38
SEED = 0x811C9DC5
PRIME = 0x01000193


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL: {message}")


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    require(len(elf) >= 64, "truncated ELF header")
    require(elf[:7] == b"\x7fELF\x02\x01\x01", "expected little-endian ELF64")
    require(struct.unpack_from("<H", elf, 18)[0] == 62, "expected x86-64 source ELF")
    phoff = struct.unpack_from("<Q", elf, 32)[0]
    entry_size, count = struct.unpack_from("<HH", elf, 54)
    require(entry_size >= 56, "invalid program-header entry size")
    require(phoff + entry_size * count <= len(elf), "truncated program-header table")
    loads = []
    for index in range(count):
        offset = phoff + index * entry_size
        if struct.unpack_from("<I", elf, offset)[0] != 1:
            continue
        file_offset, vaddr, _paddr, file_size, _memory_size, _align = struct.unpack_from("<QQQQQQ", elf, offset + 8)
        require(file_offset + file_size <= len(elf), "PT_LOAD exceeds source file")
        loads.append((file_offset, vaddr, file_size))
    require(bool(loads), "no PT_LOAD segments")
    return loads


def read_va(elf: bytes, loads: list[tuple[int, int, int]], address: int, size: int) -> bytes:
    offsets = [file_offset + address - vaddr for file_offset, vaddr, file_size in loads
               if vaddr <= address and address + size <= vaddr + file_size]
    require(len(offsets) == 1, f"virtual range {address:#x}+{size:#x} is not uniquely file-backed")
    offset = offsets[0]
    return elf[offset:offset + size]


def check_call(elf: bytes, loads: list[tuple[int, int, int]], address: int, target: int) -> None:
    instruction = read_va(elf, loads, address, 5)
    require(instruction[0] == 0xE8, f"expected rel32 call at {address:#x}")
    displacement = struct.unpack_from("<i", instruction, 1)[0]
    require(address + 5 + displacement == target, f"call at {address:#x} has unexpected target")


def check_caller(elf: bytes, loads: list[tuple[int, int, int]], seed_address: int,
                 length_address: int, data_call_address: int, hash_call_address: int) -> None:
    seed_instruction = read_va(elf, loads, seed_address, 5)
    require(seed_instruction[0] == 0xBA, f"expected 32-bit seed load at {seed_address:#x}")
    require(struct.unpack_from("<I", seed_instruction, 1)[0] == SEED,
            f"unexpected checksum seed at {seed_address:#x}")
    require(read_va(elf, loads, length_address, 3) == bytes((0x8B, 0x71, 0x10)),
            f"expected dword body length at {length_address:#x}")
    check_call(elf, loads, data_call_address, 0xA51330)
    check_call(elf, loads, hash_call_address, BODY_ADDRESS)


def verify(elf_path: Path) -> None:
    elf = elf_path.read_bytes()
    elf_digest = hashlib.sha256(elf).hexdigest()
    require(elf_digest == ELF_SHA256, "source ELF identity mismatch")
    loads = load_segments(elf)
    body = read_va(elf, loads, BODY_ADDRESS, BODY_SIZE)
    body_digest = hashlib.sha256(body).hexdigest()
    require(body_digest == BODY_SHA256, "checksum helper body digest mismatch")

    require(body[:3] == bytes((0x48, 0x85, 0xF6)), "expected length test")
    require(body[3] == 0x7E, "expected signed nonpositive-length branch")
    empty_branch = struct.unpack_from("<b", body, 4)[0]
    require(3 + 2 + empty_branch == 35, "empty-length branch must return the seed")
    require(body[16:19] == bytes((0x0F, 0xB6, 0x07)), "expected zero-extended byte load")
    require(body[22:24] == bytes((0x31, 0xD0)), "expected byte XOR with 32-bit state")
    require(body[24:26] == bytes((0x69, 0xD0)), "expected 32-bit multiply")
    require(struct.unpack_from("<I", body, 26)[0] == PRIME, "unexpected FNV-1a 32-bit prime")
    require(body[30:33] == bytes((0x48, 0x39, 0xF7)), "expected end-pointer comparison")
    require(body[33] == 0x72, "expected unsigned loop branch")
    loop_branch = struct.unpack_from("<b", body, 34)[0]
    require(33 + 2 + loop_branch == 16, "loop branch must return to the byte load")
    require(body[35:38] == bytes((0x89, 0xD0, 0xC3)), "expected state return")

    check_caller(elf, loads, 0xA4B5F1, 0xA4B5F9, 0xA4B5E8, 0xA4B5FC)
    check_caller(elf, loads, 0xA49341, 0xA49349, 0xA49338, 0xA4934C)
    print(f"PASS: ELF {elf_digest}; helper body sha256 {body_digest}; decoded helper and Save/Load callsite invariants")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "local/re/eboot.elf")
    verify(parser.parse_args().elf)


if __name__ == "__main__":
    main()
