#!/usr/bin/env python3
"""Tie native/core/leaf_3ca7c0 to original RVA 0x3ca7c0 (private; needs local/re/eboot.elf).

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
    name='leaf_3ca7c0',
    rva=0x3ca7c0,
    ctor_index=466,
    body_length=38,
    instructions=8,
    body_sha256='8a815a84cff48624b863bded16a4e50d221bbd915387025a054e3cb29e3e26a9',
    region_base=0x2ef6f88,
    region='000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000',
    stores=[dict(dest=0x2ef6f88, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000'),
        dict(dest=0x2ef6fa8, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000'),
        dict(dest=0x2ef6fc8, width=32, bytes='0000000000000000000000000000000000000000000000000000000000000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
