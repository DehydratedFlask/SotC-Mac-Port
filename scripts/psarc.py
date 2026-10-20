#!/usr/bin/env python3
"""Bounded, read-only PSARC (v1.x) TOC and member reader. Stdlib only.

Layout (big endian): 'PSAR', u16 major, u16 minor, 4s compression, u32 toc_size (includes the
32-byte header), u32 toc_entry_size, u32 entry_count, u32 block_size, u32 archive_flags.
Entry: 16-byte MD5, u32 first block index, 5-byte uncompressed size, 5-byte file offset.
Block table follows the entries: one big-endian value per block, width = bytes needed for
(block_size - 1); value 0 means a full, stored block of block_size bytes. Entry 0 is the manifest
(member paths, NUL-separated in this game's archives; newline accepted). Every length and offset is range-checked; path traversal and
unknown compression are rejected. Nothing is extracted to disk by this module.

Overlay / case policy (DATA-02, provisional until Fios/loader recovery proves otherwise):
- Member path identity is exact UTF-8 (case-sensitive). Resolve uses exact keys only.
- Unicode casefold is used solely to detect ambiguous collisions (never to merge).
- Archive basenames are exact: base `cachedFiles.psarc` and patch `cachedfiles.psarc` are
  distinct; never case-blind-copy them into one directory. Keep separate extract trees.
- Mount order for the title cachedFiles pair: base layer then patch layer (patch wins on
  exact-path collision). Observed: patch contains every base path plus three new members.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import posixpath
import zlib

HEADER = 32

# Documented policy constants (tests assert these strings stay stable).
MEMBER_PATH_MATCH = 'exact'
COLLISION_KEY = 'unicode_casefold'
ARCHIVE_BASENAME_MATCH = 'exact'
CACHEDFILES_MOUNT_ORDER = ('base', 'patch')  # low -> high priority


class PsarcError(ValueError):
    pass


def be(data, pos, n):
    if pos < 0 or pos + n > len(data):
        raise PsarcError(f'out of bounds: {pos:#x}+{n}')
    return int.from_bytes(data[pos:pos + n], 'big')


def casefold_key(path):
    """Collision-detection key only; never used as the resolve key."""
    if not isinstance(path, str):
        raise PsarcError('member path must be str')
    return path.casefold()


def archive_basename_key(name):
    """Exact basename identity for on-disk archive files (no case folding)."""
    return Path(name).name


def load_members_jsonl(path):
    """Load a members.jsonl produced by summarize/main into {exact_path: record}."""
    out = {}
    with Path(path).open() as f:
        for line in f:
            rec = json.loads(line)
            p = rec['path']
            if p in out:
                raise PsarcError(f'duplicate exact path in {path}: {p!r}')
            out[p] = rec
    return out


def build_overlay(layers):
    """Merge member maps in mount order (last layer wins on exact path).

    layers: sequence of (layer_id, {exact_path: member_record}).
    Rejects casefold collisions where two distinct exact paths would share a fold key
    within one layer or across the merged result (ambiguous under a case-blind FS).
    Returns dict with resolved map, stats, and policy echo.
    """
    if not layers:
        raise PsarcError('overlay requires at least one layer')
    fold_owners = {}  # casefold -> exact path that claimed it
    resolved = {}
    source = {}
    overrides = 0
    added = 0
    for layer_id, members in layers:
        local_fold = {}
        for path, rec in members.items():
            fk = casefold_key(path)
            if fk in local_fold and local_fold[fk] != path:
                raise PsarcError(
                    f'casefold collision inside layer {layer_id!r}: '
                    f'{local_fold[fk]!r} vs {path!r}')
            local_fold[fk] = path
        for path, rec in members.items():
            fk = casefold_key(path)
            prior = fold_owners.get(fk)
            if prior is not None and prior != path:
                raise PsarcError(
                    f'casefold collision across layers: {prior!r} vs {path!r} '
                    f'(layer {layer_id!r})')
            fold_owners[fk] = path
            if path in resolved:
                overrides += 1
            else:
                added += 1
            resolved[path] = dict(rec)
            source[path] = layer_id
    return dict(
        policy=dict(
            member_path_match=MEMBER_PATH_MATCH,
            collision_key=COLLISION_KEY,
            archive_basename_match=ARCHIVE_BASENAME_MATCH,
            mount_order=[layer_id for layer_id, _ in layers],
        ),
        members=len(resolved),
        added=added,
        overrides=overrides,
        sources=collections.Counter(source.values()),
        source_by_path=source,
        resolved=resolved,
    )


def resolve_path(overlay, path):
    """Exact-path lookup into a build_overlay result. Missing -> None."""
    if path not in overlay['resolved']:
        return None
    rec = dict(overlay['resolved'][path])
    rec['source_layer'] = overlay['source_by_path'][path]
    return rec


def overlay_from_member_jsonls(layer_paths):
    """layer_paths: sequence of (layer_id, members.jsonl Path)."""
    layers = [(lid, load_members_jsonl(p)) for lid, p in layer_paths]
    return build_overlay(layers)


class Psarc:
    def __init__(self, path):
        self.path = Path(path)
        self.f = open(self.path, 'rb')
        try:
            self._load()
        except BaseException:
            self.f.close()
            raise

    def close(self):
        self.f.close()

    def _load(self):
        self.size = self.path.stat().st_size
        head = self.f.read(HEADER)
        if len(head) != HEADER or head[:4] != b'PSAR':
            raise PsarcError('not a PSARC')
        self.version = (be(head, 4, 2), be(head, 6, 2))
        self.compression = head[8:12].decode('ascii', 'replace')
        self.toc_size, self.entry_size = be(head, 12, 4), be(head, 16, 4)
        self.count, self.block_size, self.flags = be(head, 20, 4), be(head, 24, 4), be(head, 28, 4)
        if self.compression not in ('zlib', 'lzma'):
            raise PsarcError(f'unknown compression {self.compression!r}')
        if self.compression == 'lzma':
            raise PsarcError('lzma blocks not implemented (explicitly unsupported)')
        if self.entry_size != 30 or not 0 < self.block_size <= 1 << 24:
            raise PsarcError('unsupported entry/block size')
        if not HEADER + self.entry_size * self.count <= self.toc_size <= self.size:
            raise PsarcError('TOC size inconsistent with entry count/file size')
        toc = self.f.read(self.toc_size - HEADER)
        self.entries = []
        for i in range(self.count):
            p = i * self.entry_size
            e = dict(md5=toc[p:p + 16].hex(), block=be(toc, p + 16, 4),
                     size=be(toc, p + 20, 5), offset=be(toc, p + 25, 5))
            if e['offset'] > self.size:
                raise PsarcError(f'entry {i} offset beyond archive')
            self.entries.append(e)
        self.width = max(1, ((self.block_size - 1).bit_length() + 7) // 8)
        raw = toc[self.entry_size * self.count:]
        if len(raw) % self.width:
            raise PsarcError('block table not a multiple of width')
        self.blocks = [int.from_bytes(raw[i:i + self.width], 'big') for i in range(0, len(raw), self.width)]
        for e in self.entries:
            need = -(-e['size'] // self.block_size)
            if e['size'] and e['block'] + need > len(self.blocks):
                raise PsarcError('entry references blocks outside the block table')

    def read(self, index, limit=None):
        """Decompress a member (or its first `limit` bytes)."""
        e = self.entries[index]
        want = e['size'] if limit is None else min(limit, e['size'])
        out, pos, blk = bytearray(), e['offset'], e['block']
        while len(out) < want:
            csize = self.blocks[blk]
            remaining = e['size'] - len(out)
            full = min(self.block_size, remaining)
            self.f.seek(pos)
            if csize == 0:                       # stored full block
                data = self.f.read(self.block_size)
                chunk = data[:full]
                pos += self.block_size
            else:
                raw = self.f.read(csize)
                if len(raw) != csize:
                    raise PsarcError('truncated block')
                # PSARC rule: a block whose stored size equals its decoded size is raw
                chunk = raw if csize == full else zlib.decompress(raw)
                pos += csize
            if len(chunk) != full:
                raise PsarcError(f'block {blk} decoded to {len(chunk)} bytes, expected {full}')
            out += chunk
            blk += 1
        return bytes(out[:want])

    def paths(self):
        raw = self.read(0)
        sep = b'\0' if b'\0' in raw else b'\n'   # observed: this game's manifests are NUL-separated
        names = [n.decode('utf-8') for n in raw.split(sep) if n]
        if len(names) != self.count - 1:
            raise PsarcError(f'manifest has {len(names)} paths for {self.count - 1} members')
        for n in names:
            norm = posixpath.normpath(n)
            if '..' in norm.split('/') or n.startswith('/'):
                raise PsarcError(f'unsafe member path {n!r}')
        return names


def summarize(path, sample_per_ext=2):
    a = Psarc(path)
    names = a.paths()
    ext = collections.Counter(posixpath.splitext(n)[1].lower() for n in names)
    dup = [n for n, c in collections.Counter(n.lower() for n in names).items() if c > 1]
    magics = collections.defaultdict(collections.Counter)
    seen = collections.Counter()
    for i, n in enumerate(names, 1):
        x = posixpath.splitext(n)[1].lower()
        if seen[x] < sample_per_ext and a.entries[i]['size']:
            seen[x] += 1
            magics[x][a.read(i, 8).hex()] += 1
    return a, names, dict(
        file=str(path), bytes=a.size, version=list(a.version), compression=a.compression,
        flags=a.flags, block_size=a.block_size, entries=a.count, members=len(names),
        case_insensitive_duplicates=len(dup), extensions=dict(ext.most_common()),
        sample_magic={k: dict(v) for k, v in magics.items()},
        uncompressed_total=sum(e['size'] for e in a.entries[1:]))


def write_overlay_report(overlay, out_path, extra=None):
    """Persist overlay without embedding full member payloads (source map + stats)."""
    report = dict(
        schema_version=1,
        policy=overlay['policy'],
        members=overlay['members'],
        added=overlay['added'],
        overrides=overlay['overrides'],
        sources=dict(overlay['sources']),
        source_by_path=overlay['source_by_path'],
    )
    if extra:
        report.update(extra)
    Path(out_path).write_text(json.dumps(report, indent=1, sort_keys=True) + '\n')
    return report


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('archive', type=Path, nargs='*')
    ap.add_argument('--out', type=Path, help='directory for summaries and member lists')
    ap.add_argument('--overlay', nargs=2, metavar=('LAYER_ID', 'MEMBERS_JSONL'),
                    action='append', help='overlay layer (repeatable; order = mount order)')
    ap.add_argument('--overlay-out', type=Path, help='write overlay report JSON here')
    args = ap.parse_args()
    if args.overlay:
        if not args.overlay_out:
            raise SystemExit('--overlay requires --overlay-out')
        layers = [(lid, Path(p)) for lid, p in args.overlay]
        ov = overlay_from_member_jsonls(layers)
        # Drop resolved payloads from disk report; keep source map.
        write_overlay_report(ov, args.overlay_out)
        print(f"overlay: members={ov['members']} overrides={ov['overrides']} "
              f"sources={dict(ov['sources'])} -> {args.overlay_out}")
        return
    if not args.archive or not args.out:
        raise SystemExit('archive paths and --out are required unless using --overlay')
    args.out.mkdir(parents=True, exist_ok=True)
    for path in args.archive:
        a, names, summary = summarize(path)
        digest = hashlib.sha256(path.read_bytes()).hexdigest() if path.stat().st_size < 1 << 30 else None
        summary['sha256'] = digest
        stem = f"{path.parent.name}-{path.stem}"
        (args.out / f'{stem}.summary.json').write_text(json.dumps(summary, indent=1) + '\n')
        with (args.out / f'{stem}.members.jsonl').open('w') as f:
            for i, n in enumerate(names, 1):
                e = a.entries[i]
                f.write(json.dumps(dict(id=i, path=n, size=e['size'], offset=e['offset'],
                                        block=e['block'], md5=e['md5'])) + '\n')
        print(f"{stem}: members={summary['members']} flags={summary['flags']:#x} "
              f"top_ext={list(summary['extensions'].items())[:6]}")


if __name__ == '__main__':
    main()
