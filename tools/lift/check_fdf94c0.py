#!/usr/bin/env python3
"""Tie native/core/leaf_df94c0 to the exact bytes of original RVA 0xdf94c0 (private; needs the ELF).

Pins: body length/SHA-256/template, rodata input bits, destination cell, that the call goes to the
`_FSin` import thunk (NID) shared with ctor 0xe10490, edi=1/esi=0, and the native test's inputs.
"""
import hashlib
import json
import os
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xdf94c0
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BODY_SHA256 = '4fa743f1e109cbe0b3884cf10fe5c521cfa6f24463d7150d9f9b0cb4a6a1c0fe'


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
parsed = d.parse(elf)
img = d.Image(elf, parsed['ph'])
body = img.read(A, 34)
require(hashlib.sha256(body).hexdigest() == BODY_SHA256, f'body bytes changed {hashlib.sha256(body).hexdigest()}')
template = bytes.fromhex('55 48 89 e5 c5 fa 10 05 00000000 bf 01 00 00 00 31 f6 e8 00000000 '
                         'c5 fa 11 05 00000000 5d c3'.replace(' ', ''))
require(len(template) == 34, 'template length')
masked = bytearray(body)
for lo in (8, 20, 28):
    masked[lo:lo + 4] = b'\0\0\0\0'
require(bytes(masked) == template, body.hex())

def tgt(at, end):
    return A + end + struct.unpack('<i', body[at:at + 4])[0]

src_x, call, dst = tgt(8, 12), tgt(20, 24), tgt(28, 32)
require((src_x, call, dst) == (0x1311c3c, 0xf8fd10, 0x3353330), (hex(src_x), hex(call), hex(dst)))
x_bits = struct.unpack('<I', img.read(src_x, 4))[0]
require(x_bits == 0x3d567750, hex(x_bits))
require(body[13] == 1 and body[17:19] == b'\x31\xf6', 'edi=1, esi=0 setup')   # mov edi,1 ; xor esi,esi
thunk = img.read(call, 6)
require(thunk[:2] == b'\xff\x25', 'callee is not a PLT jmp thunk')
slot = call + 6 + struct.unpack('<i', thunk[2:])[0]
sym = next(parsed['symbols'][r['symbol']] for r in parsed['relocs'] if r['target'] == slot and r['type'] in (1, 6, 7))
require(sym['nid'] == d.nid('_FSin'), sym['nid'])

# Same thunk and same input bits (separate rodata copy) as the already-verified ctor 0xe10490.
other = img.read(0xe10490, 50)
require(0xe10490 + 40 + struct.unpack('<i', other[36:40])[0] == call, '0xe10490 calls a different thunk')
other_x = 0xe10490 + 35 + struct.unpack('<i', other[31:35])[0]     # a separate rodata copy (0x1314a8c)
require(other_x != src_x and img.read(other_x, 4) == img.read(src_x, 4), '0xe10490 input bits differ')

builds = [os.environ['SOTC_NATIVE_BUILD']] if os.environ.get('SOTC_NATIVE_BUILD') else []
builds += ['local/build/agent-ctor-helper', 'local/build/native']
exe = next((ROOT / b / 'test_leaf_df94c0' for b in builds if (ROOT / b / 'test_leaf_df94c0').is_file()), None)
require(exe is not None, f'test_leaf_df94c0 not built in {builds}')
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
require(native['x_bits'] == x_bits and native['offset'] == body[13], 'native inputs differ')
print(f'OK: 0xdf94c0..{A + 34:#x} template-exact; _FSin(x_bits={x_bits:#x}, 1, 0) -> {dst:#x}; '
      f'thunk {call:#x} = _FSin (shared with 0xe10490); native inputs match; result_bits={native["result_bits"]:#x}')
