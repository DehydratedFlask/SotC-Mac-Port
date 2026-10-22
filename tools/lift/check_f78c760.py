#!/usr/bin/env python3
"""Tie native/core/guarded_static_helpers (ctor 0x78c760 + helpers 0xa77960 and 0x51230) to the pinned ELF.

Private; needs local/re/eboot.elf. See guarded_static_check.py. The pinned bytes are interpreted cold and
warm; every rip-relative reference to the static pair 0x32fb190/0x32fb198 and its guard 0x32fb1a0 is
classified (the writers are compiler-emitted copies of the same inline function, all storing the
result of 0x51230); the native dump is compared byte-for-byte.
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import guarded_static_check as c  # noqa: E402

require = c.require
CELL, STATIC, GUARD = 0x3256378, 0x32fb190, 0x32fb1a0


def writer_check(code, v0):
    def rel_target(at):
        return v0 + at + 5 + struct.unpack_from('<i', code, at + 1)[0]

    stores = 0
    for i, extra in c.xrefs(code, v0, STATIC):
        require(extra == 0, f'unexpected trailing imm at {i:#x}')
        p3, p2, p4 = c.classify(code, i, extra)
        if p3 == '488905':     # mov [static],rax  preceded by `call 0x51230 ; lea rdi,[guard]`
            require(code[i - 15] == 0xE8 and rel_target(i - 15) == 0x51230 and code[i - 10:i - 7].hex() == '488d3d',
                    f'store at {v0 + i:#x} does not follow call 0x51230')
            stores += 1
        elif p3 == '488d05' or p4 == 'c5f81005':   # lea rax,[static] / vmovups xmm0,[static]: reads
            pass
        else:
            require(False, f'unclassified reference to {STATIC:#x} at {v0 + i:#x}: {p4}')
    hi = 0
    for i, extra in c.xrefs(code, v0, STATIC + 8):
        require(extra == 0 and code[i - 3:i].hex() == '488915' and code[i - 10:i - 7].hex() == '488905',
                f'unclassified reference to {STATIC + 8:#x} at {v0 + i:#x}')
        hi += 1
    g = 0
    for i, extra in c.xrefs(code, v0, GUARD):
        require(extra == 0 and (code[i - 2:i].hex() == '8a05' or code[i - 3:i].hex() == '488d3d'),
                f'unclassified reference to guard {GUARD:#x} at {v0 + i:#x}')
        g += 1
    require(stores == hi and stores >= 1, f'low/high store counts differ: {stores}/{hi}')
    print(f'  static scan: {stores} copies of the initialiser (call 0x51230 -> stores {STATIC:#x}/{STATIC + 8:#x}), {g} guard refs, no other writer')


SPEC = dict(
    ctor=0x78c760, ctor_length=26, ctor_sha256='67b806dcde286737a40c9f6f70c2882f03ad3ac7e8b891036030746d97cd2142',
    helper=0xa77960, helper_length=70, helper_sha256='a8bf1d8ae585f1a974a46558b5ca0bec4f3a3e48646ec16d7108d011261b6446',
    helper_calls=[0xF8F3D0, 0x51230, 0xF8F3E0], ctor_index=357, guard=GUARD,
    poison=[CELL + k for k in range(-16, 32)] + [STATIC + k for k in range(16)],
    written=sorted([GUARD] + [CELL + k for k in range(16)] + [STATIC + k for k in range(16)]),
    warm_written=sorted(CELL + k for k in range(16)),
    writer_check=writer_check,
)


def main():
    cold, warm = c.run(SPEC)
    n = c.native()
    zero = '00' * 16
    # machine code, cold: static pair becomes {0,0}, the cell receives it, guard set, neighbours untouched
    require(c.hexat(cold, STATIC, 16) == zero == n['pair_first'] == n['pair_second'] == n['zero_pair'], 'static pair differs')
    require(c.hexat(cold, CELL, 16) == n['ctor_78c760_cell'] == zero, 'ctor cell differs')
    require(c.hexat(cold, CELL - 16, 16) == n['ctor_78c760_before'] == 'aa' * 16, 'bytes before the cell changed')
    require(c.hexat(cold, CELL + 16, 16) == n['ctor_78c760_after'] == 'aa' * 16, 'bytes after the cell changed')
    require(cold.rd(GUARD) == 1 and n['pair_same_address'], 'guard/singleton differs')
    # warm with an initialised guard copies whatever the static holds: the lift only models the reachable
    # sequence (static is initialised before the guard is set); the poison proves the model reads the static.
    require(c.hexat(warm, CELL, 16) == 'aa' * 16, 'warm model did not copy the (poisoned) static')
    c.readme_check('78c760')
    print('OK ctor_78c760: ctor 26 bytes (call 0xa77960; vmovups load/store), helper 70 bytes (guard acquire, call 0x51230, '
          'guard release), 0x51230 = xor eax,eax; xor edx,edx; ret; cold run gives zero pair, native dump matches')


if __name__ == '__main__':
    main()
