#!/usr/bin/env python3
"""Tie native/core/vec_ddef00 to original RVA 0xddef00 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xddef00
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 17))
template = bytes.fromhex('c5 f8 10 05 00 00 00 00 c5 f8 11 05 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body)
masked[4:8] = b'\0\0\0\0'
masked[12:16] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
src = A + 8 + struct.unpack_from('<i', body, 4)[0]
dst = A + 16 + struct.unpack_from('<i', body, 12)[0]
bits = list(struct.unpack('<4I', img.read(src, 16)))
assert bits == [0xffffffff, 0xffffffff, 0xffffffff, 0]
assert (src, dst) == (0x130f540, 0x3352ed0), (hex(src), hex(dst))
assert (src, dst) != (0x130fd00, 0x3353300)  # 0xde1ad0
assert (src, dst) != (0x1310350, 0x3353310)  # 0xde6d20
assert (src, dst) != (0x1310c90, 0x3353320)  # 0xde7fb0
assert (src, dst) != (0x13124a0, 0x3353340)  # 0xdfd5f0

exe = ROOT / 'local/build/native/test_vec_ddef00'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 60
dump = ROOT / 'local/re/work/cddef00.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xddef00 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 60; native dump matches; distinct sibling cells')
