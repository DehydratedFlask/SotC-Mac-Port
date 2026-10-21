#!/usr/bin/env python3
"""Tie native/core/leaf_99350 to original RVA 0x99350 (private; needs local/re/eboot.elf).

The 288-byte body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode_sym.py (exact
encodings, unknown bytes raise). Loads from the read-only segment become constants; the 24-byte
object at the BSS cell 0x2dd9fa8 stays symbolic (BSS is never assumed to hold its file image) and
the lea-derived address 0x2dd9f84 stays an address constant. The reload of the dword stored at
0x2ed37a0 is forwarded from that store. The native --dump declares the object bytes and the
address value it was given; the decoded stores are substituted with them and compared with the
native output (zero-initialised and 0xAA-poisoned, so coverage is checked).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common_sym as c  # noqa: E402

SPEC = dict(
    name='leaf_99350',
    rva=0x99350,
    ctor_index=548,
    body_length=288,
    instructions=38,
    steps=38,
    body_sha256='34917eaaf898d5891b9b843a2514c595bf0631ca3c0a5070e03b8a130644ceed',
    store_count=25,
    stores=['0x2ed37a0:4:09e0f200',
            '0x2ed3760:4:00000000',
            '0x2ed3764:4:00000000',
            '0x2ed376c:1:00',
            '0x2ed376d:1:00',
            '0x2ed376e:1:01',
            '0x2ed376f:1:01',
            '0x2ed3770:1:00',
            '0x2ed3771:1:00',
            '0x2ed3768:4:00000000',
            '0x2ed3680:32:0ac0fa000ac9fa000cc7fa000cc7fa000cc0fa000cc0fa000447920004479200',
            '0x2ed3778:8:A:0x2dd9f84+8',
            '0x2ed36a0:32:04440000044400000247920002479200024400000244000005c4020005c40200',
            '0x2ed3790:8:S:0x2dd9fb8+8',
            '0x2ed36c0:32:04472000044720000447200004472000024720000247200023c0fa0023c9fa00',
            '0x2ed36e0:32:24c0fa0024c9fa0025c0fa0025c9fa00264020002641200027c0220027c12200',
            '0x2ed3700:32:28c03a0028c03a000ec7fa000ec7fa0001c0240001c0240009c0fa0009c0fa00',
            '0x2ed3720:32:05c7220005c7220006c73a0006c73a0001440000014400000bc722000bc72200',
            '0x2ed3740:4:0bc40200',
            '0x2ed3744:4:0bc40200',
            '0x2ed3748:4:09e0f200',
            '0x2ed374c:4:09e0f200',
            '0x2ed3750:16:03c0220003c022000ec4fa000ec4fa00',
            '0x2ed3780:16:S:0x2dd9fa8+16',
            '0x2ed3798:1:00'],
    stores_sha256='acf0d30afe0b072e121b01bc0b7e67aea1e4bfad53aa9e5c20b0104fbfec1534',
    sources=[dict(addr=0xf9f380, size=32, kind='const'),
             dict(addr=0x2ed37a0, size=4, kind='const'),
             dict(addr=0xf9f3a0, size=32, kind='const'),
             dict(addr=0x2dd9fb8, size=8, kind='runtime'),
             dict(addr=0xf9f3c0, size=32, kind='const'),
             dict(addr=0xf9f3e0, size=32, kind='const'),
             dict(addr=0xf9f400, size=32, kind='const'),
             dict(addr=0xf9f420, size=32, kind='const'),
             dict(addr=0xf9f440, size=16, kind='const'),
             dict(addr=0x2dd9fa8, size=16, kind='runtime')],
    region_base=0x2ed3680,
    region_length=292,
    covered_bytes=279,
)

if __name__ == '__main__':
    c.run(SPEC)
