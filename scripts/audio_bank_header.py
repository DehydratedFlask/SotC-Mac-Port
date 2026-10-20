#!/usr/bin/env python3
"""Read byte-supported structure from an XFF-prefixed audio-bank member.

This is a structural observation only. It does not identify an audio codec, parse bank
records, or infer NGS2 behavior. The private AUD-01 corpus shows the word at byte 0x14
matching the decoded member length for all 44 XFF-prefixed .hd/.bd members.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parent))
from psarc import Psarc, PsarcError, _sha256_file

PROJECT_ROOT = Path(__file__).resolve().parents[1]
ASSET_MANIFEST_DIR = PROJECT_ROOT / 'local/manifests/assets'
DEFAULT_AUDIO_MANIFEST = ASSET_MANIFEST_DIR / 'audio-headers.json'
XFF_MAGIC = b'xff\0'
XFF_SIZE_WORD_OFFSET = 0x14
XFF_SIZE_WORD_WIDTH = 4
XFF_MIN_MEMBER_SIZE = XFF_SIZE_WORD_OFFSET + XFF_SIZE_WORD_WIDTH


class AudioBankFormatError(ValueError):
    """The member does not satisfy the observed XFF prefix/length structure."""


def parse_xff_member(data: bytes | bytearray | memoryview) -> dict[str, Any]:
    """Return only byte-supported facts from one complete XFF-prefixed member.

    The 4-byte ``xff\0`` prefix is observed in the selected pair and the wider
    AUD-01 .hd/.bd corpus. Bytes 0x04..0x13 are retained as opaque evidence. At
    0x14, a little-endian u32 equals the decoded member length in all 44 observed
    XFF-prefixed .hd/.bd members. No internal data spans or codec are inferred.
    """
    if not isinstance(data, (bytes, bytearray, memoryview)):
        raise TypeError('member data must be bytes-like')
    member = bytes(data)
    if len(member) < XFF_MIN_MEMBER_SIZE:
        raise AudioBankFormatError(
            f'member is {len(member)} bytes; need at least {XFF_MIN_MEMBER_SIZE} '
            'bytes to inspect the observed size word')
    if member[:len(XFF_MAGIC)] != XFF_MAGIC:
        raise AudioBankFormatError(
            f'unsupported member prefix {member[:len(XFF_MAGIC)].hex()}; '
            f'expected {XFF_MAGIC.hex()}')

    size_word = int.from_bytes(
        member[XFF_SIZE_WORD_OFFSET:XFF_MIN_MEMBER_SIZE], 'little')
    if size_word != len(member):
        raise AudioBankFormatError(
            f'u32le at {XFF_SIZE_WORD_OFFSET:#x} is {size_word}, '
            f'but decoded member length is {len(member)}')

    return {
        'magic_hex': member[:len(XFF_MAGIC)].hex(),
        'opaque_0x04_0x13_hex': member[len(XFF_MAGIC):XFF_SIZE_WORD_OFFSET].hex(),
        'u32le_at_0x14': size_word,
        'decoded_member_bytes': len(member),
        'u32le_at_0x14_matches_decoded_length': True,
        'field_meanings': 'UNKNOWN except for the observed length match',
        'codec': 'UNKNOWN',
    }


def _resolved_from_root(path_value: str | Path) -> Path:
    path = Path(path_value)
    return path.resolve() if path.is_absolute() else (PROJECT_ROOT / path).resolve()


def _matching_manifest_row(
    rows: list[dict[str, Any]], archive_path: Path, member_path: str
) -> dict[str, Any]:
    matches = [
        row for row in rows
        if row.get('path') == member_path
        and _resolved_from_root(row.get('archive', '')) == archive_path.resolve()
    ]
    if len(matches) != 1:
        raise AudioBankFormatError(
            f'expected one audio manifest row for {archive_path}:{member_path}, '
            f'found {len(matches)}')
    return matches[0]


def _load_archive_summary(archive_path: Path, summary_path: Path | None) -> tuple[Path, dict[str, Any]]:
    path = summary_path or (
        ASSET_MANIFEST_DIR / f'{archive_path.parent.name}-{archive_path.stem}.summary.json')
    try:
        summary = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        raise AudioBankFormatError(f'cannot read archive summary {path}: {exc}') from exc
    return path, summary


def inspect_members(
    archive_path: Path,
    member_paths: list[str],
    manifest_path: Path = DEFAULT_AUDIO_MANIFEST,
    summary_path: Path | None = None,
) -> dict[str, Any]:
    """Verify live archive/member identities and report bounded XFF observations."""
    archive_path = archive_path.resolve()
    manifest_path = manifest_path.resolve()
    try:
        manifest = json.loads(manifest_path.read_text())
        rows = manifest['members']
    except (OSError, json.JSONDecodeError, KeyError, TypeError) as exc:
        raise AudioBankFormatError(f'cannot read audio manifest {manifest_path}: {exc}') from exc

    summary_file, summary = _load_archive_summary(archive_path, summary_path)
    try:
        archive_bytes = archive_path.stat().st_size
        archive_sha256 = _sha256_file(archive_path)
    except (OSError, PsarcError) as exc:
        raise AudioBankFormatError(f'cannot hash archive {archive_path}: {exc}') from exc
    if summary.get('bytes') != archive_bytes:
        raise AudioBankFormatError(
            f'archive summary byte count {summary.get("bytes")} != live {archive_bytes}')
    if summary.get('sha256') != archive_sha256:
        raise AudioBankFormatError(
            f'archive summary SHA-256 {summary.get("sha256")} != live {archive_sha256}')

    archive = Psarc(archive_path)
    try:
        names = archive.paths()
        reported: list[dict[str, Any]] = []
        for member_path in member_paths:
            matches = [
                entry_id for entry_id, path in enumerate(names, start=1)
                if path == member_path
            ]
            if len(matches) != 1:
                raise AudioBankFormatError(
                    f'expected one exact PSARC path for {member_path!r}, found {len(matches)}')
            entry_id = matches[0]
            entry = archive.entries[entry_id]
            try:
                data = archive.read(entry_id)
            except (IndexError, PsarcError, OSError) as exc:
                raise AudioBankFormatError(
                    f'cannot read PSARC entry {entry_id} ({member_path}): {exc}') from exc
            if len(data) != entry['size']:
                raise AudioBankFormatError(
                    f'entry {entry_id} decoded to {len(data)} bytes, '
                    f'expected {entry["size"]}')

            digest = hashlib.sha256(data).hexdigest()
            expected = _matching_manifest_row(rows, archive_path, member_path)
            live_identity = {
                'entry_id': entry_id,
                'entry_offset': entry['offset'],
                'first_block': entry['block'],
                'size': entry['size'],
                'member_sha256': digest,
            }
            for key, value in live_identity.items():
                if expected.get(key) != value:
                    raise AudioBankFormatError(
                        f'manifest {key} mismatch for {member_path}: '
                        f'{expected.get(key)!r} != live {value!r}')

            observed = parse_xff_member(data)
            reported.append({
                'path': member_path,
                'psarc_entry_id': entry_id,
                'archive_offset': entry['offset'],
                'first_block': entry['block'],
                'decoded_member_bytes': len(data),
                'decoded_member_sha256': digest,
                'manifest_locator_and_hash_match': True,
                'observations': observed,
            })
    finally:
        archive.close()

    return {
        'schema_version': 1,
        'task': 'AUD-01-XFF-01',
        'archive': {
            'path': str(archive_path),
            'bytes': archive_bytes,
            'sha256': archive_sha256,
            'summary_path': str(summary_file),
            'summary_identity_match': True,
        },
        'limitations': [
            'The XFF bytes after the 4-byte prefix and the word at 0x14 remain opaque.',
            'The word at 0x14 is reported only as a little-endian value matching decoded length.',
            'Codec, internal records, NGS2 contract, gameplay use, and playback are UNKNOWN.',
        ],
        'members': reported,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True, help='local PSARC path')
    parser.add_argument('--member', action='append', required=True,
                        help='exact PSARC member path; repeat for each member')
    parser.add_argument('--manifest', type=Path, default=DEFAULT_AUDIO_MANIFEST,
                        help='private AUD-01 member identity manifest')
    parser.add_argument('--summary', type=Path,
                        help='private archive summary (defaults from archive parent/name)')
    args = parser.parse_args()
    try:
        report = inspect_members(args.archive, args.member, args.manifest, args.summary)
    except (AudioBankFormatError, PsarcError, OSError, ValueError) as exc:
        raise SystemExit(f'audio bank inspection failed: {exc}') from exc
    print(json.dumps(report, indent=2, sort_keys=True))


if __name__ == '__main__':
    main()
