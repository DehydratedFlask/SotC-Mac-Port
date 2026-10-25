#!/usr/bin/env python3
"""Closed-world tests for raw default-material selector rejection paths."""

from __future__ import annotations

import argparse
import hashlib
import io
import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path
from contextlib import redirect_stderr
from unittest.mock import patch


MODULE_PATH = Path(__file__).with_name("raw_material_selector.py")
SPEC = importlib.util.spec_from_file_location("raw_material_selector", MODULE_PATH)
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)


class MaterialSelectorTests(unittest.TestCase):
    def test_archive_path_outside_workspace_is_rejected(self):
        with tempfile.TemporaryDirectory() as outside:
            archive = Path(outside) / "external.psarc"
            archive.write_bytes(b"not a game archive")
            with self.assertRaisesRegex(module.VerificationError, "outside the workspace"):
                module._input_file(archive, "test")

    def test_archive_path_inside_workspace_but_outside_local_is_rejected(self):
        archive = module.ROOT / "docs" / "INDEX.md"
        self.assertTrue(archive.is_file())
        with self.assertRaisesRegex(module.VerificationError, "outside private local/"):
            module._input_file(archive, "test")

    def test_exact_shader_literal_is_unique_and_reported(self):
        prefix = b"\x01\x02\x03"
        raw = prefix + module.SELECTOR_BYTES + b"\x00tail"
        result = module.find_material_shader(raw)
        self.assertEqual(result["path"], module.SHADER_PATH)
        self.assertEqual(result["literal"], module.SELECTOR_BYTES.decode("ascii"))
        self.assertEqual(result["offset"], len(prefix))

    def test_missing_shader_path_fails_closed(self):
        with self.assertRaises(module.UnknownMaterialSelector):
            module.find_material_shader(b"material-without-a-shader")

    def test_duplicate_shader_literal_fails_closed(self):
        raw = module.SELECTOR_BYTES + b"\x00" + module.SELECTOR_BYTES
        with self.assertRaisesRegex(module.UnknownMaterialSelector, "expected one"):
            module.find_material_shader(raw)

    def test_second_shader_suffix_fails_closed_even_if_target_is_present(self):
        raw = module.SELECTOR_BYTES + b"\x00$/materials/other.csdr"
        with self.assertRaisesRegex(module.UnknownMaterialSelector, "suffixes"):
            module.find_material_shader(raw)

    def test_unproved_selector_is_reported_as_unknown(self):
        error = module.UnknownMaterialSelector("ambiguous selector")
        stderr = io.StringIO()
        with patch.object(module, "run", side_effect=error):
            with redirect_stderr(stderr):
                status = module.main(["--output", "local/unused-output"])
        self.assertEqual(status, 2)
        self.assertIn("UNKNOWN: ambiguous selector", stderr.getvalue())

    def test_run_reports_unproved_cmat_before_creating_output(self):
        output_parent = module.ROOT / "local" / "validation" / "gpu02-raw-material-selector-20261025"
        with tempfile.TemporaryDirectory(dir=output_parent) as scratch:
            output = Path(scratch) / "unproved-material"
            args = argparse.Namespace(
                cachedfiles=Path("cachedFiles.psarc"),
                pak0=Path("pak0.psarc"),
                output=output,
            )
            with patch.object(module, "_input_file",
                                 side_effect=[module.ROOT / "cachedFiles.psarc",
                                              module.ROOT / "pak0.psarc"]), \
                    patch.object(module, "read_member", return_value=(b"no shader path", {})):
                with self.assertRaises(module.UnknownMaterialSelector):
                    module.run(args)
            self.assertFalse(output.exists())

    def test_changed_or_truncated_member_fails_closed(self):
        data = b"pinned test bytes"
        digest = hashlib.sha256(data).hexdigest()
        module.validate_pinned_bytes(data, label="fixture", size=len(data), digest=digest)
        with self.assertRaisesRegex(module.VerificationError, "size"):
            module.validate_pinned_bytes(data[:-1], label="fixture", size=len(data), digest=digest)
        with self.assertRaisesRegex(module.VerificationError, "SHA-256"):
            module.validate_pinned_bytes(data + b"!", label="fixture", size=len(data) + 1,
                                         digest=digest)

    def test_raw_csdr_truncation_is_not_accepted(self):
        with self.assertRaises(module.VerificationError):
            module.validate_program_spans(b"\x00" * 15)

    def test_unsupported_compressed_member_is_not_decoded(self):
        with self.assertRaises(module.VerificationError):
            module.validate_program_spans(b"\x8c" + b"\x00" * 15)

    def test_missing_exact_archive_member_fails_closed(self):
        class MissingMemberArchive:
            def __init__(self, _path):
                self.entries = []

            def entry_index(self, _path):
                raise module.PsarcError("no such member path")

            def close(self):
                pass

        with patch.object(module, "Psarc", MissingMemberArchive):
            with self.assertRaisesRegex(module.VerificationError, "lookup/read failed"):
                module.read_member(Path("missing.psarc"), module.SHADER_PATH,
                                   label="shader", size=1, digest="0" * 64)

    def test_same_size_changed_archive_bytes_fail_with_controlled_error(self):
        changed = b"evil"
        expected_digest = hashlib.sha256(b"good").hexdigest()

        class ChangedMemberArchive:
            entries = [{"md5": module.path_md5(module.SHADER_PATH), "size": len(changed)}]

            def __init__(self, _path):
                pass

            def entry_index(self, _path):
                return 0

            def read(self, _index):
                return changed

            def close(self):
                pass

        with patch.object(module, "Psarc", ChangedMemberArchive):
            with self.assertRaisesRegex(module.VerificationError, "SHA-256"):
                module.read_member(Path("fake.psarc"), module.SHADER_PATH,
                                   label="shader", size=len(changed), digest=expected_digest)

    def test_wrong_archive_entry_md5_fails_closed(self):
        expected_md5 = module.path_md5(module.SHADER_PATH)
        wrong_md5 = "0" * 32 if expected_md5 != "0" * 32 else "1" * 32

        class WrongMd5Archive:
            entries = [{"md5": wrong_md5, "size": 4}]

            def __init__(self, _path):
                pass

            def entry_index(self, path):
                self.requested_path = path
                return 0

            def read(self, _index):
                raise AssertionError("must reject TOC path identity before reading")

            def close(self):
                pass

        with patch.object(module, "Psarc", WrongMd5Archive):
            with self.assertRaisesRegex(module.VerificationError, r"MD5[(]exact path[)]"):
                module.read_member(Path("fake.psarc"), module.SHADER_PATH,
                                   label="shader", size=4, digest="0" * 64)

    def test_wrong_archive_toc_size_fails_closed(self):
        class WrongSizeArchive:
            entries = [{"md5": module.path_md5(module.SHADER_PATH), "size": 5}]

            def __init__(self, _path):
                pass

            def entry_index(self, path):
                self.requested_path = path
                return 0

            def read(self, _index):
                raise AssertionError("must reject TOC size before reading")

            def close(self):
                pass

        with patch.object(module, "Psarc", WrongSizeArchive):
            with self.assertRaisesRegex(module.VerificationError, "TOC size 5 != pinned 4"):
                module.read_member(Path("fake.psarc"), module.SHADER_PATH,
                                   label="shader", size=4, digest="0" * 64)

    def test_unexpected_program_layout_is_not_accepted(self):
        rows = [
            {"code_offset": 0, "length": 4, "info_offset": 8, "type": 1,
             "source_type": 1, "version": 7, "shader_hash": 0, "crc32": 0},
            {"code_offset": 4, "length": 4, "info_offset": 12, "type": 0,
             "source_type": 1, "version": 7, "shader_hash": 0, "crc32": 0},
        ]
        with patch.object(module, "parse_raw_csdr", return_value=rows):
            with self.assertRaisesRegex(module.VerificationError, "code_offset"):
                module.validate_program_spans(b"\x00" * 16)


if __name__ == "__main__":
    unittest.main(verbosity=2)
