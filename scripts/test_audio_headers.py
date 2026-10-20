#!/usr/bin/env python3
"""Unit tests for AUD-01 audio header classification (no live archives required)."""
from __future__ import annotations

import sys
import unittest
import zlib
from pathlib import Path

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


if __name__ == '__main__':
    unittest.main()
