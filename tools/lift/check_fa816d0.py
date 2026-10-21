#!/usr/bin/env python3
"""Tie native/core/leaf_a816d0 to original RVA 0xa816d0 (private; needs local/re/eboot.elf).

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
    name='leaf_a816d0',
    rva=0xa816d0,
    ctor_index=304,
    body_length=47,
    instructions=7,
    body_sha256='79668c067152aa4f426511ea4813d2d5f80af74258249a626be15613bcb2076b',
    region_base=0x32fb1c0,
    region='000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000',
    stores=[dict(dest=0x32fb21c, width=4, bytes='00000000'),
        dict(dest=0x32fb1c9, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000'),
        dict(dest=0x32fb1c0, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000'),
        dict(dest=0x32fb1f9, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000'),
        dict(dest=0x32fb1ec, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
