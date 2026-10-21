#!/usr/bin/env python3
"""Offline effect tracer for the constructor "registration thunks" (CPU-01-NATIVE-SLOT12-19-01).

This is a verification aid for the pinned private ELF, NOT part of the product: it executes a tiny,
fixed subset of the original x86-64 bytes on a throw-away abstract machine so that a checker can
state, from the bytes, exactly which externally visible effects one thunk has. Nothing here is
linked into, shipped with, or called by the native game core.

Model
-----
* 64-bit registers, ZF/SF/CF/OF flags, a sparse byte memory.
* Stack memory must be written before it is read; reading a stack byte that nothing wrote raises
  `Fault` (so a thunk cannot silently depend on stack garbage).
* Image (file-backed) memory is read from the ELF. The one imported pointer the thunks read, the
  stack-protector GOT word, is pre-seeded with a fake guard variable.
* Imports are intercepted BY NAME (resolved from the ELF relocations, not hard-coded addresses):
  malloc, __cxa_atexit, memset, __stack_chk_fail (a Fault when reached).
* The pthread mutex imports used by the shared initializer `0xb30950` are intercepted by name and
  recorded with their arguments, so the recursive (2) + priority-inherit (1) attribute values are
  read from the bytes rather than assumed. `0xb30950` itself is executed from its bytes.
* Every other in-image function reached by a direct or indirect call is executed from its bytes.
* Every store outside the stack and the fake heap is an event. Any call through an owner vtable
  (`call [rax+0xc0]` ...) that is actually executed is an event too; a null owner would fault.
Unknown mnemonics, unmodelled operands and unexpected control flow raise `Fault`; the tracer
never guesses.
"""
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))

STACK_TOP = 0x7ffe00100000
STACK_LO = 0x7ffe00000000
HEAP_BASE = 0x500000000000
GUARD_GOT = 0x15fe518          # pointer to the stack-protector value, read by every thunk
GUARD_ADDR = 0x600000000000
GUARD_VALUE = 0x1122334455667788
SENTINEL = 0x7fff00000000      # return address of the traced function
MASK64 = (1 << 64) - 1
MAX_STEPS = 50000


class Fault(Exception):
    pass


REGS64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi'] + [f'r{i}' for i in range(8, 16)]
SUB = {}
for _i, _n in enumerate(['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi']):
    SUB[_n] = (_n, 64)
    SUB['e' + _n[1:]] = (_n, 32)
    SUB[_n[1:]] = (_n, 16)
    SUB[{'ax': 'al', 'cx': 'cl', 'dx': 'dl', 'bx': 'bl', 'sp': 'spl', 'bp': 'bpl', 'si': 'sil', 'di': 'dil'}[_n[1:]]] = (_n, 8)
for _i in range(8, 16):
    SUB[f'r{_i}'] = (f'r{_i}', 64)
    SUB[f'r{_i}d'] = (f'r{_i}', 32)
    SUB[f'r{_i}w'] = (f'r{_i}', 16)
    SUB[f'r{_i}b'] = (f'r{_i}', 8)

SIZES = {'byte': 1, 'word': 2, 'dword': 4, 'qword': 8, 'xmmword': 16, 'ymmword': 32}
JCC = {
    'je': lambda f: f['ZF'], 'jne': lambda f: not f['ZF'],
    'jl': lambda f: f['SF'] != f['OF'], 'jge': lambda f: f['SF'] == f['OF'],
    'jle': lambda f: f['ZF'] or f['SF'] != f['OF'], 'jg': lambda f: not f['ZF'] and f['SF'] == f['OF'],
    'jb': lambda f: f['CF'], 'jae': lambda f: not f['CF'],
    'jbe': lambda f: f['CF'] or f['ZF'], 'ja': lambda f: not f['CF'] and not f['ZF'],
    'js': lambda f: f['SF'], 'jns': lambda f: not f['SF'],
}


def to_int(text):
    text = text.strip()
    neg = text.startswith('-')
    value = int(text[1:] if neg else text, 0)
    return -value if neg else value


class Tracer:
    def __init__(self, image, instr_at, thunk_name_at):
        self.image = image
        self.instr_at = instr_at
        self.thunk_name_at = thunk_name_at          # callable(addr) -> import name or None
        self.reset()

    # ---- state ----
    def reset(self):
        self.reg = {r: 0xAAAAAAAA00000000 + i for i, r in enumerate(REGS64)}
        self.vec = {}
        self.flags = {'ZF': False, 'SF': False, 'CF': False, 'OF': False}
        self.mem = {}
        self.events = []        # externally visible effects, in order
        self.calls = []         # (kind, from, to, (rdi, rsi, rdx, rcx)) executed calls/tail jumps, in order
        self.heap_next = HEAP_BASE
        self.steps = 0
        self.reg['rsp'] = STACK_TOP
        self._seed(GUARD_GOT, GUARD_ADDR, 8)
        self._seed(GUARD_ADDR, GUARD_VALUE, 8)

    def _seed(self, addr, value, size):
        for i in range(size):
            self.mem[addr + i] = (value >> (8 * i)) & 0xff

    # ---- memory ----
    def _is_stack(self, addr):
        return STACK_LO <= addr < STACK_TOP + 0x1000

    def _is_heap(self, addr):
        return HEAP_BASE <= addr < HEAP_BASE + 0x100000

    def read(self, addr, size):
        out = 0
        for i in range(size):
            a = addr + i
            if a in self.mem:
                b = self.mem[a]
            elif self._is_stack(a):
                raise Fault(f'read of uninitialised stack byte {a:#x}')
            elif self._is_heap(a):
                raise Fault(f'read of uninitialised heap byte {a:#x}')
            else:
                try:
                    b = self.image.read(a, 1)[0]
                except ValueError:
                    raise Fault(f'read of unmapped address {a:#x}')
            out |= b << (8 * i)
        return out

    def write(self, addr, size, value, rip):
        value &= (1 << (8 * size)) - 1
        for i in range(size):
            self.mem[addr + i] = (value >> (8 * i)) & 0xff
        if not (self._is_stack(addr) or self._is_heap(addr)):
            self.events.append(('store', addr, size, value, rip))

    # ---- registers ----
    def getr(self, name):
        base, bits = SUB[name]
        return self.reg[base] & ((1 << bits) - 1)

    def setr(self, name, value):
        base, bits = SUB[name]
        value &= (1 << bits) - 1
        if bits == 64:
            self.reg[base] = value
        elif bits == 32:
            self.reg[base] = value                    # zero-extends
        else:
            keep = self.reg[base] & ~((1 << bits) - 1)
            self.reg[base] = keep | value

    # ---- operands ----
    def ea(self, text, nxt):
        inner = text[text.index('[') + 1:text.rindex(']')]
        total = 0
        for sign, term in re.findall(r'([+-])\s*([^+\-\s]+)', '+ ' + inner):
            if term == 'rip':
                value = nxt
            elif '*' in term:
                scale, regname = term.split('*')
                value = int(scale) * self.getr(regname)
            elif term in SUB:
                value = self.getr(term)
            else:
                value = int(term, 0)
            total += value if sign == '+' else -value
        return total & MASK64

    @staticmethod
    def size_of(text):
        m = re.match(r'(byte|word|dword|qword|xmmword|ymmword) ptr', text)
        return SIZES[m.group(1)] if m else None

    def get(self, text, nxt, size_hint=None):
        text = text.strip()
        if '[' in text:
            size = self.size_of(text) or size_hint
            if size is None:
                raise Fault(f'memory operand without size: {text}')
            return self.read(self.ea(text, nxt), size), size
        if text in SUB:
            return self.getr(text), SUB[text][1] // 8
        return to_int(text), size_hint

    def put(self, text, nxt, value, rip):
        text = text.strip()
        if '[' in text:
            size = self.size_of(text)
            if size is None:
                raise Fault(f'store without size: {text}')
            self.write(self.ea(text, nxt), size, value, rip)
        elif text in SUB:
            self.setr(text, value)
        else:
            raise Fault(f'unsupported destination {text}')

    def set_flags_sub(self, a, b, bits, carry_in=0):
        mask = (1 << bits) - 1
        a &= mask
        b &= mask
        res = (a - b) & mask
        sign = 1 << (bits - 1)
        self.flags['ZF'] = res == 0
        self.flags['SF'] = bool(res & sign)
        self.flags['CF'] = a < b
        self.flags['OF'] = bool((a ^ b) & (a ^ res) & sign)
        return res

    def set_flags_logic(self, res, bits):
        mask = (1 << bits) - 1
        res &= mask
        self.flags['ZF'] = res == 0
        self.flags['SF'] = bool(res & (1 << (bits - 1)))
        self.flags['CF'] = False
        self.flags['OF'] = False
        return res

    # ---- stack ----
    def push(self, value, rip):
        self.reg['rsp'] = (self.reg['rsp'] - 8) & MASK64
        self.write(self.reg['rsp'], 8, value, rip)

    def pop(self):
        value = self.read(self.reg['rsp'], 8)
        self.reg['rsp'] = (self.reg['rsp'] + 8) & MASK64
        return value

    # ---- imports ----
    def intercept_import(self, name, rip):
        r = self.reg
        if name == 'malloc':
            size = r['rdi'] & 0xffffffff
            self.events.append(('malloc', size))
            r['rax'] = self.heap_next
            self.heap_next += (size + 0xf) & ~0xf or 0x10
            return
        if name == '__cxa_atexit':
            self.events.append(('atexit', r['rdi'], r['rsi'], r['rdx']))
            r['rax'] = 0
            return
        if name == 'memset':
            dst, val, n = r['rdi'], r['rsi'] & 0xff, r['rdx']
            for i in range(n):
                self.write(dst + i, 1, val, rip)
            self.events.append(('memset', dst, val, n))
            r['rax'] = dst
            return
        if name in ('pthread_mutexattr_init', 'pthread_mutexattr_destroy'):
            self.events.append((name, r['rdi']))
            r['rax'] = 0
            return
        if name in ('pthread_mutexattr_settype', 'pthread_mutexattr_setprotocol'):
            self.events.append((name, r['rdi'], r['rsi'] & 0xffffffff))
            r['rax'] = 0
            return
        if name == 'pthread_mutex_init':
            self.events.append((name, r['rdi'], r['rsi']))
            r['rax'] = 0
            return
        if name == '__stack_chk_fail':
            raise Fault('stack protector failure path executed')
        raise Fault(f'unmodelled import {name!r} reached at {rip:#x}')

    def args(self):
        r = self.reg
        return (r['rdi'], r['rsi'], r['rdx'], r['rcx'])

    # ---- driver ----
    def run(self, entry, args=None, expect_pinned=None):
        """Run `entry` to its return. `args` maps register -> value. Returns the machine."""
        for k, v in (args or {}).items():
            self.reg[k] = v
        initial_rsp = self.reg['rsp']
        entry_regs = dict(self.reg)
        self.push(SENTINEL, entry)
        pc = entry
        while True:
            self.steps += 1
            if self.steps > MAX_STEPS:
                raise Fault('step limit exceeded')
            if pc == SENTINEL:
                break
            name = self.thunk_name_at(pc)
            if name is not None:
                self.intercept_import(name, pc)
                pc = self.pop()
                continue
            ins = self.instr_at(self.image, pc)
            if ins is None:
                raise Fault(f'undecodable instruction at {pc:#x}')
            pc = self.step(ins)
        # Post-conditions: stack balanced and callee-saved registers restored.
        if self.reg['rsp'] != initial_rsp:
            raise Fault(f'unbalanced stack: rsp {self.reg["rsp"]:#x} != {initial_rsp:#x}')
        for name in ('rbx', 'rbp', 'r12', 'r13', 'r14', 'r15'):
            if self.reg[name] != entry_regs[name]:
                raise Fault(f'callee-saved {name} not restored')
        return self

    def step(self, ins):
        addr = ins['addr']
        nxt = addr + len(ins['bytes'])
        mn = ins['mnemonic']
        ops = re.sub(r'\s*#.*$', '', ins['operands']).strip()
        ops = re.sub(r'\s*<[^>]*>$', '', ops).strip()
        r = self.reg

        def split2(s):
            depth = 0
            for i, ch in enumerate(s):
                if ch == '[':
                    depth += 1
                elif ch == ']':
                    depth -= 1
                elif ch == ',' and depth == 0:
                    return s[:i].strip(), s[i + 1:].strip()
            raise Fault(f'cannot split operands {s!r}')

        if mn == 'nop':
            return nxt
        if mn == 'push':
            value, _ = self.get(ops, nxt, 8)
            self.push(value, addr)
            return nxt
        if mn == 'pop':
            self.put(ops, nxt, self.pop(), addr)
            return nxt
        if mn in ('mov', 'movabs'):
            dst, src = split2(ops)
            size = self.size_of(dst) or (SUB[dst][1] // 8 if dst in SUB else None)
            value, _ = self.get(src, nxt, size)
            if '[' in dst and '[' not in src and src not in SUB and size == 8:
                value &= MASK64        # imm32 sign-extension is already in to_int for negatives
            self.put(dst, nxt, value, addr)
            return nxt
        if mn == 'movsxd':
            dst, src = split2(ops)
            value, _ = self.get(src, nxt, 4)
            if value & 0x80000000:
                value |= 0xffffffff00000000
            self.put(dst, nxt, value, addr)
            return nxt
        if mn == 'lea':
            dst, src = split2(ops)
            self.put(dst, nxt, self.ea(src, nxt), addr)
            return nxt
        if mn in ('add', 'sub', 'and', 'xor', 'cmp', 'test'):
            dst, src = split2(ops)
            dsize = self.size_of(dst) or (SUB[dst][1] // 8 if dst in SUB else None)
            bits = dsize * 8
            a, _ = self.get(dst, nxt, dsize)
            b, _ = self.get(src, nxt, dsize)
            mask = (1 << bits) - 1
            if mn == 'cmp':
                self.set_flags_sub(a, b, bits)
            elif mn == 'test':
                self.set_flags_logic(a & b, bits)
            elif mn == 'sub':
                self.put(dst, nxt, self.set_flags_sub(a, b & mask, bits), addr)
            elif mn == 'add':
                res = (a + (b & mask)) & mask
                sign = 1 << (bits - 1)
                self.flags.update(ZF=res == 0, SF=bool(res & sign), CF=(a + (b & mask)) > mask,
                                  OF=bool(~(a ^ b) & (a ^ res) & sign))
                self.put(dst, nxt, res, addr)
            elif mn == 'and':
                self.put(dst, nxt, self.set_flags_logic(a & (b & mask), bits), addr)
            else:
                self.put(dst, nxt, self.set_flags_logic(a ^ b, bits), addr)
            return nxt
        if mn == 'inc':
            dsize = self.size_of(ops) or SUB[ops][1] // 8
            bits = dsize * 8
            a, _ = self.get(ops, nxt, dsize)
            cf = self.flags['CF']
            res = (a + 1) & ((1 << bits) - 1)
            sign = 1 << (bits - 1)
            self.flags.update(ZF=res == 0, SF=bool(res & sign), OF=a == sign - 1, CF=cf)
            self.put(ops, nxt, res, addr)
            return nxt
        if mn == 'vxorps':
            dst, a, b = [x.strip() for x in ops.split(',')]
            if not (dst == a == b):
                raise Fault('vxorps only modelled as a zeroing idiom')
            self.vec[dst] = 0
            return nxt
        if mn == 'vmovups':
            dst, src = split2(ops)
            if '[' in dst:
                width = 32 if src.startswith('ymm') else 16
                value = self.vec.get(src)
                if value is None:
                    raise Fault(f'vector register {src} read before it was set')
                a = self.ea(dst, nxt)
                for i in range(width):
                    self.write(a + i, 1, (value >> (8 * i)) & 0xff, addr)
            else:
                width = 32 if dst.startswith('ymm') else 16
                self.vec[dst] = self.read(self.ea(src, nxt), width)
            return nxt
        if mn in JCC:
            target = to_int(ops.split()[0])
            return target if JCC[mn](self.flags) else nxt
        if mn == 'jmp':
            if ops.startswith('0x'):
                target = to_int(ops.split()[0])
                name = self.thunk_name_at(target)
                if name is not None:
                    self.calls.append(('tailjmp-import', addr, name, self.args()))
                    self.intercept_import(name, target)
                    return self.pop()
                self.calls.append(('tailjmp', addr, target, self.args()))
                return target
            target, _ = self.get(ops, nxt, 8)
            self.calls.append(('tailjmp-indirect', addr, target, self.args()))
            return target
        if mn == 'call':
            if ops.startswith('0x'):
                target = to_int(ops.split()[0])
                kind = 'call'
            else:
                target, _ = self.get(ops, nxt, 8)
                kind = 'call-indirect'
            self.calls.append((kind, addr, target, self.args()))
            self.push(nxt, addr)
            return target
        if mn == 'ret':
            return self.pop()
        raise Fault(f'unmodelled instruction {mn} {ops!r} at {addr:#x}')


def make_tracer(analyzer, image, name_by_nid, nid_at):
    import struct as _s

    def thunk_name_at(addr):
        try:
            head = image.read(addr, 6)
        except ValueError:
            return None
        if head[:2] != b'\xff\x25':
            return None
        slot = addr + 6 + _s.unpack('<i', head[2:6])[0]
        nid = nid_at.get(slot)
        return name_by_nid.get(nid) if nid else None

    return Tracer(image, analyzer.instr_at, thunk_name_at)
