#!/usr/bin/env python3
"""Prove OBS arena used-byte high-water after list surgery (private ELF).

  0x1a9bb..0x1a9fc (immediately after alloc-count inc / owner store at 0x1a9b7):
    used[+0x90] += rbx; if used > peak[+0x98] then peak = used   (unsigned jbe)
    used[+0x80] += rbx; if used > peak[+0x88] then peak = used

Distinct from arena_accounting at 0x1a123 (mode-2 subtract +0x90).
Does not cover call 0x19660 at 0x1aa0f.
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
    for t in toks:
        if t.lower() == 'dddddddd':
            pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return size


n = match(0x1a9bb,
          '49 8b 87 90 00 00 00'   # mov rax, [r15+0x90]
          '48 01 d8'               # add rax, rbx
          '49 89 87 90 00 00 00'   # mov [r15+0x90], rax
          '49 3b 87 98 00 00 00'   # cmp rax, [r15+0x98]
          '76 07'                   # jbe +7
          '49 89 87 98 00 00 00'   # mov [r15+0x98], rax
          '49 8b 87 80 00 00 00'   # mov rax, [r15+0x80]
          '48 01 d8'               # add rax, rbx
          '49 89 87 80 00 00 00'   # mov [r15+0x80], rax
          '49 3b 87 88 00 00 00'   # cmp rax, [r15+0x88]
          '76 07'                   # jbe +7
          '49 89 87 88 00 00 00')  # mov [r15+0x88], rax
assert n == 66

# Immediately precedes remainder-split path that calls 0x19660 — not lifted here.
call = img.read(0x1aa0f, 5)
assert call[0] == 0xe8
rel = struct.unpack_from('<i', call, 1)[0]
assert 0x1aa0f + 5 + rel == 0x19660

print('OK: 0x1a9bb high-water — ±used@+0x90/+0x80, peaks@+0x98/+0x88 (unsigned jbe); '
      '0x19660 call after block not lifted')
