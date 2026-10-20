#!/usr/bin/env python3
"""Rank prologue-bearing constructors by liftability: size, call targets, indirect calls.

Disassembles all candidate bodies in ONE llvm-objdump run (text is parsed, nothing executed).
Function end heuristic: first ret/ud2/jmp at or beyond the furthest forward branch target seen.
Calls are classified: PLT thunk -> import name (via imports.json), else internal address.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

CHUNK = 0x600
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
imports = json.loads((ROOT / 'local/re/imports.json').read_text())
name_by_nid = {i['nid']: i.get('name') for i in imports['imports']}
parsed = d.parse(elf)
got = {}
for r in parsed['relocs']:
    if r['type'] in (1, 6, 7) and r['symbol'] < len(parsed['symbols']):
        got[r['target']] = parsed['symbols'][r['symbol']]['nid']

ctors = [int(e['target'], 16) for e in imports['constructors']['entries']]
work = ROOT / 'local/re/work'
blob = bytearray()
for a in ctors:
    try:
        blob += img.read(a, CHUNK).ljust(CHUNK, b'\xcc')
    except ValueError:
        blob += b'\xcc' * CHUNK
(work / 'ctors.bin').write_bytes(blob)
text = subprocess.run([str(ROOT / 'scripts/disasm_range.sh'), str(work / 'ctors.bin'), '0'],
                      check=True, capture_output=True, text=True).stdout
ins = {}
for line in text.splitlines():
    m = re.match(r'\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*\t(.*)', line)
    if m:
        ins[int(m.group(1), 16)] = (len(m.group(2).split()), m.group(3).strip())


def resolve_thunk(addr):
    try:
        c = img.read(addr, 6)
    except ValueError:
        return None
    if c[:2] == b'\xff\x25':
        slot = addr + 6 + int.from_bytes(c[2:6], 'little', signed=True)
        nid = got.get(slot)
        return name_by_nid.get(nid) or nid
    return None


rows = []
for i, a in enumerate(ctors):
    base, pc, far, size = i * CHUNK, i * CHUNK, 0, 0
    calls, indirect, ops, end = [], 0, [], None
    while pc < base + CHUNK and pc in ins:
        n, text_ = ins[pc]
        ops.append(text_)
        mnem = text_.split()[0]
        m = re.search(r'\b(?:j\w+|call)\s+0x([0-9a-f]+)', text_)
        if m and mnem != 'call':
            far = max(far, int(m.group(1), 16))
        if mnem == 'call':
            if m:
                tgt = a + int(m.group(1), 16) - base
                calls.append((hex(tgt), resolve_thunk(tgt)))
            else:
                indirect += 1
        pc += n
        if (mnem in ('ret', 'ud2') or mnem == 'jmp') and pc > far:
            end = pc
            break
    rows.append(dict(ctor=hex(a), bytes=(end - base) if end else None, calls=calls, indirect_calls=indirect,
                     first=ops[:2]))
(work / 'ctor-survey.json').write_text(json.dumps(rows, indent=0))
good = [r for r in rows if r['bytes'] and r['first'] and r['first'][0].startswith('push\trbp')]
good.sort(key=lambda r: r['bytes'])
for r in good[:25]:
    print(r['ctor'], r['bytes'], 'ind', r['indirect_calls'], [(c[1] or c[0]) for c in r['calls']])
print(len(good), 'prologue ctors with detected end')
