#!/usr/bin/env python3
"""Synthetic tests for the metadata-only archive overlap map."""

from __future__ import annotations

import io
import sys
import tempfile
import unittest
from contextlib import redirect_stderr
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))

import audit_archive_overlap_map as audit
from audit_archive_overlap_map import ManifestError, compare_member_maps, parse_manifest_bytes


class ParseManifestTests(unittest.TestCase):
    def test_parses_exact_paths_and_declared_sizes(self) -> None:
        parsed = parse_manifest_bytes(
            "synthetic", '{"path":"dir/item.bin","size":17,"id":1}\n'.encode()
        )
        self.assertEqual(parsed, {"dir/item.bin": 17})

    def test_preserves_unicode_line_separator_in_exact_path(self) -> None:
        parsed = parse_manifest_bytes("synthetic", '{"path":"dir/line\u2028break","size":9}\n'.encode())
        self.assertEqual(parsed, {"dir/line\u2028break": 9})

    def test_rejects_nul_in_path(self) -> None:
        raw = b'{"path":"dir\\u0000item.bin","size":1}\n'
        with self.assertRaises(ManifestError) as context:
            parse_manifest_bytes("synthetic", raw)
        self.assertEqual(str(context.exception), "synthetic: invalid path field at line 1")

    def test_rejects_duplicate_exact_path(self) -> None:
        raw = b'{"path":"same.bin","size":1}\n{"path":"same.bin","size":2}\n'
        with self.assertRaisesRegex(ManifestError, "duplicate exact path"):
            parse_manifest_bytes("synthetic", raw)

    def test_rejects_malformed_json_record(self) -> None:
        with self.assertRaisesRegex(ManifestError, "malformed JSONL record"):
            parse_manifest_bytes("synthetic", b'{"path":"broken.bin","size":1\n')

    def test_rejects_non_utf8_manifest(self) -> None:
        with self.assertRaisesRegex(ManifestError, "manifest is not UTF-8"):
            parse_manifest_bytes("synthetic", b"\xff")

    def test_rejects_invalid_record_shape_and_size(self) -> None:
        for raw in (
            b'[]\n',
            b'{"path":"missing-size.bin"}\n',
            b'{"path":"wrong-size.bin","size":true}\n',
            b'{"path":"negative-size.bin","size":-1}\n',
        ):
            with self.subTest(raw=raw), self.assertRaises(ManifestError):
                parse_manifest_bytes("synthetic", raw)

    def test_rejects_duplicate_json_object_key(self) -> None:
        with self.assertRaisesRegex(ManifestError, "malformed JSONL record"):
            parse_manifest_bytes("synthetic", b'{"path":"a","path":"b","size":1}\n')


class CompareMemberMapsTests(unittest.TestCase):
    def test_disjoint_sets_report_zero_overlap(self) -> None:
        pairs = compare_member_maps(
            ("a", "b"), {"a": {"a.bin": 1}, "b": {"b.bin": 2}}
        )
        self.assertEqual(len(pairs), 1)
        self.assertEqual(pairs[0]["shared_path_count"], 0)
        self.assertEqual(pairs[0]["equal_declared_size_count"], 0)
        self.assertEqual(pairs[0]["different_declared_size_count"], 0)
        self.assertEqual(pairs[0]["shared_paths"], [])

    def test_exact_equal_size_overlap(self) -> None:
        pairs = compare_member_maps(
            ("a", "b"), {"a": {"same.bin": 23}, "b": {"same.bin": 23}}
        )
        pair = pairs[0]
        self.assertEqual(pair["shared_path_count"], 1)
        self.assertEqual(pair["equal_declared_size_count"], 1)
        self.assertEqual(pair["different_declared_size_count"], 0)
        self.assertEqual(pair["shared_paths"][0]["declared_size_relation"], "equal")

    def test_exact_differing_size_overlap(self) -> None:
        pairs = compare_member_maps(
            ("a", "b"), {"a": {"same.bin": 23}, "b": {"same.bin": 24}}
        )
        pair = pairs[0]
        self.assertEqual(pair["shared_path_count"], 1)
        self.assertEqual(pair["equal_declared_size_count"], 0)
        self.assertEqual(pair["different_declared_size_count"], 1)
        self.assertEqual(pair["shared_paths"][0]["declared_size_relation"], "different")

    def test_reports_all_unordered_pairs_in_input_order(self) -> None:
        pairs = compare_member_maps(
            ("a", "b", "c"), {"a": {}, "b": {}, "c": {}}
        )
        self.assertEqual(
            [(pair["archive_a"], pair["archive_b"]) for pair in pairs],
            [("a", "b"), ("a", "c"), ("b", "c")],
        )


class OutputPathTests(unittest.TestCase):
    def test_rejects_symlinked_private_output_root_before_build_or_write(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            base = Path(temporary_directory)
            outside = base / "outside"
            outside.mkdir()

            for label in ("outside", "inside"):
                with self.subTest(target=label):
                    repo_root = base / f"repo-{label}"
                    scripts = repo_root / "scripts"
                    validation = repo_root / "local" / "validation"
                    scripts.mkdir(parents=True)
                    validation.mkdir(parents=True)
                    configured_root = validation / "data02-archive-overlap-map-01"
                    target = outside if label == "outside" else scripts
                    configured_root.symlink_to(target, target_is_directory=True)
                    output = configured_root / f"guard-test-{label}.json"
                    self.assertFalse(output.exists())
                    build_report = unittest.mock.Mock(
                        side_effect=AssertionError("must reject before building")
                    )
                    try:
                        with patch.object(audit, "ROOT", repo_root), \
                                patch.object(audit, "PRIVATE_OUTPUT_DIR", configured_root), \
                                patch.object(audit, "EXPECTED_PRIVATE_OUTPUT_DIR", configured_root), \
                                patch.object(audit, "build_report", build_report), \
                                redirect_stderr(io.StringIO()):
                            result = audit.main(["--out", str(output)])

                        self.assertEqual(result, 2)
                        build_report.assert_not_called()
                        self.assertFalse(output.exists())
                    finally:
                        output.unlink(missing_ok=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)
