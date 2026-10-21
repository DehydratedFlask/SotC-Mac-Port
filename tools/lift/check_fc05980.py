#!/usr/bin/env python3
"""Tie native/core/leaf_c05980 to original RVA 0xc05980 (private; needs local/re/eboot.elf).

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
    name='leaf_c05980',
    rva=0xc05980,
    ctor_index=237,
    body_length=31,
    instructions=5,
    body_sha256='9d57b73315a8efd309df9d67d4407eafbd31ed5898796476e4919cd7b8263b9f',
    region_base=0x3351290,
    region='ffffffffffffffffffffffff000000000404',
    stores=[dict(dest=0x33512a0, width=1, bytes='04'),
        dict(dest=0x33512a1, width=1, bytes='04'),
        dict(dest=0x3351290, width=16, bytes='ffffffffffffffffffffffff00000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
