#!/usr/bin/env python3
"""Tie native/core/vec_d00600 to original RVA 0xd00600 (private; needs the ELF)."""
import json, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
A = 0xd00600
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 17))
template = bytes.fromhex('c5f8100500000000c5f8110500000000c3')
masked = bytearray(body); masked[4:8]=b'\0'*4; masked[12:16]=b'\0'*4
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
src = A + 8 + struct.unpack_from('<i', body, 4)[0]
dst = A + 16 + struct.unpack_from('<i', body, 12)[0]
bits = list(struct.unpack('<4I', img.read(src, 16)))
assert bits == [0xffffffff, 0xffffffff, 0xffffffff, 0]
assert (src, dst) == (0x12fb8a0, 0x3351db0), (hex(src), hex(dst))
assert (src, dst) != (0x12fb6b0, 0x3351da0)  # 0xcfcc60
assert (src, dst) != (0x12fc350, 0x3351dc0)  # 0xd04ce0
assert (src, dst) != (0x12fc9b0, 0x3351dd0)  # 0xd06dd0
assert (src, dst) != (0x12fce70, 0x3351de0)  # 0xd08240
exe = ROOT / 'local/build/native/test_vec_d00600'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 137
dump = ROOT / 'local/re/work/cd00600.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print('check_fd00600: OK')
