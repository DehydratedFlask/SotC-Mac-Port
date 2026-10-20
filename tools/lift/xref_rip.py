#!/usr/bin/env python3
"""List rip-relative references (modrm mod=00 rm=101) to given absolute addresses in the code segment.

Heuristic scan: the disp32 position is accepted when the previous byte is a rip-relative modrm; the instruction
end is tried as disp+0/+1/+4 (no/imm8/imm32 trailing). Output shows the 6 bytes before the disp for opcode
inspection (stores vs loads). Misses indirect uses; may include rare false positives.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

targets = [int(a, 0) for a in sys.argv[1:]]
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
cp = next(p for p in ph if p['type'] == d.PT_LOAD and p['flags'] & 1)
code = elf[cp['offset']:cp['offset'] + cp['filesz']]
hits = {t: [] for t in targets}
for i in range(1, len(code) - 4):
    if (code[i - 1] & 0xC7) != 0x05:
        continue
    disp = struct.unpack_from('<i', code, i)[0]
    for extra in (0, 1, 4):
        tgt = i + 4 + extra + disp
        if tgt in hits:
            hits[tgt].append((i, extra, code[max(0, i - 6):i].hex()))
for t, h in hits.items():
    print(f'{t:#x}: {len(h)} refs')
    for i, extra, ctx in h:
        print(f'   disp@{i:#x} +{extra} ctx={ctx}')
