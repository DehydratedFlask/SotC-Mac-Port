"""Byte-level STORE-REPLAY decoder for the long straight-line table initialisers (batch 18: startup slot 424 / 0x551140).

Independent of objdump: it decodes the raw instruction BYTES (read through `Elf.raw`, so an overlay perturbs the decode) of a straight-line range with a tiny,
closed x86-64 decoder and replays the effect of every instruction symbolically (registers hold a 64-bit constant ('c', n) or an address ('a', abs); xmm
registers hold two such 64-bit lanes). Every store through a tracked address register is recorded as {addr, bytes, target, width, lanes}.

The decoder knows EXACTLY the encodings that occur in the slot-424 range and nothing else:
  REX.W 8d /r (rip-relative)      lea r64, [rip + disp32]
  [REX.W] c7 /0 id                mov r/m32 or r/m64, imm32 (memory destination, disp8 / disp32 / none)
  REX.W 89 /r                     mov r/m64, r64 (register or memory destination)
  REX.W 83 /0 ib                  add r/m64, imm8 (register destination)
  31 /r                           xor r32, r32 (same register)
  VEX c5 / c4 forms               vmovq xmm, r64 ; vmovdqu / vmovups m128, xmm ; vpshufd xmm, xmm, imm8 ; vpunpcklqdq ; vpxor ; vxorps
Anything else (another opcode, an SIB byte, a rip-relative memory operand, a load, a branch, a vector shuffle that splits a 64-bit lane, a store through a
register that is not a tracked address) raises ReplayRefused: nothing is skipped and nothing is guessed. Nothing is executed.
"""
from __future__ import annotations

M64 = (1 << 64) - 1


class ReplayRefused(Exception):
    pass


def _s8(b: int) -> int:
    return b - 256 if b >= 128 else b


def _s32(b: bytes) -> int:
    v = int.from_bytes(b, 'little')
    return v - (1 << 32) if v >= (1 << 31) else v


class ByteReplay:
    def __init__(self, raw):
        """raw(addr, size) -> bytes (overlay aware)."""
        self.raw = raw
        self.g = {}
        self.x = {}
        self.stores = []
        self.nins = 0

    # ---- operand helpers
    def _modrm_mem(self, code, p, rexB):
        """Decode a memory modrm at code[p]; returns (reg, base_reg, disp, length_of_modrm_plus_disp). No SIB, no rip-relative."""
        m = code[p]
        mod, reg, rm = m >> 6, (m >> 3) & 7, m & 7
        if mod == 3:
            raise ReplayRefused('register form where a memory operand is required')
        if rm == 4:
            raise ReplayRefused('SIB byte not supported')
        if mod == 0:
            if rm == 5:
                raise ReplayRefused('rip-relative memory operand not supported')
            return reg, rm | (rexB << 3), 0, 1
        if mod == 1:
            return reg, rm | (rexB << 3), _s8(code[p + 1]), 2
        return reg, rm | (rexB << 3), _s32(code[p + 1:p + 5]), 5

    def _addr(self, base, disp, at):
        v = self.g.get(base)
        if v is None or v[0] != 'a':
            raise ReplayRefused('store through a register that is not a tracked address (reg %d) @%x' % (base, at))
        return v[1] + disp

    def _rec(self, at, ln, target, width, lanes):
        self.stores.append(dict(addr=at, bytes=self.raw(at, ln).hex(), target=target, width=width, lanes=lanes))

    def _gc(self, r, at, w32=False):
        v = self.g.get(r)
        if v is None:
            raise ReplayRefused('read of an undefined register %d @%x' % (r, at))
        if w32:
            if v[0] != 'c':
                raise ReplayRefused('32-bit read of an address @%x' % at)
            return ('c', v[1] & 0xffffffff)
        return v

    def _xv(self, r, at):
        v = self.x.get(r)
        if v is None:
            raise ReplayRefused('xmm%d is not defined @%x' % (r, at))
        return v

    # ---- one instruction at `at`; returns its length
    def step(self, at):
        code = self.raw(at, 16)
        p = 0
        rex = 0
        if 0x40 <= code[0] <= 0x4f:
            rex = code[0]
            p = 1
        W, R, B = (rex >> 3) & 1, (rex >> 2) & 1, rex & 1
        op = code[p]
        self.nins += 1
        if op == 0x8d and W:                                   # lea r64, [rip+disp32]
            m = code[p + 1]
            if (m >> 6) == 0 and (m & 7) == 5:
                ln = p + 6
                self.g[((m >> 3) & 7) | (R << 3)] = ('a', at + ln + _s32(code[p + 2:p + 6]))
                return ln
            reg, base, disp, mlen = self._modrm_mem(code, p + 1, B)          # lea r64, [base + disp] (no SIB)
            v = self.g.get(base)
            if v is None:
                raise ReplayRefused('lea of an undefined register @%x' % at)
            self.g[reg | (R << 3)] = (v[0], (v[1] + disp) & M64)
            return p + 1 + mlen
        if op == 0xc7:                                         # mov r/m, imm32 (memory destination)
            reg, base, disp, mlen = self._modrm_mem(code, p + 1, B)
            if reg != 0:
                raise ReplayRefused('c7 /%d not supported @%x' % (reg, at))
            ln = p + 1 + mlen + 4
            imm = _s32(code[p + 1 + mlen:p + 1 + mlen + 4])
            tgt = self._addr(base, disp, at)
            if W:
                self._rec(at, ln, tgt, 8, [('c', imm & M64)])
            else:
                self._rec(at, ln, tgt, 4, [('c', imm & 0xffffffff)])
            return ln
        if op == 0x89 and W:                                   # mov r/m64, r64
            m = code[p + 1]
            mod, reg = m >> 6, ((m >> 3) & 7) | (R << 3)
            if mod == 3:
                self.g[(m & 7) | (B << 3)] = self._gc(reg, at)
                return p + 2
            r2, base, disp, mlen = self._modrm_mem(code, p + 1, B)
            ln = p + 1 + mlen
            self._rec(at, ln, self._addr(base, disp, at), 8, [self._gc(reg, at)])
            return ln
        if op == 0x83 and W:                                   # add r/m64, imm8
            m = code[p + 1]
            if (m >> 6) != 3 or ((m >> 3) & 7) != 0:
                raise ReplayRefused('83 form not supported @%x' % at)
            d = (m & 7) | (B << 3)
            v = self.g.get(d)
            if v is None:
                raise ReplayRefused('add to an undefined register @%x' % at)
            self.g[d] = (v[0], (v[1] + _s8(code[p + 2])) & M64)
            return p + 3
        if op == 0x31 and not rex:                             # xor r32, r32
            m = code[p + 1]
            if (m >> 6) != 3 or ((m >> 3) & 7) != (m & 7):
                raise ReplayRefused('xor of two different registers @%x' % at)
            self.g[m & 7] = ('c', 0)
            return 2
        if code[0] == 0xc5:                                    # 2-byte VEX
            b1 = code[1]
            Rv = 1 - (b1 >> 7)
            vvvv = (~(b1 >> 3)) & 0xf
            Lb = (b1 >> 2) & 1
            pp = b1 & 3
            opc = code[2]
            if Lb:
                raise ReplayRefused('256-bit vector form @%x' % at)
            m = code[3]
            mod, reg, rm = m >> 6, (((m >> 3) & 7) | (Rv << 3)), m & 7
            if opc == 0x7f and pp == 2:                        # vmovdqu m128, xmm
                return self._vstore(at, code, reg, 'dqu')
            if opc == 0x11 and pp == 0:                        # vmovups m128, xmm
                return self._vstore(at, code, reg, 'ups')
            if mod != 3:
                raise ReplayRefused('VEX memory form not supported @%x' % at)
            if opc == 0x70 and pp == 1:                        # vpshufd xmm, xmm, imm8
                sel = code[4]
                src = self._xv(rm, at)
                dw = [(sel >> (2 * i)) & 3 for i in range(4)]
                lanes = []
                for k in (0, 1):
                    a, b = dw[2 * k], dw[2 * k + 1]
                    if a % 2 != 0 or b != a + 1:
                        if src[0] == src[1] == ('c', 0):
                            lanes.append(('c', 0))
                            continue
                        raise ReplayRefused('vpshufd that does not keep 64-bit lanes whole @%x' % at)
                    lanes.append(src[a // 2])
                self.x[reg] = (lanes[0], lanes[1])
                return 5
            if opc == 0x6c and pp == 1:                        # vpunpcklqdq dst, src1(vvvv), src2(rm)
                s1, s2 = self._xv(vvvv, at), self._xv(rm, at)
                self.x[reg] = (s1[0], s2[0])
                return 4
            if opc == 0xef and pp == 1 or opc == 0x57 and pp == 0:   # vpxor / vxorps of one register with itself
                if not (reg == vvvv == rm):
                    raise ReplayRefused('vector xor of two different registers @%x' % at)
                self.x[reg] = (('c', 0), ('c', 0))
                return 4
            raise ReplayRefused('2-byte VEX opcode %02x pp=%d not supported @%x' % (opc, pp, at))
        if code[0] == 0xc4:                                    # 3-byte VEX: vmovq xmm, r64
            b1, b2, opc, m = code[1], code[2], code[3], code[4]
            Rv, Bv = 1 - (b1 >> 7), 1 - ((b1 >> 5) & 1)
            mmmmm = b1 & 0x1f
            Wv, pp, Lb = b2 >> 7, b2 & 3, (b2 >> 2) & 1
            if mmmmm != 1 or not Wv or pp != 1 or Lb or opc != 0x6e or (m >> 6) != 3 or ((~(b2 >> 3)) & 0xf) != 0 or ((b1 >> 6) & 1) == 0:
                raise ReplayRefused('3-byte VEX form not supported @%x' % at)
            self.x[((m >> 3) & 7) | (Rv << 3)] = (self._gc((m & 7) | (Bv << 3), at), ('c', 0))
            return 5
        raise ReplayRefused('instruction class not supported by the byte store-replay: %s @%x' % (code[:8].hex(), at))

    def _vstore(self, at, code, xreg, kind):
        r2, base, disp, mlen = self._modrm_mem(code, 3, 0)
        ln = 3 + mlen
        lo, hi = self._xv(xreg, at)
        self._rec(at, ln, self._addr(base, disp, at), 16, [lo, hi])
        return ln

    def run(self, start, end):
        a = start
        while a < end:
            a += self.step(a)
        if a != end:
            raise ReplayRefused('replay range does not end on an instruction boundary: %x vs %x' % (a, end))
        return self
