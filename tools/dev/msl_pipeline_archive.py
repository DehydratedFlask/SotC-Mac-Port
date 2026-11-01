#!/usr/bin/env python3
"""Deterministic offline MSL -> AIR/`.metallib` archive; never links a game runtime.

Input is a manifest of shader entries delivered by the offline lowering
(`tools/dev/gcn_to_msl.py`): ``{"id", "stage", "msl_source_path"}``. Each MSL source is
compiled by the Apple Metal compiler into a content-addressed ``out/<sha256>.air``; the AIR
files of each stage are linked into one ``out/<stage>.metallib``. The archive manifest
records, per entry, the MSL digest, the AIR digest, byte sizes, the Metal compiler version
string and the exact command line.

Archives are content addressed by MSL digest. The recorded command runs from the archive
root, so its paths stay relative; the compiler itself embeds the resolved source path in
the AIR, which therefore reproduces only from the same source tree location.

This is an offline tooling step only: no Vulkan, no guest GPU, no game executable, no
runtime dependency on the archive. The manifest deliberately contains no absolute paths
and no game bytes; sources and the archive stay outside published paths.

  python3 -I tools/dev/msl_pipeline_archive.py --manifest in.json --out private/archive
  python3 -I tools/dev/msl_pipeline_archive.py --manifest in.json --out private/archive --check

Exit codes: 0 success, 1 failure (including `--check` digest mismatch), 75 Metal
toolchain unavailable (skip; reason on stderr).
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys

INPUT_SCHEMA = 'msl-pipeline-input-v1'
ARCHIVE_SCHEMA = 'msl-pipeline-archive-v1'
ARCHIVE_NAME = 'msl_archive.json'
ARTIFACT_DIR = 'out'
CHECK_DIR = '.check'
STAGES = ('vertex', 'fragment')
SDK = 'macosx'
XCRUN = 'xcrun'
EXIT_OK = 0
EXIT_FAILED = 1
EXIT_TOOLCHAIN_UNAVAILABLE = 75


class ArchiveError(Exception):
    """A deterministic archive failure (bad input, compiler error, digest mismatch)."""


class ToolchainUnavailable(Exception):
    """The Apple Metal compiler is not installed or not runnable."""


def _sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _sha256_file(path: Path) -> str:
    return _sha256_bytes(path.read_bytes())


def probe_toolchain(env: dict | None = None) -> str:
    """Return the `metal --version` string, or raise ToolchainUnavailable."""
    try:
        proc = subprocess.run([XCRUN, '-sdk', SDK, 'metal', '--version'],
                              capture_output=True, text=True, env=env)
    except OSError as exc:
        raise ToolchainUnavailable(f'cannot run {XCRUN!r}: {exc}') from None
    if proc.returncode != 0:
        detail = (proc.stderr or proc.stdout or '').strip().splitlines()
        message = detail[0] if detail else f'{XCRUN} exited {proc.returncode}'
        raise ToolchainUnavailable(f'Metal compiler unavailable: {message}')
    lines = (proc.stdout or proc.stderr or '').strip().splitlines()
    if not lines:
        raise ToolchainUnavailable('Metal compiler reported no version string')
    return ' '.join(lines[0].split())


def _validate_stage(stage: str) -> None:
    if stage not in STAGES:
        raise ArchiveError(f'unsupported stage {stage!r}; expected one of {", ".join(STAGES)}')


def load_entries(manifest_path: Path) -> list[dict]:
    """Parse and validate the input manifest; every source must sit beside the manifest."""
    path = manifest_path.resolve()
    try:
        document = json.loads(path.read_text(encoding='utf-8'))
    except (OSError, ValueError) as exc:
        raise ArchiveError(f'cannot read manifest {path.name}: {exc}') from None
    if isinstance(document, dict):
        if document.get('schema', INPUT_SCHEMA) != INPUT_SCHEMA:
            raise ArchiveError(f'unsupported input schema {document.get("schema")!r}')
        entries = document.get('entries')
    else:
        entries = document
    if not isinstance(entries, list) or not entries:
        raise ArchiveError('manifest must contain a non-empty "entries" list')

    root = path.parent
    seen: set[str] = set()
    loaded = []
    for index, item in enumerate(entries):
        if not isinstance(item, dict):
            raise ArchiveError(f'entry {index} is not an object')
        unknown = sorted(set(item) - {'id', 'stage', 'msl_source_path'})
        if unknown:
            raise ArchiveError(f'entry {index} has unknown field(s): {", ".join(unknown)}')
        for field in ('id', 'stage', 'msl_source_path'):
            if not isinstance(item.get(field), str) or not item[field]:
                raise ArchiveError(f'entry {index} requires a non-empty string {field!r}')
        identity, stage, source_text = item['id'], item['stage'], item['msl_source_path']
        if not all(c.isalnum() or c in '_.-' for c in identity) or identity in ('.', '..'):
            raise ArchiveError(f'entry {index} id {identity!r} is not a safe archive id')
        if identity in seen:
            raise ArchiveError(f'duplicate entry id {identity!r}')
        seen.add(identity)
        _validate_stage(stage)
        source = Path(source_text)
        if source.is_absolute():
            raise ArchiveError(
                f'entry {identity!r} msl_source_path must be relative to the manifest directory')
        resolved = (root / source).resolve()
        if not resolved.is_relative_to(root):
            raise ArchiveError(
                f'entry {identity!r} msl_source_path must resolve inside the manifest directory')
        if not resolved.is_file():
            raise ArchiveError(f'entry {identity!r} source is missing: {source_text}')
        loaded.append(dict(id=identity, stage=stage, source=resolved,
                           declared=Path(source_text).as_posix()))
    return loaded


def _run(argv: list[str], cwd: Path) -> None:
    try:
        proc = subprocess.run(argv, cwd=str(cwd), capture_output=True, text=True)
    except OSError as exc:
        raise ArchiveError(f'cannot run {argv[0]!r}: {exc}') from None
    if proc.returncode != 0:
        detail = (proc.stderr or proc.stdout or '').strip()
        raise ArchiveError(f'command failed ({proc.returncode}): {shlex.join(argv)}'
                           + (f'\n{detail}' if detail else ''))


def _relative(path: Path, root: Path) -> str:
    return Path(os.path.relpath(path, root)).as_posix()


def _reject_absolute_paths(document: dict) -> None:
    """Refuse to publish an archive manifest that leaks an absolute filesystem path."""
    stack = [document]
    while stack:
        item = stack.pop()
        if isinstance(item, dict):
            stack.extend(item.values())
        elif isinstance(item, list):
            stack.extend(item)
        elif isinstance(item, str):
            if item.startswith('/') or (len(item) > 2 and item[1] == ':' and item[2] in '\\/'):
                raise ArchiveError(f'archive manifest would embed an absolute path: {item!r}')


def _verified_archive(out_dir: Path, toolchain: str) -> dict:
    """Load the committed archive for `--check` and verify its recorded artifacts."""
    archive_path = out_dir / ARCHIVE_NAME
    if not archive_path.is_file():
        raise ArchiveError(f'no archive manifest to check: {archive_path.name}')
    try:
        archived = json.loads(archive_path.read_text(encoding='utf-8'))
    except ValueError as exc:
        raise ArchiveError(f'archive manifest is not valid JSON: {exc}') from None
    if archived.get('schema') != ARCHIVE_SCHEMA:
        raise ArchiveError(f'unsupported archive schema {archived.get("schema")!r}')
    if archived.get('toolchain') != toolchain:
        raise ArchiveError(f'Metal toolchain version changed: archived '
                           f'{archived.get("toolchain")!r}, local {toolchain!r}')
    recorded = [entry['air']['path'] for entry in archived.get('entries', [])]
    recorded += [record['path'] for record in (archived.get('stages') or {}).values()]
    digests = {entry['air']['path']: entry['air']['sha256']
               for entry in archived.get('entries', [])}
    digests.update({record['path']: record['sha256']
                    for record in (archived.get('stages') or {}).values()})
    for relative in recorded:
        path = out_dir / relative
        if not path.is_file():
            raise ArchiveError(f'archived artifact is missing: {relative}')
        if _sha256_file(path) != digests[relative]:
            raise ArchiveError(f'archived artifact digest changed: {relative}')
    return archived


def build(manifest_path: Path, out_dir: Path, check: bool = False) -> dict:
    """Compile the manifest into `out_dir` and return the archive document.

    With ``check=True`` the artifacts are recompiled into a scratch directory inside
    ``out_dir`` and compared against the committed archive; the archive is not rewritten.
    """
    toolchain = probe_toolchain()
    entries = load_entries(manifest_path)
    out_dir = out_dir.resolve()
    artifact_root = out_dir / ARTIFACT_DIR
    archived = _verified_archive(out_dir, toolchain) if check else None
    scratch = artifact_root / CHECK_DIR
    artifact_root.mkdir(parents=True, exist_ok=True)
    if check:
        scratch.mkdir(exist_ok=True)

    records = []
    air_by_digest: dict[str, Path] = {}
    stage_digests: dict[str, list[str]] = {stage: [] for stage in STAGES}
    try:
        for entry in entries:
            raw = entry['source'].read_bytes()
            msl_digest = _sha256_bytes(raw)
            air_path = (scratch if check else artifact_root) / f'{msl_digest}.air'
            argv = [XCRUN, '-sdk', SDK, 'metal', '-c',
                    _relative(entry['source'], out_dir),
                    '-o', _relative(air_path, out_dir)]
            _run(argv, cwd=out_dir)
            if not air_path.is_file():
                raise ArchiveError(f'compiler produced no AIR for {entry["id"]!r}')
            air_by_digest[msl_digest] = air_path
            if msl_digest not in stage_digests[entry['stage']]:
                stage_digests[entry['stage']].append(msl_digest)
            records.append(dict(
                id=entry['id'], stage=entry['stage'],
                msl_source=entry['declared'],
                msl_sha256=msl_digest, msl_bytes=len(raw),
                air=dict(path=_relative(artifact_root / f'{msl_digest}.air', out_dir),
                         sha256=_sha256_file(air_path), bytes=air_path.stat().st_size),
                command=dict(argv=argv, line=shlex.join(argv)),
            ))

        stages: dict[str, dict] = {}
        for stage in STAGES:
            digests = sorted(stage_digests[stage])
            if not digests:
                continue
            metallib_path = (scratch if check else artifact_root) / f'{stage}.metallib'
            argv = [XCRUN, '-sdk', SDK, 'metal', '-o',
                    _relative(metallib_path, out_dir)]
            argv += [_relative(air_by_digest[digest], out_dir) for digest in digests]
            _run(argv, cwd=out_dir)
            if not metallib_path.is_file():
                raise ArchiveError(f'linker produced no metallib for stage {stage!r}')
            stages[stage] = dict(
                path=_relative(artifact_root / f'{stage}.metallib', out_dir),
                sha256=_sha256_file(metallib_path), bytes=metallib_path.stat().st_size,
                air_sha256=digests, command=dict(argv=argv, line=shlex.join(argv)),
            )

        document = dict(
            schema=ARCHIVE_SCHEMA,
            toolchain=toolchain,
            sdk=SDK,
            counts=dict(entries=len(records), unique_air=len(air_by_digest),
                        stages=sorted(stages)),
            stages=stages,
            entries=sorted(records, key=lambda record: record['id']),
        )
        _reject_absolute_paths(document)
        if check:
            mismatch = compare(archived, document)
            if mismatch:
                raise ArchiveError('archive check failed:\n' + '\n'.join(mismatch))
        else:
            with open(out_dir / ARCHIVE_NAME, 'w', encoding='utf-8', newline='\n') as handle:
                handle.write(json.dumps(document, indent=2, sort_keys=True) + '\n')
        return document
    finally:
        if check:
            for leftover in scratch.glob('*'):
                leftover.unlink()
            scratch.rmdir()


def compare(archived: dict, local: dict) -> list[str]:
    """Return human-readable differences between an archived and a fresh manifest."""
    problems = []
    old = {entry['id']: entry for entry in archived.get('entries', [])}
    new = {entry['id']: entry for entry in local.get('entries', [])}
    for identity in sorted(old.keys() - new.keys()):
        problems.append(f'{identity}: archived entry is missing from the local manifest')
    for identity in sorted(new.keys() - old.keys()):
        problems.append(f'{identity}: local entry is not present in the archive')
    for identity in sorted(old.keys() & new.keys()):
        before, after = old[identity], new[identity]
        if before.get('stage') != after.get('stage'):
            problems.append(f'{identity}: stage {before.get("stage")} -> {after.get("stage")}')
        for field in ('msl_sha256', 'msl_bytes'):
            if before.get(field) != after.get(field):
                problems.append(f'{identity}: {field} differs: archived '
                                f'{before.get(field)} != local {after.get(field)}')
        for field in ('sha256', 'bytes'):
            if before.get('air', {}).get(field) != after.get('air', {}).get(field):
                problems.append(f'{identity}: air {field} differs: archived '
                                f'{before.get("air", {}).get(field)} != '
                                f'local {after.get("air", {}).get(field)}')
    old_stages, new_stages = archived.get('stages') or {}, local.get('stages') or {}
    for stage, record in sorted(new_stages.items()):
        prior = old_stages.get(stage)
        if prior is None:
            problems.append(f'{stage}: metallib is not present in the archive')
            continue
        for field in ('sha256', 'bytes'):
            if prior.get(field) != record.get(field):
                problems.append(f'{stage}: metallib {field} differs: archived '
                                f'{prior.get(field)} != local {record.get(field)}')
    for stage in sorted(old_stages.keys() - new_stages.keys()):
        problems.append(f'{stage}: archived metallib was not relinked')
    return problems


def _report(document: dict) -> None:
    for entry in document['entries']:
        print(f'{entry["stage"]}\t{entry["id"]}\tmsl={entry["msl_sha256"]} '
              f'air={entry["air"]["sha256"]} air_bytes={entry["air"]["bytes"]}')
    for stage, record in sorted(document['stages'].items()):
        print(f'metallib\t{stage}\t{record["path"]}\tsha256={record["sha256"]} '
              f'bytes={record["bytes"]}')


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--manifest', required=True, type=Path,
                        help='JSON manifest of {id, stage, msl_source_path} entries')
    parser.add_argument('--out', required=True, type=Path,
                        help=f'archive root; receives {ARTIFACT_DIR}/ and {ARCHIVE_NAME}')
    parser.add_argument('--check', action='store_true',
                        help='recompile and fail if any recorded digest differs')
    args = parser.parse_args(argv)
    try:
        document = build(args.manifest, args.out, check=args.check)
    except ToolchainUnavailable as exc:
        print(f'SKIPPED: {exc}', file=sys.stderr)
        return EXIT_TOOLCHAIN_UNAVAILABLE
    except (ArchiveError, OSError, ValueError, KeyError) as exc:
        print(f'FAILED: {exc}', file=sys.stderr)
        return EXIT_FAILED
    _report(document)
    verb = 'PASSED check' if args.check else 'PASSED archive'
    print(f'{verb}: {args.out.resolve().name}/{ARCHIVE_NAME} '
          f'entries={document["counts"]["entries"]} '
          f'unique_air={document["counts"]["unique_air"]} '
          f'toolchain={document["toolchain"]!r}')
    return EXIT_OK


if __name__ == '__main__':
    raise SystemExit(main())
