#!/usr/bin/env python3
"""Tie native/core/vec_e0b180 to original RVA 0xe0b180 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xe0b180
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
assert (src, dst) == (0x13141b0, 0x3353390), (hex(src), hex(dst))
# Distinct from sibling mask copies with the same lane bits.
assert (src, dst) != (0x1329570, 0x33567d0)  # 0xf30460
assert (src, dst) != (0x12d6b20, 0x3350ce0)  # 0xb52110
assert (src, dst) != (0x12d8d90, 0x3350db0)  # 0xb743c0
assert (src, dst) != (0x1313740, 0x3353370)  # 0xe085e0

exe = ROOT / 'local/build/native/test_vec_e0b180'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 49
dump = ROOT / 'local/re/work/ce0b180.bin'
dump.parent.mkdir(parents=True, exist_ok=True)
dump.write_bytes(bytes(body))
print(f'OK: 0xe0b180 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 49; native dump matches; distinct sibling cells')
