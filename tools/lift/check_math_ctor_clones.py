#!/usr/bin/env python3
"""Prove early-list math-constant clones match 0xf6cad0's idiom and bit patterns (private ELF).

Clones (same 6 floats + lane0 + xyz guarded + unguarded xyz copy):
  0xf66640 (ctor index 1), 0xf53de0 (index 2), 0xf3d9d0 (index 8).

Verified ELF fact (re-derived here, pinned below): the clones do NOT own separate guarded cells.
All four bodies (the canonical 0xf6cad0 and the three clones) use the same 8 guard/value cell pairs
0x3351ef8..0x3351f80, so whichever runs first initialises them and the others skip. What differs
per body is (a) the rodata source of the guarded xyz-mask copy (same bits, different address) and
(b) the destination of the final unguarded xyz-mask copy, which is a distinct BSS cell per body.
Native reuse: sotc::init_math_constants; one MathConstants carries the shared guarded cells and
the unguarded copy, so each clone's own unguarded cell is a separate xyz_mask_copy instance.
"""
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'lift'))
import guarded_const_ctor as g

# body -> (guarded xyz-mask rodata source, unguarded xyz-mask destination cell)
PER_BODY = {
    0xf6cad0: (0x132e660, 0x3356a90),
    0xf66640: (0x132d240, 0x3356a40),
    0xf53de0: (0x132cf30, 0x33569f0),
    0xf3d9d0: (0x132c190, 0x3356820),
}
CLONES = (0xf66640, 0xf53de0, 0xf3d9d0)
# Shared (guard, value) cell pairs: six floats, then the lane0 mask, then the xyz mask.
SHARED_FLOAT_CELLS = [(0x3351ef8, 0x3351ef4), (0x3351f08, 0x3351f00), (0x3351f18, 0x3351f10),
                      (0x3351f28, 0x3351f20), (0x3351f38, 0x3351f30), (0x3351f48, 0x3351f40)]
SHARED_LANE0_CELL = (0x3351f60, 0x3351f50)
SHARED_XYZ_CELL = (0x3351f80, 0x3351f70)

elf = ROOT / 'local/re/eboot.elf'
base_recs, base_stop, _, base_tail = g.lift(elf, 0xf6cad0)
base_bits = [r['bits'] for r in base_recs]
base_kinds = [t['kind'] for t in base_tail]
assert base_kinds == ['guarded_vec_u32_lane0', 'guarded_vec_copy', 'vec_copy_then_ret']

build = os.environ.get('SOTC_NATIVE_BUILD', 'local/build/native')
exe = ROOT / build / 'test_math_constants'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['floats'] == base_bits


def check_shape(A, recs, tail):
    """The cell layout this checker pins for body A (canonical or clone)."""
    assert [(r['guard'], r['value']) for r in recs] == SHARED_FLOAT_CELLS, hex(A)
    assert (tail[0]['guard'], tail[0]['value']) == SHARED_LANE0_CELL, hex(A)
    assert (tail[1]['guard'], tail[1]['value']) == SHARED_XYZ_CELL, hex(A)
    src, dst = PER_BODY[A]
    assert tail[1]['source'] == tail[2]['source'] == src, hex(A)
    assert tail[2]['value'] == dst, hex(A)


check_shape(0xf6cad0, base_recs, base_tail)
seen_sources, seen_dests = {PER_BODY[0xf6cad0][0]}, {PER_BODY[0xf6cad0][1]}

for A in CLONES:
    recs, stop, nxt, tail = g.lift(elf, A)
    assert len(recs) == 6, (hex(A), len(recs))
    assert [r['bits'] for r in recs] == base_bits, hex(A)
    assert [t['kind'] for t in tail] == base_kinds, hex(A)
    assert tail[0]['bits'] == base_tail[0]['bits']
    assert tail[1]['bits'] == base_tail[1]['bits'] == tail[2]['bits']
    assert nxt.startswith('90') or nxt.startswith('55') or nxt.startswith('c3'), (hex(A), nxt)
    check_shape(A, recs, tail)
    # Shared guarded cells (identical to the canonical body) ...
    assert [(r['guard'], r['value']) for r in recs] == [(r['guard'], r['value']) for r in base_recs], hex(A)
    # ... but a distinct rodata source for the guarded xyz copy and a distinct unguarded destination.
    assert tail[1]['source'] not in seen_sources, hex(A)
    assert tail[2]['value'] not in seen_dests, hex(A)
    seen_sources.add(tail[1]['source'])
    seen_dests.add(tail[2]['value'])
    print(f'OK: {A:#x}..{stop:#x} math-clone; bits == 0xf6cad0 / native dump; shares the 8 guarded '
          f'cells with 0xf6cad0; own xyz-copy source {tail[1]["source"]:#x} and destination {tail[2]["value"]:#x}')

assert len(seen_sources) == len(seen_dests) == 4
print('OK: all math ctor clones verified')
