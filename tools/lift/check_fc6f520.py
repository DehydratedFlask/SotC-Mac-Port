#!/usr/bin/env python3
"""Tie native/core/leaf_c6f520 to original RVA 0xc6f520 (private; needs local/re/eboot.elf).

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
    name='leaf_c6f520',
    rva=0xc6f520,
    ctor_index=187,
    body_length=66,
    instructions=9,
    body_sha256='e0c40c7aa8633df5cc76ae0740bbd2454f2b17225541ea841f86fdd7628c99f7',
    region_base=0x3351930,
    region='ffffffffffffffffffffffff000000000000803f0000000000000000000000000000803f0000000000000000000000000000803f000080bf000000000000000000000000000080bf000000000000000000000000000080bf',
    stores=[dict(dest=0x3351940, width=32, bytes='0000803f0000000000000000000000000000803f000000000000000000000000'),
        dict(dest=0x3351960, width=32, bytes='0000803f000080bf000000000000000000000000000080bf0000000000000000'),
        dict(dest=0x3351930, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351980, width=8, bytes='00000000000080bf')],
)

if __name__ == '__main__':
    c.run(SPEC)
