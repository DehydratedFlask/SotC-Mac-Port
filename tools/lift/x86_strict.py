#!/usr/bin/env python3
"""Strict mini x86-64 decoder + offline interpreter for small CPU-01 helper bodies (private; needs local/re/eboot.elf).

Offline analysis only. Nothing is executed on the host CPU and nothing here is a product runtime: pinned ELF
bytes are matched against a fixed table of encodings (anything unlisted raises DecodeError) and interpreted by
a throw-away model so the native C++ lift can be compared with what the *machine code* does.

Used by check_f161f80.py, check_f14c3a0.py, check_f14c4a0.py, check_ff730.py, check_f14c9a0.py,
check_f14d880.py, check_fb25470.py.

Model
  * 16 GPRs (64-bit; 32-bit writes zero-extend), ymm0-1 (32 bytes), flags ZF SF CF OF (flags left "undefined"
    by imul/xchg are poisoned and a conditional branch on them raises).
  * Memory: byte overlay on the pinned image. Reads: overlay -> file-backed bytes -> 0 for mapped BSS; anything
    else raises. Writes must hit mapped image memory or a declared scratch region. A synthetic stack is used
    for push/pop/call/ret and is not counted as a program write.
  * Locked RMW (lock inc, xchg with memory) are logged as accesses with locked=True.
  * Calls: in-image callees are interpreted; PLT thunks (`jmp [rip+d]`) are resolved through the ELF
    relocations to a NID. Modelled imports: __cxa_guard_acquire (rax = 1 iff guard byte == 0),
    __cxa_guard_release (guard byte = 1), __cxa_atexit (recorded; rax = 0). Any other import raises.
  * `jmp rax`-style indirect transfers decode but raise when executed (the unsupported parent path).
  * Registers not set by the caller hold a poison value (selectable) so that a result that depends on
    an unset register differs between two poison choices; the checkers run every case with two poisons.
"""
import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_ctor_decode as base  # noqa: E402
import ps4_dynlink as dyn  # noqa: E402

DecodeError = base.DecodeError
load_image = base.load_image
ELF_SHA256 = base.ELF_SHA256
M64 = (1 << 64) - 1
M32 = 0xFFFFFFFF
STACK_TOP = 0x7FFF_F000
SENTINEL = 0xDEAD_0000_0000
REGS = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
IMPORTS = ('__cxa_guard_acquire', '__cxa_guard_release', '__cxa_atexit')


# ----------------------------------------------------------------------------------------------- decoder
class Insn:
    __slots__ = ('pc', 'n', 'mn', 'ops', 'lock', 'raw')

    def __init__(self, pc, n, mn, ops, lock, raw):
        self.pc, self.n, self.mn, self.ops, self.lock, self.raw = pc, n, mn, ops, lock, raw

    def __repr__(self):
        return f'{self.pc:#x} {self.mn} {self.ops}'


def reg(i, w):
    return ('reg', i, w)


def imm(v):
    return ('imm', v)


def _modrm(c, i, rex, pc, w, imm_len=0):
    """Decode modrm at c[i]. Returns (regfield, rm_operand, next_i). rm is reg or mem operand of width w."""
    m = c[i]
    mod, regf, rm = m >> 6, ((m >> 3) & 7) | (8 if rex & 4 else 0), m & 7
    i += 1
    if mod == 3:
        return regf, reg(rm | (8 if rex & 1 else 0), w), i
    base_r, index, scale = None, None, 1
    if rm == 4:
        sib = c[i]
        i += 1
        scale = 1 << (sib >> 6)
        idx = ((sib >> 3) & 7) | (8 if rex & 2 else 0)
        bs = (sib & 7) | (8 if rex & 1 else 0)
        index = None if idx == 4 else idx
        if (sib & 7) == 5 and mod == 0:
            raise DecodeError(f'sib without base at {pc:#x}')
        base_r = bs
    elif rm == 5 and mod == 0:
        disp = struct.unpack_from('<i', c, i)[0]
        i += 4
        return regf, ('mem', 'rip', None, 1, pc + i + imm_len + disp - 0, w), i   # patched by caller (needs insn len)
    else:
        base_r = rm | (8 if rex & 1 else 0)
    disp = 0
    if mod == 1:
        disp = struct.unpack_from('<b', c, i)[0]
        i += 1
    elif mod == 2:
        disp = struct.unpack_from('<i', c, i)[0]
        i += 4
    return regf, ('mem', base_r, index, scale, disp, w), i


def decode(c, pc):
    """Decode one instruction from the byte string c (>= 16 bytes) located at pc."""
    i, lock, rex, vex = 0, False, 0, None
    if c[i] == 0xF0:
        lock, i = True, i + 1
    # multi-byte nop prefixes (66 / 2e) only for 0f 1f
    j = i
    while c[j] in (0x66, 0x2E):
        j += 1
    if j > i and c[j:j + 2] == b'\x0f\x1f':
        regf, rm, k = _modrm(c, j + 2, 0, pc, 4)
        if rm[0] == 'mem' and rm[1] == 'rip':
            raise DecodeError(f'rip-relative nop at {pc:#x}')
        return Insn(pc, k, 'nop', (), False, c[:k])
    if c[i] == 0xC4:         # VEX3: only vmovups xmm/ymm load/store (map 0f, pp none, W0, vvvv unused)
        b1, b2, op = c[i + 1], c[i + 2], c[i + 3]
        if (b1 & 0x1F) != 1 or (b2 & 0x83) != 0 or (b2 >> 3) & 0xF != 0xF:
            raise DecodeError(f'unsupported VEX3 {c[i:i + 5].hex()} at {pc:#x}')
        vl = 32 if b2 & 4 else 16
        vrex = (4 if not b1 & 0x80 else 0) | (2 if not b1 & 0x40 else 0) | (1 if not b1 & 0x20 else 0)
        regf, rm, k = _modrm(c, i + 4, vrex, pc, vl)
        if op == 0x10 and rm[0] == 'mem':
            return _fix_rip(Insn(pc, k, 'vmovups_load', (reg(regf, vl), rm), lock, c[:k]), pc)
        if op == 0x11 and rm[0] == 'mem':
            return _fix_rip(Insn(pc, k, 'vmovups_store', (rm, reg(regf, vl)), lock, c[:k]), pc)
        raise DecodeError(f'unsupported VEX3 opcode {op:#x} at {pc:#x}')
    if c[i] in (0xC5,):      # VEX2
        b1, op = c[i + 1], c[i + 2]
        vl = 32 if b1 & 4 else 16
        if (b1 & 3) != 0 or (b1 >> 3) & 0xF != 0xF:   # pp must be none (ps), vvvv unused (1111)
            raise DecodeError(f'unsupported VEX2 {c[i:i + 4].hex()} at {pc:#x}')
        rexr = 0 if b1 & 0x80 else 4
        regf, rm, k = _modrm(c, i + 3, rexr, pc, vl)
        if op == 0x10 and rm[0] == 'mem':
            ins = Insn(pc, k, 'vmovups_load', (reg(regf, vl), rm), lock, c[:k])
        elif op == 0x11 and rm[0] == 'mem':
            ins = Insn(pc, k, 'vmovups_store', (rm, reg(regf, vl)), lock, c[:k])
        elif op == 0x57 and rm[0] == 'reg' and rm[1] == regf:          # vxorps x,x,x : zero idiom only
            ins = Insn(pc, k, 'vzero', (reg(regf, vl),), lock, c[:k])
        else:
            raise DecodeError(f'unsupported VEX2 opcode {op:#x} at {pc:#x}')
        return _fix_rip(ins, pc)
    if 0x40 <= c[i] <= 0x4F:
        rex, i = c[i], i + 1
    w = 8 if rex & 8 else 4
    op = c[i]
    i += 1

    def rm_form(width, imm_len=0):
        regf, rm, k = _modrm(c, i, rex, pc, width, imm_len)
        return regf, rm, k

    if 0x50 <= op <= 0x57:
        return Insn(pc, i, 'push', (reg((op - 0x50) | (8 if rex & 1 else 0), 8),), lock, c[:i])
    if 0x58 <= op <= 0x5F:
        return Insn(pc, i, 'pop', (reg((op - 0x58) | (8 if rex & 1 else 0), 8),), lock, c[:i])
    if op == 0x90:
        return Insn(pc, i, 'nop', (), lock, c[:i])
    if op == 0xC3:
        return Insn(pc, i, 'ret', (), lock, c[:i])
    if op in (0xE8, 0xE9):
        rel = struct.unpack_from('<i', c, i)[0]
        i += 4
        return Insn(pc, i, 'call' if op == 0xE8 else 'jmp', (imm(pc + i + rel),), lock, c[:i])
    if 0x70 <= op <= 0x7F:
        cc = {0x72: 'jb', 0x74: 'je', 0x75: 'jne', 0x7E: 'jle'}.get(op)
        if cc is None:
            raise DecodeError(f'unsupported jcc {op:#x} at {pc:#x}')
        rel = struct.unpack_from('<b', c, i)[0]
        i += 1
        return Insn(pc, i, cc, (imm(pc + i + rel),), lock, c[:i])
    if 0xB8 <= op <= 0xBF and not rex & 8:
        v = struct.unpack_from('<I', c, i)[0]
        return Insn(pc, i + 4, 'mov', (reg((op - 0xB8) | (8 if rex & 1 else 0), 4), imm(v)), lock, c[:i + 4])
    if op in (0x89, 0x8B, 0x01, 0x03, 0x31, 0x39, 0x85, 0x8D, 0x87):
        regf, rm, k = rm_form(w)
        r = reg(regf, w)
        mn = {0x89: 'mov', 0x8B: 'mov', 0x01: 'add', 0x03: 'add', 0x31: 'xor', 0x39: 'cmp', 0x85: 'test', 0x8D: 'lea', 0x87: 'xchg'}[op]
        if op == 0x8D:
            if rm[0] != 'mem':
                raise DecodeError(f'lea with register operand at {pc:#x}')
            ops = (r, rm)
        elif op in (0x8B, 0x03):
            ops = (r, rm)
        else:
            ops = (rm, r)
        return _fix_rip(Insn(pc, k, mn, ops, lock, c[:k]), pc)
    if op in (0x88, 0x8A, 0x84):
        regf, rm, k = rm_form(1)
        if not rex and (regf >= 4 or (rm[0] == 'reg' and rm[1] >= 4)):
            raise DecodeError(f'legacy high-byte register at {pc:#x}')
        r = reg(regf, 1)
        mn = {0x88: 'mov', 0x8A: 'mov', 0x84: 'test'}[op]
        ops = (r, rm) if op == 0x8A else (rm, r)
        return _fix_rip(Insn(pc, k, mn, ops, lock, c[:k]), pc)
    if op == 0x69:
        regf, rm, k = rm_form(w, 4)
        v = struct.unpack_from('<I', c, k)[0]
        return _fix_rip(Insn(pc, k + 4, 'imul3', (reg(regf, w), rm, imm(v)), lock, c[:k + 4]), pc)
    if op in (0x83, 0x81):
        il = 1 if op == 0x83 else 4
        regf, rm, k = rm_form(w, il)
        v = struct.unpack_from('<b' if op == 0x83 else '<i', c, k)[0]
        mn = {0: 'add', 5: 'sub', 7: 'cmp'}.get(regf & 7)
        if mn is None:
            raise DecodeError(f'unsupported group1 /{regf & 7} at {pc:#x}')
        return _fix_rip(Insn(pc, k + il, mn, (rm, imm(v)), lock, c[:k + il]), pc)
    if op == 0xC6:
        regf, rm, k = rm_form(1, 1)
        if regf & 7:
            raise DecodeError(f'unsupported c6 /{regf & 7} at {pc:#x}')
        return _fix_rip(Insn(pc, k + 1, 'mov', (rm, imm(c[k])), lock, c[:k + 1]), pc)
    if op == 0xC7:
        regf, rm, k = rm_form(w, 4)
        if regf & 7:
            raise DecodeError(f'unsupported c7 /{regf & 7} at {pc:#x}')
        v = struct.unpack_from('<i', c, k)[0]
        return _fix_rip(Insn(pc, k + 4, 'mov', (rm, imm(v)), lock, c[:k + 4]), pc)
    if op == 0xFF:
        regf, rm, k = rm_form(w)
        sub = regf & 7
        if sub == 0:
            return _fix_rip(Insn(pc, k, 'inc', (rm,), lock, c[:k]), pc)
        if sub == 4 and rm[0] == 'reg':
            return Insn(pc, k, 'jmp_reg', (reg(rm[1], 8),), lock, c[:k])
        if sub == 4 and rm[0] == 'mem' and rm[1] == 'rip':
            return _fix_rip(Insn(pc, k, 'jmp_mem', (rm,), lock, c[:k]), pc)
        raise DecodeError(f'unsupported ff /{sub} at {pc:#x}')
    if op == 0x0F:
        op2 = c[i]
        i += 1
        if op2 == 0xB6:
            regf, rm, k = _modrm(c, i, rex, pc, 1)
            return _fix_rip(Insn(pc, k, 'movzx8', (reg(regf, w), rm), lock, c[:k]), pc)
        if op2 == 0xC1:
            regf, rm, k = _modrm(c, i, rex, pc, w)
            if rm[0] != 'mem':
                raise DecodeError(f'xadd with register destination at {pc:#x}')
            return _fix_rip(Insn(pc, k, 'xadd', (rm, reg(regf, w)), lock, c[:k]), pc)
        if op2 == 0x1F:
            regf, rm, k = _modrm(c, i, rex, pc, 4)
            return Insn(pc, k, 'nop', (), lock, c[:k])
    raise DecodeError(f'unsupported encoding {c[:10].hex()} at {pc:#x}')


def _fix_rip(ins, pc):
    """rip-relative operands were computed with the displacement end; add the final instruction length.

    _modrm returns pc+i+imm_len+disp where i is the offset just after the disp field, i.e. it already holds
    pc + (offset of the end of the instruction) when imm_len covers the immediate that follows, so nothing
    needs patching; this hook only validates the invariant (target must be a plain integer)."""
    for op in ins.ops:
        if op[0] == 'mem' and op[1] == 'rip' and not isinstance(op[4], int):
            raise DecodeError('bad rip operand')
    return ins


def read_code(image, pc, n=16):
    try:
        return image.read(pc, n)
    except ValueError:
        # near the end of a segment: read what exists
        for k in range(n - 1, 0, -1):
            try:
                return image.read(pc, k) + b'\x90' * (n - k)
            except ValueError:
                continue
        raise


def decode_range(image, rva, size):
    """Linear decode [rva, rva+size); must end exactly on the boundary. Returns list[Insn]."""
    out, pc = [], rva
    while pc < rva + size:
        ins = decode(read_code(image, pc), pc)
        out.append(ins)
        pc += ins.n
    if pc != rva + size:
        raise DecodeError(f'decode of {rva:#x} overran: ends at {pc:#x}, expected {rva + size:#x}')
    return out


def body_sha(image, rva, size):
    return hashlib.sha256(image.read(rva, size)).hexdigest()


def import_names(elf):
    """GOT slot -> import name for the modelled libc++abi NIDs (names are hashed with dyn.nid, not hard-coded)."""
    p = dyn.parse(elf)
    want = {dyn.nid(n): n for n in IMPORTS}
    out = {}
    for r in p['relocs']:
        if r['table'] != 'JMPREL':
            continue
        nid = p['symbols'][r['symbol']]['nid']
        out[r['target']] = want.get(nid, f'import:{nid}')
    return out


# --------------------------------------------------------------------------------------------- interpreter
class Result:
    pass


class Machine:
    def __init__(self, image, got, seed=None, poison=0, scratch=()):
        self.image, self.got = image, got
        self.mem = {}                      # addr -> byte overlay (program-visible memory)
        self.stack = {}
        self.scratch = list(scratch)       # [(base, size)] writable non-image regions
        self.writes = []                   # (addr, byte) in program order, non-stack
        self.reads = []                    # (addr, width) non-stack
        self.accesses = []                 # (kind, addr, width, locked) for locked RMW
        self.calls = []                    # ('in-image'|import name, target, args dict)
        self.regs = [(poison * 0x0101010101010101) & M64 for _ in REGS]
        self.regs[REGS.index('rsp')] = STACK_TOP
        self.ymm = [bytes([poison]) * 32, bytes([poison]) * 32]
        self.flags = dict(zf=False, sf=False, cf=False, of=False)
        self.undef = True
        self.steps = 0
        self.poke_seed(seed or {})

    # memory ---------------------------------------------------------------------------------------
    def poke_seed(self, seed):
        for a, b in seed.items():
            self.mem[a] = b & 0xFF

    def poke(self, addr, data):
        for k, b in enumerate(data):
            self.mem[addr + k] = b

    def _is_stack(self, a):
        return STACK_TOP - 0x10000 <= a <= STACK_TOP + 0x100

    def _writable(self, a):
        return self.image.mapped(a) or any(b <= a < b + s for b, s in self.scratch)

    def rd8(self, a, log=True):
        if self._is_stack(a):
            return self.stack.get(a, 0xCC)
        if a in self.mem:
            return self.mem[a]
        try:
            return self.image.read(a, 1)[0]
        except ValueError:
            if self.image.mapped(a) or any(b <= a < b + s for b, s in self.scratch):
                return 0
            raise DecodeError(f'read outside the image at {a:#x}')

    def wr8(self, a, v):
        if self._is_stack(a):
            self.stack[a] = v & 0xFF
            return
        if not self._writable(a):
            raise DecodeError(f'write outside the image at {a:#x}')
        self.mem[a] = v & 0xFF
        self.writes.append((a, v & 0xFF))

    def read(self, a, n):
        if not self._is_stack(a):
            self.reads.append((a, n))
        return int.from_bytes(bytes(self.rd8(a + k) for k in range(n)), 'little')

    def write(self, a, n, v):
        for k, b in enumerate((v & ((1 << (8 * n)) - 1)).to_bytes(n, 'little')):
            self.wr8(a + k, b)

    # operands -------------------------------------------------------------------------------------
    def ea(self, op):
        _, b, idx, sc, disp, _w = op
        if b == 'rip':
            return disp
        a = self.regs[b] + disp
        if idx is not None:
            a += self.regs[idx] * sc
        return a & M64

    def get(self, op):
        k = op[0]
        if k == 'imm':
            return op[1]
        if k == 'reg':
            return self.regs[op[1]] & ((1 << (8 * op[2])) - 1)
        return self.read(self.ea(op), op[5])

    def put(self, op, v):
        if op[0] == 'reg':
            w = op[2]
            if w == 8:
                self.regs[op[1]] = v & M64
            elif w == 4:
                self.regs[op[1]] = v & M32           # zero-extends
            elif w == 1:
                self.regs[op[1]] = (self.regs[op[1]] & ~0xFF & M64) | (v & 0xFF)
            else:
                raise DecodeError('unsupported register width')
        else:
            self.write(self.ea(op), op[5], v)

    @staticmethod
    def width(op):
        return op[2] if op[0] == 'reg' else op[5]

    def set_flags(self, res, w, cf=None, of=None, keep_cf=False):
        bits = 8 * w
        r = res & ((1 << bits) - 1)
        self.flags['zf'] = r == 0
        self.flags['sf'] = bool(r >> (bits - 1))
        if not keep_cf:
            self.flags['cf'] = bool(cf)
        self.flags['of'] = bool(of)
        self.undef = False

    # execution ------------------------------------------------------------------------------------
    def push(self, v):
        self.regs[4] = (self.regs[4] - 8) & M64
        self.write(self.regs[4], 8, v)

    def pop(self):
        v = self.read(self.regs[4], 8)
        self.regs[4] = (self.regs[4] + 8) & M64
        return v

    def thunk(self, target):
        try:
            code = self.image.read(target, 6)
        except ValueError:
            return None
        if code[:2] != b'\xff\x25':
            return None
        slot = target + 6 + struct.unpack_from('<i', code, 2)[0]
        return self.got.get(slot)

    def do_import(self, name, target):
        r = self.regs
        args = dict(rdi=r[7], rsi=r[6], rdx=r[2])
        self.calls.append((name, target, args))
        if name == '__cxa_guard_acquire':
            r[0] = 1 if self.read(r[7], 1) == 0 else 0
        elif name == '__cxa_guard_release':
            self.write(r[7], 1, 1)
        elif name == '__cxa_atexit':
            r[0] = 0
        else:
            raise DecodeError(f'unmodelled import {name} at {target:#x}')

    def cond(self, mn):
        if self.undef:
            raise DecodeError('conditional branch on undefined flags')
        f = self.flags
        return {'je': f['zf'], 'jne': not f['zf'], 'jb': f['cf'], 'jle': f['zf'] or f['sf'] != f['of']}[mn]

    def run(self, rva, regs=None, max_steps=2_000_000):
        """Call rva with the given register values; returns Result."""
        for k, v in (regs or {}).items():
            self.regs[REGS.index(k)] = v & M64
        saved = {k: self.regs[REGS.index(k)] for k in ('rbx', 'rbp', 'r12', 'r13', 'r14', 'r15')}
        rsp0 = self.regs[4]
        self.push(SENTINEL)
        pc = rva
        cache = {}
        while True:
            self.steps += 1
            if self.steps > max_steps:
                raise DecodeError('step limit')
            ins = cache.get(pc)
            if ins is None:
                ins = cache[pc] = decode(read_code(self.image, pc), pc)
            mn, ops, nxt = ins.mn, ins.ops, pc + ins.n
            if mn == 'nop':
                pass
            elif mn == 'push':
                self.push(self.get(ops[0]))
            elif mn == 'pop':
                self.put(ops[0], self.pop())
            elif mn == 'ret':
                tgt = self.pop()
                if tgt == SENTINEL:
                    break
                nxt = tgt
            elif mn == 'call':
                t = ops[0][1]
                name = self.thunk(t)
                if name is not None:
                    self.do_import(name, t)
                else:
                    self.calls.append(('in-image', t, dict(rdi=self.regs[7], rsi=self.regs[6], rdx=self.regs[2])))
                    self.push(nxt)
                    nxt = t
            elif mn == 'jmp':
                t = ops[0][1]
                name = self.thunk(t)
                if name is not None:               # tail call through a PLT thunk: call + ret
                    self.do_import(name, t)
                    nxt = self.pop()
                    if nxt == SENTINEL:
                        break
                else:
                    nxt = t
            elif mn in ('jmp_reg', 'jmp_mem'):
                raise DecodeError(f'indirect jump at {pc:#x} reached (unsupported path)')
            elif mn in ('je', 'jne', 'jb', 'jle'):
                if self.cond(mn):
                    nxt = ops[0][1]
            elif mn == 'mov':
                self.put(ops[0], self.get(ops[1]))
            elif mn == 'lea':
                self.put(ops[0], self.ea(ops[1]))
            elif mn == 'movzx8':
                self.put(ops[0], self.get(ops[1]))
            elif mn == 'xchg':
                a, b = ops
                locked = a[0] == 'mem' or b[0] == 'mem'
                m = a if a[0] == 'mem' else b
                if locked:
                    self.accesses.append(('rmw', self.ea(m), m[5], True))
                va, vb = self.get(a), self.get(b)
                self.put(a, vb)
                self.put(b, va)
            elif mn == 'xadd':
                m = ops[0]
                self.accesses.append(('rmw', self.ea(m), m[5], True))
                old_v, src = self.get(m), self.get(ops[1])
                self.put(m, old_v + src)
                self.put(ops[1], old_v)
                self.undef = True
            elif mn in ('add', 'sub', 'cmp', 'xor', 'test', 'inc'):
                self.alu(ins)
            elif mn == 'imul3':
                w = self.width(ops[0])
                src = self.get(ops[1])
                v = ops[2][1]
                bits = 8 * w
                sx = lambda x: x - (1 << bits) if x >> (bits - 1) else x   # noqa: E731
                self.put(ops[0], (sx(src) * sx(v & ((1 << bits) - 1))) & ((1 << bits) - 1))
                self.undef = True                                           # flags architecturally undefined
            elif mn == 'vmovups_load':
                n = ops[0][2]
                a = self.ea(ops[1])
                data = bytes(self.rd8(a + k) for k in range(n))
                self.reads.append((a, n))
                self.ymm[ops[0][1]] = data + b'\x00' * (32 - n)      # VEX zeroes the upper lanes
            elif mn == 'vzero':
                self.ymm[ops[0][1]] = b'\x00' * 32
            elif mn == 'vmovups_store':
                n = ops[1][2]
                a = self.ea(ops[0])
                for k, b in enumerate(self.ymm[ops[1][1]][:n]):
                    self.wr8(a + k, b)
            else:
                raise DecodeError(f'unmodelled mnemonic {mn} at {pc:#x}')
            pc = nxt
        res = Result()
        res.rax = self.regs[0]
        res.regs = list(self.regs)
        res.steps = self.steps
        if self.regs[4] != rsp0:
            raise DecodeError('stack imbalance')
        for k, v in saved.items():
            if self.regs[REGS.index(k)] != v:
                raise DecodeError(f'callee-saved {k} not preserved')
        return res

    def alu(self, ins):
        mn, ops = ins.mn, ins.ops
        dst = ops[0]
        w = self.width(dst)
        bits = 8 * w
        mask = (1 << bits) - 1
        a = self.get(dst)
        if mn == 'inc':
            r = (a + 1) & mask
            if ins.lock:
                self.accesses.append(('rmw', self.ea(dst), w, True))
            self.put(dst, r)
            self.set_flags(r, w, cf=False, of=(a == (1 << (bits - 1)) - 1), keep_cf=True)
            return
        b = self.get(ops[1])
        if ops[1][0] == 'imm':
            b &= mask            # sign-extended immediates arrive as Python negatives
        if mn == 'add':
            r = a + b
            self.set_flags(r, w, cf=r >> bits, of=((a ^ r) & (b ^ r)) >> (bits - 1) & 1)
            self.put(dst, r & mask)
        elif mn in ('sub', 'cmp'):
            r = a - b
            self.set_flags(r & mask, w, cf=a < b, of=((a ^ b) & (a ^ r)) >> (bits - 1) & 1)
            if mn == 'sub':
                self.put(dst, r & mask)
        elif mn == 'xor':
            r = a ^ b
            self.set_flags(r, w, cf=False, of=False)
            self.put(dst, r)
        elif mn == 'test':
            self.set_flags(a & b, w, cf=False, of=False)


def make(image, got, seed=None, poison=0, scratch=()):
    return Machine(image, got, seed, poison, scratch)


def mem_bytes(m, addr, n):
    return bytes(m.rd8(addr + k) for k in range(n))
