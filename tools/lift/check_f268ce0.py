#!/usr/bin/env python3
"""Tie native/core/guarded_static_helpers (ctor 0x268ce0 + helper 0x268d40) to the pinned ELF.

Private; needs local/re/eboot.elf. See guarded_static_check.py. Every rip-relative reference to the static
byte 0x2eeec88 and its guard 0x2eeec90 is classified: the only writers are compiler-emitted copies of the
initialiser `mov byte [0x2eeec88],0xff`; the native dump is compared byte-for-byte.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import guarded_static_check as c  # noqa: E402

require = c.require
CELL, STATIC, GUARD = 0x2eeebac, 0x2eeec88, 0x2eeec90


def writer_check(code, v0):
    stores = loads = 0
    for i, extra in c.xrefs(code, v0, STATIC):
        if code[i - 2:i].hex() == 'c605':
            require(extra == 1 and code[i + 4] == 0xFF, f'static store at {v0 + i:#x} is not imm8 0xff')
            stores += 1
        else:
            require(extra == 0 and code[i - 2:i].hex() == '8a05', f'unclassified reference to {STATIC:#x} at {v0 + i:#x}')
            loads += 1
    g = 0
    for i, extra in c.xrefs(code, v0, GUARD):
        require(extra == 0 and (code[i - 2:i].hex() == '8a05' or code[i - 3:i].hex() == '488d3d'),
                f'unclassified reference to guard {GUARD:#x} at {v0 + i:#x}')
        g += 1
    require(stores >= 1 and loads >= 1, 'expected initialiser stores and loads')
    print(f'  static scan: {stores} initialiser copies (mov byte [{STATIC:#x}],0xff), {loads} loads, {g} guard refs, no other writer')


SPEC = dict(
    ctor=0x268ce0, ctor_length=17, ctor_sha256='557e81a9bd03b19f6cb6a0b25d5a471a13bd319295e094e12776f27abb0664e3',
    helper=0x268d40, helper_length=57, helper_sha256='047fc327bae3f471ebd5b1bc5a435094bafc940b8f5d4e47106d68cd1b379eab',
    helper_calls=[0xF8F3D0, 0xF8F3E0], ctor_index=496, guard=GUARD,
    poison=[CELL - 1, CELL, CELL + 1, STATIC],
    written=sorted([GUARD, CELL, STATIC]),
    warm_written=[CELL],
    writer_check=writer_check,
)


def main():
    cold, warm = c.run(SPEC)
    n = c.native()
    require(cold.rd(STATIC) == 0xFF == n['flag_first'] == n['flag_second'], 'static flag differs')
    require(c.hexat(cold, CELL - 1, 3) == n['ctor_268ce0'] == 'aaffaa', 'ctor cell or its neighbours differ')
    require(cold.rd(GUARD) == 1, 'guard not set')
    require(warm.rd(CELL) == 0xAA, 'warm model did not read the (poisoned) static')
    c.readme_check('268ce0')
    print('OK ctor_268ce0: ctor 17 bytes (call 0x268d40; mov [cell],al), helper 57 bytes (guard acquire, mov byte 0xff, '
          'guard release); cold run stores 0xff, native dump matches')


if __name__ == '__main__':
    main()
