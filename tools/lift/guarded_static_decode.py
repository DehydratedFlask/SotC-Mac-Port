#!/usr/bin/env python3
"""Strict decoder + offline interpreter for the guarded-static helper family (CPU-01, offline analysis only).

Covers 0x51230, 0xa77960, 0x268d40 and the ctors 0x78c760 / 0x268ce0. Nothing from the game is run as a
product runtime: pinned ELF bytes are matched by exact encoding (anything unlisted raises DecodeError) and
interpreted by a ~20-opcode throw-away model so native results are compared with the *machine code*.

Model: rax/rcx/rdx/rdi as 64-bit, one xmm0 (16 bytes), BSS memory = zeros unless pre-seeded, ZF.
Calls: in-image functions are interpreted recursively; PLT thunks (`jmp [rip+d]`) are resolved through the
ELF relocations to a NID and only the two Itanium static-guard NIDs (computed here from the names
__cxa_guard_acquire / __cxa_guard_release) are modelled: acquire returns 1 iff guard byte == 0, release
sets the guard byte to 1 (single thread). Any other import raises.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
sys.path.insert(0, str(ROOT / 'scripts'))
import leaf_ctor_decode as base  # noqa: E402
import ps4_dynlink as dyn  # noqa: E402

DecodeError = base.DecodeError
load_image = base.load_image
ELF_SHA256 = base.ELF_SHA256
M64 = (1 << 64) - 1
STEP_LIMIT = 10_000


def _rip(c, pc, at, length):
    return pc + length + struct.unpack_from('<i', c, at)[0]


def decode_one(image, pc):
    """(length, mnemonic, operand). Exact encodings only."""
    c = image.read(pc, 8)
    if c[0] == 0x55: return 1, 'push rbp', None
    if c[:3] == b'\x48\x89\xe5': return 3, 'mov rbp,rsp', None
    if c[0] == 0x5D: return 1, 'pop rbp', None
    if c[0] == 0xC3: return 1, 'ret', None
    if c[0] == 0xE8: return 5, 'call', pc + 5 + struct.unpack_from('<i', c, 1)[0]
    if c[:2] == b'\x8a\x05': return 6, 'mov al,[rip]', _rip(c, pc, 2, 6)
    if c[:2] == b'\x88\x05': return 6, 'mov [rip],al', _rip(c, pc, 2, 6)
    if c[:2] == b'\x84\xc0': return 2, 'test al,al', None
    if c[:2] == b'\x85\xc0': return 2, 'test eax,eax', None
    if c[0] == 0x75: return 2, 'jne', pc + 2 + struct.unpack_from('<b', c, 1)[0]
    if c[0] == 0x74: return 2, 'je', pc + 2 + struct.unpack_from('<b', c, 1)[0]
    if c[:3] == b'\x48\x8d\x3d': return 7, 'lea rdi,[rip]', _rip(c, pc, 3, 7)
    if c[:3] == b'\x48\x8d\x05': return 7, 'lea rax,[rip]', _rip(c, pc, 3, 7)
    if c[:3] == b'\x48\x8d\x0d': return 7, 'lea rcx,[rip]', _rip(c, pc, 3, 7)
    if c[:3] == b'\x48\x89\x05': return 7, 'mov [rip],rax', _rip(c, pc, 3, 7)
    if c[:3] == b'\x48\x89\x15': return 7, 'mov [rip],rdx', _rip(c, pc, 3, 7)
    if c[:2] == b'\xc6\x05': return 7, 'mov byte [rip],imm8', (_rip(c, pc, 2, 7), c[6])
    if c[:4] == b'\xc5\xf8\x10\x00': return 4, 'vmovups xmm0,[rax]', None
    if c[:4] == b'\xc5\xf8\x11\x01': return 4, 'vmovups [rcx],xmm0', None
    if c[:2] == b'\x31\xc0': return 2, 'xor eax,eax', None
    if c[:2] == b'\x31\xd2': return 2, 'xor edx,edx', None
    raise DecodeError(f'unsupported encoding {c.hex()} at {pc:#x}')


def decode_function(image, rva):
    """Linear walk to the last ret reachable by forward branches. Returns list of (pc, len, mnem, operand)."""
    out, pc, far = [], rva, rva
    for _ in range(200):
        n, name, arg = decode_one(image, pc)
        out.append((pc, n, name, arg))
        if name in ('jne', 'je'):
            if not pc < arg <= rva + 0x100:
                raise DecodeError(f'branch at {pc:#x} is not a short forward branch')
            far = max(far, arg)
        pc += n
        if name == 'ret' and pc > far:
            return out
    raise DecodeError('function too long for the strict decoder')


class Machine:
    def __init__(self, image, got_names, seed=None):
        self.image, self.got_names = image, got_names
        self.mem = dict(seed or {})       # addr -> byte (only bytes touched or seeded)
        self.writes, self.calls = [], []
        self.reg = {'rax': 0xDEAD_BEEF_0000_0000, 'rcx': 0xCAFE_0000_0000_0000, 'rdx': 0xBAAD_0000_0000_0000, 'rdi': 0}
        self.xmm0 = bytes(range(0xF0, 0x100))
        self.zf, self.depth = False, 0

    def rd(self, addr):
        if addr in self.mem:
            return self.mem[addr]
        try:
            return self.image.read(addr, 1)[0]
        except ValueError:
            if self.image.mapped(addr):
                return 0                     # BSS
            raise DecodeError(f'read outside the image at {addr:#x}')

    def wr(self, addr, b):
        if not self.image.mapped(addr):
            raise DecodeError(f'write outside the image at {addr:#x}')
        self.mem[addr] = b & 0xFF
        self.writes.append((addr, b & 0xFF))

    def thunk(self, target):
        code = self.image.read(target, 6)
        if code[:2] != b'\xff\xff'[:0] + b'\xff\x25':
            return None
        slot = target + 6 + struct.unpack_from('<i', code, 2)[0]
        return self.got_names.get(slot)

    def call(self, target):
        name = self.thunk(target)
        if name is not None:
            self.calls.append((target, name))
            guard = self.reg['rdi']
            if name == '__cxa_guard_acquire':
                self.reg['rax'] = 1 if self.rd(guard) == 0 else 0
            elif name == '__cxa_guard_release':
                self.wr(guard, 1)
            else:
                raise DecodeError(f'unmodelled import {name} at {target:#x}')
            return
        self.calls.append((target, 'in-image'))
        self.run(target)

    def run(self, rva):
        self.depth += 1
        if self.depth > 4:
            raise DecodeError('call depth')
        fn = {pc: (n, nm, a) for pc, n, nm, a in decode_function(self.image, rva)}
        pc, steps = rva, 0
        while True:
            steps += 1
            if steps > STEP_LIMIT:
                raise DecodeError('step limit')
            if pc not in fn:
                raise DecodeError(f'jump into the middle of an instruction at {pc:#x}')
            n, name, a = fn[pc]
            nxt, r = pc + n, self.reg
            if name in ('push rbp', 'mov rbp,rsp', 'pop rbp'):
                pass
            elif name == 'ret':
                self.depth -= 1
                return
            elif name == 'call':
                self.call(a)
            elif name == 'mov al,[rip]':
                r['rax'] = (r['rax'] & ~0xFF & M64) | self.rd(a)
            elif name == 'mov [rip],al':
                self.wr(a, r['rax'] & 0xFF)
            elif name == 'test al,al':
                self.zf = (r['rax'] & 0xFF) == 0
            elif name == 'test eax,eax':
                self.zf = (r['rax'] & 0xFFFFFFFF) == 0
            elif name == 'jne':
                nxt = a if not self.zf else nxt
            elif name == 'je':
                nxt = a if self.zf else nxt
            elif name.startswith('lea'):
                r[{'lea rdi,[rip]': 'rdi', 'lea rax,[rip]': 'rax', 'lea rcx,[rip]': 'rcx'}[name]] = a
            elif name in ('mov [rip],rax', 'mov [rip],rdx'):
                v = r['rax' if name.endswith('rax') else 'rdx']
                for i in range(8):
                    self.wr(a + i, (v >> (8 * i)) & 0xFF)
            elif name == 'mov byte [rip],imm8':
                self.wr(a[0], a[1])
            elif name == 'vmovups xmm0,[rax]':
                self.xmm0 = bytes(self.rd(r['rax'] + i) for i in range(16))
            elif name == 'vmovups [rcx],xmm0':
                for i in range(16):
                    self.wr(r['rcx'] + i, self.xmm0[i])
            elif name == 'xor eax,eax':
                r['rax'], self.zf = 0, True
            elif name == 'xor edx,edx':
                r['rdx'], self.zf = 0, True
            else:
                raise DecodeError(f'unmodelled mnemonic {name}')
            pc = nxt


def import_names(elf):
    """GOT slot -> libc++abi name for the two guard NIDs, derived from the ELF relocations + hashed names."""
    p = dyn.parse(elf)
    want = {dyn.nid('__cxa_guard_acquire'): '__cxa_guard_acquire', dyn.nid('__cxa_guard_release'): '__cxa_guard_release'}
    out = {}
    for r in p['relocs']:
        if r['table'] != 'JMPREL':
            continue
        nid = p['symbols'][r['symbol']]['nid']
        out[r['target']] = want.get(nid, f'import:{nid}')
    return out
