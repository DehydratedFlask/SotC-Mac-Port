#!/usr/bin/env python3
"""Tie native/core/zero_struct12 to original RVA 0x24ebf0 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x24ebf0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 23))
template = bytes.fromhex(
    '48 8d 05 00 00 00 00'
    'c5 f8 57 c0'
    'c5 f8 13 00'
    'c7 40 08 00 00 00 00'
    'c3'.replace(' ', ''))
masked = bytearray(body)
masked[3:7] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
cell = A + 7 + struct.unpack_from('<i', body, 3)[0]
assert cell == 0x2eebc48, hex(cell)
print(f'OK: 0x24ebf0 lea rax,[{cell:#x}]; vxorps; vmovlps [rax],xmm0; '
      f'mov dword [rax+8],0; ret — clears 12 bytes')
