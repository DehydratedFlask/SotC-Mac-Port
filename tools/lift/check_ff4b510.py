#!/usr/bin/env python3
"""Tie native/core/vec_neg036 to original RVA 0xf4b510 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import guarded_const_ctor as g  # noqa: E402
import ps4_dynlink as d  # noqa: E402

A = 0xf4b510
elf_path = ROOT / 'local/re/eboot.elf'
recs, stop, nxt, tail = g.lift(elf_path, A)
assert recs == [] and len(tail) == 1 and tail[0]['kind'] == 'guarded_vec_copy', (recs, tail)
assert stop == A + 38, hex(stop)  # ret at +38
t = tail[0]
assert t['bits'] == [0xbeb851ea] * 4, t['bits']
assert t['guard'] == 0x3356840 and t['value'] == 0x3356830 and t['source'] == 0x132c8f0

elf = elf_path.read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = bytearray(img.read(A, 39))
template = bytes.fromhex(
    '48 8d 05 00 00 00 00 80 38 00 75 1a c5 f8 10 05 00 00 00 00'
    '48 8d 0d 00 00 00 00 48 c7 00 01 00 00 00 c5 f8 11 01 c3'.replace(' ', ''))
masked = bytearray(body)
for lo, hi in ((3, 7), (16, 20), (23, 27)):
    masked[lo:hi] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())

exe = ROOT / 'local/build/native/test_vec_neg036'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == t['bits']
print(f'OK: 0xf4b510 guarded_vec_copy; guard={t["guard"]:#x} value={t["value"]:#x} '
      f'source={t["source"]:#x} bits=0xbeb851ea*4; native dump matches')
