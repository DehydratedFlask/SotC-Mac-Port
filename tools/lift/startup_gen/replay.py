"""Symbolic STORE-REPLAY tracker for the long straight-line table initialisers (batch 18: startup slot 424 / 0x551140).

It reads the disassembly of ONE function (objdump text over the pinned ELF) and replays a bounded straight-line instruction range symbolically:
registers hold either a 64-bit constant ('c', n) or an address ('a', abs); xmm registers hold two such 64-bit lanes. Every store to memory through
an address register is recorded as (instruction address, instruction bytes, absolute target, width, lane values) in program order.

Fail-closed: an instruction class that is not listed below, an operand form that is not understood, a branch / call / ret inside the range, a load
from memory, a store through a register whose value is not an address, a store whose value register is undefined, or a vector shuffle that does not keep
64-bit lanes whole raises ReplayRefused. Nothing is skipped, nothing is guessed. The objdump comment of vpunpcklqdq / vpshufd ("xmm0 = xmm1[0],xmm0[0]")
is re-parsed and compared with the lane semantics implemented here, so the instruction semantics are pinned by an independent text of the same bytes.

Nothing is executed: the tracker reads bytes and text only.
"""
import re

M64 = (1 << 64) - 1


class ReplayRefused(Exception):
    pass


GPR64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
GPR32 = {'eax': 'rax', 'ecx': 'rcx', 'edx': 'rdx', 'ebx': 'rbx', 'esi': 'rsi', 'edi': 'rdi',
         **{'r%dd' % i: 'r%d' % i for i in range(8, 16)}}
XMM = ['xmm%d' % i for i in range(16)]

MEM = re.compile(r'^(-?0x[0-9a-f]+)?\((%[a-z0-9]+)\)$')
IMM = re.compile(r'^\$(-?0x[0-9a-f]+)$')
REG = re.compile(r'^%([a-z0-9]+)$')
SUF = {'movb': 1, 'movw': 2, 'movl': 4, 'movq': 8}
COMMENT = re.compile(r'#\s*(xmm\d+) = (.*)$')


def split_ops(ops):
    ops = ops.split('#')[0].strip()
    out, depth, cur = [], 0, ''
    for ch in ops:
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


class Replay:
    def __init__(self, seed=None):
        self.g = {}            # 64-bit register name -> value
        self.x = {}            # xmm name -> (lo, hi)
        self.stores = []       # records
        self.nins = 0
        for k, v in (seed or {}).items():
            self.g[k] = v

    # ---- helpers
    def _reg(self, name):
        m = REG.match(name)
        if not m:
            raise ReplayRefused('operand is not a register: %r' % name)
        r = m.group(1)
        if r in GPR32:
            return GPR32[r], 32
        if r in GPR64:
            return r, 64
        if r in XMM:
            return r, 'x'
        raise ReplayRefused('unknown register %r' % name)

    def _gval(self, name, ins):
        r, w = self._reg(name)
        if w == 'x':
            raise ReplayRefused('xmm where a general register is required @%x' % ins['addr'])
        if r not in self.g:
            raise ReplayRefused('read of an undefined register %s @%x' % (r, ins['addr']))
        v = self.g[r]
        if w == 32:
            if v[0] != 'c':
                raise ReplayRefused('32-bit read of an address @%x' % ins['addr'])
            return ('c', v[1] & 0xffffffff)
        return v

    def _xval(self, name, ins):
        r, w = self._reg(name)
        if w != 'x' or r not in self.x:
            raise ReplayRefused('xmm register %s is not defined @%x' % (name, ins['addr']))
        return self.x[r]

    def _mem_target(self, op, ins):
        m = MEM.match(op)
        if not m:
            raise ReplayRefused('memory operand form not understood: %r @%x' % (op, ins['addr']))
        disp = int(m.group(1), 16) if m.group(1) else 0
        r, w = self._reg(m.group(2))
        if w != 64 or r not in self.g or self.g[r][0] != 'a':
            raise ReplayRefused('store through a register that is not a tracked address: %r @%x' % (op, ins['addr']))
        return self.g[r][1] + disp

    def _store(self, ins, target, width, lanes):
        self.stores.append(dict(addr=ins['addr'], bytes=ins['bytes'], target=target, width=width, lanes=lanes))

    # ---- one instruction
    def step(self, ins):
        self.nins += 1
        mn = ins['mnem']
        ops = split_ops(ins['ops'])
        if mn in ('leaq',):
            src, dst = ops
            d, w = self._reg(dst)
            assert w == 64
            if ins['rip'] is not None:
                self.g[d] = ('a', ins['rip'])
                return
            m = MEM.match(src)
            if not m:
                raise ReplayRefused('lea source form not understood @%x' % ins['addr'])
            disp = int(m.group(1), 16) if m.group(1) else 0
            r, rw = self._reg(m.group(2))
            if r not in self.g:
                raise ReplayRefused('lea of an undefined register @%x' % ins['addr'])
            v = self.g[r]
            self.g[d] = (v[0], (v[1] + disp) & M64)
            return
        if mn in SUF and len(ops) == 2:
            width = SUF[mn]
            src, dst = ops
            if MEM.match(dst):                       # store
                target = self._mem_target(dst, ins)
                mi = IMM.match(src)
                if mi:
                    v = int(mi.group(1), 16) & ((1 << (8 * width)) - 1) if width < 8 else int(mi.group(1), 16) & M64
                    self._store(ins, target, width, [('c', v)])
                    return
                r, rw = self._reg(src)
                if rw == 'x':
                    raise ReplayRefused('scalar store from xmm @%x' % ins['addr'])
                if width == 8:
                    if rw != 64:
                        raise ReplayRefused('movq store from a 32-bit register @%x' % ins['addr'])
                    self._store(ins, target, 8, [self._gval(src, ins)])
                    return
                v = self._gval(src, ins)
                if v[0] != 'c':
                    raise ReplayRefused('sub-qword store of an address @%x' % ins['addr'])
                self._store(ins, target, width, [('c', v[1] & ((1 << (8 * width)) - 1))])
                return
            d, dw = self._reg(dst)                  # register destination
            if dw == 'x' or MEM.match(src):
                raise ReplayRefused('load / xmm move form not allowed @%x' % ins['addr'])
            mi = IMM.match(src)
            if mi:
                if mn == 'movl':
                    self.g[d] = ('c', int(mi.group(1), 16) & 0xffffffff)
                elif mn == 'movq':
                    self.g[d] = ('c', int(mi.group(1), 16) & M64)
                else:
                    raise ReplayRefused('byte / word immediate to register @%x' % ins['addr'])
                return
            if mn == 'movq':
                self.g[d] = self._gval(src, ins)
                return
            if mn == 'movl':
                v = self._gval(src, ins)
                if v[0] != 'c':
                    raise ReplayRefused('movl of an address @%x' % ins['addr'])
                self.g[d] = ('c', v[1] & 0xffffffff)
                return
            raise ReplayRefused('register move form @%x' % ins['addr'])
        if mn == 'xorl' and len(ops) == 2:
            a, aw = self._reg(ops[0])
            b, bw = self._reg(ops[1])
            if a != b or aw != 32 or bw != 32:
                raise ReplayRefused('xorl of two different registers @%x' % ins['addr'])
            self.g[a] = ('c', 0)
            return
        if mn == 'addq' and len(ops) == 2:
            mi = IMM.match(ops[0])
            d, dw = self._reg(ops[1])
            if not mi or dw != 64 or d not in self.g:
                raise ReplayRefused('addq form @%x' % ins['addr'])
            v = self.g[d]
            self.g[d] = (v[0], (v[1] + int(mi.group(1), 16)) & M64)
            return
        if mn == 'vmovq' and len(ops) == 2:
            src, dst = ops
            d, dw = self._reg(dst)
            if dw != 'x' or MEM.match(src):
                raise ReplayRefused('vmovq form @%x' % ins['addr'])
            self.x[d] = (self._gval(src, ins), ('c', 0))
            return
        if mn in ('vxorps', 'vpxor') and len(ops) == 3:
            a, aw = self._reg(ops[0]); b, bw = self._reg(ops[1]); d, dw = self._reg(ops[2])
            if not (a == b == d and aw == bw == dw == 'x'):
                raise ReplayRefused('vector xor of two different registers @%x' % ins['addr'])
            self.x[d] = (('c', 0), ('c', 0))
            return
        if mn in ('vmovaps', 'vmovdqa') and len(ops) == 2 and not MEM.match(ops[0]) and not MEM.match(ops[1]):
            self.x[self._reg(ops[1])[0]] = self._xval(ops[0], ins)
            return
        if mn == 'vpunpcklqdq' and len(ops) == 3:
            s2 = self._xval(ops[0], ins); s1 = self._xval(ops[1], ins); d, dw = self._reg(ops[2])
            self.x[d] = (s1[0], s2[0])            # dst = {src1.lo, src2.lo}; AT&T order: src2, src1, dst
            self._cross_check_comment(ins, d, 'xmm%s[0],xmm%s[0]' % (self._reg(ops[1])[0][3:], self._reg(ops[0])[0][3:]))
            return
        if mn == 'vpshufd' and len(ops) == 3:
            mi = IMM.match(ops[0])
            src = self._xval(ops[1], ins); d, dw = self._reg(ops[2])
            sel = int(mi.group(1), 16)
            dw4 = [(sel >> (2 * i)) & 3 for i in range(4)]      # destination dword i <- source dword dw4[i]
            lanes = []
            for k in (0, 1):
                a, b = dw4[2 * k], dw4[2 * k + 1]
                if a % 2 != 0 or b != a + 1:
                    if src[0] == src[1] and src[0][0] == 'c' and src[0][1] == 0:
                        lanes.append(('c', 0))
                        continue
                    raise ReplayRefused('vpshufd that does not keep 64-bit lanes whole @%x' % ins['addr'])
                lanes.append(src[a // 2])
            self.x[d] = (lanes[0], lanes[1])
            self._cross_check_comment(ins, d, 'xmm%s[%d,%d,%d,%d]' % ((self._reg(ops[1])[0][3:],) + tuple(dw4)))
            return
        if mn in ('vmovdqu', 'vmovups') and len(ops) == 2 and MEM.match(ops[1]):
            target = self._mem_target(ops[1], ins)
            lo, hi = self._xval(ops[0], ins)
            self._store(ins, target, 16, [lo, hi])
            return
        raise ReplayRefused('instruction class not supported by the store-replay tracker: %s %s @%x' % (mn, ins['ops'], ins['addr']))

    def _cross_check_comment(self, ins, d, expect_tail):
        m = COMMENT.search(ins['text'])
        if not m:
            raise ReplayRefused('no objdump lane comment to pin the vector semantics @%x' % ins['addr'])
        got = m.group(2).replace(' ', '')
        if m.group(1) != d or got != expect_tail.replace(' ', ''):
            raise ReplayRefused('lane semantics disagree with the objdump comment @%x: %r vs %r' % (ins['addr'], got, expect_tail))

    def run(self, ins_list, start, end):
        """Replay every instruction with start <= addr < end (the range must be one straight line: no branch / call / ret)."""
        sel = [x for x in ins_list if start <= x['addr'] < end]
        if not sel:
            raise ReplayRefused('empty replay range')
        for x in sel:
            if x['mnem'].startswith(('call', 'j', 'ret')):
                raise ReplayRefused('control transfer inside the replay range @%x' % x['addr'])
            self.step(x)
        return self
