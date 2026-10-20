#!/usr/bin/env python3
"""Tie native/core/store_zero32 to original RVA 0xa3ade0 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xa3ade0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 11))
template = bytes.fromhex('c7 05 00 00 00 00 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[2:6] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
cell = A + 10 + struct.unpack_from('<i', body, 2)[0]
imm = struct.unpack_from('<I', body, 6)[0]
assert cell == 0x32f5980 and imm == 0, (hex(cell), imm)
print(f'OK: 0xa3ade0 mov dword [{cell:#x}], 0; ret')
