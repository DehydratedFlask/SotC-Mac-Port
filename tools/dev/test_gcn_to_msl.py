#!/usr/bin/env python3
"""Synthetic lowering/ABI failures; no game code or private ABI fixtures."""
import hashlib
import io
import importlib.util
import copy
import re
from pathlib import Path
import sys
import tempfile
import unittest
from contextlib import redirect_stderr
from dataclasses import replace
from unittest.mock import patch


def module(name,file):
    spec=importlib.util.spec_from_file_location(name,Path(__file__).with_name(file))
    value=importlib.util.module_from_spec(spec);sys.modules[name]=value;spec.loader.exec_module(value);return value


m=module('gcn_to_msl','gcn_to_msl.py')
e=module('gcn_encoding_oracle','test_gcn_inventory.py')
o=module('default_shader_oracle','default_shader_oracle.py')


def float_input(field,c):
    return dict(kind='float',field=field,component=c)


def synthetic():
    vs=e.vop1(dst=3,src=242)+b''.join(e.exp(target=t,done=int(t==12),src=(0,1,2,3)) for t in (12,32,33))+e.END
    ps=e.vop2(op=47,dst=4,src0=256,src1=1)+e.vop2(op=47,dst=5,src0=258,src1=3)+e.exp(target=0,compr=1,done=1,vm=1,src=(4,5,0,0))+e.END
    abi=dict(schema=m.VERSION,floating_point='nearest_even_ftz32_rtz16',
             vs=dict(sha256=hashlib.sha256(vs).hexdigest(),initial_vector={str(j):float_input('position',c) for j,c in enumerate('xyz')}),
             ps=dict(sha256=hashlib.sha256(ps).hexdigest(),initial_vector={str(j):float_input('material',c) for j,c in enumerate('xyzw')}))
    return vs,ps,abi


def synthetic_logo_ps_rows():
    """Build synthetic row objects for the profile guard without shader bytes."""
    critical = {0x28, 0x2c, 0x44, 0x88, 0x8c, 0x94}
    fillers = [offset for offset in range(0, 0x94, 4) if offset not in critical][:28]
    offsets = sorted(fillers + list(critical))
    rows = [m.gcn.Instruction(offset, 4, 'SOPP', 12, 's_waitcnt', {'simm16': 0}, ())
            for offset in offsets]

    def install(offset, encoding, opcode, mnemonic, fields, requirements=(), size=4):
        index = next(i for i, row in enumerate(rows) if row.offset == offset)
        rows[index] = m.gcn.Instruction(offset, size, encoding, opcode, mnemonic, fields,
                                        tuple(requirements))

    install(0x28, 'SOP1', 4, 's_mov_b64', {'src0': 126, 'sdst': 106},
            ('reads_exec_mask',))
    install(0x2c, 'SOP1', 10, 's_wqm_b64', {'src0': 126, 'sdst': 126},
            ('reads_exec_mask', 'whole_quad_mode', 'writes_exec_mask'))
    install(0x44, 'MIMG', 32, 'image_sample', {
        'dmask': 15, 'unorm': 0, 'glc': 0, 'da': 0, 'r128': 0, 'tfe': 0, 'lwe': 0,
        'slc': 0, 'vaddr': 2, 'vdata': 2, 'srsrc': 0, 'ssamp': 2,
        'resource_sgpr': 0, 'sampler_sgpr': 8,
    }, ('implicit_quad_derivatives', 'texture_sample'), size=8)
    install(0x88, 'SOP1', 4, 's_mov_b64', {'src0': 106, 'sdst': 126},
            ('reads_vcc_mask', 'writes_exec_mask'))
    install(0x8c, 'EXP', 0, 'exp', {
        'target': 0, 'en': 15, 'compr': 1, 'done': 1, 'vm': 1,
        'vsrc0': 1, 'vsrc1': 0, 'vsrc2': 0, 'vsrc3': 0,
    }, ('export',), size=8)
    install(0x94, 'SOPP', 1, 's_endpgm', {'simm16': 0})
    return rows


def synthetic_logo_ps_manifest():
    return {
        'schema': m.LOGO_PS_SCHEMA,
        'sha256': m.LOGO_PS_SHA256,
        'initial_scalar': {'0': {'kind': 'pointer', 'view': 'binding_table'}},
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


def synthetic_mubuf(components, stream_format=None, stream_field='position',
                    descriptor_view='vertices', stream_view=None, vertex_index=True,
                    mubuf_overrides=None):
    mubuf_fields = dict(op=components - 1, data=10, addr=0, resource=0, offset=0,
                        soffset=128, offen=0, idxen=1, glc=0, addr64=0, lds=0,
                        slc=0, tfe=0)
    mubuf_fields.update(mubuf_overrides or {})
    code = e.mubuf(**mubuf_fields)
    code += b''.join(e.exp(target=target, done=int(target == 12),
                           src=(10, 11, 12, 13)) for target in (12, 32, 33)) + e.END

    initial_vector = {'0': (dict(kind='vertex_index') if vertex_index
                            else float_input('position', 'x'))}
    for component in range(components, 4):
        initial_vector[str(10 + component)] = float_input('material', 'xyzw'[component])
    if stream_format is None:
        stream_format = f'explicit_float{components}'
    if stream_view is None:
        stream_view = descriptor_view
    abi = dict(
        sha256=hashlib.sha256(code).hexdigest(),
        initial_scalar={'0': dict(kind='descriptor', view=descriptor_view)},
        initial_vector=initial_vector,
        streams={stream_view: dict(format=stream_format, field=stream_field)},
    )
    return code, abi


def lower_synthetic_mubuf(components, **kwargs):
    code, abi = synthetic_mubuf(components, **kwargs)
    lowerer = m.Lowerer(abi, 'vs')
    lines, report = lowerer.run(code)
    return lowerer, lines, report


class LoweringTests(unittest.TestCase):
    def test_complete_synthetic_pair(self):
        vs,ps,abi=synthetic();source,report=m.generate(vs,ps,abi)
        self.assertEqual([s['instructions'] for s in report['stages']],[5,4])
        self.assertIn('out.clip = float4(u.position.x, u.position.y, u.position.z, 1.0f);',source)
        self.assertIn('half_rtz(u.material.x)',source)
        self.assertNotIn('switch',source)
        self.assertNotIn('sgpr',source.lower())
        self.assertNotIn('vgpr',source.lower())
        self.assertIn('kernel void verify_pair',source)

    def test_missing_profile_and_wrong_digest_fail(self):
        vs,ps,abi=synthetic();abi.pop('floating_point')
        with self.assertRaises(m.LoweringError):m.generate(vs,ps,abi)
        vs,ps,abi=synthetic();abi['ps']['sha256']='0'*64
        with self.assertRaises(m.LoweringError):m.generate(vs,ps,abi)

    def test_missing_vector_input_fails(self):
        vs,ps,abi=synthetic();abi['vs']['initial_vector'].pop('1')
        with self.assertRaisesRegex(m.LoweringError,'unbound vector'):m.generate(vs,ps,abi)

    def test_whole_quad_rejected_before_lowering(self):
        p=e.sop1(op=10,dst=126,src=126)+e.END
        with self.assertRaisesRegex(m.LoweringError,'quad/wave'):
            m.Lowerer(dict(sha256=hashlib.sha256(p).hexdigest()),'ps').run(p)

    def test_mubuf_float_widths_write_exact_destination_cells(self):
        channels = 'xyzw'
        for components in (2, 3, 4):
            with self.subTest(components=components):
                lowerer, lines, report = lower_synthetic_mubuf(components)
                self.assertEqual(report['instructions'], 5)
                self.assertEqual(report['stage'], 'vs')
                for component in range(components):
                    self.assertEqual(
                        lowerer.v[10 + component],
                        m.Cell('float', f'u.position.{channels[component]}'))
                for component in range(components, 4):
                    self.assertEqual(
                        lowerer.v[10 + component],
                        m.Cell('float', f'u.material.{channels[component]}'))
                self.assertTrue(any(line.startswith('    out.clip = float4(') for line in lines))

    def test_mubuf_component_width_format_mismatches_fail(self):
        for components, stream_format in ((2, 'explicit_float3'),
                                          (3, 'explicit_float4'),
                                          (4, 'explicit_float2'),
                                          (2, 'explicit_float1')):
            with self.subTest(components=components, stream_format=stream_format):
                code, abi = synthetic_mubuf(components, stream_format=stream_format)
                with self.assertRaisesRegex(m.LoweringError,
                                            'unsupported/unbound formatted stream ABI'):
                    m.Lowerer(abi, 'vs').run(code)

    def test_mubuf_binding_index_and_fetch_contract_mismatches_fail(self):
        invalid_cases = (
            {'stream_view': 'other'},
            {'vertex_index': False},
            {'mubuf_overrides': {'offset': 1}},
            {'mubuf_overrides': {'offen': 1}},
            {'mubuf_overrides': {'idxen': 0}},
            {'mubuf_overrides': {'glc': 1}},
            {'mubuf_overrides': {'addr64': 1}},
            {'mubuf_overrides': {'lds': 1}},
            {'mubuf_overrides': {'slc': 1}},
            {'mubuf_overrides': {'tfe': 1}},
            {'mubuf_overrides': {'addr': 1}},
            {'mubuf_overrides': {'resource': 1}},
        )
        for kwargs in invalid_cases:
            with self.subTest(kwargs=kwargs):
                code, abi = synthetic_mubuf(3, **kwargs)
                with self.assertRaises(m.LoweringError):
                    m.Lowerer(abi, 'vs').run(code)

    def test_resource_pair_type_and_overlap(self):
        a=dict(initial_scalar={'0':dict(kind='pointer',view='uniforms')})
        l=m.Lowerer(a,'vs');self.assertEqual(l.pair(0,'pointer'),'uniforms')
        with self.assertRaises(m.LoweringError):l.pair(0,'descriptor')
        a['initial_scalar']['1']=dict(kind='interpolation_control')
        with self.assertRaisesRegex(m.LoweringError,'overlapping'):m.Lowerer(a,'vs')

    def test_unbound_memory_view_and_offset(self):
        l=m.Lowerer(dict(views={'uniforms':{'0':dict(kind='float4',field='camera')}}),'vs')
        self.assertEqual(l.load('uniforms',1,1)[0].value,'u.camera.y')
        with self.assertRaisesRegex(m.LoweringError,'unbound memory'):l.load('uniforms',4,1)
        with self.assertRaisesRegex(m.LoweringError,'unbound memory'):l.load('missing',0,1)

    def test_overlapping_and_invalid_views(self):
        for view in ({'0':dict(kind='float4',field='camera'),'1':dict(kind='float4',field='material')},
                     {'-1':dict(kind='float4',field='camera')}):
            l=m.Lowerer(dict(views={'uniforms':view}),'vs')
            with self.assertRaises(m.LoweringError):l.load('uniforms',0,1)

    def test_binding_expressions_are_not_executable_text(self):
        l=m.Lowerer({},'vs')
        for b in (dict(kind='float',field='injected();',component='x'),
                  dict(kind='float',field='camera',component=None),
                  dict(kind='pointer',view='bad();'),dict(kind='arbitrary_code')):
            with self.assertRaises(m.LoweringError):l.bind(b)

    def test_incomplete_exports_fail(self):
        p=e.vop1(dst=0,src=128)+e.END
        with self.assertRaisesRegex(m.LoweringError,'incomplete'):
            m.Lowerer(dict(sha256=hashlib.sha256(p).hexdigest()),'vs').run(p)

    def test_synthetic_oracle_known_equation(self):
        c=o.base_case();r=o.evaluate(c)
        self.assertEqual(r[:4],[0,0,.25,1])
        self.assertEqual(r[4:12],[0,0,.25,0,0,1,0,0])
        self.assertEqual(r[12:],[o.half_rtz(x) for x in [.8,.4,.2,1]])

    def test_half_rtz_numeric_boundaries(self):
        for x,expected in ((0.0,0),(-0.0,0x8000),(1.0,0x3c00),(1.0005,0x3c00),
                           (65504.0,0x7bff),(65536.0,0x7bff),(2**-24,1),(2**-25,0)):
            self.assertEqual(o.half_rtz_bits(x),expected)

    def test_pinned_logo_ps_sample_lowers_to_synthetic_ps_only_msl(self):
        rows = synthetic_logo_ps_rows()
        self.assertEqual(len(rows), m.LOGO_PS_ROWS)
        source, report = m._lower_logo_ps_rows(rows, synthetic_logo_ps_manifest())

        self.assertIn('struct ShaderCase {', source)
        case_body = source.split('struct ShaderCase {', 1)[1].split('};', 1)[0]
        self.assertEqual(re.findall(r'float4\s+(\w+);', case_body), list(m.CASE_FIELDS))
        self.assertIn('vertex RasterValue asset_vertex', source)
        self.assertIn('fragment float4 asset_fragment', source)
        self.assertIn('texture2d<float> texture0 [[texture(0)]]', source)
        self.assertIn('sampler sampler0 [[sampler(0)]]', source)
        self.assertIn('out.position = float4(u.position.xyz, 1.0f);', source)
        self.assertIn('out.param0 = u.param0_p0;', source)
        self.assertIn('add32(mul32(u.position.x, u.material.x), 0.5f)', source)
        self.assertIn('add32(mul32(u.position.y, u.material.y), 0.5f)', source)
        self.assertIn('u.param0_p0 = in.param0;', source)
        self.assertIn('u.param1_p0 = in.param1;', source)
        self.assertIn('u.param0_p10 = float4(0.0f);', source)
        self.assertIn('u.param0_p20 = float4(0.0f);', source)
        self.assertIn('u.param1_p10 = float4(0.0f);', source)
        self.assertIn('u.param1_p20 = float4(0.0f);', source)
        self.assertIn('u.barycentric = float4(0.0f);', source)
        self.assertIn('texture0.sample(sampler0, uv)', source)
        self.assertNotIn('sample_compare', source)
        self.assertNotIn('s_wqm_b64', source)
        self.assertNotIn('kernel void verify_pair', source)
        self.assertNotIn('recovered_vs', source)
        self.assertEqual(report['code_sha256'], m.LOGO_PS_SHA256)
        self.assertEqual(report['code_bytes'], 152)
        self.assertEqual(report['stages'], [dict(stage='ps', sha256=m.LOGO_PS_SHA256,
                                                  instructions=34)])
        self.assertEqual(report['image_sample']['lod'], 'implicit')
        self.assertIn('AMD WQM/Metal helper-lane and derivative equivalence NOT VERIFIED',
                      report['limitations'])

    def test_logo_ps_manifest_is_closed_world_and_strict(self):
        rows = synthetic_logo_ps_rows()
        base = synthetic_logo_ps_manifest()
        invalid = []

        wrong = dict(base); wrong['unexpected'] = True
        invalid.append((wrong, 'unknown field'))
        wrong = dict(base); wrong.pop('sha256')
        invalid.append((wrong, 'missing field'))
        wrong = {**base, 'schema': 'other'}
        invalid.append((wrong, 'schema'))
        wrong = {**base, 'sha256': '0' * 64}
        invalid.append((wrong, 'sha256'))
        wrong = {**base, 'image_sample': {**base['image_sample'], 'texture_slot': 1}}
        invalid.append((wrong, 'texture_slot'))
        wrong = {**base, 'image_sample': {**base['image_sample'], 'lod': 'explicit'}}
        invalid.append((wrong, 'lod'))
        wrong = {**base, 'views': {**base['views'],
                                   'image_descriptor_storage': {'0': {'kind': 'descriptor',
                                                                      'identity': 'texture0',
                                                                      'dwords': 8}}}}
        invalid.append((wrong, 'kind'))

        for manifest, expected_path in invalid:
            with self.subTest(expected_path=expected_path):
                with self.assertRaisesRegex(m.LoweringError,
                                            'unsupported logo PS synthetic ABI') as raised:
                    m._lower_logo_ps_rows(rows, manifest)
                self.assertIn(expected_path, str(raised.exception))

    def test_logo_ps_profile_and_hash_mutations_fail_closed(self):
        code = bytes(m.LOGO_PS_BYTES)
        manifest = synthetic_logo_ps_manifest()
        with self.assertRaisesRegex(m.LoweringError, 'SHA-256 mismatch') as raised:
            m.generate_logo_ps(code, manifest)
        self.assertIn(m.LOGO_PS_SHA256, str(raised.exception))

        changed = bytearray(code)
        changed[0] ^= 1
        with self.assertRaisesRegex(m.LoweringError, 'SHA-256 mismatch'):
            m.generate_logo_ps(bytes(changed), manifest)

        rows = synthetic_logo_ps_rows()

        def replace_fields(offset, **changes):
            result = list(rows)
            index = next(i for i, row in enumerate(result) if row.offset == offset)
            fields = dict(result[index].fields)
            fields.update(changes)
            result[index] = replace(result[index], fields=fields)
            return result

        mutations = (
            (replace_fields(0x28, sdst=108), '0x28'),
            (replace_fields(0x2c, src0=124), '0x2c'),
            (replace_fields(0x44, dmask=7), '0x44'),
            (replace_fields(0x44, glc=1), '0x44'),
            (replace_fields(0x44, vaddr=3), '0x44'),
            (replace_fields(0x88, src0=108), '0x88'),
            (replace_fields(0x8c, done=0), '0x8c'),
            (replace_fields(0x94, simm16=1), '0x94'),
            (list(reversed(rows)), 'boundary/order'),
        )
        for mutated_rows, expected in mutations:
            with self.subTest(expected=expected):
                with self.assertRaisesRegex(m.LoweringError, 'pinned logo PS'):
                    m._validate_logo_ps_rows(mutated_rows)

        exec_write = list(rows)
        index = next(i for i, row in enumerate(exec_write) if row.offset == 0x40)
        exec_write[index] = replace(exec_write[index], requirements=('writes_exec_mask',))
        with self.assertRaisesRegex(m.LoweringError, 'intervening EXEC write'):
            m._validate_logo_ps_rows(exec_write)

    def test_logo_pair_generator_composes_pinned_stage_wrappers(self):
        abi = m._expected_logo_pair_abi()
        vs_lines = [
            '    out.clip = float4(u.position.x, u.position.y, 0.0f, 1.0f);',
            '    out.param0 = float4(u.object_row0);',
            '    out.param1 = float4(u.position.x, u.position.y, 0.0f, 0.0f);',
        ]
        vs_report = dict(stage='vs', sha256=m.LOGO_VS_SHA256, instructions=25,
                         input_reads=['u.camera.x', 'u.camera.y', 'u.object_row0.x'])
        ps_report = dict(stages=[dict(stage='ps', sha256=m.LOGO_PS_SHA256,
                                      instructions=m.LOGO_PS_ROWS)],
                         image_sample=dict(texture_slot=0, sampler_slot=0,
                                            format='rgba8unorm_2d', lod='implicit'))
        with (patch.object(m, '_validate_pinned_logo_vs_code'),
              patch.object(m.Lowerer, 'run', return_value=(vs_lines, vs_report)),
              patch.object(m, '_validate_pinned_logo_ps_code',
                           return_value=synthetic_logo_ps_rows()),
              patch.object(m, '_lower_logo_ps_rows',
                           return_value=(m._logo_ps_source(), ps_report))):
            source, report = m.generate_logo_pair(b'vs fixture', b'ps fixture', abi)

        self.assertEqual(source.count('struct ShaderCase {'), 1)
        self.assertEqual(source.count('vertex RasterValue asset_vertex('), 1)
        self.assertEqual(source.count('fragment float4 asset_fragment('), 1)
        self.assertIn('inline VertexValue recovered_logo_vs(', source)
        self.assertIn('out.clip = float4(u.position.x, u.position.y, 0.0f, 1.0f);', source)
        self.assertIn('out.param0 = float4(u.object_row0);', source)
        self.assertIn('out.param1 = float4(u.position.x, u.position.y, 0.0f, 0.0f);', source)
        self.assertIn('texture2d<float> texture0 [[texture(0)]]', source)
        self.assertIn('sampler sampler0 [[sampler(0)]]', source)
        self.assertIn('texture0.sample(sampler0, uv)', source)
        self.assertNotIn('Synthetic harness mapping', source)
        self.assertNotIn('kernel void verify_pair', source)
        self.assertEqual(report['profile'], 'pinned-logo-vs-ps-synthetic-pair')
        self.assertEqual(report['stages'], [vs_report, ps_report['stages'][0]])
        self.assertEqual(report['image_sample']['texture_slot'], 0)
        self.assertIn('AMD WQM/Metal helper-lane and derivative equivalence NOT VERIFIED',
                      report['limitations'])

    def test_logo_pair_manifest_rejects_wrong_hashes_and_unsupported_aliases(self):
        abi = m._expected_logo_pair_abi()
        m._validate_logo_pair_abi(abi)

        wrong_alias = copy.deepcopy(abi)
        wrong_alias['vs']['streams']['logo_param1']['field'] = 'material'
        with self.assertRaisesRegex(m.LoweringError, 'unsupported logo pair synthetic ABI'):
            m._validate_logo_pair_abi(wrong_alias)

        wrong_format = copy.deepcopy(abi)
        wrong_format['vs']['streams']['logo_clip']['format'] = 'explicit_float4'
        with self.assertRaisesRegex(m.LoweringError, 'logo_clip.format'):
            m._validate_logo_pair_abi(wrong_format)

        wrong_texture = copy.deepcopy(abi)
        wrong_texture['ps']['image_sample']['texture_slot'] = 1
        with self.assertRaisesRegex(m.LoweringError, 'ps.image_sample.texture_slot'):
            m._validate_logo_pair_abi(wrong_texture)

    def test_logo_pair_generator_rejects_wrong_vs_and_ps_digests(self):
        abi = m._expected_logo_pair_abi()
        with self.assertRaisesRegex(m.LoweringError, 'pinned logo VS SHA-256 mismatch'):
            m.generate_logo_pair(bytes(m.LOGO_VS_BYTES), bytes(m.LOGO_PS_BYTES), abi)

        # Exercise the PS digest gate independently of the earlier VS digest gate.
        with (patch.object(m, '_validate_pinned_logo_vs_code'),
              patch.object(m.Lowerer, 'run', return_value=([], dict(stage='vs')))):
            with self.assertRaisesRegex(m.LoweringError, 'pinned logo PS SHA-256 mismatch'):
                m.generate_logo_pair(b'v' * m.LOGO_VS_BYTES,
                                     bytes(m.LOGO_PS_BYTES), abi)

    def test_logo_pair_cli_rejects_outside_local_abi_and_output(self):
        workspace = Path(m.__file__).resolve().parents[2]
        local = workspace / 'local'
        cases = (
            (Path(tempfile.gettempdir()) / 'gcn-to-msl-outside.abi.json',
             local / 'unused.metal', local / 'unused-report.json', 'ABI'),
            (local / 'unused-abi.json',
             Path(tempfile.gettempdir()) / 'gcn-to-msl-outside.metal',
             local / 'unused-report.json', 'MSL output'),
        )
        for abi, output, report, expected in cases:
            with self.subTest(expected=expected):
                stderr = io.StringIO()
                args = [
                    '--profile', 'logo-pair',
                    '--vs', str(local / 'missing-vs.bin'),
                    '--ps', str(local / 'missing-ps.bin'),
                    '--abi', str(abi), '--output', str(output), '--report', str(report),
                ]
                with redirect_stderr(stderr):
                    result = m.main(args)
                self.assertEqual(result, 1)
                self.assertIn(f'logo-pair {expected} must resolve under workspace local/',
                              stderr.getvalue())

    def test_logo_pair_cli_rejects_output_colliding_with_shader_input(self):
        workspace = Path(m.__file__).resolve().parents[2]
        local = workspace / 'local'
        vs = local / 'missing-vs.bin'
        report = local / 'unused-report.json'
        stderr = io.StringIO()
        args = [
            '--profile', 'logo-pair',
            '--vs', str(vs), '--ps', str(local / 'missing-ps.bin'),
            '--abi', str(local / 'unused-abi.json'), '--output', str(vs),
            '--report', str(report),
        ]
        with redirect_stderr(stderr):
            result = m.main(args)
        self.assertEqual(result, 1)
        self.assertIn('logo-pair MSL output path collides with pinned logo VS path',
                      stderr.getvalue())
        self.assertFalse(vs.exists())
        self.assertFalse(report.exists())

    def test_private_local_path_rejects_symlink_escape(self):
        workspace = Path(m.__file__).resolve().parents[2]
        local_root = m._private_local_root(workspace)
        with tempfile.TemporaryDirectory() as outside_dir, \
                tempfile.TemporaryDirectory(dir=local_root) as private_dir:
            escape = Path(private_dir) / 'escape'
            escape.symlink_to(outside_dir, target_is_directory=True)
            with self.assertRaisesRegex(m.LoweringError,
                                        'logo-pair MSL output must resolve under workspace local/'):
                m._require_private_local_path(escape / 'logo-pair.metal',
                                              local_root, 'MSL output')


if __name__=='__main__':unittest.main()
