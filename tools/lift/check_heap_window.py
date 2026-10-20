#!/usr/bin/env python3
"""Prove how the Tagged31 heap window (base/limit cells) is produced in the original (private; needs the ELF).

Verifies, byte-exact apart from rip-relative displacements ('dd' x4 in templates):
  * 0x55fe4a: region start/size -> cells 0x2f07ff8 (start) and 0x2f08000 (end = start + size)
  * 0x55fe8a: copy-out into the Region struct passed in rdi (+0x30 start, +0x38 end, +0x20/+0x28 zeros)
  * 0x57d590: that struct lives at 0x2f3b8c8, so +0x30/+0x38 are 0x2f3b8f8/0x2f3b900
  * 0x57d5a8: heap base cell 0x2f3b8c0 = [0x2f3b8f8] & 0xffffffff80000000
"""
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)


def match(addr, template):
    """template: hex with 'dddddddd' for rip disp32. Returns list of absolute targets, in order."""
    pos, out, raw = 0, [], b''
    toks = re.findall(r'dddddddd|[0-9a-f]{2}', template.replace(' ', ''))
    size = sum(4 if t == 'dddddddd' else 1 for t in toks)
    code = img.read(addr, size)
    for t in toks:
        if t == 'dddddddd':
            disp = struct.unpack_from('<i', code, pos)[0]
            out.append(('disp', pos, disp))
            pos += 4
        else:
            assert code[pos] == int(t, 16), (hex(addr + pos), code[pos], t)
            pos += 1
    # resolve each disp against the end of its instruction: caller supplies insn ends below
    return out, addr


def target(addr, pos, disp, insn_end_off):
    return addr + insn_end_off + disp


# --- region -> start/end cells
out, a = match(0x55fe4a, '48 8b 48 60  488d15 dddddddd  488d35 dddddddd  4c89f7  48890a  488d15 dddddddd  48034858  48890e')
start_cell = target(a, *out[0][1:], insn_end_off=0x55fe55 - a)
end_cell = target(a, *out[1][1:], insn_end_off=0x55fe5c - a)
zero_cell = target(a, *out[2][1:], insn_end_off=0x55fe69 - a)
assert (start_cell, end_cell, zero_cell) == (0x2f07ff8, 0x2f08000, 0x2f08008), tuple(map(hex, (start_cell, end_cell, zero_cell)))

# --- copy-out into the Region struct (rbx)
out, a = match(0x55fe8a, '488d05 dddddddd  488d15 dddddddd  488d0d dddddddd  c5fc57c0  488b00  48894320  488b02  488d15 dddddddd  48894328  488b01')
z0 = target(a, *out[0][1:], insn_end_off=0x55fe91 - a)
z1 = target(a, *out[1][1:], insn_end_off=0x55fe98 - a)
st = target(a, *out[2][1:], insn_end_off=0x55fe9f - a)
en = target(a, *out[3][1:], insn_end_off=0x55feb4 - a)
assert (z0, z1, st, en) == (0x2f08008, 0x2f08010, 0x2f07ff8, 0x2f08000), tuple(map(hex, (z0, z1, st, en)))
match(0x55fec5, '48894330  488b02  48894338')          # [rbx+0x30] = start ; [rbx+0x38] = end
assert img.read(0x55fd56, 3) == bytes.fromhex('4889fb'), 'mov rbx,rdi'

# --- struct location
out, a = match(0x57d590, '488d3d dddddddd  e9 dddddddd')
region = target(a, *out[0][1:], insn_end_off=7)
jmp = a + 12 + struct.unpack_from('<i', img.read(0x57d598, 4))[0]
assert region == 0x2f3b8c8 and jmp == 0x55fd30, (hex(region), hex(jmp))

# --- base cell
out, a = match(0x57d5a8, '488d0d dddddddd  48c7c000000080  482305 dddddddd  488901')
base_cell = target(a, *out[0][1:], insn_end_off=7)
p_cell = target(a, *out[1][1:], insn_end_off=7 + 7 + 7)
assert base_cell == 0x2f3b8c0 and p_cell == region + 0x30 == 0x2f3b8f8, (hex(base_cell), hex(p_cell))

print('OK: Region struct @0x2f3b8c8 {+0x30 start, +0x38 end} filled from allocator region (start=R[0x60], end=start+R[0x58]); '
      'heap base cell 0x2f3b8c0 = [0x2f3b8f8] & ~0x7fffffff; limit cell 0x2f08000 = region end')
