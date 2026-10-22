#!/usr/bin/env python3
"""Pin PLT thunk 0xf8f390 as the __cxa_atexit import (tail of ctor 0xde0160 and many cleanup thunks).

Private: needs local/re/eboot.elf. Confirms the 6-byte jmp [rip] thunk, its GOT slot, and the
JMPREL/RELA NID equal to ps4_dynlink.nid('__cxa_atexit').
"""
import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

THUNK = 0xf8f390
GOT = 0x15fffb8
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
parsed = d.parse(elf)
img = d.Image(elf, parsed['ph'])
thunk = img.read(THUNK, 6)
require(thunk[:2] == b'\xff\x25', f'thunk is not jmp [rip]: {thunk.hex()}')
slot = THUNK + 6 + struct.unpack('<i', thunk[2:])[0]
require(slot == GOT, f'GOT slot {slot:#x} != {GOT:#x}')
sym = next(
    parsed['symbols'][r['symbol']]
    for r in parsed['relocs']
    if r['target'] == slot and r['type'] in (1, 6, 7)
)
require(sym['nid'] == d.nid('__cxa_atexit'),
        f"nid {sym['nid']} != __cxa_atexit {d.nid('__cxa_atexit')}")
print(f'OK: 0xf8f390 PLT jmp [GOT {GOT:#x}] = __cxa_atexit (NID {sym["nid"]})')
