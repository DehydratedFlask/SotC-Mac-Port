#!/usr/bin/env python3
"""Tie native/core/vec_ce6fb0 to original RVA 0xce6fb0 (private; needs the ELF)."""
import json, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
A = 0xce6fb0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 17))
template = bytes.fromhex('c5f8100500000000c5f8110500000000c3')
masked = bytearray(body); masked[4:8]=b'\0'*4; masked[12:16]=b'\0'*4
assert bytes(masked) == template
src = A + 8 + struct.unpack_from('<i', body, 4)[0]
dst = A + 16 + struct.unpack_from('<i', body, 12)[0]
bits = list(struct.unpack('<4I', img.read(src, 16)))
assert bits == [0xffffffff, 0xffffffff, 0xffffffff, 0]
assert (src, dst) == (0x12f89a0, 0x3351ce0), (hex(src), hex(dst))
assert (src, dst) != (0x12f8560, 0x3351cc0)  # 0xce6130
assert (src, dst) != (0x12f8790, 0x3351cd0)  # 0xce67b0
assert (src, dst) != (0x12f8bb0, 0x3351cf0)  # 0xce7300
assert (src, dst) != (0x12f8d90, 0x3351d00)  # 0xce74a0
exe = ROOT / 'local/build/native/test_vec_ce6fb0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 150
dump = ROOT / 'local/re/work/cce6fb0.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print('check_fce6fb0: OK')
