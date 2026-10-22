#!/usr/bin/env python3
"""Tie native/core/hashed_string_14c3a0 `copy_hashed_14c3a0` to original helper RVA 0x14c3a0 (private).

The 40-byte body is pinned (SHA-256, 12 instructions, no call) and INTERPRETED with x86_strict.py on a
poisoned destination: only hash (dword @0) and bytes @8..0x27 may be written (the 4 gap bytes at +4 must
stay untouched), `lock inc qword [ctl]` happens exactly when ctl != 0 (one locked 8-byte RMW), and the
results are compared with `test_hashed_string_14c3a0 --dump`. Also checks the counter wrap INT64_MAX.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402

require = C.require
RVA = 0x14c3a0
SRC, DST, CTR, TEXT = 0x6000_0000, 0x6000_1000, 0x6000_2000, 0x6000_3000


def scenario(ctx, poison, hash_, size, aux, ctl, counter, seed_rsi=0):
    m = ctx.machine(poison=poison, scratch=[(0x6000_0000, 0x4000)])
    m.poke(SRC, hash_.to_bytes(4, 'little') + b'\x5c' * 4 + TEXT.to_bytes(8, 'little') + (size & (2**64 - 1)).to_bytes(8, 'little')
           + aux.to_bytes(8, 'little') + ctl.to_bytes(8, 'little'))
    m.poke(DST, b'\xaa' * 0x28)
    m.poke(CTR, (counter & (2**64 - 1)).to_bytes(8, 'little'))
    m.run(RVA, {'rdi': DST, 'rsi': SRC})
    require(m.calls == [], 'copy_hashed made a call')
    exp = {0, 1, 2, 3} | set(range(8, 0x28))
    require({a - DST for a in C.writes_set(m) if DST <= a < DST + 0x28} == exp and not [a for a in C.writes_set(m) if not DST <= a < DST + 0x28 and not CTR <= a < CTR + 8],
            'unexpected write set')
    expect_acc = [('rmw', CTR, 8, True)] if ctl else []
    require(m.accesses == expect_acc, f'locked accesses {m.accesses}')
    return dict(hash=C.le(m, DST, 4), gap=C.hexat(m, DST + 4, 4), data=C.le(m, DST + 8, 8), size=C.le(m, DST + 16, 8),
                aux=C.le(m, DST + 24, 8), ctl=C.le(m, DST + 32, 8), counter=C.le(m, CTR, 8))


def main():
    ctx = C.Ctx()
    insns = ctx.pin_named(RVA)
    require(not any(i.mn in ('call', 'push', 'pop') for i in insns), 'unexpected call/stack use')
    require(sum(1 for i in insns if i.lock) == 1, 'expected exactly one locked instruction')

    nat = C.native('test_hashed_string_14c3a0')
    a = C.two_poisons(lambda p: scenario(ctx, p, 0xA1B2C3D4, 6, 0x1122334455667788, 0, 0))
    require(a['hash'] == nat['copy']['hash'] and a['gap'] == nat['copy']['gap'] == 'aaaaaaaa' and a['size'] == nat['copy']['size'] and
            a['aux'] == nat['copy']['aux'] and a['data'] == TEXT and nat['copy']['data_is_src'] and a['ctl'] == 0 and nat['copy']['ctl_null'],
            f'null-ctl copy differs: machine {a} native {nat["copy"]}')
    b = C.two_poisons(lambda p: scenario(ctx, p, 0xA1B2C3D4, 6, 0x1122334455667788, CTR, 100))
    require(b['counter'] == nat['copy_ctl']['counter'] == 101 and b['ctl'] == CTR and nat['copy_ctl']['ctl_is_src'], 'ctl copy differs')
    w = C.two_poisons(lambda p: scenario(ctx, p, 1, 0, 0, CTR, 2**63 - 1))
    require(w['counter'] == 2**63, 'counter did not wrap like lock inc qword')   # INT64_MAX+1 as unsigned bits = INT64_MIN
    C.readme_check('core/hashed_string_14c3a0.{hpp,cpp}', 'check_f14c3a0.py')
    print('OK copy_hashed_14c3a0: 40-byte body sha256=f58aaac0928d1cea... (12 insns, one lock inc qword [ctl] only when ctl != 0), '
          'dst writes = hash dword + bytes 8..0x27 (gap +4 untouched), native dump matches the interpreted machine code')


if __name__ == '__main__':
    main()
