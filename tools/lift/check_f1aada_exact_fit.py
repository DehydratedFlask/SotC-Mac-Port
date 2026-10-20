#!/usr/bin/env python3
"""Prove OBS exact-fit consume finalize (private ELF).

  0x1aada..0x1aaf3 (no call to 0x19660 on this path):
    mov rcx, [r8]           ; load start (unused for stores in this window)
    mov edi, [r15+0x78]     ; arena flag word
    test rsi, rsi
    mov [r10], dil          ; node+0x68 flag_a
    mov [r9], rbx           ; node+0x58 size = req
    mov byte [r14+0x69], 1  ; flag_b
    mov [r14+0x40], rsi     ; ptr/ctx
    mov dword [r14+0x48], -1

Does not cover page-map @0x1ab08 or 0x19660.
"""
import re
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
    for t in toks:
        if t.lower() == 'dddddddd':
            pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return size


n = match(0x1aada,
          '49 8b 08'             # mov rcx, [r8]
          '41 8b 7f 78'          # mov edi, [r15+0x78]
          '48 85 f6'             # test rsi, rsi
          '41 88 3a'             # mov [r10], dil
          '49 89 19'             # mov [r9], rbx
          '41 c6 46 69 01'       # mov byte [r14+0x69], 1
          '49 89 76 40'          # mov [r14+0x40], rsi
          '41 c7 46 48 ff ff ff ff')  # mov dword [r14+0x48], -1
assert n == 33

# Prove this path is reached without 0x19660: jbe from capacity cmp lands at 0x1aaa0
# which jumps to 0x1aada (lea/jmp chain), not the call at 0x1aa0f.
match(0x1aa01, '0f 86 99 00 00 00')  # jbe 0x1aaa0
match(0x1aaa0,
      '4d 8d 46 60'             # lea r8, [r14+0x60]
      '4d 8d 56 68'             # lea r10, [r14+0x68]
      '4d 8d 4e 58'             # lea r9, [r14+0x58]
      'eb 2c')                  # jmp 0x1aada

print('OK: 0x1aada exact-fit finalize — flag_a←[arena+0x78], size=req, flag_b=1, '
      'ptr=ctx, sentinel=~0; path skips 0x19660')
