#!/usr/bin/env python3
"""Prove the 0x19ae0 allocator-region API contract against the ELF (private; needs local/re/eboot.elf).

Checks (byte-exact aside from rip-relative / call displacements marked dddddddd):
  * Prologue saves edx→[rbp-0x40], ecx→ebx, rsi→r15, rdi→r12
  * Forward into 0x19e00: rdi=r12, rsi=0, rdx=0, rcx=r15, r8=0, r9=[rbp-0x40], [rsp]=ebx
  * Call site 0x55fe2c: edi=0, esi=0x6400000, edx=0x20000000, ecx=1, then call 0x19ae0
  * After return: reader uses [rax+0x60] start and [rax+0x58] size
  * Inner writer 0x19fd9: mov [rax+0x58], r13; mov [rax+0x60], rcx
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


# Prologue: save args
match(0x19ae0,
      '55 48 89 e5 41 57 41 56 41 55 41 54 53 48 83 ec 28'
      '48 8b 05 dddddddd'          # mov rax, [rip+tls]
      '89 55 c0'                    # mov [rbp-0x40], edx
      '89 cb'                       # mov ebx, ecx
      '49 89 f7'                    # mov r15, rsi
      '49 89 fc')                   # mov r12, rdi

# Forward to 0x19e00
disps = match(0x19b5d,
              '44 8b 4d c0'         # mov r9d, [rbp-0x40]  (= original edx)
              '31 f6'               # xor esi, esi
              '31 d2'               # xor edx, edx
              '45 31 c0'            # xor r8d, r8d
              '4c 89 e7'            # mov rdi, r12
              '4c 89 f9'            # mov rcx, r15          (= original rsi)
              '89 1c 24'            # mov [rsp], ebx        (= original ecx)
              'e8 dddddddd')        # call 0x19e00
assert 0x19b71 + 5 + disps[0] == 0x19e00, hex(0x19b71 + 5 + disps[0])

# Shadow-heap call site
disps = match(0x55fe2c,
              '31 ff'                           # xor edi, edi
              'be 00 00 40 06'                   # mov esi, 0x6400000
              'ba 00 00 00 20'                   # mov edx, 0x20000000
              'b9 01 00 00 00'                   # mov ecx, 1
              '41 c7 45 00 01 00 00 00'         # mov dword [r13], 1
              'e8 dddddddd')                    # call 0x19ae0
assert 0x55fe45 + 5 + disps[0] == 0x19ae0, hex(0x55fe45 + 5 + disps[0])

# Reader immediately after return
match(0x55fe4a, '48 8b 48 60')                  # mov rcx, [rax+0x60]
match(0x55fe69, '48 03 48 58')                  # add rcx, [rax+0x58]

# Inner writer of size/start
match(0x19fd9, '4c 89 68 58  48 89 48 60  c6 40 69 01')

# Alignment rounding in 0x19e00 (edx/r9 → ebx)
match(0x19e2d, '81 c3 ff ff 00 00  81 e3 00 00 ff ff')

print('OK: 0x19ae0 contract — args (rdi,rsi=size,edx=align,ecx=flags), forward to 0x19e00, '
      'Shadow call (0,0x6400000,0x20000000,1), R[+0x58]=size R[+0x60]=start, 64KiB align round')
