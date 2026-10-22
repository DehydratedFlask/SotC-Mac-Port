#!/usr/bin/env python3
"""Tie native/core/hashed_string_14c3a0 `make_hashed_14c4a0` to original helper RVA 0x14c4a0 (private).

The 66-byte body is pinned (SHA-256, 22 instructions); its only direct call is 0x161f80 (also pinned and
tied by check_f161f80.py). The pinned bytes are INTERPRETED (callee included) with x86_strict.py: copies
0x20 bytes from rdx to this+8, `lock inc qword [ctl]` when ctl != 0, calls 0x161f80({ptr,len} at this+8,
0x811c9dc5) and stores eax at this+0. The incoming esi is never read: two different rsi values and two
poisons must give identical results. Compared with `test_hashed_string_14c3a0 --dump` (spans 6, 1, 0, -1).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402

require = C.require
RVA = 0x14c4a0
SRC, DST, CTR, TEXT = 0x6000_0000, 0x6000_1000, 0x6000_2000, 0x6000_3000


def scenario(ctx, poison, rsi, size, aux, counter):
    m = ctx.machine(poison=poison, scratch=[(0x6000_0000, 0x4000)])
    m.poke(TEXT, b'foobar')
    m.poke(SRC, TEXT.to_bytes(8, 'little') + (size & (2**64 - 1)).to_bytes(8, 'little') + aux.to_bytes(8, 'little') + CTR.to_bytes(8, 'little'))
    m.poke(DST, b'\xaa' * 0x28)
    m.poke(CTR, counter.to_bytes(8, 'little'))
    m.run(RVA, {'rdi': DST, 'rsi': rsi, 'rdx': SRC})
    require([(c[0], c[1]) for c in m.calls] == [('in-image', 0x161f80)], f'calls {m.calls}')
    require(m.calls[0][2]['rdi'] == DST + 8 and m.calls[0][2]['rsi'] == 0x811C9DC5, 'call arguments differ')
    wr = {a - DST for a in C.writes_set(m) if DST <= a < DST + 0x28}
    require(wr == {0, 1, 2, 3} | set(range(8, 0x28)), 'unexpected write set')
    require(all(DST <= a < DST + 0x28 or CTR <= a < CTR + 8 for a in C.writes_set(m)), 'write outside the destination / counter')
    require(m.accesses == [('rmw', CTR, 8, True)], f'locked accesses {m.accesses}')
    return dict(hash=C.le(m, DST, 4), gap=C.hexat(m, DST + 4, 4), aux=C.le(m, DST + 24, 8), counter=C.le(m, CTR, 8),
                data=C.le(m, DST + 8, 8), size=C.le(m, DST + 16, 8), ctl=C.le(m, DST + 32, 8))


def main():
    ctx = C.Ctx()
    ctx.pin_named(0x161f80)
    insns = ctx.pin_named(RVA)
    require([i.ops[0][1] for i in insns if i.mn == 'call'] == [0x161f80], 'unexpected call targets')
    nat = C.native('test_hashed_string_14c3a0')['make']
    for rec in nat:
        ln = rec['len']
        got = C.two_poisons(lambda p: scenario(ctx, p, 0x1234, ln, 0x0102030405060708, 5))
        alt = scenario(ctx, 0x00, 0xFFFFFFFFFFFFFFFF, ln, 0x0102030405060708, 5)
        require(got == alt, 'result depends on the incoming rsi/esi')
        require(got['hash'] == rec['hash'] and got['gap'] == rec['gap'] == 'aaaaaaaa' and got['aux'] == rec['aux'] and
                got['counter'] == rec['counter'] == 6 and got['data'] == TEXT and got['size'] == ln & (2**64 - 1) and got['ctl'] == CTR,
                f'len {ln}: machine {got} native {rec}')
    C.readme_check('check_f14c4a0.py')
    print(f'OK make_hashed_14c4a0: 66-byte body sha256=1737b4626bd4f2a2... (22 insns, one call 0x161f80 with esi=0x811c9dc5, incoming esi unused), '
          f'{len(nat)} spans (6, 1, 0, -1) match the interpreted machine code incl. call arguments and the locked ctl increment')


if __name__ == '__main__':
    main()
