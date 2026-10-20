#!/usr/bin/env python3
"""Byte-pattern census of bit-packing idioms in the code segment (CPU-02 evidence; no disassembler).

Counts exact encodings only. A count is a lead for inspection, not proof of pointer semantics.
Prints per-idiom totals and the 10 busiest 4 KiB pages so clusters (one source file / one library) show up.
"""
import collections
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
cp = next(p for p in ph if p['type'] == d.PT_LOAD and p['flags'] & 1)
code = elf[cp['offset']:cp['offset'] + cp['filesz']]

IDIOMS = {
    'and r32,0x7fffffff (81 /4)':        rb'(?:\x41)?\x81[\xe0-\xe7]\xff\xff\xff\x7f',
    'and eax,0x7fffffff (25)':           rb'\x25\xff\xff\xff\x7f',
    'and r64?,0x7fffffff (48 81 /4)':    rb'\x48\x81[\xe0-\xe7]\xff\xff\xff\x7f',
    'or  r32,0x80000000 (81 /1)':        rb'(?:\x41)?\x81[\xc8-\xcf]\x00\x00\x00\x80',
    'or  eax,0x80000000 (0d)':           rb'\x0d\x00\x00\x00\x80',
    'bt/bts/btr r,31 (0f ba /4-6 1f)':   rb'(?:[\x40-\x4f])?\x0f\xba[\xe0-\xf7]\x1f',
    'shr r32,31 (c1 /5 1f)':             rb'(?:\x41)?\xc1[\xe8-\xef]\x1f',
    'movabs r,0xffffffffff (40-bit mask)': rb'[\x48\x49][\xb8-\xbf]\xff\xff\xff\xff\xff\x00\x00\x00',
    'and r64,0xffffffff (mov r32,r32 zext not counted)': rb'\x48\x81[\xe0-\xe7]\xff\xff\xff\xff',
    'shl r64,24 (c1 /4 18) then shr (40-bit field)': rb'[\x48\x49]\xc1[\xe0-\xe7]\x18',
}
for name, pat in IDIOMS.items():
    pages = collections.Counter()
    n = 0
    for m in re.finditer(pat, code, re.S):
        n += 1
        pages[m.start() >> 12] += 1
    top = ', '.join(f'{p << 12:#x}:{c}' for p, c in pages.most_common(4))
    print(f'{n:6d}  {name:52s} {top}')
