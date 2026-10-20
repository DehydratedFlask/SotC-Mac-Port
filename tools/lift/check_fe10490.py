#!/usr/bin/env python3
"""Tie native/core/trig_constants to the exact bytes of original RVA 0xe10490 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xe10490
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
parsed = d.parse(elf)
img = d.Image(elf, parsed['ph'])
body = img.read(A, 50)
template = bytes.fromhex('55 48 89 e5 c5 f8 10 05 00000000 bf 01 00 00 00 31 f6 c5 f8 11 05 00000000 '
                         'c5 fa 10 05 00000000 e8 00000000 c5 fa 11 05 00000000 5d c3'.replace(' ', ''))
assert len(template) == 50, len(template)
masked = bytearray(body)
for lo in (8, 23, 31, 36, 44):
    masked[lo:lo + 4] = b'\0\0\0\0'
assert bytes(masked) == template, body.hex()
assert img.read(A + 50, 1) in (b'\x90', b'\xcc', b'\x00')

def tgt(at, end):
    return A + end + struct.unpack('<i', body[at:at + 4])[0]

src_vec, dst_vec, src_x, call, dst_f = tgt(8, 12), tgt(23, 27), tgt(31, 35), tgt(36, 40), tgt(44, 48)
mask_bits = struct.unpack('<4I', img.read(src_vec, 16))
x_bits = struct.unpack('<I', img.read(src_x, 4))[0]
thunk = img.read(call, 6)
assert thunk[:2] == b'\xff\x25'
slot = call + 6 + struct.unpack('<i', thunk[2:])[0]
sym = next(parsed['symbols'][r['symbol']] for r in parsed['relocs'] if r['target'] == slot and r['type'] in (1, 6, 7))
assert sym['nid'] == d.nid('_FSin'), sym['nid']
assert mask_bits == (0xffffffff, 0xffffffff, 0xffffffff, 0), mask_bits
native = json.loads(subprocess.run([str(ROOT / 'local/build/native/test_trig_constants'), '--dump'],
                                   check=True, capture_output=True, text=True).stdout)
assert native['x_bits'] == x_bits == 0x3d567750 and native['offset'] == 1 and body[13] == 1
print(f'OK: 0xe10490..{A + 50:#x} template-exact; vec {src_vec:#x}->{dst_vec:#x}; _FSin(x_bits={x_bits:#x}, 1, 0) -> {dst_f:#x}; native inputs match')
