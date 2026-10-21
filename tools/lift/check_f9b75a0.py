#!/usr/bin/env python3
"""Tie native/core/leaf_9b75a0 to original RVA 0x9b75a0 (private; needs local/re/eboot.elf).

The 50-byte body (a 256-iteration loop zeroing two overlapping 16-byte windows per 0x28-byte
record, then a dword at +0x2800) is simulated from the pinned ELF with
tools/lift/leaf_ctor_decode_sym.py (exact encodings; the trip count comes from the decoded
add/cmp/jne immediates, not from a pin). All 513 decoded stores are pinned by count and SHA-256 of
their ordered list, and the region image and coverage are compared with the native --dump
(zero-initialised and 0xAA-poisoned: the 12-byte gaps between record windows must stay 0xAA).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common_sym as c  # noqa: E402

SPEC = dict(
    name='leaf_9b75a0',
    rva=0x9b75a0,
    ctor_index=319,
    body_length=50,
    instructions=10,
    steps=1285,      # 3 setup + 256 * 5 loop instructions + dword store + ret
    body_sha256='f637b629955c1842a70cdacab00b192f872f10df2dd627b4caaf2ce37a45ec14',
    store_count=513,
    stores_sha256='3b026a71359ccc640890577ffe8039e9410af55665119c0c73389029286a4943',
    sources=[],
    region_base=0x32ef798,
    region_length=0x27fc,
    covered_bytes=256 * 28 + 4,
)

if __name__ == '__main__':
    c.run(SPEC)
