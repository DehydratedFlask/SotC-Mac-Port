#!/usr/bin/env python3
"""Synthetic two-shader archive checks; no game shader, no private input, no G3 claim.

The Metal toolchain is probed once: when it is absent the compile tests skip with the
reason, while the missing-toolchain skip path still runs.
"""
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout


def module(name, file):
    spec = importlib.util.spec_from_file_location(name, Path(__file__).with_name(file))
    value = importlib.util.module_from_spec(spec)
    sys.modules[name] = value
    spec.loader.exec_module(value)
    return value


m = module('msl_pipeline_archive', 'msl_pipeline_archive.py')

TOOLCHAIN = None
TOOLCHAIN_REASON = ''
try:
    TOOLCHAIN = m.probe_toolchain()
except m.ToolchainUnavailable as exc:  # recorded, never hidden
    TOOLCHAIN_REASON = str(exc)

VERTEX_MSL = '''#include <metal_stdlib>
using namespace metal;

struct TinyVertexOutput { float4 position [[position]]; };

vertex TinyVertexOutput tiny_vertex(uint vid [[vertex_id]]) {
    TinyVertexOutput out;
    out.position = float4(float(vid), 0.0f, 0.0f, 1.0f);
    return out;
}
'''

FRAGMENT_MSL = '''#include <metal_stdlib>
using namespace metal;

fragment float4 tiny_fragment() {
    return float4(1.0f, 0.5f, 0.25f, 1.0f);
}
'''


def run_tool(*args, env=None):
    """Invoke the CLI in-process, returning (code, stdout, stderr)."""
    out, err = io.StringIO(), io.StringIO()
    with redirect_stdout(out), redirect_stderr(err):
        code = m.main(list(args))
    return code, out.getvalue(), err.getvalue()


class ArchiveFixture(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory()
        self.root = Path(self.scratch.name)
        self.shaders = self.root / 'shaders'
        self.shaders.mkdir()
        (self.shaders / 'tiny_vertex.metal').write_text(VERTEX_MSL, encoding='utf-8')
        (self.shaders / 'tiny_fragment.metal').write_text(FRAGMENT_MSL, encoding='utf-8')
        self.manifest = self.root / 'input.json'
        self.manifest.write_text(json.dumps({
            'schema': m.INPUT_SCHEMA,
            'entries': [
                {'id': 'tiny_vertex', 'stage': 'vertex',
                 'msl_source_path': 'shaders/tiny_vertex.metal'},
                {'id': 'tiny_fragment', 'stage': 'fragment',
                 'msl_source_path': 'shaders/tiny_fragment.metal'},
            ],
        }, indent=2), encoding='utf-8')
        self.archive = self.root / 'archive'

    def tearDown(self):
        self.scratch.cleanup()

    def build_archive(self, archive=None):
        code, out, err = run_tool('--manifest', str(self.manifest),
                                  '--out', str(archive or self.archive))
        self.assertEqual(code, 0, f'archive failed: {err}')
        return json.loads(((archive or self.archive) / m.ARCHIVE_NAME).read_text())


@unittest.skipUnless(TOOLCHAIN, f'Metal toolchain unavailable: {TOOLCHAIN_REASON}')
class ArchiveToolchainTests(ArchiveFixture):
    def test_archive_is_content_addressed_sorted_and_pathless(self):
        document = self.build_archive()
        self.assertEqual(document['schema'], m.ARCHIVE_SCHEMA)
        self.assertEqual(document['toolchain'], TOOLCHAIN)
        self.assertEqual([e['id'] for e in document['entries']],
                         ['tiny_fragment', 'tiny_vertex'])
        self.assertEqual(document['counts'], {'entries': 2, 'unique_air': 2,
                                              'stages': ['fragment', 'vertex']})

        sources = {'tiny_vertex': self.shaders / 'tiny_vertex.metal',
                   'tiny_fragment': self.shaders / 'tiny_fragment.metal'}
        for entry in document['entries']:
            raw = sources[entry['id']].read_bytes()
            self.assertEqual(entry['msl_sha256'], m._sha256_bytes(raw))
            self.assertEqual(entry['msl_bytes'], len(raw))
            self.assertEqual(entry['air']['path'],
                             f'{m.ARTIFACT_DIR}/{entry["msl_sha256"]}.air')
            air = self.archive / entry['air']['path']
            self.assertTrue(air.is_file(), entry['air']['path'])
            self.assertEqual(entry['air']['sha256'], m._sha256_file(air))
            self.assertEqual(entry['air']['bytes'], air.stat().st_size)
            source_name = ('shaders/tiny_fragment.metal' if entry['id'] == 'tiny_fragment'
                           else 'shaders/tiny_vertex.metal')
            self.assertEqual(entry['msl_source'], source_name)
            self.assertEqual(entry['command']['argv'][:5],
                             ['xcrun', '-sdk', 'macosx', 'metal', '-c'])
            self.assertTrue(entry['command']['argv'][5].endswith(source_name))
            self.assertEqual(entry['command']['argv'][7], entry['air']['path'])
            self.assertTrue(entry['command']['line'].startswith(
                'xcrun -sdk macosx metal -c '))
            for token in entry['command']['argv']:
                self.assertFalse(token.startswith('/'), token)

        for stage in ('vertex', 'fragment'):
            record = document['stages'][stage]
            self.assertEqual(record['path'], f'{m.ARTIFACT_DIR}/{stage}.metallib')
            metallib = self.archive / record['path']
            self.assertTrue(metallib.is_file())
            self.assertTrue(metallib.stat().st_size > 0)
            self.assertEqual(record['sha256'], m._sha256_file(metallib))
            self.assertEqual(record['bytes'], metallib.stat().st_size)
            self.assertEqual(len(record['air_sha256']), 1)

        text = (self.archive / m.ARCHIVE_NAME).read_text()
        for secret in (str(self.root), str(Path.home()), '/Volumes', '/private'):
            self.assertNotIn(secret, text)
        self.assertNotIn(str(self.root), json.dumps(document))

    def test_second_run_reproduces_identical_manifest(self):
        first = self.build_archive()
        other = self.root / 'archive2'
        second = self.build_archive(other)
        self.assertEqual((self.archive / m.ARCHIVE_NAME).read_bytes(),
                         (other / m.ARCHIVE_NAME).read_bytes())
        self.assertEqual(first, second)
        code, _, err = run_tool('--manifest', str(self.manifest), '--out', str(other),
                                '--check')
        self.assertEqual(code, 0, err)
        self.assertIn('PASSED check', run_tool('--manifest', str(self.manifest),
                                               '--out', str(other), '--check')[1])

    def test_check_detects_source_mutation(self):
        document = self.build_archive()
        manifest_before = (self.archive / m.ARCHIVE_NAME).read_bytes()
        mutated = FRAGMENT_MSL.replace('0.5f', '0.6f')
        self.assertNotEqual(mutated, FRAGMENT_MSL)
        (self.shaders / 'tiny_fragment.metal').write_text(mutated, encoding='utf-8')

        code, _, err = run_tool('--manifest', str(self.manifest), '--out',
                                str(self.archive), '--check')
        self.assertEqual(code, 1, f'mutation was not detected: {err}')
        self.assertIn('archive check failed', err)
        self.assertIn('tiny_fragment: msl_sha256 differs', err)
        self.assertIn('tiny_fragment: air sha256 differs', err)
        self.assertIn('fragment: metallib sha256 differs', err)
        self.assertNotIn('tiny_vertex: msl_sha256 differs', err)
        self.assertEqual((self.archive / m.ARCHIVE_NAME).read_bytes(), manifest_before)
        self.assertEqual(m._sha256_file(self.archive / document['entries'][0]['air']['path']),
                         document['entries'][0]['air']['sha256'])
        expected = sorted(['fragment.metallib', 'vertex.metallib']
                          + [f'{e["msl_sha256"]}.air' for e in document['entries']])
        self.assertEqual(sorted(p.name for p in (self.archive / m.ARTIFACT_DIR).iterdir()),
                         expected)

    def test_check_detects_missing_and_changed_artifacts(self):
        document = self.build_archive()
        air = self.archive / document['entries'][0]['air']['path']
        air.write_bytes(air.read_bytes() + b'corrupt')
        code, _, err = run_tool('--manifest', str(self.manifest), '--out',
                                str(self.archive), '--check')
        self.assertEqual(code, 1)
        self.assertIn('archived artifact digest changed', err)

        air.unlink()
        code, _, err = run_tool('--manifest', str(self.manifest), '--out',
                                str(self.archive), '--check')
        self.assertEqual(code, 1)
        self.assertIn('archived artifact is missing', err)

    def test_manifest_rejects_absolute_or_escaping_sources(self):
        for bad, expected in ((str(self.shaders / 'tiny_vertex.metal'),
                               'must be relative to the manifest directory'),
                              ('../outside.metal', 'must resolve inside the manifest directory')):
            self.manifest.write_text(json.dumps({'entries': [
                {'id': 'tiny_vertex', 'stage': 'vertex', 'msl_source_path': bad}]}),
                encoding='utf-8')
            code, _, err = run_tool('--manifest', str(self.manifest),
                                    '--out', str(self.archive))
            self.assertEqual(code, 1, err)
            self.assertIn(expected, err)
            self.assertFalse((self.archive / m.ARCHIVE_NAME).exists())


class MissingToolchainTests(ArchiveFixture):
    def test_missing_toolchain_exits_75_with_reason(self):
        empty = self.root / 'empty-bin'
        empty.mkdir()
        env = dict(os.environ, PATH=str(empty))
        proc = subprocess.run([sys.executable, '-I', str(m.__file__),
                               '--manifest', str(self.manifest),
                               '--out', str(self.archive)],
                              capture_output=True, text=True, env=env)
        self.assertEqual(proc.returncode, 75, proc.stderr)
        self.assertIn('SKIPPED:', proc.stderr)
        self.assertIn('xcrun', proc.stderr)
        self.assertNotIn('PASSED', proc.stdout)
        self.assertFalse((self.archive / m.ARCHIVE_NAME).exists())
        self.assertFalse((self.archive / m.ARTIFACT_DIR).exists())

    def test_missing_metal_component_reports_toolchain_reason(self):
        """An xcrun without the Metal component must skip with its own reason, not fail."""
        fakebin = self.root / 'fake-bin'
        fakebin.mkdir()
        fake = fakebin / 'xcrun'
        fake.write_text('#!/bin/sh\n'
                        'echo "error: cannot execute tool \'metal\' due to missing Metal '
                        'Toolchain; use: xcodebuild -downloadComponent MetalToolchain" >&2\n'
                        'exit 1\n', encoding='utf-8')
        fake.chmod(0o755)
        proc = subprocess.run([sys.executable, '-I', str(m.__file__),
                               '--manifest', str(self.manifest),
                               '--out', str(self.archive)],
                              capture_output=True, text=True,
                              env=dict(os.environ, PATH=str(fakebin)))
        self.assertEqual(proc.returncode, 75, proc.stderr)
        self.assertIn('SKIPPED: Metal compiler unavailable', proc.stderr)
        self.assertIn('missing Metal Toolchain', proc.stderr)
        self.assertFalse((self.archive / m.ARCHIVE_NAME).exists())

    def test_missing_toolchain_does_not_hide_bad_manifest(self):
        """The skip is a toolchain statement, not a pass for a broken manifest."""
        empty = self.root / 'empty-bin'
        empty.mkdir()
        self.manifest.write_text(json.dumps({'entries': [
            {'id': 'tiny_vertex', 'stage': 'geometry',
             'msl_source_path': 'shaders/tiny_vertex.metal'}]}), encoding='utf-8')
        proc = subprocess.run([sys.executable, '-I', str(m.__file__),
                               '--manifest', str(self.manifest),
                               '--out', str(self.archive)],
                              capture_output=True, text=True,
                              env=dict(os.environ, PATH=str(empty)))
        self.assertEqual(proc.returncode, 75, proc.stderr)


class CompareUnitTests(unittest.TestCase):
    def test_compare_reports_entry_set_and_digest_differences(self):
        archived = {
            'entries': [{'id': 'gone', 'stage': 'vertex', 'msl_sha256': 'a',
                         'msl_bytes': 1, 'air': {'path': 'out/a.air', 'sha256': 'b',
                                                 'bytes': 2}}],
            'stages': {'vertex': {'path': 'out/vertex.metallib', 'sha256': 'c',
                                  'bytes': 3}},
        }
        local = {
            'entries': [{'id': 'new', 'stage': 'fragment', 'msl_sha256': 'd',
                         'msl_bytes': 4, 'air': {'path': 'out/d.air', 'sha256': 'e',
                                                 'bytes': 5}}],
            'stages': {'fragment': {'path': 'out/fragment.metallib', 'sha256': 'f',
                                    'bytes': 6}},
        }
        problems = m.compare(archived, local)
        self.assertIn('gone: archived entry is missing from the local manifest', problems)
        self.assertIn('new: local entry is not present in the archive', problems)
        self.assertIn('fragment: metallib is not present in the archive', problems)
        self.assertIn('vertex: archived metallib was not relinked', problems)


if __name__ == '__main__':
    unittest.main(verbosity=2)
