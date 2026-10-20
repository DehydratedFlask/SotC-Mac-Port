#!/usr/bin/env python3
"""Tie native/core/zero_xmm_qword to original RVA 0x58bd10 (private; needs the ELF)."""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x58bd10
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 24))
template = bytes.fromhex(
    'c5 f8 57 c0 c5 f8 11 05 00 00 00 00'
    '48 c7 05 00 00 00 00 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[8:12] = b'\0\0\0\0'
masked[15:19] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
xmm = A + 12 + struct.unpack_from('<i', body, 8)[0]
q = A + 23 + struct.unpack_from('<i', body, 15)[0]
imm = struct.unpack_from('<I', body, 19)[0]
assert xmm == 0x2f4e4b0 and q == 0x2f4e4d0 and imm == 0, (hex(xmm), hex(q), imm)
assert q - xmm == 0x20, 'expected 16-byte gap between xmm end and qword'
print(f'OK: 0x58bd10 vxorps; vmovups [{xmm:#x}]; mov qword [{q:#x}], 0; ret')
