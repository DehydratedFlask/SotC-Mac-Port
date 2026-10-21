#!/usr/bin/env python3
"""Tie native/core/leaf_8f3d90 to original RVA 0x8f3d90 (private; needs local/re/eboot.elf).

The body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode.py, which matches each
instruction by exact encoding and raises on anything unknown. The body SHA-256, length, ordered
store list and region image are pinned, and the native --dump bytes (zero-initialised and
0xAA-poisoned, so coverage is checked too) are compared with the decoded store image.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as c  # noqa: E402

SPEC = dict(
    name='leaf_8f3d90',
    rva=0x8f3d90,
    ctor_index=320,
    body_length=257,
    instructions=33,
    body_sha256='5de758451dcb2ef7073cca4378dc74f9936f4f89fc450e3ada146e26bbe1d755',
    region_base=0x3294f90,
    region='0001ffff0203ffff0405ffff0607ffff0c0d0e0f0001ffff0203ffff0405ffff0001ffff0001ffff0203ffff0405ffff0c0d0e0f0c0d0e0f0001ffff0203ffff0001ffff0203ffff0203ffff0405ffff0c0d0e0f0001ffff0001ffff0203ffff0001ffff0001ffff0001ffff0203ffff0c0d0e0f0c0d0e0f0c0d0e0f0001ffff0001ffff0203ffff0405ffff0405ffff0c0d0e0f0001ffff0203ffff0203ffff0001ffff0001ffff0203ffff0203ffff0c0d0e0f0c0d0e0f0001ffff0001ffff0001ffff0203ffff0203ffff0203ffff0c0d0e0f0001ffff0001ffff0001ffff0001ffff0001ffff0001ffff0001ffff0c0d0e0f0c0d0e0f0c0d0e0f0c0d0e0f',
    stores=[dict(dest=0x3294f90, width=16, bytes='0001ffff0203ffff0405ffff0607ffff'),
        dict(dest=0x3294fa0, width=16, bytes='0c0d0e0f0001ffff0203ffff0405ffff'),
        dict(dest=0x3294fb0, width=16, bytes='0001ffff0001ffff0203ffff0405ffff'),
        dict(dest=0x3294fc0, width=16, bytes='0c0d0e0f0c0d0e0f0001ffff0203ffff'),
        dict(dest=0x3294fd0, width=16, bytes='0001ffff0203ffff0203ffff0405ffff'),
        dict(dest=0x3294fe0, width=16, bytes='0c0d0e0f0001ffff0001ffff0203ffff'),
        dict(dest=0x3294ff0, width=16, bytes='0001ffff0001ffff0001ffff0203ffff'),
        dict(dest=0x3295000, width=16, bytes='0c0d0e0f0c0d0e0f0c0d0e0f0001ffff'),
        dict(dest=0x3295010, width=16, bytes='0001ffff0203ffff0405ffff0405ffff'),
        dict(dest=0x3295020, width=16, bytes='0c0d0e0f0001ffff0203ffff0203ffff'),
        dict(dest=0x3295030, width=16, bytes='0001ffff0001ffff0203ffff0203ffff'),
        dict(dest=0x3295040, width=16, bytes='0c0d0e0f0c0d0e0f0001ffff0001ffff'),
        dict(dest=0x3295050, width=16, bytes='0001ffff0203ffff0203ffff0203ffff'),
        dict(dest=0x3295060, width=16, bytes='0c0d0e0f0001ffff0001ffff0001ffff'),
        dict(dest=0x3295070, width=16, bytes='0001ffff0001ffff0001ffff0001ffff'),
        dict(dest=0x3295080, width=16, bytes='0c0d0e0f0c0d0e0f0c0d0e0f0c0d0e0f')],
)

if __name__ == '__main__':
    c.run(SPEC)
