#!/usr/bin/env python3
"""Prove OBS free-path page debit + idle-pool insert (private ELF).

  0x1ae67..0x1ae6b: shr rax,16 ; sub dword [r15+0x70], eax
  0x1aecd..0x1af03: load global pool; list-insert at +0x1f0; owner=+0x1e0; inc [+0x200]

Does not cover page-map fill @0x1ab08 or 0x19660.
Unlink + dec [arena+0x48] immediately before idle insert is already tied in check_f1a902_list.
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
    pos, abs_rel = 0, []
    for t in toks:
        if t.lower() == 'dddddddd':
            abs_rel.append(struct.unpack_from('<i', code, pos)[0])
            pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return abs_rel


match(0x1ae67,
      '48 c1 e8 10'    # shr rax, 0x10
      '41 29 47 70')   # sub [r15+0x70], eax

# Same cell also debited after page-map path (r13d), still not lifting that fill.
match(0x1abd2, '45 29 6f 70')  # sub [r15+0x70], r13d

rels = match(0x1aecd,
             '48 8b 0d dddddddd'   # mov rcx, [rip+disp] → 0x2dfca78
             '48 89 40 20'         # mov [rax+0x20], rax
             '48 8d 91 e0 01 00 00'  # lea rdx, [rcx+0x1e0]
             '48 89 50 28'         # mov [rax+0x28], rdx
             '48 8b b1 f0 01 00 00'  # mov rsi, [rcx+0x1f0]
             '48 89 70 30'         # mov [rax+0x30], rsi
             '48 89 b9 f0 01 00 00'  # mov [rcx+0x1f0], rdi
             '48 8b 70 30'         # mov rsi, [rax+0x30]
             '48 89 7e 08'         # mov [rsi+0x8], rdi
             'ff 81 00 02 00 00'   # inc dword [rcx+0x200]
             '48 89 50 38')        # mov [rax+0x38], rdx
# rip-relative target
rip = 0x1aecd + 7
assert rip + rels[0] == 0x2dfca78, (hex(rip + rels[0]),)

print('OK: 0x1ae67 page debit [arena+0x70]-=(bytes>>16); idle-pool insert '
      '@[0x2dfca78]+0x1e0/0x1f0/+0x200 (list_insert_at_head shape)')
