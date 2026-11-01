#!/usr/bin/env python3
"""Strict, bounded offline GFX7 instruction inventory; never execute shader code.

Encoding facts: AMD Sea Islands ISA 1.3 (December 2013), chapters 6/7/11/13:
https://docs.amd.com/v/u/en-US/sea-islands-instruction-set-architecture_0
This is an original implementation from the ISA, with opcode values cross-checked
against a pinned reference table. It contains no game bytes or instruction list.
Only the explicitly listed opcodes are supported. Other valid GFX7 instructions
are deliberately unsupported. Decoding success does not establish Metal semantics.

Input must be an exact little-endian program span, with one final s_endpgm.
Unknown opcodes, unsupported encodings/operands, reserved fields and truncations
raise DecodeError; no resynchronization or skipped dwords. Listings/JSON from
private inputs are private derivative evidence and must stay outside the repo.
"""
from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
import json
from pathlib import Path
import struct
import sys

ISA_URL = 'https://docs.amd.com/v/u/en-US/sea-islands-instruction-set-architecture_0'
VERSION = 'gfx7-bounded-inventory-v9'

# These are encoding facts, not a translation/semantic implementation.
# SOP2/SOPC/VOPC rows below are opcode-value facts from the GFX7 ISA tables,
# cross-checked against the pinned `reference/` decoder. Values the ISA marks reserved
# (SOP2 12/13) and control-transfer/VI-only rows are not listed, so they still reject:
# s_cbranch_g_fork (SOP2 43), s_setvskip (SOPC 16), s_memtime / s_dcache_inv (SMRD 29..31)
# carry no destination or rely on later ISAs. SOPP 5 `s_cbranch_scc1` is a branch for the
# same reason: a linear span decoder cannot state where control resumes, so control transfer
# stays out of the table (only s_endpgm/s_waitcnt/s_ttracedata are SOPP rows).
OPCODES = {
    'SOP1': {3: 's_mov_b32', 4: 's_mov_b64', 10: 's_wqm_b64',
             36: 's_and_saveexec_b64'},
    # SOPK: immediate scalar moves/compares. Exact opcode names from the pinned PS4 SOPK table
    # (InstructionFormatSOPK in reference/shadPS4 .../format.cpp).
    'SOPK': {0: 's_movk_i32', 2: 's_cmovk_i32',
             3: 's_cmpk_eq_i32', 4: 's_cmpk_lg_i32', 5: 's_cmpk_gt_i32',
             6: 's_cmpk_ge_i32', 7: 's_cmpk_lt_i32', 8: 's_cmpk_le_i32',
             9: 's_cmpk_eq_u32', 10: 's_cmpk_lg_u32', 11: 's_cmpk_gt_u32',
             12: 's_cmpk_ge_u32', 13: 's_cmpk_lt_u32', 14: 's_cmpk_le_u32',
             15: 's_addk_i32', 16: 's_mulk_i32'},
    'SOP2': {0: 's_add_u32', 1: 's_sub_u32', 2: 's_add_i32', 3: 's_sub_i32',
             4: 's_addc_u32', 5: 's_subb_u32', 6: 's_min_i32', 7: 's_min_u32',
             8: 's_max_i32', 9: 's_max_u32', 10: 's_cselect_b32', 11: 's_cselect_b64',
             14: 's_and_b32', 15: 's_and_b64', 16: 's_or_b32', 17: 's_or_b64',
             18: 's_xor_b32', 19: 's_xor_b64', 20: 's_andn2_b32', 21: 's_andn2_b64',
             22: 's_orn2_b32', 23: 's_orn2_b64', 24: 's_nand_b32', 25: 's_nand_b64',
             26: 's_nor_b32', 27: 's_nor_b64', 28: 's_xnor_b32', 29: 's_xnor_b64',
             30: 's_lshl_b32', 31: 's_lshl_b64', 32: 's_lshr_b32', 33: 's_lshr_b64',
             34: 's_ashr_i32', 35: 's_ashr_i64', 36: 's_bfm_b32', 37: 's_bfm_b64',
             38: 's_mul_i32', 39: 's_bfe_u32', 40: 's_bfe_i32', 41: 's_bfe_u64',
             42: 's_bfe_i64', 44: 's_absdiff_i32'},
    'SOPC': {0: 's_cmp_eq_i32', 1: 's_cmp_lg_i32', 2: 's_cmp_gt_i32',
             3: 's_cmp_ge_i32', 4: 's_cmp_lt_i32', 5: 's_cmp_le_i32',
             6: 's_cmp_eq_u32', 7: 's_cmp_lg_u32', 8: 's_cmp_gt_u32',
             9: 's_cmp_ge_u32', 10: 's_cmp_lt_u32', 11: 's_cmp_le_u32',
             12: 's_bitcmp0_b32', 13: 's_bitcmp1_b32', 14: 's_bitcmp0_b64',
             15: 's_bitcmp1_b64'},
    'SOPP': {1: 's_endpgm', 10: 's_barrier', 12: 's_waitcnt', 22: 's_ttracedata'},
    'SMRD': {0: 's_load_dword', 1: 's_load_dwordx2', 2: 's_load_dwordx4',
             3: 's_load_dwordx8', 4: 's_load_dwordx16',
             8: 's_buffer_load_dword', 9: 's_buffer_load_dwordx2',
             10: 's_buffer_load_dwordx4', 11: 's_buffer_load_dwordx8',
             12: 's_buffer_load_dwordx16'},
    'VOP1': {1: 'v_mov_b32', 5: 'v_cvt_f32_i32', 6: 'v_cvt_f32_u32',
             8: 'v_cvt_i32_f32', 11: 'v_cvt_f32_f16', 32: 'v_fract_f32',
             36: 'v_floor_f32', 42: 'v_rcp_f32', 46: 'v_rsq_f32',
             51: 'v_sqrt_f32'},
    'VOP2': {0: 'v_cndmask_b32', 3: 'v_add_f32', 4: 'v_sub_f32',
             5: 'v_subrev_f32', 8: 'v_mul_f32', 15: 'v_min_f32',
             16: 'v_max_f32',
             22: 'v_lshrrev_b32', 26: 'v_lshlrev_b32', 27: 'v_and_b32',
             38: 'v_sub_i32',
             31: 'v_mac_f32', 37: 'v_add_i32', 47: 'v_cvt_pkrtz_f16_f32'},
    'VOP3': {1: 'v_cmp_lt_f32', 3: 'v_cmp_le_f32', 4: 'v_cmp_gt_f32',
             6: 'v_cmp_ge_f32', 11: 'v_cmp_ngt_f32', 136: 'v_cmp_class_f32',
             194: 'v_cmp_eq_u32', 197: 'v_cmp_ne_u32',
             256: 'v_cndmask_b32',
             264: 'v_mul_f32', 303: 'v_cvt_pkrtz_f16_f32', 321: 'v_mad_f32',
             322: 'v_mad_i32_i24', 349: 'v_sad_u32', 350: 'v_cvt_pk_u8_f32'},
    'VOPC': {0: 'v_cmp_f_f32', 1: 'v_cmp_lt_f32', 2: 'v_cmp_eq_f32',
             3: 'v_cmp_le_f32', 4: 'v_cmp_gt_f32', 5: 'v_cmp_lg_f32',
             6: 'v_cmp_ge_f32', 7: 'v_cmp_o_f32', 8: 'v_cmp_u_f32',
             9: 'v_cmp_nge_f32', 10: 'v_cmp_nlg_f32', 11: 'v_cmp_ngt_f32',
             12: 'v_cmp_nle_f32', 13: 'v_cmp_neq_f32', 14: 'v_cmp_nlt_f32',
             15: 'v_cmp_t_f32',
             16: 'v_cmpx_f_f32', 17: 'v_cmpx_lt_f32', 18: 'v_cmpx_eq_f32',
             19: 'v_cmpx_le_f32', 20: 'v_cmpx_gt_f32', 21: 'v_cmpx_lg_f32',
             22: 'v_cmpx_ge_f32', 23: 'v_cmpx_o_f32', 24: 'v_cmpx_u_f32',
             25: 'v_cmpx_nge_f32', 26: 'v_cmpx_nlg_f32', 27: 'v_cmpx_ngt_f32',
             28: 'v_cmpx_nle_f32', 29: 'v_cmpx_neq_f32', 30: 'v_cmpx_nlt_f32',
             31: 'v_cmpx_t_f32',
             # Integer compares (the `cmpx` half). Values cross-checked against the pinned
             # shadPS4 frontend table: 208 V_CMPX_F_U32 .. 213 V_CMPX_NE_U32.
             136: 'v_cmp_class_f32',
             192: 'v_cmp_f_u32', 193: 'v_cmp_lt_u32', 194: 'v_cmp_eq_u32',
             195: 'v_cmp_le_u32', 196: 'v_cmp_gt_u32', 197: 'v_cmp_ne_u32',
             198: 'v_cmp_ge_u32', 199: 'v_cmp_t_u32',
             208: 'v_cmpx_f_u32', 209: 'v_cmpx_lt_u32', 210: 'v_cmpx_eq_u32',
             211: 'v_cmpx_le_u32', 212: 'v_cmpx_gt_u32', 213: 'v_cmpx_ne_u32',
             214: 'v_cmpx_ge_u32', 215: 'v_cmpx_t_u32'},
    'VINTRP': {0: 'v_interp_p1_f32', 1: 'v_interp_p2_f32'},
    'MUBUF': {0: 'buffer_load_format_x', 1: 'buffer_load_format_xy',
              2: 'buffer_load_format_xyz',
              3: 'buffer_load_format_xyzw',
              12: 'buffer_load_dword', 13: 'buffer_load_dwordx2',
              14: 'buffer_load_dwordx4',
              15: 'buffer_store_dword', 16: 'buffer_store_dwordx2',
              17: 'buffer_store_dwordx4'},
    'MIMG': {32: 'image_sample'},
    'EXP': {0: 'exp'},
}

# MUBUF component counts per modelled opcode (format loads/stores 1..4, dword forms 1/2/4).
MUBUF_COMPONENTS = {0: 1, 1: 2, 2: 3, 3: 4, 4: 1, 5: 2, 6: 3, 7: 4,
                    12: 1, 13: 2, 14: 4, 15: 1, 16: 2, 17: 4}

# Control-transfer rows that a linear span decoder must refuse: the target is an immediate offset,
# and inventing fall-through would fabricate a program. Names come from the pinned PS4 table in
# reference/shadPS4/src/shader_recompiler/frontend/format.cpp (SOPP block).
CONTROL_TRANSFER_OPCODES = {
    'SOPP': {2: 's_branch', 4: 's_cbranch_scc0', 5: 's_cbranch_scc1',
             6: 's_cbranch_vccz', 7: 's_cbranch_vccnz',
             8: 's_cbranch_execz', 9: 's_cbranch_execnz'},
    'SOP2': {43: 's_cbranch_g_fork'},
    'SOPK': {17: 's_cbranch_i_fork'},
}


# SOP2 operand widths: 64-bit destination, 64-bit first source, 64-bit second source.
# The shift/bitfield forms take 64-bit values but a 32-bit shift/bitfield descriptor, so
# only the boolean ops pair both sources; s_bfm_b64's two sources are both 32-bit.
SOP2_DST64 = frozenset((11, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 41, 42))
SOP2_SRC0_64 = frozenset((11, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 41, 42))
SOP2_SRC1_64 = frozenset((11, 15, 17, 19, 21, 23, 25, 27, 29))
# SOPC opcodes whose first source is a 64-bit pair (the second is the bit index).
SOPC_64BIT_SRC0 = frozenset((14, 15))
# SOP1 rows whose destination and first source are 64-bit pairs (index 36 saves EXEC into
# SDST and masks EXEC with SRC0, so both operands are wide).
SOP1_64BIT = frozenset((4, 10, 36))
# VOP3-encoded VOPC-class compares: the VDST field names the mask destination (VCC or an SGPR
# pair) instead of a vector register, and the modifier fields must be clear. Names and opcode
# values come from the pinned PS4 frontend table.
VOP3_COMPARE = frozenset((1, 3, 4, 6, 11, 136, 194, 197))
# VOPC rows that overwrite EXEC instead of writing the VCC pair (`cmpx`). Everything else in the
# modelled VOPC set writes VCC, including the integer compares at 192-199.
VOPC_WRITES_EXEC = frozenset(range(16, 32)) | frozenset(range(208, 216))

# VOP3 rows that read all three source fields; every other modelled VOP3 row reads two.
VOP3_THREE_SOURCE = frozenset((256, 321, 322, 349, 350))
# VOP3 ABS/CLAMP/OMOD/NEG modulate floating-point results only. On an integer row the ISA
# defines none of them, so a nonzero value is a shape this decoder does not model rather
# than a fold to guess.
VOP3_INTEGER = frozenset((256, 322, 349))
# VOP2 integer add writes its carry out to VCC; the VOP2 form has no SDST field to name it.
VOP2_WRITES_VCC = frozenset((37, 38))
# VOP2 rows whose mask operand is implicit: only `v_cndmask_b32`, which selects with VCC.
VOP2_READS_VCC = frozenset((0,))

# mask, tag, opcode shift, opcode mask, fixed dword count. Specific
# encodings precede VOP2 (which otherwise also matches VOP1/VOPC).
FORMATS = (
    ('SOP1', 0xff800000, 0xbe800000, 8, 255, 1),
    ('SOPC', 0xff800000, 0xbf000000, 16, 127, 1),
    ('SOPP', 0xff800000, 0xbf800000, 16, 127, 1),
    ('SOPK', 0xf0000000, 0xb0000000, 23, 31, 1),
    ('SOP2', 0xc0000000, 0x80000000, 23, 127, 1),
    ('SMRD', 0xf8000000, 0xc0000000, 22, 31, 1),
    ('VINTRP', 0xfc000000, 0xc8000000, 16, 3, 1),
    ('VOP3', 0xfc000000, 0xd0000000, 17, 511, 2),
    ('MUBUF', 0xfc000000, 0xe0000000, 18, 127, 2),
    ('MIMG', 0xfc000000, 0xf0000000, 18, 127, 2),
    ('EXP', 0xfc000000, 0xf8000000, 0, 0, 2),
    ('VOP1', 0xfe000000, 0x7e000000, 9, 255, 1),
    ('VOPC', 0xfe000000, 0x7c000000, 17, 255, 1),
    ('VOP2', 0x80000000, 0, 25, 63, 1),
)

# Fields are (name, lsb, width), across a little-endian 64-bit integer.
FIELDS = {
    'SOP1': (('src0', 0, 8), ('sdst', 16, 7)),
    'SOP2': (('ssrc0', 0, 8), ('ssrc1', 8, 8), ('sdst', 16, 7)),
    'SOPK': (('simm16', 0, 16), ('sdst', 16, 7)),
    'SOPC': (('ssrc0', 0, 8), ('ssrc1', 8, 8)),
    'SOPP': (('simm16', 0, 16),),
    'SMRD': (('offset', 0, 8), ('imm', 8, 1), ('sbase', 9, 6), ('sdst', 15, 7)),
    'VOP1': (('src0', 0, 9), ('vdst', 17, 8)),
    'VOP2': (('src0', 0, 9), ('vsrc1', 9, 8), ('vdst', 17, 8)),
    'VOPC': (('src0', 0, 9), ('vsrc1', 9, 8)),
    'VOP3': (('vdst', 0, 8), ('abs', 8, 3), ('clamp', 11, 1),
             ('src0', 32, 9), ('src1', 41, 9), ('src2', 50, 9),
             ('omod', 59, 2), ('neg', 61, 3)),
    'VINTRP': (('vsrc', 0, 8), ('attrchan', 8, 2), ('attr', 10, 6), ('vdst', 18, 8)),
    'MUBUF': (('offset', 0, 12), ('offen', 12, 1), ('idxen', 13, 1), ('glc', 14, 1),
              ('addr64', 15, 1), ('lds', 16, 1), ('vaddr', 32, 8), ('vdata', 40, 8),
              ('srsrc', 48, 5), ('slc', 54, 1), ('tfe', 55, 1), ('soffset', 56, 8)),
    'MIMG': (('dmask', 8, 4), ('unorm', 12, 1), ('glc', 13, 1), ('da', 14, 1),
             ('r128', 15, 1), ('tfe', 16, 1), ('lwe', 17, 1), ('slc', 25, 1),
             ('vaddr', 32, 8), ('vdata', 40, 8), ('srsrc', 48, 5), ('ssamp', 53, 5)),
    'EXP': (('en', 0, 4), ('target', 4, 6), ('compr', 10, 1), ('done', 11, 1),
            ('vm', 12, 1), ('vsrc0', 32, 8), ('vsrc1', 40, 8),
            ('vsrc2', 48, 8), ('vsrc3', 56, 8)),
}


class DecodeError(ValueError):
    """A visible, position-specific failure; never a partially accepted program."""

    def __init__(self, offset: int, reason: str):
        self.offset = offset
        self.reason = reason
        super().__init__(f'at byte {offset:#x}: {reason}')


@dataclass(frozen=True)
class Instruction:
    offset: int
    size: int
    encoding: str
    opcode: int
    mnemonic: str
    fields: dict[str, int]
    requirements: tuple[str, ...]


def _register(value: int) -> bool:
    # Flat scratch selectors 104/105 are present in ISA table 6.1 even though
    # chapter 13 labels them reserved. They are excluded from this bounded subset.
    return 0 <= value <= 103 or 106 <= value <= 124 or value in (126, 127)


def _source(value: int, *, vector: bool = False, literal: bool = True) -> bool:
    return (_register(value) or 128 <= value <= 208 or 240 <= value <= 247
            or value in (251, 252, 253) or (literal and value == 255)
            or (vector and (value == 254 or 256 <= value <= 511)))


def _literals(form: str, word: int) -> tuple[str, ...]:
    """Which operand fields select an inline literal; one extra dword follows per instruction.

    The ISA source-operand table makes selector 255 read the literal in the next word.
    SOP2/SOPC have two 8-bit sources, so only one of them may select it; SMRD uses the same
    selector in its OFFSET field when IMM=0 (register/literal offset form), which the pinned
    reference decoder also reads as an extra literal dword.
    """
    if form == 'SOP1':
        return ('src0',) if word & 255 == 255 else ()
    if form in ('VOP1', 'VOP2', 'VOPC'):
        return ('src0',) if word & 511 == 255 else ()
    if form in ('SOP2', 'SOPC'):
        names = ('ssrc0',) if word & 255 == 255 else ()
        return names + (('ssrc1',) if (word >> 8) & 255 == 255 else ())
    if form == 'SMRD' and (word & 0x100) == 0:
        return ('offset',) if word & 255 == 255 else ()
    return ()


def _validate(form: str, op: int, f: dict[str, int], at: int) -> tuple[str, ...]:
    def require(ok: bool, reason: str) -> None:
        if not ok:
            raise DecodeError(at, reason)

    requirements: set[str] = set()
    if form == 'SOP1':
        require(_register(f['sdst']), 'unsupported scalar destination')
        require(_source(f['src0']), 'reserved/unsupported scalar source')
        if op in SOP1_64BIT:
            require(f['sdst'] % 2 == 0 and f['sdst'] != 124, 'invalid 64-bit destination pair')
            if f['src0'] < 128:
                require(f['src0'] % 2 == 0 and f['src0'] != 124, 'invalid 64-bit source pair')
        if op == 10:
            requirements.add('whole_quad_mode')
        if op == 36:
            requirements.update(('reads_exec_mask', 'writes_exec_mask'))
        if f['sdst'] in (126, 127):
            requirements.add('writes_exec_mask')
        if f['src0'] in (126, 127, 252):
            requirements.add('reads_exec_mask')
        if f['src0'] in (106, 107, 251):
            requirements.add('reads_vcc_mask')
    elif form == 'SOP2':
        require(_register(f['sdst']), 'unsupported scalar destination')
        for name in ('ssrc0', 'ssrc1'):
            require(_source(f[name]), 'reserved/unsupported scalar source')
        if op in SOP2_DST64:
            require(f['sdst'] % 2 == 0 and f['sdst'] != 124, 'invalid 64-bit destination pair')
        for name, wide in (('ssrc0', op in SOP2_SRC0_64), ('ssrc1', op in SOP2_SRC1_64)):
            if wide and f[name] < 128:
                require(f[name] % 2 == 0 and f[name] != 124, 'invalid 64-bit source pair')
        if f['sdst'] in (126, 127):
            requirements.add('writes_exec_mask')
        for name in ('ssrc0', 'ssrc1'):
            if f[name] in (126, 127, 252):
                requirements.add('reads_exec_mask')
            if f[name] in (106, 107, 251):
                requirements.add('reads_vcc_mask')
    elif form == 'SOPC':
        for name in ('ssrc0', 'ssrc1'):
            require(_source(f[name]), 'reserved/unsupported scalar source')
        if op in SOPC_64BIT_SRC0 and f['ssrc0'] < 128:
            require(f['ssrc0'] % 2 == 0 and f['ssrc0'] != 124, 'invalid 64-bit source pair')
        for name in ('ssrc0', 'ssrc1'):
            if f[name] in (126, 127, 252):
                requirements.add('reads_exec_mask')
            if f[name] in (106, 107, 251):
                requirements.add('reads_vcc_mask')
        requirements.add('writes_scc')
    elif form == 'SOPK':
        require(_register(f['sdst']), 'unsupported scalar destination')
        if 3 <= op <= 14 or op == 15:
            requirements.add('writes_scc')
        if op == 2:
            requirements.add('reads_scc')
    elif form == 'SOPP':
        if op == 1:
            require(f['simm16'] == 0, 'unsupported nonzero s_endpgm immediate')
        elif op == 10:
            require(f['simm16'] == 0, 'unsupported nonzero s_barrier immediate')
            requirements.add('wave_barrier')

        elif op == 12:
            require(f['simm16'] & ~0x1f7f == 0, 'reserved s_waitcnt bits')
            f.update(vmcnt=f['simm16'] & 15, expcnt=(f['simm16'] >> 4) & 7,
                     lgkmcnt=(f['simm16'] >> 8) & 31)
        else:
            require(f['simm16'] == 0, 'unsupported s_ttracedata immediate')
            requirements.add('thread_trace_marker')
    elif form == 'SMRD':
        count = 1 << (op - 8 if op >= 8 else op)
        f.update(dwords=count, base_sgpr=f['sbase'] * 2)
        require(f['sdst'] <= 103 and f['sdst'] + count <= 104,
                'unsupported SMRD destination range')
        require(f['sdst'] % min(count, 4) == 0, 'misaligned SMRD destination')
        require(f['base_sgpr'] + (4 if op >= 8 else 2) <= 104,
                'unsupported SMRD resource/base range')
        if op >= 8:
            require(f['base_sgpr'] % 4 == 0, 'misaligned buffer descriptor')
        if not f['imm']:
            # IMM=0 makes OFFSET a 32-bit scalar operand: an SGPR index, or selector 255
            # for the inline literal that immediately follows (size already accounts for
            # it). Other selectors are not modelled and keep the old rejection text.
            require(f['offset'] <= 103 or f['offset'] == 255,
                    'unsupported SMRD register/literal offset')
        requirements.add('scalar_memory')
    elif form in ('VOP1', 'VOP2', 'VOP3'):
        names = ('src0', 'src1', 'src2') if form == 'VOP3' else ('src0',)
        for name in names:
            require(_source(f[name], vector=True, literal=form != 'VOP3'),
                    'reserved/unsupported vector source')
            if f[name] == 254:
                require(name == 'src0', 'LDS direct can only supply src0')
                requirements.add('lds_direct')
            if f[name] in (126, 127, 252):
                requirements.add('reads_exec_mask')
            if f[name] in (106, 107, 251):
                requirements.add('reads_vcc_mask')
        if form == 'VOP3':
            used = names if op in VOP3_THREE_SOURCE else names[:2]
            if op == 256:
                mask = f['src2']
                require(mask < 104 or mask in (106, 107, 126, 127),
                        'cndmask mask must be a scalar register or VCC/EXEC')
                if mask in (106, 107):
                    requirements.add('reads_vcc_mask')
                if mask in (126, 127):
                    requirements.add('reads_exec_mask')
            if op in VOP3_COMPARE:
                require(not (f['abs'] or f['clamp'] or f['omod'] or f['neg']),
                        'modifier field set on a compare row')
                if f['vdst'] in (106, 107):
                    requirements.add('writes_vcc_mask')
                else:
                    require(f['vdst'] % 2 == 0 and f['vdst'] != 124,
                            'invalid compare mask destination pair')
            if op in VOP3_INTEGER:
                require(not (f['abs'] or f['clamp'] or f['omod'] or f['neg']),
                        'floating-point modifier field set on an integer VOP3 instruction')
            # GFX7's scalar read-port restriction applies to the actual operands.
            sgprs = {f[n] for n in used if f[n] < 128}
            require(len(sgprs) <= 1, 'multiple distinct scalar sources in VOP3')
            # Unused source/control bits are retained in the inventory; they are
            # not reinterpreted as an instruction or discarded as extra dwords.
        if form == 'VOP2' and op in VOP2_READS_VCC:
            requirements.add('reads_vcc_mask')
        if form == 'VOP2' and op in VOP2_WRITES_VCC:
            requirements.add('writes_vcc_mask')
        if (op == 47 and form == 'VOP2') or (op == 303 and form == 'VOP3'):
            requirements.add('fp16_round_toward_zero')
    elif form == 'VOPC':
        # VSRC0 is the 9-bit vector source; VSRC1 is a plain 8-bit VGPR number and there is
        # no VDST field. Opcodes 0-15 write the VCC pair, 16-31 (`cmpx`) overwrite EXEC.
        require(_source(f['src0'], vector=True), 'reserved/unsupported vector source')
        if f['src0'] == 254:
            requirements.add('lds_direct')
        if f['src0'] in (126, 127, 252):
            requirements.add('reads_exec_mask')
        if f['src0'] in (106, 107, 251):
            requirements.add('reads_vcc_mask')
        if op in VOPC_WRITES_EXEC:
            requirements.add('writes_exec_mask')
        else:
            requirements.add('writes_vcc_mask')
    elif form == 'VINTRP':
        requirements.add('fixed_function_interpolation')
    elif form == 'MUBUF':
        # Format loads/stores take 1..4 components from the opcode; the dword forms use the
        # explicit sizes GFX7 defines (dword 1, x2 2, x4 4). There is no dwordx3 on this ISA.
        components = MUBUF_COMPONENTS.get(op)
        require(components is not None, 'unsupported MUBUF opcode width')
        f.update(resource_sgpr=f['srsrc'] * 4, components=components)
        require(f['resource_sgpr'] + 4 <= 104, 'unsupported buffer descriptor range')
        require(_source(f['soffset'], literal=False), 'unsupported buffer scalar offset')
        require(f['vdata'] + components <= 256, 'buffer destination range overflows VGPRs')
        require(not ((f['offen'] and f['idxen'] or f['addr64']) and f['vaddr'] == 255),
                'buffer address pair overflows VGPRs')
        if f['lds']:
            requirements.add('lds_memory')
        requirements.add('formatted_vertex_fetch')
    elif form == 'MIMG':
        require(f['dmask'] != 0, 'image dmask must not be zero')
        f.update(resource_sgpr=f['srsrc'] * 4, sampler_sgpr=f['ssamp'] * 4)
        require(f['resource_sgpr'] + (4 if f['r128'] else 8) <= 104,
                'unsupported image descriptor range')
        require(f['sampler_sgpr'] + 4 <= 104, 'unsupported sampler range')
        require(f['vdata'] + f['dmask'].bit_count() + f['tfe'] <= 256,
                'image destination range overflows VGPRs')
        requirements.update(('texture_sample', 'implicit_quad_derivatives'))
    elif form == 'EXP':
        t = f['target']
        require(t <= 9 or 12 <= t <= 15 or 32 <= t <= 63, 'reserved export target')
        if f['compr']:
            require(f['en'] in (0, 3, 12, 15), 'invalid compressed export enable')
        requirements.add('export')
    return tuple(sorted(requirements))


def decode(code: bytes, *, allow_branches: bool = False) -> list[Instruction]:
    """Decode the entire exact program or raise; s_endpgm must be last.

    `allow_branches=True` admits control-transfer rows as explicit one-dword `Instruction`s tagged
    `requirements=('control_transfer',)`. It does **not** imply fall-through: the returned list is a
    listing, and every consumer that walks it must treat a control-transfer row as a block
    terminator (`gcn_blocks.blocks()` does exactly that). The default stays strict.
    """
    if not code or len(code) % 4:
        raise DecodeError(0, 'program must be a nonempty multiple of four bytes')
    instructions: list[Instruction] = []
    offset = 0
    while offset < len(code):
        word = struct.unpack_from('<I', code, offset)[0]
        spec = next((s for s in FORMATS if word & s[1] == s[2]), None)
        if spec is None:
            raise DecodeError(offset, 'unsupported encoding')
        form, mask, tag, shift, opmask, count = spec
        op = (word >> shift) & opmask
        mnemonic = OPCODES[form].get(op)
        if mnemonic is None:
            if form in CONTROL_TRANSFER_OPCODES and op in CONTROL_TRANSFER_OPCODES[form]:
                name = CONTROL_TRANSFER_OPCODES[form][op]
                if not allow_branches:
                    raise DecodeError(offset,
                                      f'unsupported control transfer {form} opcode {op} ({name})')
                if offset + 4 > len(code):
                    raise DecodeError(offset, f'truncated {form} branch')
                instructions.append(Instruction(
                    offset=offset, size=4, encoding=form, opcode=op, mnemonic=name,
                    fields={'simm16': word & 0xffff}, requirements=('control_transfer',)))
                offset += 4
                continue
            raise DecodeError(offset, f'unsupported {form} opcode {op}')
        literals = _literals(form, word)
        if len(literals) > 1:
            # One inline-literal dword per instruction, so two selectors are ambiguous.
            raise DecodeError(offset, f'multiple inline literals in one {form} instruction')
        has_literal = bool(literals)
        size = (count + int(has_literal)) * 4
        if offset + size > len(code):
            raise DecodeError(offset, f'truncated {form} instruction/literal ({size} bytes required)')
        encoded = int.from_bytes(code[offset:offset + count * 4], 'little')
        fields = {n: (encoded >> lo) & ((1 << width) - 1) for n, lo, width in FIELDS[form]}
        defined = mask | (opmask << shift)
        for _, lo, width in FIELDS[form]:
            defined |= ((1 << width) - 1) << lo
        if encoded & ~defined:
            raise DecodeError(offset, f'nonzero reserved {form} bits')
        if has_literal:
            fields['literal'] = struct.unpack_from('<I', code, offset + 4)[0]
        requirements = _validate(form, op, fields, offset)
        instructions.append(Instruction(offset, size, form, op, mnemonic, fields, requirements))
        offset += size
        if mnemonic == 's_endpgm' and offset != len(code):
            raise DecodeError(offset, 'bytes follow s_endpgm inside the declared program')
    if instructions[-1].mnemonic != 's_endpgm':
        raise DecodeError(offset, 'program does not end with s_endpgm')
    return instructions


PER_LANE_BLOCKERS = frozenset(('whole_quad_mode', 'writes_exec_mask', 'reads_exec_mask',
                               'reads_vcc_mask', 'lds_direct', 'wave_barrier',
                               'lds_memory', 'implicit_quad_derivatives'))


def inventory(code: bytes) -> dict:
    ins = decode(code)
    blockers = [dict(offset=i.offset, mnemonic=i.mnemonic,
                     reasons=sorted(PER_LANE_BLOCKERS.intersection(i.requirements)))
                for i in ins if PER_LANE_BLOCKERS.intersection(i.requirements)]
    return dict(version=VERSION, isa=ISA_URL, sha256=hashlib.sha256(code).hexdigest(),
                bytes=len(code), consumed_bytes=sum(i.size for i in ins),
                instruction_count=len(ins), unknown_count=0, exact_endpgm=True,
                encodings=dict(sorted(Counter(i.encoding for i in ins).items())),
                opcodes=dict(sorted(Counter(i.mnemonic for i in ins).items())),
                requirements=dict(sorted(Counter(r for i in ins for r in i.requirements).items())),
                per_lane_eligible=not blockers, per_lane_blockers=blockers,
                instructions=[asdict(i) for i in ins])


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('program', type=Path, help='exact code span; private input stays private')
    p.add_argument('--json', action='store_true', help='include mnemonic/field inventory in JSON')
    p.add_argument('--summary-only', action='store_true', help='omit per-instruction listing')
    p.add_argument('--require-per-lane', action='store_true', help='fail on quad/wave/LDS requirements')
    args = p.parse_args(argv)
    try:
        result = inventory(args.program.read_bytes())
    except (OSError, DecodeError) as exc:
        print(f'FAILED: {exc}', file=sys.stderr)
        return 1
    if args.summary_only:
        result.pop('instructions')
    if args.json:
        print(json.dumps(result, indent=2))
    else:
        for row in result.get('instructions', []):
            print(f"{row['offset']:04x} +{row['size']}: {row['mnemonic']} {row['fields']}")
        print(json.dumps({k: v for k, v in result.items() if k != 'instructions'}, indent=2))
    if args.require_per_lane and not result['per_lane_eligible']:
        print('FAILED: instruction inventory requires quad/wave/LDS semantics', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
