#!/usr/bin/env python3
"""Tie native/core/vec_d32b70 to original RVA 0xd32b70 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xd32b70
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
assert (src, dst) == (0x1300d30, 0x3351ee0), (hex(src), hex(dst))
assert (src, dst) != (0x1300ba0, 0x3351ed0)  # 0xd32720
assert (src, dst) != (0x13009a0, 0x3351ec0)  # 0xd2ffe0
assert (src, dst) != (0x1301a50, 0x33520a0)  # 0xd3b790
assert (src, dst) != (0x13016d0, 0x3352090)  # 0xd3a6d0
assert (src, dst) != (0x1301e30, 0x33520b0)  # 0xd40010

exe = ROOT / 'local/build/native/test_vec_d32b70'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 118
dump = ROOT / 'local/re/work/cd32b70.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd32b70 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 118; native dump matches; distinct sibling cells')
