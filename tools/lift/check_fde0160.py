#!/usr/bin/env python3
"""Tie native/core/leaf_de0160 to the exact bytes of original RVA 0xde0160 (private; needs the ELF).

Pins: 60-byte body SHA-256/template, memset PLT identity + (dst,1,0x400), adjacent 16-byte zero store,
dtor stub at 0xddf980 = `ret`, dso cell 0x1604218, tail __cxa_atexit PLT, and native --dump sizes.
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

A = 0xde0160
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BODY_SHA256 = '4824cb9bf8f87100209cf7a7b59015b18d87f527bead3bfd43f77e6df6da597b'
BODY_LEN = 60


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
parsed = d.parse(elf)
img = d.Image(elf, parsed['ph'])
body = img.read(A, BODY_LEN)
require(hashlib.sha256(body).hexdigest() == BODY_SHA256,
        f'body bytes changed {hashlib.sha256(body).hexdigest()}')
template = bytes.fromhex(
    '55 48 89 e5 48 8d 3d 00 00 00 00 be 01 00 00 00 ba 00 04 00 00 e8 00 00 00 00 '
    '48 8d 15 00 00 00 00 c5 f8 57 c0 48 8d 3d 00 00 00 00 31 f6 c5 f8 11 05 00 00 00 00 '
    '5d e9 00 00 00 00'.replace(' ', ''))
require(len(template) == BODY_LEN, 'template length')
masked = bytearray(body)
for lo, hi in ((7, 11), (22, 26), (29, 33), (40, 44), (50, 54), (56, 60)):
    masked[lo:hi] = b'\0\0\0\0'
require(bytes(masked) == template, body.hex())


def tgt(at, end):
    return A + end + struct.unpack('<i', body[at:at + 4])[0]


buf, call, dso, dtor, zero, jmp = (
    tgt(7, 11), tgt(22, 26), tgt(29, 33), tgt(40, 44), tgt(50, 54), tgt(56, 60))
require((buf, call, dso, dtor, zero, jmp) ==
        (0x3352ee0, 0xf8f4f0, 0x1604218, 0xddf980, 0x33532e0, 0xf8f390),
        (hex(buf), hex(call), hex(dso), hex(dtor), hex(zero), hex(jmp)))
require(zero - buf == 0x400, f'trailer not adjacent: {zero - buf:#x}')
require(body[11:21] == bytes.fromhex('be01000000ba00040000'), 'esi=1 / edx=0x400 setup')
require(body[44:46] == b'\x31\xf6', 'xor esi,esi before atexit')

for thunk_rva, want_name, want_got in (
        (call, 'memset', 0x1600068),
        (jmp, '__cxa_atexit', 0x15fffb8)):
    thunk = img.read(thunk_rva, 6)
    require(thunk[:2] == b'\xff\x25', f'{want_name} thunk is not jmp [rip]')
    slot = thunk_rva + 6 + struct.unpack('<i', thunk[2:])[0]
    require(slot == want_got, f'{want_name} GOT {slot:#x}')
    sym = next(parsed['symbols'][r['symbol']] for r in parsed['relocs']
               if r['target'] == slot and r['type'] in (1, 6, 7))
    require(sym['nid'] == d.nid(want_name), f"{want_name} nid {sym['nid']}")

require(img.read(dtor, 1) == b'\xc3', f'dtor {dtor:#x} is not a ret stub')
# First qword of the shared dso handle record is its own address (observed layout fact).
dso_qw = struct.unpack('<Q', img.read(dso, 8))[0]
require(dso_qw == dso, f'dso self-pointer {dso_qw:#x} != {dso:#x}')

builds = [os.environ['SOTC_NATIVE_BUILD']] if os.environ.get('SOTC_NATIVE_BUILD') else []
builds += ['local/build/native', 'local/build/agent-ctor-helper']
exe = next((ROOT / b / 'test_leaf_de0160' for b in builds if (ROOT / b / 'test_leaf_de0160').is_file()), None)
require(exe is not None, f'test_leaf_de0160 not built in {builds}')
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
require(native == {'fill_bytes': 0x400, 'fill_value': 1, 'trailer_bytes': 16, 'trailer_value': 0},
        native)
print(f'OK: 0xde0160..{A + BODY_LEN:#x} template-exact; memset({buf:#x},1,0x400); '
      f'zero 16 @ {zero:#x}; atexit({dtor:#x}=ret, 0, dso={dso:#x}) omitted in native; '
      f'PLT {call:#x}=memset, {jmp:#x}=__cxa_atexit; native dump matches')
