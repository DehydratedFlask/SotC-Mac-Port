#!/usr/bin/env python3
"""Tie native/core/init_f4eba0 to original RVA 0xf4eba0 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xf4eba0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = img.read(A, 101)
assert body[-1] == 0xc3 and body[:4] == bytes.fromhex('c5f976c0')

def rip(disp_at, end):
    return A + end + struct.unpack_from('<i', body, disp_at)[0]

assert rip(15, 19) == 0x3356940
assert rip(58, 62) == 0x3356958
assert struct.unpack('<Q', img.read(rip(7, 11), 8))[0] == 0xffffffff00000000
assert list(struct.unpack('<4I', img.read(rip(51, 55), 16))) == [0x34000000] * 4
assert struct.unpack('<Q', img.read(rip(65, 69), 8))[0] == 0xff7fffffff7fffff

exe = ROOT / 'local/build/native/test_init_f4eba0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['q1'] == 0xffffffff00000000
assert native['q2'] == 0xff7fffffff7fffff
assert native['f2'] == 0xff7fffff
assert native['eps0'] == 0x34000000
print('OK: 0xf4eba0 (idx 5) BSS @0x3356940 layout + constants; native dump matches')
