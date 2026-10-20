#!/usr/bin/env python3
"""Prove OBS intrusive list surgery after split (private ELF).

  * 0x1a913..0x1a92f: insert neu before successor (non-empty path)
  * 0x1a961: owner = sentinel
  * 0x1a974..0x1a98f: unlink (prev.next=next, next.prev=prev, self-loop next)
  * 0x1a997..0x1a9b7: insert at arena head (+0x10), owner=arena, inc [arena+0x20]
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
    pos, disps = 0, []
    for t in toks:
        if t.lower() == 'dddddddd':
            disps.append(struct.unpack_from('<i', code, pos)[0])
            pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return disps


# insert_before non-empty
match(0x1a913,
      '48 8d 51 20'             # lea rdx, [rcx+0x20]
      '4d 89 76 20'             # mov [r14+0x20], r14
      '49 89 56 30'             # mov [r14+0x30], rdx
      '48 8b 51 28'             # mov rdx, [rcx+0x28]
      '49 89 56 28'             # mov [r14+0x28], rdx
      '48 89 41 28'             # mov [rcx+0x28], rax
      '49 8b 4e 28'             # mov rcx, [r14+0x28]
      '48 89 41 10'             # mov [rcx+0x10], rax
      '41 ff 47 48')            # inc dword [r15+0x48]

match(0x1a961, '4d 89 46 38')   # mov [r14+0x38], r8

# unlink
match(0x1a974,
      '49 8b 56 30'             # mov rdx, [r14+0x30]
      '49 8b 4e 28'             # mov rcx, [r14+0x28]
      '49 8d 46 20'             # lea rax, [r14+0x20]
      '4c 8b 22'                # mov r12, [rdx]
      '48 89 51 10'             # mov [rcx+0x10], rdx
      '49 8b 56 30'             # mov rdx, [r14+0x30]
      '48 89 4a 08'             # mov [rdx+0x8], rcx
      '49 89 46 30'             # mov [r14+0x30], rax
      '41 ff 4f 48')            # dec dword [r15+0x48]

# insert at allocated head
match(0x1a997,
      '4d 89 76 20'             # mov [r14+0x20], r14
      '4d 89 7e 28'             # mov [r14+0x28], r15
      '49 8b 4f 10'             # mov rcx, [r15+0x10]
      '49 89 4e 30'             # mov [r14+0x30], rcx
      '49 89 47 10'             # mov [r15+0x10], rax
      '49 8b 4e 30'             # mov rcx, [r14+0x30]
      '48 89 41 08'             # mov [rcx+0x8], rax
      '41 ff 47 20'             # inc dword [r15+0x20]
      '4d 89 7e 38')            # mov [r14+0x38], r15

print('OK: 0x1a902 list surgery — insert_before, owner@+0x38, unlink self-loop, '
      'head insert via [arena+0x10], inc [arena+0x20]')
