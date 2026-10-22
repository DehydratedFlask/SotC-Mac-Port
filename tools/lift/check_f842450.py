#!/usr/bin/env python3
"""Tie native/core/hashed_name_ctors (init_ctor_842450) to original RVA 0x842450 (private; needs local/re/eboot.elf).

Decoded with tools/lift/hash_helper_decode.py (exact encodings; the only call is to 0x52a990); the
hashes are obtained by interpreting the pinned machine code of 0x52a990/0x161fb0 on the original
strings and compared with the native ctor run on caller-supplied copies. See hash_ctor_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_ctor_check as c  # noqa: E402

SPEC = dict(
    name='init_ctor_842450',
    rva=0x842450,
    ctor_index=323,
    body_length=88,
    instructions=23,
    body_sha256='3793f770ca9073808e632513bd7990a613ae676a132dbd85222719b808a274a1',
    helper_sha256='0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0',
    region_base=0x32936c8,
    # (name string address, name cell, hash cell)
    records=[(0x128e808, 0x32936c8, 0x32936d0), (0x128e814, 0x32936d8, 0x32936e0), (0x128e824, 0x32936e8, 0x32936f0)],
    hashes=[0x9744358d, 0x019b017a, 0x49104024],
)

if __name__ == '__main__':
    c.run(SPEC)
