#!/usr/bin/env python3
"""Tie native/core/hashed_string_14c3a0 `empty_string_f730` to original function-local static getter 0xf730.

Pins the 114-byte body (23 insns) and its atexit destructor 0xf7b0 (73 bytes, 26 insns). The pinned getter
is INTERPRETED cold (guard 0, BSS window 0x2dd9f50..0x2dd9f7f poisoned) and warm (guard 1): the cold run
must write exactly ptr (= object + 0x28), 24 zero bytes at +8..+0x1f and the guard byte, call
__cxa_guard_acquire / __cxa_atexit(0xf7b0, object, __dso_handle) / __cxa_guard_release and return the object;
the warm run writes and calls nothing. The destructor 0xf7b0 is interpreted on the built object (ctl = 0):
it must write nothing, call nothing and return -- this is the machine evidence for the documented omission
of the atexit registration. Compared with `test_hashed_string_14c3a0 --dump`.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402

require = C.require
RVA, OBJ, GUARD, BUF = 0xf730, 0x2dd9f50, 0x2dd9f70, 0x2dd9f78


def cold(ctx, poison, bg):
    seed = {a: bg for a in range(OBJ, OBJ + 0x30)}
    seed[GUARD] = 0
    m = ctx.machine(seed=seed, poison=poison)
    r = m.run(RVA)
    return m, r


def main():
    ctx = C.Ctx()
    insns = ctx.pin_named(RVA)
    require([i.ops[0][1] for i in insns if i.mn == 'call'] == [0xF8F3D0, 0xF8F390, 0xF8F3E0], 'call targets changed')
    ctx.pin_named(0xf7b0)

    nat = C.native('test_hashed_string_14c3a0')['empty']
    for bg in (0xAA, 0x00):
        def once(poison):
            m, r = cold(ctx, poison, bg)
            require(r.rax == OBJ, 'getter does not return the object')
            require([c[0] for c in m.calls] == ['__cxa_guard_acquire', '__cxa_atexit', '__cxa_guard_release'], f'calls {m.calls}')
            require(m.calls[0][2]['rdi'] == GUARD and m.calls[2][2]['rdi'] == GUARD, 'guard address differs')
            require((m.calls[1][2]['rdi'], m.calls[1][2]['rsi'], m.calls[1][2]['rdx']) == (0xf7b0, OBJ, C.DSO), 'atexit arguments differ')
            ws = {a for a in C.writes_set(m)}
            require(ws == set(range(OBJ, OBJ + 0x20)) | {GUARD}, f'write set {sorted(hex(a) for a in ws)}')
            return C.hexat(m, OBJ, 0x38)
        image = C.two_poisons(once)
        b = bytes.fromhex(image)
        require(int.from_bytes(b[0:8], 'little') == BUF and b[8:0x20] == bytes(0x18), 'cold object differs')
        require(b[0x20] == 1 and b[0x21:0x30] == bytes([bg]) * 0x0f, 'guard byte (+0x20) / bytes after it differ')
        require(b[0x28] == bg, 'the byte ptr designates (+0x28) was written')
    m, r = cold(ctx, 0, 0x00)
    require(C.hexat(m, GUARD, 1) == '01', 'guard not set')
    require(BUF - OBJ == nat['data_offset'] == 0x28 and nat['size'] == 0 and nat['aux'] == 0 and nat['ctl_null'] and nat['nul_byte'] == 0 and
            nat['same_address'], 'native static differs')
    require(C.le(m, OBJ + 8, 8) == nat['size'] and C.le(m, OBJ + 16, 8) == nat['aux'] and C.le(m, OBJ + 24, 8) == 0 and C.hexat(m, BUF, 1) == '00' == '%02x' % nat['nul_byte'],
            'object fields differ')

    # warm: guard already 1 -> no write, no call, same return
    mw = ctx.machine(seed={GUARD: 1}, poison=0)
    rw = mw.run(RVA)
    require(rw.rax == OBJ and mw.writes == [] and mw.calls == [], 'warm path is not a pure lea')
    # exit-time destructor on the built object (ctl == 0): nothing happens
    md = ctx.machine(seed={a: 0 for a in range(OBJ, OBJ + 0x30)}, poison=0)
    md.run(0xf7b0, {'rdi': OBJ})
    require(md.writes == [] and md.calls == [] and md.accesses == [], 'dtor 0xf7b0 is not a no-op for ctl == 0')
    C.readme_check('check_ff730.py')
    print('OK empty_string_f730: 114-byte body sha256=03645245094b2faf... (23 insns), cold run writes ptr(+0x28)/zeros/guard and registers '
          'atexit(0xf7b0), warm run is a pure lea, dtor 0xf7b0 (73 bytes) is a no-op for ctl == 0; native static matches')


if __name__ == '__main__':
    main()
