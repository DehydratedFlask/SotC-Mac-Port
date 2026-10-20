#!/usr/bin/env python3
"""Unit tests for AUD-01 audio header classification (no live archives required)."""
from __future__ import annotations

import hashlib
import sys
import unittest
import zlib
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import classify_audio_headers as cah


class FamilyTests(unittest.TestCase):
    def test_fbad_family(self):
        self.assertEqual(cah.family_of(bytes.fromhex('fbad989755020700')), 'sxd-fbad9897')

    def test_xff_family(self):
        self.assertEqual(cah.family_of(b'xff\x00\x00\x00\x00\x00'), 'hd-bd-ascii-xff')

    def test_qath_family(self):
        self.assertEqual(cah.family_of(b'qath' + b'\x00' * 12), 'bd-ascii-qath')

    def test_8c0a_family(self):
        self.assertEqual(cah.family_of(bytes.fromhex('8c0a0033e58833e3')), 'sxd-high-entropy-8c0x')
        self.assertEqual(cah.family_of(bytes.fromhex('8c060008c98808c7')), 'sxd-high-entropy-8c0x')

    def test_80ae_family(self):
        self.assertEqual(cah.family_of(b'80AE' + b'\x00' * 4), 'sxd-ascii-80AE')

    def test_le_small_ints(self):
        self.assertEqual(
            cah.family_of(bytes.fromhex('0100000000000000140d0000')),
            'sxd-le-small-ints',
        )

    def test_le_small_ints_short_prefix(self):
        self.assertEqual(
            cah.family_of(bytes.fromhex("01000000000000")),
            'other-01000000',
        )

    def test_near_empty(self):
        self.assertEqual(
            cah.family_of(bytes.fromhex('3ebe7b0000000000') + b'\x00' * 24),
            'hd-bd-near-empty-3ebe7b',
        )


class EntropyAndProbeTests(unittest.TestCase):
    def test_shannon_bounds(self):
        self.assertEqual(cah.shannon(b''), 0.0)
        self.assertEqual(cah.shannon(b'\x00' * 100), 0.0)
        self.assertGreater(cah.shannon(bytes(range(256))), 7.9)

    def test_trivial_inflate_rejected(self):
        # Near-empty stub previously produced a 1-byte raw_inflate "success".
        data = bytes.fromhex('3ebe7b0000000000') + b'\x00' * 64
        probe = cah.decompress_probe(data)
        self.assertFalse(probe['raw_inflate_offset2']['ok'])
        self.assertTrue(probe['raw_inflate_offset2'].get('rejected_trivial')
                        or probe['raw_inflate_offset2'].get('err'))

    def test_real_zlib_accepted(self):
        payload = b'A' * 64
        wrapped = zlib.compress(payload)
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['zlib_offset0']['ok'])
        self.assertEqual(probe['zlib_offset0']['out_len'], 64)


class ClassifyFixtureTests(unittest.TestCase):
    def test_codec_always_unknown_on_empty_set(self):
        payload = cah.classify_archives([])
        self.assertEqual(payload['member_count'], 0)
        self.assertEqual(payload['classes'], [])
        self.assertIn('codec intentionally UNKNOWN', ' '.join(payload['limitations']))

    def test_full_member_hash_and_psarc_entry_locator(self):
        member = bytes(range(256)) * 20 + b'tail'

        class SyntheticPsarc:
            def __init__(self, _path):
                self.entries = [
                    {'size': 0, 'offset': 0, 'block': 0},
                    {'size': len(member), 'offset': 0x1234, 'block': 17},
                ]
                self.read_calls = []
                self.closed = False

            def paths(self):
                return ['sound/fixture.sxd']

            def read(self, index, limit=None):
                self.read_calls.append((index, limit))
                return member if limit is None else member[:limit]

            def close(self):
                self.closed = True

        instances = []

        def open_synthetic(path):
            archive = SyntheticPsarc(path)
            instances.append(archive)
            return archive

        with patch.object(cah, 'Psarc', side_effect=open_synthetic):
            payload = cah.classify_archives([Path('synthetic.psarc')])

        self.assertEqual(payload['member_count'], 1)
        row = payload['members'][0]
        self.assertEqual(row['member_sha256'], hashlib.sha256(member).hexdigest())
        self.assertNotEqual(row['member_sha256'], hashlib.sha256(member[:cah.SAMPLE]).hexdigest())
        self.assertEqual(row['entry_id'], 1)
        self.assertEqual(row['entry_offset'], 0x1234)
        self.assertEqual(row['first_block'], 17)
        self.assertEqual(row['size'], len(member))
        self.assertEqual(row['sample_bytes'], cah.SAMPLE)
        self.assertEqual(instances[0].read_calls, [(1, None)])
        self.assertTrue(instances[0].closed)
        self.assertEqual(row['codec'], 'UNKNOWN')


if __name__ == '__main__':
    unittest.main()
