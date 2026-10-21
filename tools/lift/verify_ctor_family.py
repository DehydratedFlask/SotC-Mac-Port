#!/usr/bin/env python3
"""Replay the whole slot 10-19 / 24-47 constructor family (CPU-01-SLOT10-47-FAMILY-01).

For every slot that `tools/lift/classify_ctors.py` labelled `descriptor_mutex_atexit`, re-derive the
reachable body and assert the family contract:

  * span and body digest equal the private manifest entry
  * exactly two `.bss` globals, exactly 0x10 bytes apart
  * exactly one `malloc`, exactly one mutex-init call (`0xb30950`)
  * exactly one `__cxa_atexit` registration, as a call or as a tail call
  * a reference to the shared cleanup `0xe44a10`
  * no rip-relative reference outside the loadable image

Exit 0 only when all slots pass. Offline; nothing executed.
"""
import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import ps4_dynlink as d  # noqa: E402

BSS_START = 0x2DD7100
IMAGE_END = 0x3956AA4
MUTEX_INIT = '0xb30950'
CLEANUP = '0xe44a10'
MANIFEST = ROOT / 'local/re/work/ctor-classes.json'


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def main():
    import analyze_slot_struct as a

    require(MANIFEST.is_file(), 'run tools/lift/classify_ctors.py first')
    manifest = json.loads(MANIFEST.read_text())
    rows = [r for r in manifest['rows'] if r['class'] == 'descriptor_mutex_atexit']
    require(len(rows) == 34, f'manifest has {len(rows)} family slots, expected 34')
    require([r['index'] for r in rows] == list(range(10, 20)) + list(range(24, 48)),
            'family indices are not the contiguous runs 10-19 and 24-47')

    _, image, name_by_nid, nid_at = a.load_image()
    table = {}
    for row in rows:
        rva = int(row['rva'], 16)
        report = a.build_report(rva, image, name_by_nid, nid_at)
        tag = f"index {row['index']} / {row['rva']}"
        require(report['span_bytes'] == row['span_bytes'], f'{tag} span changed')
        require(report['body_sha256'] == row['body_sha256'], f'{tag} body digest changed')

        targets = [c['target'] for c in report['direct_calls']]
        names = [(c['thunk'] or {}).get('name') for c in report['direct_calls']]
        require(names.count('malloc') == 1, f'{tag} malloc count {names.count("malloc")}')
        require(MUTEX_INIT in targets, f'{tag} missing mutex initializer')
        atexit = names.count('__cxa_atexit') + sum(
            1 for b in report['branch_edges']
            if (b.get('thunk') or {}).get('name') == '__cxa_atexit')
        require(atexit == 1, f'{tag} atexit registrations {atexit}')

        refs = [r for r in report['rip_relative_refs']]
        require(not [r for r in refs if not r['file_backed']],
                f'{tag} has a reference outside the image')
        bss = sorted({int(r['target'], 16) for r in refs
                      if BSS_START <= int(r['target'], 16) < IMAGE_END})
        require(len(bss) == 2, f'{tag} .bss globals {[hex(x) for x in bss]}')
        require(bss[1] - bss[0] == 0x10, f'{tag} globals are {bss[1] - bss[0]:#x} apart')
        require(any(r['target'] == CLEANUP for r in refs), f'{tag} missing cleanup {CLEANUP}')
        table[row['index']] = (row['rva'], report['span_bytes'], hex(bss[0]), hex(bss[1]),
                               'tail' if atexit and not names.count('__cxa_atexit') else 'call')

    for index in sorted(table):
        rva, span, lo, hi, kind = table[index]
        print(f'{index:>3} {rva} {span:>4} B  globals {lo} / {hi}  atexit {kind}')
    print(f'OK: {len(table)} family slots share malloc(0x10) + mutex init {MUTEX_INIT} + '
          f'__cxa_atexit({CLEANUP}) with one adjacent .bss global pair each')


if __name__ == '__main__':
    main()
