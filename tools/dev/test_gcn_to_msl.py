#!/usr/bin/env python3
"""Synthetic lowering/ABI failures; no game code or private ABI fixtures."""
import hashlib
import importlib.util
from pathlib import Path
import sys
import unittest


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


if __name__=='__main__':unittest.main()
