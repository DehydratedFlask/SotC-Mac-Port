#!/usr/bin/env python3
"""Classify every entry of the 573-slot original constructor table by reachable-body idiom.

Uses tools/lift/analyze_slot_struct.py (reachability walk, offline) so bodies are bounded by the
real control flow instead of a fixed window — a fixed window runs into neighbouring functions and
produces false positives (observed: a `0xe8`-to-mutex-init byte match inside `0xf53de0`'s window
that does not belong to it).

Classes:
  leaf_vec_copy            exact 17-byte unguarded 16-byte constant copy then ret
  guarded_const            exact guarded static-constant initializer idiom (32-byte repetitions)
  descriptor_mutex_atexit  calls the pthread mutex initializer 0xb30950 *and* malloc/__cxa_atexit
  mutex_only               calls 0xb30950 without allocation/atexit registration
  other                    every remaining body (span, call and indirect-edge counts recorded)

Writes a private manifest to local/re/work/ctor-classes.json and prints a summary. Nothing is
executed and no Ghidra/REA session is used.
"""
import json
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import ps4_dynlink as d  # noqa: E402

TABLE_START = 0x2DD70E0
TABLE_COUNT = 573
MUTEX_INIT = 0xB30950
LEAF = bytes.fromhex('c5 f8 10 05 00000000 c5 f8 11 05 00000000 c3')
GUARDED_PREFIX = bytes.fromhex('48 8d 05')


def main():
    import analyze_slot_struct as a

    elf, image, name_by_nid, nid_at = a.load_image()
    rvas = [int.from_bytes(image.read(TABLE_START - 8 * i, 8), 'little')
            for i in range(TABLE_COUNT)]
    rows = []
    for index, rva in enumerate(rvas):
        row = {'index': index, 'rva': hex(rva)}
        try:
            report = a.build_report(rva, image, name_by_nid, nid_at,
                                    max_instructions=5000)
        except SystemExit:
            row['class'] = 'undecoded'
            rows.append(row)
            continue
        body = bytes.fromhex('')  # placeholder for clarity
        body_bytes = image.read(rva, min(report['span_bytes'], 4096))
        masked = bytearray(body_bytes[:17])
        if len(masked) == 17:
            masked[4:8] = b'\0' * 4
            masked[12:16] = b'\0' * 4
        calls = report['direct_calls']
        targets = {int(c['target'], 16) for c in calls}
        names = {(c['thunk'] or {}).get('name') for c in calls}
        row.update({
            'span_bytes': report['span_bytes'],
            'instructions': report['reachable_instructions'],
            'returns': report['blocks_terminated_by_ret'],
            'direct_calls': len(calls),
            'indirect': len(report['indirect_transfers']),
            'body_sha256': report['body_sha256'],
        })
        if len(masked) == 17 and bytes(masked) == LEAF:
            row['class'] = 'leaf_vec_copy'
        elif body_bytes.startswith(GUARDED_PREFIX) and b'\x80\x38\x00\x75' in body_bytes[:40]:
            row['class'] = 'guarded_const'
        elif MUTEX_INIT in targets and ({'malloc'} & names or '__cxa_atexit' in names):
            row['class'] = 'descriptor_mutex_atexit'
        elif MUTEX_INIT in targets:
            row['class'] = 'mutex_only'
        else:
            row['class'] = 'other'
        rows.append(row)
        if (index + 1) % 50 == 0:
            print(f'  ... {index + 1}/{TABLE_COUNT}', file=sys.stderr)

    counts = Counter(r['class'] for r in rows)
    out = ROOT / 'local/re/work/ctor-classes.json'
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({
        'note': 'constructor idioms classified from reachable bodies; ELF sha256 '
                'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3',
        'table_start': hex(TABLE_START), 'count': TABLE_COUNT,
        'summary': dict(sorted(counts.items())), 'rows': rows}, indent=1) + '\n')
    print(json.dumps({'summary': dict(sorted(counts.items())), 'written': str(out)}, indent=1))
    family = [r['index'] for r in rows if r['class'] == 'descriptor_mutex_atexit']
    print(f"descriptor_mutex_atexit indices: {family}")


if __name__ == '__main__':
    main()
