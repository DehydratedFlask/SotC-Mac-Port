#!/usr/bin/env python3
"""Tie native/core/init_f4b540 to simulated stores of original RVA 0xf4b540 (private ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xf4b540
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
code = img.read(A, 285)
assert code[-1] == 0xc3

pc = 0
stores = []
xmm = {0: None, 1: None}
rax = rcx = None
rax_val = None

def i32(x):
    return struct.unpack_from('<i', code, x)[0]

while pc < len(code):
    b = code[pc:]
    if b[0] == 0xc3:
        break
    if b[:4] == bytes.fromhex('c5f057c9'):
        xmm[1] = bytes(16); pc += 4; continue
    if b[:3] == bytes.fromhex('488d05'):
        rax = A + pc + 7 + i32(pc + 3); pc += 7; continue
    if b[:3] == bytes.fromhex('488d0d'):
        rcx = A + pc + 7 + i32(pc + 3); pc += 7; continue
    if b[:3] == bytes.fromhex('488b05'):
        rax_val = img.read(A + pc + 7 + i32(pc + 3), 8); pc += 7; continue
    if b[:3] == bytes.fromhex('488905'):
        stores.append((A + pc + 7 + i32(pc + 3), rax_val)); pc += 7; continue
    if b[:4] == bytes.fromhex('c5f81005'):
        xmm[0] = img.read(A + pc + 8 + i32(pc + 4), 16); pc += 8; continue
    if b[:4] == bytes.fromhex('c5fa1005'):
        xmm[0] = img.read(A + pc + 8 + i32(pc + 4), 4) + bytes(12); pc += 8; continue
    if b[:4] == bytes.fromhex('c5f81105'):
        stores.append((A + pc + 8 + i32(pc + 4), xmm[0])); pc += 8; continue
    if b[:4] == bytes.fromhex('c5f8110d'):
        stores.append((A + pc + 8 + i32(pc + 4), xmm[1])); pc += 8; continue
    if b[:4] == bytes.fromhex('c5f81100'):
        stores.append((rax, xmm[0])); pc += 4; continue
    if b[:4] == bytes.fromhex('c5f81101'):
        stores.append((rcx, xmm[0])); pc += 4; continue
    if b[:4] == bytes.fromhex('c5f81305'):
        stores.append((A + pc + 8 + i32(pc + 4), xmm[0][:8])); pc += 8; continue
    if b[:4] == bytes.fromhex('c5f8130d'):
        stores.append((A + pc + 8 + i32(pc + 4), xmm[1][:8])); pc += 8; continue
    raise SystemExit(f'unknown insn at {pc:#x}: {b[:8].hex()}')

lo = min(s[0] for s in stores)
hi = max(s[0] + len(s[1]) for s in stores)
assert lo == 0x3356850 and hi - lo == 240
mem = bytearray(hi - lo)
for dst, val in stores:
    mem[dst - lo:dst - lo + len(val)] = val

exe = ROOT / 'local/build/native/test_init_f4b540'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['len'] == 240 and native['b0'] == mem[0] and native['b32'] == mem[32] and native['b128'] == mem[128]
assert mem[0:4] == bytes.fromhex('00000080')
assert mem[0x20:0x24] == bytes.fromhex('00000034')
assert mem[0x80:0x84] == bytes.fromhex('0000803f')
print(f'OK: 0xf4b540 (idx 6) 20 stores → 240 B @0x3356850; native len/samples match')
