#!/usr/bin/env python3
"""Tie native/core/addr to original RVA 0x66dec0 (private; needs the ELF).

Matches (byte-exact apart from rip-relative displacements) the tagged-field decode prologue and one field-retag arm,
and recovers the addresses of the two global cells (heap base, heap limit) from those displacements.
Only these fragments are matched — the 13-way type dispatcher around them is NOT lifted.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)


def match(addr, template_hex, disp_at):
    t = bytes.fromhex(template_hex)
    got = bytearray(img.read(addr, len(t)))
    real = bytes(got)
    got[disp_at:disp_at + 4] = b'\0\0\0\0'
    assert bytes(got) == t, (hex(addr), bytes(got).hex(), t.hex())
    return real


A = 0x66dec0
pro = match(A, '488d0d00000000' '89f2' '31c0' '81e2ffffff7f' '85f6' '488b09' '480f48c1' '4801d0' '0f84d7000000', 3)
base_cell = A + 7 + struct.unpack('<i', pro[3:7])[0]

B = 0x66deff
arm_prefix = '8b5020' '85d2' '0f88b0000000' '89d6' '81e6ffffff7f' '0f84a2000000'
arm = match(B, arm_prefix + '488d3d00000000' '89f6' '4801ce' '483b37' '0f838d000000'
            '81ca00000080' '895020' 'c3', len(bytes.fromhex(arm_prefix)) + 3)
disp_off = len(bytes.fromhex(arm_prefix)) + 3
limit_cell = B + disp_off + 4 + struct.unpack('<i', arm[disp_off:disp_off + 4])[0]

assert base_cell == 0x2f3b8c0, hex(base_cell)
assert limit_cell == 0x2f08000, hex(limit_cell)
print(f'OK: decode prologue @0x66dec0 and retag arm @0x66deff byte-exact; heap base cell {base_cell:#x}, heap limit cell {limit_cell:#x}; '
      f'rule: ptr=(bit31?[base]:0)+(v&0x7fffffff); tag if untagged, nonzero, [base]+off < [limit]')
