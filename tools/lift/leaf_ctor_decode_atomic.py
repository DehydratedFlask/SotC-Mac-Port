#!/usr/bin/env python3
"""Symbolic leaf-constructor decoder with xor-zero and xchg (CPU-01, leaf batch 36).

Offline analysis only; nothing is executed. Wraps tools/lift/leaf_ctor_decode_sym.py (unchanged)
and adds exactly two encodings used by RVA 0x7ce8d0:
  31 /r   xor r32,r32 (register form, both operands the same GPR in rax/rcx/rdx) -> known 0
  87 /r   xchg r/m32,r32 (memory form, r32 in eax/ecx/edx): stores the 4 register bytes to the
          memory operand with an *atomic* (locked, full-barrier) exchange; the old memory value
          is returned to the register and is run-time state (kept symbolic, never used here).
Everything else is delegated to the base simulator, which raises on any unknown encoding.
A store made by xchg carries atomic=True so the native lift can be held to the same ordering.
"""
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_ctor_decode_sym as base  # noqa: E402

DecodeError = base.DecodeError
load_image = base.load_image
ELF_SHA256 = base.ELF_SHA256
GPR = base.GPR
region = base.region
substitute = base.substitute


class Machine(base.Machine):
    def step(self, c, pc):
        if c[0] == 0x31 and c[1] >> 6 == 3 and ((c[1] >> 3) & 7) == (c[1] & 7) and (c[1] & 7) in GPR:
            r = GPR[c[1] & 7]
            self.gpr[r] = dict(kind='imm', val=0)
            return 2, f'xor {r}d,{r}d', None
        if c[0] == 0x87 and (c[1] >> 6) != 3 and ((c[1] >> 3) & 7) in GPR:
            r = GPR[(c[1] >> 3) & 7]
            addr, used = self.ea(c, 1, pc, 0)
            new = self.gpr_bytes(r, 4, pc)
            old = self.load(addr, 4, pc)
            self.store(addr, new)
            self.stores[-1]['atomic'] = True
            self.gpr[r] = dict(kind='bytes', tokens=old + [0] * 4)
            return 1 + used, f'xchg [mem],{r}d', None
        return super().step(c, pc)


def decode(image, rva):
    return Machine(image, rva).run()


if __name__ == '__main__':
    elf, image = load_image()
    for arg in sys.argv[1:]:
        r = decode(image, int(arg, 16))
        print(arg, r['end'] - int(arg, 16), len(r['stores']), 'stores', r['steps'], 'steps')
        for s in r['stores']:
            print(hex(s['dest']), s['width'], s.get('atomic', False), s['bytes'])
        print(r['sources'])
