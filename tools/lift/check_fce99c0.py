#!/usr/bin/env python3
"""Tie native/core/vec_ce99c0 to original RVA 0xce99c0 (private; needs the ELF)."""
import json, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
A = 0xce99c0
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
assert (src, dst) == (0x12f9370, 0x3351d20), (hex(src), hex(dst))
assert (src, dst) != (0x12f9100, 0x3351d10)  # 0xce8440
assert (src, dst) != (0x12f95c0, 0x3351d30)  # 0xceb070
assert (src, dst) != (0x12f97d0, 0x3351d40)  # 0xceb1c0
assert (src, dst) != (0x12f9b60, 0x3351d60)  # 0xceb890
exe = ROOT / 'local/build/native/test_vec_ce99c0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 146
dump = ROOT / 'local/re/work/cce99c0.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print('check_fce99c0: OK')
