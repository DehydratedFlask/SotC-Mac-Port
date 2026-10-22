#!/usr/bin/env python3
"""Tie native/core/registry_object_14c9a0 `init_ctor_b25470` to original startup ctor RVA 0xb25470.

Pins the 105-byte body (23 insns): 400 iterations (rbx 0x110 .. 0x1b690 step 0x118) over the BSS array 0x332f650,
each calling 0x14d880, storing the result at +0x108, clearing bytes +0x110 and +0x00 and calling 0x14ccb0;
the body ends by tail-jumping to __cxa_atexit(0xb24be0, 0, __dso_handle) [documented omission natively].
INTERPRETED cold with the array window poisoned 0xAA: every slot must end byte-identical to the native slot (flags
zero, node pointer = object, 0xAA elsewhere), the object counter must be 401 (1 from the first getter call + 400
add_refs) and the machine's write set must lie in the array, the object window, the two static windows, the guards and the cell.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x86_check_common as C  # noqa: E402
import x86_strict as X  # noqa: E402

require = C.require
RVA = 0xb25470


def main():
    ctx = C.Ctx()
    insns = ctx.pin_named(RVA)
    for r in (0x14d880, 0x14c9a0, 0x14c3a0, 0x14c4a0, 0x161f80, 0xf730, 0x14cb70):
        ctx.pin_named(r)
    require([i.ops[0][1] for i in insns if i.mn == 'call'] == [0x14d880, 0x14ccb0], 'call targets changed')
    require(insns[-1].mn == 'jmp' and insns[-1].ops[0][1] == 0xf8f390, 'tail is not the __cxa_atexit thunk')
    cmp_ = [i for i in insns if i.mn == 'cmp'][0]
    require(cmp_.ops[1][1] == 0x1b690 and [i for i in insns if i.mn == 'add' and i.ops[1][1] == C.STRIDE] and
            insns[1].mn == 'mov' and any(i.mn == 'mov' and i.ops[1] == ('imm', 0x110) for i in insns), 'loop bounds changed')
    require((0x1b690 - 0x110) % C.STRIDE == 0 and (0x1b690 - 0x110) // C.STRIDE == 400 and C.ARRAY_BYTES == 400 * C.STRIDE, 'iteration count is not 400')

    leaf = ctx.image.read(0x14ccb0, 5)
    require(leaf.hex() == 'f0ff4730c3', '0x14ccb0 bytes changed')   # lock inc dword [rdi+0x30] ; ret (tied by check_f14ccb0.py)
    nat = C.native('test_registry_object_14c9a0')['b25470']
    require(nat['count'] == 400 and nat['stride'] == C.STRIDE and nat['slots_identical'] and nat['nodes_all_object'] and nat['cell_is_object'], 'native b25470 block differs')

    def once(poison):
        seed = C.cold_seed(0, 0, {C.ARRAY + k: 0xAA for k in range(C.ARRAY_BYTES)})
        mm = ctx.machine(seed=seed, poison=poison)
        mm.run(RVA)
        return (None, X.mem_bytes(mm, C.ARRAY, C.ARRAY_BYTES).hex(), tuple(C.import_trace(mm)), C.le(mm, C.OBJ + 0x30, 4), tuple(C.writes_set(mm)))
    m = C.two_poisons(once)
    slots_hex, trace, counter, writes = m[1], m[2], m[3], m[4]
    raw = bytes.fromhex(slots_hex)
    first = None
    for i in range(400):
        s = raw[i * C.STRIDE:(i + 1) * C.STRIDE]
        node = int.from_bytes(s[0x108:0x110], 'little')
        require(node == C.OBJ, f'slot {i}: node {node:#x}')
        mh = C.masked_hex(s, (0x108,))
        first = first or mh
        require(mh == first, f'slot {i} differs from slot 0')
    require(first == nat['slot0'], 'native slot image differs from the interpreted machine code')
    require(counter == nat['counter'] == 401, f'counter {counter} vs native {nat["counter"]}')
    ats = [t for t in trace if t[0] == '__cxa_atexit']
    require(ats == [('__cxa_atexit', 0xf7b0, C.EMPTY, C.DSO), ('__cxa_atexit', 0x95610, C.HS, C.DSO), ('__cxa_atexit', 0x14cb70, C.OBJ, C.DSO),
                    ('__cxa_atexit', 0xb24be0, 0, C.DSO)], f'atexit registrations {ats}')
    require(trace.count(('in-image', 0x14d880)) == 400 and trace.count(('in-image', 0x14ccb0)) == 400 and trace.count(('in-image', 0x14c9a0)) == 1,
            'call counts differ (400 getter calls, 400 add_refs, one ctor)')
    ok = [a for a in writes if not (C.ARRAY <= a < C.ARRAY + C.ARRAY_BYTES or C.OBJ <= a < C.OBJ + 0x100 or C.HS <= a < C.HS + 0x28 or C.EMPTY <= a < C.EMPTY + 0x30
                                    or a in (C.OBJ_GUARD, C.HS_GUARD, C.EMPTY_GUARD) or C.CELL <= a < C.CELL + 8)]
    require(ok == [], f'writes outside the known windows: {[hex(a) for a in ok[:5]]}')
    arr_written = {a - C.ARRAY for a in writes if C.ARRAY <= a < C.ARRAY + C.ARRAY_BYTES}
    require(arr_written == {i * C.STRIDE + o for i in range(400) for o in [0, 0x110] + list(range(0x108, 0x110))}, 'array write set differs')
    C.readme_check('check_fb25470.py')
    print('OK init_ctor_b25470: 105-byte body sha256=8e2300c708f4ab24... (23 insns), 400 slots (stride 0x118) interpreted cold: node/flags/untouched bytes '
          'match native, counter 401, atexit registrations f7b0/95610/14cb70/b24be0 recorded as omitted natively')


if __name__ == '__main__':
    main()
