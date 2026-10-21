#!/usr/bin/env python3
"""AUD-01 audio inventory: member identity plus prefix/entropy observations.

Read-only. Uses scripts/psarc.py to decode each matching member in memory. SHA-256 covers
the complete decoded member; the observed family and entropy use its first N bytes. PSARC
entry IDs are table indices (entry 0 is the path manifest). Does NOT identify codecs, invent
NGS2 semantics, or extract members to disk.
"""
from __future__ import annotations

import argparse
import collections
import hashlib
import json
import math
import posixpath
import sys
import zlib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from psarc import Psarc

AUDIO_EXTS = ('.sxd', '.sxd1', '.sxd2', '.hd', '.bd')
DEFAULT_ARCHIVES = (
    'local/extracted/CUSA08809/cachedFiles.psarc',
    'local/extracted/CUSA08809-patch/cachedfiles.psarc',
    'local/extracted/CUSA08809/pak2.psarc',
    'local/extracted/CUSA08809/pak3.psarc',
    'local/extracted/CUSA08809/pakN.psarc',
)
SAMPLE = 4096


def shannon(data: bytes) -> float:
    if not data:
        return 0.0
    counts = collections.Counter(data)
    n = len(data)
    return -sum((c / n) * math.log2(c / n) for c in counts.values())


def ascii_preview(data: bytes, n: int = 16) -> str:
    return ''.join(chr(b) if 32 <= b < 127 else '.' for b in data[:n])


def family_of(head: bytes | bytearray | memoryview) -> str:
    """Label by observed prefix only. Labels are OBS, not codec IDs."""
    # Prefix matching is anchored at offset 0 and never scans, so the input must
    # already be bytes-like. Reject anything else before the falsy test: without
    # this guard ``None`` and ``''`` fell into the '' -> 'empty' label and other
    # non-bytes leaked AttributeError from ``.startswith``.
    if isinstance(head, memoryview):
        head = head.tobytes()
    elif not isinstance(head, (bytes, bytearray)):
        raise TypeError(
            f'family_of expects bytes-like input, got {type(head).__name__}')
    if not head:
        return 'empty'
    if head.startswith(b'\xfb\xad\x98\x97'):
        return 'sxd-fbad9897'
    if head.startswith(b'xff\x00'):
        return 'hd-bd-ascii-xff'
    if head.startswith(b'qath'):
        return 'bd-ascii-qath'
    if len(head) >= 3 and head[0] == 0x3e and head[1] == 0xbe and head[2] == 0x7b:
        return 'hd-bd-near-empty-3ebe7b'
    if head[:2] in (b'\x8c\x0a', b'\x8c\x06'):
        return 'sxd-high-entropy-8c0x'
    if head.startswith(b'80AE'):
        return 'sxd-ascii-80AE'
    # little-endian looking small integers at offset 0 (common in low-entropy sxd*)
    if len(head) >= 8:
        w0 = int.from_bytes(head[:4], 'little')
        w1 = int.from_bytes(head[4:8], 'little')
        if w0 <= 0x20 and w1 <= 0x20:
            return 'sxd-le-small-ints'
    return f'other-{head[:4].hex()}'


def decompress_probe(data: bytes) -> dict:
    """Record whether zlib/lzma wrappers decode. Tiny outputs are treated as non-evidence."""
    out: dict = {}

    def record(name: str, blob: bytes | None, err: str | None = None) -> None:
        if err is not None:
            out[name] = {'ok': False, 'err': err}
            return
        assert blob is not None
        # Reject trivial "success" (e.g. 1-byte inflate of near-empty stubs).
        meaningful = len(blob) >= 16
        out[name] = {
            'ok': meaningful,
            'out_len': len(blob),
            'rejected_trivial': not meaningful,
        }

    for name, off, wbits in (
        ('zlib_offset0', 0, 15),
        ('zlib_offset2', 2, 15),
        ('raw_inflate_offset0', 0, -15),
        ('raw_inflate_offset2', 2, -15),
    ):
        if len(data) <= off:
            record(name, None, 'too_short')
            continue
        try:
            record(name, zlib.decompress(data[off:], wbits))
        except Exception as exc:  # noqa: BLE001 — classification probe
            record(name, None, type(exc).__name__)

    try:
        import lzma
    except ImportError:
        out['lzma'] = {'ok': False, 'err': 'ImportError'}
        return out

    for off in (0, 2, 4):
        name = f'lzma_offset{off}'
        if len(data) <= off:
            record(name, None, 'too_short')
            continue
        try:
            record(name, lzma.decompress(data[off:]))
        except Exception as exc:  # noqa: BLE001
            record(name, None, type(exc).__name__)
    return out


def classify_archives(archives: list[Path], sample: int = SAMPLE) -> dict:
    members: list[dict] = []
    for apath in archives:
        a = Psarc(apath)
        try:
            names = a.paths()
            for i, name in enumerate(names, 1):
                ext = posixpath.splitext(name)[1].lower()
                if ext not in AUDIO_EXTS:
                    continue
                entry = a.entries[i]
                member = a.read(i)
                head = member[:sample]
                family = family_of(head)
                row = {
                    'archive': str(apath),
                    'archive_stem': apath.name,
                    'entry_id': i,
                    'entry_offset': entry['offset'],
                    'first_block': entry['block'],
                    'path': name,
                    'ext': ext,
                    'size': entry['size'],
                    'member_sha256': hashlib.sha256(member).hexdigest(),
                    'magic8': head[:8].hex(),
                    'magic16': head[:16].hex(),
                    'ascii16': ascii_preview(head, 16),
                    'entropy_bits_per_byte': round(shannon(head), 4),
                    'sample_bytes': len(head),
                    'family': family,
                    'codec': 'UNKNOWN',
                    'zlib_xz_probe': decompress_probe(head) if head else {},
                }
                members.append(row)
        finally:
            a.close()

    buckets: dict[tuple, list[dict]] = collections.defaultdict(list)
    for row in members:
        key = (row['ext'], row['family'], row['magic8'][:8])  # group by first 4 bytes hex
        buckets[key].append(row)

    classes = []
    for (ext, family, magic4), rows in sorted(
            buckets.items(), key=lambda kv: (-len(kv[1]), kv[0])):
        ents = [r['entropy_bits_per_byte'] for r in rows]
        sizes = [r['size'] for r in rows]
        decomp = collections.Counter()
        for r in rows:
            for k, v in r['zlib_xz_probe'].items():
                if v.get('ok'):
                    decomp[k] += 1
        classes.append({
            'ext': ext,
            'family': family,
            'magic4_hex': magic4,
            'magic8_examples': sorted({r['magic8'] for r in rows})[:8],
            'count': len(rows),
            'archives': sorted({r['archive_stem'] for r in rows}),
            'size_min': min(sizes),
            'size_max': max(sizes),
            'size_sum': sum(sizes),
            'entropy_min': min(ents),
            'entropy_max': max(ents),
            'entropy_mean': round(sum(ents) / len(ents), 4),
            'ascii16_example': rows[0]['ascii16'],
            'example_paths': [r['path'] for r in rows[:5]],
            'decompress_ok_counts': dict(decomp),
            'codec': 'UNKNOWN',
        })

    family_totals = collections.Counter(r['family'] for r in members)
    return {
        'schema_version': 1,
        'task': 'AUD-01-headers',
        'limitations': [
            'Magic and Shannon entropy only; codec intentionally UNKNOWN.',
            'No NGS2 / voice / rack semantics inferred.',
            'Decompress probes reject outputs shorter than 16 bytes as non-evidence.',
            'Music/dialogue stream location beyond these members remains OPEN.',
        ],
        'member_count': len(members),
        'family_totals': dict(family_totals),
        'classes': classes,
        'members': members,
    }


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', type=Path, default=Path('local/manifests/assets/audio-headers.json'))
    ap.add_argument('archive', type=Path, nargs='*', help='PSARC paths (default: CUSA08809 set)')
    args = ap.parse_args()
    archives = args.archive or [Path(p) for p in DEFAULT_ARCHIVES]
    missing = [p for p in archives if not p.is_file()]
    if missing:
        raise SystemExit(f'missing archives: {missing}')
    payload = classify_archives(archives)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(payload, indent=2) + '\n')
    print(f"wrote {args.out} members={payload['member_count']} classes={len(payload['classes'])}")
    for c in payload['classes']:
        print(
            f"  {c['ext']:6} {c['family']:28} n={c['count']:3} "
            f"H={c['entropy_min']:.2f}-{c['entropy_max']:.2f} "
            f"magic4={c['magic4_hex']} arch={c['archives']}"
        )


if __name__ == '__main__':
    main()
