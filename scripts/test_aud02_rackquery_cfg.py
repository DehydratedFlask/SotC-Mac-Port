#!/usr/bin/env python3
"""Synthetic CFG/dataflow and rejection fixtures; no original game data."""
import importlib.util
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

MODULE_PATH = Path(__file__).with_name('aud02_rackquery_cfg.py')
SPEC = importlib.util.spec_from_file_location('aud02_cfg', MODULE_PATH)
CFG = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CFG
SPEC.loader.exec_module(CFG)


def ins(address, mnemonic, *operands):
    # Dataflow fixtures intentionally use abstract unit-size instructions. Byte
    # reconciliation fixtures below independently use an actual MOV encoding.
    return CFG.Instruction(address, b'\x90', mnemonic, tuple(operands))


def state(**overrides):
    value = {reg: CFG.unknown('fixture-entry:' + reg) for reg in CFG.REGISTERS}
    value.update(overrides)
    return value


def constant(value, definition=0):
    return frozenset({CFG.Origin('immediate', str(value), definition)})


def values(origins):
    return {origin.detail for origin in origins if origin.kind == 'immediate'}


def minimal_elf():
    raw = bytearray(0x205)
    raw[:7] = b'\x7fELF\x02\x01\x01'
    struct.pack_into('<H', raw, 18, 62)
    struct.pack_into('<Q', raw, 32, 64)
    struct.pack_into('<HH', raw, 54, 56, 1)
    struct.pack_into('<IIQQQQQQ', raw, 64, 1, 5, 0x200, 0x1000, 0, 5, 5, 0x1000)
    raw[0x200:] = bytes.fromhex('ba07000000')
    return bytes(raw)


class FlowTests(unittest.TestCase):
    def solve(self, blocks, instructions, preserve=()):
        return CFG.solve(blocks, {i.address: i for i in instructions}, blocks[0].start, frozenset(preserve))[0]

    def test_unique_predecessor(self):
        before = self.solve([CFG.Block(0, 2, (4,)), CFG.Block(4, 6, ())], [
            ins(0, 'mov', 'edx', '7'), ins(1, 'jmp', '4'), ins(4, 'mov', 'rsi', 'rdx'), ins(5, 'ret')])
        self.assertEqual(values(before[5]['rsi']), {'7'})
        self.assertFalse(any(x.kind == 'unknown' for x in before[5]['rsi']))

    def diamond(self, left, right):
        return self.solve([CFG.Block(0, 1, (1, 3)), CFG.Block(1, 3, (5,)), CFG.Block(3, 5, (5,)), CFG.Block(5, 6, ())], [
            ins(0, 'je', '3'), ins(1, 'mov', 'edx', str(left)), ins(2, 'jmp', '5'),
            ins(3, 'mov', 'edx', str(right)), ins(4, 'jmp', '5'), ins(5, 'ret')])[5]['rdx']

    def test_equal_diamond_retains_definition_sites(self):
        origins = self.diamond(7, 7)
        self.assertEqual(values(origins), {'7'})
        self.assertEqual({x.definition for x in origins}, {1, 3})
        self.assertEqual(CFG.describe(origins)['status'], 'conditional-unique-origin')

    def test_conflicting_diamond(self):
        origins = self.diamond(7, 8)
        self.assertEqual(values(origins), {'7', '8'})
        self.assertEqual(CFG.describe(origins)['status'], 'conditional-multiple-origins')

    def test_loop_reaches_fixed_point_and_keeps_entry_alternative(self):
        blocks = [CFG.Block(0, 2, (2,)), CFG.Block(2, 3, (3, 6)), CFG.Block(3, 5, (2,)), CFG.Block(6, 7, ())]
        instructions = [ins(0, 'mov', 'edx', '7'), ins(1, 'jmp', '2'), ins(2, 'je', '6'),
                        ins(3, 'mov', 'edx', '8'), ins(4, 'jmp', '2'), ins(6, 'ret')]
        before, info = CFG.solve(blocks, {i.address: i for i in instructions}, 0)
        self.assertEqual(values(before[6]['rdx']), {'7', '8'})
        self.assertLess(info['transfer_count'], 20)

    def test_missing_edge_is_unknown_even_with_known_other_predecessor(self):
        blocks = [CFG.Block(0, 1, (1,)), CFG.Block(1, 3, (5,)), CFG.Block(3, 5, (5,)), CFG.Block(5, 6, ())]
        before, info = CFG.solve(blocks, {i.address: i for i in [ins(0, 'je', '3'), ins(1, 'mov', 'edx', '7'),
                ins(2, 'jmp', '5'), ins(3, 'nop'), ins(4, 'jmp', '5'), ins(5, 'ret')]}, 0)
        self.assertIn('7', values(before[5]['rdx']))
        self.assertTrue(any(x.kind == 'unknown' for x in before[5]['rdx']))
        self.assertTrue(any(x['kind'] == 'missing-edge' for x in info['diagnostics']))

    def test_dangling_successor_marks_global_unknown(self):
        before, info = CFG.solve([CFG.Block(0, 1, (99,)), CFG.Block(1, 2, ())], {0: ins(0, 'jmp', '99'), 1: ins(1, 'ret')}, 0)
        self.assertTrue(any(x.detail == 'unbounded-or-terminal-cfg-gap' for x in before[1]['rdx']))
        self.assertTrue(any(x['kind'] == 'dangling-successor' for x in info['diagnostics']))

    def test_call_clobbers_even_usual_callee_saved_registers_by_default(self):
        initial = state(rbx=constant(7), rsp=constant(8))
        after = CFG.transfer(ins(1, 'call', '99'), initial, frozenset())
        self.assertTrue(all(x.kind == 'unknown' for x in after['rbx']))
        self.assertTrue(all(x.kind == 'unknown' for x in after['rsp']))

    def test_explicit_preservation_is_recorded_and_caller_register_still_killed(self):
        before = self.solve([CFG.Block(0, 5, ())], [ins(0, 'mov', 'ebx', '7'), ins(1, 'mov', 'edx', '8'),
            ins(2, 'call', '99'), ins(3, 'mov', 'rsi', 'rbx'), ins(4, 'ret')], preserve=('rbx',))
        self.assertEqual(values(before[4]['rsi']), {'7'})
        self.assertEqual({x.assumptions for x in before[4]['rsi']}, {('rbx',)})
        self.assertTrue(all(x.kind == 'unknown' for x in before[4]['rdx']))

    def test_partial_writes_kill_entire_family_including_high_byte(self):
        for register in ('dl', 'dh', 'dx'):
            with self.subTest(register=register):
                after = CFG.transfer(ins(1, 'mov', register, '1'), state(rdx=constant(7)), frozenset())
                self.assertTrue(all(x.kind == 'unknown' for x in after['rdx']))

    def test_32bit_write_zero_extends_and_invalid_copy_is_unknown(self):
        self.assertEqual(values(CFG.transfer(ins(1, 'mov', 'edx', '-1'), state(), frozenset())['rdx']), {'4294967295'})
        self.assertTrue(all(x.kind == 'unknown' for x in CFG.transfer(ins(1, 'mov', 'edx', 'ebx'), state(), frozenset())['rdx']))

    def test_unknown_explicit_and_implicit_writes_do_not_prove_origin(self):
        initial = state(rdx=constant(7), rbx=constant(8))
        for operation in (ins(1, 'add', 'rdx', '1'), ins(1, 'xchg', 'rdx', 'rbx'), ins(1, 'mul', 'rbx'), ins(1, 'syscall')):
            with self.subTest(operation=operation):
                self.assertTrue(any(x.kind == 'unknown' for x in CFG.transfer(operation, initial, frozenset())['rdx']))

    def test_lea_retains_unknown_base_and_call_assumption(self):
        initial = state(rsp=frozenset({CFG.Origin('symbolic-entry-rsp', 'rsp', -1, ('rsp',))}))
        after = CFG.transfer(ins(1, 'lea', 'rbx', '[rsp+32]'), initial, frozenset())
        self.assertEqual({x.assumptions for x in after['rbx']}, {('rsp',)})
        initial['rsp'] = CFG.unknown('call-clobber')
        self.assertTrue(any(x.kind == 'unknown' for x in CFG.transfer(ins(1, 'lea', 'rbx', '[rsp+32]'), initial, frozenset())['rbx']))

    def test_block_gaps_overlap_and_missing_entry_rejected(self):
        for blocks in ([CFG.Block(0, 2, ())], [CFG.Block(0, 1, ()), CFG.Block(0, 1, ())], [CFG.Block(3, 4, ())]):
            with self.subTest(blocks=blocks):
                with self.assertRaises(ValueError):
                    CFG.solve(blocks, {0: ins(0, 'ret')}, 0)

    def test_unobserved_indirect_edges_not_reported_complete(self):
        _, summary = CFG.solve([CFG.Block(0, 1, (1,)), CFG.Block(1, 2, ())], {0: ins(0, 'jmp', 'rax'), 1: ins(1, 'ret')}, 0)
        self.assertEqual(summary['graph_completeness'], 'NOT_ESTABLISHED')
        self.assertTrue(any(x['kind'] == 'indirect-successor-set-unverified' for x in summary['diagnostics']))


class InputTests(unittest.TestCase):
    def test_each_input_hash_is_pinned(self):
        with tempfile.TemporaryDirectory() as temp:
            paths = [Path(temp) / name for name in ('baseline', 'cache', 'elf')]
            for path, content in zip(paths, (b'{}', b'{}', minimal_elf())):
                path.write_bytes(content)
            expected = [CFG.digest(p.read_bytes()) for p in paths]
            for rejected in range(3):
                hashes = expected.copy()
                hashes[rejected] = '0' * 64
                with self.subTest(rejected=rejected), patch.multiple(CFG, EXPECTED_BASELINE=hashes[0], EXPECTED_CACHE=hashes[1], EXPECTED_ELF=hashes[2]):
                    with self.assertRaisesRegex(ValueError, 'SHA256 mismatch'):
                        CFG.load_pinned(*paths)

    def test_bad_elf_and_unmapped_bytes_rejected(self):
        with self.assertRaises(ValueError):
            CFG.elf_segments(b'bad')
        raw = minimal_elf()
        segments = CFG.elf_segments(raw)
        self.assertEqual(CFG.image_bytes(raw, segments, 0x1000, 5), bytes.fromhex('ba07000000'))
        with self.assertRaises(ValueError):
            CFG.image_bytes(raw, segments, 0x1004, 5)

    def test_operand_canonicalization_matches_rip_and_branch_alias(self):
        raw = bytes.fromhex('488d0510000000')
        a = CFG.parse_text(0x1000, raw, 'LEA RAX, [0x00001017]')
        b = CFG.parse_text(0x1000, raw, 'lea rax, [rip + 0x10] # 0x1017')
        self.assertEqual(a, b)
        self.assertEqual(CFG.parse_text(0, b'\x74\x05', 'JZ 0x00000007'), CFG.parse_text(0, b'\x74\x05', 'je 0x7 <PT_LOAD#0+7>'))

    def test_presentation_rules_preserve_real_operand_differences(self):
        raw = bytes.fromhex('488d548b08')
        self.assertEqual(CFG.parse_text(0, raw, 'lea rdx, [rbx+4*rcx+0x8]'),
                         CFG.parse_text(0, raw, 'LEA RDX, [RBX + RCX*0x4 + 8]'))
        self.assertEqual(CFG.parse_text(0, raw, 'lea rdx, [rbp-8]'),
                         CFG.parse_text(0, raw, 'lea rdx, [rbp + -0x8]'))
        self.assertEqual(CFG.parse_text(0, raw, 'lea rdx, [rbx+rcx]'),
                         CFG.parse_text(0, raw, 'lea rdx, [rbx+rcx*1]'))
        nop = bytes.fromhex('660f1f0400')
        self.assertEqual(CFG.parse_text(0, nop, 'nop word ptr [rax+rax]'),
                         CFG.parse_text(0, nop, 'nop dword ptr [rax+rax*1]'))
        reference = CFG.parse_text(0, raw, 'mov rdx, qword ptr [rbx+rcx*4+8]')
        for text in ('mov rdx, dword ptr [rbx+rcx*4+8]',
                     'mov rdx, qword ptr [rbx+rcx*8+8]',
                     'mov rdx, qword ptr [rbx+rsi*4+8]',
                     'mov rdx, qword ptr [rbx+rcx*4+9]',
                     'mov rdx, qword ptr [rbx+rcx*4-8]',
                     'mov rsi, qword ptr [rbx+rcx*4+8]'):
            with self.subTest(text=text):
                self.assertNotEqual(reference, CFG.parse_text(0, raw, text))
        self.assertNotEqual(CFG.parse_text(0, nop, 'nop word ptr [rax+rax]'),
                            CFG.parse_text(0, nop, 'nop word ptr [rax+rax*2]'))

    def test_instruction_reconciliation_rejects_wrong_bytes_text_or_missing_boundary(self):
        raw = minimal_elf()
        previous = {'sweep_provenance': {'fixture': {'ranges_inclusive': [['0x1000', '0x1004']]}}, 'callsites': []}
        capture = {'data': {'raw_result': {'value': {'assembly': ['0x1000: MOV EDX, 0x7'], 'procedure': {'body': {'ranges': [{'start': '0x1000', 'end': '0x1004'}]}}}}}}
        result = type('Process', (), {'returncode': 0, 'stdout': ' 1000: ba 07 00 00 00\tmov\tedx, 0x7\n', 'stderr': ''})()
        with patch.object(CFG.subprocess, 'run', return_value=result):
            decoded, _ = CFG.decode_and_reconcile(previous, capture, raw, 'objdump', '/private/fixture')
            self.assertEqual(len(decoded), 1)
            for text in ('0x1000: MOV EDX, 0x8', '0x1001: MOV EDX, 0x7'):
                capture['data']['raw_result']['value']['assembly'] = [text]
                with self.assertRaises(ValueError):
                    CFG.decode_and_reconcile(previous, capture, raw, 'objdump', '/private/fixture')
            capture['data']['raw_result']['value']['assembly'] = ['0x1000: MOV EDX, 0x7']
            changed = bytearray(raw)
            changed[-1] = 1
            with self.assertRaisesRegex(ValueError, 'bytes differ'):
                CFG.decode_and_reconcile(previous, capture, bytes(changed), 'objdump', '/private/fixture')

    def test_public_output_is_rejected(self):
        result = type('Process', (), {'returncode': 1})()
        with patch.object(CFG.subprocess, 'run', return_value=result), self.assertRaisesRegex(ValueError, 'private destination'):
            CFG.private_output_path(Path(__file__).with_name('public-result.json'))


if __name__ == '__main__':
    unittest.main()
