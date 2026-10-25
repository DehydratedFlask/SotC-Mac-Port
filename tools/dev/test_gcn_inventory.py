#!/usr/bin/env python3
"""Independent synthetic encoding oracle for the bounded offline GFX7 reader.

No game fixtures, copied reference code, runtime GPU execution or shared decoder
bit-extraction helpers. The oracle concatenates MSB-first ISA fields; production
extracts little-endian masks. Expected opcodes/fields are specified independently.
"""
import contextlib
import importlib.util
import io
import json
from pathlib import Path
import random
import struct
import sys
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('gcn_inventory', Path(__file__).with_name('gcn_inventory.py'))
gcn = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = gcn
spec.loader.exec_module(gcn)


def word(*fields):
    """Concatenate (width,value) fields from MSB to LSB, independent of reader."""
    text = ''
    for width, value in fields:
        if not 0 <= value < 2 ** width:
            raise ValueError('oracle field does not fit')
        text += format(value, f'0{width}b')
    if len(text) != 32:
        raise ValueError('oracle word is not exactly 32 bits')
    return struct.pack('<I', int(text, 2))


def sop1(op=3, dst=6, src=9):
    return word((9, 0b101111101), (7, dst), (8, op), (8, src))


def sopp(op=1, imm=0):
    return word((9, 0b101111111), (7, op), (16, imm))


def smrd(op=2, dst=12, base=6, imm=1, offset=17):
    return word((5, 0b11000), (5, op), (7, dst), (6, base), (1, imm), (8, offset))


def vop1(op=1, dst=7, src=270):
    return word((7, 0b0111111), (8, dst), (8, op), (9, src))


def vop2(op=8, dst=9, src1=5, src0=257):
    return word((1, 0), (6, op), (8, dst), (8, src1), (9, src0))


def vop3(op=321, dst=11, src0=267, src1=268, src2=269, abs_=3, clamp=1, omod=2, neg=5):
    return (word((6, 0b110100), (9, op), (5, 0), (1, clamp), (3, abs_), (8, dst))
            + word((3, neg), (2, omod), (9, src2), (9, src1), (9, src0)))


def vintrp(op=0, dst=19, attr=31, chan=2, src=17):
    return word((6, 0b110010), (8, dst), (2, op), (6, attr), (2, chan), (8, src))


def mubuf(op=2, data=13, addr=21, resource=6, offset=513, soffset=128,
          offen=1, idxen=0, glc=1, addr64=0, lds=0, slc=1, tfe=0):
    return (word((6, 0b111000), (1, 0), (7, op), (1, 0), (1, lds), (1, addr64),
                 (1, glc), (1, idxen), (1, offen), (12, offset))
            + word((8, soffset), (1, tfe), (1, slc), (1, 0), (5, resource), (8, data), (8, addr)))


def mimg(op=32, data=15, addr=25, resource=6, sampler=3, mask=15,
         unorm=0, glc=1, da=1, r128=0, tfe=0, lwe=0, slc=1):
    return (word((6, 0b111100), (1, slc), (7, op), (1, lwe), (1, tfe), (1, r128),
                 (1, da), (1, glc), (1, unorm), (4, mask), (8, 0))
            + word((6, 0), (5, sampler), (5, resource), (8, data), (8, addr)))


def exp(target=12, en=15, compr=0, done=1, vm=0, src=(3, 5, 7, 9)):
    return (word((6, 0b111110), (13, 0), (1, vm), (1, done), (1, compr), (6, target), (4, en))
            + word(*[(8, x) for x in reversed(src)]))


END = sopp()


def one(encoded):
    return gcn.decode(encoded + END)[0]


class EncodingOracleTests(unittest.TestCase):
    def test_known_control_word(self):
        self.assertEqual(END, struct.pack('<I', 0xbf810000))

    def test_scalar_and_control_fields(self):
        i = one(sop1(dst=107, src=255) + struct.pack('<I', 0x13579bdf))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size), ('SOP1', 3, 's_mov_b32', 8))
        self.assertEqual(i.fields, dict(sdst=107, src0=255, literal=0x13579bdf))
        i = one(sopp(12, 0x1032))
        self.assertEqual(i.fields, dict(simm16=0x1032, vmcnt=2, expcnt=3, lgkmcnt=16))
        i = one(smrd())
        self.assertEqual(i.fields, dict(offset=17, imm=1, sbase=6, sdst=12, dwords=4, base_sgpr=12))

    def test_smrd_destination_high_bit_and_opcode_are_independent(self):
        # Explicitly catches the overlapping/omitted bit-21 typo in the PDF.
        i = one(smrd(0, dst=65))
        self.assertEqual((i.opcode, i.fields['sdst']), (0, 65))
        i = one(smrd(1, dst=64))
        self.assertEqual((i.opcode, i.fields['sdst']), (1, 64))

    def test_vector_fields_and_literal(self):
        i = one(vop1(src=255) + struct.pack('<I', 0x2468ace0))
        self.assertEqual(i.fields, dict(src0=255, vdst=7, literal=0x2468ace0))
        i = one(vop2())
        self.assertEqual(i.fields, dict(src0=257, vsrc1=5, vdst=9))
        i = one(vop3())
        self.assertEqual(i.fields, dict(vdst=11, abs=3, clamp=1, src0=267,
                                       src1=268, src2=269, omod=2, neg=5))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size), ('VOP3', 321, 'v_mad_f32', 8))

    def test_interpolation_fields(self):
        self.assertEqual(one(vintrp()).fields, dict(vsrc=17, attrchan=2, attr=31, vdst=19))

    def test_buffer_fields(self):
        self.assertEqual(one(mubuf()).fields, dict(offset=513, offen=1, idxen=0, glc=1,
                         addr64=0, lds=0, vaddr=21, vdata=13, srsrc=6, slc=1, tfe=0,
                         soffset=128, resource_sgpr=24, components=3))

    def test_image_fields(self):
        self.assertEqual(one(mimg()).fields, dict(dmask=15, unorm=0, glc=1, da=1,
                         r128=0, tfe=0, lwe=0, slc=1, vaddr=25, vdata=15, srsrc=6,
                         ssamp=3, resource_sgpr=24, sampler_sgpr=12))

    def test_export_fields(self):
        self.assertEqual(one(exp()).fields, dict(en=15, target=12, compr=0, done=1,
                         vm=0, vsrc0=3, vsrc1=5, vsrc2=7, vsrc3=9))

    def test_every_supported_opcode(self):
        # Deliberately does not enumerate the decoder's OPCODES table.
        families = [
            (sop1, [(3, 's_mov_b32'), (4, 's_mov_b64'), (10, 's_wqm_b64')]),
            (sopp, [(12, 's_waitcnt'), (22, 's_ttracedata')]),
            (smrd, [(0, 's_load_dword'), (1, 's_load_dwordx2'), (2, 's_load_dwordx4'),
                    (3, 's_load_dwordx8'), (4, 's_load_dwordx16'),
                    (10, 's_buffer_load_dwordx4'), (11, 's_buffer_load_dwordx8')]),
            (vop1, [(1, 'v_mov_b32'), (46, 'v_rsq_f32')]),
            (vop2, [(4, 'v_sub_f32'), (8, 'v_mul_f32'), (31, 'v_mac_f32'),
                    (47, 'v_cvt_pkrtz_f16_f32')]),
            (vop3, [(264, 'v_mul_f32'), (303, 'v_cvt_pkrtz_f16_f32'), (321, 'v_mad_f32')]),
            (vintrp, [(0, 'v_interp_p1_f32'), (1, 'v_interp_p2_f32')]),
            (mubuf, [(1, 'buffer_load_format_xy'), (2, 'buffer_load_format_xyz'),
                     (3, 'buffer_load_format_xyzw')]),
            (mimg, [(32, 'image_sample')]),
        ]
        for make, cases in families:
            for op, name in cases:
                with self.subTest(op=op, mnemonic=name):
                    kw = dict(src=8) if make is sop1 else {}
                    self.assertEqual(one(make(op=op, **kw)).mnemonic, name)
        self.assertEqual(one(exp()).mnemonic, 'exp')
        self.assertEqual(gcn.decode(END)[0].mnemonic, 's_endpgm')

    def test_randomized_field_oracle(self):
        rng = random.Random(731301)
        for _ in range(1000):
            a, b, c = (rng.randrange(256) for _ in range(3))
            dst = rng.randrange(256)
            abs_, clamp, omod, neg = rng.randrange(8), rng.randrange(2), rng.randrange(4), rng.randrange(8)
            f = one(vop3(dst=dst, src0=a+256, src1=b+256, src2=c+256,
                         abs_=abs_, clamp=clamp, omod=omod, neg=neg)).fields
            self.assertEqual(f, dict(vdst=dst, abs=abs_, clamp=clamp, src0=a+256,
                                     src1=b+256, src2=c+256, omod=omod, neg=neg))

    def test_exact_boundaries_and_literal_not_instruction(self):
        # A literal identical to s_endpgm must not terminate the program.
        program = sop1(src=255) + END + vop3() + exp() + END
        rows = gcn.decode(program)
        self.assertEqual([(i.offset, i.size) for i in rows], [(0, 8), (8, 8), (16, 8), (24, 4)])
        self.assertEqual(rows[0].fields['literal'], 0xbf810000)

    def test_quad_and_exec_semantics_are_visible(self):
        i = one(sop1(op=10, dst=126, src=126))
        self.assertEqual(i.requirements, ('reads_exec_mask', 'whole_quad_mode', 'writes_exec_mask'))
        self.assertFalse(gcn.inventory(sop1(op=10, dst=126, src=126) + END)['per_lane_eligible'])
        self.assertFalse(gcn.inventory(mimg() + END)['per_lane_eligible'])
        self.assertTrue(gcn.inventory(vop3() + vintrp() + exp() + END)['per_lane_eligible'])


class RejectionTests(unittest.TestCase):
    def reject(self, encoded, contains=None):
        with self.assertRaises(gcn.DecodeError) as caught:
            gcn.decode(encoded)
        if contains:
            self.assertIn(contains, str(caught.exception))

    def test_empty_and_unaligned(self):
        for p in (b'', b'\x00', END+b'\x00', END+b'\x00\x00', END+b'\x00\x00\x00'):
            self.reject(p)

    def test_every_truncated_prefix_of_synthetic_program(self):
        program = sop1(src=255)+struct.pack('<I', 0x13579bdf)+smrd()+vop3()+mubuf()+mimg()+exp()+END
        for n in range(len(program)):
            with self.subTest(prefix=n):
                self.reject(program[:n])

    def test_fixed_64_bit_width(self):
        for p in (vop3(), mubuf(), mimg(), exp()):
            self.reject(p[:4], 'truncated')

    def test_literal_width(self):
        for p in (sop1(src=255), vop1(src=255), vop2(src0=255)):
            self.reject(p, 'truncated')
        self.reject(smrd(imm=0, offset=255)+END, 'unsupported SMRD')

    def test_unknown_opcode_each_family(self):
        for p in (sop1(255), sopp(127), smrd(31), vop1(255), vop2(63),
                  vop3(511), vintrp(3), mubuf(127), mimg(127)):
            self.reject(p+END, 'unsupported')

    def test_unknown_encoding(self):
        # DS, VOPC, SOP2 and reserved high-prefix encodings are not guessed.
        for p in (0xd8000000, 0x7c000000, 0x80000000, 0xfc000000):
            self.reject(struct.pack('<I', p)+END, 'unsupported')

    def test_reserved_bits(self):
        cases = [(vop3(), 12), (vop3(), 16), (mubuf(), 17), (mubuf(), 25),
                 (mubuf(), 53), (mimg(), 0), (mimg(), 7), (mimg(), 58),
                 (mimg(), 63), (exp(), 13), (exp(), 25)]
        for p, bit in cases:
            self.reject((int.from_bytes(p, 'little') | 1 << bit).to_bytes(len(p), 'little')+END,
                        'reserved')

    def test_bad_operands_and_alignment(self):
        for p in (sop1(dst=125), sop1(src=209), sop1(op=4, dst=7, src=8),
                  sop1(op=4, dst=6, src=7), sop1(op=4, dst=124, src=8),
                  smrd(dst=13), smrd(dst=100, op=4), smrd(base=53),
                  smrd(op=10, base=3), vop1(src=209), vop3(src0=255),
                  vop3(src0=4, src1=8), vop3(src1=254),
                  mubuf(resource=31), mubuf(data=255), mubuf(soffset=255),
                  mimg(mask=0), mimg(resource=25), mimg(sampler=31), exp(target=10),
                  exp(compr=1, en=1), sopp(1, 1), sopp(12, 0x80), sopp(22, 1)):
            self.reject(p+END)

    def test_end_is_exact_and_required(self):
        self.reject(vop3(), 'does not end')
        self.reject(END+END, 'follow s_endpgm')
        self.reject(END+vop3(), 'follow s_endpgm')

    def test_decode_error_identifies_offset(self):
        with self.assertRaises(gcn.DecodeError) as c:
            gcn.decode(vop1()+struct.pack('<I', 0xffffffff))
        self.assertEqual(c.exception.offset, 4)

    def test_cli_failures_and_private_listing_option(self):
        with tempfile.TemporaryDirectory() as td:
            path = Path(td)/'synthetic.bin'
            path.write_bytes(sop1(op=10, dst=126, src=126)+END)
            with contextlib.redirect_stdout(io.StringIO()) as out, contextlib.redirect_stderr(io.StringIO()) as err:
                code = gcn.main([str(path), '--json', '--summary-only', '--require-per-lane'])
            self.assertEqual(code, 1)
            result = json.loads(out.getvalue())
            self.assertNotIn('instructions', result)
            self.assertEqual(result['unknown_count'], 0)
            self.assertIn('quad/wave/LDS', err.getvalue())
            path.write_bytes(b'\x00')
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()) as err:
                self.assertEqual(gcn.main([str(path)]), 1)
            self.assertIn('FAILED', err.getvalue())


if __name__ == '__main__':
    unittest.main()
