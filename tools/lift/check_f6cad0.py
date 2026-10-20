#!/usr/bin/env python3
"""Tie native/core/math_constants to the exact bytes of original RVA 0xf6cad0 (private; needs the ELF).

Independence note: this checks the native bit patterns against what the binary stores. The test in
native/tests separately checks them against IEEE-754 identities, so neither oracle is the other's copy.
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'lift'))
import guarded_const_ctor as g

recs, stop, nxt, tail = g.lift(ROOT / 'local/re/eboot.elf', 0xf6cad0)
assert len(recs) == 6 and [t['kind'] for t in tail] == ['guarded_vec_u32_lane0', 'guarded_vec_copy', 'vec_copy_then_ret']
assert nxt.startswith('9090'), f'function should end at ret then padding, got {nxt}'
exe = ROOT / 'local/build/native/test_math_constants'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
binary = dict(floats=[r['bits'] for r in recs], lane0_mask=tail[0]['bits'], xyz_mask=tail[1]['bits'])
assert tail[2]['bits'] == tail[1]['bits'], 'unguarded copy must equal the guarded xyz mask'
assert native == binary, f'MISMATCH\nnative={native}\nbinary={binary}'
print(f'OK: 0xf6cad0..{stop:#x} ({stop - 0xf6cad0:#x} bytes) fully matched; native == binary for 6 floats + 2 masks')
