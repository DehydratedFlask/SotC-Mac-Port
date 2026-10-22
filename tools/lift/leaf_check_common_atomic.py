#!/usr/bin/env python3
"""Binary-vs-native tie for leaf constructor lifts that contain atomic `xchg` stores (CPU-01, batch 36).

Counterpart of leaf_check_common_sym.py (private; needs local/re/eboot.elf). The body is decoded
with tools/lift/leaf_ctor_decode_atomic.py (the symbolic simulator plus `xor r32,r32` and the
memory form of `xchg r/m32,r32`). run(spec) reuses leaf_check_common_sym.run with that decoder, so
the pinned body hash, instruction/step counts, ordered store list, sources, region and the native
--dump (zero-initialised and poisoned) are all compared exactly. It additionally pins which decoded
stores were `xchg` (atomic, full barrier) and requires the native test's dump to declare exactly the
same cell offsets as atomic.
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_check_common as cc  # noqa: E402
import leaf_check_common_sym as cs  # noqa: E402
import leaf_ctor_decode_atomic as A  # noqa: E402


def run(spec):
    cs.Y = A                      # decode with the atomic-capable simulator
    cs.run(spec)
    elf, image = A.load_image()
    d = A.decode(image, spec['rva'])
    atomic = [s['dest'] for s in d['stores'] if s.get('atomic')]
    cc.require(atomic == spec['atomic_dests'], f'atomic stores changed: {[hex(a) for a in atomic]}')
    base, _ = A.region(d['stores'], 0)
    exe = cc.native_executable(f'test_{spec["name"]}')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    cc.require(native['atomic_offsets'] == [a - base for a in atomic],
               f'native atomic offsets {native["atomic_offsets"]} differ from decoded {[a - base for a in atomic]}')
    print(f'OK {spec["name"]}: {len(atomic)} atomic xchg stores at cell offsets {native["atomic_offsets"]} match the decode')
