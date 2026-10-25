#!/usr/bin/env python3
"""Extract the 275 input-table (0x551140) name-string pairs into a private local fixture.

Reads the pinned private ELF at local/re/eboot.elf plus the local simulated table image
at local/validation/inputtable-agent/inputtable_image_PRIVATE.bin (pointer VAs only as a
decode cache; never published). Writes:

  local/fixtures/inputtable_551140_names.bin
  local/fixtures/inputtable_551140_names.meta.json  (lengths / RVAs / hashes only; no string bytes)

Binary format (little-endian), magic 'I275', version 1:
  magic  'I275' (4)
  u32    version (=1)
  u32    count  (=275)
  repeated count times:
    u32  len_a ; <len_a bytes>   # name_a WITHOUT trailing NUL; loader adds NUL
    u32  len_b ; <len_b bytes>   # name_b WITHOUT trailing NUL

Does not print string contents. Exit non-zero on ELF / hash-pair digest mismatch.
Pinned hash-pair payload (275 × {u32 h1=fnv_cstr_52a990(name_a), u32 h2=fnv_cstr_a2ddc0(name_b)} LE)
sha256 must equal HASHPAIR_SHA256 below.
"""

from __future__ import annotations

import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
IMAGE = ROOT / "local/validation/inputtable-agent/inputtable_image_PRIVATE.bin"
COUNT = 275
STRIDE = 0x60
HASHPAIR_SHA256 = "a1aa059e47b80f1d450d46f47d0885858d04323299c8e01b6a3bd2425f329258"
OUT_BIN = ROOT / "local/fixtures/inputtable_551140_names.bin"
OUT_META = ROOT / "local/fixtures/inputtable_551140_names.meta.json"
FNV_SEED = 0x811C9DC5
FNV_PRIME = 0x01000193


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def read_cstr(img: d.Image, addr: int) -> bytes:
    out = bytearray()
    a = addr
    while True:
        b = img.read(a, 1)[0]
        if b == 0:
            break
        out.append(b)
        a += 1
        require(len(out) < 4096, f"cstr too long at {addr:#x}")
    return bytes(out)


def fnv_sx(data: bytes) -> int:
    h = FNV_SEED
    for b in data:
        c = b if b < 128 else b - 256
        h = ((h ^ (c & 0xFFFFFFFF)) * FNV_PRIME) & 0xFFFFFFFF
    return h


def ascii_toupper(b: int) -> int:
    return b - 32 if 97 <= b <= 122 else b


def fnv_path(data: bytes) -> int:
    h = FNV_SEED
    for b in data:
        if b == 0xFF:
            folded = 0xFF
        else:
            folded = ascii_toupper(b)
            if folded == 0x5C:
                folded = 0x2F
        h = ((h ^ folded) * FNV_PRIME) & 0xFFFFFFFF
    return h


def main() -> int:
    elf_path = ROOT / "local/re/eboot.elf"
    require(elf_path.is_file(), "missing local/re/eboot.elf")
    require(IMAGE.is_file(), "missing inputtable_image_PRIVATE.bin (local decode cache)")
    elf = elf_path.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    image = IMAGE.read_bytes()
    require(len(image) == COUNT * STRIDE, "image size")

    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    pairs: list[tuple[int, bytes, int, bytes, int, int]] = []
    hash_blob = bytearray()
    for i in range(COUNT):
        off = i * STRIDE
        va_a, va_b = struct.unpack_from("<QQ", image, off)
        require(va_a != 0 and va_b != 0, f"null name VA at idx {i}")
        sa = read_cstr(img, va_a)
        sb = read_cstr(img, va_b)
        h1 = fnv_sx(sa)
        h2 = fnv_path(sb)
        hash_blob += struct.pack("<II", h1, h2)
        pairs.append((va_a, sa, va_b, sb, h1, h2))

    digest = hashlib.sha256(hash_blob).hexdigest()
    require(digest == HASHPAIR_SHA256, f"hashpair sha changed: {digest}")

    OUT_BIN.parent.mkdir(parents=True, exist_ok=True)
    with OUT_BIN.open("wb") as f:
        f.write(b"I275")
        f.write(struct.pack("<II", 1, COUNT))
        for _, sa, _, sb, _, _ in pairs:
            f.write(struct.pack("<I", len(sa)))
            f.write(sa)
            f.write(struct.pack("<I", len(sb)))
            f.write(sb)

    meta = {
        "schema": "inputtable-551140-names-v1",
        "elf_sha256": ELF_SHA256,
        "count": COUNT,
        "hashpair_sha256": HASHPAIR_SHA256,
        "fixture_sha256": hashlib.sha256(OUT_BIN.read_bytes()).hexdigest(),
        "entries": [
            {
                "idx": i,
                "rva_a": f"{va_a:#x}",
                "rva_b": f"{va_b:#x}",
                "len_a": len(sa),
                "len_b": len(sb),
                "hash_a": f"{h1:#010x}",
                "hash_b": f"{h2:#010x}",
                "sha256_a": hashlib.sha256(sa).hexdigest(),
                "sha256_b": hashlib.sha256(sb).hexdigest(),
            }
            for i, (va_a, sa, va_b, sb, h1, h2) in enumerate(pairs)
        ],
    }
    OUT_META.write_text(json.dumps(meta, indent=2) + "\n")
    print(
        f"OK extract_inputtable_551140_names_fixture: {COUNT} pairs; "
        f"hashpair={HASHPAIR_SHA256[:12]}… → {OUT_BIN.relative_to(ROOT)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
