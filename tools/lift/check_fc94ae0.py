#!/usr/bin/env python3
"""Tie native/core/leaf_c94ae0 to original RVA 0xc94ae0 (private; needs local/re/eboot.elf).

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
    name='leaf_c94ae0',
    rva=0xc94ae0,
    ctor_index=176,
    body_length=93,
    instructions=13,
    body_sha256='2c88e2ef1bce1b6ab1c61c657c9bdca4a669f1274415f9ec1964036832caebae',
    region_base=0x3351a40,
    region='ffffffffffffffffffffffff00000000000000800000008000000080000000805f7089305f7089305f7089305f708930000000000000000000000000000000000000004000000040000000400000004095bfd63395bfd63395bfd63395bfd633',
    stores=[dict(dest=0x3351a40, width=16, bytes='ffffffffffffffffffffffff00000000'),
        dict(dest=0x3351a80, width=16, bytes='00000040000000400000004000000040'),
        dict(dest=0x3351a90, width=16, bytes='95bfd63395bfd63395bfd63395bfd633'),
        dict(dest=0x3351a50, width=16, bytes='00000080000000800000008000000080'),
        dict(dest=0x3351a60, width=16, bytes='5f7089305f7089305f7089305f708930'),
        dict(dest=0x3351a70, width=16, bytes='00000000000000000000000000000000')],
)

if __name__ == '__main__':
    c.run(SPEC)
