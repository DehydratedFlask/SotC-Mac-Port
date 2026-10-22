#!/usr/bin/env python3
"""Tie native/core/leaf_7ce8d0 to original RVA 0x7ce8d0 (private; needs local/re/eboot.elf).

The 41-byte body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode_atomic.py (exact
encodings, unknown bytes raise): movabs, xor r32,r32, plain qword stores and two locked
`xchg [cell],ecx` stores of 0 (the old values are read as run-time sources and discarded). The
native --dump bytes (zero-initialised and 0xAA-poisoned) and its declared atomic cell offsets are
compared with the decode.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common_atomic as c  # noqa: E402

SPEC = dict(
    name='leaf_7ce8d0',
    rva=0x7ce8d0,
    ctor_index=346,
    body_length=41,
    instructions=8,
    steps=8,
    body_sha256='680cb68a4709673b45f1da3f2ffc618957722c69c3fb99ea20c88f66ea5429ae',
    store_count=4,
    stores=['0x325e470:8:010000000000803f',
            '0x325e480:4:00000000',
            '0x325e490:8:010000000000803f',
            '0x325e4a0:4:00000000'],
    stores_sha256='f7344656fce9233c85375576bc8651bb256c698732ada1fce4e89d426b5a3b40',
    sources=[dict(addr=0x325e480, size=4, kind='runtime'),
             dict(addr=0x325e4a0, size=4, kind='runtime')],
    region_base=0x325e470,
    region_length=52,
    covered_bytes=24,
    atomic_dests=[0x325e480, 0x325e4a0],
)

if __name__ == '__main__':
    c.run(SPEC)
