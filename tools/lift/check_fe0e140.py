#!/usr/bin/env python3
"""Tie native/core/normal_threshold to the compare block inside original RVA 0xe0e140 (private; needs the ELF).

Only the block 0xe0e4c0..0xe0e4e2 is matched byte-for-byte (no rip-relative fields inside it), and the threshold
global it compares against is shown to be the one written by constructor 0xe10490.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

block = img.read(0xe0e4c0, 0x22)
expected = bytes.fromhex(
    'c5ca591b'            # vmulss xmm3, xmm6, [rbx]        a.y * b.y
    'c5c259cd'            # vmulss xmm1, xmm7, xmm5         b.x * a.x
    'c5fa107d00'          # vmovss xmm7, [rbp]              a.z
    'c5c2596431' '08'     # vmulss xmm4, xmm7, [rcx+rsi+8] a.z * b.z
    'c5e258c9'            # vaddss xmm1, xmm3, xmm1         (y+x)
    'c5f258cc'            # vaddss xmm1, xmm1, xmm4         +z
    'c4c1782ec8'          # vucomiss xmm1, xmm8             compare with threshold
    '765a')               # jbe skip                        taken when dot <= thr or unordered
assert block[:len(expected) - 2] == expected[:-2], (block.hex(), expected.hex())
assert block[len(expected) - 2] == 0x76, 'expected jbe'

load = img.read(0xe0e170, 8)
assert load[:4] == bytes.fromhex('c57a1005'), load.hex()           # vmovss xmm8, [rip+disp]
glob = 0xe0e178 + struct.unpack('<i', load[4:])[0]
assert glob == 0x33533b0, hex(glob)                                 # == destination written by ctor 0xe10490
print(f'OK: compare block @0xe0e4c0 byte-exact (dot=((ay*by)+(bx*ax))+(az*bz) > [{glob:#x}], unordered->skip); '
      f'threshold global is the cos(3deg) cell written by ctor 0xe10490')
