#!/usr/bin/env python3
"""Tie native/core/hashed_name_ctors (init_ctor_7dace0) to original RVA 0x7dace0 (private; needs local/re/eboot.elf).

Decoded with tools/lift/hash_helper_decode.py (exact encodings; the only call is to 0x52a990); the
hashes are obtained by interpreting the pinned machine code of 0x52a990/0x161fb0 on the original
strings and compared with the native ctor run on caller-supplied copies. See hash_ctor_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_ctor_check as c  # noqa: E402

SPEC = dict(
    name='init_ctor_7dace0',
    rva=0x7dace0,
    ctor_index=341,
    body_length=63,
    instructions=18,
    body_sha256='b7bc2076210eb545aa9faf949ffd169dcb317e4b0876e89cd9137977bc3116de',
    helper_sha256='0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0',
    region_base=0x325efb0,
    # (name string address, name cell, hash cell)
    records=[(0x1289a20, 0x325efb0, 0x325efb8), (0x1289a2e, 0x325efc0, 0x325efc8)],
    hashes=[0xc30b4677, 0x472539c3],
)

if __name__ == '__main__':
    c.run(SPEC)
