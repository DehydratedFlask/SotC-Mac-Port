#!/usr/bin/env python3
"""Prove OBS carve-fit math bytes inside 0x1a800 (private ELF).

Checks (byte-exact aside from branch displacements marked dddddddd):
  * Page budget: shr r13,16; cmp [r15+0x70], r13d; jb fail
  * Align prep: dec ecx; mov rdx,rcx; not rdx
  * Fit: cmp free_size, req; jb next
  * lead = ((start + (align-1)) & mask) - start; je exact_node
  * cmp lead+req, free_size; jbe split else next
  * Callers from 0x19e00: 0x19e72 and 0x19f47
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


# Prologue arg shuffle: rdx→rbx (size), rdi→r15 (arena)
match(0x1a800, '55 48 89 e5 41 57 41 56 41 55 41 54 53 48 83 ec 18'
               '48 89 d3 49 89 ff 31 c0')

# Page budget
match(0x1a819, '49 89 dd 49 c1 ed 10 45 39 6f 70 0f 82 dddddddd')

# Align mask: dec ecx; mov rdx,rcx; not rdx
match(0x1a83d, 'ff c9 48 89 ca 48 f7 d2')

# free_size >= req; load start; compute lead
match(0x1a866, '49 8b 44 24 58'          # mov rax, [r12+0x58] size
               '48 39 d8'                # cmp rax, rbx
               '72 e0'                   # jb next
               '49 8b 7c 24 60'          # mov rdi, [r12+0x60] start
               '4c 8d 34 0f'             # lea r14, [rdi+rcx]  start+(align-1)
               '49 21 d6'                # and r14, rdx
               '49 29 fe'                # sub r14, rdi       lead
               '74 20'                   # je exact_node @0x1a8a1
               '49 8d 3c 1e'             # lea rdi, [r14+rbx] lead+req
               '48 39 c7'                # cmp rdi, rax
               '76 1f')                  # jbe split

# Exact-node path
match(0x1a8a1, '4d 89 e6 e9 dddddddd')   # mov r14,r12; jmp continue

# Callers inside 0x19e00
disps = match(0x19e72, 'e8 dddddddd')
assert 0x19e72 + 5 + disps[0] == 0x1a800, hex(0x19e72 + 5 + disps[0])
disps = match(0x19f47, 'e8 dddddddd')
assert 0x19f47 + 5 + disps[0] == 0x1a800, hex(0x19f47 + 5 + disps[0])

# Non-zero rdi at 0x479c2 is object context (mov rdi,rbx), not preferred VA —
# documents why MAP_FIXED is not adopted from that site.
match(0x479a6, '48 89 fb'                 # mov rbx, rdi
               '31 ff'                     # xor edi, edi
               '80 7b 09 00'               # cmp byte [rbx+9], 0
               '74 4e'                     # je skip (short)
               '48 81 c6 ff ff 00 00'      # add rsi, 0xffff
               '48 89 df'                  # mov rdi, rbx  (= object)
               '48 81 e6 00 00 ff ff')     # and rsi, -0x10000
disps = match(0x479c2, 'e8 dddddddd')
assert 0x479c2 + 5 + disps[0] == 0x19ae0, hex(0x479c2 + 5 + disps[0])

print('OK: 0x1a800 carve-fit — page budget, align mask, lead/fit/exact_node; '
      'callers 0x19e72/0x19f47; 0x479c2 rdi=object context (not MAP_FIXED VA)')
