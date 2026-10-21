#!/usr/bin/env python3
"""Tie native/core/leaf_163a40 to original RVA 0x163a40 (private; needs local/re/eboot.elf).

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
    name='leaf_163a40',
    rva=0x163a40,
    ctor_index=526,
    body_length=129,
    instructions=16,
    body_sha256='b29415f293ad46ccbae776e5bd31da7e4a556c1d2cc52dc1bf55336186ebd315',
    region_base=0x2ee3c60,
    region='0000000000000000000000000000000000000000000000000000000000000000000000bf000000bf000000bf0000003f0000003f0000003f0000000000000000ffff7f7fffff7f7fffff7f7fffff7fffffff7fffffff7fff0000000000000000ffff7fffffff7fffffff7fffffff7f7fffff7f7fffff7f7f',
    stores=[dict(dest=0x2ee3c60, width=16, bytes='00000000000000000000000000000000'),
        dict(dest=0x2ee3c70, width=8, bytes='0000000000000000'),
        dict(dest=0x2ee3c80, width=16, bytes='000000bf000000bf000000bf0000003f'),
        dict(dest=0x2ee3c90, width=4, bytes='0000003f'),
        dict(dest=0x2ee3ca0, width=16, bytes='ffff7f7fffff7f7fffff7f7fffff7fff'),
        dict(dest=0x2ee3cb0, width=4, bytes='ffff7fff'),
        dict(dest=0x2ee3c94, width=4, bytes='0000003f'),
        dict(dest=0x2ee3cb4, width=4, bytes='ffff7fff'),
        dict(dest=0x2ee3cc0, width=16, bytes='ffff7fffffff7fffffff7fffffff7f7f'),
        dict(dest=0x2ee3cd0, width=8, bytes='ffff7f7fffff7f7f')],
)

if __name__ == '__main__':
    c.run(SPEC)
