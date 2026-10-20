#!/usr/bin/env python3
"""Tie native/core/init_f4fd40 to original RVA 0xf4fd40 (private; needs the ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xf4fd40
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = img.read(A, 45)
assert body[-1] == 0xc3 and len(body) == 45

def rip(disp_at, end):
    return A + end + struct.unpack_from('<i', body, disp_at)[0]

# Insn layout (OBS):
# 0: vpcmpeqd xmm0,xmm0,xmm0
# 4: vmovups xmm1,[rip]          ; epsilon source
# 12: vmovq  [0x3356980],xmm0
# 20: vmovss xmm0,[rip]
# 28: vmovups [0x3356990],xmm1
# 36: vmovlps [0x3356988],xmm0
assert rip(16, 20) == 0x3356980
assert rip(32, 36) == 0x3356990
assert rip(40, 44) == 0x3356988
eps = struct.unpack('<4I', img.read(rip(8, 12), 16))
assert list(eps) == [0x34000000] * 4

exe = ROOT / 'local/build/native/test_init_f4fd40'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['ones0'] == (1 << 64) - 1
assert native['ones1'] == (1 << 64) - 1
assert native['eps0'] == 0x34000000
print('OK: 0xf4fd40 (idx 4) ones@ 0x3356980/0x3356988, eps@ 0x3356990; native dump matches')
