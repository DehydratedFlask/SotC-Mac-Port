#!/usr/bin/env python3
"""Tie native constructor registration thunks (slots 12-19) to the pinned ELF by traced effects.

For each of the eight thunks this replays the original bytes on tools/lift/thunk_trace.py (an
offline abstract machine, never part of the product) and asserts, from the bytes:

  * the thunk body digest;
  * the ONLY externally visible effects are: store of the vtable address point V into .bss global A,
    malloc(0x10) (the size word at 0x12d2d1c), store of that pointer into A+0x10, the recursive (2) +
    priority-inherit (1) pthread mutex initialisation, and __cxa_atexit(0xe44a10, A+0x10, 0x1604218);
  * that every class-specific stack probe (base ctor / derived ctor / virtual slot-0 destructor /
    inline destructor) reached from the thunk touches nothing outside its own stack frame, i.e. it
    is effect-free when run as the thunk runs it (owner = null, field20 = field28 = 0x10);
  * that the native table in native/core/registration_thunks.hpp equals the traced values (thunk RVA
    from the ELF constructor table, V, A, probe shape).

It also runs two negative controls on the tracer itself so that "no effect" cannot mean "the tracer
skipped the calls". Exit 0 only when every assertion holds. Nothing here is linked into the product.
"""
import hashlib
import importlib.util
import re
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import ps4_dynlink as d  # noqa: E402
import thunk_trace as tt  # noqa: E402

TABLE_START = 0x2dd70e0
BASE_CTOR, BASE_DTOR = 0xe1eda0, 0xe1ede0
CLEANUP, DSO = 0xe44a10, 0x1604218
CLEANUP_SHA256 = 'd3ceea8ac71a4632f3bc86ab9a12b7e9de941ffced937951c07342206072875c'  # 31 bytes

# index: (thunk rva, body size, body sha256, global A, vtable address point V, shape, callee)
EXPECTED = {
    12: (0xf06340, 205, 'a16f79f44e9fa082e8e7a25d300210dd6538aa4f5f1341d66a6e30769b3dcb53',
         0x3355f88, 0x15f9f00, 'base_only', None),
    13: (0xf058f0, 79, 'd021c860c421a25ee64f18d6007daa897d963d4a9be62d51928061b0eee6675b',
         0x3355470, 0x15f9298, 'callee_inline', 0xf030e0),
    14: (0xf02ff0, 239, '0c803e076d6f24f81b7cb9cab7deca5e633c4e24f9041350455f87a88f83b7fa',
         0x3355028, 0x15f8670, 'vslot0_dtor', 0xf011f0),
    15: (0xf01100, 226, '006d8ef333df1b54dde77fcd144deec4277711d05567fe36b20cc31804be7d24',
         0x3354e00, 0x15f7a28, 'base_only', None),
    16: (0xf00090, 284, 'aed062395020b9ce82f5c04c640cd1b42ba83a844c84ac9bdbbfda9fbc2ca5a0',
         0x3354b58, 0x15f6dc0, 'vslot0_dtor', 0xefe410),
    17: (0xefe320, 239, 'd157e4bf9c9e7f3900051edb9659906cca7c45c454af68a29d05c73ec9b2a612',
         0x33549f0, 0x15f63b8, 'vslot0_dtor', 0xefd140),
    18: (0xefd020, 281, 'a14f6aa8db166697a1cc3eb0ad2feb49c15c482e725864333c748335b9492638',
         0x3354660, 0x15f54f8, 'vslot0_dtor', 0xefad20),
    19: (0xefacd0, 79, '2750b1530f9edd2b561c213b7c6be699443617bbc464e0769bfc1910bfa51f6e',
         0x3354190, 0x15f4448, 'callee_inline', 0xef8600),
}
HEADER = ROOT / 'native/core/registration_thunks.hpp'


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def load_analyzer():
    spec = importlib.util.spec_from_file_location('slot_struct', ROOT / 'tools/lift/analyze_slot_struct.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.WORK = Path(tempfile.mkdtemp(prefix='thunk-check-'))
    module._cache.clear()
    return module


def q(image, addr):
    return int.from_bytes(image.read(addr, 8), 'little')


def check_slot(analyzer, image, name_by_nid, nid_at, index, want):
    rva, size, sha, glob_a, vptr, shape, callee = want
    tag = f'slot {index} {rva:#x}'
    require(hashlib.sha256(image.read(rva, size)).hexdigest() == sha, f'{tag}: body digest changed')

    analyzer._cache.clear()
    t = tt.make_tracer(analyzer, image, name_by_nid, nid_at)
    try:
        t.run(rva)
    except tt.Fault as exc:
        raise SystemExit(f'FAIL {tag}: tracer fault: {exc}')

    events = [e for e in t.events
              if not (e[0] == 'memset' and tt.STACK_LO <= e[1] < tt.STACK_TOP + 0x1000)]
    require(len(events) == 9, f'{tag}: {len(events)} visible effects, expected 9: {events}')
    # 1. publish the vtable address point
    require(events[0][:4] == ('store', glob_a, 8, vptr), f'{tag}: first effect {events[0]}')
    # 2. malloc(*[0x12d2d1c]) == malloc(0x10)
    require(image.read(0x12d2d1c, 4) == b'\x10\x00\x00\x00', f'{tag}: size word is not 0x10')
    require(events[1] == ('malloc', 0x10), f'{tag}: malloc effect {events[1]}')
    heap = events[2][3]
    require(events[2][:3] == ('store', glob_a + 0x10, 8), f'{tag}: second global {events[2]}')
    require(tt.HEAP_BASE <= heap < tt.HEAP_BASE + 0x100000, f'{tag}: published pointer is not the malloc block')
    # 3. recursive (2) + priority-inherit (1) mutex, attribute object reused for init
    attr = events[3][1]
    require(events[3] == ('pthread_mutexattr_init', attr), f'{tag}: {events[3]}')
    require(events[4] == ('pthread_mutexattr_settype', attr, 2), f'{tag}: {events[4]}')
    require(events[5] == ('pthread_mutexattr_setprotocol', attr, 1), f'{tag}: {events[5]}')
    require(events[6] == ('pthread_mutex_init', heap, attr), f'{tag}: {events[6]}')
    require(events[7] == ('pthread_mutexattr_destroy', attr), f'{tag}: {events[7]}')
    # 4. cleanup registration
    require(events[8] == ('atexit', CLEANUP, glob_a + 0x10, DSO), f'{tag}: atexit effect {events[8]}')

    # Vtable identity: [V-0x10] and [V-8] are zero (no RTTI), [V] is the slot-0 destructor in the image.
    require(q(image, vptr - 0x10) == 0 and q(image, vptr - 8) == 0, f'{tag}: unexpected vtable header')
    dtor = q(image, vptr)
    require(0 < dtor < 0x1000000, f'{tag}: vtable slot 0 {dtor:#x} is not an image code address')

    # Probe shape, from the executed call sequence.
    calls = t.calls
    base_ctors = [c for c in calls if c[2] == BASE_CTOR]
    base_dtors = [c for c in calls if c[2] == BASE_DTOR]
    require(len(base_ctors) == 1 and len(base_dtors) == 1, f'{tag}: base ctor/dtor not each called once')
    for c in base_ctors:
        require(c[3][1] == 0 and c[3][2] == 0x10 and c[3][3] == 0x10,
                f'{tag}: base ctor args (owner, field20, field28) = {c[3][1:]}')
    indirect = [c for c in calls if c[0] in ('call-indirect', 'tailjmp-indirect')]
    if shape == 'base_only':
        require(not indirect and calls[0][2] == BASE_CTOR and calls[1][2] == BASE_DTOR,
                f'{tag}: base_only shape violated: {calls[:3]}')
    elif shape == 'callee_inline':
        require(calls[0][2] == callee and not indirect, f'{tag}: callee_inline shape violated')
        require(base_ctors[0][1] > callee, f'{tag}: base ctor not inside the callee')
    elif shape == 'vslot0_dtor':
        require(calls[0][2] == callee, f'{tag}: first call is not the class constructor {callee:#x}')
        require(len(indirect) == 1 and indirect[0][0] == 'call-indirect' and indirect[0][2] == dtor,
                f'{tag}: expected exactly one indirect call to vtable slot 0 {dtor:#x}: {indirect}')
        # The derived constructor receives the same (owner=0, 0x10, 0x10) arguments.
        require(calls[0][3][1:] == (0, 0x10, 0x10), f'{tag}: class ctor args {calls[0][3]}')
    else:
        raise SystemExit(f'FAIL {tag}: unknown shape {shape}')
    return t, dtor


def negative_controls(analyzer, image, name_by_nid, nid_at):
    """The tracer must expose a vcall when the guard bytes allow it, and no call when they forbid it."""
    obj, owner, vtbl = tt.STACK_LO + 0x1000, tt.STACK_LO + 0x2000, tt.STACK_LO + 0x3000
    # (a) release_blocks-shaped 0xf01610 with owns=1, ptr=0x1234, a fake owner: must reach the vcall.
    t = tt.make_tracer(analyzer, image, name_by_nid, nid_at)
    for off in (0x50, 0x80, 0x98, 0xb0, 0xd0, 0xf0):
        t.write(obj + off, 1, 0, 0)
    t.write(obj + 0x50, 1, 1, 0)
    t.write(obj + 0x48, 8, 0x1234, 0)
    t.write(obj + 8, 8, owner, 0)
    t.write(owner, 8, vtbl, 0)
    t.write(vtbl + 0xc0, 8, 0xbadf00d, 0)
    try:
        t.run(0xf01610, {'rdi': obj})
    except tt.Fault:
        pass
    else:
        raise SystemExit('FAIL negative control (a): tracer did not fault on the fake owner target')
    last = t.calls[-1]
    require(last[0] == 'call-indirect' and last[2] == 0xbadf00d and last[3][1] == 0x1234,
            f'negative control (a): vcall not observed: {last}')
    # (b) the same function with every owns byte zero must return without any call.
    t = tt.make_tracer(analyzer, image, name_by_nid, nid_at)
    for off in (0x50, 0x80, 0x98, 0xb0, 0xd0, 0xf0):
        t.write(obj + off, 1, 0, 0)
    t.write(obj + 8, 8, owner, 0)
    t.run(0xf01610, {'rdi': obj})
    require(not t.calls and not t.events, 'negative control (b): zeroed object produced a call/effect')
    # (c) reading a stack byte nothing wrote must fault (no silent dependence on stack garbage).
    t = tt.make_tracer(analyzer, image, name_by_nid, nid_at)
    try:
        t.run(0xf01610, {'rdi': obj})
    except tt.Fault as exc:
        require('uninitialised' in str(exc), f'negative control (c): wrong fault {exc}')
    else:
        raise SystemExit('FAIL negative control (c): uninitialised read did not fault')


def check_header(index_to_trace):
    text = HEADER.read_text()
    match = re.search(r'kRegistrationThunkDescriptors\s*=\s*\{\{(.*?)\}\};', text, re.DOTALL)
    require(match, 'cannot find kRegistrationThunkDescriptors in registration_thunks.hpp')
    rows = re.findall(r'\{\s*(\d+)\s*,\s*(0x[0-9a-fA-F]+)\s*,\s*(0x[0-9a-fA-F]+)\s*,\s*(0x[0-9a-fA-F]+)\s*,'
                      r'\s*RegistrationProbeShape::(\w+)\s*\}', match.group(1))
    require(len(rows) == len(EXPECTED), f'native table has {len(rows)} rows, expected {len(EXPECTED)}')
    for (idx, rva, vptr, glob_a, shape), (want_idx, want) in zip(rows, sorted(EXPECTED.items())):
        require(int(idx) == want_idx, f'native row order: {idx} != {want_idx}')
        require((int(rva, 16), int(glob_a, 16), int(vptr, 16), shape) == (want[0], want[3], want[4], want[5]),
                f'native row {idx} differs from the traced values')


def main():
    analyzer = load_analyzer()
    _, image, name_by_nid, nid_at = analyzer.load_image()

    # The runner table in the ELF must hold the thunk RVAs at their indices.
    for index, want in EXPECTED.items():
        table_rva = q(image, TABLE_START - 8 * index)
        require(table_rva == want[0], f'ELF constructor table[{index}] = {table_rva:#x}, expected {want[0]:#x}')

    # Shared cleanup 0xe44a10 is the pinned 31-byte pthread_mutex_destroy + free routine.
    require(hashlib.sha256(image.read(CLEANUP, 31)).hexdigest() == CLEANUP_SHA256, 'cleanup body changed')

    traces = {}
    for index, want in sorted(EXPECTED.items()):
        traces[index] = check_slot(analyzer, image, name_by_nid, nid_at, index, want)

    negative_controls(analyzer, image, name_by_nid, nid_at)
    check_header(traces)

    for index, (t, dtor) in sorted(traces.items()):
        want = EXPECTED[index]
        names = [f'{c[0]}:{c[2]:#x}' if isinstance(c[2], int) else f'{c[0]}:{c[2]}' for c in t.calls]
        internal = [n for n in names if not any(x in n for x in ('0xf90', '0xb30'))]
        print(f'slot {index:>2} {want[0]:#x} {want[5]:<13} A={want[3]:#x} V={want[4]:#x} '
              f'vslot0={dtor:#x} steps={t.steps} calls={len(names)}')
    print('OK: slots 12-19 are pure registration thunks (publish V, malloc(0x10) mutex, recursive+PI '
          'init, atexit(0xe44a10, A+0x10, 0x1604218)); every class-specific stack probe is effect-free '
          'under owner=null/field20=field28=0x10; native table matches; 3 tracer negative controls pass')


if __name__ == '__main__':
    main()
