#!/usr/bin/env python3
"""Offline bounded per-lane GFX7 -> ordinary MSL expressions.

The caller supplies a private ABI manifest: exact program digests, named scalar
memory views and formatted stream bindings. No game bytes, per-asset instruction
list, original addresses or guessed resources are built in. Every unbound read,
unsupported control, alias or instruction raises LoweringError. Generated MSL is
private derivative data. This tool is never linked into a game runtime.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys

spec = importlib.util.spec_from_file_location('gcn_inventory', Path(__file__).with_name('gcn_inventory.py'))
gcn = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = gcn
spec.loader.exec_module(gcn)

VERSION = 'bounded-ssa-msl-v1'
# Harness data only. These are immutable typed inputs, never guest registers.
CASE_FIELDS = ('position', 'encoded_normal', 'object_row0', 'object_row1', 'object_row2',
               'clip_row0', 'clip_row1', 'clip_row2', 'clip_row3', 'camera', 'material',
               'param0_p0', 'param0_p10', 'param0_p20', 'param1_p0', 'param1_p10', 'param1_p20',
               'barycentric')
LOGO_PS_SCHEMA = 'bounded-logo-ps-synthetic-v1'
LOGO_PS_SHA256 = '4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a'
LOGO_PS_BYTES = 152
LOGO_PS_ROWS = 34
LOGO_PAIR_SCHEMA = 'bounded-logo-pair-synthetic-v1'
LOGO_VS_SHA256 = '430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c'
LOGO_VS_BYTES = 140


class LoweringError(ValueError):
    pass


@dataclass(frozen=True)
class Cell:
    kind: str
    value: str
    part: int = 0


class Lowerer:
    def __init__(self, abi: dict, stage: str):
        self.abi = abi
        self.stage = stage
        self.s: dict[int, Cell] = {}
        self.v: dict[int, Cell] = {}
        self.lines: list[str] = []
        self.exports: dict[int, list[str]] = {}
        self.serial = 0
        self.input_reads: set[str] = set()
        self.interp_pending: dict[int, tuple[int, int]] = {}
        for n, item in abi.get('initial_scalar', {}).items():
            for j, cell in enumerate(self.bind(item)):
                slot=int(n)+j
                if not 0 <= slot < 128 or slot in self.s:
                    raise LoweringError('invalid/overlapping initial scalar binding')
                self.s[slot] = cell
        for n, item in abi.get('initial_vector', {}).items():
            cells = self.bind(item)
            if len(cells) != 1 or not 0 <= int(n) < 256 or int(n) in self.v:
                raise LoweringError('invalid/overlapping initial vector binding')
            self.v[int(n)] = cells[0]

    def bind(self, b: dict) -> list[Cell]:
        kind = b.get('kind')
        if kind in ('pointer', 'descriptor'):
            if not re.fullmatch(r'[a-z][a-z0-9_]*', b.get('view', '')):
                raise LoweringError('invalid view identity')
            return [Cell(kind, b['view'], i) for i in range(2 if kind == 'pointer' else 4)]
        if kind == 'float4':
            field = b.get('field')
            if field not in CASE_FIELDS:
                raise LoweringError('unknown named float4 input')
            return [Cell('float', f'u.{field}.{c}') for c in 'xyzw']
        if kind == 'float':
            field, component = b.get('field'), b.get('component')
            if field not in CASE_FIELDS or not isinstance(component,str) or len(component) != 1 or component not in 'xyzw':
                raise LoweringError('invalid named float input')
            return [Cell('float', f'u.{field}.{component}')]
        if kind == 'vertex_index':
            return [Cell(kind, 'one_vertex_per_case')]
        if kind == 'interpolation_control':
            return [Cell(kind, 'supplied_coefficients')]
        raise LoweringError(f'unsupported ABI cell {kind!r}')

    def pair(self, n: int, kind: str) -> str:
        count = 2 if kind == 'pointer' else 4
        cells = [self.s.get(n+i) for i in range(count)]
        if any(c is None for c in cells):
            raise LoweringError('unbound scalar resource cell')
        first = cells[0]
        if first.kind != kind or any(c != Cell(kind, first.value, j) for j, c in enumerate(cells)):
            raise LoweringError('resource pair/descriptor was overwritten or has the wrong type')
        return first.value

    def load(self, view: str, offset: int, count: int) -> list[Cell]:
        entries = self.abi.get('views', {}).get(view)
        if entries is None:
            raise LoweringError(f'unbound memory view {view}')
        memory: dict[int, Cell] = {}
        for at, item in entries.items():
            if not 0 <= int(at) <= 0xffffffff:
                raise LoweringError('invalid ABI view offset')
            for j, cell in enumerate(self.bind(item)):
                if int(at)+j in memory:
                    raise LoweringError('overlapping ABI view entries')
                memory[int(at)+j] = cell
        try:
            return [memory[offset+j] for j in range(count)]
        except KeyError as exc:
            raise LoweringError(f'unbound memory view {view} dword {exc.args[0]}') from None

    def float_cell(self, cell: Cell) -> str:
        if cell.kind == 'float':
            if cell.value.startswith('u.'):
                self.input_reads.add(cell.value)
            return cell.value
        if cell.kind == 'uint':
            return f'as_type<float>({cell.value})'
        raise LoweringError(f'non-arithmetic resource {cell.kind} used as float')

    def src(self, n: int, literal: int | None = None) -> Cell:
        if n >= 256:
            if n-256 not in self.v:
                raise LoweringError(f'unbound vector input v{n-256}')
            return self.v[n-256]
        if n < 128:
            if n not in self.s:
                raise LoweringError(f'unbound scalar input s{n}')
            return self.s[n]
        if 128 <= n <= 208:
            value = n-128 if n <= 192 else 192-n
            return Cell('uint', f'0x{value & 0xffffffff:08x}u')
        floats = {240: '0.5f', 241: '-0.5f', 242: '1.0f', 243: '-1.0f',
                  244: '2.0f', 245: '-2.0f', 246: '4.0f', 247: '-4.0f'}
        if n in floats:
            return Cell('float', floats[n])
        if n == 255 and literal is not None:
            return Cell('uint', f'0x{literal:08x}u')
        raise LoweringError(f'unsupported source selector {n}')

    def assign(self, register: int, expr: str, kind: str = 'float') -> None:
        name = f't{self.serial}'
        self.serial += 1
        self.lines.append(f'    {"uint" if kind == "packed" else "float"} {name} = {expr};')
        self.v[register] = Cell(kind, name)

    def arithmetic(self, i):
        f, name = i.fields, i.mnemonic
        a = self.float_cell(self.src(f['src0'], f.get('literal')))
        args = [a]
        if i.encoding == 'VOP3':
            args += [self.float_cell(self.src(f['src1']))]
            if name == 'v_mad_f32':
                args += [self.float_cell(self.src(f['src2']))]
            for j in range(len(args)):
                if f['abs'] & 1 << j:
                    args[j] = f'fabs({args[j]})'
                if f['neg'] & 1 << j:
                    args[j] = f'(-{args[j]})'
        elif i.encoding == 'VOP2':
            args += [self.float_cell(self.src(f['vsrc1']+256))]
        if name == 'v_mov_b32':
            # Bit-preserving moves are expressions, not float arithmetic.
            self.v[f['vdst']] = self.src(f['src0'], f.get('literal'))
            return
        if name == 'v_sub_f32':
            expr = f'add32({args[0]}, -({args[1]}))'
        elif name == 'v_mul_f32':
            expr = f'mul32({args[0]}, {args[1]})'
        elif name in ('v_mac_f32', 'v_mad_f32'):
            c = (self.float_cell(self.src(f['vdst']+256)) if name == 'v_mac_f32' else args[2])
            expr = f'add32(mul32({args[0]}, {args[1]}), {c})'
        elif name == 'v_rsq_f32':
            expr = f'rsq32({args[0]})'
        elif name == 'v_cvt_pkrtz_f16_f32':
            if i.encoding == 'VOP3' and any(f[x] for x in ('abs', 'neg', 'omod', 'clamp')):
                raise LoweringError('packed conversion modifiers are unsupported')
            self.assign(f['vdst'], f'(half_rtz({args[0]}) | (half_rtz({args[1]}) << 16))', 'packed')
            return
        else:
            raise LoweringError(f'unsupported arithmetic {name}')
        if i.encoding == 'VOP3':
            if f['omod']:
                expr = f'mul32({expr}, { {1:"2.0f", 2:"4.0f", 3:"0.5f"}[f["omod"]]})'
            if f['clamp']:
                expr = f'sat32({expr})'
        self.assign(f['vdst'], expr)

    def run(self, code: bytes) -> tuple[list[str], dict]:
        if self.abi.get('sha256') != hashlib.sha256(code).hexdigest():
            raise LoweringError('program digest does not match explicit ABI manifest')
        rows = gcn.decode(code)
        if not gcn.inventory(code)['per_lane_eligible']:
            raise LoweringError('quad/wave/LDS instruction blocks per-lane conversion')
        for i in rows:
            f, name = i.fields, i.mnemonic
            try:
                if name == 's_mov_b32':
                    self.s[f['sdst']] = self.src(f['src0'], f.get('literal'))
                elif name in ('s_waitcnt', 's_ttracedata', 's_endpgm'):
                    pass # Ordering/trace facts, no computation in straight-line native code.
                elif i.encoding == 'SMRD':
                    if f['imm'] != 1:
                        raise LoweringError('register scalar memory offset is unsupported')
                    kind = 'descriptor' if name.startswith('s_buffer') else 'pointer'
                    view = self.pair(f['base_sgpr'], kind)
                    cells = self.load(view, f['offset'], f['dwords'])
                    for j, cell in enumerate(cells):
                        self.s[f['sdst']+j] = cell
                elif i.encoding == 'MUBUF':
                    view = self.pair(f['resource_sgpr'], 'descriptor')
                    stream = self.abi.get('streams', {}).get(view)
                    components = f['components']
                    expected_format = (f'explicit_float{components}'
                                       if components in (2, 3, 4) else None)
                    if (stream is None or expected_format is None
                        or stream.get('format') != expected_format
                        or any(f[k] for k in ('offset','offen','glc','addr64','lds','slc','tfe'))
                        or f['idxen'] != 1 or f['soffset'] != 128
                        or self.v.get(f['vaddr']) != Cell('vertex_index', 'one_vertex_per_case')):
                        raise LoweringError('unsupported/unbound formatted stream ABI')
                    field = stream.get('field')
                    cells = self.bind(dict(kind='float4',field=field))
                    for j in range(components):
                        self.v[f['vdata']+j] = cells[j]
                elif i.encoding == 'VINTRP':
                    if self.abi.get('interpolation') != 'supplied_float_coefficients':
                        raise LoweringError('interpolation mode is not explicitly supplied')
                    if self.s.get(124) != Cell('interpolation_control','supplied_coefficients'):
                        raise LoweringError('unbound interpolation control')
                    if f['attr'] not in (0,1):
                        raise LoweringError('unbound interpolation attribute')
                    component = 'xyzw'[f['attrchan']]
                    src = self.float_cell(self.src(f['vsrc']+256))
                    if f['vsrc'] != i.opcode:
                        raise LoweringError('unsupported interpolation source ABI')
                    a=f'u.param{f["attr"]}_p{10 if i.opcode==0 else 20}.{component}'
                    self.input_reads.add(a)
                    if i.opcode == 0:
                        c=f'u.param{f["attr"]}_p0.{component}'
                        self.input_reads.add(c)
                        self.interp_pending[f['vdst']] = (f['attr'],f['attrchan'])
                    elif self.interp_pending.pop(f['vdst'],None) == (f['attr'],f['attrchan']):
                        c=self.float_cell(self.src(f['vdst']+256))
                    else:
                        raise LoweringError('interpolation P2 has no matching P1')
                    self.assign(f['vdst'],f'add32(mul32({a}, {src}), {c})')
                elif i.encoding.startswith('VOP'):
                    self.arithmetic(i)
                elif i.encoding == 'EXP':
                    if f['en'] != 15 or f['target'] in self.exports:
                        raise LoweringError('unsupported partial/repeated export')
                    if f['compr']:
                        if self.stage != 'ps' or f['target'] != 0 or not f['done'] or not f['vm']:
                            raise LoweringError('unsupported compressed export contract')
                        parts=[self.src(f[f'vsrc{j}']+256) for j in range(2)]
                        if any(c.kind!='packed' for c in parts):
                            raise LoweringError('compressed export does not have packed half input')
                        self.exports[0]=[f'half_float({c.value}{shift})' for c in parts for shift in (' & 65535u',' >> 16')]
                    else:
                        if self.stage != 'vs' or f['target'] not in (12,32,33) or f['vm']:
                            raise LoweringError('unsupported vertex export ABI')
                        self.exports[f['target']]=[self.float_cell(self.src(f[f'vsrc{j}']+256)) for j in range(4)]
                else:
                    raise LoweringError(f'unsupported lowering {name}')
            except LoweringError as exc:
                raise LoweringError(f'{self.stage}+{i.offset:#x}: {exc}') from None
        required = {12,32,33} if self.stage=='vs' else {0}
        if set(self.exports) != required or self.interp_pending:
            raise LoweringError('incomplete stage exports/interpolation')
        if self.stage=='vs':
            for target, member in ((12,'clip'),(32,'param0'),(33,'param1')):
                self.lines.append(f'    out.{member} = float4({", ".join(self.exports[target])});')
        else:
            self.lines.append(f'    return float4({", ".join(self.exports[0])});')
        return self.lines, dict(stage=self.stage,sha256=self.abi['sha256'],
                                instructions=len(rows),input_reads=sorted(self.input_reads))


HELPERS = r'''
#pragma clang fp contract(off)
#include <metal_stdlib>
using namespace metal;
inline float clean32(float x) {
    uint b = as_type<uint>(x);
    return ((b & 0x7f800000u) == 0u) ? as_type<float>(b & 0x80000000u) : x;
}
inline float mul32(float a, float b) { return clean32(clean32(a) * clean32(b)); }
inline float add32(float a, float b) { return clean32(clean32(a) + clean32(b)); }
inline float rsq32(float a) { return clean32(1.0f / sqrt(clean32(a))); }
inline float sat32(float a) { return !(a > 0.0f) ? 0.0f : (a >= 1.0f ? 1.0f : a); }
inline uint half_rtz(float x) {
    uint b=as_type<uint>(x), sign=(b>>16)&0x8000u, e=(b>>23)&255u, m=b&0x7fffffu;
    if(e==255u) return sign | (m ? (0x7e00u | ((m>>13)&0x1ffu)) : 0x7c00u);
    int k=int(e)-127;
    if(k>15) return sign|0x7bffu;
    if(k < -24) return sign;
    if(k >= -14) return sign|(uint(k+15)<<10)|(m>>13);
    return sign|((m|0x800000u)>>uint(-k-1));
}
inline float half_float(uint h) {
    uint s=(h&0x8000u)<<16, e=(h>>10)&31u, m=h&1023u;
    if(e==31u) return as_type<float>(s|0x7f800000u|(m<<13));
    if(e==0u) return (s ? -1.0f : 1.0f)*float(m)*0x1p-24f;
    return as_type<float>(s|((e+112u)<<23)|(m<<13));
}
'''


def _expected_logo_ps_manifest() -> dict:
    """Return the one closed-world, synthetic ABI admitted by the logo PS profile."""
    return {
        'schema': LOGO_PS_SCHEMA,
        'sha256': LOGO_PS_SHA256,
        'initial_scalar': {
            '0': {'kind': 'pointer', 'view': 'binding_table'},
        },
        'initial_vector': {
            '0': {'kind': 'float', 'field': 'barycentric', 'component': 'x'},
            '1': {'kind': 'float', 'field': 'barycentric', 'component': 'y'},
        },
        'views': {
            'binding_table': {
                '6': {'kind': 'pointer', 'view': 'image_descriptor_storage'},
                '8': {'kind': 'pointer', 'view': 'sampler_descriptor_storage'},
            },
            'image_descriptor_storage': {
                '0': {'kind': 'image_descriptor', 'identity': 'texture0', 'dwords': 8},
            },
            'sampler_descriptor_storage': {
                '4': {'kind': 'descriptor', 'identity': 'sampler0', 'dwords': 4},
            },
        },
        'image_sample': {
            'resource_sgpr': 0,
            'sampler_sgpr': 8,
            'texture_identity': 'texture0',
            'sampler_identity': 'sampler0',
            'texture_slot': 0,
            'sampler_slot': 0,
            'format': 'rgba8unorm_2d',
            'coordinate_components': 2,
            'normalized_coordinates': True,
            'lod': 'implicit',
        },
    }


def _expected_logo_pair_abi() -> dict:
    """Return the one synthetic VS/PS ABI admitted by the composed logo profile."""
    return {
        'schema': LOGO_PAIR_SCHEMA,
        'vs': {
            'sha256': LOGO_VS_SHA256,
            'initial_scalar': {
                '0': {'kind': 'pointer', 'view': 'root'},
            },
            'initial_vector': {
                '0': {'kind': 'vertex_index'},
            },
            'views': {
                'root': {
                    '0': {'kind': 'pointer', 'view': 'streams'},
                    '2': {'kind': 'pointer', 'view': 'view_constants'},
                },
                'streams': {
                    '0': {'kind': 'descriptor', 'view': 'logo_clip'},
                    '4': {'kind': 'descriptor', 'view': 'logo_param0'},
                    '12': {'kind': 'descriptor', 'view': 'logo_param1'},
                },
                'view_constants': {
                    '130': {'kind': 'float', 'field': 'camera', 'component': 'x'},
                    '131': {'kind': 'float', 'field': 'camera', 'component': 'y'},
                },
            },
            'streams': {
                'logo_clip': {'format': 'explicit_float2', 'field': 'position'},
                'logo_param0': {'format': 'explicit_float4', 'field': 'object_row0'},
                'logo_param1': {'format': 'explicit_float2', 'field': 'position'},
            },
        },
        'ps': _expected_logo_ps_manifest(),
    }


def _validate_logo_pair_abi(abi: dict) -> None:
    mismatch = _manifest_difference(abi, _expected_logo_pair_abi())
    if mismatch:
        raise LoweringError(f'unsupported logo pair synthetic ABI: {mismatch}')


def _manifest_difference(actual, expected, path='manifest') -> str | None:
    if type(actual) is not type(expected):
        return f'{path} has the wrong JSON type'
    if isinstance(expected, dict):
        missing = sorted(expected.keys() - actual.keys(), key=str)
        extra = sorted(actual.keys() - expected.keys(), key=str)
        if missing:
            return f'{path} is missing field(s): {", ".join(missing)}'
        if extra:
            return f'{path} has unknown field(s): {", ".join(extra)}'
        for key in expected:
            mismatch = _manifest_difference(actual[key], expected[key], f'{path}.{key}')
            if mismatch:
                return mismatch
    elif actual != expected:
        return f'{path} must be {expected!r}'
    return None


def _validate_logo_ps_manifest(abi: dict) -> None:
    mismatch = _manifest_difference(abi, _expected_logo_ps_manifest())
    if mismatch:
        raise LoweringError(f'unsupported logo PS synthetic ABI: {mismatch}')


def _validate_logo_ps_rows(rows: list) -> None:
    if len(rows) != LOGO_PS_ROWS:
        raise LoweringError(f'pinned logo PS instruction profile requires {LOGO_PS_ROWS} rows; got {len(rows)}')
    offsets = tuple(row.offset for row in rows)
    if offsets != tuple(sorted(offsets)) or len(set(offsets)) != len(offsets):
        raise LoweringError('pinned logo PS instruction profile has an unexpected instruction boundary/order')

    by_offset = {row.offset: row for row in rows}

    def require_row(offset: int, encoding: str, mnemonic: str, size: int, **fields) -> None:
        row = by_offset[offset]
        if (row.encoding, row.mnemonic, row.size) != (encoding, mnemonic, size):
            raise LoweringError(f'pinned logo PS profile mismatch at {offset:#x}: expected {mnemonic}')
        for name, expected in fields.items():
            if row.fields.get(name) != expected:
                raise LoweringError(
                    f'pinned logo PS profile mismatch at {offset:#x}: {name} must be {expected!r}')

    require_row(0x28, 'SOP1', 's_mov_b64', 4, src0=126, sdst=106)
    require_row(0x2c, 'SOP1', 's_wqm_b64', 4, src0=126, sdst=126)

    samples = [row for row in rows if row.mnemonic == 'image_sample']
    if len(samples) != 1 or samples[0].offset != 0x44:
        raise LoweringError('pinned logo PS profile requires one base IMAGE_SAMPLE at 0x44')
    require_row(0x44, 'MIMG', 'image_sample', 8, dmask=15,
                unorm=0, glc=0, da=0, r128=0, tfe=0, lwe=0, slc=0,
                vaddr=2, vdata=2, resource_sgpr=0, sampler_sgpr=8,
                srsrc=0, ssamp=2)

    require_row(0x88, 'SOP1', 's_mov_b64', 4, src0=106, sdst=126)
    if any('writes_exec_mask' in row.requirements for row in rows
           if 0x2c < row.offset < 0x88):
        raise LoweringError('pinned logo PS profile has an intervening EXEC write')
    if any(row.mnemonic.startswith(('s_branch', 's_cbranch')) for row in rows):
        raise LoweringError('pinned logo PS profile does not admit branches')

    require_row(0x8c, 'EXP', 'exp', 8, target=0, en=15, compr=1, done=1, vm=1,
                vsrc0=1, vsrc1=0, vsrc2=0, vsrc3=0)
    require_row(0x94, 'SOPP', 's_endpgm', 4, simm16=0)
    if rows[-1] != by_offset[0x94]:
        raise LoweringError('pinned logo PS profile requires an exact final s_endpgm')


def _validate_pinned_logo_ps_code(code: bytes) -> list:
    if len(code) != LOGO_PS_BYTES:
        raise LoweringError(f'pinned logo PS must be {LOGO_PS_BYTES} bytes; got {len(code)}')
    digest = hashlib.sha256(code).hexdigest()
    if digest != LOGO_PS_SHA256:
        raise LoweringError(
            f'pinned logo PS SHA-256 mismatch: expected {LOGO_PS_SHA256}, got {digest}')
    try:
        rows = gcn.decode(code)
    except gcn.DecodeError as exc:
        raise LoweringError(f'pinned logo PS decode failed: {exc}') from None
    _validate_logo_ps_rows(rows)
    return rows


def _validate_pinned_logo_vs_code(code: bytes) -> None:
    if len(code) != LOGO_VS_BYTES:
        raise LoweringError(f'pinned logo VS must be {LOGO_VS_BYTES} bytes; got {len(code)}')
    digest = hashlib.sha256(code).hexdigest()
    if digest != LOGO_VS_SHA256:
        raise LoweringError(
            f'pinned logo VS SHA-256 mismatch: expected {LOGO_VS_SHA256}, got {digest}')


def _compose_logo_pair_msl(vs_lines: list[str], ps_source: str) -> str:
    """Keep the E636 vertex and E646 fragment wrappers around shared PS helpers."""
    ps_header = '// Pinned logo PS; synthetic slot-0 resources only. Retail bindings and WQM equivalence NOT CLAIMED.\n'
    if not ps_source.startswith(ps_header):
        raise LoweringError('unexpected pinned logo PS MSL header')
    shared = ps_source[len(ps_header):]
    vertex_start = shared.find('\nvertex RasterValue asset_vertex(')
    fragment_start = shared.find('\nfragment float4 asset_fragment(', vertex_start + 1)
    if vertex_start < 0 or fragment_start < 0:
        raise LoweringError('pinned logo PS MSL is missing its stage wrappers')
    vertex_source = shared[vertex_start:fragment_start]
    fragment_source = shared[fragment_start:]
    if vertex_source.count('vertex RasterValue asset_vertex(') != 1:
        raise LoweringError('pinned logo PS MSL has an unexpected vertex wrapper')
    if fragment_source.count('fragment float4 asset_fragment(') != 1:
        raise LoweringError('pinned logo PS MSL has an unexpected fragment wrapper')

    source = ('// Pinned E636 logo VS + E646 logo PS; synthetic ABI only. '
              'Retail bindings and WQM/Metal derivative equivalence NOT CLAIMED.\n')
    source += shared[:vertex_start]
    source += '\nstruct VertexValue { float4 clip; float4 param0; float4 param1; };\n'
    source += '\ninline VertexValue recovered_logo_vs(thread const ShaderCase& u) {\n'
    source += '    VertexValue out;\n' + '\n'.join(vs_lines) + '\n    return out;\n}\n'
    source += r'''
vertex RasterValue asset_vertex(device const ShaderCase* inputs [[buffer(0)]],
                                uint vid [[vertex_id]]) {
    ShaderCase u = inputs[vid];
    VertexValue v = recovered_logo_vs(u);
    RasterValue out;
    out.position = v.clip;
    out.param0 = v.param0;
    out.param1 = v.param1;
    return out;
}
'''
    source += fragment_source
    if 'kernel void verify_pair' in source:
        raise LoweringError('logo pair MSL must not contain the compute/oracle sink')
    return source


def _logo_ps_source() -> str:
    source = '// Pinned logo PS; synthetic slot-0 resources only. Retail bindings and WQM equivalence NOT CLAIMED.\n'
    source += HELPERS
    source += '\nstruct ShaderCase {\n' + ''.join(f'    float4 {field};\n' for field in CASE_FIELDS) + '};\n'
    source += r'''
struct RasterValue { float4 position [[position]]; float4 param0; float4 param1; };

inline float4 recovered_logo_ps(thread const ShaderCase& u,
                                texture2d<float> texture0, sampler sampler0) {
    float2 uv = float2(u.param1_p0.x, u.param1_p0.y);
    float4 sampled = texture0.sample(sampler0, uv);
    float4 multiplied = float4(mul32(sampled.x, u.param0_p0.x),
                               mul32(sampled.y, u.param0_p0.y),
                               mul32(sampled.z, u.param0_p0.z),
                               mul32(sampled.w, u.param0_p0.w));
    return float4(half_float(half_rtz(multiplied.x)),
                  half_float(half_rtz(multiplied.y)),
                  half_float(half_rtz(multiplied.z)),
                  half_float(half_rtz(multiplied.w)));
}

vertex RasterValue asset_vertex(device const ShaderCase* inputs [[buffer(0)]],
                                uint vid [[vertex_id]]) {
    ShaderCase u = inputs[vid];
    RasterValue out;
    // Synthetic harness mapping: preserve xyz and supply a valid clip-space w.
    out.position = float4(u.position.xyz, 1.0f);
    out.param0 = u.param0_p0;
    out.param1 = float4(add32(mul32(u.position.x, u.material.x), 0.5f),
                        add32(mul32(u.position.y, u.material.y), 0.5f),
                        0.0f, 0.0f);
    return out;
}

fragment float4 asset_fragment(RasterValue in [[stage_in]],
                               constant ShaderCase& uniforms [[buffer(0)]],
                               texture2d<float> texture0 [[texture(0)]],
                               sampler sampler0 [[sampler(0)]]) {
    ShaderCase u = uniforms;
    u.param0_p0 = in.param0;
    u.param1_p0 = in.param1;
    u.param0_p10 = float4(0.0f);
    u.param0_p20 = float4(0.0f);
    u.param1_p10 = float4(0.0f);
    u.param1_p20 = float4(0.0f);
    u.barycentric = float4(0.0f);
    return recovered_logo_ps(u, texture0, sampler0);
}
'''
    return source


def _lower_logo_ps_rows(rows: list, abi: dict) -> tuple[str, dict]:
    _validate_logo_ps_rows(rows)
    _validate_logo_ps_manifest(abi)
    source = _logo_ps_source()
    return source, dict(
        version=LOGO_PS_SCHEMA,
        profile='pinned-logo-ps-image-sample',
        code_sha256=LOGO_PS_SHA256,
        code_bytes=LOGO_PS_BYTES,
        stages=[dict(stage='ps', sha256=LOGO_PS_SHA256, instructions=LOGO_PS_ROWS)],
        image_sample=dict(texture_slot=0, sampler_slot=0, format='rgba8unorm_2d',
                          coordinate_components=2, normalized_coordinates=True,
                          lod='implicit', instruction_offset='0x44'),
        msl_sha256=hashlib.sha256(source.encode()).hexdigest(),
        limitations=['synthetic slot-0 texture/sampler ABI',
                     'validated WQM/save/restore controls are not emitted as runtime operations',
                     'AMD WQM/Metal helper-lane and derivative equivalence NOT VERIFIED',
                     'retail resource bindings and SPI interpolation UNVERIFIED',
                     'G3 NOT RUN'])


def generate_logo_ps(code: bytes, abi: dict) -> tuple[str, dict]:
    """Lower only the SHA-pinned logo PS through its explicit synthetic ABI."""
    rows = _validate_pinned_logo_ps_code(code)
    return _lower_logo_ps_rows(rows, abi)


def generate_logo_pair(vs: bytes, ps: bytes, abi: dict) -> tuple[str, dict]:
    """Lower the exact pinned logo shader pair under its one synthetic ABI."""
    _validate_logo_pair_abi(abi)
    _validate_pinned_logo_vs_code(vs)
    vs_lines, vs_report = Lowerer(abi['vs'], 'vs').run(vs)
    ps_rows = _validate_pinned_logo_ps_code(ps)
    ps_source, ps_report = _lower_logo_ps_rows(ps_rows, abi['ps'])
    source = _compose_logo_pair_msl(vs_lines, ps_source)
    report = dict(
        version=LOGO_PAIR_SCHEMA,
        profile='pinned-logo-vs-ps-synthetic-pair',
        code_sha256=dict(vs=LOGO_VS_SHA256, ps=LOGO_PS_SHA256),
        code_bytes=dict(vs=LOGO_VS_BYTES, ps=LOGO_PS_BYTES),
        stages=[vs_report, ps_report['stages'][0]],
        image_sample=ps_report['image_sample'],
        msl_sha256=hashlib.sha256(source.encode()).hexdigest(),
        limitations=[
            'synthetic vertex ABI: position supplies logo_clip and logo_param1; object_row0 supplies logo_param0',
            'camera inputs remain explicit synthetic scaling inputs',
            'synthetic slot-0 texture/sampler ABI',
            'validated WQM/save/restore controls are not emitted as runtime operations',
            'AMD WQM/Metal helper-lane and derivative equivalence NOT VERIFIED',
            'retail resource bindings and SPI interpolation UNVERIFIED',
            'G3 NOT RUN',
        ],
    )
    return source, report


def generate(vs: bytes, ps: bytes, abi: dict) -> tuple[str, dict]:
    if abi.get('schema') != VERSION or abi.get('floating_point') != 'nearest_even_ftz32_rtz16':
        raise LoweringError('explicit supported schema/floating-point harness profile required')
    vl, vm = Lowerer(abi['vs'],'vs').run(vs)
    pl, pm = Lowerer(abi['ps'],'ps').run(ps)
    source = '// Private asset-derived harness; live binding and G3 NOT RUN.\n'+HELPERS
    source += '\nstruct ShaderCase {\n'+''.join(f'    float4 {x};\n' for x in CASE_FIELDS)+'};\n'
    source += '\nstruct VertexValue { float4 clip; float4 param0; float4 param1; };\n'
    source += '\ninline VertexValue recovered_vs(thread const ShaderCase& u) {\n    VertexValue out;\n'
    source += '\n'.join(vl)+'\n    return out;\n}\n'
    source += '\ninline float4 recovered_ps(thread const ShaderCase& u) {\n'+'\n'.join(pl)+'\n}\n'
    source += '''
kernel void verify_pair(device const ShaderCase* inputs [[buffer(0)]],
                        device float4* outputs [[buffer(1)]], uint tid [[thread_position_in_grid]]) {
    ShaderCase u=inputs[tid]; VertexValue v=recovered_vs(u);
    outputs[tid*4]=v.clip; outputs[tid*4+1]=v.param0; outputs[tid*4+2]=v.param1;
    outputs[tid*4+3]=recovered_ps(u);
}
struct RasterValue { float4 position [[position]]; float4 param0; float4 param1; };
vertex RasterValue asset_vertex(device const ShaderCase* inputs [[buffer(0)]], uint vid [[vertex_id]]) {
    ShaderCase u=inputs[vid]; VertexValue v=recovered_vs(u);
    RasterValue out; out.position=v.clip; out.param0=v.param0; out.param1=v.param1; return out;
}
fragment float4 asset_fragment(RasterValue in [[stage_in]], constant ShaderCase& uniforms [[buffer(0)]]) {
    ShaderCase u=uniforms;
    // Explicit harness contract: native interpolated values supplied as P0.
    // Retail SPI interpolation mode is unverified.
    u.param0_p0=in.param0; u.param1_p0=in.param1;
    u.param0_p10=float4(0); u.param0_p20=float4(0);
    u.param1_p10=float4(0); u.param1_p20=float4(0); u.barycentric=float4(0);
    return recovered_ps(u);
}
'''
    return source, dict(version=VERSION,profile=abi['floating_point'],stages=[vm,pm],
                        msl_sha256=hashlib.sha256(source.encode()).hexdigest(),
                        limitations=['harness ABI only','retail descriptor/SPI mode unverified','G3 NOT RUN'])


def _private_local_root(workspace: Path) -> Path:
    workspace = workspace.resolve()
    local_root = (workspace / 'local').resolve()
    if not local_root.is_relative_to(workspace):
        raise LoweringError('workspace local/ must resolve inside the workspace')
    return local_root


def _require_private_local_path(path: Path, local_root: Path, label: str) -> Path:
    resolved = path.resolve()
    if not resolved.is_relative_to(local_root):
        raise LoweringError(f'logo-pair {label} must resolve under workspace local/')
    return resolved


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--profile',choices=('pair','logo-ps','logo-pair'),default='pair')
    p.add_argument('--vs',type=Path); p.add_argument('--ps',required=True,type=Path)
    p.add_argument('--abi',required=True,type=Path); p.add_argument('--output',required=True,type=Path)
    p.add_argument('--report',required=True,type=Path)
    a=p.parse_args(argv)
    try:
        workspace=Path(__file__).resolve().parents[2]
        if a.profile == 'logo-pair':
            local_root = _private_local_root(workspace)
            for name, label in (('abi', 'ABI'), ('output', 'MSL output'),
                                ('report', 'report'), ('vs', 'pinned logo VS'),
                                ('ps', 'pinned logo PS')):
                path = getattr(a, name)
                if path is None:
                    continue
                setattr(a, name, _require_private_local_path(path, local_root, label))
            seen_paths = {}
            for label, path in (('ABI', a.abi), ('pinned logo VS', a.vs),
                                ('pinned logo PS', a.ps), ('MSL output', a.output),
                                ('report', a.report)):
                if path in seen_paths:
                    raise LoweringError(
                        f'logo-pair {label} path collides with {seen_paths[path]} path')
                seen_paths[path] = label
        for target in (a.output,a.report):
            resolved=target.resolve()
            if resolved.is_relative_to(workspace) and not resolved.is_relative_to(workspace/'local'):
                raise LoweringError('asset-derived outputs inside this repo must stay under private local/')
        abi=json.loads(a.abi.read_text())
        if a.profile == 'logo-ps':
            if a.vs is not None:
                raise LoweringError('logo-ps profile accepts only --ps; omit --vs')
            if not a.ps.resolve().is_relative_to(workspace/'local'):
                raise LoweringError('pinned logo PS input must be supplied from private local/')
            source,report=generate_logo_ps(a.ps.read_bytes(),abi)
        elif a.profile == 'logo-pair':
            if a.vs is None:
                raise LoweringError('logo-pair profile requires --vs and --ps')
            if not a.vs.resolve().is_relative_to(workspace/'local'):
                raise LoweringError('pinned logo VS input must be supplied from private local/')
            if not a.ps.resolve().is_relative_to(workspace/'local'):
                raise LoweringError('pinned logo PS input must be supplied from private local/')
            source,report=generate_logo_pair(a.vs.read_bytes(),a.ps.read_bytes(),abi)
        else:
            if a.vs is None:
                raise LoweringError('pair profile requires --vs')
            source,report=generate(a.vs.read_bytes(),a.ps.read_bytes(),abi)
        a.output.parent.mkdir(parents=True,exist_ok=True)
        a.output.write_text(source); a.report.write_text(json.dumps(report,indent=2)+'\n')
    except (OSError,ValueError,KeyError,TypeError) as e:
        print(f'FAILED: {e}',file=sys.stderr); return 1
    return 0


if __name__=='__main__':
    raise SystemExit(main())
