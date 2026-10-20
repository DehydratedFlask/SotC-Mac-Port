#!/usr/bin/env python3
"""Prove early-list math-constant clones match 0xf6cad0's idiom and bit patterns (private ELF).

Clones (same 6 floats + lane0 + xyz guarded + unguarded xyz copy):
  0xf66640 (ctor index 1), 0xf53de0 (index 2), 0xf3d9d0 (index 8).
Native reuse: sotc::init_math_constants on distinct StartupState MathConstants cells.
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'lift'))
import guarded_const_ctor as g

CLONES = (0xf66640, 0xf53de0, 0xf3d9d0)
base_recs, base_stop, _, base_tail = g.lift(ROOT / 'local/re/eboot.elf', 0xf6cad0)
base_bits = [r['bits'] for r in base_recs]
base_kinds = [t['kind'] for t in base_tail]
assert base_kinds == ['guarded_vec_u32_lane0', 'guarded_vec_copy', 'vec_copy_then_ret']

exe = ROOT / 'local/build/native/test_math_constants'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['floats'] == base_bits

for A in CLONES:
    recs, stop, nxt, tail = g.lift(ROOT / 'local/re/eboot.elf', A)
    assert len(recs) == 6, (hex(A), len(recs))
    assert [r['bits'] for r in recs] == base_bits, hex(A)
    assert [t['kind'] for t in tail] == base_kinds, hex(A)
    assert tail[0]['bits'] == base_tail[0]['bits']
    assert tail[1]['bits'] == base_tail[1]['bits'] == tail[2]['bits']
    assert nxt.startswith('90') or nxt.startswith('55') or nxt.startswith('c3'), (hex(A), nxt)
    # Distinct BSS from the canonical 0xf6cad0 cells (value of first float).
    assert recs[0]['value'] != base_recs[0]['value'], hex(A)
    print(f'OK: {A:#x}..{stop:#x} math-clone; bits == 0xf6cad0 / native dump; distinct cells')

print('OK: all math ctor clones verified')
