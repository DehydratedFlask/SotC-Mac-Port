#!/usr/bin/env python3
"""Publish-path audit fixtures (no git required)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import audit_publish_paths as audit


class PublishPathTests(unittest.TestCase):
    def test_audio_headers_json_forbidden(self):
        bad = audit.violations(["local/manifests/assets/audio-headers.json"])
        self.assertTrue(bad, "game-derived audio-headers.json must fail audit")

    def test_classifier_scripts_allowed(self):
        paths = [
            "scripts/classify_audio_headers.py",
            "scripts/test_audio_headers.py",
            "scripts/audit_publish_paths.py",
            "scripts/test_audit_publish_paths.py",
        ]
        self.assertEqual(audit.violations(paths), [])

    def test_local_prefix_and_game_names(self):
        cases = [
            "local/re/eboot.elf",
            "Shadow of the Colossus/foo.pkg",
            "reference/shadPS4/README.md",
            "logs/extract.log",
            "something.psarc",
            "eboot.bin",
        ]
        for path in cases:
            with self.subTest(path=path):
                self.assertTrue(audit.violations([path]), path)

    def test_dotdot_traversal_into_forbidden_roots_rejected(self):
        # An allowlisted top component must not launder a path that resolves
        # into a forbidden root (or out of the repo) via "..".
        cases = [
            "docs/../local/manifests/x.json",
            "tools/../reference/shadPS4/README.md",
            "scripts/../Shadow of the Colossus/asset.dat",
            "./docs/../logs/extract.log",
            "docs\\..\\local\\x.json",
            "../outside.txt",
        ]
        for path in cases:
            with self.subTest(path=path):
                self.assertTrue(audit.violations([path]), path)

    def test_nested_forbidden_under_allowlisted_dirs_rejected(self):
        # Forbidden dirs/dumps must be caught at any depth, not only at the top.
        cases = [
            "docs/Shadow of the Colossus/x.dat",
            "tools/a/b/Shadow of the Colossus/x.dat",
            "native/local/x",
            "launcher/sub/local/x.json",
            "tools/AgentStores/state.json",
            "docs/.graft/index.db",
            "tools/foo/eboot.bin",
            "scripts/x.sprx",
            "native/libSceFoo.sprx",
            "docs/dump.self",
            "tools/x/dump.elf",
            "docs/game.pkg",
            "native/data/game.psarc",
            "docs/EBOOT.BIN",
        ]
        for path in cases:
            with self.subTest(path=path):
                self.assertTrue(audit.violations([path]), path)

    def test_nested_lookalike_names_still_allowed(self):
        cases = [
            "tools/lift/xref_rip.py",
            "tools/pkg-extractor/CMakeLists.txt",
            "docs/localization.md",
            "native/local_tests/a.c",
            "tools/graft/README.md",
            "docs/Shadow of the Colossus notes.md",
        ]
        self.assertEqual(audit.violations(cases), [])


if __name__ == "__main__":
    unittest.main()
