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

    def test_parses_two_crlf_delimited_records(self) -> None:
        parsed = parse_manifest_bytes(
            "synthetic",
            b'{"path":"first.bin","size":17}\r\n'
            b'{"path":"nested/second.bin","size":29}\r\n',
        )
        self.assertEqual(parsed, {"first.bin": 17, "nested/second.bin": 29})

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

    def test_rejects_nonstandard_json_numeric_constants(self) -> None:
        for constant in (b"NaN", b"Infinity", b"-Infinity"):
            raw = b'{"path":"fixture.bin","size":' + constant + b'}\n'
            with self.subTest(constant=constant):
                with self.assertRaises(ManifestError) as context:
                    parse_manifest_bytes("synthetic", raw)
                self.assertEqual(
                    str(context.exception),
                    "synthetic: malformed JSONL record at line 1",
                )

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

    def test_empty_manifest_is_a_blank_record_not_zero_records(self) -> None:
        # Honest result: the parser does NOT treat a zero-byte file as "zero
        # records". A JSONL file with no records is indistinguishable from a
        # missing or truncated one, so the blank-record guard fires on line 1.
        with self.assertRaises(ManifestError) as context:
            parse_manifest_bytes("synthetic", b"")
        self.assertEqual(
            str(context.exception), "synthetic: blank JSONL record at line 1"
        )

    def test_blank_line_only_manifest_names_the_first_blank_line(self) -> None:
        for raw, line in (
            (b"\n", 1),
            (b"\n\n", 1),
            (b"   \n", 1),
            (b"\t \n", 1),
            (b'{"path":"a.bin","size":1}\n\n', 2),
        ):
            with self.subTest(raw=raw):
                with self.assertRaises(ManifestError) as context:
                    parse_manifest_bytes("synthetic", raw)
                self.assertEqual(
                    str(context.exception),
                    f"synthetic: blank JSONL record at line {line}",
                )

    def test_rejects_empty_string_path(self) -> None:
        for raw in (b'{"path":"","size":1}\n', b'{"path":"","size":1}'):
            with self.subTest(raw=raw):
                with self.assertRaises(ManifestError) as context:
                    parse_manifest_bytes("synthetic", raw)
                self.assertEqual(
                    str(context.exception), "synthetic: invalid path field at line 1"
                )

    def test_accepts_backslash_path_verbatim(self) -> None:
        # A backslash is an ordinary filename character on POSIX, not a separator,
        # and this audit only intersects exact strings; it never opens a manifest
        # path. Backslashes and ".." segments are therefore accepted unnormalized
        # and reported verbatim instead of being silently rewritten.
        self.assertEqual(
            parse_manifest_bytes("synthetic", b'{"path":"dir\\\\item.bin","size":5}\n'),
            {"dir\\item.bin": 5},
        )
        self.assertEqual(
            parse_manifest_bytes(
                "synthetic", b'{"path":"..\\\\..\\\\etc\\\\passwd","size":7}\n'
            ),
            {"..\\..\\etc\\passwd": 7},
        )
        # Forward-slash and dot segments are likewise preserved exactly, so two
        # differently spelled paths stay distinct records rather than colliding.
        self.assertEqual(
            parse_manifest_bytes(
                "synthetic",
                b'{"path":"a/./b/../c.bin","size":1}\n{"path":"a/c.bin","size":2}\n',
            ),
            {"a/./b/../c.bin": 1, "a/c.bin": 2},
        )


class CompareMemberMapsTests(unittest.TestCase):
    def test_rejects_duplicate_archive_identifiers(self) -> None:
        with self.assertRaises(ValueError) as context:
            compare_member_maps(("base", "base"), {"base": {"item.bin": 1}})
        self.assertEqual(str(context.exception), "archive identifiers must be unique")

    def test_rejects_archive_identifiers_that_do_not_match_member_map_keys(self) -> None:
        with self.assertRaises(ValueError) as context:
            compare_member_maps(("base", "patch"), {"base": {}})
        self.assertEqual(
            str(context.exception),
            "archive identifiers must match member-map keys",
        )

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

    def test_rejects_invalid_sizes_on_unshared_paths(self) -> None:
        for path, invalid_size in (("negative.bin", -1), ("boolean.bin", True)):
            with self.subTest(path=path, invalid_size=invalid_size):
                with self.assertRaisesRegex(
                    ValueError, "member maps must contain non-negative integer sizes"
                ):
                    compare_member_maps(
                        ("a", "b"),
                        {"a": {path: invalid_size}, "b": {"other.bin": 1}},
                    )

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
