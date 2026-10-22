#!/usr/bin/env python3
"""Tie native/core/hashed_name_ctors (init_ctor_669450) to original RVA 0x669450 (private; needs local/re/eboot.elf).

Decoded with tools/lift/hash_helper_decode.py (exact encodings; the only call is to 0x52a990); the
hashes are obtained by interpreting the pinned machine code of 0x52a990/0x161fb0 on the original
strings and compared with the native ctor run on caller-supplied copies. See hash_ctor_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_ctor_check as c  # noqa: E402

SPEC = dict(
    name='init_ctor_669450',
    rva=0x669450,
    ctor_index=394,
    body_length=38,
    instructions=13,
    body_sha256='617951d5ca3f591a08acc34527a1a1eba06f02a7ef0a2a9f8d3e0a5cf6314996',
    helper_sha256='0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0',
    region_base=0x3012638,
    # (name string address, name cell, hash cell)
    records=[(0x126fdb0, 0x3012638, 0x3012640)],
    hashes=[0xbedd0298],
)

if __name__ == '__main__':
    c.run(SPEC)
