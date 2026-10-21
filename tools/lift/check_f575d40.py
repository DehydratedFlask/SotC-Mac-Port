#!/usr/bin/env python3
"""Tie native/core/leaf_575d40 to original RVA 0x575d40 (private; needs local/re/eboot.elf).

The 29-byte body (a 20-byte copy from the run-time BSS cell 0x2eea480 to 0x2f21520) is decoded from
the pinned ELF with tools/lift/leaf_ctor_decode_sym.py, which matches each instruction by exact
encoding and keeps the source bytes symbolic: BSS is never assumed to hold its file image. The
native --dump declares the source bytes it was given; the decoded stores are substituted with them
and compared with the native output (zero-initialised and 0xAA-poisoned, so coverage is checked).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common_sym as c  # noqa: E402

SPEC = dict(
    name='leaf_575d40',
    rva=0x575d40,
    ctor_index=417,
    body_length=29,
    instructions=7,
    steps=7,
    body_sha256='e27a7ad0e4f126eab845e75cd66af0330610cded26e2fb231e6470d3f94db96d',
    store_count=2,
    stores=['0x2f21530:4:S:0x2eea490+4',
            '0x2f21520:16:S:0x2eea480+16'],
    stores_sha256='56f14eda7b2502f030fb2bb7cfcd45187b7684ffccb04db80d5e6629916233b0',
    sources=[dict(addr=0x2eea490, size=4, kind='runtime'),
             dict(addr=0x2eea480, size=16, kind='runtime')],
    region_base=0x2f21520,
    region_length=20,
    covered_bytes=20,
)

if __name__ == '__main__':
    c.run(SPEC)
