#!/usr/bin/env python3
"""Tie native/core/vec_d7e640 to original RVA 0xd7e640 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xd7e640
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
assert (src, dst) == (0x1307200, 0x3352a30), (hex(src), hex(dst))
assert (src, dst) != (0x13077e0, 0x3352a40)  # 0xd7fe40
assert (src, dst) != (0x1307dc0, 0x3352a50)  # 0xd7fe90
assert (src, dst) != (0x1307f70, 0x3352a60)  # 0xd80e60
assert (src, dst) != (0x1308450, 0x3352a70)  # 0xd84ee0
assert (src, dst) != (0x130a970, 0x3352a90)  # 0xd85490
assert (src, dst) != (0x130ca00, 0x3352c30)  # 0xdac410

exe = ROOT / 'local/build/native/test_vec_d7e640'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 88
dump = ROOT / 'local/re/work/cd7e640.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd7e640 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 88; native dump matches; distinct sibling cells')
