#!/usr/bin/env python3
"""Tie native/core/clear_flag64 to original RVA 0x3bd70 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x3bd70
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 10))
# xor eax,eax; xchg rax, [rip+disp]; ret  — REX.W form (distinct from 0x196c90's 32-bit xchg)
template = bytes.fromhex('31 c0 48 87 05 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[5:9] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
cell = A + 9 + struct.unpack_from('<i', body, 5)[0]
assert cell == 0x2ea4f20, hex(cell)
print(f'OK: 0x3bd70 xor-eax; xchg rax, [{cell:#x}]; ret — seq_cst clear of u64 cell '
      f'(not Drive 0x196c90 u32 @0x2ee4530)')
