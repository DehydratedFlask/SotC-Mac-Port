#!/usr/bin/env python3
"""Strict symbolic simulator for call-free leaf constructor bodies (CPU-01, batch 35).

Offline analysis only; nothing is executed. It extends the idea of leaf_ctor_decode.py (which stays
unchanged and keeps its straight-line, file-backed-load-only contract) for the three batch-35 bodies
that need a little more: a bounded counted loop (0x9b75a0), a copy of runtime state (0x575d40) and
a copy of runtime state plus address-valued constants (0x99350).

Every instruction is matched by exact encoding; anything unlisted raises DecodeError. Values are
tracked per byte:
  int               a known byte (immediates, and loads from the read-only code/rodata segment)
  'S:<addr>'        the byte that was at <addr> when the body started (non-read-only memory such as
                    BSS/.data is run-time state, never assumed to be its file image)
  'A:<value>:<k>'   byte k of an address-valued constant (lea of an original image address)
Loads from an address the body already stored to are forwarded from that store.
Supported subset: VEX2 vmovups xmm/ymm, vmovss, vxorps zero idiom; lea r64,[rip+d]; movabs;
mov eax|ecx,imm32; mov r32/r64,[mem]; mov [mem],r32/r64; mov [mem],imm8/imm32/simm32;
add rax,imm8; cmp rax,imm32; jne rel8; ret. Registers rax/rcx/rdx only. Memory operands:
[rip+d32], [reg+disp8/disp32], [rcx+rax+disp8] (SIB scale 1).
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_ctor_decode as base  # noqa: E402

DecodeError = base.DecodeError
load_image = base.load_image
ELF_SHA256 = base.ELF_SHA256
READONLY_END = 22624632        # end of the first (r-x, file-backed) PT_LOAD: code + rodata
GPR = {0: 'rax', 1: 'rcx', 2: 'rdx'}
STEP_LIMIT = 100000


def _sx8(x):
    return x - 256 if x >= 128 else x


class Machine:
    def __init__(self, image, rva):
        self.image, self.rva = image, rva
        self.gpr = {}            # name -> dict(kind='imm'|'addr'|'bytes', val=int | tokens=list)
        self.vec = {}            # idx -> list of 32 byte tokens
        self.mem = {}            # addr -> byte token written by this body
        self.stores, self.sources = [], []
        self.static = {}         # pc -> (len, name), first time seen
        self.zf = None
        self.steps = 0

    # ---- operands -------------------------------------------------------------------------
    def numeric(self, name, pc):
        v = self.gpr.get(name)
        if v is None or v['kind'] not in ('imm', 'addr'):
            raise DecodeError(f'{name} is not a known number/address at {pc:#x}')
        return v['val']

    def ea(self, c, i, pc, imm_len):
        """Parse the memory operand at c[i]; return (address, bytes consumed from i)."""
        modrm = c[i]
        mod, rm = modrm >> 6, modrm & 7
        if mod == 0 and rm == 5:
            return pc + i + 5 + imm_len + struct.unpack_from('<i', c, i + 1)[0], 5
        if mod == 3:
            raise DecodeError(f'register operand where memory expected at {pc:#x}')
        if rm == 4:                                   # SIB: only [rcx + rax*1 + disp]
            if c[i + 1] != 0x01 or mod not in (0, 1):
                raise DecodeError(f'unsupported SIB {c[i + 1]:#04x} mod {mod} at {pc:#x}')
            addr = self.numeric('rcx', pc) + self.numeric('rax', pc)
            if mod == 0:
                return addr, 2
            return addr + _sx8(c[i + 2]), 3
        if rm not in GPR:
            raise DecodeError(f'unsupported base register {rm} at {pc:#x}')
        addr = self.numeric(GPR[rm], pc)
        if mod == 0:
            return addr, 1
        if mod == 1:
            return addr + _sx8(c[i + 1]), 2
        return addr + struct.unpack_from('<i', c, i + 1)[0], 5

    def load(self, addr, size, pc):
        out = []
        for a in range(addr, addr + size):
            if a in self.mem:
                out.append(self.mem[a])
            elif a < READONLY_END:
                out.append(self.image.read(a, 1)[0])
            else:
                out.append(f'S:{a:#x}')
        self.sources.append(dict(addr=addr, size=size,
                                 kind='runtime' if any(isinstance(b, str) and b.startswith('S:') for b in out) else 'const'))
        return out

    def store(self, addr, toks):
        toks = list(toks)
        for k, t in enumerate(toks):
            self.mem[addr + k] = t
        self.stores.append(dict(dest=addr, width=len(toks), bytes=toks))

    def gpr_bytes(self, name, width, pc):
        v = self.gpr.get(name)
        if v is None:
            raise DecodeError(f'{name} unset at {pc:#x}')
        if v['kind'] == 'imm':
            return list(struct.pack('<Q', v['val'] & (2 ** 64 - 1))[:width])
        if v['kind'] == 'addr':
            return [f'A:{v["val"]:#x}:{k}' for k in range(width)]
        return list(v['tokens'][:width])

    # ---- run ------------------------------------------------------------------------------
    def run(self):
        pc = self.rva
        while True:
            self.steps += 1
            if self.steps > STEP_LIMIT:
                raise DecodeError('step limit exceeded')
            c = self.image.read(pc, 16)
            n, name, nxt = self.step(c, pc)
            self.static.setdefault(pc, (n, name))
            if name == 'ret':
                break
            pc = nxt if nxt is not None else pc + n
            if pc < self.rva or pc > self.rva + 0x1000:
                raise DecodeError(f'control left the body at {pc:#x}')
        end = max(p + l for p, (l, _) in self.static.items())
        insns = [(p, l, nm) for p, (l, nm) in sorted(self.static.items())]
        return dict(stores=self.stores, sources=self.sources, insns=insns, end=end, steps=self.steps)

    def step(self, c, pc):
        g = self.gpr
        if c[0] == 0xC3:
            return 1, 'ret', None
        if c[0] == 0xC5:                                              # VEX2
            p1, op = c[1], c[2]
            if p1 & 0x80 == 0:
                raise DecodeError(f'high register in VEX at {pc:#x}')
            vvvv, l256, pp = (~(p1 >> 3)) & 0xF, (p1 >> 2) & 1, p1 & 3
            modrm = c[3]
            reg = (modrm >> 3) & 7
            size = 32 if l256 else 16
            if op == 0x57 and pp == 0 and modrm >> 6 == 3 and vvvv == reg == (modrm & 7):
                self.vec[reg] = [0] * 32
                return 4, f'vxorps v{reg}/{size}', None
            if op in (0x10, 0x11) and pp == 0:
                addr, used = self.ea(c, 3, pc, 0)
                if op == 0x10:
                    self.vec[reg] = self.load(addr, size, pc) + [0] * (32 - size)
                else:
                    if reg not in self.vec:
                        raise DecodeError(f'v{reg} unset at {pc:#x}')
                    self.store(addr, self.vec[reg][:size])
                return 3 + used, f'vmovups v{reg}/{size}', None
            if op == 0x10 and pp == 2 and not l256:                    # vmovss load
                addr, used = self.ea(c, 3, pc, 0)
                self.vec[reg] = self.load(addr, 4, pc) + [0] * 28
                return 3 + used, f'vmovss v{reg}', None
            if op == 0x11 and pp == 2 and not l256:                    # vmovss store
                addr, used = self.ea(c, 3, pc, 0)
                self.store(addr, self.vec[reg][:4])
                return 3 + used, f'vmovss store v{reg}', None
            raise DecodeError(f'unsupported VEX {c[:5].hex()} at {pc:#x}')
        if c[0] == 0x48 and c[1] == 0x8D and (c[2] & 0xC7) == 0x05 and ((c[2] >> 3) & 7) in GPR:
            r = GPR[(c[2] >> 3) & 7]
            g[r] = dict(kind='addr', val=pc + 7 + struct.unpack_from('<i', c, 3)[0])
            return 7, f'lea {r}', None
        if c[0] == 0x48 and 0xB8 <= c[1] <= 0xBF and (c[1] - 0xB8) in GPR:
            r = GPR[c[1] - 0xB8]
            g[r] = dict(kind='imm', val=struct.unpack_from('<Q', c, 2)[0])
            return 10, f'movabs {r}', None
        if 0xB8 <= c[0] <= 0xBF and (c[0] - 0xB8) in GPR:               # mov r32, imm32
            r = GPR[c[0] - 0xB8]
            g[r] = dict(kind='imm', val=struct.unpack_from('<I', c, 1)[0])
            return 5, f'mov {r}d,imm32', None
        if c[0] == 0x48 and c[1] == 0x83 and (c[2] >> 3) & 7 == 0 and c[2] >> 6 == 3 and (c[2] & 7) == 0:
            g['rax'] = dict(kind='imm', val=(self.numeric('rax', pc) + _sx8(c[3])) & (2 ** 64 - 1))
            return 4, 'add rax,imm8', None
        if c[0] == 0x48 and c[1] == 0x3D:                               # cmp rax, simm32
            self.zf = self.numeric('rax', pc) == (struct.unpack_from('<i', c, 2)[0] & (2 ** 64 - 1))
            return 6, 'cmp rax,imm32', None
        if c[0] == 0x75:                                                # jne rel8
            if self.zf is None:
                raise DecodeError(f'jne without flags at {pc:#x}')
            tgt = pc + 2 + _sx8(c[1])
            return 2, 'jne', None if self.zf else tgt
        if c[0] in (0x8B, 0x89) and ((c[1] >> 3) & 7) in (0, 1) and (c[1] >> 6) != 3:     # mov r32 <-> mem
            r = GPR[(c[1] >> 3) & 7]
            addr, used = self.ea(c, 1, pc, 0)
            if c[0] == 0x8B:
                g[r] = dict(kind='bytes', tokens=self.load(addr, 4, pc) + [0] * 4)
                return 1 + used, f'mov {r}d,[mem]', None
            self.store(addr, self.gpr_bytes(r, 4, pc))
            return 1 + used, f'mov [mem],{r}d', None
        if c[0] == 0x48 and c[1] in (0x8B, 0x89) and ((c[2] >> 3) & 7) in GPR and (c[2] >> 6) != 3:
            r = GPR[(c[2] >> 3) & 7]
            addr, used = self.ea(c, 2, pc, 0)
            if c[1] == 0x8B:
                g[r] = dict(kind='bytes', tokens=self.load(addr, 8, pc))
                return 2 + used, f'mov {r},[mem]', None
            self.store(addr, self.gpr_bytes(r, 8, pc))
            return 2 + used, f'mov [mem],{r}', None
        if c[0] == 0x48 and c[1] == 0xC7 and (c[2] >> 3) & 7 == 0 and (c[2] >> 6) != 3:
            addr, used = self.ea(c, 2, pc, 4)
            self.store(addr, list(struct.pack('<q', struct.unpack_from('<i', c, 2 + used)[0])))
            return 2 + used + 4, 'mov qword imm', None
        if c[0] == 0xC7 and (c[1] >> 3) & 7 == 0 and (c[1] >> 6) != 3:
            addr, used = self.ea(c, 1, pc, 4)
            self.store(addr, list(c[1 + used:1 + used + 4]))
            return 1 + used + 4, 'mov dword imm', None
        if c[0] == 0xC6 and (c[1] >> 3) & 7 == 0 and (c[1] >> 6) != 3:
            addr, used = self.ea(c, 1, pc, 1)
            self.store(addr, [c[1 + used]])
            return 1 + used + 1, 'mov byte imm', None
        raise DecodeError(f'unrecognised encoding at {pc:#x}: {c.hex(" ")}')


def decode(image, rva):
    return Machine(image, rva).run()


def region(stores, fill=0):
    """(base, token list) of the span covered by the stores; uncovered bytes hold `fill`."""
    lo = min(s['dest'] for s in stores)
    hi = max(s['dest'] + s['width'] for s in stores)
    buf = [fill] * (hi - lo)
    for s in stores:
        buf[s['dest'] - lo:s['dest'] - lo + s['width']] = s['bytes']
    return lo, buf


def substitute(tokens, source_byte, address_value):
    """Replace 'S:<addr>' by source_byte(addr) and 'A:<v>:<k>' by byte k of address_value(v)."""
    out = []
    for t in tokens:
        if isinstance(t, int):
            out.append(t)
        elif t.startswith('S:'):
            out.append(source_byte(int(t[2:], 16)))
        elif t.startswith('A:'):
            _, v, k = t.split(':')
            out.append((address_value(int(v, 16)) >> (8 * int(k))) & 0xFF)
        else:
            raise DecodeError(f'bad token {t}')
    return out


if __name__ == '__main__':
    elf, image = load_image()
    for arg in sys.argv[1:]:
        r = decode(image, int(arg, 16))
        print(arg, r['end'] - int(arg, 16), len(r['stores']), 'stores', r['steps'], 'steps')
