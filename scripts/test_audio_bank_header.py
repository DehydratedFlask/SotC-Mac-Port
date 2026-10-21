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

    def test_reports_are_equal_for_bytes_like_inputs(self):
        member = make_member()
        reports = [
            abh.parse_xff_member(member),
            abh.parse_xff_member(bytearray(member)),
            abh.parse_xff_member(memoryview(member)),
        ]

        self.assertEqual(reports[1], reports[0])
        self.assertEqual(reports[2], reports[0])
        self.assertTrue(reports[0]['u32le_at_0x14_matches_decoded_length'])

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

    def test_rejects_size_word_shorter_than_decoded_span(self):
        member = bytearray(make_member())
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = (
            (len(member) - 1).to_bytes(4, 'little'))
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'decoded member length'):
            abh.parse_xff_member(member)

    def test_rejects_wrong_byte_order_size_word(self):
        member = bytearray(make_member())
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = len(member).to_bytes(4, 'big')
        with self.assertRaisesRegex(abh.AudioBankFormatError, 'decoded member length'):
            abh.parse_xff_member(member)

    def test_rejects_declared_length_zero(self):
        # A zero word can never match a >=24-byte member, so it is always a reject.
        member = bytearray(make_member(b''))
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = (0).to_bytes(4, 'little')
        with self.assertRaises(abh.AudioBankFormatError) as ctx:
            abh.parse_xff_member(member)
        self.assertIs(type(ctx.exception), abh.AudioBankFormatError)
        self.assertEqual(
            str(ctx.exception),
            'u32le at 0x14 is 0, but decoded member length is 24',
        )

    def test_declared_length_is_read_as_unsigned_32_bit(self):
        # 0xffffffff is a plausible "unknown length" sentinel and 0x80000000 would
        # be negative under a signed read; both must be reported as u32 values.
        for word, expected in (
            (0xFFFFFFFF, 'u32le at 0x14 is 4294967295, but decoded member length is 24'),
            (0x80000000, 'u32le at 0x14 is 2147483648, but decoded member length is 24'),
        ):
            with self.subTest(word=hex(word)):
                member = bytearray(make_member(b''))
                member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = (
                    word.to_bytes(4, 'little'))
                with self.assertRaises(abh.AudioBankFormatError) as ctx:
                    abh.parse_xff_member(member)
                self.assertIs(type(ctx.exception), abh.AudioBankFormatError)
                self.assertEqual(str(ctx.exception), expected)

    def test_rejects_header_only_length_declared_inside_a_longer_buffer(self):
        # 24 is the header-only span, i.e. a declared length inside the buffer but
        # four bytes smaller than the buffer itself.
        member = bytearray(make_member(b'    '))
        self.assertEqual(len(member), abh.XFF_MIN_MEMBER_SIZE + 4)
        member[abh.XFF_SIZE_WORD_OFFSET:abh.XFF_MIN_MEMBER_SIZE] = (
            abh.XFF_MIN_MEMBER_SIZE.to_bytes(4, 'little'))
        with self.assertRaises(abh.AudioBankFormatError) as ctx:
            abh.parse_xff_member(member)
        self.assertIs(type(ctx.exception), abh.AudioBankFormatError)
        self.assertEqual(
            str(ctx.exception),
            'u32le at 0x14 is 24, but decoded member length is 28',
        )

    def test_accepts_declared_length_above_16_bit_range(self):
        # Guards against a 16-bit truncation of either the word or the comparison.
        payload = b'\x5a' * 70000
        member = make_member(payload)
        self.assertEqual(len(member), 70024)
        self.assertGreater(len(member), 0xFFFF)
        report = abh.parse_xff_member(member)
        self.assertEqual(report['u32le_at_0x14'], 70024)
        self.assertEqual(report['decoded_member_bytes'], 70024)
        self.assertTrue(report['u32le_at_0x14_matches_decoded_length'])
        self.assertEqual(report['magic_hex'], abh.XFF_MAGIC.hex())
        self.assertEqual(report['opaque_0x04_0x13_hex'], bytes(range(16)).hex())
        self.assertEqual(report['field_meanings'],
                         'UNKNOWN except for the observed length match')
        self.assertEqual(report['codec'], 'UNKNOWN')

    def test_reports_exact_shortfall_when_the_size_word_is_cut_off(self):
        # The header word is the final four bytes of the minimum member, so a
        # 20- or 23-byte buffer holds only a fragment of it.
        for length in (20, 23):
            with self.subTest(length=length):
                truncated = abh.XFF_MAGIC + bytes(length - len(abh.XFF_MAGIC))
                self.assertEqual(len(truncated), length)
                with self.assertRaises(abh.AudioBankFormatError) as ctx:
                    abh.parse_xff_member(truncated)
                self.assertIs(type(ctx.exception), abh.AudioBankFormatError)
                self.assertEqual(
                    str(ctx.exception),
                    f'member is {length} bytes; need at least 24 bytes to '
                    'inspect the observed size word',
                )

    def test_rejects_non_bytes_like_input_with_type_error(self):
        # A str of the exact valid member length must not be coerced or accepted.
        member_text = 'xff\0' + 'a' * (abh.XFF_MIN_MEMBER_SIZE - len(abh.XFF_MAGIC))
        self.assertEqual(len(member_text), abh.XFF_MIN_MEMBER_SIZE)
        with self.assertRaises(TypeError) as ctx:
            abh.parse_xff_member(member_text)
        self.assertEqual(str(ctx.exception), 'member data must be bytes-like')


if __name__ == '__main__':
    unittest.main()
