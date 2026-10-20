#!/usr/bin/env python3
"""Tie native/core/init_ea52f0 to simulated stores of original RVA 0xea52f0 (private ELF)."""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xea52f0
elf = (ROOT / 'local/re/eboot.elf').read_bytes()
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
code = img.read(A, 102)
assert code[-1] == 0xc3

pc = 0
ymm = {}
xmm = {}
rax = None
stores = []

def i32(o):
    return struct.unpack_from('<i', code, o)[0]

while pc < len(code):
    b = code[pc:]
    if b[0] == 0xc3:
        break
    if b[:4] == bytes.fromhex('c5fc100d'):
        ymm[1] = img.read(A + pc + 8 + i32(pc + 4), 32); pc += 8; continue
    if b[:4] == bytes.fromhex('c5fc1015'):
        ymm[2] = img.read(A + pc + 8 + i32(pc + 4), 32); pc += 8; continue
    if b[:3] == bytes.fromhex('488d05'):
        rax = A + pc + 7 + i32(pc + 3); pc += 7; continue
    if b[:4] == bytes.fromhex('c5f81005'):
        xmm[0] = img.read(A + pc + 8 + i32(pc + 4), 16); pc += 8; continue
    if b[:4] == bytes.fromhex('c5fc1108'):
        stores.append((rax, ymm[1])); pc += 4; continue
    if b[:5] == bytes.fromhex('c5fc115020'):
        stores.append((rax + 0x20, ymm[2])); pc += 5; continue
    if b[:4] == bytes.fromhex('c5f81105'):
        stores.append((A + pc + 8 + i32(pc + 4), xmm[0])); pc += 8; continue
    if b[:5] == bytes.fromhex('c5fc114840'):
        stores.append((rax + 0x40, ymm[1])); pc += 5; continue
    if b[:5] == bytes.fromhex('c5fc115060'):
        stores.append((rax + 0x60, ymm[2])); pc += 5; continue
    if b[:4] == bytes.fromhex('c5fc1188'):
        stores.append((rax + struct.unpack_from('<i', code, pc + 4)[0], ymm[1])); pc += 8; continue
    if b[:3] == bytes.fromhex('48c780'):
        off = struct.unpack_from('<i', code, pc + 3)[0]
        imm = struct.unpack_from('<i', code, pc + 7)[0]
        stores.append((rax + off, struct.pack('<Q', imm & 0xffffffff))); pc += 11; continue
    raise SystemExit(f'unknown insn at {pc:#x}: {b[:8].hex()}')

lo = min(a for a, _ in stores)
hi = max(a + len(v) for a, v in stores)
assert lo == 0x3354020 and hi - lo == 184
mem = bytearray(hi - lo)
for dst, val in stores:
    mem[dst - lo:dst - lo + len(val)] = val

exe = ROOT / 'local/build/native/test_init_ea52f0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['len'] == 184 and native['b0'] == mem[0] and native['b16'] == mem[16] and native['b176'] == mem[176]
assert mem[0:16] == bytes.fromhex('ffffffffffffffffffffffff00000000')
assert mem[16:24] == struct.pack('<Q', 0x0c)
assert mem[176:184] == struct.pack('<Q', 0x5c)
print('OK: 0xea52f0 (idx 22) stores → 184 B @0x3354020; native len/samples match')
