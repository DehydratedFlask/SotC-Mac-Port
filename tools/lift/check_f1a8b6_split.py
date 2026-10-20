#!/usr/bin/env python3
"""Prove OBS free-region split surgery at 0x1a8b6..0x1a8fa (private ELF)."""
import re, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

def match(addr, template):
    toks = re.findall(r'dddddddd|[0-9a-fA-F]{2}', template.replace(' ', ''))
    size = sum(4 if t.lower() == 'dddddddd' else 1 for t in toks)
    code = img.read(addr, size)
    pos, disps = 0, []
    for t in toks:
        if t.lower() == 'dddddddd':
            disps.append(struct.unpack_from('<i', code, pos)[0]); pos += 4
        else:
            b = int(t, 16)
            assert code[pos] == b, (hex(addr + pos), f'{code[pos]:02x}', t)
            pos += 1
    return disps

disps = match(0x1a8b1, 'e8 dddddddd')
assert 0x1a8b1 + 5 + disps[0] == 0x19660
match(0x1a8b6,
      '49 8b 74 24 60 49 8b 4c 24 58 41 8a 54 24 68 4c 01 f6 4c 29 f1'
      '88 50 68 48 89 48 58 48 89 70 60 c6 40 69 00'
      '48 c7 40 40 00 00 00 00 c7 40 48 ff ff ff ff'
      '49 8b 4c 24 58 4c 29 f1 48 89 48 50'
      '4d 89 74 24 58 4d 89 74 24 50')
match(0x19660, '55 48 89 e5 41 56 53 48 83 ec 10')
match(0x197ac, '48 83 c0 70')
print('OK: 0x1a8b6 split surgery — neu fields + free.size/cap=lead; call 0x19660; stride 0x70')
