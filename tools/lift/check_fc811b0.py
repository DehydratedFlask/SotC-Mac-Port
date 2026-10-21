#!/usr/bin/env python3
"""Tie native/core/leaf_c811b0 to original RVA 0xc811b0 (private; needs local/re/eboot.elf).

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
    name='leaf_c811b0',
    rva=0xc811b0,
    ctor_index=179,
    body_length=33,
    instructions=5,
    body_sha256='e19a7032d03f1bfbe22127c38b426b663472834d7f074790c2d5fd879267056b',
    region_base=0x3351a00,
    region='ffffffffffffffffffffffff0000000000000080000000800000008000000080',
    stores=[dict(dest=0x3351a00, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351a10, width=16, bytes='00000080000000800000008000000080')],
)

if __name__ == '__main__':
    c.run(SPEC)
