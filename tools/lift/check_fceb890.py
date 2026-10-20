#!/usr/bin/env python3
"""Tie native/core/vec_ceb890 to original RVA 0xceb890 (private; needs the ELF)."""
import json, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
A = 0xceb890
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
assert (src, dst) == (0x12f9b60, 0x3351d60), (hex(src), hex(dst))
assert (src, dst) != (0x12f9990, 0x3351d50)  # 0xceb2c0
assert (src, dst) != (0x12f9ec0, 0x3351d70)  # 0xcee8a0
assert (src, dst) != (0x12fa4e0, 0x3351d80)  # 0xcf44a0
assert (src, dst) != (0x12fa850, 0x3351d90)  # 0xcfcbc0
exe = ROOT / 'local/build/native/test_vec_ceb890'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == 142
dump = ROOT / 'local/re/work/cceb890.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print('check_fceb890: OK')
