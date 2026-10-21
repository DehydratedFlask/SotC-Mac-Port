#!/usr/bin/env python3
"""Tie native/core/leaf_c0d380 to original RVA 0xc0d380 (private; needs local/re/eboot.elf).

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
    name='leaf_c0d380',
    rva=0xc0d380,
    ctor_index=229,
    body_length=92,
    instructions=13,
    body_sha256='736e9487ca2d1ade70ec22ae9bc0a0e799df8cb7ae5aec200ecf40fb9f03ebc7',
    region_base=0x3351320,
    region='ffffffffffffffffffffffff00000000200000000000000020000000000000001800000000000000180000000000000020000000000000001800000000000000000000000000000000000000000000000000000000000000200000000000000020000000000000002000000000000000000000000000000018000000000000000000000000000000000000000000000000000000000000001800000000000000',
    stores=[dict(dest=0x3351350, width=8, bytes='2000000000000000'),
        dict(dest=0x3351388, width=8, bytes='2000000000000000'),
        dict(dest=0x3351378, width=16, bytes='20000000000000002000000000000000'),
        dict(dest=0x3351330, width=32, bytes='2000000000000000200000000000000018000000000000001800000000000000'),
        dict(dest=0x3351398, width=8, bytes='1800000000000000'),
        dict(dest=0x3351358, width=8, bytes='1800000000000000'),
        dict(dest=0x3351320, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x33513b8, width=8, bytes='1800000000000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
