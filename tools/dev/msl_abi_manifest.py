#!/usr/bin/env python3
"""Per-shader ABI manifest for the offline GCN -> MSL path: schema, hole generator, validator.

`gcn_to_msl.py` refuses every program without an ABI manifest, and no retail `.csdr` member has
one, so lowering stands at **0 of 34,291**. The reason is measurable rather than vague: a shader
names register *slots* (`SMRD` `sbase`, `MUBUF` `srsrc`, `EXP` `target`) and never says which
buffer or memory each slot addresses. That mapping lives in the draw state.

This tool does not invent it. It turns a decoded program into the exact form that state has to
fill:

  --skeleton   emit `msl-abi-manifest-v1` with every required binding present as `null`
  --json       the measured hole list (slots, kinds, formats, export targets, view dwords, policy)
  --manifest   validate a filled manifest against the same decoded program, per slot
  --census     measure the hole list over a deterministic corpus sample (private output only)

It decodes and counts. It never lowers, never compiles, never executes and never guesses a
binding, so nothing here moves `lowered` off zero by itself.

Schema and measurement: `docs/MSL_OFFLINE.md`, section "ABI manifests: the schema, the
holes, and why `lowered` is still 0".

  python3 -I tools/dev/msl_abi_manifest.py --self-test
  python3 -I tools/dev/msl_abi_manifest.py --program span.bin --stage vs --json
  python3 -I tools/dev/msl_abi_manifest.py --program span.bin --stage vs --skeleton form.json
  python3 -I tools/dev/msl_abi_manifest.py --program span.bin --manifest filled.json
  python3 -I tools/dev/msl_abi_manifest.py --census --sample 512 --top 20 \\
      --out local/validation/msl-abi-holes
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]

_SPEC = importlib.util.spec_from_file_location('gcn_inventory', HERE / 'gcn_inventory.py')
gcn = importlib.util.module_from_spec(_SPEC)
sys.modules['gcn_inventory'] = gcn
_SPEC.loader.exec_module(gcn)

SCHEMA = 'msl-abi-manifest-v1'
ANALYSIS_SCHEMA = 'msl-abi-holes-v1'
CENSUS_SCHEMA = 'msl-abi-hole-census-v1'
STAGES = ('vs', 'ps')
# The *named* sink in `gcn_to_msl.Lowerer` (the closed synthetic harness profile) demands exactly
# this target set. It is a harness convenience, not a shader fact, so it is **not** the contract
# this analyzer measures: the `target-array` sink accepts any set of modelled targets. What remains
# a stage contract is that a vertex stage must define a full clip position.
HARNESS_EXPORT_CONTRACT = {'vs': (12, 32, 33), 'ps': (0,)}
# `EXP` target spaces, in the pinned `IR::Attribute` numbering (RenderTarget0..7, Depth 8,
# Null 9, Position0..3 12..15, Param0..31 32..63). The decoder already rejects other targets.
EXPORT_SPACES = (('rt', 0, 8), ('depth', 8, 9), ('pos', 12, 16), ('param', 32, 64))
NULL_EXPORT_TARGET = 9
CLIP_TARGET = 12
IDENTIFIER = re.compile(r'[a-z][a-z0-9_]*')
SHA256 = re.compile(r'[0-9a-f]{64}')
# Mirrors `gcn_to_msl.CASE_FIELDS` (the only names `Lowerer.bind()` accepts for `float4`/`float`).
# `--self-test` imports that module and fails if the two drift apart.
CASE_FIELDS = ('position', 'encoded_normal', 'object_row0', 'object_row1', 'object_row2',
               'clip_row0', 'clip_row1', 'clip_row2', 'clip_row3', 'camera', 'material',
               'param0_p0', 'param0_p10', 'param0_p20', 'param1_p0', 'param1_p10', 'param1_p20',
               'barycentric')
# Register selectors that are not ordinary SGPR file slots: 104/105 flat scratch, 106/107 VCC,
# 125/126/127 EXEC-class. A program that *reads* one of those is a per-lane blocker anyway
# (`reads_vcc_mask`/`reads_exec_mask`), so no hole is emitted for it.
MASK_SELECTORS = frozenset((104, 105, 106, 107, 125, 126, 127))
# `Lowerer.arithmetic` computes these rows. A manifest cannot add a row, so a program containing
# any other arithmetic mnemonic is shape-blocked, not binding-blocked, and is reported as such.
LOWERED_ARITHMETIC = frozenset(('v_mov_b32', 'v_add_f32', 'v_sub_f32', 'v_mul_f32',
                                'v_mac_f32', 'v_mad_f32', 'v_rsq_f32',
                                'v_cvt_pkrtz_f16_f32', 'v_cvt_f32_f16', 'v_cvt_f32_i32',
                                'v_and_b32', 'v_add_i32', 'v_sub_i32',
                                'v_lshlrev_b32', 'v_lshrrev_b32',
                                'v_mad_u32_u24', 'v_sad_u32'))
BINDING_WIDTH = {'pointer': 2, 'descriptor': 4, 'float4': 4, 'float': 1,
                 'vertex_index': 1, 'interpolation_control': 1}
BINDING_KINDS = tuple(BINDING_WIDTH)
# `Lowerer.__init__` requires exactly one cell per `initial_vector` entry.
SINGLE_CELL_KINDS = ('float', 'vertex_index', 'interpolation_control')
VIEW_BINDING_KINDS = ('pointer', 'descriptor', 'float4', 'float')
# Register/target namespaces: a scalar slot number and an export target number are not
# comparable, so cross-section diagnostics stay inside one namespace.
NAMESPACE = {'scalar_bases': 'sgpr', 'buffer_descriptors': 'sgpr',
             'initial_scalars': 'sgpr', 'initial_vectors': 'vgpr', 'exports': 'target'}
MANIFEST_KEYS = ('schema', 'shader', 'provenance', 'scalar_bases', 'buffer_descriptors',
                 'exports', 'initial_scalars', 'initial_vectors', 'views', 'policy')
SHADER_KEYS = ('id', 'stage', 'sha256', 'bytes')
POLICY_KEYS = ('floating_point', 'interpolation', 'vertex_select', 'vertex_mov_subtract_p10')
INTERPOLATION_MODES = ('supplied_float_coefficients', 'supplied_vertex_values')
FLOATING_POINT_PROFILES = ('nearest_even_ftz32_rtz16',)
VERTEX_SELECT_TARGETS = ('p10', 'p20', 'p0')


class AbiError(ValueError):
    """A refused manifest or analysis input; never a partially accepted document."""


def _hex(offset: int) -> str:
    return f'0x{offset:x}'


def _is_sgpr(value: int) -> bool:
    return 0 <= value <= 103 or 108 <= value <= 124


# --------------------------------------------------------------------------------------------
# Analysis: what a decoded program demands from a manifest.
# --------------------------------------------------------------------------------------------

class _Analysis:
    """Accumulate the manifest holes of one decoded program.

    Reads are taken from the *encoding*, not from the lowering's supported subset: a register the
    program reads before writing it has to be bound whether or not the current lowerer can express
    the instruction that reads it. The subset limits are collected separately as `shape` blockers,
    because no manifest can satisfy them.
    """

    def __init__(self, stage: str | None):
        self.stage = stage
        self.scalar_defined: dict[int, str] = {}
        self.vector_defined: dict[int, str] = {}
        # Which instruction wrote each scalar cell, so a base/descriptor that is *loaded* from
        # memory can be told apart from one that is computed (and therefore not bindable at all).
        self.writer: dict[int, tuple] = {}
        self.loads: list[dict] = []
        self.bases: dict[int, dict] = {}
        self.descriptors: dict[int, dict] = {}
        self.exports: dict[int, dict] = {}
        self.null_exports: list[dict] = []
        self.valid_mask_exports: list[int] = []
        self.initial_scalars: dict[int, dict] = {}
        self.initial_vectors: dict[int, dict] = {}
        self.policies: dict[str, dict] = {}
        self.shape: list[dict] = []

    # -- diagnostics ---------------------------------------------------------------------
    def blocker(self, row, kind: str, reason: str) -> None:
        self.shape.append({'offset': row.offset, 'mnemonic': row.mnemonic,
                           'kind': kind, 'reason': reason})

    # -- reads ---------------------------------------------------------------------------
    def need_scalar(self, slot: int, row, role: str | None = None) -> None:
        if slot in self.scalar_defined or not _is_sgpr(slot):
            return
        entry = self.initial_scalars.setdefault(
            slot, {'slot': slot, 'sites': [], 'role': None})
        if row.offset not in entry['sites']:
            entry['sites'].append(row.offset)
        if role and entry['role'] is None:
            entry['role'] = role

    def need_vector(self, slot: int, row, role: str | None = None) -> None:
        if slot in self.vector_defined:
            return
        entry = self.initial_vectors.setdefault(
            slot, {'slot': slot, 'sites': [], 'role': None, 'plain_sites': 0})
        if row.offset not in entry['sites']:
            entry['sites'].append(row.offset)
        if role == 'vertex_index':
            entry['role'] = 'vertex_index'
        else:
            entry['plain_sites'] += 1

    def need_source(self, value: int, row) -> None:
        """Read one source selector: VGPR, SGPR, constant, literal or mask."""
        if value >= 256:
            self.need_vector(value - 256, row)
        elif value in MASK_SELECTORS:
            return
        elif _is_sgpr(value):
            self.need_scalar(value, row)

    def need_base(self, slot: int, kind: str, row, dwords: int, offset: int) -> None:
        width = BINDING_WIDTH[kind]
        entry = self.bases.get(slot)
        if entry is None:
            entry = self.bases[slot] = {'slot': slot, 'kind': kind, 'width': width,
                                        'sites': [], 'view_dwords': []}
        elif entry['kind'] != kind:
            self.blocker(row, 'base-kind-conflict',
                         f'scalar base s{slot} is read as both {entry["kind"]} '
                         f'(earlier site {_hex(entry["sites"][0])}) and {kind}')
        if row.offset not in entry['sites']:
            entry['sites'].append(row.offset)
        request = {'offset': offset, 'dwords': dwords}
        if request not in entry['view_dwords']:
            entry['view_dwords'].append(request)

    def need_descriptor(self, slot: int, row, components: int) -> None:
        entry = self.descriptors.get(slot)
        format_name = f'explicit_float{components}'
        if entry is None:
            entry = self.descriptors[slot] = {
                'slot': slot, 'kind': 'descriptor', 'width': BINDING_WIDTH['descriptor'],
                'formats': [], 'sites': []}
        if format_name not in entry['formats']:
            entry['formats'].append(format_name)
        if row.offset not in entry['sites']:
            entry['sites'].append(row.offset)

    # -- per-row model -------------------------------------------------------------------
    def consume(self, row) -> None:
        f, name, enc = row.fields, row.mnemonic, row.encoding
        if name == 's_mov_b32':
            self.need_source(f['src0'], row)
            self.scalar_defined[f['sdst']] = 'derived'
            self.writer[f['sdst']] = ('other', None, name, row.offset)
        elif name in ('s_waitcnt', 's_ttracedata', 's_endpgm'):
            pass
        elif enc == 'SMRD':
            kind = 'descriptor' if name.startswith('s_buffer') else 'pointer'
            self.need_base(f['base_sgpr'], kind, row, f['dwords'], f['offset'])
            if f['imm'] != 1:
                self.need_scalar(f['offset'], row)
                self.blocker(row, 'smrd-offset',
                             'SMRD register/literal offset (IMM=0) is not lowerable; '
                             'only the immediate form has a stated semantics here')
            index = len(self.loads)
            self.loads.append({'index': index, 'offset': f['offset'], 'dwords': f['dwords'],
                               'slot': f['base_sgpr'], 'dst': f['sdst'], 'row': row})
            for j in range(f['dwords']):
                self.scalar_defined[f['sdst'] + j] = 'loaded'
                self.writer[f['sdst'] + j] = ('smrd', index, name, row.offset)
        elif enc == 'MUBUF':
            self.need_descriptor(f['resource_sgpr'], row, f['components'])
            if f['soffset'] < 128:
                self.need_scalar(f['soffset'], row)
            fetch = f['idxen'] == 1 and 0 <= row.opcode <= 7
            self.need_vector(f['vaddr'], row, 'vertex_index' if fetch else None)
            for j in range(f['components']):
                self.vector_defined[f['vdata'] + j] = 'loaded'
            bad = [k for k in ('offset', 'offen', 'glc', 'addr64', 'lds', 'slc', 'tfe') if f[k]]
            if bad or f['idxen'] != 1 or f['soffset'] != 128:
                self.blocker(row, 'stream-shape',
                             'formatted-stream shape outside the lowered contract '
                             f'(nonzero {", ".join(bad) or "none"}, idxen={f["idxen"]}, '
                             f'soffset={f["soffset"]})')
        elif enc == 'VINTRP':
            if row.opcode == 2:
                self.policies.setdefault('interpolation', {
                    'name': 'interpolation', 'value': 'supplied_vertex_values',
                    'reason': f'v_interp_mov_f32 at {_hex(row.offset)}'})
                select = self.policies.setdefault('vertex_select', {
                    'name': 'vertex_select', 'value': None,
                    'reason': 'VSRC selector has two disagreeing descriptions; the map is '
                              'manifest-supplied'})
                select.setdefault('selectors', [])
                if f['vsrc'] not in select['selectors']:
                    select['selectors'].append(f['vsrc'])
            else:
                self.policies.setdefault('interpolation', {
                    'name': 'interpolation', 'value': 'supplied_float_coefficients',
                    'reason': f'{name} at {_hex(row.offset)}'})
                self.need_scalar(124, row, 'interpolation_control')
            self.vector_defined[f['vdst']] = 'derived'
        elif enc in ('VOP1', 'VOP2', 'VOP3', 'VOPC'):
            if enc == 'VOP1':
                self.need_source(f['src0'], row)
                self.vector_defined[f['vdst']] = 'derived'
            elif enc == 'VOP2':
                self.need_source(f['src0'], row)
                self.need_vector(f['vsrc1'], row)
                self.vector_defined[f['vdst']] = 'derived'
            elif enc == 'VOPC':
                self.need_source(f['src0'], row)
                self.need_vector(f['vsrc1'], row)
            else:
                if row.opcode in gcn.VOP3_UNARY:
                    used = ('src0',)
                elif row.opcode in gcn.VOP3_THREE_SOURCE:
                    used = ('src0', 'src1', 'src2')
                else:
                    used = ('src0', 'src1')
                for operand in used:
                    self.need_source(f[operand], row)
                if row.opcode not in gcn.VOP3_COMPARE:
                    self.vector_defined[f['vdst']] = 'derived'
            if name not in LOWERED_ARITHMETIC:
                self.blocker(row, 'unsupported-row',
                             f'{name} is not a lowering row (supported: '
                             f'{", ".join(sorted(LOWERED_ARITHMETIC))})')
        elif enc == 'EXP':
            self.export_row(row)
        elif enc == 'MIMG':
            self.blocker(row, 'image-sample',
                         'image_sample is not a row of the per-lane lowerer')
        elif name.startswith('s_cbranch') or name == 's_branch':
            self.blocker(row, 'control-flow',
                         'control transfer: this listing is linear, the lowerer is not '
                         'reached at all')
        else:
            self.blocker(row, 'unsupported-row', f'{name} has no lowering row')

    # -- exports ---------------------------------------------------------------------------
    def export_row(self, row) -> None:
        """Model one `EXP` row exactly as `gcn_to_msl.Lowerer.export_row` does.

        `en` selects the components this row writes and `vsrc<j>` feeds component `j`; the
        compressed form packs two fp16 components per dword with `en` bit 0 exporting R,G from
        `vsrc0` and bit 2 exporting B,A from `vsrc1` (pinned PS4 translator, render target and
        depth only); an `en == 0` row and the Null target export nothing; and `vm` is the wave
        valid mask, modelled only on a row that carries no component (the null-export idiom).
        """
        f = row.fields
        target, enable, compr = f['target'], f['en'], f['compr']
        null = enable == 0 or target == NULL_EXPORT_TARGET
        if f['vm'] and self.stage != 'ps':
            self.blocker(row, 'export-shape',
                         'valid-mask export is only defined on a fragment stage')
            return
        if not null and compr and not 0 <= target <= 8:
            self.blocker(row, 'export-shape',
                         'compressed export outside a render-target/depth target')
            return
        if not null and compr and not enable & 5:
            self.blocker(row, 'export-shape', 'compressed export enables no component pair')
            return
        if null:
            if f['vm']:
                self.valid_mask_exports.append(target)
            self.null_exports.append({'target': target, 'offset': row.offset,
                                      'valid_mask': bool(f['vm'])})
            return
        if f['vm']:
            self.valid_mask_exports.append(target)
        if compr:
            components = ([0, 1] if enable & 1 else []) + ([2, 3] if enable & 4 else [])
        else:
            components = [j for j in range(4) if enable & (1 << j)]
        for component in components:
            source = (0 if component < 2 else 1) if compr else component
            self.need_vector(f[f'vsrc{source}'], row)
        sources = [f[f'vsrc{j}'] for j in range(4)]
        entry = self.exports.get(target)
        if entry is None:
            entry = self.exports[target] = {
                'target': target, 'sources': sources, 'components': list(components),
                'sites': [row.offset], 'compressed': bool(compr)}
        else:
            entry['sites'].append(row.offset)
            entry['sources'] = sources
            entry['components'] = sorted(set(entry['components']) | set(components))

    def _producer(self, slot: int, width: int, sites: list[int]) -> tuple[str, dict | None]:
        """Classify how a base/descriptor slot is produced, at the sites that read it.

        `direct`: the slot is bound by a manifest — nothing wrote it, or it is read only up to the
        instruction that overwrites it (`s_load_dwordx2 s[0:1], [s0:1], 4` reads the root pointer
        and stores the result into the same registers; every read of the root happens first).
        `resident`: every cell comes from ONE `SMRD` load that is read after it, so the manifest can
        declare the *memory* that load reads instead. `opaque`: anything else (a move, several
        loads, a partial overwrite, or a read both before and after an in-place overwrite) — no
        `SMRD` load of a single bound view can express it.
        """
        writers = [self.writer.get(slot + j) for j in range(width)]
        if all(w is None for w in writers):
            return 'direct', None
        first_write = min(w[3] for w in writers if w is not None)
        before = [site for site in sites if site <= first_write]
        after = [site for site in sites if site > first_write]
        if before and not after:
            return 'direct', None
        if before and after:
            return 'reassigned', {'names': sorted({w[2] for w in writers if w is not None}),
                                  'offset': first_write}
        first = writers[0]
        if first is not None and first[0] == 'smrd' and all(w == first for w in writers):
            load = self.loads[first[1]]
            return 'resident', {'load': first[1], 'via': load['slot'],
                                'offset': load['offset'] + (slot - load['dst']),
                                'row': load['row']}
        names = sorted({w[2] for w in writers if w is not None})
        return 'opaque', {'names': names, 'offset': first_write}

    @staticmethod
    def _runs(dwords) -> list[dict]:
        """Contiguous dword runs, in the `{'offset', 'dwords'}` shape the manifest already uses."""
        runs, start, previous = [], None, None
        for dword in sorted(dwords):
            if start is None:
                start = previous = dword
            elif dword == previous + 1:
                previous = dword
            else:
                runs.append({'offset': start, 'dwords': previous - start + 1})
                start = previous = dword
        if start is not None:
            runs.append({'offset': start, 'dwords': previous - start + 1})
        return runs

    def result(self, code: bytes, rows: list, inventory: dict) -> dict:
        direct, descriptors, resident, opaque = [], [], [], []
        for entries, role in ((self.bases, 'scalar_base'),
                              (self.descriptors, 'buffer_descriptor')):
            for slot in sorted(entries):
                entry = entries[slot]
                width = BINDING_WIDTH[entry['kind']]
                producer, info = self._producer(slot, width, entry['sites'])
                if producer == 'direct':
                    (direct if role == 'scalar_base' else descriptors).append(entry)
                elif producer == 'resident':
                    claim = {'slot': slot, 'role': role, 'kind': entry['kind'],
                             'via': info['via'], 'offset': info['offset'], 'width': width,
                             'sites': list(entry['sites']), 'load': info['load'],
                             'view_dwords': []}
                    if role == 'buffer_descriptor':
                        claim['formats'] = entry['formats']
                    resident.append(claim)
                elif producer == 'reassigned':
                    label = 'scalar base' if role == 'scalar_base' else 'buffer descriptor'
                    self.shape.append({
                        'offset': info['offset'],
                        'mnemonic': 'SMRD',
                        'kind': 'base-reassignment',
                        'reason': f'{label} s{slot} is read before and after it is overwritten at '
                                  f'{_hex(info["offset"])}, so its reads address two different '
                                  f'views; one cell cannot be both'})
                else:
                    label = ('scalar base' if role == 'scalar_base'
                             else 'buffer descriptor')
                    opaque.append({'slot': slot, 'role': role, 'kind': entry['kind'],
                                   'sites': list(entry['sites']),
                                   'names': info['names']})
                    self.shape.append({
                        'offset': entry['sites'][0],
                        'mnemonic': 'SMRD' if role == 'scalar_base' else 'MUBUF',
                        'kind': 'derived-base' if role == 'scalar_base' else 'derived-descriptor',
                        'reason': f'{label} s{slot} is written by '
                                  f'{", ".join(info["names"])}, not by one SMRD load of a bound '
                                  f'view, so neither a direct binding nor a memory-resident cell '
                                  f'can express it'})
        # One manifest cell per dword: two uses of the same dword with a different kind or span
        # cannot both be declared, which is a property of the cell model rather than of the shader.
        by_parent: dict[int, list[dict]] = {}
        for claim in resident:
            by_parent.setdefault(claim['via'], []).append(claim)
        conflicts: list[dict] = []
        for parent, claims in sorted(by_parent.items()):
            for index, first in enumerate(claims):
                for second in claims[index + 1:]:
                    span = (first['offset'], first['width'], first['kind'])
                    other = (second['offset'], second['width'], second['kind'])
                    overlap = max(first['offset'], second['offset']) < min(
                        first['offset'] + first['width'], second['offset'] + second['width'])
                    if overlap and span != other:
                        conflicts.append({
                            'offset': second['sites'][0], 'mnemonic': 'SMRD',
                            'kind': 'resident-kind-conflict',
                            'reason': f's{first["slot"]} needs dword(s) '
                                      f'[{first["offset"]}..{first["offset"] + first["width"] - 1}] '
                                      f'as {first["kind"]} and s{second["slot"]} needs '
                                      f'[{second["offset"]}..'
                                      f'{second["offset"] + second["width"] - 1}] as '
                                      f'{second["kind"]} in the same view (s{parent}); one dword '
                                      f'cannot hold two cell types'})
        self.shape.extend(conflicts)
        # Every dword an SMRD load reads has to be defined by the view it reads, except the dwords
        # that are exactly a resident cell of that same load.
        data: dict[int, set[int]] = {}
        for load in self.loads:
            for dword in range(load['offset'], load['offset'] + load['dwords']):
                covered = any(claim['offset'] <= dword < claim['offset'] + claim['width']
                              for claim in by_parent.get(load['slot'], []))
                if not covered:
                    data.setdefault(load['slot'], set()).add(dword)
        for entry in direct + descriptors + resident:
            entry['view_dwords'] = self._runs(data.get(entry['slot'], set()))
        modes = {entry['value'] for entry in self.policies.values()
                 if entry['name'] == 'interpolation'}
        if len(modes) > 1:
            self.shape.append({'offset': 0, 'mnemonic': 'VINTRP',
                               'kind': 'interpolation-mode-conflict',
                               'reason': 'both interpolation modes are required by one program'})
        for slot, entry in self.initial_vectors.items():
            if entry['role'] == 'vertex_index' and entry['plain_sites']:
                self.shape.append({
                    'offset': entry['sites'][0], 'mnemonic': 'VOP', 'kind': 'view-role-conflict',
                    'reason': f'v{slot} is used both as a vertex index and as a plain '
                              f'addition input; the lowering requires the vertex-index cell'})
        if self.stage == 'vs':
            clip = self.exports.get(CLIP_TARGET)
            if clip is None or clip['components'] != [0, 1, 2, 3]:
                self.shape.append({
                    'offset': 0, 'mnemonic': 'EXP', 'kind': 'export-contract',
                    'reason': f'a vertex stage must export all four clip components to target '
                              f'{CLIP_TARGET}; program exports '
                              f'{ {t: e["components"] for t, e in sorted(self.exports.items())} }'})
        holes = {
            'scalar_bases': sorted(direct, key=lambda e: e['slot']),
            'buffer_descriptors': sorted(descriptors, key=lambda e: e['slot']),
            'resident_slots': sorted(resident, key=lambda e: e['slot']),
            'export_targets': sorted(self.exports.values(), key=lambda e: e['target']),
            'null_exports': sorted(self.null_exports, key=lambda e: (e['target'], e['offset'])),
            'valid_mask_exports': sorted(set(self.valid_mask_exports)),
            'initial_vectors': sorted(self.initial_vectors.values(), key=lambda e: e['slot']),
            'initial_scalars': sorted(self.initial_scalars.values(), key=lambda e: e['slot']),
            'policy': sorted(self.policies.values(), key=lambda e: e['name']),
        }
        counts = {
            'scalar_bases': len(holes['scalar_bases']),
            'buffer_descriptors': len(holes['buffer_descriptors']),
            'resident_slots': len(holes['resident_slots']),
            'export_targets': len(holes['export_targets']),
            'null_exports': len(holes['null_exports']),
            'initial_vectors': len(holes['initial_vectors']),
            'initial_scalars': len(holes['initial_scalars']),
            'policy': len(holes['policy']),
            'view_dword_requests': sum(len(e['view_dwords'])
                                       for e in holes['scalar_bases'] + holes['resident_slots']),
            'shape': len(self.shape),
        }
        counts['binding_holes'] = sum(counts[k] for k in
                                      ('scalar_bases', 'buffer_descriptors', 'resident_slots',
                                       'export_targets', 'initial_vectors', 'initial_scalars',
                                       'policy'))
        # Slots the program touches but no manifest can bind, with the producing rows named.
        derived_map = {
            'scalar_bases': sorted(({'slot': e['slot'], 'kind': e['kind'],
                                     'sites': e['sites'], 'written_by': e['names']}
                                    for e in opaque if e['role'] == 'scalar_base'),
                                   key=lambda e: e['slot']),
            'buffer_descriptors': sorted(({'slot': e['slot'], 'sites': e['sites'],
                                           'written_by': e['names']}
                                          for e in opaque if e['role'] == 'buffer_descriptor'),
                                         key=lambda e: e['slot']),
        }
        counts['derived_slots'] = len(derived_map['scalar_bases']) + len(
            derived_map['buffer_descriptors'])
        return {
            'schema': ANALYSIS_SCHEMA,
            'stage': self.stage,
            'sha256': hashlib.sha256(code).hexdigest(),
            'bytes': len(code),
            'instructions': len(rows),
            'per_lane_eligible': inventory['per_lane_eligible'],
            'per_lane_blockers': inventory['per_lane_blockers'],
            'holes': holes,
            'derived': derived_map,
            'counts': counts,
            # A manifest is necessary but not sufficient: `shape` lists everything a manifest
            # cannot fix, so "waiting on bindings" is never claimed for a shape-blocked program.
            'manifest_only': not self.shape,
            'shape': self.shape,
        }


def analyze(code: bytes, stage: str | None = None) -> dict:
    """Return the manifest holes of one exact program span.

    Raises `AbiError` when the program does not decode linearly: the lowerer's entry point is
    `gcn_inventory.decode()`, so a basic-block-only program never reaches a manifest at all.
    """
    if not code:
        raise AbiError('empty program span')
    if stage is not None and stage not in STAGES:
        raise AbiError(f'unsupported stage {stage!r}; expected one of {", ".join(STAGES)}')
    try:
        rows = gcn.decode(code)
    except gcn.DecodeError as exc:
        raise AbiError(f'program does not decode linearly ({exc}); a basic-block-only program '
                       f'cannot reach the lowerer, so it has no manifest to build') from None
    state = _Analysis(stage)
    for row in rows:
        state.consume(row)
    return state.result(code, rows, gcn.inventory(code))


# --------------------------------------------------------------------------------------------
# Skeleton: the form an owner or a state trace fills in.
# --------------------------------------------------------------------------------------------

def skeleton(analysis: dict, identity: str | None = None,
             provenance: str = 'state-trace') -> dict:
    """Emit `msl-abi-manifest-v1` with exactly the holes present, each `null` until supplied.

    Only what the *encoding* forces is pre-filled, never a resource: a `MUBUF` carry of a formatted
    vertex fetch reads the vertex index, the interpolation-control cell `s124` is the coefficient
    selector, the interpolation mode follows from the `VINTRP` row, and `floating_point` has one
    accepted profile. `vertex_select` stays null: the pinned translator and the ISA summary disagree
    on that mapping, so it is the owner's call.
    """
    holes = analysis['holes']
    document = {
        'schema': SCHEMA,
        'shader': {'id': identity or analysis['sha256'][:16], 'stage': analysis['stage'],
                   'sha256': analysis['sha256'], 'bytes': analysis['bytes']},
        'provenance': {'bindings': provenance,
                       'note': 'every null is an unfilled hole; slot numbers are measured, '
                               'addresses are not'},
        'scalar_bases': {str(e['slot']): None for e in holes['scalar_bases']},
        'buffer_descriptors': {str(e['slot']): None for e in holes['buffer_descriptors']},
        'exports': {str(e['target']): None for e in holes['export_targets']},
        'initial_scalars': {str(e['slot']): ({'kind': 'interpolation_control'}
                                             if e['role'] == 'interpolation_control' else None)
                            for e in holes['initial_scalars']},
        'initial_vectors': {str(e['slot']): ({'kind': 'vertex_index'}
                                             if e['role'] == 'vertex_index' else None)
                            for e in holes['initial_vectors']},
        'views': {},
        'policy': {},
    }
    for entry in holes.get('resident_slots') or []:
        section = ('scalar_bases' if entry['role'] == 'scalar_base'
                   else 'buffer_descriptors')
        form = {'kind': entry['kind'], 'via': str(entry['via']),
                'offset': entry['offset'], 'view': None}
        if section == 'buffer_descriptors':
            form.update({'format': None, 'field': None})
        document[section][str(entry['slot'])] = form
    # `gcn_to_msl.generate()` admits exactly one floating-point profile, so it is a constant here.
    document['policy']['floating_point'] = FLOATING_POINT_PROFILES[0]
    for entry in holes['policy']:
        if entry['name'] == 'interpolation':
            document['policy']['interpolation'] = entry['value']
        elif entry['name'] == 'vertex_select':
            document['policy']['vertex_select'] = None
    return document


def synthetic_fill(analysis: dict, identity: str = 'synthetic_fill') -> dict:
    """Mechanically fill a skeleton from its own hole list.

    This is *not* a binding: every value it supplies is a placeholder chosen to satisfy the form
    (`view_<slot>` for a resource, `target_<n>` for an export, a named case field for a data dword).
    Its purpose is to prove the form is closable and to let the lowering run offline; a program
    with no holes produces a manifest with no bindings at all, which is the only case where such a
    fill is not a placeholder.
    """
    holes = analysis['holes']
    document = skeleton(analysis, identity, 'synthetic')
    views: dict[str, dict] = {}

    def view_dwords(view: str, requests: list[dict]) -> None:
        entries = views.setdefault(view, {})
        for request in requests:
            for offset in range(request['offset'], request['offset'] + request['dwords']):
                entries[str(offset)] = {'kind': 'float', 'field': 'camera', 'component': 'x'}

    for entry in holes['scalar_bases']:
        view = f'view_{entry["slot"]}'
        document['scalar_bases'][str(entry['slot'])] = {'kind': entry['kind'], 'view': view}
        view_dwords(view, entry['view_dwords'])
    for entry in holes['buffer_descriptors']:
        view = f'view_{entry["slot"]}'
        document['buffer_descriptors'][str(entry['slot'])] = {
            'kind': 'descriptor', 'view': view,
            'format': entry['formats'][0], 'field': 'position'}
        view_dwords(view, entry['view_dwords'])
    for entry in holes.get('resident_slots') or []:
        section = ('scalar_bases' if entry['role'] == 'scalar_base'
                   else 'buffer_descriptors')
        binding = document[section][str(entry['slot'])]
        binding['view'] = f'view_{entry["slot"]}'
        if section == 'buffer_descriptors':
            binding['format'] = entry['formats'][0]
            binding['field'] = 'position'
        view_dwords(binding['view'], entry['view_dwords'])
    for entry in holes['export_targets']:
        document['exports'][str(entry['target'])] = {
            'semantic': f'target_{entry["target"]}'}
    for entry in holes['initial_vectors']:
        document['initial_vectors'][str(entry['slot'])] = (
            {'kind': 'vertex_index'} if entry['role'] == 'vertex_index'
            else {'kind': 'float', 'field': 'camera', 'component': 'x'})
    for entry in holes['initial_scalars']:
        document['initial_scalars'][str(entry['slot'])] = {'kind': 'interpolation_control'}
    for entry in holes['policy']:
        if entry['name'] == 'interpolation':
            document['policy']['interpolation'] = entry['value']
        elif entry['name'] == 'vertex_select':
            document['policy']['vertex_select'] = {str(selector): 'p10'
                                                   for selector in entry['selectors']}
    document['views'] = views
    return document


# --------------------------------------------------------------------------------------------
# Validation: every used slot bound, every bound slot used, per-slot diagnostics.
# --------------------------------------------------------------------------------------------

def _problem(code: str, slot, message: str) -> dict:
    return {'code': code, 'slot': slot, 'message': message}


def _check_document(manifest) -> list[dict]:
    """Structural checks that must pass before any per-slot comparison is meaningful."""
    problems: list[dict] = []

    def require(condition: bool, code: str, slot, message: str) -> bool:
        if not condition:
            problems.append(_problem(code, slot, message))
        return condition

    if not isinstance(manifest, dict):
        return [_problem('not-an-object', None, 'manifest must be a JSON object')]
    unknown = sorted(set(manifest) - set(MANIFEST_KEYS))
    if unknown:
        return [_problem('unknown-field', None,
                         f'unknown top-level field(s): {", ".join(unknown)}')]
    if not require(manifest.get('schema') == SCHEMA, 'schema', None,
                   f'schema must be {SCHEMA!r}, got {manifest.get("schema")!r}'):
        return problems
    shader = manifest.get('shader')
    if not require(isinstance(shader, dict), 'shader', None, '"shader" must be an object'):
        return problems
    unknown = sorted(set(shader) - set(SHADER_KEYS))
    require(not unknown, 'unknown-field', None,
            f'unknown shader field(s): {", ".join(unknown)}')
    require(isinstance(shader.get('id'), str) and bool(shader['id']), 'shader-id', None,
            '"shader.id" must be a non-empty string')
    require(shader.get('stage') in STAGES, 'stage', None,
            f'"shader.stage" must be one of {", ".join(STAGES)}, got {shader.get("stage")!r}')
    require(isinstance(shader.get('sha256'), str)
            and bool(SHA256.fullmatch(shader['sha256'])), 'digest', None,
            '"shader.sha256" must be a lowercase 64-hex digest')
    require(isinstance(shader.get('bytes'), int) and not isinstance(shader['bytes'], bool)
            and shader['bytes'] > 0, 'bytes', None,
            '"shader.bytes" must be a positive integer')
    provenance = manifest.get('provenance')
    if provenance is not None:
        if require(isinstance(provenance, dict), 'provenance', None,
                   '"provenance" must be an object'):
            unknown = sorted(set(provenance) - {'bindings', 'note'})
            require(not unknown, 'unknown-field', None,
                    f'unknown provenance field(s): {", ".join(unknown)}')
            require(provenance.get('bindings') in ('state-trace', 'owner-supplied', 'synthetic'),
                    'provenance', None,
                    '"provenance.bindings" must be state-trace, owner-supplied or synthetic')
    for section, kinds in (('scalar_bases', ('pointer', 'descriptor')),
                           ('buffer_descriptors', ('descriptor',)),
                           ('initial_scalars', BINDING_KINDS),
                           ('initial_vectors', SINGLE_CELL_KINDS)):
        value = manifest.get(section, {})
        if not require(isinstance(value, dict), section, None,
                       f'"{section}" must be an object'):
            return problems
        for key, binding in value.items():
            if not require(bool(re.fullmatch(r'[0-9]+', str(key))), 'slot-key', key,
                           f'"{section}" key {key!r} is not a decimal slot number'):
                continue
            problems.extend(_check_binding(binding, f'{section}[{key}]', kinds,
                                           require_format=section == 'buffer_descriptors'))
            if isinstance(binding, dict) and section in ('scalar_bases', 'buffer_descriptors'):
                if 'via' in binding and not (isinstance(binding['via'], str)
                                             and re.fullmatch(r'[0-9]+', binding['via'])):
                    problems.append(_problem(
                        'invalid-binding', f'{section}[{key}]',
                        f'{section}[{key}].via must be the decimal slot the load reads'))
                if 'offset' in binding and not (isinstance(binding['offset'], int)
                                                and not isinstance(binding['offset'], bool)
                                                and binding['offset'] >= 0):
                    problems.append(_problem(
                        'invalid-binding', f'{section}[{key}]',
                        f'{section}[{key}].offset must be a non-negative dword offset'))
    exports = manifest.get('exports', {})
    if not require(isinstance(exports, dict), 'exports', None, '"exports" must be an object'):
        return problems
    for key, entry in exports.items():
        where = f'exports[{key}]'
        if not require(bool(re.fullmatch(r'[0-9]+', str(key))), 'slot-key', key,
                       f'"exports" key {key!r} is not a decimal export target'):
            continue
        if entry is None:
            problems.append(_problem('unfilled-slot', where,
                                     f'{where} is still null: this hole was not filled in'))
            continue
        if not require(isinstance(entry, dict), 'invalid-binding', where,
                       f'{where} must be an object or null'):
            continue
        unknown = sorted(set(entry) - {'semantic', 'sources', 'note'})
        require(not unknown, 'unknown-field', where,
                f'{where} has unknown field(s): {", ".join(unknown)}')
        require(isinstance(entry.get('semantic'), str) and bool(entry['semantic']),
                'invalid-binding', where,
                f'{where}.semantic must be a non-empty string naming the MRT/param')
        if 'sources' in entry:
            sources = entry['sources']
            require(isinstance(sources, list) and len(sources) == 4
                    and all(isinstance(x, int) and not isinstance(x, bool) and 0 <= x < 256
                            for x in sources),
                    'invalid-binding', where,
                    f'{where}.sources must be four VGPR numbers when pinned')
    views = manifest.get('views', {})
    if not require(isinstance(views, dict), 'views', None, '"views" must be an object'):
        return problems
    for name, entries in views.items():
        if not require(isinstance(name, str) and bool(IDENTIFIER.fullmatch(name)), 'view-name',
                       name, f'"views" key {name!r} is not a lowercase view identifier'):
            continue
        if not require(isinstance(entries, dict), 'views', name,
                       f'views[{name}] must be an object keyed by dword offset'):
            continue
        for key, binding in entries.items():
            if not require(bool(re.fullmatch(r'[0-9]+', str(key))), 'slot-key', key,
                           f'views[{name}] key {key!r} is not a decimal dword offset'):
                continue
            problems.extend(_check_binding(binding, f'views[{name}][{key}]',
                                           VIEW_BINDING_KINDS))
    policy = manifest.get('policy') or {}
    unknown = sorted(set(policy) - set(POLICY_KEYS))
    require(not unknown, 'unknown-field', None,
            f'unknown policy field(s): {", ".join(unknown)}')
    if 'floating_point' in policy:
        require(policy['floating_point'] in FLOATING_POINT_PROFILES, 'policy', 'floating_point',
                f'floating_point must be one of {", ".join(FLOATING_POINT_PROFILES)}')
    if 'interpolation' in policy:
        require(policy['interpolation'] in INTERPOLATION_MODES, 'policy', 'interpolation',
                f'interpolation must be one of {", ".join(INTERPOLATION_MODES)}')
    if 'vertex_select' in policy:
        select = policy['vertex_select']
        if require(isinstance(select, dict), 'policy', 'vertex_select',
                   'vertex_select must be an object mapping VSRC to p10/p20/p0'):
            for key, value in select.items():
                require(bool(re.fullmatch(r'[0-9]+', str(key))), 'policy', 'vertex_select',
                        f'vertex_select key {key!r} is not a decimal selector')
                require(value in VERTEX_SELECT_TARGETS, 'policy', 'vertex_select',
                        f'vertex_select[{key}] must be one of '
                        f'{", ".join(VERTEX_SELECT_TARGETS)}')
    if 'vertex_mov_subtract_p10' in policy:
        value = policy['vertex_mov_subtract_p10']
        require(isinstance(value, list)
                and all(isinstance(x, int) and not isinstance(x, bool) for x in value),
                'policy', 'vertex_mov_subtract_p10',
                'vertex_mov_subtract_p10 must be a list of integers')
    return problems


def _check_binding(binding, where: str, kinds, require_format: bool = False) -> list[dict]:
    problems: list[dict] = []
    if binding is None:
        return [_problem('unfilled-slot', where, f'{where} is still null: this hole was not '
                                                f'filled in')]
    if not isinstance(binding, dict):
        return [_problem('invalid-binding', where, f'{where} must be an object or null')]
    unfilled = sorted(key for key, value in binding.items() if value is None)
    if unfilled:
        return [_problem('unfilled-slot', where,
                         f'{where} still has null field(s) {", ".join(unfilled)}: this hole was '
                         f'not filled in')]
    kind = binding.get('kind')
    allowed = BINDING_KINDS if kinds is None else kinds
    if kind not in BINDING_KINDS:
        return [_problem('invalid-binding', where,
                         f'{where}.kind must be one of {", ".join(BINDING_KINDS)}, '
                         f'got {kind!r}')]
    if kinds is not None and kind not in allowed:
        problems.append(_problem('invalid-binding', where,
                                 f'{where}.kind must be one of {", ".join(allowed)} for this '
                                 f'section, got {kind!r}'))
    if kind in ('pointer', 'descriptor'):
        if not (isinstance(binding.get('view'), str)
                and bool(IDENTIFIER.fullmatch(binding['view']))):
            problems.append(_problem('invalid-binding', where,
                                     f'{where}.view must be a lowercase identifier naming the '
                                     f'view this {kind} addresses'))
    if kind in ('float', 'float4'):
        if binding.get('field') not in CASE_FIELDS:
            problems.append(_problem('invalid-binding', where,
                                     f'{where}.field must be one of the named case fields'))
    if kind == 'float' and binding.get('component') not in tuple('xyzw'):
        problems.append(_problem('invalid-binding', where,
                                 f'{where}.component must be one of x, y, z, w'))
    if require_format:
        if binding.get('format') not in ('explicit_float2', 'explicit_float3',
                                         'explicit_float4'):
            problems.append(_problem('invalid-binding', where,
                                     f'{where}.format must be explicit_float2/3/4'))
    return problems


def validate(manifest: dict, analysis: dict) -> list[dict]:
    """Return every reason this manifest does not describe this program.

    Fail-closed and per slot: each diagnostic names the slot, the requirement it failed and the
    instruction offset that established the requirement. An empty list means the manifest covers
    the program's holes exactly. It does not mean the program lowers: `shape` blockers are
    reported here too, because no manifest can satisfy them.
    """
    problems = _check_document(manifest)
    if problems:
        return problems
    shader = manifest['shader']
    if shader['sha256'] != analysis['sha256']:
        problems.append(_problem('digest', None,
                                 f'shader.sha256 {shader["sha256"]} does not match this program '
                                 f'{analysis["sha256"]}'))
        return problems
    if shader['bytes'] != analysis['bytes']:
        problems.append(_problem('bytes', None,
                                 f'shader.bytes {shader["bytes"]} does not match this program '
                                 f'{analysis["bytes"]}'))
    if shader['stage'] != analysis['stage']:
        problems.append(_problem('stage', None,
                                 f'shader.stage {shader["stage"]!r} does not match the analysed '
                                 f'stage {analysis["stage"]!r}'))
    problems.extend(_check_holes(manifest, analysis))
    for blocker in analysis['shape']:
        problems.append(_problem('shape-blocker', blocker['mnemonic'],
                                 f'{_hex(blocker["offset"])}: {blocker["reason"]}; no manifest '
                                 f'can lower this program'))
    return problems


def _requirements(holes: dict) -> dict:
    """Which manifest section each measured requirement belongs to, direct and resident alike."""
    residents = holes.get('resident_slots') or []
    return {
        'scalar_bases': list(holes['scalar_bases']) + [
            entry for entry in residents if entry['role'] == 'scalar_base'],
        'buffer_descriptors': list(holes['buffer_descriptors']) + [
            entry for entry in residents if entry['role'] == 'buffer_descriptor'],
    }


def _resolve_view(manifest: dict, slot: int) -> str | None:
    """The view name a slot's binding addresses, whichever section holds that slot."""
    for section in ('scalar_bases', 'buffer_descriptors'):
        binding = (manifest.get(section) or {}).get(str(slot))
        if isinstance(binding, dict) and isinstance(binding.get('view'), str):
            return binding['view']
    return None


def _check_holes(manifest: dict, analysis: dict) -> list[dict]:
    holes, derived_map = analysis['holes'], analysis.get('derived') or {}
    required = _requirements(holes)
    problems: list[dict] = []
    sections = {
        'scalar_bases': ('scalar_base', 's', required['scalar_bases'], 'slot'),
        'buffer_descriptors': ('buffer_descriptor', 's', required['buffer_descriptors'], 'slot'),
        'exports': ('export_target', '', holes['export_targets'], 'target'),
        'initial_scalars': ('initial_scalar', 's', holes['initial_scalars'], 'slot'),
        'initial_vectors': ('initial_vector', 'v', holes['initial_vectors'], 'slot'),
    }
    supplied = {name: {str(e[key]) for e in entries}
                for name, (_, _, entries, key) in sections.items()}
    # Where each slot is really used, so a binding in the wrong section gets a precise reason
    # instead of "never uses it". SGPR slots, VGPR slots and export targets are separate
    # namespaces: scalar s12 and export target 12 are not the same thing.
    used_as: dict[tuple[str, str], set[str]] = {}
    for name, (_, _, entries, key) in sections.items():
        for entry in entries:
            used_as.setdefault((NAMESPACE[name], str(entry[key])), set()).add(name)
    for name, entries in derived_map.items():
        for entry in entries:
            used_as.setdefault((NAMESPACE[name], str(entry['slot'])), set()).add(name)
    for name, (label, prefix, entries, key) in sections.items():
        bound = manifest.get(name) or {}
        for entry in entries:
            slot = str(entry[key])
            sites = ', '.join(_hex(site) for site in entry['sites']) or 'no site'
            if 'via' in entry:
                sites += (f'; read from the view s{entry["via"]} addresses at dword(s) '
                          f'[{entry["offset"]}..{entry["offset"] + entry["width"] - 1}]')
            if slot not in bound:
                problems.append(_problem(
                    f'missing-{label.replace("_", "-")}', f'{prefix}{slot}',
                    f'{label} {prefix}{slot} is used at {sites} but the manifest does not bind '
                    f'it'))
                continue
            if bound[slot] is None:
                continue  # already reported by _check_document as unfilled-slot
            if 'via' in entry:
                problems.extend(_check_resident(label, prefix, entry, bound[slot], sites))
            problems.extend(_check_kind(name, label, prefix, entry, bound[slot], sites))
        for extra in sorted(set(bound) - supplied[name], key=int):
            derived = [entry for entry in derived_map.get(name, [])
                       if str(entry['slot']) == extra]
            if derived:
                sites = ', '.join(_hex(site) for site in derived[0]['sites'])
                problems.append(_problem(
                    'derived-slot', f'{prefix}{extra}',
                    f'{label} {prefix}{extra} is used at {sites} but written by '
                    f'{", ".join(derived[0].get("written_by") or ["an earlier instruction"])}, '
                    f'not by one SMRD load of a bound view; the program is shape-blocked rather '
                    f'than binding-blocked'))
            elif used_as.get((NAMESPACE[name], extra), set()) - {name}:
                other = sorted(used_as[(NAMESPACE[name], extra)] - {name})[0]
                problems.append(_problem(
                    'wrong-section', f'{prefix}{extra}',
                    f'{label} {prefix}{extra} is used by this program as {other}, '
                    f'not as {name}'))
            else:
                problems.append(_problem(
                    f'unused-{label.replace("_", "-")}', f'{prefix}{extra}',
                    f'{label} {prefix}{extra} is bound but this program never uses it'))
    policy = manifest.get('policy') or {}
    needed = {entry['name'] for entry in holes['policy']}
    for name in ('interpolation', 'vertex_select', 'vertex_mov_subtract_p10'):
        if name in policy and name not in needed:
            problems.append(_problem(
                'unused-policy', f'policy.{name}',
                f'policy.{name} is declared but this program needs no such knob'))
    for entry in holes['policy']:
        if entry['name'] == 'interpolation':
            if policy.get('interpolation') != entry['value']:
                problems.append(_problem(
                    'missing-policy', 'policy.interpolation',
                    f'policy.interpolation must be {entry["value"]!r} ({entry["reason"]}); '
                    f'got {policy.get("interpolation")!r}'))
        elif entry['name'] == 'vertex_select':
            select = policy.get('vertex_select')
            for selector in entry['selectors']:
                if not isinstance(select, dict) or str(selector) not in select:
                    problems.append(_problem(
                        'missing-policy', f'policy.vertex_select[{selector}]',
                        f'VSRC selector {selector} needs an explicit p10/p20/p0 mapping; '
                        f'{entry["reason"]}'))
    problems.extend(_check_views(manifest, holes))
    return problems


def _check_resident(label: str, prefix: str, entry: dict, binding: dict,
                    sites: str) -> list[dict]:
    """A memory-resident cell must declare the producer the encoding measured, not another one."""
    problems: list[dict] = []
    where = f'{label} {prefix}{entry["slot"]}'
    via = binding.get('via')
    if not (isinstance(via, str) and re.fullmatch(r'[0-9]+', via)):
        problems.append(_problem('invalid-binding', where,
                                 f'{where}.via must be the decimal slot the load reads'))
        return problems
    if int(via) != entry['via']:
        problems.append(_problem(
            'wrong-producer', where,
            f'{where} is loaded through s{entry["via"]} at {sites}; the manifest declares via '
            f's{via}'))
    if binding.get('offset') != entry['offset']:
        problems.append(_problem(
            'wrong-producer', where,
            f'{where} sits at dword {entry["offset"]} of that view at {sites}; the manifest '
            f'declares offset {binding.get("offset")!r}'))
    return problems


def _check_kind(section: str, label: str, prefix: str, entry: dict, binding: dict,
                sites: str) -> list[dict]:
    """Per-section semantic checks on one filled binding."""
    problems: list[dict] = []
    where = f'{label} {prefix}{entry.get("slot", entry.get("target"))}'
    if section == 'scalar_bases':
        if binding.get('kind') != entry['kind']:
            problems.append(_problem(
                'wrong-kind', where,
                f'{where} is read as a {entry["kind"]} at {sites} but is bound as '
                f'{binding.get("kind")!r}; the lowering reads {entry["width"]} consecutive '
                f'scalar cells for a {entry["kind"]}'))
    if section == 'buffer_descriptors':
        if binding.get('kind') != 'descriptor':
            problems.append(_problem('wrong-kind', where,
                                     f'{where} must be bound as a descriptor'))
        if binding.get('format') not in entry['formats']:
            problems.append(_problem(
                'wrong-format', where,
                f'{where} feeds a {"/".join(entry["formats"])} load at {sites} but the manifest '
                f'declares format {binding.get("format")!r}'))
    if section == 'exports':
        sources = binding.get('sources')
        if sources is not None and sources != entry['sources']:
            problems.append(_problem(
                'export-source-mismatch', where,
                f'{where} is fed by registers {entry["sources"]} at {sites}; the manifest pins '
                f'{sources}'))
        if not isinstance(binding.get('semantic'), str) or not binding['semantic']:
            problems.append(_problem('invalid-binding', where,
                                     f'{where}.semantic must be a non-empty string'))
    if section == 'initial_vectors' and entry.get('role') == 'vertex_index':
        if binding.get('kind') != 'vertex_index':
            problems.append(_problem(
                'wrong-kind', where,
                f'{where} carries the vertex index into a formatted fetch at {sites}; the '
                f'lowering requires kind "vertex_index", got {binding.get("kind")!r}'))
    if section == 'initial_scalars' and entry.get('role') == 'interpolation_control':
        if binding.get('kind') != 'interpolation_control':
            problems.append(_problem(
                'wrong-kind', where,
                f'{where} is the interpolation coefficient selector at {sites}; the lowering '
                f'requires kind "interpolation_control", got {binding.get("kind")!r}'))
    return problems


def _implied_cells(manifest: dict, holes: dict) -> tuple[dict[str, dict[str, dict]], list[dict]]:
    """The view cells the resident declarations imply, keyed by the parent view they sit in.

    A resident slot is not bound directly: the manifest declares that dword `offset` of the view
    named by the slot's *producer* (`via`) holds a `pointer`/`descriptor` cell addressing the
    slot's `view`. This is where those declarations become concrete view entries, and where an
    unresolvable producer chain is reported.
    """
    implied: dict[str, dict[str, dict]] = {}
    problems: list[dict] = []
    for entry in holes.get('resident_slots') or []:
        section = ('scalar_bases' if entry['role'] == 'scalar_base'
                   else 'buffer_descriptors')
        binding = (manifest.get(section) or {}).get(str(entry['slot']))
        if not isinstance(binding, dict) or not isinstance(binding.get('view'), str):
            continue  # unbound/malformed: already reported
        parent = _resolve_view(manifest, entry['via'])
        if parent is None:
            problems.append(_problem(
                'unresolved-resident-parent', f's{entry["via"]}',
                f's{entry["slot"]} is read from the view that s{entry["via"]} addresses, but no '
                f'binding names a view for s{entry["via"]}'))
            continue
        cell = {'kind': entry['kind'], 'view': binding['view']}
        table = implied.setdefault(parent, {})
        previous = table.get(str(entry['offset']))
        if previous is not None and previous != cell:
            problems.append(_problem(
                'resident-cell-conflict', f'view {parent}',
                f'view {parent!r} dword {entry["offset"]} would have to be a '
                f'{previous["kind"]} and a {cell["kind"]} at once'))
            continue
        table[str(entry['offset'])] = cell
    return implied, problems


def _check_views(manifest: dict, holes: dict) -> list[dict]:
    """Every dword a scalar load reads must be defined exactly once, and nothing surplus."""
    problems: list[dict] = []
    implied, inferred = _implied_cells(manifest, holes)
    problems.extend(inferred)
    declared = manifest.get('views') or {}
    effective: dict[str, dict[str, dict]] = {}
    for name, entries in declared.items():
        if isinstance(entries, dict):
            effective[name] = {str(key): value for key, value in entries.items()}
    for parent, cells in implied.items():
        table = effective.setdefault(parent, {})
        for offset, cell in cells.items():
            previous = table.get(offset)
            if previous is not None and previous != cell:
                problems.append(_problem(
                    'resident-cell-conflict', f'view {parent}',
                    f'view {parent!r} declares dword {offset} as '
                    f'{previous.get("kind") if isinstance(previous, dict) else previous!r} but the '
                    f'resident declaration needs a {cell["kind"]} there'))
                continue
            table[offset] = cell
    required: dict[str, set[int]] = {}
    for name, entries in (('scalar_bases', holes['scalar_bases']),
                          ('buffer_descriptors', holes['buffer_descriptors'])):
        for entry in entries:
            binding = (manifest.get(name) or {}).get(str(entry['slot']))
            if not isinstance(binding, dict) or not isinstance(binding.get('view'), str):
                continue  # unbound/malformed: already reported
            for request in entry['view_dwords']:
                required.setdefault(binding['view'], set()).update(
                    range(request['offset'], request['offset'] + request['dwords']))
    for entry in holes.get('resident_slots') or []:
        binding = (manifest.get('scalar_bases' if entry['role'] == 'scalar_base'
                                else 'buffer_descriptors') or {}).get(str(entry['slot']))
        if not isinstance(binding, dict) or not isinstance(binding.get('view'), str):
            continue
        for request in entry['view_dwords']:
            required.setdefault(binding['view'], set()).update(
                range(request['offset'], request['offset'] + request['dwords']))
        parent = _resolve_view(manifest, entry['via'])
        if parent is not None:
            required.setdefault(parent, set()).update(
                range(entry['offset'], entry['offset'] + entry['width']))
    for name, offsets in sorted(required.items()):
        entries = declared.get(name)
        if not isinstance(entries, dict) or not entries:
            if implied.get(name):
                entries = {}  # every cell here is resident; nothing to declare by hand
            else:
                problems.append(_problem(
                    'missing-view', f'view {name}',
                    f'scalar loads read dwords {sorted(offsets)} through view {name!r}, but the '
                    f'manifest declares no entries for it'))
                continue
        covered: set[int] = set()
        for key, binding in sorted(entries.items(), key=lambda kv: int(kv[0])):
            problems.extend(_check_binding(binding, f'views[{name}][{key}]',
                                           VIEW_BINDING_KINDS))
            if not isinstance(binding, dict) or binding.get('kind') not in BINDING_WIDTH:
                continue
            width = BINDING_WIDTH[binding['kind']]
            span = set(range(int(key), int(key) + width))
            covered.update(span)
            # Entry granularity: a pointer/float4 binding is one entry, so a load reading any of
            # its dwords uses it. A dword *outside* every entry is the error that matters.
            if not span & offsets:
                problems.append(_problem(
                    'unused-view-entry', f'view {name}',
                    f'view {name!r}[{key}] covers dword(s) {sorted(span)} but this program '
                    f'reads only {sorted(offsets)}'))
        table = effective.get(name, {})
        for key, binding in table.items():
            if not isinstance(binding, dict) or binding.get('kind') not in BINDING_WIDTH:
                continue
            covered.update(range(int(key), int(key) + BINDING_WIDTH[binding['kind']]))
        missing = sorted(offsets - covered)
        if missing:
            problems.append(_problem(
                'missing-view-dword', f'view {name}',
                f'view {name!r} does not define dword(s) {missing} that a scalar load reads'))
    return problems


def to_lowerer_stage_abi(manifest: dict) -> dict:
    """Project a filled manifest onto the per-stage dict `gcn_to_msl.Lowerer` consumes.

    Pure dict transformation: nothing is decoded, lowered or compiled here. It exists so that a
    validated manifest is the *only* missing input on the lowering path. The projection selects
    the faithful `target-array` export sink, and a resident (memory-loaded) base or descriptor
    becomes a `pointer`/`descriptor` cell in the parent view, which is exactly the cell a
    two-level `s_load_dwordx4 s[..], [s0+off]` of that view produces.
    """
    problems = _check_document(manifest)
    if problems:
        raise AbiError('manifest is not well formed: ' + problems[0]['message'])
    abi: dict = {'schema': 'bounded-ssa-msl-v1', 'sha256': manifest['shader']['sha256'],
                 'initial_scalar': {}, 'initial_vector': {}, 'views': {}, 'streams': {},
                 'export_sink': 'target-array'}

    def project(section: str, binding: dict) -> None:
        if 'via' not in binding:
            return
        parent = _resolve_view(manifest, int(binding['via']))
        if parent is None:
            raise AbiError(f'{section} slot {binding["view"]}: no binding names a view for '
                           f's{binding["via"]}, so the resident cell has no parent view')
        cell = {'kind': binding['kind'], 'view': binding['view']}
        table = abi['views'].setdefault(parent, {})
        previous = table.get(str(binding['offset']))
        if previous is not None and previous != cell:
            raise AbiError(f'view {parent!r} dword {binding["offset"]} would be both '
                           f'{previous["kind"]} and {cell["kind"]}')
        table[str(binding['offset'])] = cell

    for slot, binding in sorted(manifest.get('scalar_bases', {}).items(),
                                key=lambda kv: int(kv[0])):
        if 'via' in binding:
            project('scalar_bases', binding)
        else:
            abi['initial_scalar'][slot] = {'kind': binding['kind'], 'view': binding['view']}
    for slot, binding in sorted(manifest.get('buffer_descriptors', {}).items(),
                                key=lambda kv: int(kv[0])):
        if 'via' in binding:
            project('buffer_descriptors', binding)
        else:
            abi['initial_scalar'][slot] = {'kind': 'descriptor', 'view': binding['view']}
        abi['streams'][binding['view']] = {'format': binding['format'],
                                           'field': binding['field']}
    abi['initial_scalar'].update(manifest.get('initial_scalars') or {})
    abi['initial_vector'].update(manifest.get('initial_vectors') or {})
    for name, entries in sorted((manifest.get('views') or {}).items()):
        table = abi['views'].setdefault(name, {})
        for key, value in sorted(entries.items(), key=lambda kv: int(kv[0])):
            previous = table.get(str(key))
            if previous is not None and previous != value:
                raise AbiError(f'view {name!r} dword {key} is declared twice with different '
                               f'cells')
            table[str(key)] = value
    for name, table in abi['views'].items():
        abi['views'][name] = {key: table[key] for key in sorted(table, key=int)}
    policy = manifest.get('policy') or {}
    for key in ('interpolation', 'vertex_select', 'vertex_mov_subtract_p10'):
        if key in policy:
            abi[key] = policy[key]
    return abi


def to_lowerer_pair(vs_manifest: dict, ps_manifest: dict) -> dict:
    """Project two filled manifests onto the pair ABI the closed pair harness accepts.

    The pair profile (`gcn_to_msl.generate`) composes the *named* sink, which writes exactly
    `{12,32,33}` for a vertex stage and `{0}` for a fragment stage. This projection therefore
    refuses a stage that exports any other set: such a stage is lowered with
    `gcn_to_msl.generate_stage()` and the `target-array` sink, one stage at a time.
    """
    if vs_manifest['shader']['stage'] != 'vs' or ps_manifest['shader']['stage'] != 'ps':
        raise AbiError('to_lowerer_pair() takes one vs manifest and one ps manifest')
    profile = ((vs_manifest.get('policy') or {}).get('floating_point')
               or (ps_manifest.get('policy') or {}).get('floating_point'))
    if profile not in FLOATING_POINT_PROFILES:
        raise AbiError('both manifests must declare a supported policy.floating_point')
    for name, manifest in (('vs', vs_manifest), ('ps', ps_manifest)):
        expected = set(HARNESS_EXPORT_CONTRACT[name])
        observed = {int(key) for key in (manifest.get('exports') or {})}
        if observed != expected:
            raise AbiError(f'the pair harness writes exactly {sorted(expected)} for a {name} '
                           f'stage; this manifest exports {sorted(observed)} - lower it with '
                           f'gcn_to_msl.generate_stage() instead')
    vs, ps = to_lowerer_stage_abi(vs_manifest), to_lowerer_stage_abi(ps_manifest)
    vs.pop('export_sink', None)
    ps.pop('export_sink', None)
    return {'schema': 'bounded-ssa-msl-v1', 'floating_point': profile, 'vs': vs, 'ps': ps}


# --------------------------------------------------------------------------------------------
# Rendering.
# --------------------------------------------------------------------------------------------

def render_holes(analysis: dict, identity: str | None = None) -> list[str]:
    counts = analysis['counts']
    lines = [
        f'shader  : {identity or "-"}  stage={analysis["stage"]}  bytes={analysis["bytes"]}  '
        f'sha256={analysis["sha256"]}',
        f'decode  : {analysis["instructions"]} instructions, '
        f'per-lane eligible={analysis["per_lane_eligible"]}',
        f'holes   : {counts["scalar_bases"]} scalar base(s), '
        f'{counts["buffer_descriptors"]} buffer descriptor(s), '
        f'{counts.get("resident_slots", 0)} resident slot(s), '
        f'{counts["export_targets"]} export target(s), '
        f'{counts.get("null_exports", 0)} null export(s), '
        f'{counts["initial_vectors"]} initial vector(s), '
        f'{counts["initial_scalars"]} initial scalar(s), '
        f'{counts["policy"]} policy knob(s), '
        f'{counts["view_dword_requests"]} scalar view dword request(s)',
    ]
    for entry in analysis['holes']['scalar_bases']:
        dwords = ', '.join(f'[{r["offset"]}..{r["offset"] + r["dwords"] - 1}]'
                           for r in entry['view_dwords'])
        lines.append(f'  scalar_base        s{entry["slot"]:<3} {entry["kind"]:<10} '
                     f'{entry["width"]} cells  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}'
                     + (f'  view dwords {dwords}' if dwords else ''))
    for entry in analysis['holes']['buffer_descriptors']:
        lines.append(f'  buffer_descriptor  s{entry["slot"]:<3} descriptor  needs format '
                     f'{"/".join(entry["formats"])} + a named field  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}')
    for entry in analysis['holes'].get('resident_slots') or []:
        dwords = ', '.join(f'[{r["offset"]}..{r["offset"] + r["dwords"] - 1}]'
                           for r in entry['view_dwords'])
        extra = (f'  needs format {"/".join(entry["formats"])}'
                 if entry.get('formats') else '')
        lines.append(f'  resident_{entry["role"]:<9} s{entry["slot"]:<3} {entry["kind"]:<10} '
                     f'{entry["width"]} cell(s) at dword {entry["offset"]} of the view '
                     f's{entry["via"]} addresses  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}{extra}'
                     + (f'  own view dwords {dwords}' if dwords else ''))
    for entry in analysis['holes'].get('null_exports') or []:
        lines.append(f'  null_export        target {entry["target"]:<3} writes no component'
                     + ('  (valid mask)' if entry['valid_mask'] else '')
                     + f'  site {_hex(entry["offset"])}')
    for entry in analysis['holes']['export_targets']:
        lines.append(f'  export_target      MRT {entry["target"]:<3} sources '
                     f'{entry["sources"]}  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}')
    for entry in analysis['holes']['initial_vectors']:
        role = f'  role {entry["role"]}' if entry['role'] else ''
        lines.append(f'  initial_vector     v{entry["slot"]:<3} read before written  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}{role}')
    for entry in analysis['holes']['initial_scalars']:
        role = f'  role {entry["role"]}' if entry['role'] else ''
        lines.append(f'  initial_scalar     s{entry["slot"]:<3} read before written  sites '
                     f'{", ".join(_hex(s) for s in entry["sites"])}{role}')
    for entry in analysis['holes']['policy']:
        detail = (f'selectors {entry["selectors"]}' if entry.get('selectors') else
                  str(entry['value']))
        lines.append(f'  policy             {entry["name"]} = {detail}  '
                     f'({entry["reason"]})')
    if analysis['shape']:
        lines.append(f'shape   : {len(analysis["shape"])} blocker(s) a manifest cannot fix')
        for blocker in analysis['shape']:
            lines.append(f'  {_hex(blocker["offset"]):>7} {blocker["mnemonic"]:<22} '
                         f'{blocker["reason"]}')
    else:
        lines.append('shape   : none (a filled manifest is the only missing input)')
    return lines


# --------------------------------------------------------------------------------------------
# Corpus census (private output only).
# --------------------------------------------------------------------------------------------

def _load_coverage():
    """Load the sibling census module for its sampler, container reader and stage split."""
    spec = importlib.util.spec_from_file_location('msl_corpus_coverage_for_abi',
                                                  HERE / 'msl_corpus_coverage.py')
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def resolve_output(out: Path) -> Path:
    resolved = out.resolve()
    local = (ROOT / 'local').resolve()
    if resolved.is_relative_to(ROOT.resolve()) and not resolved.is_relative_to(local):
        raise AbiError('private census output inside this repo must stay under local/')
    return resolved


def census(archives: list[Path], sample: int, top: int, out: Path,
           overwrite: bool = False) -> tuple[dict, Path]:
    """Measure the manifest hole list over a deterministic sample of the private corpus.

    Nothing is lowered: rows are (member, stage row) pairs that decode linearly and pass the
    decoder's per-lane gate, i.e. exactly the population that could reach a manifest. Only paths,
    digests, sizes, slot numbers, instruction offsets and the decoder's own strings are recorded.
    """
    cov = _load_coverage()
    target = resolve_output(out)
    missing = [str(p) for p in archives if not p.is_file()]
    if missing:
        raise AbiError('private archive(s) missing, no census produced:\n  ' + '\n  '.join(missing))
    identities, population = [], []
    for path in archives:
        path = path.resolve()
        archive = cov.Psarc(path)
        try:
            members = archive.members()
            csdr = [(name, index) for name, index in members if name.endswith('.csdr')]
        finally:
            archive.close()
        relative = (str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path))
        identities.append({'name': path.name, 'path': relative, 'bytes': path.stat().st_size,
                           'members': len(members), 'csdr_members': len(csdr)})
        population.extend((name, index, path) for name, index in csdr)
    population.sort(key=lambda item: (item[0], item[2].name))
    indices = cov.sample_indices(len(population), sample)

    handles: dict[Path, object] = {}
    members = rows = 0
    counters = Counter()
    eligible: list[dict] = []
    try:
        for position in indices:
            name, index, path = population[position]
            archive = handles.get(path)
            if archive is None:
                archive = handles[path] = cov.Psarc(path)
            data = archive.read(index)
            members += 1
            try:
                blobs = cov.parse_raw_csdr(data)
            except cov.ContainerError as exc:
                counters['compressed-prefix' if data[:2] in cov.OODLE_LIKE_PREFIXES
                         else 'container-rejected'] += 1
                continue
            for blob in blobs:
                code = data[blob['code_offset']:blob['code_offset'] + blob['length']]
                spans, _split = cov.split_stages(code)
                label = cov.STAGE_BY_TYPE.get(blob['type'])
                for span in spans:
                    rows += 1
                    try:
                        analysis = analyze(span, label if label in STAGES else None)
                    except AbiError:
                        counters['not-linear'] += 1
                        continue
                    if not analysis['per_lane_eligible']:
                        counters['per-lane-blocked'] += 1
                        continue
                    if label not in STAGES:
                        counters['other-stage'] += 1
                        continue
                    counters['per-lane-eligible'] += 1
                    analysis.update({'archive': path.name, 'path': name,
                                     'member_bytes': len(data),
                                     'stage_evidence': 'INF filename co-occurrence; not '
                                                       'verified semantics'})
                    eligible.append(analysis)
    finally:
        for archive in handles.values():
            archive.close()

    eligible.sort(key=lambda row: (-row['bytes'], row['path']))
    named = eligible[:top]
    aggregate = _aggregate(eligible, 'all per-lane-eligible programs in the sample')
    top_aggregate = _aggregate(named, f'the {len(named)} largest per-lane-eligible programs')
    document = {
        'schema': CENSUS_SCHEMA,
        'sample': {'requested': sample, 'selected': len(indices), 'population': len(population),
                   'rule': 'gcn sampling rule from msl_corpus_coverage.sample_indices: even '
                           'spacing round(i*(N-1)/(k-1)) over the sorted [(path, archive)] list',
                   'indices': indices, 'archives': identities},
        'eligibility': {'members_read': members, 'program_rows': rows,
                        'not_linear': counters['not-linear'],
                        'per_lane_blocked': counters['per-lane-blocked'],
                        'per_lane_eligible': counters['per-lane-eligible'],
                        'other_stage': counters['other-stage'],
                        'compressed_prefix': counters['compressed-prefix'],
                        'container_rejected': counters['container-rejected']},
        'aggregate_all': aggregate,
        'aggregate_top': top_aggregate,
        'top': [{'path': row['path'], 'archive': row['archive'],
                 'stage': row['stage'], 'code_bytes': row['bytes'],
                 'code_sha256': row['sha256'], 'instructions': row['instructions'],
                 'counts': row['counts'], 'holes': row['holes'], 'derived': row['derived'],
                 'manifest_only': row['manifest_only'], 'shape': row['shape']}
                for row in named],
        'tool': {'path': str(Path(__file__).resolve().relative_to(ROOT)),
                 'sha256': hashlib.sha256(Path(__file__).resolve().read_bytes()).hexdigest(),
                 'python': sys.version.split()[0]},
        'limits': ['decodes and counts only: nothing is lowered, compiled or executed',
                   'stage is the INF filename nibble, not verified semantics',
                   'slots, counts, offsets and digests only; no payload bytes or listings',
                   'shape blockers are reported because no manifest can satisfy them'],
    }
    target.mkdir(parents=True, exist_ok=True)
    path_out = target / 'abi_holes.json'
    if path_out.exists() and not overwrite:
        raise AbiError(f'{path_out} already exists; pass --overwrite to replace earlier evidence')
    path_out.write_text(json.dumps(document, indent=2, sort_keys=True) + '\n')
    return document, path_out


def _distribution(values: list[int]) -> dict:
    if not values:
        return {'count': 0, 'min': None, 'median': None, 'max': None}
    ordered = sorted(values)
    middle = len(ordered) // 2
    median = (ordered[middle] if len(ordered) % 2
              else (ordered[middle - 1] + ordered[middle]) / 2)
    return {'count': len(ordered), 'min': ordered[0], 'median': median, 'max': ordered[-1]}


def _aggregate(rows: list[dict], label: str) -> dict:
    bases = Counter()
    descriptors = Counter()
    residents = Counter()
    targets = Counter()
    nulls = Counter()
    vectors = Counter()
    scalars = Counter()
    policies = Counter()
    formats = Counter()
    shape_kinds = Counter()
    view_dwords = 0
    manifest_only = 0
    for row in rows:
        holes = row['holes']
        for entry in row['shape']:
            shape_kinds[entry.get('kind', 'other')] += 1
        for entry in holes['scalar_bases']:
            bases[entry['slot']] += 1
            view_dwords += len(entry['view_dwords'])
        for entry in holes['buffer_descriptors']:
            descriptors[entry['slot']] += 1
            for name in entry['formats']:
                formats[name] += 1
        for entry in holes.get('resident_slots') or []:
            residents[entry['slot']] += 1
            view_dwords += len(entry['view_dwords'])
            for name in entry.get('formats') or ():
                formats[name] += 1
        for entry in holes.get('null_exports') or []:
            nulls[entry['target']] += 1
        for entry in holes['export_targets']:
            targets[entry['target']] += 1
        for entry in holes['initial_vectors']:
            vectors[entry['slot']] += 1
        for entry in holes['initial_scalars']:
            scalars[entry['slot']] += 1
        for entry in holes['policy']:
            policies[entry['name']] += 1
        manifest_only += 1 if row['manifest_only'] else 0
    return {
        'label': label,
        'shaders': len(rows),
        'manifest_only_shaders': manifest_only,
        'shape_blocked_shaders': len(rows) - manifest_only,
        'code_bytes': _distribution([row['bytes'] for row in rows]),
        'instructions': _distribution([row['instructions'] for row in rows]),
        'distinct_scalar_base_slots': len(bases),
        'distinct_buffer_descriptor_slots': len(descriptors),
        'distinct_resident_slots': len(residents),
        'distinct_export_targets': len(targets),
        'distinct_null_export_targets': len(nulls),
        'distinct_initial_vector_slots': len(vectors),
        'distinct_initial_scalar_slots': len(scalars),
        'scalar_base_slot_histogram': dict(sorted(bases.items())),
        'buffer_descriptor_slot_histogram': dict(sorted(descriptors.items())),
        'resident_slot_histogram': dict(sorted(residents.items())),
        'export_target_histogram': dict(sorted(targets.items())),
        'null_export_target_histogram': dict(sorted(nulls.items())),
        'initial_vector_slot_histogram': dict(sorted(vectors.items())),
        'initial_scalar_slot_histogram': dict(sorted(scalars.items())),
        'policy_histogram': dict(sorted(policies.items())),
        'descriptor_format_histogram': dict(sorted(formats.items())),
        'shape_kind_histogram': dict(sorted(shape_kinds.items())),
        'scalar_view_dword_requests': view_dwords,
    }


# --------------------------------------------------------------------------------------------
# Self-test: synthetic spans only, no private archive touched.
# --------------------------------------------------------------------------------------------

def _self_test() -> int:
    checks = 0

    def check(name: str, condition: bool) -> None:
        nonlocal checks
        if not condition:
            raise AssertionError(f'self-test failed: {name}')
        checks += 1

    def encode(form: str, op: int, **values) -> bytes:
        """Encode one instruction from the decoder's own field table (no bytes are borrowed)."""
        _form, _mask, tag, shift, opmask, count = next(
            s for s in gcn.FORMATS if s[0] == form)
        word = tag | ((op & opmask) << shift)
        for name, low, width in gcn.FIELDS[form]:
            if name in values:
                word |= (values[name] & ((1 << width) - 1)) << low
        return word.to_bytes(count * 4, 'little')

    def program(*instructions: bytes) -> bytes:
        return b''.join(instructions)

    def codes(problems: list[dict]) -> list[str]:
        return [p['code'] for p in problems]

    # The decoder and this tool must agree on what a span is.
    smrd = lambda op, **kw: encode('SMRD', op, imm=1, **kw)          # noqa: E731
    mubuf = lambda op, **kw: encode('MUBUF', op, **kw)               # noqa: E731
    vop1 = lambda op, **kw: encode('VOP1', op, **kw)                 # noqa: E731
    vop2 = lambda op, **kw: encode('VOP2', op, **kw)                 # noqa: E731
    exp = lambda target, **kw: encode('EXP', 0, en=15, target=target, done=1, **kw)
    end = encode('SOPP', 1)

    # A small synthetic VS: two scalar loads, one formatted fetch, one move, one multiply,
    # three exports.  Built here so the hole list can be asserted exactly.
    vs = program(
        smrd(0, offset=0, sbase=0, sdst=4),                      # base s0 (pointer), s4 = load
        smrd(1, offset=2, sbase=4, sdst=6),                      # base s8 (pointer), s6..s7
        mubuf(3, srsrc=5, vaddr=0, vdata=4, idxen=1, soffset=128),  # descriptor s20, v0 index
        vop1(1, src0=260, vdst=8),                               # v8 = v4
        vop2(8, src0=264, vsrc1=10, vdst=9),                     # v9 = v8 * v10
        exp(12, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9),
        exp(32, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9),
        exp(33, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9),
        end)
    check('synthetic vs decodes', len(gcn.decode(vs)) == 9)
    analysis = analyze(vs, 'vs')
    holes = analysis['holes']
    check('vs scalar bases', [e['slot'] for e in holes['scalar_bases']] == [0, 8])
    check('vs scalar base kind', [e['kind'] for e in holes['scalar_bases']]
          == ['pointer', 'pointer'])
    check('vs first base view dwords',
          holes['scalar_bases'][0]['view_dwords'] == [{'offset': 0, 'dwords': 1}])
    check('vs second base view dwords',
          holes['scalar_bases'][1]['view_dwords'] == [{'offset': 2, 'dwords': 2}])
    check('vs descriptors', [e['slot'] for e in holes['buffer_descriptors']] == [20])
    check('vs descriptor format', holes['buffer_descriptors'][0]['formats'] == ['explicit_float4'])
    check('vs export targets', [e['target'] for e in holes['export_targets']] == [12, 32, 33])
    check('vs initial vectors', [e['slot'] for e in holes['initial_vectors']] == [0, 10])
    check('vs vaddr role', holes['initial_vectors'][0]['role'] == 'vertex_index')
    check('vs no initial scalars', holes['initial_scalars'] == [])
    check('vs no policy', holes['policy'] == [])
    check('vs manifest only', analysis['manifest_only'])

    # Skeleton: exactly the holes, nulled, and the two forced values pre-filled.
    form = skeleton(analysis, 'synthetic_vs')
    check('skeleton schema', form['schema'] == SCHEMA)
    check('skeleton base slots', sorted(form['scalar_bases']) == ['0', '8'])
    check('skeleton base unfilled', all(v is None for v in form['scalar_bases'].values()))
    check('skeleton descriptors', sorted(form['buffer_descriptors']) == ['20'])
    check('skeleton exports', sorted(form['exports']) == ['12', '32', '33'])
    check('skeleton v0 pre-filled', form['initial_vectors']['0'] == {'kind': 'vertex_index'})
    check('skeleton v10 unfilled', form['initial_vectors']['10'] is None)
    check('skeleton floating point', form['policy']['floating_point'] == FLOATING_POINT_PROFILES[0])

    # An unfilled skeleton fails closed, one diagnostic per hole.
    unfilled = validate(form, analysis)
    check('unfilled skeleton fails', unfilled)
    check('one unfilled diagnostic per scalar base',
          codes(unfilled).count('unfilled-slot') >= 5)
    check('unfilled diagnostics are not generic',
          all(p['slot'] for p in unfilled if p['code'] == 'unfilled-slot'))

    # A correctly filled manifest validates with no diagnostics.
    filled = {
        'schema': SCHEMA,
        'shader': {'id': 'synthetic_vs', 'stage': 'vs', 'sha256': analysis['sha256'],
                   'bytes': analysis['bytes']},
        'provenance': {'bindings': 'synthetic', 'note': 'self-test'},
        'scalar_bases': {'0': {'kind': 'pointer', 'view': 'root'},
                         '8': {'kind': 'pointer', 'view': 'constants'}},
        'buffer_descriptors': {'20': {'kind': 'descriptor', 'view': 'streams',
                                      'format': 'explicit_float4', 'field': 'position'}},
        'exports': {'12': {'semantic': 'clip', 'sources': [9, 9, 9, 9]},
                    '32': {'semantic': 'param0', 'sources': [9, 9, 9, 9]},
                    '33': {'semantic': 'param1', 'sources': [9, 9, 9, 9]}},
        'initial_scalars': {},
        'initial_vectors': {'0': {'kind': 'vertex_index'},
                            '10': {'kind': 'float', 'field': 'camera', 'component': 'x'}},
        'views': {'root': {'0': {'kind': 'pointer', 'view': 'streams'}},
                  'constants': {'2': {'kind': 'float', 'field': 'camera', 'component': 'x'},
                                '3': {'kind': 'float', 'field': 'camera', 'component': 'y'}}},
        'policy': {'floating_point': FLOATING_POINT_PROFILES[0]},
    }
    check('filled manifest validates', validate(filled, analysis) == [])

    def fill(analysed: dict) -> dict:
        """The module helper, so the closability claim is tested once and reused."""
        return synthetic_fill(analysed)

    check('skeleton is closable', validate(fill(analysis), analysis) == [])

    # Projection onto the lowerer's per-stage dict.
    abi = to_lowerer_stage_abi(filled)
    check('projection initial scalar', abi['initial_scalar']['0']
          == {'kind': 'pointer', 'view': 'root'})
    check('projection descriptor binding',
          abi['initial_scalar']['20'] == {'kind': 'descriptor', 'view': 'streams'})
    check('projection stream', abi['streams']['streams']
          == {'format': 'explicit_float4', 'field': 'position'})
    check('projection digest', abi['sha256'] == analysis['sha256'])
    check('projection keeps vector kind', abi['initial_vector']['0'] == {'kind': 'vertex_index'})

    _DROP = object()

    def mutated(**changes) -> dict:
        document = json.loads(json.dumps(filled))
        for key, value in changes.items():
            if value is _DROP:
                document.pop(key, None)
            else:
                document[key] = value
        return document

    # Missing scalar base: precise slot and site.
    missing_base = validate(mutated(scalar_bases={'0': {'kind': 'pointer', 'view': 'root'}}),
                            analysis)
    check('missing base code', 'missing-scalar-base' in codes(missing_base))
    check('missing base slot', any(p['slot'] == 's8' for p in missing_base))
    check('missing base message names the site',
          any('0x4' in p['message'] for p in missing_base if p['slot'] == 's8'))

    # Wrong kind: the shader reads it as a pointer.
    wrong_kind = validate(mutated(scalar_bases={
        '0': {'kind': 'descriptor', 'view': 'root'},
        '8': {'kind': 'pointer', 'view': 'constants'}}), analysis)
    check('wrong kind code', 'wrong-kind' in codes(wrong_kind))
    check('wrong kind slot', any(p['slot'] == 'scalar_base s0' for p in wrong_kind))

    # Wrong buffer format.
    wrong_format = validate(mutated(buffer_descriptors={
        '20': {'kind': 'descriptor', 'view': 'streams', 'format': 'explicit_float2',
               'field': 'position'}}), analysis)
    check('wrong format code', 'wrong-format' in codes(wrong_format))

    # Undeclared export target.
    undeclared = validate(mutated(exports={
        '12': {'semantic': 'clip', 'sources': [9, 9, 9, 9]},
        '32': {'semantic': 'param0', 'sources': [9, 9, 9, 9]}}), analysis)
    check('undeclared export code', 'missing-export-target' in codes(undeclared))
    check('undeclared export slot', any(p['slot'] == '33' for p in undeclared))

    # A binding for a slot the program never touches.
    extra = validate(mutated(scalar_bases={
        '0': {'kind': 'pointer', 'view': 'root'},
        '8': {'kind': 'pointer', 'view': 'constants'},
        '12': {'kind': 'pointer', 'view': 'unused'}}), analysis)
    check('unused binding code', 'unused-scalar-base' in codes(extra))
    check('unused binding message', any('never uses it' in p['message'] for p in extra))

    # Export source pin that disagrees with the program.
    bad_source = validate(mutated(exports={
        '12': {'semantic': 'clip', 'sources': [1, 2, 3, 4]},
        '32': {'semantic': 'param0', 'sources': [9, 9, 9, 9]},
        '33': {'semantic': 'param1', 'sources': [9, 9, 9, 9]}}), analysis)
    check('export source mismatch code', 'export-source-mismatch' in codes(bad_source))

    # Digest and bytes.
    digest = validate(mutated(shader={'id': 'synthetic_vs', 'stage': 'vs',
                                      'sha256': '0' * 64, 'bytes': analysis['bytes']}), analysis)
    check('digest mismatch', codes(digest) == ['digest'])
    size = validate(mutated(shader={'id': 'synthetic_vs', 'stage': 'vs',
                                    'sha256': analysis['sha256'],
                                    'bytes': analysis['bytes'] + 4}), analysis)
    check('bytes mismatch', codes(size) == ['bytes'])
    stage = validate(mutated(shader={'id': 'synthetic_vs', 'stage': 'ps',
                                     'sha256': analysis['sha256'],
                                     'bytes': analysis['bytes']}), analysis)
    check('stage mismatch', 'stage' in codes(stage))

    # Malformed documents: unknown field, bad schema, non-object, bad binding.
    check('unknown top-level field',
          codes(validate(mutated(extra_field={}), analysis))[0] == 'unknown-field')
    check('bad schema', codes(validate(mutated(schema='msl-abi-manifest-v0'), analysis))[0]
          == 'schema')
    check('non-object', codes(validate([], analysis))[0] == 'not-an-object')
    check('invalid binding kind',
          'invalid-binding' in codes(validate(mutated(scalar_bases={
              '0': {'kind': 'texture', 'view': 'root'},
              '8': {'kind': 'pointer', 'view': 'constants'}}), analysis)))
    check('bad sha shape',
          'digest' in codes(validate(mutated(shader={'id': 'x', 'stage': 'vs', 'sha256': 'abc',
                                                     'bytes': 4}), analysis)))
    check('bad slot key',
          'slot-key' in codes(validate(mutated(scalar_bases={
              's0': {'kind': 'pointer', 'view': 'root'}}), analysis)))

    # Views: a missing dword is reported with the dword number, an extra one as unused.
    check('missing view dword',
          'missing-view-dword' in codes(validate(mutated(views={
              'root': {'0': {'kind': 'pointer', 'view': 'streams'}},
              'constants': {'2': {'kind': 'float', 'field': 'camera', 'component': 'x'}}}),
              analysis)))
    check('unused view entry',
          'unused-view-entry' in codes(validate(mutated(views={
              'root': {'0': {'kind': 'pointer', 'view': 'streams'}},
              'constants': {'2': {'kind': 'float', 'field': 'camera', 'component': 'x'},
                            '3': {'kind': 'float', 'field': 'camera', 'component': 'y'},
                            '12': {'kind': 'float', 'field': 'camera', 'component': 'z'}}}),
              analysis)))
    check('missing view', 'missing-view' in codes(validate(mutated(views={}), analysis)))
    check('unused policy knob',
          'unused-policy' in codes(validate(mutated(
              policy={'floating_point': FLOATING_POINT_PROFILES[0],
                      'interpolation': 'supplied_vertex_values'}), analysis)))

    # Shape blockers: a manifest cannot fix an unsupported row, so validation fails closed.
    additive = program(vop1(1, src0=260, vdst=8),
                       vop1(53, src0=256, vdst=9),   # v_sin_f32: decodable, not a lowering row
                       exp(12, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9),
                       exp(32, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9),
                       exp(33, vsrc0=9, vsrc1=9, vsrc2=9, vsrc3=9), end)
    additive_holes = analyze(additive, 'vs')
    check('additive is not manifest-only', not additive_holes['manifest_only'])
    check('additive blocker names the row',
          any('v_sin_f32' in b['reason'] for b in additive_holes['shape']))
    check('shape blocker reaches the validator',
          'shape-blocker' in codes(validate(fill(additive_holes), additive_holes)))

    # The synthetic harness export contract is gone: a vertex stage that writes clip plus one
    # param is lowerable, because the target-array sink is indexed by target.
    partial = program(vop1(1, src0=260, vdst=8),
                      exp(12, vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8),
                      exp(32, vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8), end)
    partial_holes = analyze(partial, 'vs')
    check('clip plus one param is manifest-only', partial_holes['manifest_only'])
    check('its export holes are the targets it writes',
          [e['target'] for e in partial_holes['holes']['export_targets']] == [12, 32])
    check('clip plus one param fill validates',
          validate(fill(partial_holes), partial_holes) == [])

    # What a vertex stage must still do is position itself: no full clip export is a blocker.
    no_clip = program(vop1(1, src0=260, vdst=8),
                      exp(32, vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8), end)
    no_clip_holes = analyze(no_clip, 'vs')
    check('a vertex stage without a full clip export is blocked',
          any(blocker['kind'] == 'export-contract' for blocker in no_clip_holes['shape']))

    # A partial `en` writes only the components it enables, so no component value is invented.
    partial_en = program(vop1(1, src0=256, vdst=4),
                         encode('EXP', 0, en=3, target=0, done=1, vsrc0=4, vsrc1=4), end)
    partial_en_holes = analyze(partial_en, 'ps')
    check('partial en records the enabled components',
          [e['components'] for e in partial_en_holes['holes']['export_targets']] == [[0, 1]])
    check('partial en is manifest-only', partial_en_holes['manifest_only'])

    # A null export (en = 0) writes nothing, so it is neither an export hole nor a blocker.
    null_export = program(encode('EXP', 0, en=0, target=9, done=1, vm=1), end)
    null_holes = analyze(null_export, 'ps')
    check('a null export is not an export hole', null_holes['holes']['export_targets'] == [])
    check('a null export is recorded with its valid mask',
          [(e['target'], e['valid_mask']) for e in null_holes['holes']['null_exports']]
          == [(9, True)])
    check('a null export with the valid mask is manifest-only', null_holes['manifest_only'])
    check('a null export needs no binding at all', null_holes['counts']['binding_holes'] == 0)

    # The valid mask is EXEC, i.e. the covered-lane mask a per-lane harness already evaluates, so
    # on a fragment stage it is recorded rather than re-emitted; on a vertex stage the encoding
    # gives it no meaning and it is refused.
    vm_components = program(vop1(1, src0=256, vdst=4),
                            encode('EXP', 0, en=15, target=0, done=1, vm=1,
                                   vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4), end)
    vm_holes = analyze(vm_components, 'ps')
    check('a fragment valid-mask export is recorded',
          vm_holes['holes']['valid_mask_exports'] == [0])
    check('a fragment valid-mask export is manifest-only', vm_holes['manifest_only'])
    vm_vertex = program(vop1(1, src0=260, vdst=8),
                        encode('EXP', 0, en=15, target=12, done=1, vm=1,
                               vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8),
                        exp(32, vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8),
                        exp(33, vsrc0=8, vsrc1=8, vsrc2=8, vsrc3=8), end)
    vm_vertex_holes = analyze(vm_vertex, 'vs')
    check('a vertex valid-mask export is blocked',
          any(blocker['kind'] == 'export-shape' and 'fragment stage' in blocker['reason']
              for blocker in vm_vertex_holes['shape']))

    # Two-level addressing: a descriptor and a base that are *loaded* from a bound view. The
    # lowering already accepts a memory-resident descriptor (the cell is a `descriptor` in the
    # parent view), so this is a hole, not a shape blocker.
    two_level = program(
        smrd(2, offset=0, sbase=0, sdst=16),                       # s[16:19] <- root[0..3]
        smrd(2, offset=4, sbase=8, sdst=20),                       # s[20:23] <- [s16 + 4]
        mubuf(3, srsrc=5, vaddr=0, vdata=4, idxen=1, soffset=128),  # descriptor s20
        exp(12, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4),
        exp(32, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4),
        exp(33, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4), end)
    two_level_holes = analyze(two_level, 'vs')
    check('a memory-resident base and descriptor are holes',
          [(e['slot'], e['kind'], e['via'], e['offset'])
           for e in two_level_holes['holes']['resident_slots']]
          == [(16, 'pointer', 0, 0), (20, 'descriptor', 16, 4)])
    check('two-level program is manifest-only', two_level_holes['manifest_only'])
    check('the root keeps only its unused dwords as data',
          [(r['offset'], r['dwords'])
           for r in two_level_holes['holes']['scalar_bases'][0]['view_dwords']]
          == [(2, 2)])
    check('two-level fill validates', validate(fill(two_level_holes), two_level_holes) == [])
    check('a resident descriptor projects into the parent view',
          to_lowerer_stage_abi(fill(two_level_holes))['views']['view_16']
          == {'4': {'kind': 'descriptor', 'view': 'view_20'}})

    # One dword cannot be two cell types: that is a shape blocker, not a binding failure.
    conflicted = program(
        smrd(2, offset=0, sbase=0, sdst=16),        # s[16:19] <- root[0..3], used as a pointer
        smrd(2, offset=0, sbase=0, sdst=20),        # s[20:23] <- root[0..3], used as a descriptor
        smrd(1, offset=4, sbase=8, sdst=24),        # a load through s16 makes it a base
        mubuf(3, srsrc=5, vaddr=0, vdata=4, idxen=1, soffset=128),
        exp(12, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4),
        exp(32, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4),
        exp(33, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4), end)
    conflicted_holes = analyze(conflicted, 'vs')
    check('an overlapping pointer and descriptor are a shape blocker',
          any(blocker['kind'] == 'resident-kind-conflict'
              for blocker in conflicted_holes['shape']))
    check('the conflicting program is not manifest-only', not conflicted_holes['manifest_only'])


    # A synthetic PS that only exports MRT0: the pair projection needs both stages.
    ps_code = program(vop1(1, src0=256, vdst=4),
                      exp(0, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4), end)
    ps_holes = analyze(ps_code, 'ps')
    check('ps is manifest-only', ps_holes['manifest_only'])
    check('ps exports mrt0', [e['target'] for e in ps_holes['holes']['export_targets']] == [0])
    check('ps fill validates', validate(fill(ps_holes), ps_holes) == [])
    pair = to_lowerer_pair(fill(analysis), fill(ps_holes))
    check('pair schema', pair['schema'] == 'bounded-ssa-msl-v1')
    check('pair profile', pair['floating_point'] == FLOATING_POINT_PROFILES[0])
    check('pair stages', (pair['vs']['sha256'], pair['ps']['sha256'])
          == (analysis['sha256'], ps_holes['sha256']))
    try:
        to_lowerer_pair(fill(ps_holes), fill(analysis))
    except AbiError as exc:
        check('pair refuses swapped stages', 'vs manifest' in str(exc))
    else:
        raise AssertionError('self-test failed: pair projection accepted a ps as vs')

    # VINTRP 2: the vertex-select map is a hole, the mode is forced by the encoding.
    interp = program(encode('VINTRP', 2, vsrc=1, attrchan=0, attr=0, vdst=2),
                     exp(12, vsrc0=2, vsrc1=2, vsrc2=2, vsrc3=2),
                     exp(32, vsrc0=2, vsrc1=2, vsrc2=2, vsrc3=2),
                     exp(33, vsrc0=2, vsrc1=2, vsrc2=2, vsrc3=2), end)
    interp_holes = analyze(interp, 'vs')
    check('interp mode forced',
          [p['value'] for p in interp_holes['holes']['policy'] if p['name'] == 'interpolation']
          == ['supplied_vertex_values'])
    interp_form = skeleton(interp_holes, 'synthetic_interp')
    check('interp skeleton mode pre-filled',
          interp_form['policy']['interpolation'] == 'supplied_vertex_values')
    check('interp skeleton select unfilled', interp_form['policy']['vertex_select'] is None)
    check('interp selectors recorded',
          [p['selectors'] for p in interp_holes['holes']['policy']
           if p['name'] == 'vertex_select'] == [[1]])

    # A block-only program cannot reach the lowerer and has no manifest to build.
    branch = encode('SOPP', 5, simm16=1) + vop1(1, src0=260, vdst=8) + end
    try:
        analyze(branch, 'vs')
    except AbiError as exc:
        check('branch program refused', 'does not decode linearly' in str(exc))
    else:
        raise AssertionError('self-test failed: a branch program was analysed')

    # Constants used here must match the lowering module, or the form is unfillable.
    spec = importlib.util.spec_from_file_location('gcn_to_msl_for_abi',
                                                  HERE / 'gcn_to_msl.py')
    lowering = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(lowering)
    check('case fields match gcn_to_msl', tuple(lowering.CASE_FIELDS) == CASE_FIELDS)
    check('export spaces match gcn_to_msl', tuple(lowering.EXPORT_SPACES) == EXPORT_SPACES)
    check('sink slots match the export spaces',
          sum(high - low for _, low, high in EXPORT_SPACES) == lowering.EXPORT_SINK_SLOTS)
    check('null export target matches gcn_to_msl',
          lowering.NULL_EXPORT_TARGET == NULL_EXPORT_TARGET)
    check('lowered arithmetic matches gcn_to_msl',
          frozenset(LOWERED_ARITHMETIC) == frozenset(lowering.ARITHMETIC_ROWS))
    check('floating point profile matches', list(FLOATING_POINT_PROFILES) == ['nearest_even_ftz32_rtz16'])

    # Aggregation smoke test: counts are over distinct slots, not occurrences.
    aggregate = _aggregate([analysis, additive_holes], 'two rows')
    check('aggregate shader count', aggregate['shaders'] == 2)
    check('aggregate distinct bases', aggregate['distinct_scalar_base_slots'] == 2)
    check('aggregate manifest-only count', aggregate['manifest_only_shaders'] == 1)

    print(f'msl abi manifest self-test: {checks} checks passed (synthetic spans only)')
    return 0


# --------------------------------------------------------------------------------------------
# CLI.
# --------------------------------------------------------------------------------------------

def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--program', type=Path, help='exact little-endian program span (private)')
    parser.add_argument('--stage', choices=STAGES,
                        help='stage for --program; unknown stages skip the export contract')
    parser.add_argument('--json', action='store_true', help='hole list as JSON')
    parser.add_argument('--skeleton', type=Path,
                        help='write an unfilled msl-abi-manifest-v1 form to this path')
    parser.add_argument('--manifest', type=Path,
                        help='validate this filled manifest against --program')
    parser.add_argument('--id', help='stable shader id for the skeleton')
    parser.add_argument('--census', action='store_true', help='measure the hole list over a sample')
    parser.add_argument('--archives', nargs='*', type=Path, default=None)
    parser.add_argument('--sample', type=int, default=512)
    parser.add_argument('--top', type=int, default=20)
    parser.add_argument('--out', type=Path, default=ROOT / 'local/validation/msl-abi-holes')
    parser.add_argument('--overwrite', action='store_true')
    args = parser.parse_args(argv)
    try:
        if args.self_test:
            return _self_test()
        if args.census:
            archives = args.archives or [ROOT / name for name in (
                'local/extracted/CUSA08809/pak0.psarc',
                'local/extracted/CUSA08809/pak1.psarc',
                'local/extracted/CUSA08809/pakN.psarc')]
            document, path = census(archives, args.sample, args.top, args.out, args.overwrite)
            eligibility = document['eligibility']
            print(f"== MSL ABI hole census (decode only; nothing lowered) ==")
            print(f"members        : {eligibility['members_read']} read, "
                  f"{eligibility['program_rows']} program rows "
                  f"({eligibility['not_linear']} not linear, "
                  f"{eligibility['per_lane_blocked']} per-lane blocked, "
                  f"{eligibility['other_stage']} other stage)")
            for key in ('aggregate_all', 'aggregate_top'):
                agg = document[key]
                print(f"{key:<14} : {agg['shaders']} shaders -> "
                      f"{agg['distinct_scalar_base_slots']} scalar-base slots, "
                      f"{agg['distinct_buffer_descriptor_slots']} descriptor slots, "
                      f"{agg['distinct_export_targets']} export targets, "
                      f"{agg['distinct_initial_vector_slots']} initial vectors; "
                      f"manifest-only {agg['manifest_only_shaders']}, "
                      f"shape-blocked {agg['shape_blocked_shaders']}")
            print(f'wrote          : {path}')
            return 0
        if args.program is None:
            parser.error('pass --self-test, --census, or --program')
        manifest = None
        stage = args.stage
        if args.manifest is not None:
            manifest = json.loads(args.manifest.read_text())
            declared = (manifest.get('shader') or {}).get('stage') if isinstance(
                manifest, dict) else None
            if stage is None and declared in STAGES:
                # A manifest names its own stage; --stage only has to agree when both are given.
                stage = declared
        analysis = analyze(args.program.read_bytes(), stage)
        if manifest is not None:
            problems = validate(manifest, analysis)
            shape = [p for p in problems if p['code'] == 'shape-blocker']
            binding = [p for p in problems if p['code'] != 'shape-blocker']
            for problem in binding:
                print(f'FAILED: {problem["slot"]}: {problem["message"]}', file=sys.stderr)
            for problem in shape:
                print(f'BLOCKED: {problem["slot"]}: {problem["message"]}', file=sys.stderr)
            if binding:
                print(f'FAILED: {len(binding)} unfilled/incorrect binding(s); nothing was '
                      f'lowered', file=sys.stderr)
                return 1
            if shape:
                print(f'BLOCKED: manifest covers every hole of {analysis["sha256"][:16]}... but '
                      f'{len(shape)} lowering-shape blocker(s) remain; nothing was lowered',
                      file=sys.stderr)
                return 1
            print(f'PASSED manifest check: {args.manifest.name} covers '
                  f'{analysis["counts"]["binding_holes"]} hole(s) of '
                  f'{analysis["sha256"][:16]}... ; no lowering attempted')
            return 0
        if args.skeleton is not None:
            document = skeleton(analysis, args.id or args.program.stem, 'owner-supplied')
            resolved = args.skeleton.resolve()
            if resolved.is_relative_to(ROOT.resolve()) and not resolved.is_relative_to(
                    (ROOT / 'local').resolve()):
                raise AbiError('skeleton output inside this repo must stay under local/')
            resolved.parent.mkdir(parents=True, exist_ok=True)
            resolved.write_text(json.dumps(document, indent=2, sort_keys=True) + '\n')
            print(f'wrote skeleton: {resolved} '
                  f'({analysis["counts"]["binding_holes"]} hole(s) to fill)')
            return 0
        if args.json:
            print(json.dumps(analysis, indent=2, sort_keys=True))
        else:
            for line in render_holes(analysis, args.id or args.program.name):
                print(line)
        return 0
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print(f'FAILED: {exc}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
