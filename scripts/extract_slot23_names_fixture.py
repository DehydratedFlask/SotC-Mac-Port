#!/usr/bin/env python3
"""Extract the 14 slot-23 (0xe71990) name strings into a private local fixture.

Reads the pinned private ELF at local/re/eboot.elf (never published). Writes:
  local/fixtures/slot23_e71990_names.bin
  local/fixtures/slot23_e71990_names.meta.json  (lengths/hashes/RVAs only; no string bytes)

Binary format (little-endian):
  magic  'S23N' (4)
  u32    version (=1)
  u32    count  (=14)
  repeated count times:
    u32  byte_length
    <byte_length bytes>   # raw string bytes WITHOUT a trailing NUL; loader adds NUL

Does not print string contents. Exit non-zero on hash mismatch.
"""

from __future__ import annotations

import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "lift"))
import leaf_ctor_decode_sym as Y  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
# LEA targets decoded from ctor 0xe71990 (tools/lift/check_fe71990.py STORES).
NAME_RVAS = [
    0x131C990,
    0x131C9A2,
    0x131C9B2,
    0x131C9C3,
    0x131C9D5,
    0x131C9E4,
    0x131C9F7,
    0x131CA0A,
    0x131CA1D,
    0x131CA30,
    0x131CA44,
    0x131CA59,
    0x131CA78,
    0x131CA94,
]
# Hash immediates from the original ctor (also native/core/leaf_e71990.hpp).
HASHES = [
    0xF1E58049,
    0x257DA323,
    0x47A2C413,
    0xED7E6AE8,
    0xB2D47DCD,
    0x4941F51C,
    0x4941F51D,
    0x4941F51E,
    0x4941F51F,
    0x10EF7257,
    0x7CAE59C9,
    0x330BBB24,
    0x2B1488E7,
    0x00000000,  # sentinel immediate; not name_hash(string)
]


def djb2_xor(data: bytes) -> int:
    h = 5381
    for b in data:
        h = ((h * 33) ^ (b - 256 if b >= 128 else b)) & 0xFFFFFFFF
    return h or 1


def read_cstr(image, addr: int) -> bytes:
    out = bytearray()
    a = addr
    while True:
        b = image.read(a, 1)[0]
        if b == 0:
            break
        out.append(b)
        a += 1
        if len(out) > 4096:
            raise RuntimeError(f"string at {addr:#x} exceeds 4096 bytes")
    return bytes(out)


def main() -> int:
    elf_path = ROOT / "local" / "re" / "eboot.elf"
    if not elf_path.is_file():
        print(f"missing private ELF: {elf_path}", file=sys.stderr)
        return 2
    digest = hashlib.sha256(elf_path.read_bytes()).hexdigest()
    if digest != ELF_SHA256:
        print(f"unexpected ELF sha256: {digest}", file=sys.stderr)
        return 2

    _, image = Y.load_image()
    records = []
    strings = []
    for i, (rva, expected) in enumerate(zip(NAME_RVAS, HASHES)):
        raw = read_cstr(image, rva)
        strings.append(raw)
        if i < 13:
            got = djb2_xor(raw)
            if got != expected:
                print(f"record {i}: name_hash mismatch {got:#x} != {expected:#x}", file=sys.stderr)
                return 1
        records.append(
            {
                "index": i,
                "original_rva": rva,
                "byte_len": len(raw),
                "hash": expected,
                "sha256": hashlib.sha256(raw).hexdigest(),
            }
        )

    out_dir = ROOT / "local" / "fixtures"
    out_dir.mkdir(parents=True, exist_ok=True)
    fixture = out_dir / "slot23_e71990_names.bin"
    payload = bytearray()
    payload += b"S23N"
    payload += struct.pack("<I", 1)
    payload += struct.pack("<I", 14)
    for raw in strings:
        payload += struct.pack("<I", len(raw))
        payload += raw
    fixture.write_bytes(payload)

    meta = {
        "fixture": "local/fixtures/slot23_e71990_names.bin",
        "format": "S23N v1",
        "sha256": hashlib.sha256(payload).hexdigest(),
        "count": 14,
        "source_elf_sha256": ELF_SHA256,
        "ctor_rva": 0xE71990,
        "env": "SOTC_SLOT23_NAMES_FIXTURE",
        "records": records,
    }
    (out_dir / "slot23_e71990_names.meta.json").write_text(json.dumps(meta, indent=2) + "\n")
    print(
        f"OK wrote {fixture.relative_to(ROOT)} sha256={meta['sha256']} "
        f"bytes={len(payload)} records=14 (strings not printed)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
