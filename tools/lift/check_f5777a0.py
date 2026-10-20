#!/usr/bin/env python3
"""Tie native/core/zero_xmm16 to original RVA 0x5777a0 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x5777a0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 13))
template = bytes.fromhex('c5 f8 57 c0 c5 f8 11 05 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[8:12] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
dst = A + 12 + struct.unpack_from('<i', body, 8)[0]
assert dst == 0x2f295b8, hex(dst)
print(f'OK: 0x5777a0 vxorps xmm0; vmovups [{dst:#x}], xmm0; ret')
