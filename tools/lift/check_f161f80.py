#!/usr/bin/env python3
"""Tie native/core/fnv_span_161f80 to original helper RVA 0x161f80 (private; needs local/re/eboot.elf).

The 38-byte body is pinned by SHA-256, decoded strictly (14 instructions, no call, no stack use) and
INTERPRETED with tools/lift/x86_strict.py on many {ptr,len,seed} cases (empty, negative and INT64_MIN
lengths, high bytes, 300 bytes, random). Each case is also checked against an independent Python
zero-extended FNV-1a and against `test_fnv_span_161f80 --dump`. The machine run may read only the 16 span
bytes and exactly `len` data bytes and must write nothing.
"""
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402

require = C.require
RVA, SIZE = 0x161f80, 38
SHA = '552c900d77771ee72cb1c8ed6f39bd510460ae7b614d92b654f1a85ca6031599'
DATA, SPAN = 0x6000_0000, 0x6100_0000


def fnv(data, seed):
    h = seed
    for b in data:
        h = ((h ^ b) * 0x01000193) & 0xFFFFFFFF
    return h


def main():
    ctx = C.Ctx()
    insns = ctx.pin(RVA, SIZE, SHA, 14)
    require(not any(i.mn in ('call', 'push', 'pop', 'jmp_reg') for i in insns), 'helper makes calls / uses the stack')
    require(any(i.mn == 'imul3' and i.ops[2][1] == 0x01000193 for i in insns), 'FNV prime multiply missing')
    require([i.mn for i in insns].count('movzx8') == 1, 'byte load is not zero-extending')

    rnd = random.Random(0x161F80)
    cases = [(0x811C9DC5, 0, b''), (0x811C9DC5, 1, b'a'), (0x811C9DC5, 6, b'foobar'), (0x12345678, 0, b'xyz'),
             (0x12345678, -1, b'xyz'), (0xCAFEF00D, -(1 << 63), b'xyz'), (0, 1, b'\x80'), (0, 1, b'\xff'),
             (0xFFFFFFFF, 3, b'\x00\x00\x00'), (0x811C9DC5, 300, bytes(range(256)) + bytes(44)), (7, 5, b'ab\xe3\x81\x82')]
    for _ in range(40):
        n = rnd.randrange(1, 60)
        cases.append((rnd.getrandbits(32), n, bytes(rnd.randrange(256) for _ in range(n))))

    want = []
    for seed, ln, data in cases:
        def once(poison):
            m = ctx.machine(poison=poison, scratch=[(DATA, 0x1000), (SPAN, 0x100)])
            m.poke(DATA, data)
            m.poke(SPAN, DATA.to_bytes(8, 'little') + (ln & ((1 << 64) - 1)).to_bytes(8, 'little'))
            r = m.run(RVA, {'rdi': SPAN, 'rsi': 0xDEADBEEF00000000 | seed})
            require(r.rax >> 32 == 0, 'eax result is not zero-extended')
            require(m.writes == [], 'helper wrote memory')
            reads = sorted(a for a, w in m.reads for a in range(a, a + w))
            expect = list(range(SPAN, SPAN + 16, 1)) if ln > 0 else list(range(SPAN + 8, SPAN + 16))
            expect = sorted(set(expect) | set(range(DATA, DATA + max(ln, 0))))
            require(sorted(set(reads)) == expect, f'unexpected reads for len {ln}')
            return r.rax
        got = C.two_poisons(once)
        require(got == (seed if ln <= 0 else fnv(data[:ln], seed)), f'machine code differs from reference FNV for len {ln}')
        want.append(got)
    args = [f'{s:x}:{ln}:{d.hex() or "00"}' for s, ln, d in cases]
    native = C.native('test_fnv_span_161f80', args)
    require(native == want, 'native fnv_span_161f80 differs from the interpreted machine code')
    C.readme_check('core/fnv_span_161f80.{hpp,cpp}', 'check_f161f80.py')
    print(f'OK fnv_span_161f80: 38-byte body sha256={SHA[:16]}... (14 insns, no call/stack, movzx + imul 0x1000193), '
          f'{len(cases)} cases incl. len<=0 and INT64_MIN match the interpreted machine code and the native lift')


if __name__ == '__main__':
    main()
