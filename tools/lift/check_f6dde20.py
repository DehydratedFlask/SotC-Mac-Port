#!/usr/bin/env python3
"""Tie native/core/hashed_name_ctors (init_ctor_6dde20) to original RVA 0x6dde20 (private; needs local/re/eboot.elf).

Decoded with tools/lift/hash_helper_decode.py (exact encodings; the only call is to 0x52a990); the
hashes are obtained by interpreting the pinned machine code of 0x52a990/0x161fb0 on the original
strings and compared with the native ctor run on caller-supplied copies. See hash_ctor_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_ctor_check as c  # noqa: E402

SPEC = dict(
    name='init_ctor_6dde20',
    rva=0x6dde20,
    ctor_index=378,
    body_length=81,
    instructions=16,
    body_sha256='d0eefca38d5bd1450c961236ad2452ee4d7af73a1a59d28a660b1107c7ff4f6f',
    helper_sha256='0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0',
    region_base=0x3240c68,
    # (name string address, name cell, hash cell)
    records=[(0x12764b5, 0x3240c68, 0x3240c70), (0x12764c2, 0x3240c78, 0x3240c80), (0x12764d3, 0x3240c88, 0x3240c90)],
    hashes=[0xbeb62505, 0x559f2211, 0xff2b538c],
)

if __name__ == '__main__':
    c.run(SPEC)
