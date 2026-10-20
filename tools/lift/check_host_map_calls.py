#!/usr/bin/env python3
"""Prove OBS kernel-map call sites that back allocator region 0x19ae0 (private ELF).

Ties (byte-exact aside from call displacements):
  * Grow-pool 0x19875..0x198b1:
      ReserveVirtual(rdi=&slot, rsi=size, edx=0, ecx=0x100000)
      MapFlexible(rdi=slot, rsi=size, edx=0x33, ecx=0x10)
  * Fresh-reserve path 0x19f8a..0x19fb2:
      align = max(ebx, 0x100000); ReserveVirtual(rdi=&local, rsi=r13_size, edx=0, ecx=align)
  * PLT targets: ReserveVirtual→0xf8f5b0, MapFlexible→0xf8f5a0
"""
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)


def match(addr, template):
    toks = re.findall(r'dddddddd|[0-9a-fA-F]{2}', template.replace(' ', ''))
    size = sum(4 if t.lower() == 'dddddddd' else 1 for t in toks)
    code = img.read(addr, size)
    pos = 0
    disps = []
    for t in toks:
        if t.lower() == 'dddddddd':
            disps.append(struct.unpack_from('<i', code, pos)[0])
            pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return disps


match(0x1987c,
      '31 d2'
      'b9 00 00 10 00'
      '4c 89 f6'
      '48 c7 83 08 d9 20 00 00 00 00 00')
disps = match(0x19891, 'e8 dddddddd')
assert 0x19891 + 5 + disps[0] == 0xf8f5b0, hex(0x19891 + 5 + disps[0])

match(0x1989d,
      'ba 33 00 00 00'
      'b9 10 00 00 00'
      '4c 89 f6'
      '48 81 c7 08 d9 20 00')
disps = match(0x198b1, 'e8 dddddddd')
assert 0x198b1 + 5 + disps[0] == 0xf8f5a0, hex(0x198b1 + 5 + disps[0])

match(0x19f8a,
      '81 fb 00 00 10 00'
      'b8 00 00 10 00'
      '48 8d 7d c8'
      '4c 89 ee')
match(0x19fa9,
      '0f 47 c3'
      '31 d2'
      '31 db'
      '89 c1')
disps = match(0x19fb2, 'e8 dddddddd')
assert 0x19fb2 + 5 + disps[0] == 0xf8f5b0, hex(0x19fb2 + 5 + disps[0])

match(0x19fd9, '4c 89 68 58  48 89 48 60  c6 40 69 01')

print('OK: host_map call sites — ReserveVirtual@0x19891/0x19fb2 → PLT 0xf8f5b0; '
      'MapFlexible@0x198b1 → PLT 0xf8f5a0; align≥1MiB; R[+0x58]/[+0x60] writer')
