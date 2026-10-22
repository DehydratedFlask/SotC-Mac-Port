#!/usr/bin/env python3
"""Synthetic fixtures for the bounded XFF audio-bank header observations."""
from __future__ import annotations

import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

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

    def test_rejects_xff_magic_only_in_opaque_span(self):
        opaque = bytearray(16)
        opaque[4:8] = abh.XFF_MAGIC
        member = bytearray(make_member(b'bounded body', bytes(opaque)))
        member[:4] = b'qath'

        with self.assertRaises(abh.AudioBankFormatError) as ctx:
            abh.parse_xff_member(member)
        self.assertIs(type(ctx.exception), abh.AudioBankFormatError)
        self.assertEqual(
            str(ctx.exception),
            'unsupported member prefix 71617468; expected 78666600',
        )

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


class SyntheticPsarc:
    def __init__(self, member_path: str, member: bytes, offset: int, block: int):
        self._member_path = member_path
        self._member = member
        self.entries = [
            {'size': 0, 'offset': 0, 'block': 0},
            {'size': len(member), 'offset': offset, 'block': block},
        ]
        self.read_calls: list[int] = []
        self.closed = False

    def paths(self) -> list[str]:
        return [self._member_path]

    def read(self, entry_id: int) -> bytes:
        self.read_calls.append(entry_id)
        if entry_id != 1:
            raise AssertionError(f'unexpected synthetic entry id {entry_id}')
        return self._member

    def close(self) -> None:
        self.closed = True


class InspectMembersTests(unittest.TestCase):
    def _fixture(self, *, manifest_member_sha256: str | None = None):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        archive_path = root / 'fixture.psarc'
        archive_bytes = b'synthetic archive identity bytes'
        archive_path.write_bytes(archive_bytes)

        summary_path = root / 'fixture.summary.json'
        archive_sha256 = hashlib.sha256(archive_bytes).hexdigest()
        summary_path.write_text(json.dumps({
            'bytes': len(archive_bytes),
            'sha256': archive_sha256,
        }))

        member_path = 'sound/fixture.hd'
        member = make_member(b'synthetic member payload')
        member_sha256 = hashlib.sha256(member).hexdigest()
        reader = SyntheticPsarc(member_path, member, offset=0x1234, block=7)
        manifest_path = root / 'audio.json'
        manifest_path.write_text(json.dumps({
            'members': [{
                'archive': str(archive_path.resolve()),
                'path': member_path,
                'entry_id': 1,
                'entry_offset': 0x1234,
                'first_block': 7,
                'size': len(member),
                'member_sha256': manifest_member_sha256 or member_sha256,
            }],
        }))

        return {
            'archive_path': archive_path,
            'archive_bytes': archive_bytes,
            'archive_sha256': archive_sha256,
            'summary_path': summary_path,
            'manifest_path': manifest_path,
            'member_path': member_path,
            'member': member,
            'member_sha256': member_sha256,
            'reader': reader,
        }

    def test_inspect_members_accepts_full_archive_and_member_identity(self):
        fixture = self._fixture()
        with patch.object(abh, 'Psarc', return_value=fixture['reader']) as psarc:
            report = abh.inspect_members(
                fixture['archive_path'],
                [fixture['member_path']],
                fixture['manifest_path'],
                fixture['summary_path'],
            )

        psarc.assert_called_once_with(fixture['archive_path'].resolve())
        self.assertEqual(fixture['reader'].read_calls, [1])
        self.assertTrue(fixture['reader'].closed)
        self.assertEqual(report['schema_version'], 1)
        self.assertEqual(report['task'], 'AUD-01-XFF-01')
        self.assertEqual(report['archive'], {
            'path': str(fixture['archive_path'].resolve()),
            'bytes': len(fixture['archive_bytes']),
            'sha256': fixture['archive_sha256'],
            'summary_path': str(fixture['summary_path']),
            'summary_identity_match': True,
        })
        self.assertEqual(report['members'], [{
            'path': fixture['member_path'],
            'psarc_entry_id': 1,
            'archive_offset': 0x1234,
            'first_block': 7,
            'decoded_member_bytes': len(fixture['member']),
            'decoded_member_sha256': fixture['member_sha256'],
            'manifest_locator_and_hash_match': True,
            'observations': abh.parse_xff_member(fixture['member']),
        }])

    def test_inspect_members_rejects_stale_archive_summary_before_open(self):
        fixture = self._fixture()
        summary = json.loads(fixture['summary_path'].read_text())
        summary['bytes'] -= 1
        fixture['summary_path'].write_text(json.dumps(summary))

        with patch.object(abh, 'Psarc') as psarc:
            with self.assertRaises(abh.AudioBankFormatError) as context:
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_not_called()
        self.assertEqual(
            str(context.exception),
            f"archive summary byte count {summary['bytes']} != live {len(fixture['archive_bytes'])}",
        )

    def test_inspect_members_rejects_stale_archive_summary_sha256_before_open(self):
        fixture = self._fixture()
        summary = json.loads(fixture['summary_path'].read_text())
        summary['sha256'] = '0' * 64
        fixture['summary_path'].write_text(json.dumps(summary))

        with patch.object(abh, 'Psarc') as psarc:
            with self.assertRaises(abh.AudioBankFormatError) as context:
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_not_called()
        self.assertEqual(
            str(context.exception),
            f"archive summary SHA-256 {summary['sha256']} != live {fixture['archive_sha256']}",
        )

    def test_inspect_members_closes_reader_after_manifest_identity_mismatch(self):
        fixture = self._fixture(manifest_member_sha256='0' * 64)
        with patch.object(abh, 'Psarc', return_value=fixture['reader']) as psarc:
            with self.assertRaisesRegex(
                    abh.AudioBankFormatError,
                    'manifest member_sha256 mismatch for sound/fixture\\.hd'):
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_called_once_with(fixture['archive_path'].resolve())
        self.assertEqual(fixture['reader'].read_calls, [1])
        self.assertTrue(fixture['reader'].closed)

    def test_inspect_members_wraps_psarc_read_error_and_closes_reader(self):
        fixture = self._fixture()
        reader = fixture['reader']
        with patch.object(
                reader, 'read', side_effect=abh.PsarcError('synthetic read failure')) as read, \
                patch.object(abh, 'Psarc', return_value=reader) as psarc:
            with self.assertRaises(abh.AudioBankFormatError) as context:
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_called_once_with(fixture['archive_path'].resolve())
        read.assert_called_once_with(1)
        self.assertEqual(
            str(context.exception),
            'cannot read PSARC entry 1 (sound/fixture.hd): synthetic read failure',
        )
        self.assertTrue(reader.closed)

    def test_inspect_members_rejects_zero_exact_path_matches_and_closes_reader(self):
        fixture = self._fixture()
        reader = fixture['reader']
        with patch.object(reader, 'paths', return_value=[]), \
                patch.object(abh, 'Psarc', return_value=reader) as psarc:
            with self.assertRaises(abh.AudioBankFormatError) as ctx:
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_called_once_with(fixture['archive_path'].resolve())
        self.assertEqual(
            str(ctx.exception),
            "expected one exact PSARC path for 'sound/fixture.hd', found 0",
        )
        self.assertEqual(reader.read_calls, [])
        self.assertTrue(reader.closed)

    def test_inspect_members_rejects_duplicate_exact_path_matches_and_closes_reader(self):
        fixture = self._fixture()
        reader = fixture['reader']
        with patch.object(
                reader,
                'paths',
                return_value=[fixture['member_path'], fixture['member_path']]), \
                patch.object(abh, 'Psarc', return_value=reader) as psarc:
            with self.assertRaises(abh.AudioBankFormatError) as ctx:
                abh.inspect_members(
                    fixture['archive_path'],
                    [fixture['member_path']],
                    fixture['manifest_path'],
                    fixture['summary_path'],
                )

        psarc.assert_called_once_with(fixture['archive_path'].resolve())
        self.assertEqual(
            str(ctx.exception),
            "expected one exact PSARC path for 'sound/fixture.hd', found 2",
        )
        self.assertEqual(reader.read_calls, [])
        self.assertTrue(reader.closed)


if __name__ == '__main__':
    unittest.main()
