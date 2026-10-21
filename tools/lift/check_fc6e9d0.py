#!/usr/bin/env python3
"""Tie native/core/leaf_c6e9d0 to original RVA 0xc6e9d0 (private; needs local/re/eboot.elf).

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
    name='leaf_c6e9d0',
    rva=0xc6e9d0,
    ctor_index=188,
    body_length=149,
    instructions=19,
    body_sha256='eef704640113df03738665ef3515eb38f3422a4cdf7be26426bf3be1daa05949',
    region_base=0x3351830,
    region='ffffffffffffffffffffffff000000000000803f0000000000000000000000000000803f0000000000000000000000000000803f000080bf000000000000000000000000000080bf000000000000000000000000000080bf00000000000000000000000000000000000000000000000000000000f30435bff30435bff304353f00000000f30435bf00000000f304353ff30435bff30435bf00000000f30435bf00000000f304353ff304353ff304353f00000000f304353f00000000f30435bff304353ff30435bf00000000f304353ff304353ff30435bf00000000f304353ff304353f00000000f30435bff304353f00000000f30435bff30435bf00000000',
    stores=[dict(dest=0x3351840, width=32, bytes='0000803f0000000000000000000000000000803f000000000000000000000000'),
        dict(dest=0x3351860, width=32, bytes='0000803f000080bf000000000000000000000000000080bf0000000000000000'),
        dict(dest=0x3351880, width=4, bytes='00000000'),
        dict(dest=0x3351830, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351884, width=4, bytes='000080bf'),
        dict(dest=0x33518a0, width=32, bytes='00000000f30435bff30435bff304353f00000000f30435bf00000000f304353f'),
        dict(dest=0x33518c0, width=32, bytes='f30435bff30435bf00000000f30435bf00000000f304353ff304353ff304353f'),
        dict(dest=0x33518e0, width=32, bytes='00000000f304353f00000000f30435bff304353ff30435bf00000000f304353f'),
        dict(dest=0x3351900, width=32, bytes='f304353ff30435bf00000000f304353ff304353f00000000f30435bff304353f'),
        dict(dest=0x3351920, width=16, bytes='00000000f30435bff30435bf00000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
