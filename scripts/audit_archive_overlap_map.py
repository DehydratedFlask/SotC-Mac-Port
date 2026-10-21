#!/usr/bin/env python3
"""Build a metadata-only exact-path overlap map for the seven PSARC manifests.

The private JSON output contains shared member paths. Standard output contains
only aggregate counts. The manifest ``size`` field is treated solely as the
PSARC TOC-declared size; this audit does not read archive or member payloads.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from itertools import combinations
from pathlib import Path
from typing import Any, Mapping, Sequence


ROOT = Path(__file__).resolve().parent.parent
PRIVATE_OUTPUT_DIR = ROOT / "local/validation/data02-archive-overlap-map-01"
EXPECTED_PRIVATE_OUTPUT_DIR = ROOT / "local/validation/data02-archive-overlap-map-01"
MANIFEST_DIR = ROOT / "local/manifests/assets"

# This order is the stable archive order used for the 21 unordered pairs.
ARCHIVES: tuple[tuple[str, str], ...] = (
    ("base_cachedFiles", "CUSA08809-cachedFiles.members.jsonl"),
    ("patch_cachedfiles", "CUSA08809-patch-cachedfiles.members.jsonl"),
    ("pak0", "CUSA08809-pak0.members.jsonl"),
    ("pak1", "CUSA08809-pak1.members.jsonl"),
    ("pak2", "CUSA08809-pak2.members.jsonl"),
    ("pak3", "CUSA08809-pak3.members.jsonl"),
    ("pakN", "CUSA08809-pakN.members.jsonl"),
)


class ManifestError(ValueError):
    """An input is not a valid exact-path/declared-size member JSONL."""


def _unique_object(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("duplicate JSON object key")
        result[key] = value
    return result


def _reject_constant(_value: str) -> None:
    raise ValueError("non-standard JSON numeric constant")


def parse_manifest_bytes(archive_id: str, raw: bytes) -> dict[str, int]:
    """Parse path and size fields from JSONL without normalizing exact paths."""
    try:
        decoded = raw.decode("utf-8")
    except UnicodeDecodeError as exc:
        raise ManifestError(f"{archive_id}: manifest is not UTF-8") from exc

    members: dict[str, int] = {}
    lines = decoded.split("\n")
    if decoded.endswith("\n"):
        lines.pop()  # A final JSONL newline does not add an empty record.
    for line_number, line in enumerate(lines, start=1):
        if line.endswith("\r"):
            line = line[:-1]  # Accept CRLF while preserving other path characters.
        if not line.strip():
            raise ManifestError(f"{archive_id}: blank JSONL record at line {line_number}")
        try:
            record = json.loads(
                line,
                object_pairs_hook=_unique_object,
                parse_constant=_reject_constant,
            )
        except (json.JSONDecodeError, ValueError) as exc:
            raise ManifestError(
                f"{archive_id}: malformed JSONL record at line {line_number}"
            ) from exc

        if type(record) is not dict:
            raise ManifestError(f"{archive_id}: record at line {line_number} is not an object")
        path = record.get("path")
        if type(path) is not str or not path or "\x00" in path:
            raise ManifestError(f"{archive_id}: invalid path field at line {line_number}")
        size = record.get("size")
        # bool is a subclass of int, but is not a declared byte count.
        if type(size) is not int or size < 0:
            raise ManifestError(f"{archive_id}: invalid size field at line {line_number}")
        if path in members:
            raise ManifestError(f"{archive_id}: duplicate exact path record at line {line_number}")
        members[path] = size
    return members


def compare_member_maps(
    archive_ids: Sequence[str], member_maps: Mapping[str, Mapping[str, int]]
) -> list[dict[str, Any]]:
    """Return deterministic per-pair exact-path and declared-size relations."""
    ids = tuple(archive_ids)
    if len(ids) != len(set(ids)):
        raise ValueError("archive identifiers must be unique")
    if set(ids) != set(member_maps):
        raise ValueError("archive identifiers must match member-map keys")

    for member_map in member_maps.values():
        for size in member_map.values():
            if type(size) is not int or size < 0:
                raise ValueError("member maps must contain non-negative integer sizes")

    pairs: list[dict[str, Any]] = []
    for archive_a, archive_b in combinations(ids, 2):
        map_a = member_maps[archive_a]
        map_b = member_maps[archive_b]
        shared_paths = sorted(set(map_a).intersection(map_b))
        relations: list[dict[str, Any]] = []
        equal_count = 0
        different_count = 0
        for path in shared_paths:
            size_a = map_a[path]
            size_b = map_b[path]
            relation = "equal" if size_a == size_b else "different"
            if relation == "equal":
                equal_count += 1
            else:
                different_count += 1
            relations.append({
                "path": path,
                "declared_size_a": size_a,
                "declared_size_b": size_b,
                "declared_size_relation": relation,
            })
        pairs.append({
            "archive_a": archive_a,
            "archive_b": archive_b,
            "shared_path_count": len(shared_paths),
            "equal_declared_size_count": equal_count,
            "different_declared_size_count": different_count,
            "shared_paths": relations,
        })
    return pairs


def load_inputs() -> tuple[list[dict[str, Any]], dict[str, dict[str, int]]]:
    """Read and hash exactly the seven named JSONL metadata inputs."""
    inputs: list[dict[str, Any]] = []
    member_maps: dict[str, dict[str, int]] = {}
    for archive_id, filename in ARCHIVES:
        path = MANIFEST_DIR / filename
        try:
            raw = path.read_bytes()
        except OSError as exc:
            raise ManifestError(f"cannot read required manifest for {archive_id}: {exc}") from exc
        member_maps[archive_id] = parse_manifest_bytes(archive_id, raw)
        inputs.append({
            "archive_id": archive_id,
            "manifest": path.relative_to(ROOT).as_posix(),
            "manifest_bytes": len(raw),
            "manifest_sha256": hashlib.sha256(raw).hexdigest(),
            "record_count": len(member_maps[archive_id]),
        })
    return inputs, member_maps


def build_report() -> dict[str, Any]:
    inputs, member_maps = load_inputs()
    archive_ids = [archive_id for archive_id, _filename in ARCHIVES]
    pairs = compare_member_maps(archive_ids, member_maps)
    return {
        "schema_version": 1,
        "task_id": "DATA-02-ARCHIVE-OVERLAP-MAP-01",
        "analysis_scope": "exact member-path overlap and manifest size-field comparison",
        "size_interpretation": "The manifest size field is treated strictly as PSARC TOC-declared size.",
        "inputs": inputs,
        "archive_count": len(inputs),
        "unordered_pair_count": len(pairs),
        "pairwise_results": pairs,
        "limitations": [
            "JSONL metadata only; no archive or member payload bytes were read.",
            "Equal declared sizes do not establish byte equality or content identity.",
            "Pair order is deterministic serialization order and conveys no precedence.",
            "No loader behavior, Fios behavior, mount order, overlay policy, override priority, or game use is inferred.",
        ],
    }


def _output_path(value: str) -> Path:
    path = Path(value)
    if not path.is_absolute():
        path = ROOT / path
    path = path.resolve()
    repo_root = ROOT.resolve()
    private_root = PRIVATE_OUTPUT_DIR.resolve()
    expected_private_root = EXPECTED_PRIVATE_OUTPUT_DIR
    if private_root != expected_private_root:
        raise ValueError("private task output directory must resolve to its configured repository path")
    try:
        expected_private_root.relative_to(repo_root)
    except ValueError as exc:
        raise ValueError("configured private task output directory must stay inside the repository") from exc
    try:
        path.relative_to(private_root)
    except ValueError as exc:
        raise ValueError("output must stay under local/validation/data02-archive-overlap-map-01") from exc
    return path


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, help="private JSON report path under the task output directory")
    args = parser.parse_args(argv)

    try:
        output = _output_path(args.out)
        report = build_report()
        output.parent.mkdir(parents=True, exist_ok=True)
        serialized = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
        output.write_text(serialized, encoding="utf-8", newline="\n")
    except (ManifestError, OSError, ValueError) as exc:
        print(f"archive overlap audit failed: {exc}", file=sys.stderr)
        return 2

    all_pairs = report["pairwise_results"]
    shared = sum(row["shared_path_count"] for row in all_pairs)
    equal = sum(row["equal_declared_size_count"] for row in all_pairs)
    different = sum(row["different_declared_size_count"] for row in all_pairs)
    print(
        f"wrote {output.relative_to(ROOT).as_posix()} "
        f"archives={report['archive_count']} pairs={report['unordered_pair_count']} "
        f"shared_path_occurrences={shared} equal_declared_sizes={equal} "
        f"different_declared_sizes={different}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
