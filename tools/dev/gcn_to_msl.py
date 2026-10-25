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


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--vs',required=True,type=Path); p.add_argument('--ps',required=True,type=Path)
    p.add_argument('--abi',required=True,type=Path); p.add_argument('--output',required=True,type=Path)
    p.add_argument('--report',required=True,type=Path)
    a=p.parse_args(argv)
    try:
        workspace=Path(__file__).resolve().parents[2]
        for target in (a.output,a.report):
            resolved=target.resolve()
            if resolved.is_relative_to(workspace) and not resolved.is_relative_to(workspace/'local'):
                raise LoweringError('asset-derived outputs inside this repo must stay under private local/')
        source,report=generate(a.vs.read_bytes(),a.ps.read_bytes(),json.loads(a.abi.read_text()))
        a.output.parent.mkdir(parents=True,exist_ok=True)
        a.output.write_text(source); a.report.write_text(json.dumps(report,indent=2)+'\n')
    except (OSError,ValueError,KeyError,TypeError) as e:
        print(f'FAILED: {e}',file=sys.stderr); return 1
    return 0


if __name__=='__main__':
    raise SystemExit(main())
