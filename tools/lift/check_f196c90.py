#!/usr/bin/env python3
"""Tie native/core/clear_flag32 to original RVA 0x196c90 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x196c90
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 9))
template = bytes.fromhex('31 c0 87 05 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[4:8] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
cell = A + 8 + struct.unpack_from('<i', body, 4)[0]
assert cell == 0x2ee4530, hex(cell)
print(f'OK: 0x196c90 xor-eax; xchg dword [{cell:#x}], eax; ret — seq_cst clear of u32 cell')
