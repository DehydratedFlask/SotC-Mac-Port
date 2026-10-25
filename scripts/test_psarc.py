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


def build(members, comp='zlib', manifest_sep=b'\0', tamper=None, manifest_paths=None,
          block_size=BS, sort_by_md5=False, md5_override=None):
    """members: list of (path, bytes). Block-table width follows block_size.

    manifest_paths overrides the manifest text only (payload list stays `members`), so a
    structurally valid archive can declare a manifest path count that differs from count - 1.
    Each member entry carries its real TOC value MD5(path); entry 0 (manifest) is all zero.
    sort_by_md5 reorders entries 1.. by that value, like real archives (manifest text order is
    whatever `members`/`manifest_paths` give). md5_override maps member index (0-based) to a
    16-byte value to forge missing/duplicate TOC values.
    """
    manifest_names = [p for p, _ in members] if manifest_paths is None else list(manifest_paths)
    names = manifest_sep.join(p.encode() for p in manifest_names)
    payloads = [names] + [d for _, d in members]
    digests = [bytes(16)] + [
        (md5_override or {}).get(i) or hashlib.md5(p.encode()).digest()
        for i, (p, _) in enumerate(members)]
    blocks, body, entries = [], b'', []
    for data, digest in zip(payloads, digests):
        first = len(blocks)
        entries.append((digest, first, len(data), len(body)))
        for i in range(0, len(data), block_size):
            chunk = data[i:i + block_size]
            z = zlib.compress(chunk)
            stored = chunk if len(z) >= len(chunk) else z      # raw when not smaller
            blocks.append(len(stored) if len(stored) != block_size else 0)
            body += stored
    width = max(1, ((block_size - 1).bit_length() + 7) // 8)
    block_table = b''.join(value.to_bytes(width, 'big') for value in blocks)
    if sort_by_md5:
        entries = entries[:1] + sorted(entries[1:], key=lambda e: e[0])
    toc = b''
    for digest, first, size, off in entries:
        toc += digest + struct.pack('>I', first) + size.to_bytes(5, 'big') + off.to_bytes(5, 'big')
    toc += block_table
    toc_size = 32 + len(toc)
    entries2 = b''
    for digest, first, size, off in entries:
        entries2 += digest + struct.pack('>I', first) + size.to_bytes(5, 'big') + (off + toc_size).to_bytes(5, 'big')
    toc = entries2 + block_table
    head = b'PSAR' + struct.pack('>HH4sIIIII', 1, 4, comp.encode(), toc_size, 30, len(payloads), block_size, 12)
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

    def write_jsonl(self, lines):
        d = tempfile.TemporaryDirectory()
        self.addCleanup(d.cleanup)
        p = Path(d.name) / 'members.jsonl'
        p.write_text('\n'.join(lines) + '\n')
        return p

    # --- DATA-02-PSARC-PAIRING-FIX-01: pair manifest paths to TOC entries by MD5(path) ---------

    PAIRING_FILES = [(f'dir/{name}.bin', f'payload-{name}-'.encode() * 3)
                     for name in ('alpha', 'bravo', 'charlie', 'delta', 'echo', 'foxtrot',
                                  'golf', 'hotel', 'india', 'juliet')]

    def test_path_md5_is_md5_of_utf8_path(self):
        self.assertEqual(psarc.path_md5('a/b'), hashlib.md5(b'a/b').hexdigest())
        self.assertEqual(psarc.path_md5('\u00e9'), hashlib.md5('\u00e9'.encode('utf-8')).hexdigest())
        with self.assertRaises(psarc.PsarcError):
            psarc.path_md5(b'a/b')

    def test_members_preserves_exact_utf8_path_spelling(self):
        # Hash the manifest spelling: no casefold, Unicode normalization, slash
        # collapse, dot-segment removal, or whitespace stripping.
        paths = ['Dir/a', 'dir/a', 'caf\u00e9', 'cafe\u0301', 'a/./b', 'a//b',
                 ' padded ', 'line\nbreak']
        files = [(path, f'payload-{i}'.encode()) for i, path in enumerate(paths)]
        archive = self.open(build(files, sort_by_md5=True))
        self.assertEqual([path for path, _ in archive.members()], paths)
        for path, data in files:
            entry_id = archive.entry_index(path)
            self.assertEqual(archive.entries[entry_id]['md5'],
                             hashlib.md5(path.encode('utf-8')).hexdigest())
            self.assertEqual(archive.read(entry_id), data)
        with self.assertRaisesRegex(psarc.PsarcError, r'^no such member path '):
            archive.entry_index('a/b')

    def test_members_rejects_distinct_paths_with_ambiguous_hash(self):
        archive = self.open(build([('a', b'1'), ('b', b'2')]))
        with mock.patch.object(psarc, 'path_md5', return_value=hashlib.md5(b'a').hexdigest()):
            with self.assertRaisesRegex(psarc.PsarcError, r'^entry 1 claimed by multiple'):
                archive.members()

    def test_members_cache_is_not_mutable_through_returned_list(self):
        archive = self.open(build([('a', b'1')]))
        archive.members().clear()
        self.assertEqual(archive.members(), [('a', 1)])
        self.assertEqual(archive.entry_index('a'), 1)

    def test_entry_index_rejects_non_string_path(self):
        archive = self.open(build([('a', b'1')]))
        for path in (None, b'a', ['a']):
            with self.subTest(path=path):
                with self.assertRaisesRegex(psarc.PsarcError, '^member path must be str$'):
                    archive.entry_index(path)

    def test_members_pairs_by_md5_when_toc_is_md5_sorted_and_manifest_alphabetical(self):
        files = sorted(self.PAIRING_FILES)                       # alphabetical manifest
        a = self.open(build(files, sort_by_md5=True))
        self.assertEqual(a.paths(), [p for p, _ in files])
        toc = [e['md5'] for e in a.entries[1:]]
        self.assertEqual(toc, sorted(toc))                       # fixture really is MD5 ordered
        payload = dict(files)
        positional_wrong = 0
        for k, (path, entry_id) in enumerate(a.members()):
            self.assertEqual(a.entries[entry_id]['md5'], psarc.path_md5(path))
            self.assertEqual(a.read(entry_id), payload[path])    # corrected pairing
            self.assertEqual(a.entry_index(path), entry_id)
            if entry_id != k + 1:
                positional_wrong += 1
                self.assertNotEqual(a.read(k + 1), payload[path])  # old positional pairing is wrong
        self.assertGreaterEqual(positional_wrong, len(files) - 2)  # old pairing failed ~everywhere

    def test_members_works_when_toc_order_equals_manifest_order(self):
        files = [('a', b'1'), ('b', b'22')]
        a = self.open(build(files))
        self.assertEqual(a.members(), [('a', 1), ('b', 2)])

    def test_summary_and_members_jsonl_use_md5_pairing(self):
        files = sorted(self.PAIRING_FILES)
        payload = dict(files)
        with tempfile.TemporaryDirectory() as tmp:
            arch = Path(tmp) / 'pk' / 'a.psarc'
            arch.parent.mkdir()
            arch.write_bytes(build(files, sort_by_md5=True))
            out = Path(tmp) / 'out'
            with mock.patch.object(sys, 'argv', ['psarc.py', str(arch), '--out', str(out)]), \
                    mock.patch('builtins.print'):
                psarc.main()
            rows = [json.loads(line) for line in
                    (out / 'pk-a.members.jsonl').read_text().splitlines()]
            summary = json.loads((out / 'pk-a.summary.json').read_text())
        self.assertEqual([r['path'] for r in rows], [p for p, _ in files])
        a = self.open(build(files, sort_by_md5=True))
        for r in rows:
            self.assertEqual(r['md5'], psarc.path_md5(r['path']))
            self.assertEqual(r['size'], len(payload[r['path']]))
            self.assertEqual(a.read(r['id']), payload[r['path']])
        self.assertEqual(summary['members'], len(files))
        self.assertEqual(summary['uncompressed_total'], sum(len(d) for _, d in files))

    def test_members_rejects_missing_toc_value(self):
        a = self.open(build([('a', b'1'), ('b', b'2')], md5_override={1: bytes([7]) * 16}))
        with self.assertRaisesRegex(psarc.PsarcError, r"^no TOC entry for MD5\('b'\)$"):
            a.members()

    def test_members_rejects_duplicate_toc_value(self):
        a = self.open(build([('a', b'1'), ('b', b'2')],
                            md5_override={1: hashlib.md5(b'a').digest()}))
        with self.assertRaisesRegex(psarc.PsarcError, r'^duplicate TOC value '):
            a.members()

    def test_members_rejects_duplicate_manifest_path(self):
        a = self.open(build([('a', b'1'), ('a', b'2')]))
        with self.assertRaisesRegex(psarc.PsarcError, r'^duplicate TOC value '):
            a.members()
        b = self.open(build([('a', b'1'), ('b', b'2')], manifest_paths=['a', 'a']))
        with self.assertRaisesRegex(psarc.PsarcError, r"^entry 1 claimed by multiple"):
            b.members()

    def test_entry_index_unknown_path(self):
        a = self.open(build([('a', b'1')]))
        with self.assertRaisesRegex(psarc.PsarcError, r"^no such member path 'A'$"):
            a.entry_index('A')                                   # exact, case-sensitive

    def test_roundtrip_compressible_and_incompressible(self):
        files = [('a/x.bin', b'A' * 100), ('b/y.bin', bytes(range(40))), ('z', b'')]
        a = self.open(build(files))
        self.assertEqual(a.paths(), ['a/x.bin', 'b/y.bin', 'z'])
        self.assertEqual(a.read(1), b'A' * 100)
        self.assertEqual(a.read(2), bytes(range(40)))
        self.assertEqual(a.read(3), b'')
        self.assertEqual(a.read(1, 5), b'AAAAA')

    def test_block_table_values_use_width_from_block_size(self):
        payload = bytes(range(256))
        a = self.open(build([('a', payload)], block_size=257))
        self.assertEqual(a.width, 2)
        self.assertEqual(a.blocks[a.entries[1]['block']], 256)
        self.assertEqual(a.read(1), payload)

    def test_rejects_block_table_with_partial_width_entry(self):
        data = bytearray(build([('a', bytes(range(256)))], block_size=257))
        toc_size = int.from_bytes(data[12:16], 'big')
        struct.pack_into('>I', data, 12, toc_size + 1)

        with self.assertRaisesRegex(
            psarc.PsarcError, '^block table not a multiple of width$'
        ):
            self.open(bytes(data))

    def test_rejects_block_size_outside_supported_range(self):
        valid = build([('a', b'1')])
        for block_size in (0, (1 << 24) + 1):
            with self.subTest(block_size=block_size):
                data = bytearray(valid)
                struct.pack_into('>I', data, 24, block_size)
                with self.assertRaises(psarc.PsarcError) as error:
                    self.open(bytes(data))
                self.assertEqual(str(error.exception), 'unsupported entry/block size')

    def test_rejects_unsupported_entry_size(self):
        valid = build([('a', b'1')])
        for entry_size in (29, 31):
            with self.subTest(entry_size=entry_size):
                data = bytearray(valid)
                struct.pack_into('>I', data, 16, entry_size)
                with self.assertRaises(psarc.PsarcError) as error:
                    self.open(bytes(data))
                self.assertEqual(str(error.exception), 'unsupported entry/block size')

    def test_newline_manifest_accepted(self):
        a = self.open(build([('p/q', b'1'), ('r', b'2')], manifest_sep=b'\n'))
        self.assertEqual(a.paths(), ['p/q', 'r'])

    def test_nul_manifest_preserves_newline_in_member_path(self):
        member_paths = ['line\nbreak.bin', 'other.bin']
        members = [(member_paths[0], b'1'), (member_paths[1], b'2')]
        archive = self.open(build(members, manifest_sep=b'\0'))
        self.assertIn(b'\0', archive.read(0))
        self.assertEqual(archive.paths(), member_paths)

    def test_nul_manifest_accepts_one_terminal_nul(self):
        archive = self.open(build([('one.bin', b'1')], manifest_paths=['one.bin', '']))
        self.assertEqual(archive.read(0), b'one.bin\0')
        self.assertEqual(archive.paths(), ['one.bin'])

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

    def test_rejects_toc_size_beyond_archive_file(self):
        data = bytearray(build([('a', b'1')]))
        struct.pack_into('>I', data, 12, len(data) + 1)

        with self.assertRaises(psarc.PsarcError) as error:
            self.open(bytes(data))
        self.assertEqual(
            str(error.exception),
            'TOC size inconsistent with entry count/file size',
        )

    def test_rejects_first_block_outside_block_table(self):
        data = bytearray(build([('a', b'1')]))
        struct.pack_into('>I', data, 32 + 30 + 16, 0xffffffff)

        with self.assertRaises(psarc.PsarcError) as error:
            self.open(bytes(data))
        self.assertEqual(str(error.exception), 'entry references blocks outside the block table')

    def test_rejects_zero_entry_archive_without_manifest(self):
        data = bytearray(build([('a', b'1')]))
        struct.pack_into('>I', data, 12, 32)          # toc_size: header only
        struct.pack_into('>I', data, 20, 0)           # entry_count 0 -> no manifest entry
        with self.assertRaises(psarc.PsarcError) as error:
            self.open(bytes(data)).paths()
        self.assertEqual(str(error.exception), 'archive has no manifest entry')

    def test_rejects_entry_offset_beyond_archive(self):
        data = bytearray(build([('a', b'1'), ('b', b'2')]))
        self.assertEqual(self.open(bytes(data)).paths(), ['a', 'b'])   # baseline is valid
        # Entry 1 starts at 32 + 30 and its 5-byte offset field is at +25.
        data[32 + 30 + 25:32 + 30 + 30] = (len(data) + 1).to_bytes(5, 'big')

        with self.assertRaises(psarc.PsarcError) as error:
            self.open(bytes(data))
        self.assertEqual(str(error.exception), 'entry 1 offset beyond archive')

    def test_rejects_manifest_path_count_mismatch(self):
        cases = (
            ('one-too-many', ['a', 'b'], [('a', b'1')], 'manifest has 2 paths for 1 members'),
            ('one-too-few', ['a'], [('a', b'1'), ('b', b'2')], 'manifest has 1 paths for 2 members'),
        )
        for label, manifest_paths, members, message in cases:
            with self.subTest(case=label):
                a = self.open(build(members, manifest_paths=manifest_paths))
                with self.assertRaises(psarc.PsarcError) as error:
                    a.paths()
                self.assertEqual(str(error.exception), message)

    def test_be_bounds(self):
        self.assertEqual(psarc.be(b'\x01\x02', 0, 2), 0x0102)
        self.assertEqual(psarc.be(b'\x01\x02', 1, 1), 0x02)          # ends exactly at len
        self.assertEqual(psarc.be(b'\x01\x02', 2, 0), 0)             # n == 0 at the end
        self.assertEqual(psarc.be(b'', 0, 0), 0)
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.be(b'\x01\x02', 1, 2)                              # one byte past the end
        self.assertEqual(str(error.exception), 'out of bounds: 0x1+2')
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.be(b'\x01\x02', -1, 1)                             # negative position
        self.assertEqual(str(error.exception), 'out of bounds: -0x1+1')
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.be(b'\x01\x02', 3, 0)                              # zero length past the end
        self.assertEqual(str(error.exception), 'out of bounds: 0x3+0')

    def test_read_limit_parameter(self):
        stored_payload = bytes(range(16))                # zlib expands it -> stored raw block
        compressed_payload = b'A' * 48                   # zlib shrinks it -> compressed blocks
        stored = self.open(build([('s', stored_payload)]))
        compressed = self.open(build([('c', compressed_payload)]))
        self.assertEqual(stored.blocks[stored.entries[1]['block']], 0)
        self.assertNotEqual(compressed.blocks[compressed.entries[1]['block']], 0)

        for a, full in ((stored, stored_payload), (compressed, compressed_payload)):
            with self.subTest(kind='stored' if a is stored else 'compressed'):
                self.assertEqual(a.read(1, 0), b'')                      # limit 0
                self.assertEqual(a.read(1, 5), full[:5])                 # partial
                self.assertEqual(a.read(1, len(full)), full)             # limit == size
                self.assertEqual(a.read(1, len(full) + 99), full)        # clamped to size
                self.assertEqual(a.read(1, len(full)), a.read(1))        # equals full read
        self.assertEqual(compressed.read(1, 20), b'A' * 20)              # crosses a block

    def test_read_rejects_negative_limit(self):
        archive = self.open(build([('c', b'A' * 48)]))
        for limit in (-1, -2, -99):
            with self.subTest(limit=limit):
                with self.assertRaises(psarc.PsarcError) as error:
                    archive.read(1, limit)
                self.assertEqual(str(error.exception), f'invalid negative read limit: {limit}')
        self.assertEqual(archive.read(1, None), b'A' * 48)               # None stays whole-member
        self.assertEqual(archive.read(1, 0), b'')                        # 0 stays valid

    def test_prefix_read_ignores_truncated_later_block(self):
        archive = self.open(build([('c', b'A' * 48)])[:-1])
        self.assertEqual(archive.read(1, 5), b'A' * 5)
        with self.assertRaises(psarc.PsarcError) as error:
            archive.read(1)
        self.assertEqual(str(error.exception), 'truncated block')

    def test_mixed_compressed_and_stored_blocks_in_one_member(self):
        payload = b'A' * 16 + bytes(range(16))
        archive = self.open(build([('mixed.bin', payload)], block_size=16))
        first_block = archive.entries[1]['block']

        self.assertGreater(archive.blocks[first_block], 0)
        self.assertEqual(archive.blocks[first_block + 1], 0)
        self.assertEqual(archive.read(1), payload)

    def test_compressed_block_decoded_length_mismatch_rejected(self):
        def declare_one_byte_less(data):
            data = bytearray(data)
            member_entry_offset = 32 + 30             # entry 1 follows the manifest entry
            size_offset = member_entry_offset + 20    # 16-byte digest + u32 block + 5-byte size
            declared = int.from_bytes(data[size_offset:size_offset + 5], 'big')
            data[size_offset:size_offset + 5] = (declared - 1).to_bytes(5, 'big')
            return bytes(data)

        archive = self.open(
            build([('c', b'A' * 64)], tamper=declare_one_byte_less)
        )
        with self.assertRaisesRegex(
            psarc.PsarcError,
            '^block 4 decoded to 16 bytes, expected 15$',
        ):
            archive.read(1)

    def test_overlay_requires_a_layer(self):
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.build_overlay([])
        self.assertEqual(str(error.exception), 'overlay requires at least one layer')

    def test_load_members_jsonl_rejects_duplicate_exact_path(self):
        p = self.write_jsonl([json.dumps(dict(id=1, path='same', md5='1', size=1)),
                              json.dumps(dict(id=2, path='same', md5='2', size=2))])
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.load_members_jsonl(p)
        self.assertEqual(str(error.exception), f'duplicate exact path in {p}: \'same\'')

    def test_overlay_single_layer_stats_and_resolve(self):
        ov = psarc.build_overlay([('base', {'only/one': dict(path='only/one', md5='1', size=1)})])
        self.assertEqual(ov['members'], 1)
        self.assertEqual(ov['added'], 1)
        self.assertEqual(ov['overrides'], 0)
        self.assertEqual(dict(ov['sources']), {'base': 1})
        self.assertEqual(psarc.resolve_path(ov, 'only/one'),
                         dict(path='only/one', md5='1', size=1, source_layer='base'))
        self.assertIsNone(psarc.resolve_path(ov, 'ONLY/one'))
        self.assertIsNone(psarc.resolve_path(ov, 'only/on'))

    def test_truncated_archive_rejected(self):
        with self.assertRaises(psarc.PsarcError):
            self.open(build([('a', b'1' * 64)])[:20])

    def test_truncated_compressed_block_rejected(self):
        with self.assertRaisesRegex(psarc.PsarcError, '^truncated block$'):
            self.open(build([('a', b'A' * 64)])[:-1]).read(1)

    def test_truncated_stored_block_rejected(self):
        with self.assertRaisesRegex(
            psarc.PsarcError, '^block 1 decoded to 15 bytes, expected 16$'
        ):
            self.open(build([('a', bytes(range(16)))])[:-1]).read(1)

    def test_corrupt_zlib_block_rejected(self):
        def corrupt_last_block(data):
            data = bytearray(data)
            data[-1] ^= 0xFF                      # last byte = adler32 tail of final zlib block
            return bytes(data)
        a = self.open(build([('a', b'A' * 64)], tamper=corrupt_last_block))
        with self.assertRaisesRegex(psarc.PsarcError, '^block 4 has corrupt zlib data$'):
            a.read(1)

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

    def test_load_members_jsonl_rejects_non_object_record(self):
        for text, kind in (('[]', 'list'), ('"x"', 'str'), ('5', 'int')):
            with self.subTest(value=text):
                p = self.write_jsonl([json.dumps(dict(id=1, path='ok', md5='1', size=1)), text])
                with self.assertRaises(psarc.PsarcError) as error:
                    psarc.load_members_jsonl(p)
                self.assertEqual(str(error.exception),
                                 f'{p}:2: member record must be a JSON object, got {kind}')

    def test_load_members_jsonl_rejects_record_without_path(self):
        p = self.write_jsonl([json.dumps(dict(id=1, md5='1', size=1))])
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.load_members_jsonl(p)
        self.assertEqual(str(error.exception), f'{p}:1: member record has no "path"')

    def test_load_members_jsonl_rejects_non_string_path(self):
        p = self.write_jsonl([json.dumps(dict(id=1, path='ok', md5='1', size=1)),
                              json.dumps(dict(id=2, path=5, md5='2', size=2))])
        with self.assertRaises(psarc.PsarcError) as error:
            psarc.load_members_jsonl(p)
        self.assertEqual(str(error.exception), f'{p}:2: member path must be a string, got int')

    def test_load_members_jsonl_rejects_malformed_json_with_file_and_line(self):
        cases = (
            (['{"path":'], 1),
            ([json.dumps(dict(id=1, path='ok', md5='1', size=1)), '{"path":'], 2),
        )
        for lines, lineno in cases:
            with self.subTest(line=lineno):
                p = self.write_jsonl(lines)
                with self.assertRaises(psarc.PsarcError) as error:
                    psarc.load_members_jsonl(p)
                self.assertEqual(str(error.exception),
                                 f'{p}:{lineno}: malformed JSONL record')

    def test_load_members_jsonl_blank_line_still_raises_json_decode_error(self):
        p = self.write_jsonl([''])
        with self.assertRaises(json.JSONDecodeError):
            psarc.load_members_jsonl(p)

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
