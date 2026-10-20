#!/usr/bin/env python3
"""Tie native/core/vec_d7b930 to original RVA 0xd7b930 (private; needs the ELF)."""
import json, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
A = 0xd7b930
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 17))
template = bytes.fromhex('c5 f8 10 05 00 00 00 00 c5 f8 11 05 00 00 00 00 c3'.replace(' ', ''))
masked = bytearray(body); masked[4:8]=b'\0'*4; masked[12:16]=b'\0'*4
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
src = A + 8 + struct.unpack_from('<i', body, 4)[0]
dst = A + 16 + struct.unpack_from('<i', body, 12)[0]
bits = list(struct.unpack('<4I', img.read(src, 16)))
assert bits == [0xffffffff, 0xffffffff, 0xffffffff, 0]
assert (src, dst) == (0x1306100, 0x33529d0), (hex(src), hex(dst))
assert (src, dst) != (0x1306860, 0x33529e0)  # 0xd7bc60
assert (src, dst) != (0x1306a00, 0x33529f0)  # 0xd7be60
assert (src, dst) != (0x1306b90, 0x3352a00)  # 0xd7c380
assert (src, dst) != (0x1306d20, 0x3352a10)  # 0xd7c4d0
exe = ROOT / 'local/build/native/test_vec_d7b930'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits and native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 94
dump = ROOT / 'local/re/work/cd7b930.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK: 0xd7b930 unguarded vec copy; src={src:#x} dst={dst:#x} bits=~0,~0,~0,0; '
      f'index 94; native dump matches; distinct sibling cells')
