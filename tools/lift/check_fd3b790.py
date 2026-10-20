#!/usr/bin/env python3
"""Tie native/core/vec_d3b790 to original RVA 0xd3b790 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xd3b790
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
assert (src, dst) == (0x1301a50, 0x33520a0), (hex(src), hex(dst))
assert (src, dst) != (0x1301e30, 0x33520b0)  # 0xd40010
assert (src, dst) != (0x1302220, 0x33520c0)  # 0xd405a0
assert (src, dst) != (0x13023d0, 0x33520d0)  # 0xd40ea0
assert (src, dst) != (0x1302650, 0x33520e0)  # 0xd42b60

exe = ROOT / 'local/build/native/test_vec_d3b790'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 112
dump = ROOT / 'local/re/work/cd3b790.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd3b790 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 112; native dump matches; distinct sibling cells')
