#!/usr/bin/env python3
"""Tie native/core/leaf_83d800 to original RVA 0x83d800 (private; needs local/re/eboot.elf).

The body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode.py, which matches each
instruction by exact encoding and raises on anything unknown. The body SHA-256, length, ordered
store list and region image are pinned, and the native --dump bytes (zero-initialised and
0xAA-poisoned, so coverage is checked) are compared with the decoded store image.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as c  # noqa: E402

SPEC = dict(
    name='leaf_83d800',
    rva=0x83d800,
    ctor_index=324,
    body_length=61,
    instructions=7,
    body_sha256='96ae949df94916300b5a8fd9364c70a1eba0acd14035dac5a98998d80f44f235',
    region_base=0x3293400,
    region='00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ffffffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ffffffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ffffffff',
    stores=[dict(dest=0x3293400, width=4, bytes='00000000'),
        dict(dest=0x3293458, width=4, bytes='ffffffff'),
        dict(dest=0x3293460, width=4, bytes='00000000'),
        dict(dest=0x32934b8, width=4, bytes='ffffffff'),
        dict(dest=0x32934c0, width=4, bytes='00000000'),
        dict(dest=0x3293518, width=4, bytes='ffffffff')],
)

if __name__ == '__main__':
    c.run(SPEC)
