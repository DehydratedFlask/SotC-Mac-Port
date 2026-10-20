#!/usr/bin/env python3
"""Tie native/core/vec_d2cb30 to original RVA 0xd2cb30 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xd2cb30
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
assert (src, dst) == (0x13007c0, 0x3351eb0), (hex(src), hex(dst))
assert (src, dst) != (0x12ff9e0, 0x3351ea0)  # 0xd2c7e0
assert (src, dst) != (0x13009a0, 0x3351ec0)  # 0xd2ffe0
assert (src, dst) != (0x1300ba0, 0x3351ed0)  # 0xd32720
assert (src, dst) != (0x1300d30, 0x3351ee0)  # 0xd32b70
assert (src, dst) != (0x12ff030, 0x3351e90)  # 0xd20590

exe = ROOT / 'local/build/native/test_vec_d2cb30'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 121
dump = ROOT / 'local/re/work/cd2cb30.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd2cb30 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 121; native dump matches; distinct sibling cells')
