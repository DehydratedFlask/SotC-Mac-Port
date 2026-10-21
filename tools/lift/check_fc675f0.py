#!/usr/bin/env python3
"""Tie native/core/leaf_c675f0 to original RVA 0xc675f0 (private; needs local/re/eboot.elf).

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
    name='leaf_c675f0',
    rva=0xc675f0,
    ctor_index=193,
    body_length=115,
    instructions=19,
    body_sha256='b953cd69ec1629261841b2855e8fcf376838bbcb7ce0a370aece02eda81c4c88',
    region_base=0x3351760,
    region='ffffffffffffffffffffffff0000000000000000000000000000000000000000ffffffff00000000000000000000000000000000ffffffff0000000000000000ffffffffffffffff00000000000000000000000000000000ffffffff00000000ffffffff00000000ffffffff0000000000000000ffffffffffffffff00000000ffffffffffffffffffffffff00000000',
    stores=[dict(dest=0x3351770, width=16, bytes='00000000000000000000000000000000'),
        dict(dest=0x3351760, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351780, width=16, bytes='ffffffff000000000000000000000000'),
        dict(dest=0x3351790, width=16, bytes='00000000ffffffff0000000000000000'),
        dict(dest=0x33517a0, width=16, bytes='ffffffffffffffff0000000000000000'),
        dict(dest=0x33517b0, width=16, bytes='0000000000000000ffffffff00000000'),
        dict(dest=0x33517c0, width=16, bytes='ffffffff00000000ffffffff00000000'),
        dict(dest=0x33517d0, width=16, bytes='00000000ffffffffffffffff00000000'),
        dict(dest=0x33517e0, width=16, bytes='ffffffffffffffffffffffff00000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
