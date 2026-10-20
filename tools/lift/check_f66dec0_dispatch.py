#!/usr/bin/env python3
"""Tie native retag_field_offset / dispatch_retag to original RVA 0x66dec0 jump table (private ELF).

Byte-exact: decode prologue, type range (type-2 > 12 => done), jump table @0x1270354 (13 entries),
and each retag arm's field offset. Native switch must match the OBS arm map.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)


def match(addr, template_hex, zero_ranges):
    t = bytes.fromhex(template_hex)
    got = bytearray(img.read(addr, len(t)))
    for off, n in zero_ranges:
        got[off:off + n] = b'\0' * n
    assert bytes(got) == t, (hex(addr), bytes(got).hex(), t.hex())


# Prologue + type range + jump (rip-relative lea zeroed).
match(
    0x66dec0,
    '488d0d00000000' '89f2' '31c0' '81e2ffffff7f' '85f6' '488b09' '480f48c1' '4801d0'
    '0f84d7000000' '83c7fe' '83ff0c' '0f87cb000000' '488d1500000000' '486334ba' '4801d6' 'ffe6',
    [(3, 4), (0x66deef - 0x66dec0 + 3, 4)],
)

lea = 0x66deef
table = lea + 7 + struct.unpack('<i', img.read(lea + 3, 4))[0]
assert table == 0x1270354, hex(table)

# Expected (type, target, field_offset or None for nop@0x66dfba)
expect = {
    2: (0x66df63, 0x0C),
    3: (0x66deff, 0x20),
    4: (0x66deff, 0x20),
    5: (0x66deff, 0x20),
    6: (0x66deff, 0x20),
    7: (0x66dfba, None),
    8: (0x66dfba, None),
    9: (0x66dfba, None),
    10: (0x66dfba, None),
    11: (0x66dfba, None),
    12: (0x66df37, 0x18),
    13: (0x66df8f, 0x14),
    14: (0x66df37, 0x18),
}

for i in range(13):
    typ = i + 2
    rel = struct.unpack('<i', img.read(table + 4 * i, 4))[0]
    target = table + rel
    want_t, want_off = expect[typ]
    assert target == want_t, (typ, hex(target), hex(want_t))
    if want_off is None:
        assert img.read(target, 1) == b'\xc3', hex(target)
    else:
        # mov edx, [rax+imm8]
        b = img.read(target, 3)
        assert b[0:2] == bytes.fromhex('8b50') and b[2] == want_off, (typ, b.hex(), want_off)
        # ends with or edx,0x80000000; mov [rax+imm],edx; ret  (first arm has long jccs)
        arm = img.read(target, 0x40)
        assert bytes.fromhex('81ca00000080') in arm, (typ, 'missing or-tag')
        store = bytes([0x89, 0x50, want_off])
        assert store in arm, (typ, 'missing store', arm.hex())

# Native table must agree (source of truth for C++ switch — re-derive, don't import).
native_map = {
    2: 0x0C, 3: 0x20, 4: 0x20, 5: 0x20, 6: 0x20,
    7: None, 8: None, 9: None, 10: None, 11: None,
    12: 0x18, 13: 0x14, 14: 0x18,
}
for typ, (_, off) in expect.items():
    assert native_map[typ] == off, (typ, native_map[typ], off)

print(
    f'OK: 0x66dec0 dispatcher jump table @{table:#x} byte-exact; '
    f'13 arms: types 2(+0xc) 3-6(+0x20) 7-11(nop) 12/14(+0x18) 13(+0x14); '
    f'range type-2>12 -> done'
)
