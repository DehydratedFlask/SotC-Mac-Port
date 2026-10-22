#!/usr/bin/env python3
"""Tie native/core/leaf_e71990 to original RVA 0xe71990 (private; needs local/re/eboot.elf).

The 337-byte body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode_sym.py (exact
encodings, unknown bytes raise). The 14 `lea` addresses stored at +0x00 of each record stay
address constants (A: tokens, one per record); the 14 hash immediates are constants. The native
--dump declares the 14 address values it was given; the decoded stores are substituted with them
and compared with the native output (zero-initialised and 0xAA-poisoned, so coverage is checked).
The checker also asserts that the 13 non-zero hash immediates are exactly sotc::name_hash values
of the original strings read from the pinned ELF at the same 13 addresses (so the strings are used
only for this private offline comparison, never embedded) and that the 14th is the literal 0.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common_sym as c  # noqa: E402
import leaf_ctor_decode_sym as Y  # noqa: E402

STORES = ['0x3353f40:8:A:0x131c990+8', '0x3353f48:4:4980e5f1', '0x3353f50:8:A:0x131c9a2+8', '0x3353f58:4:23a37d25',
          '0x3353f60:8:A:0x131c9b2+8', '0x3353f68:4:13c4a247', '0x3353f70:8:A:0x131c9c3+8', '0x3353f78:4:e86a7eed',
          '0x3353f80:8:A:0x131c9d5+8', '0x3353f88:4:cd7dd4b2', '0x3353f90:8:A:0x131c9e4+8', '0x3353f98:4:1cf54149',
          '0x3353fa0:8:A:0x131c9f7+8', '0x3353fa8:4:1df54149', '0x3353fb0:8:A:0x131ca0a+8', '0x3353fb8:4:1ef54149',
          '0x3353fc0:8:A:0x131ca1d+8', '0x3353fc8:4:1ff54149', '0x3353fd0:8:A:0x131ca30+8', '0x3353fd8:4:5772ef10',
          '0x3353fe0:8:A:0x131ca44+8', '0x3353fe8:4:c959ae7c', '0x3353ff0:8:A:0x131ca59+8', '0x3353ff8:4:24bb0b33',
          '0x3354000:8:A:0x131ca78+8', '0x3354008:4:e788142b', '0x3354010:8:A:0x131ca94+8', '0x3354018:4:00000000']

SPEC = dict(
    name='leaf_e71990',
    rva=0xe71990,
    ctor_index=23,
    body_length=337,
    instructions=43,
    steps=43,
    body_sha256='3932fb85b759ae53dd1675142ffda4b449be725f241cfeefdb40a07a2beaf774',
    store_count=28,
    stores=STORES,
    stores_sha256='31aef6d958281c1be79cdd1e9d0c4180afbe6b3d08e97a5113180375bafcfa71',
    sources=[],
    region_base=0x3353f40,
    region_length=220,
    covered_bytes=168,
)


def djb2_xor(data):
    """name_hash of agentB's checkpoint: h = 5381; h = (h*33) ^ sign_extend32(c); 0 -> 1."""
    h = 5381
    for b in data:
        h = ((h * 33) ^ (b - 256 if b >= 128 else b)) & 0xFFFFFFFF
    return h or 1


def check_hash_relationship():
    elf, image = Y.load_image()
    d = Y.decode(image, SPEC['rva'])
    stores = d['stores']
    names = [int(s['bytes'][0].split(':')[1], 16) for s in stores[0::2]]
    hashes = [int.from_bytes(bytes(s['bytes']), 'little') for s in stores[1::2]]
    c.require(len(names) == len(hashes) == 14, 'expected 14 {name address, hash} records')
    for k in range(13):
        text = bytearray()
        a = names[k]
        while True:
            b = image.read(a, 1)[0]
            if b == 0:
                break
            text.append(b)
            a += 1
        c.require(djb2_xor(text) == hashes[k], f'record {k}: hash immediate is not name_hash(string)')
    c.require(hashes[13] == 0, 'sentinel hash is not the literal 0')
    print('OK leaf_e71990: 13/13 hash immediates equal name_hash(original string); record 13 is the literal 0')


if __name__ == '__main__':
    c.run(SPEC)
    check_hash_relationship()
