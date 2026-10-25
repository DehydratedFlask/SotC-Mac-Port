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
VERSION = 'gfx7-bounded-inventory-v1'

# These are encoding facts, not a translation/semantic implementation.
OPCODES = {
    'SOP1': {3: 's_mov_b32', 4: 's_mov_b64', 10: 's_wqm_b64'},
    'SOPP': {1: 's_endpgm', 12: 's_waitcnt', 22: 's_ttracedata'},
    'SMRD': {0: 's_load_dword', 1: 's_load_dwordx2', 2: 's_load_dwordx4',
             3: 's_load_dwordx8', 4: 's_load_dwordx16',
             10: 's_buffer_load_dwordx4', 11: 's_buffer_load_dwordx8'},
    'VOP1': {1: 'v_mov_b32', 46: 'v_rsq_f32'},
    'VOP2': {4: 'v_sub_f32', 8: 'v_mul_f32', 31: 'v_mac_f32',
             47: 'v_cvt_pkrtz_f16_f32'},
    'VOP3': {264: 'v_mul_f32', 303: 'v_cvt_pkrtz_f16_f32', 321: 'v_mad_f32'},
    'VINTRP': {0: 'v_interp_p1_f32', 1: 'v_interp_p2_f32'},
    'MUBUF': {1: 'buffer_load_format_xy', 2: 'buffer_load_format_xyz',
              3: 'buffer_load_format_xyzw'},
    'MIMG': {32: 'image_sample'},
    'EXP': {0: 'exp'},
}

# mask, tag, opcode shift, opcode mask, fixed dword count. Specific
# encodings precede VOP2 (which otherwise also matches VOP1/VOPC).
FORMATS = (
    ('SOP1', 0xff800000, 0xbe800000, 8, 255, 1),
    ('SOPP', 0xff800000, 0xbf800000, 16, 127, 1),
    ('SMRD', 0xf8000000, 0xc0000000, 22, 31, 1),
    ('VINTRP', 0xfc000000, 0xc8000000, 16, 3, 1),
    ('VOP3', 0xfc000000, 0xd0000000, 17, 511, 2),
    ('MUBUF', 0xfc000000, 0xe0000000, 18, 127, 2),
    ('MIMG', 0xfc000000, 0xf0000000, 18, 127, 2),
    ('EXP', 0xfc000000, 0xf8000000, 0, 0, 2),
    ('VOP1', 0xfe000000, 0x7e000000, 9, 255, 1),
    ('VOP2', 0x80000000, 0, 25, 63, 1),
)

# Fields are (name, lsb, width), across a little-endian 64-bit integer.
FIELDS = {
    'SOP1': (('src0', 0, 8), ('sdst', 16, 7)),
    'SOPP': (('simm16', 0, 16),),
    'SMRD': (('offset', 0, 8), ('imm', 8, 1), ('sbase', 9, 6), ('sdst', 15, 7)),
    'VOP1': (('src0', 0, 9), ('vdst', 17, 8)),
    'VOP2': (('src0', 0, 9), ('vsrc1', 9, 8), ('vdst', 17, 8)),
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


def _validate(form: str, op: int, f: dict[str, int], at: int) -> tuple[str, ...]:
    def require(ok: bool, reason: str) -> None:
        if not ok:
            raise DecodeError(at, reason)

    requirements: set[str] = set()
    if form == 'SOP1':
        require(_register(f['sdst']), 'unsupported scalar destination')
        require(_source(f['src0']), 'reserved/unsupported scalar source')
        if op in (4, 10):
            require(f['sdst'] % 2 == 0 and f['sdst'] != 124, 'invalid 64-bit destination pair')
            if f['src0'] < 128:
                require(f['src0'] % 2 == 0 and f['src0'] != 124, 'invalid 64-bit source pair')
        if op == 10:
            requirements.add('whole_quad_mode')
        if f['sdst'] in (126, 127):
            requirements.add('writes_exec_mask')
        if f['src0'] in (126, 127, 252):
            requirements.add('reads_exec_mask')
        if f['src0'] in (106, 107, 251):
            requirements.add('reads_vcc_mask')
    elif form == 'SOPP':
        if op == 1:
            require(f['simm16'] == 0, 'unsupported nonzero s_endpgm immediate')
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
            # Literal/register-offset SMRD is deliberately not part of this
            # bounded reader; it must never silently acquire the wrong width.
            require(0 <= f['offset'] <= 103, 'unsupported SMRD register/literal offset')
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
            used = names if op == 321 else names[:2]
            # GFX7's scalar read-port restriction applies to the actual operands.
            sgprs = {f[n] for n in used if f[n] < 128}
            require(len(sgprs) <= 1, 'multiple distinct scalar sources in VOP3')
            # Unused source/control bits are retained in the inventory; they are
            # not reinterpreted as an instruction or discarded as extra dwords.
        if (op == 47 and form == 'VOP2') or (op == 303 and form == 'VOP3'):
            requirements.add('fp16_round_toward_zero')
    elif form == 'VINTRP':
        requirements.add('fixed_function_interpolation')
    elif form == 'MUBUF':
        f.update(resource_sgpr=f['srsrc'] * 4, components=op + 1)
        require(f['resource_sgpr'] + 4 <= 104, 'unsupported buffer descriptor range')
        require(_source(f['soffset'], literal=False), 'unsupported buffer scalar offset')
        require(f['vdata'] + op + 1 <= 256, 'buffer destination range overflows VGPRs')
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


def decode(code: bytes) -> list[Instruction]:
    """Decode the entire exact program or raise; s_endpgm must be last."""
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
            raise DecodeError(offset, f'unsupported {form} opcode {op}')
        has_literal = (form == 'SOP1' and word & 255 == 255
                       or form in ('VOP1', 'VOP2') and word & 511 == 255)
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
                               'reads_vcc_mask', 'lds_direct',
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
