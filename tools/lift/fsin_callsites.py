#!/usr/bin/env python3
"""Classify every direct call to the `_FSin` PLT thunk by how edi/esi are set just before it.

Evidence for the meaning of the first integer argument (quadrant offset: 0 = sin, 1 = cos).
Pattern match on the 24 bytes preceding each `call`; anything else is counted as unclassified.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
parsed = d.parse(elf)
code_ph = next(p for p in parsed['ph'] if p['type'] == d.PT_LOAD and p['flags'] & 1)
code = elf[code_ph['offset']:code_ph['offset'] + code_ph['filesz']]
nid = d.nid('_FSin')
thunk = None
for a in range(0xf8f000, 0xf90400, 16):                   # PLT region observed around 0xf8f4xx..0xf90300
    c = code[a:a + 6]
    if c[:2] == b'\xff\x25':
        slot = a + 6 + struct.unpack('<i', c[2:])[0]
        if any(r['target'] == slot and parsed['symbols'][r['symbol']]['nid'] == nid for r in parsed['relocs'] if r['type'] in (1, 6, 7)):
            thunk = a
assert thunk is not None, 'thunk not found'
counts = {'edi0_esi0': 0, 'edi1_esi0': 0, 'other': 0}
sites = []
i = code.find(b'\xe8')
while i != -1:
    if i + 5 <= len(code) and i + 5 + struct.unpack('<i', code[i + 1:i + 5])[0] == thunk:
        sites.append(i)
        pre = code[max(0, i - 24):i]
        if b'\xbf\x01\x00\x00\x00\x31\xf6' in pre:
            counts['edi1_esi0'] += 1
        elif b'\x31\xff\x31\xf6' in pre or b'\x31\xf6\x31\xff' in pre:
            counts['edi0_esi0'] += 1
        else:
            counts['other'] += 1
    i = code.find(b'\xe8', i + 1)
print(f'thunk={thunk:#x} call_sites={len(sites)} {counts}')
