#!/usr/bin/env python3
"""Tie native/core/leaf_ca18d0 to original RVA 0xca18d0 (private; needs local/re/eboot.elf).

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
    name='leaf_ca18d0',
    rva=0xca18d0,
    ctor_index=174,
    body_length=93,
    instructions=13,
    body_sha256='e7a350415b4fb1ebcdab7a12409bfd4bd357fbe97ef5732887c7bcec86815656',
    region_base=0x3351ab0,
    region='ffffffffffffffffffffffff00000000000000800000008000000080000000805f7089305f7089305f7089305f708930000000000000000000000000000000000000004000000040000000400000004095bfd63395bfd63395bfd63395bfd633',
    stores=[dict(dest=0x3351ab0, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351af0, width=16, bytes='00000040000000400000004000000040'),
        dict(dest=0x3351b00, width=16, bytes='95bfd63395bfd63395bfd63395bfd633'),
        dict(dest=0x3351ac0, width=16, bytes='00000080000000800000008000000080'),
        dict(dest=0x3351ad0, width=16, bytes='5f7089305f7089305f7089305f708930'),
        dict(dest=0x3351ae0, width=16, bytes='00000000000000000000000000000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
