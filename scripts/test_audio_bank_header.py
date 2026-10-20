#!/usr/bin/env python3
"""Synthetic fixtures for the bounded XFF audio-bank header observations."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import audio_bank_header as abh


def make_member(payload: bytes = b'fixture-payload', opaque: bytes = bytes(range(16))) -> bytes:
    if len(opaque) != abh.XFF_SIZE_WORD_OFFSET - len(abh.XFF_MAGIC):
        raise ValueError('opaque fixture span must be exactly 16 bytes')
    member_size = abh.XFF_MIN_MEMBER_SIZE + len(payload)
    return (abh.XFF_MAGIC + opaque + member_size.to_bytes(4, 'little') + payload)


class XffMemberTests(unittest.TestCase):
    def test_reports_only_observed_magic_opaque_span_and_length_word(self):
        opaque = bytes.fromhex('00000000000000000100000000000000')
        member = make_member(b'bounded body', opaque)

        report = abh.parse_xff_member(member)

        self.assertEqual(report['magic_hex'], '78666600')
        self.assertEqual(report['opaque_0x04_0x13_hex'], opaque.hex())
        self.assertEqual(report['u32le_at_0x14'], len(member))
        self.assertEqual(report['decoded_member_bytes'], len(member))
        self.assertTrue(report['u32le_at_0x14_matches_decoded_length'])
        self.assertEqual(report['codec'], 'UNKNOWN')
        self.assertEqual(report['field_meanings'],
                         'UNKNOWN except for the observed length match')

    def test_accepts_minimum_bounded_member(self):
        member = make_member(b'')
        self.assertEqual(len(member), abh.XFF_MIN_MEMBER_SIZE)
        self.assertEqual(abh.parse_xff_member(member)['u32le_at_0x14'], len(member))

    def test_rejects_truncated_before_size_word(self):
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'need at least'):
            abh.parse_xff_member(abh.XFF_MAGIC + bytes(abh.XFF_MIN_MEMBER_SIZE - 5))

    def test_rejects_unknown_prefix(self):
        member = bytearray(make_member())
        member[:4] = b'qath'
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'unsupported member prefix'):
            abh.parse_xff_member(member)

    def test_rejects_size_word_that_exceeds_decoded_span(self):
        member = bytearray(make_member())
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = (
            (len(member) + 1).to_bytes(4, 'little'))
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'decoded member length'):
            abh.parse_xff_member(member)

    def test_rejects_wrong_byte_order_size_word(self):
        member = bytearray(make_member())
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = len(member).to_bytes(4, 'big')
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'decoded member length'):
            abh.parse_xff_member(member)


if __name__ == '__main__':
    unittest.main()
