#!/usr/bin/env python3
"""Synthetic fixtures for the offline rdi/rsi/rdx argument-setup extractor (no game bytes)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import aud02_rackquery_args as q

FIXTURE = """
local/re/eboot.elf:\tfile format elf64-x86-64

  1000: 48 8d 95 90 ff ff ff            \tlea\trdx, [rbp - 0x70]
  1007: bf 02 40 00 00                  \tmov\tedi, 0x4002
  100c: 48 89 de                        \tmov\trsi, rbx
  100f: 44 89 73 1c                     \tmov\tdword ptr [rbx + 0x1c], r14d
  1013: e8 08 04 00 00                  \tcall\t0x1420 <PT_LOAD#0+0x1420>
  1018: 85 c0                           \ttest\teax, eax
  101a: 0f 88 00 00 00 00               \tjs\t0x1020 <PT_LOAD#0+0x1020>
  1020: 48 8b 75 98                     \tmov\trsi, qword ptr [rbp - 0x68]
  1024: 90                              \tnop
"""


class ParseTests(unittest.TestCase):
    def test_parse_strips_comments_and_splits_operands(self):
        ins = q.parse_objdump(FIXTURE)
        self.assertEqual(len(ins), 9)
        self.assertEqual(ins[0].text, "lea rdx, [rbp - 0x70]")
        self.assertEqual(ins[4].ops, ("0x1420",))
        self.assertEqual(ins[8].ops, ())
        self.assertEqual(ins[4].end, 0x1018)

    def test_register_families(self):
        self.assertEqual(q.REG_TO_FAMILY["edi"], "rdi")
        self.assertEqual(q.REG_TO_FAMILY["r14d"], "r14")


class AnalysisTests(unittest.TestCase):
    def setUp(self):
        self.ins = q.parse_objdump(FIXTURE)
        self.targets = q.branch_targets(self.ins)

    def test_branch_targets(self):
        self.assertEqual(self.targets, {0x1020})

    def test_last_writer_per_arg_register(self):
        r = q.analyze_callsite(self.ins, self.targets, 0x1013, self.ins)
        self.assertTrue(r["instruction_boundary_in_linear_sweep"])
        self.assertEqual(r["args"]["rdi"]["kind"], "immediate")
        self.assertEqual(r["args"]["rdi"]["value"], 0x4002)
        self.assertEqual(r["args"]["rsi"]["kind"], "register-copy")
        self.assertEqual(r["args"]["rsi"]["source"], "rbx")
        self.assertEqual(r["args"]["rdx"]["kind"], "lea")
        self.assertEqual(r["arg3_slot"], {"base": "rbp", "disp": -0x70})

    def test_post_call_sign_check_and_plus8_read(self):
        r = q.analyze_callsite(self.ins, self.targets, 0x1013, self.ins)
        self.assertTrue(r["after_call"]["eax_sign_checked_immediately"])
        self.assertEqual(r["after_call"]["first_read_of_arg3_slot_plus8_in_next8"]["addr"], "0x1020")

    def test_misaligned_address_is_not_a_boundary(self):
        r = q.analyze_callsite(self.ins, self.targets, 0x1014, self.ins)
        self.assertFalse(r["instruction_boundary_in_linear_sweep"])

    def test_backward_scan_stops_at_prior_call(self):
        text = ("  10: bf 01 00 00 00                  \tmov\tedi, 0x1\n"
                "  15: e8 00 00 00 00                  \tcall\t0x1a\n"
                "  1a: e8 00 00 00 00                  \tcall\t0x1f\n")
        ins = q.parse_objdump(text)
        r = q.analyze_callsite(ins, set(), 0x1A, ins)
        self.assertIsNone(r["args"]["rdi"])  # defined before an intervening call: not credited
        self.assertIn("prior call", r["backward_scan_stop"])

    def test_register_copy_chases_callee_saved_across_call(self):
        text = ("  10: 48 8d 9c 24 20 0e 00 00         \tlea\trbx, [rsp + 0xe20]\n"
                "  18: e8 00 00 00 00                  \tcall\t0x1d\n"
                "  1d: 48 89 da                        \tmov\trdx, rbx\n"
                "  20: e8 00 00 00 00                  \tcall\t0x25\n")
        ins = q.parse_objdump(text)
        r = q.analyze_callsite(ins, set(), 0x20, ins)
        self.assertEqual(r["arg3_slot"], {"base": "rsp", "disp": 0xE20})
        self.assertEqual(r["register_copy_chains"]["rdx"]["resolution"], "in-block")

    def test_caller_saved_copy_does_not_cross_call(self):
        text = ("  0c: 48 8d 84 24 20 0e 00 00         \tlea\trax, [rsp + 0xe20]\n"
                "  14: e8 00 00 00 00                  \tcall\t0x19\n"
                "  19: 48 89 c2                        \tmov\trdx, rax\n"
                "  1c: 48 8d 84 24 40 0e 00 00         \tlea\trax, [rsp + 0xe40]\n"
                "  24: e8 00 00 00 00                  \tcall\t0x29\n")
        ins = q.parse_objdump(text)
        r = q.analyze_callsite(ins, set(), 0x24, ins)
        # two writers of rax in the function and the nearest in-block one precedes a call: unresolved
        self.assertIsNone(r["arg3_slot"])
        self.assertEqual(r["register_copy_chains"]["rdx"]["resolution"], "unresolved")

    def test_function_unique_writer_fallback_and_struct_stores(self):
        text = ("  08: 48 89 fb                        \tmov\trbx, rdi\n"
                "  0b: 90                              \tnop\n"
                "  0c: 85 c0                           \ttest\teax, eax\n"
                "  0e: 74 02                           \tje\t0x12\n"
                "  10: 90                              \tnop\n"
                "  12: c7 43 1c 05 00 00 00            \tmov\tdword ptr [rbx + 0x1c], 0x5\n"
                "  19: 48 89 de                        \tmov\trsi, rbx\n"
                "  1c: e8 00 00 00 00                  \tcall\t0x21\n")
        ins = q.parse_objdump(text)
        r = q.analyze_callsite(ins, q.branch_targets(ins), 0x1C, ins)
        self.assertEqual(r["register_copy_chains"]["rsi"]["resolution"], "function-unique-writer")
        self.assertIsNone(r["arg2_slot"])  # rbx is a copy of rdi, not a stack slot: not claimed

    def test_sign_check_tolerates_interposed_non_eax_mov(self):
        text = ("  10: e8 00 00 00 00                  \tcall\t0x15\n"
                "  15: 41 be ff ff ff ff               \tmov\tr14d, 0xffffffff\n"
                "  1b: 85 c0                           \ttest\teax, eax\n"
                "  1d: 0f 88 00 00 00 00               \tjs\t0x23\n")
        ins = q.parse_objdump(text)
        self.assertTrue(q.post_call_facts(ins, 0, None)["eax_sign_checked_immediately"])
        text2 = text.replace("41 be ff ff ff ff               \tmov\tr14d, 0xffffffff",
                             "b8 01 00 00 00                  \tmov\teax, 0x1")
        ins2 = q.parse_objdump(text2)
        self.assertFalse(q.post_call_facts(ins2, 0, None)["eax_sign_checked_immediately"])

    def test_xor_zeroing_and_zero_width_regs(self):
        text = ("  10: 31 f6                           \txor\tesi, esi\n"
                "  12: e8 00 00 00 00                  \tcall\t0x17\n")
        ins = q.parse_objdump(text)
        r = q.analyze_callsite(ins, set(), 0x12, ins)
        self.assertEqual(r["args"]["rsi"]["kind"], "zeroing")


if __name__ == "__main__":
    unittest.main()
