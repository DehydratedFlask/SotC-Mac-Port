#!/usr/bin/env python3
"""Tie native/core/registry_object_14c9a0 `getter_14d880` to original guarded-static getter RVA 0x14d880.

Pins the 121-byte body (32 insns) and the atexit destructor 0x14cb70 (154 bytes, SHA only: its callees are
non-leaf with indirect vcalls, so it is NOT lifted). The pinned getter is INTERPRETED (callees included):
  cold   : cell 0x2ee3b00 null, guard 0x2ee3c10 = 0 -> runs 0x14c9a0(object, 0), registers
           __cxa_atexit(0x14cb70, object, __dso_handle) [documented omission], stores cell = object, `lock inc
           dword [object+0x30]` (counter 0 -> 1) and returns the object;
  early  : cell non-null -> returns the cell contents (even a foreign pointer), no write, no call, no lock;
  re-entry: cell null again with the guard already set -> no ctor re-run (a marker byte in the object survives),
           counter + 1, cell restored.
Deltas and return identities are compared with `test_registry_object_14c9a0 --dump` (getter block).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402
import x86_strict as X  # noqa: E402

require = C.require
RVA = 0x14d880
COUNTER = C.OBJ + 0x30


def main():
    ctx = C.Ctx()
    insns = ctx.pin_named(RVA)
    for r in (0x14c9a0, 0x14c3a0, 0x14c4a0, 0x161f80, 0xf730, 0x14cb70):
        ctx.pin_named(r)
    require([i.ops[0][1] for i in insns if i.mn == 'call'] == [0xf8f3d0, 0x14c9a0, 0xf8f390, 0xf8f3e0], 'call targets changed')
    require(sum(1 for i in insns if i.lock) == 1 and [i for i in insns if i.lock][0].mn == 'inc', 'expected exactly one lock inc')

    # ---- cold ---------------------------------------------------------------------------------------------------
    def cold(poison):
        m = ctx.machine(seed=C.cold_seed(0xAA, 0xAA), poison=poison)
        r = m.run(RVA)
        require(r.rax == C.OBJ, 'cold getter does not return the object')
        require(C.le(m, C.CELL, 8) == C.OBJ, 'cell not set')
        require(C.le(m, COUNTER, 4) == 1, 'counter after the first call is not 1 (ctor zeroes it, the inline lock inc makes it 1)')
        tr = C.import_trace(m)
        ats = [t for t in tr if t[0] == '__cxa_atexit']
        require(ats == [('__cxa_atexit', 0xf7b0, C.EMPTY, C.DSO), ('__cxa_atexit', 0x95610, C.HS, C.DSO), ('__cxa_atexit', 0x14cb70, C.OBJ, C.DSO)],
                f'atexit registrations {ats}')
        require(('__cxa_guard_acquire', C.OBJ_GUARD) == tr[0][:2], 'first call is not the object guard acquire')
        require(m.accesses[-1] == ('rmw', COUNTER, 4, True) and len([a for a in m.accesses if a[1] == COUNTER]) == 2, f'counter accesses {m.accesses}')
        require(C.hexat(m, C.OBJ_GUARD, 1) == '01', 'guard not released')
        return X.mem_bytes(m, C.OBJ, 0x100).hex(), tuple(tr)
    c = C.two_poisons(cold)

    # ---- early return: object already built, cell set -----------------------------------------------------------
    built = bytes.fromhex(c[0])
    seed = C.cold_seed(0, 0, {C.OBJ + k: b for k, b in enumerate(built)})
    seed.update({C.CELL + k: b for k, b in enumerate(C.OBJ.to_bytes(8, 'little'))})
    seed.update({C.OBJ_GUARD: 1, C.HS_GUARD: 1})
    me = ctx.machine(seed=seed, poison=0)
    re_ = me.run(RVA)
    require(re_.rax == C.OBJ and me.writes == [] and me.calls == [] and me.accesses == [], 'early return is not a pure load/test/ret')
    # a foreign pointer in the cell is returned as-is
    foreign = 0x6000_0000
    seed2 = dict(seed)
    seed2.update({C.CELL + k: b for k, b in enumerate(foreign.to_bytes(8, 'little'))})
    mf = ctx.machine(seed=seed2, poison=0, scratch=[(foreign, 0x100)])
    require(mf.run(RVA).rax == foreign and mf.writes == [], 'foreign cell pointer not returned unchanged')

    # ---- re-entry: cell null, guard set -> no ctor, counter + 1 -------------------------------------------------
    seed3 = dict(seed)
    seed3.update({C.CELL + k: 0 for k in range(8)})
    seed3[C.OBJ + 0x60] = 0x77                       # marker in a field the ctor would reset to 0
    mr = ctx.machine(seed=seed3, poison=0)
    rr = mr.run(RVA)
    require(rr.rax == C.OBJ and C.le(mr, C.CELL, 8) == C.OBJ, 're-entry did not restore the cell / return the object')
    require(C.le(mr, COUNTER, 4) == C.le(me, COUNTER, 4) + 1, 're-entry did not increment the counter once')
    require(C.hexat(mr, C.OBJ + 0x60, 1) == '77' and mr.calls == [], 'ctor was re-run on re-entry')
    require(C.writes_set(mr) == sorted([C.CELL + k for k in range(8)] + [COUNTER + k for k in range(4)]), 'unexpected re-entry write set')

    nat = C.native('test_registry_object_14c9a0')['getter']
    require(nat['r1_obj'] and nat['r2_obj'] and nat['c1'] - nat['c0'] == 0 and nat['c2'] - nat['c1'] == 1, f'native getter differs: {nat}')
    C.readme_check('check_f14d880.py')
    print('OK getter_14d880: 121-byte body sha256=b7586580add23db8... (32 insns) cold run = guard + 0x14c9a0(obj,0) + atexit(0x14cb70) [omitted natively] '
          '+ cell store + lock inc (counter 1); early return / foreign cell / null-cell re-entry (+1, no ctor) match native')


if __name__ == '__main__':
    main()
