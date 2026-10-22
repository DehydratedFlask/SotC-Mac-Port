#!/usr/bin/env python3
"""Unit tests for AUD-01 audio header classification (no live archives required)."""
from __future__ import annotations

import hashlib
import lzma
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


class FamilyBoundaryTests(unittest.TestCase):
    """Prefix-anchoring boundaries: empty, too short, one byte short, two families.

    Every fixed signature is matched at offset 0 only (``startswith``/``head[:2]``);
    the classifier never scans for a signature at a later offset. The fixed
    signatures also have distinct first bytes, so no buffer can satisfy two of them
    at offset 0 and the branch order in ``family_of`` is defensive, not observable.
    """

    def test_empty_buffer_is_the_empty_family(self):
        self.assertEqual(cah.family_of(b''), 'empty')
        # An empty bytes-like value is still "empty", never the hex fallback.
        self.assertEqual(cah.family_of(bytearray()), 'empty')
        self.assertEqual(cah.family_of(memoryview(b'')), 'empty')

    def test_buffers_shorter_than_every_signature_fall_back_to_hex(self):
        # No observed signature is shorter than two bytes, and the 2-byte 8c0x
        # family needs both bytes, so single truncated lead bytes are unmatched.
        self.assertEqual(cah.family_of(b'\xfb'), 'other-fb')
        self.assertEqual(cah.family_of(b'\x8c'), 'other-8c')
        self.assertEqual(cah.family_of(b'\x8c\x00'), 'other-8c00')
        # 3-byte ASCII fragments of the 4-byte qath / 80AE signatures.
        self.assertEqual(cah.family_of(b'qat'), 'other-716174')
        self.assertEqual(cah.family_of(b'80A'), 'other-383041')
        # A 3-byte fragment of the 4-byte fbad9897 signature is not that family.
        self.assertEqual(cah.family_of(b'\xfb\xad\x98'), 'other-fbad98')

    def test_xff_prefix_one_byte_short_is_not_the_xff_family(self):
        # b'xff\x00' is the complete 4-byte signature; b'xff' is one byte short.
        self.assertEqual(cah.family_of(b'xff\x00'), 'hd-bd-ascii-xff')
        self.assertEqual(cah.family_of(b'xff'), 'other-786666')
        # The ASCII signature compare is case-sensitive (upper-case is not xff).
        self.assertEqual(cah.family_of(b'xFF\x00'), 'other-78464600')

    def test_offset_zero_family_wins_over_a_later_family_signature(self):
        # Second-family signature at a nonzero offset is ignored, not scanned for.
        qath_at_2 = b'\x8c\x0aqath'
        self.assertEqual(qath_at_2.index(b'qath'), 2)
        self.assertEqual(cah.family_of(qath_at_2), 'sxd-high-entropy-8c0x')

        xff_at_4 = bytes.fromhex('fbad9897') + b'xff\x00'
        self.assertEqual(xff_at_4.index(b'xff\x00'), 4)
        self.assertEqual(cah.family_of(xff_at_4), 'sxd-fbad9897')

        qath_at_4 = b'80AEqath'
        self.assertEqual(qath_at_4.index(b'qath'), 4)
        self.assertEqual(cah.family_of(qath_at_4), 'sxd-ascii-80AE')

    def test_non_bytes_input_raises_descriptive_type_error(self):
        # Before this guard existed, None and '' were silently labelled 'empty'
        # and other non-bytes leaked AttributeError from .startswith.
        for value, type_name in (
            (None, 'NoneType'),
            ('', 'str'),
            ('qath', 'str'),
            (5, 'int'),
            (['x'], 'list'),
            (True, 'bool'),
        ):
            with self.subTest(value=value):
                with self.assertRaises(TypeError) as ctx:
                    cah.family_of(value)
                self.assertEqual(
                    str(ctx.exception),
                    f'family_of expects bytes-like input, got {type_name}',
                )

    def test_bytes_like_inputs_classify_identically(self):
        raw = b'\x00qath' + b'\x00' * 12
        windowed = memoryview(raw)[1:]
        self.assertEqual(cah.family_of(bytes(raw[1:])), 'bd-ascii-qath')
        self.assertEqual(cah.family_of(bytearray(raw[1:])), 'bd-ascii-qath')
        self.assertEqual(cah.family_of(memoryview(raw[1:])), 'bd-ascii-qath')
        self.assertEqual(cah.family_of(windowed), 'bd-ascii-qath')


class EntropyAndProbeTests(unittest.TestCase):
    def test_shannon_bounds(self):
        self.assertEqual(cah.shannon(b''), 0.0)
        self.assertEqual(cah.shannon(b'\x00' * 100), 0.0)
        self.assertGreater(cah.shannon(bytes(range(256))), 7.9)

    def test_empty_probe_input_reports_too_short_for_every_offset(self):
        expected = {
            'zlib_offset0': {'ok': False, 'err': 'too_short'},
            'zlib_offset2': {'ok': False, 'err': 'too_short'},
            'raw_inflate_offset0': {'ok': False, 'err': 'too_short'},
            'raw_inflate_offset2': {'ok': False, 'err': 'too_short'},
            'lzma_offset0': {'ok': False, 'err': 'too_short'},
            'lzma_offset2': {'ok': False, 'err': 'too_short'},
            'lzma_offset4': {'ok': False, 'err': 'too_short'},
        }
        self.assertEqual(cah.decompress_probe(b''), expected)

    def test_probe_offsets_equal_to_input_length_report_too_short(self):
        two_byte = cah.decompress_probe(b'\x00\x01')
        for name in ('zlib_offset2', 'raw_inflate_offset2', 'lzma_offset2'):
            with self.subTest(name=name):
                self.assertEqual(two_byte[name], {'ok': False, 'err': 'too_short'})

        four_byte = cah.decompress_probe(b'\x00\x01\x02\x03')
        self.assertEqual(four_byte['lzma_offset4'], {'ok': False, 'err': 'too_short'})

    def test_trivial_inflate_rejected(self):
        # Near-empty stub previously produced a 1-byte raw_inflate "success".
        data = bytes.fromhex('3ebe7b0000000000') + b'\x00' * 64
        probe = cah.decompress_probe(data)
        self.assertFalse(probe['raw_inflate_offset2']['ok'])
        self.assertTrue(probe['raw_inflate_offset2'].get('rejected_trivial')
                        or probe['raw_inflate_offset2'].get('err'))

    def test_trivial_lzma_probe_output_rejected(self):
        payload = b'X'
        probe = cah.decompress_probe(lzma.compress(payload))
        self.assertEqual(probe['lzma_offset0'], {
            'ok': False,
            'out_len': 1,
            'rejected_trivial': True,
        })

    def test_trivial_zlib_probe_output_rejected(self):
        probe = cah.decompress_probe(zlib.compress(b'X'))
        self.assertEqual(probe['zlib_offset0'], {
            'ok': False,
            'out_len': 1,
            'rejected_trivial': True,
        })

    def test_real_zlib_accepted(self):
        payload = b'A' * 64
        wrapped = zlib.compress(payload)
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['zlib_offset0']['ok'])
        self.assertEqual(probe['zlib_offset0']['out_len'], 64)

    def test_real_zlib_accepted_after_two_byte_prefix(self):
        payload = b'B' * 64
        wrapped = b'\x00\x01' + zlib.compress(payload)
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['zlib_offset2']['ok'])
        self.assertEqual(probe['zlib_offset2']['out_len'], len(payload))

    def test_raw_deflate_accepted_at_offset_zero(self):
        payload = b'C' * 64
        compressor = zlib.compressobj(wbits=-15)
        wrapped = compressor.compress(payload) + compressor.flush()
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['raw_inflate_offset0']['ok'])
        self.assertEqual(probe['raw_inflate_offset0']['out_len'], len(payload))

    def test_raw_deflate_accepted_after_two_byte_prefix(self):
        payload = b'D' * 64
        compressor = zlib.compressobj(wbits=-15)
        raw_stream = compressor.compress(payload) + compressor.flush()
        wrapped = b'\x00\x01' + raw_stream
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['raw_inflate_offset2']['ok'])
        self.assertEqual(probe['raw_inflate_offset2']['out_len'], 64)

    def test_lzma_accepted_after_four_byte_prefix(self):
        payload = b'E' * 64
        wrapped = b'\x00\x01\x02\x03' + lzma.compress(payload)
        probe = cah.decompress_probe(wrapped)
        self.assertTrue(probe['lzma_offset4']['ok'])
        self.assertEqual(probe['lzma_offset4']['out_len'], 64)


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
