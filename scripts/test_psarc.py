#!/usr/bin/env python3
"""Synthetic PSARC fixtures (no game data): stored/zlib blocks, bounds and path rejection."""
import hashlib
import json
import os
import struct
import sys
import tempfile
import unittest
import zlib
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import psarc

BS = 16


def build(members, comp='zlib', manifest_sep=b'\0', tamper=None):
    """members: list of (path, bytes). Block size 16 -> 1-byte block table widths."""
    names = manifest_sep.join(p.encode() for p, _ in members)
    payloads = [names] + [d for _, d in members]
    blocks, body, entries = [], b'', []
    for data in payloads:
        first = len(blocks)
        entries.append((first, len(data), len(body)))
        for i in range(0, len(data), BS):
            chunk = data[i:i + BS]
            z = zlib.compress(chunk)
            stored = chunk if len(z) >= len(chunk) else z      # raw when not smaller
            blocks.append(len(stored) if len(stored) != BS else 0)
            body += stored
    toc = b''
    for first, size, off in entries:
        toc += bytes(16) + struct.pack('>I', first) + size.to_bytes(5, 'big') + off.to_bytes(5, 'big')
    toc += bytes(blocks)
    toc_size = 32 + len(toc)
    entries2 = b''
    for first, size, off in entries:
        entries2 += bytes(16) + struct.pack('>I', first) + size.to_bytes(5, 'big') + (off + toc_size).to_bytes(5, 'big')
    toc = entries2 + bytes(blocks)
    head = b'PSAR' + struct.pack('>HH4sIIIII', 1, 4, comp.encode(), toc_size, 30, len(payloads), BS, 12)
    data = head + toc + body
    return tamper(data) if tamper else data


class Tests(unittest.TestCase):
    def open(self, data):
        d = tempfile.TemporaryDirectory()
        self.addCleanup(d.cleanup)
        p = Path(d.name) / 'a.psarc'
        p.write_bytes(data)
        a = psarc.Psarc(p)
        self.addCleanup(a.close)
        return a

    def test_roundtrip_compressible_and_incompressible(self):
        files = [('a/x.bin', b'A' * 100), ('b/y.bin', bytes(range(40))), ('z', b'')]
        a = self.open(build(files))
        self.assertEqual(a.paths(), ['a/x.bin', 'b/y.bin', 'z'])
        self.assertEqual(a.read(1), b'A' * 100)
        self.assertEqual(a.read(2), bytes(range(40)))
        self.assertEqual(a.read(1, 5), b'AAAAA')

    def test_newline_manifest_accepted(self):
        a = self.open(build([('p/q', b'1'), ('r', b'2')], manifest_sep=b'\n'))
        self.assertEqual(a.paths(), ['p/q', 'r'])

    def test_rejects_traversal_and_absolute(self):
        for bad in ('../x', 'a/../../x', '/etc/passwd'):
            with self.assertRaises(psarc.PsarcError):
                self.open(build([(bad, b'1')])).paths()

    def test_rejects_bad_magic_unknown_and_lzma(self):
        with self.assertRaises(psarc.PsarcError):
            self.open(b'NOPE' + bytes(60))
        with self.assertRaises(psarc.PsarcError):
            self.open(build([('a', b'1')], comp='zzzz'))
        with self.assertRaises(psarc.PsarcError):
            self.open(build([('a', b'1')], comp='lzma'))

    def test_rejects_inconsistent_toc(self):
        data = bytearray(build([('a', b'1')]))
        data[20:24] = struct.pack('>I', 999)          # entry count far beyond TOC
        with self.assertRaises(psarc.PsarcError):
            self.open(bytes(data))

    def test_truncated_archive_rejected(self):
        with self.assertRaises(psarc.PsarcError):
            self.open(build([('a', b'1' * 64)])[:20])

    def test_truncated_compressed_block_rejected(self):
        with self.assertRaisesRegex(psarc.PsarcError, '^truncated block$'):
            self.open(build([('a', b'A' * 64)])[:-1]).read(1)

    def test_sha256_file_reads_multiple_bounded_chunks(self):
        data = bytes(range(251))
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'sample.bin'
            path.write_bytes(data)
            read_sizes = []
            original_open = Path.open

            class ReadSpy:
                def __init__(self, stream):
                    self.stream = stream

                def __enter__(self):
                    self.stream.__enter__()
                    return self

                def __exit__(self, *args):
                    return self.stream.__exit__(*args)

                def fileno(self):
                    return self.stream.fileno()

                def read(self, size=-1):
                    read_sizes.append(size)
                    return self.stream.read(size)

            def traced_open(target, *args, **kwargs):
                stream = original_open(target, *args, **kwargs)
                return ReadSpy(stream) if target == path else stream

            with mock.patch.object(Path, 'open', new=traced_open):
                actual = psarc._sha256_file(path, chunk_size=64)

        self.assertEqual(actual, hashlib.sha256(data).hexdigest())
        self.assertGreater(len(read_sizes), 2)
        self.assertTrue(all(0 < size <= 64 for size in read_sizes))

    def test_sha256_file_rejects_path_replacement_during_hash(self):
        data = b'original archive bytes' * 8
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'sample.bin'
            replacement = Path(tmp) / 'replacement.bin'
            path.write_bytes(data)
            original_open = Path.open

            class ReplacingReader:
                def __init__(self, stream):
                    self.stream = stream
                    self.replaced = False

                def __enter__(self):
                    self.stream.__enter__()
                    return self

                def __exit__(self, *args):
                    return self.stream.__exit__(*args)

                def fileno(self):
                    return self.stream.fileno()

                def read(self, size=-1):
                    chunk = self.stream.read(size)
                    if not self.replaced:
                        with open(replacement, 'wb') as out:
                            out.write(data)
                        os.replace(replacement, path)
                        self.replaced = True
                    return chunk

            def replacing_open(target, *args, **kwargs):
                stream = original_open(target, *args, **kwargs)
                return ReplacingReader(stream) if target == path else stream

            with mock.patch.object(Path, 'open', new=replacing_open):
                with self.assertRaisesRegex(psarc.PsarcError, 'changed while hashing'):
                    psarc._sha256_file(path, chunk_size=16)

    def test_overlay_patch_wins_exact_path(self):
        base = {'a/x.bin': dict(path='a/x.bin', md5='aa', size=1),
                'only/base': dict(path='only/base', md5='bb', size=2)}
        patch = {'a/x.bin': dict(path='a/x.bin', md5='cc', size=3),
                 'only/patch': dict(path='only/patch', md5='dd', size=4)}
        ov = psarc.build_overlay([('base', base), ('patch', patch)])
        self.assertEqual(ov['policy']['member_path_match'], psarc.MEMBER_PATH_MATCH)
        self.assertEqual(ov['policy']['collision_key'], psarc.COLLISION_KEY)
        self.assertEqual(ov['policy']['mount_order'], ['base', 'patch'])
        self.assertEqual(ov['members'], 3)
        self.assertEqual(ov['overrides'], 1)
        self.assertEqual(psarc.resolve_path(ov, 'a/x.bin')['md5'], 'cc')
        self.assertEqual(psarc.resolve_path(ov, 'a/x.bin')['source_layer'], 'patch')
        self.assertEqual(psarc.resolve_path(ov, 'only/base')['source_layer'], 'base')
        self.assertEqual(psarc.resolve_path(ov, 'only/patch')['source_layer'], 'patch')
        self.assertIsNone(psarc.resolve_path(ov, 'A/x.bin'))  # exact only

    def test_overlay_rejects_casefold_collision(self):
        base = {'Foo': dict(path='Foo', md5='1', size=1)}
        patch = {'foo': dict(path='foo', md5='2', size=1)}
        with self.assertRaisesRegex(psarc.PsarcError, 'casefold collision'):
            psarc.build_overlay([('base', base), ('patch', patch)])

    def test_archive_basename_policy_is_exact(self):
        self.assertEqual(psarc.archive_basename_key('cachedFiles.psarc'), 'cachedFiles.psarc')
        self.assertEqual(psarc.archive_basename_key('cachedfiles.psarc'), 'cachedfiles.psarc')
        self.assertNotEqual(
            psarc.archive_basename_key('cachedFiles.psarc'),
            psarc.archive_basename_key('cachedfiles.psarc'))
        self.assertEqual(psarc.ARCHIVE_BASENAME_MATCH, 'exact')
        self.assertEqual(psarc.CACHEDFILES_MOUNT_ORDER, ('base', 'patch'))

    def test_overlay_from_member_jsonls_roundtrip(self):
        d = tempfile.TemporaryDirectory()
        self.addCleanup(d.cleanup)
        base_p = Path(d.name) / 'base.jsonl'
        patch_p = Path(d.name) / 'patch.jsonl'
        base_p.write_text(json.dumps(dict(id=1, path='shared', md5='1', size=1)) + '\n')
        patch_p.write_text(
            json.dumps(dict(id=1, path='shared', md5='2', size=2)) + '\n' +
            json.dumps(dict(id=2, path='new', md5='3', size=3)) + '\n')
        ov = psarc.overlay_from_member_jsonls([('base', base_p), ('patch', patch_p)])
        out = Path(d.name) / 'overlay.json'
        report = psarc.write_overlay_report(ov, out, extra=dict(
            note='fixture', patch_only=['new']))
        loaded = json.loads(out.read_text())
        self.assertEqual(loaded['overrides'], 1)
        self.assertEqual(loaded['sources']['patch'], 2)
        self.assertEqual(loaded['source_by_path']['shared'], 'patch')
        self.assertEqual(report['patch_only'], ['new'])


if __name__ == '__main__':
    unittest.main()
