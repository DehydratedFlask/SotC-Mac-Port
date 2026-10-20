#!/usr/bin/env python3
"""Fixture tests for ps4_dynlink using synthetic bytes only (no game data)."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import ps4_dynlink as d


def image(vaddr, payload):
    ph = [dict(type=d.PT_LOAD, flags=5, offset=0, vaddr=vaddr, paddr=vaddr,
               filesz=len(payload), memsz=len(payload), align=8)]
    return d.Image(bytes(payload), ph)


class Tests(unittest.TestCase):
    def test_nid_known_values(self):
        self.assertEqual(d.nid('_init_env'), 'bzQExy189ZI')   # also derived by pinned prepare.py
        self.assertEqual(d.nid('atexit'), '8G2LB+A3rzg')

    def test_encode_id_widths(self):
        self.assertEqual([d.encode_id(v) for v in (0, 63, 64, 0xfff, 0x1000)],
                         ['A', '-', 'BA', '--', 'BAA'])

    def test_thunk_resolves_got_slot(self):
        code = b'\xff\x25' + struct.pack('<i', 0x10 - 6) + b'\0' * 16   # jmp [rip+0x10-6] at 0x100
        img = image(0x100, code)
        got = {0x110: dict(symbol=1, type='R_X86_64_JUMP_SLOT', nid='X', addend=0)}
        out = d.resolve_thunk(img, 0x100, got)
        self.assertEqual(out['got_slot'], '0x110')
        self.assertEqual(out['relocation']['nid'], 'X')
        self.assertEqual(d.resolve_thunk(image(0, b'\x90' * 8), 0, {})['kind'], 'not-a-plt-jmp')

    def test_ctor_walk_applies_relocs_skips_zero_stops_at_sentinel(self):
        payload = struct.pack('<4Q', 0xffffffffffffffff, 0, 0, 0)   # slots 0x1000..0x1018
        img = image(0x1000, payload)
        relocs = [dict(type=8, target=0x1010, addend=0x4242)]
        out = d.walk_ctors(img, relocs, 0x1018)
        self.assertEqual([e['target'] for e in out['entries']], ['0x4242'])
        self.assertEqual(out['sentinel'], '0x1000')

    def test_ctor_walk_unterminated_is_error(self):
        with self.assertRaises(ValueError):
            d.walk_ctors(image(0x1000, struct.pack('<2Q', 1, 2)), [], 0x1008, limit=1)

    def test_out_of_bounds_read_rejected(self):
        with self.assertRaises(ValueError):
            image(0, b'\0' * 8).read(4, 8)
        with self.assertRaises(ValueError):
            d.span(b'abc', 2, 5)


if __name__ == '__main__':
    unittest.main()
