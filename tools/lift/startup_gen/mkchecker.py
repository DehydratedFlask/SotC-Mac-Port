import sys
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_HERE)
import genpins
from genpins import gen, fmt_pin
ROOT=_ROOT
def pyrepr_bytes(b): return repr(b)
def write_checker(n, doc, callees, native, summary, bool_tags=(), float_tags=(), int_tags=(), extras=(), m6_callees=None, m6_bytes=(), lib='lane_m6_lib', drop_strings=(), extra_strings=(), extra_pins=(), extra_defs='', pins_filter=None, extra_lengths=(), at=None, fname=None, label=None, no_table=False, spec_extra=None):
    d=gen(n,at=at)
    extra_pins=list(extra_pins)
    if d['pad']=='':   # no alignment padding: pin the last body byte and the first bytes of the NEXT function so a flipped boundary byte cannot survive
        from genpins import e as _e
        extra_pins+=[("imm",d['base']+d['size']-1,_e.raw(d['base']+d['size']-1,1).hex()),("imm",d['base']+d['size'],_e.raw(d['base']+d['size'],8).hex())]
    strings=[(a,s) for a,s in sorted(d['strings'].items()) if a>=0xf90000 and a not in drop_strings]
    strings+=list(extra_strings)
    lines=[]
    lines.append('#!/usr/bin/env python3')
    lines.append('"""'+doc.strip()+'\n"""\n')
    lines.append('from __future__ import annotations\n\nimport sys\nfrom pathlib import Path\n')
    lines.append('sys.path.insert(0, str(Path(__file__).resolve().parent))')
    lines.append('import bool_page_lib as bp  # noqa: E402\n')
    if extra_defs: lines.append(extra_defs)
    lines.append('SPEC = {')
    lines.append(f'    "slot": {n},')
    if label: lines.append(f'    "label": {label!r},')
    if no_table: lines.append('    "no_table": True,')
    lines.append(f'    "base": {d["base"]:#x},')
    lines.append(f'    "size": {d["size"]},')
    lines.append(f'    "body_sha256": "{d["sha"]}",')
    lines.append(f'    "pad": "{d["pad"]}",')
    lines.append(f'    "callees": {callees!r},')
    lines.append(f'    "bool_tags": {[hex(t) for t in bool_tags]!r}'.replace("'",'')+',')
    lines.append(f'    "float_tags": {[hex(t) for t in float_tags]!r}'.replace("'",'')+',')
    if int_tags: lines.append(f'    "m6_int_tags": {[hex(t) for t in int_tags]!r}'.replace("'",'')+',')
    lines.append('    "strings": [')
    for a,s in strings: lines.append(f'        ({a:#x}, {s!r}),')
    lines.append('    ],')
    lines.append('    "lengths": [')
    for t,a,hx in list(d['lens'])+list(extra_lengths): lines.append(f'        ({t!r}, {a:#x}, "{hx}"),')
    lines.append('    ],')
    lines.append('    "pins": [')
    for p in d['pins']:
        if pins_filter and not pins_filter(p): continue
        lines.append(fmt_pin(p))
    for p in extra_pins: lines.append(fmt_pin(p))
    lines.append('    ],')
    if m6_callees:
        lines.append('    "m6_callees": {')
        for k,(r,s,h) in m6_callees.items(): lines.append(f'        {k!r}: ({r:#x}, {s}, "{h}"),')
        lines.append('    },')
    if m6_bytes:
        lines.append('    "m6_bytes": [')
        for r,h,w in m6_bytes: lines.append(f'        ({r:#x}, "{h}", {w!r}),')
        lines.append('    ],')
    for k_,v_ in (spec_extra or {}).items(): lines.append(f'    {k_!r}: {v_!r},')
    lines.append(f'    "m6_extras": {list(extras)!r},')
    lines.append('    "native": '+repr(native).replace("}, '","},\n        '")+',')
    lines.append('}\n\n')
    lines.append('if __name__ == "__main__":')
    lines.append(f'    raise SystemExit(bp.main(SPEC, {summary!r}))')
    open(ROOT+f'tools/lift/{fname or "check_ctor_slot_%d.py"%n}','w').write('\n'.join(lines)+'\n')
    import os; os.chmod(ROOT+f'tools/lift/{fname or "check_ctor_slot_%d.py"%n}',0o755)
    return d
