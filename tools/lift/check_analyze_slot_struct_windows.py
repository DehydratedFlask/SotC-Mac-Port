#!/usr/bin/env python3
"""Regression for decode-window resynchronisation in tools/lift/analyze_slot_struct.py.

The walker decodes fixed 0x400-byte windows. An instruction whose bytes straddle a window edge used
to be decoded as a truncated `<unknown>` row followed by misaligned instructions, which silently
dropped real direct calls and RIP-relative references (slot 306: 0xa637ae and 0xa643ad).

1. Synthetic fixture (no private input): `call rel32`, `lea [rip+d]` and `mov [rip+d]` straddle the
   0x400 and 0x800 window edges. Fails on the old fixed-window decoder, passes on the overlap decoder.
2. Pinned private-ELF check (skipped when local/re/eboot.elf is absent): slot 306 and slot 0x2569d0,
   with counts cross-checked against an independent llvm-objdump linear decode.

Nothing is executed; the disassembler only reads bytes.
"""
import importlib.util
import struct
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def load_module():
    spec = importlib.util.spec_from_file_location('slot_struct_under_test', HERE / 'analyze_slot_struct.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.WORK = Path(tempfile.mkdtemp(prefix='slot-struct-windows-'))
    module._cache.clear()
    return module


class FakeImage:
    """vaddr -> bytes over a single flat buffer; reads outside it raise ValueError like the real Image."""

    def __init__(self, base, data):
        self.base, self.data = base, bytes(data)

    def read(self, vaddr, size):
        lo = vaddr - self.base
        if lo < 0 or lo + size > len(self.data):
            raise ValueError(f'{vaddr:#x}+{size} outside fixture')
        return self.data[lo:lo + size]

    def mapped(self, vaddr, size=1):
        return self.base <= vaddr and vaddr + size <= self.base + len(self.data)


BASE = 0x1000
FUNC2 = BASE + 0x900            # call target (recorded, not walked)
CALL_AT = BASE + 0x3FE          # 5-byte call: bytes 0x3fe..0x402, crosses the 0x400 edge
LEA_AT = BASE + 0x403           # 7-byte lea, right after the straddling call
MOV_AT = BASE + 0x7FD           # 7-byte mov [rip+d], rax: bytes 0x7fd..0x803, crosses the 0x800 edge
LEA_DISP = 0x120
MOV_DISP = 0x240


def build_fixture():
    data = bytearray(b"\x90" * 0x1000)          # nops, so every byte offset is an instruction start
    data[0x3FE:0x403] = b'\xe8' + struct.pack('<i', FUNC2 - (CALL_AT + 5))
    data[0x403:0x40A] = b'\x48\x8d\x05' + struct.pack('<i', LEA_DISP)          # lea rax, [rip+d]
    data[0x7FD:0x804] = b'\x48\x89\x05' + struct.pack('<i', MOV_DISP)          # mov [rip+d], rax
    data[0x804] = 0xC3                                                          # ret
    expected = set(range(BASE, CALL_AT))                                       # leading nops
    expected |= {CALL_AT, LEA_AT}
    expected |= set(range(LEA_AT + 7, MOV_AT))                                 # nops between
    expected |= {MOV_AT, BASE + 0x804}                                         # call targets are not walked
    return FakeImage(BASE, data), expected


class WindowResyncSynthetic(unittest.TestCase):
    def setUp(self):
        self.m = load_module()
        self.image, self.expected = build_fixture()

    def test_straddling_instructions_are_decoded_whole(self):
        report = self.m.build_report(BASE, self.image, {}, {}, 'synthetic')

        self.assertEqual([(c['at'], c['target']) for c in report['direct_calls']],
                         [(hex(CALL_AT), hex(FUNC2))], 'straddling call was lost or mis-decoded')
        refs = {r['at']: r['target'] for r in report['rip_relative_refs']}
        self.assertEqual(refs.get(hex(LEA_AT)), hex(LEA_AT + 7 + LEA_DISP))
        self.assertEqual(refs.get(hex(MOV_AT)), hex(MOV_AT + 7 + MOV_DISP))
        self.assertEqual(set(refs), {hex(LEA_AT), hex(MOV_AT)}, 'spurious RIP-relative rows')
        self.assertEqual([s['at'] for s in report['rip_relative_stores']], [hex(MOV_AT)])
        self.assertEqual(report['reachable_instructions'], len(self.expected))
        self.assertEqual(report['blocks_terminated_by_ret'], 1)

    def test_every_true_boundary_decodes_without_unknown_rows(self):
        self.m.build_report(BASE, self.image, {}, {}, 'synthetic')
        for addr in sorted(self.expected):
            ins = self.m.instr_at(self.image, addr)
            self.assertIsNotNone(ins, hex(addr))
            self.assertNotIn('unknown', ins['mnemonic'], hex(addr))

    def test_window_table_keeps_whole_straddler_and_drops_overlap_rows(self):
        table = self.m.decode_from(self.image, BASE)
        self.assertEqual(table[CALL_AT]['mnemonic'], 'call')
        self.assertEqual(table[CALL_AT]['bytes'], self.image.read(CALL_AT, 5))
        self.assertTrue(all(a < BASE + 0x400 for a in table), 'overlap-tail row leaked into the table')
        self.assertNotIn(BASE + 0x400, table)       # next window starts at the first missing boundary
        self.assertEqual(self.m.instr_at(self.image, BASE + 0x403)['mnemonic'], 'lea')


ELF = ROOT / 'local/re/eboot.elf'


@unittest.skipUnless(ELF.exists(), 'local/re/eboot.elf (private game image) is absent')
class PinnedPrivateBodies(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.m = load_module()
        _, cls.image, cls.names, cls.nids = cls.m.load_image()

    def report(self, rva):
        self.m._cache.clear()
        return self.m.build_report(rva, self.image, self.names, self.nids)

    def test_slot_306(self):
        r = self.report(0xA633B0)
        self.assertEqual(len(r['direct_calls']), 198)
        self.assertEqual(len([i for i in r['indirect_transfers'] if i.get('kind') == 'call']), 11)
        self.assertEqual(len(r['indirect_transfers']), 11)
        self.assertEqual(r['reachable_instructions'], 1568)
        self.assertEqual(r['span_bytes'], 8461)
        self.assertEqual(r['end_exclusive'], hex(0xA633B0 + 8461))
        calls = {c['at']: c['target'] for c in r['direct_calls']}
        self.assertEqual(calls['0xa637ae'], '0xf8f390')    # __cxa_atexit, straddles 0xa637b0
        self.assertEqual(calls['0xa643ad'], '0xc300')      # float variable ctor, straddles 0xa643b0
        refs = {x['at']: x['target'] for x in r['rip_relative_refs']}
        self.assertEqual(refs['0xa637b3'], '0x32f9348')    # lea right after the straddling call

    def test_slot_2569d0_matches_independent_linear_decode(self):
        # llvm-objdump over [0x2569d0, 0x259b22): 2,142 instructions, 359 direct and 10 indirect calls.
        r = self.report(0x2569D0)
        self.assertEqual(r['reachable_instructions'], 2142)
        self.assertEqual(len(r['direct_calls']), 359)
        self.assertEqual(len([i for i in r['indirect_transfers'] if i.get('kind') == 'call']), 10)
        self.assertEqual(r['end_exclusive'], hex(0x259B22))

    def test_small_bodies_unchanged(self):
        # Bodies that never straddle a window edge keep their previous results.
        self.assertEqual(self.report(0x25A4D0)['reachable_instructions'], 233)
        self.assertEqual(self.report(0xA2AFB0)['reachable_instructions'], 114)


if __name__ == '__main__':
    unittest.main()
