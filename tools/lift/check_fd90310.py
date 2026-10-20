#!/usr/bin/env python3
"""Tie native/core/vec_d90310 to original RVA 0xd90310 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xd90310
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
assert (src, dst) == (0x130b1a0, 0x3352c10), (hex(src), hex(dst))
assert (src, dst) != (0x130b660, 0x3352c20)  # 0xd94cb0
assert (src, dst) != (0x130ca00, 0x3352c30)  # 0xdac410
assert (src, dst) != (0x130d050, 0x3352c40)  # 0xdb3960
assert (src, dst) != (0x130d380, 0x3352c60)  # 0xdb5dd0
assert (src, dst) != (0x130d730, 0x3352c70)  # 0xdb7e60
assert (src, dst) != (0x130dbb0, 0x3352c90)  # 0xdbb280

exe = ROOT / 'local/build/native/test_vec_d90310'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 79
dump = ROOT / 'local/re/work/cd90310.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd90310 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 79; native dump matches; distinct sibling cells')
