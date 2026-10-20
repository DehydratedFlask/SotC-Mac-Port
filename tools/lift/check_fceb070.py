#!/usr/bin/env python3
"""Tie native/core/vec_ceb070 to original RVA 0xceb070 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xceb070
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
assert (src, dst) == (0x12f95c0, 0x3351d30), (hex(src), hex(dst))
assert (src, dst) != (0x12f97d0, 0x3351d40)  # 0xceb1c0
assert (src, dst) != (0x12f9990, 0x3351d50)  # 0xceb2c0
assert (src, dst) != (0x12f9b60, 0x3351d60)  # 0xceb890
assert (src, dst) != (0x12f9370, 0x3351d20)  # 0xce99c0

exe = ROOT / 'local/build/native/test_vec_ceb070'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 87
dump = ROOT / 'local/re/work/cceb070.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xceb070 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 87; native dump matches; distinct sibling cells')
