#!/usr/bin/env python3
"""Tie native/core/registry_object_14c9a0 `ctor_14c9a0` (parent == nullptr ONLY) to original RVA 0x14c9a0.

Pins the 464-byte body (115 insns) and its callees 0xf730, 0x14c4a0, 0x14c3a0, 0x161f80. The pinned code is
INTERPRETED cold (all three guards 0; object window poisoned 0xAA, and zero BSS) and warm (hashed-static
guard 1): exact object image (pointer slots compared as offsets), the exact set of locked RMWs (the three
xchg stores at +0x30/+0x40/+0x50; no `lock inc` because every ctl is null), the exact program-order trace of
imports (guards, atexit(0xf7b0 / 0x95610) registrations) and in-image callees. The parent path is checked
structurally: its code (0x14cafa..0x14cb60, containing call 0xf590 and `jmp rax` through the manager cell
0x2ee2d00 vtable) is reachable only through the `test r14,r14 ; je 0x14cb61` at 0x14caf5; the manager cell is
never read when parent == null; and interpreting with parent != null cannot complete (the model refuses the
indirect transfer) -- the native lift aborts instead (test_registry_object_14c9a0, SIGABRT).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402
import x86_strict as X  # noqa: E402

require = C.require
RVA = 0x14c9a0
WRITTEN = [(0x00, 0x28), (0x30, 4), (0x40, 4), (0x50, 8), (0x60, 4), (0x68, 8), (0x70, 4), (0x78, 0x20), (0x98, 4), (0xa8, 0x20),
           (0xc8, 4), (0xd0, 0x20), (0xf0, 4), (0xf4, 4), (0xf8, 1)]


def run_cold(ctx, poison, obj_bg, side_bg):
    m = ctx.machine(seed=C.cold_seed(obj_bg, side_bg), poison=poison)
    m.run(RVA, {'rdi': C.OBJ, 'rsi': 0})
    return m


def obj_view(m):
    raw = X.mem_bytes(m, C.OBJ, 0x100)
    rel = lambda off: int.from_bytes(raw[off:off + 8], 'little') - (C.OBJ if off in (8, 0x10) else C.EMPTY)   # noqa: E731
    return dict(bytes=C.masked_hex(raw, C.SLOTS_MASK), self08=rel(8), self10=rel(0x10), p78=rel(0x78), pd0=rel(0xd0))


def main():
    ctx = C.Ctx()
    insns = ctx.pin_named(RVA)
    for r in (0x161f80, 0x14c3a0, 0x14c4a0, 0xf730, 0xf7b0, 0x95610):
        ctx.pin_named(r)
    ctx.pin_named(0x14cb70)

    calls = sorted({i.ops[0][1] for i in insns if i.mn == 'call'})
    require(calls == sorted([0xf730, 0x14c4a0, 0x14c3a0, 0xf590, 0xf8f3d0, 0xf8f390, 0xf8f3e0]), f'call targets {[hex(c) for c in calls]}')
    # ---- parent path: reachable only via the test/je at 0x14caf5 ------------------------------------------
    by_pc = {i.pc: i for i in insns}
    require(by_pc[0x14caf5].mn == 'test' and by_pc[0x14caf8].mn == 'je' and by_pc[0x14caf8].ops[0][1] == 0x14cb61, 'parent test/je changed')
    require(by_pc[0x14cb5f].mn == 'jmp_reg' and by_pc[0x14cb61].mn == 'add' and by_pc[0x14cb6f].mn == 'ret', 'parent tail / epilogue layout changed')
    for i in insns:
        if i.mn in ('je', 'jne', 'jb', 'jle', 'jmp') and i.pc < 0x14cafa:
            require(not 0x14cafa <= i.ops[0][1] < 0x14cb61, f'branch at {i.pc:#x} enters the parent-only region')
    require([i.pc for i in insns if i.mn == 'call' and i.ops[0][1] == 0xf590] == [0x14cb0e], 'call 0xf590 is not parent-only')

    nat = C.native('test_registry_object_14c9a0')
    snapshots = {}
    for obj_bg, key in ((0xAA, 'ctor_aa'), (0x00, 'ctor_00')):
        def once(poison):
            m = run_cold(ctx, poison, obj_bg, obj_bg)
            ws = {a - C.OBJ for a in C.writes_set(m) if C.OBJ <= a < C.OBJ + 0x100}
            require(ws == {o + k for o, n in WRITTEN for k in range(n)}, f'object write set differs: {sorted(ws)}')
            require(m.accesses == [('rmw', C.OBJ + 0x30, 4, True), ('rmw', C.OBJ + 0x40, 4, True), ('rmw', C.OBJ + 0x50, 8, True)],
                    f'locked accesses {m.accesses}')
            require(not any(a <= C.MANAGER < a + w for a, w in m.reads), 'manager cell 0x2ee2d00 read on the parent == null path')
            require(all(not (C.MANAGER <= a < C.MANAGER + 8) for a in C.writes_set(m)), 'manager cell written')
            ok = [a for a in C.writes_set(m) if not (C.OBJ <= a < C.OBJ + 0x100 or C.HS <= a < C.HS + 0x28 or C.EMPTY <= a < C.EMPTY + 0x30
                                                     or a in (C.HS_GUARD, C.EMPTY_GUARD))]
            require(ok == [], f'writes outside the known windows: {ok}')
            tr = C.import_trace(m)
            require(tr == [('__cxa_guard_acquire', C.HS_GUARD), ('in-image', 0xf730),
                           ('__cxa_guard_acquire', C.EMPTY_GUARD),
                           ('__cxa_atexit', 0xf7b0, C.EMPTY, C.DSO), ('__cxa_guard_release', C.EMPTY_GUARD),
                           ('in-image', 0x14c4a0), ('in-image', 0x161f80),
                           ('__cxa_atexit', 0x95610, C.HS, C.DSO), ('__cxa_guard_release', C.HS_GUARD),
                           ('in-image', 0x14c3a0), ('in-image', 0x14c3a0)], f'trace {tr}')
            v = obj_view(m)
            v['hs'] = X.mem_bytes(m, C.HS, 0x28).hex()
            return v
        v = C.two_poisons(once)
        n = nat[key]
        require(v['bytes'] == n['bytes'] and v['self08'] == v['self10'] == 0 == n['self08'] == n['self10'] and v['p78'] == v['pd0'] == 0x28 == n['p78'] == n['pd0'],
                f'{key}: native object differs from the interpreted machine code')
        snapshots[key] = v
    # static hashed empty string and the 0xf730 object, from the zero-BSS run
    m = run_cold(ctx, 0, 0, 0)
    hs, e = X.mem_bytes(m, C.HS, 0x28), X.mem_bytes(m, C.EMPTY, 0x20)
    require(C.masked_hex(hs, (8,)) == nat['hs']['bytes'] and int.from_bytes(hs[8:16], 'little') - C.EMPTY == nat['hs']['p'] == 0x28, 'static hashed string differs')
    require(int.from_bytes(hs[0:4], 'little') == 0x811c9dc5, 'hashed empty string hash is not the FNV offset basis')
    require(C.masked_hex(e, (0,)) == nat['empty']['bytes'] and int.from_bytes(e[0:8], 'little') - C.EMPTY == nat['empty']['p'] == 0x28, 'empty string object differs')
    first_bytes = X.mem_bytes(m, C.OBJ, 0x100)

    # ---- warm: hashed static already built, guards set -------------------------------------------------------
    warm_seed = C.cold_seed(0xAA, 0xAA, {C.HS_GUARD: 1, **{C.HS + k: b for k, b in enumerate(hs)}, **{C.EMPTY + k: b for k, b in enumerate(e)}})
    mw = ctx.machine(seed=warm_seed, poison=0)
    mw.run(RVA, {'rdi': C.OBJ, 'rsi': 0})
    require(C.import_trace(mw) == [('in-image', 0x14c3a0), ('in-image', 0x14c3a0)], f'warm trace {C.import_trace(mw)}')
    require(C.masked_hex(X.mem_bytes(mw, C.OBJ, 0x100), C.SLOTS_MASK) == snapshots['ctor_aa']['bytes'], 'warm object differs from cold')

    # ---- parent != null cannot be interpreted to completion (indirect manager transfer) ------------------------
    scratch = [(0x6000_0000, 0x1000)]
    mp = ctx.machine(seed=C.cold_seed(0xAA, 0, {}), poison=0, scratch=scratch)
    try:
        mp.run(RVA, {'rdi': C.OBJ, 'rsi': 0x6000_0000})
    except X.DecodeError as exc:
        why = str(exc)
    else:
        require(False, 'interpreter completed the parent != null path (it is not lifted and must not be modelled)')
    C.readme_check('core/registry_object_14c9a0.{hpp,cpp}', 'check_f14c9a0.py')
    print('OK ctor_14c9a0 (parent==nullptr): 464-byte body sha256=91c83a0d5d613fa2... (115 insns) cold(0xAA/0)/warm interpreted; object image, '
          '3 locked xchg stores, guard/atexit trace and manager cell untouched match native; parent path structurally isolated '
          f'(interpretation stops: {why[:60]})')


if __name__ == '__main__':
    main()
