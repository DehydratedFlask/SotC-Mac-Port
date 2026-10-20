#!/usr/bin/env python3
"""Tie native/core/scratch_reset to the exact bytes of original RVA 0x125990 (private; needs the ELF).

Matches the 41-byte body against a template with only the three disp32 fields free, resolves the
call through its PLT thunk/GOT/relocation to an import, and confirms that import's NID is memset's.
"""
import json
import subprocess
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0x125990
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
parsed = d.parse(elf)
img = d.Image(elf, parsed['ph'])
body = bytearray(img.read(A, 41))
template = bytes.fromhex('55 48 89 e5 53 50 48 8d 3d 00 00 00 00 31 f6 ba 00 20 00 00 31 db e8 00 00 00 00 '
                         '48 87 1d 00 00 00 00 48 83 c4 08 5b 5d c3'.replace(' ', ''))
assert len(template) == 41, len(template)
masked = bytearray(body)
for lo, hi in ((9, 13), (23, 27), (30, 34)):
    masked[lo:hi] = b'\0\0\0\0'
assert bytes(masked) == template, f'body differs from template: {bytes(body).hex()}'
assert img.read(A + 41, 1) in (b'\x90', b'\xcc', b'\x00'), 'expected padding after ret'

def target(at, end):          # rip-relative displacement at body[at:at+4], insn ends at A+end
    return A + end + struct.unpack('<i', body[at:at + 4])[0]

buf = target(9, 13)
call = target(23, 27)
slot = target(30, 34)
thunk = img.read(call, 6)
assert thunk[:2] == b'\xff\x25'
got_slot = call + 6 + struct.unpack('<i', thunk[2:])[0]
sym = next(parsed['symbols'][r['symbol']] for r in parsed['relocs'] if r['target'] == got_slot and r['type'] in (1, 6, 7))
assert sym['nid'] == d.nid('memset'), (sym['nid'], d.nid('memset'))
length = struct.unpack('<I', body[16:20])[0]
exe = ROOT / 'local/build/native/test_scratch_reset'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['scratch_bytes'] == length == 0x2000
print(f'OK: 0x125990..{A + 41:#x} template-exact; memset({buf:#x},0,{length:#x}); xchg[{slot:#x}],0; '
      f'slot-buffer={slot - buf:#x}; native size == binary size')
