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


def sop2(op=0, dst=6, s0=8, s1=10, literal=None):
    encoded = word((2, 0b10), (7, op), (7, dst), (8, s1), (8, s0))
    return encoded + (struct.pack('<I', literal) if literal is not None else b'')


def sopc(op=9, s0=8, s1=4, literal=None):
    encoded = word((9, 0b101111110), (7, op), (8, s1), (8, s0))
    return encoded + (struct.pack('<I', literal) if literal is not None else b'')


def vopc(op=4, src0=257, src1=2, literal=None):
    encoded = word((7, 0b0111110), (8, op), (8, src1), (9, src0))
    return encoded + (struct.pack('<I', literal) if literal is not None else b'')


def smrd(op=2, dst=12, base=6, imm=1, offset=17, literal=None):
    encoded = word((5, 0b11000), (5, op), (7, dst), (6, base), (1, imm), (8, offset))
    return encoded + (struct.pack('<I', literal) if literal is not None else b'')


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

    def test_scalar_alu_fields_and_literal(self):
        i = one(sop2(op=14, dst=106, s0=2, s1=255, literal=0x2a))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('SOP2', 14, 's_and_b32', 8))
        self.assertEqual(i.fields, dict(ssrc0=2, ssrc1=255, sdst=106, literal=0x2a))
        i = one(sop2(op=38, dst=106, s0=2, s1=192))
        self.assertEqual(i.fields, dict(ssrc0=2, ssrc1=192, sdst=106))
        i = one(sop2(op=42, dst=8, s0=10, s1=12))
        self.assertEqual((i.mnemonic, i.size), ('s_bfe_i64', 4))

    def test_scalar_compare_fields(self):
        i = one(sopc(op=9, s0=3, s1=4))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('SOPC', 9, 's_cmp_ge_u32', 4))
        self.assertEqual(i.fields, dict(ssrc0=3, ssrc1=4))
        self.assertEqual(i.requirements, ('writes_scc',))
        self.assertEqual(one(sopc(op=0, s0=8, s1=255, literal=7)).fields,
                         dict(ssrc0=8, ssrc1=255, literal=7))

    def test_compare_to_vcc_encoding(self):
        # VOPC has no VDST: opcodes 0-15 write VCC, 16-31 (`cmpx`) overwrite EXEC.
        i = one(vopc(op=4, src0=257, src1=2))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('VOPC', 4, 'v_cmp_gt_f32', 4))
        self.assertEqual(i.fields, dict(src0=257, vsrc1=2))
        self.assertEqual(i.requirements, ('writes_vcc_mask',))
        i = one(vopc(op=16))
        self.assertEqual((i.mnemonic, i.requirements), ('v_cmpx_f_f32', ('writes_exec_mask',)))
        i = one(vopc(op=16, src0=255, literal=0x3f800000))
        self.assertEqual((i.size, i.fields['literal']), (8, 0x3f800000))

    def test_integer_cmpx_and_dword_buffers(self):
        # VOPC 208..213 are the integer `cmpx` compares; they overwrite EXEC like the f32 cmpx set.
        i = one(vopc(op=209, src0=257, src1=2))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('VOPC', 209, 'v_cmpx_lt_u32', 4))
        self.assertEqual(i.requirements, ('writes_exec_mask',))
        self.assertEqual(one(vopc(op=212)).mnemonic, 'v_cmpx_gt_u32')
        self.assertEqual(one(vopc(op=208)).mnemonic, 'v_cmpx_f_u32')
        self.assertEqual(one(vopc(op=213)).mnemonic, 'v_cmpx_ne_u32')

        # MUBUF dword forms: 12/13/14 load 1/2/4 components; 15..17 store them.
        load = one(mubuf(op=14, data=17, addr=59, resource=6))
        self.assertEqual((load.mnemonic, load.fields['components']), ('buffer_load_dwordx4', 4))
        self.assertEqual(one(mubuf(op=12)).fields['components'], 1)
        self.assertEqual(one(mubuf(op=13)).fields['components'], 2)
        self.assertEqual(one(mubuf(op=15)).mnemonic, 'buffer_store_dword')
        self.assertEqual(one(mubuf(op=17)).fields['components'], 4)

        # Neighbouring rows stay unsupported: no dwordx3, no store_format_xyzw in this table.
        for opcode in (11, 18):
            with self.assertRaises(gcn.DecodeError):
                one(mubuf(op=opcode))
        for opcode in (207, 214):
            with self.assertRaises(gcn.DecodeError):
                one(vopc(op=opcode))

    def test_barrier_subrev_and_byte_pack(self):
        # SOPP 10 s_barrier decodes as a fixed one-dword instruction and is a wave-level blocker.
        # The pinned PS4 SOPP table puts S_BARRIER at 10; opcode 8 is S_CBRANCH_EXECZ, a branch.
        barrier = one(sopp(op=10))
        self.assertEqual((barrier.encoding, barrier.mnemonic, barrier.size),
                         ('SOPP', 's_barrier', 4))
        self.assertEqual(barrier.requirements, ('wave_barrier',))
        self.assertFalse(gcn.inventory(sopp(op=10) + sopp(op=1))['per_lane_eligible'])

        # VOP2 5 v_subrev_f32: src0 - src1 (reversed operands), still two sources.
        self.assertEqual(one(vop2(op=5)).mnemonic, 'v_subrev_f32')
        # VOP3 350 v_cvt_pk_u8_f32 is a three-source row.
        packed = one(vop3(op=350, abs_=0, clamp=0, omod=0, neg=0))
        self.assertEqual(packed.mnemonic, 'v_cvt_pk_u8_f32')
        self.assertEqual(packed.fields['src2'], 269)

        # Neighbouring rows still reject: branches, other SOPP rows, other VOP2/VOP3 slots.
        # Branch rows are named as control transfers and never decoded; a barrier with a nonzero
        # immediate is refused too.
        for imm, opcode in ((0, 2), (0, 8), (0, 9), (1, 10)):
            with self.assertRaises(gcn.DecodeError):
                one(sopp(op=opcode, imm=imm))
        with self.assertRaises(gcn.DecodeError) as caught:
            one(sopp(op=8))
        self.assertIn('control transfer', caught.exception.reason)
        with self.assertRaises(gcn.DecodeError):
            one(vop2(op=6))
        with self.assertRaises(gcn.DecodeError):
            one(vop3(op=351, abs_=0, clamp=0, omod=0, neg=0))

    def test_scalar_memory_register_and_literal_offsets(self):
        # IMM=1 is the byte-immediate offset; IMM=0 makes OFFSET an SGPR selector and
        # selector 255 consumes the following dword (same width rule as SOP1 literals).
        i = one(smrd(op=0, dst=6, base=10, imm=0, offset=7))
        self.assertEqual((i.size, i.fields['offset'], i.fields['imm']), (4, 7, 0))
        i = one(smrd(op=0, dst=28, base=10, imm=0, offset=255, literal=0x100))
        self.assertEqual((i.size, i.fields['literal'], i.fields['base_sgpr']), (8, 0x100, 20))

    def test_buffer_load_widths(self):
        for op, dwords in ((8, 1), (9, 2), (10, 4), (11, 8), (12, 16)):
            with self.subTest(op=op):
                i = one(smrd(op=op, dst=0, base=0))
                self.assertEqual((i.fields['dwords'], i.fields['base_sgpr']), (dwords, 0))

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

    def test_integer_vector_rows(self):
        # VOP2 37 writes its carry out to VCC (the VOP2 form has no SDST field to name);
        # 22 is the reversed-shift row and 3 the f32 add. VOP1 36 is a one-source rounding row.
        i = one(vop2(op=37, dst=9, src1=5, src0=257))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('VOP2', 37, 'v_add_i32', 4))
        self.assertEqual(i.fields, dict(src0=257, vsrc1=5, vdst=9))
        self.assertEqual(i.requirements, ('writes_vcc_mask',))
        i = one(vop2(op=22, dst=4, src1=6, src0=8))
        self.assertEqual((i.mnemonic, i.requirements), ('v_lshrrev_b32', ()))
        i = one(vop2(op=3, dst=4, src1=6, src0=8))
        self.assertEqual((i.mnemonic, i.requirements), ('v_add_f32', ()))
        i = one(vop1(op=36, dst=11, src=272))
        self.assertEqual((i.encoding, i.mnemonic, i.fields),
                         ('VOP1', 'v_floor_f32', dict(src0=272, vdst=11)))

    def test_vop3_three_source_integer_rows(self):
        i = one(vop3(op=349, dst=11, src0=267, src1=128, src2=269,
                     abs_=0, clamp=0, omod=0, neg=0))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic, i.size),
                         ('VOP3', 349, 'v_sad_u32', 8))
        self.assertEqual(i.fields, dict(vdst=11, abs=0, clamp=0, src0=267, src1=128,
                                        src2=269, omod=0, neg=0))
        i = one(vop3(op=322, dst=12, src0=266, src1=267, src2=268,
                     abs_=0, clamp=0, omod=0, neg=0))
        self.assertEqual((i.mnemonic, i.fields['src2']), ('v_mad_i32_i24', 268))
        # The third source really is read: a VGPR and an inline constant both decode.
        self.assertEqual(one(vop3(op=349, src0=257, src1=258, src2=190,
                                  abs_=0, clamp=0, omod=0, neg=0)).fields['src2'], 190)

    def test_saveexec_and_single_format_fetch(self):
        i = one(sop1(op=36, dst=8, src=10))
        self.assertEqual((i.encoding, i.opcode, i.mnemonic),
                         ('SOP1', 36, 's_and_saveexec_b64'))
        self.assertEqual(i.fields, dict(src0=10, sdst=8))
        self.assertEqual(i.requirements, ('reads_exec_mask', 'writes_exec_mask'))
        # Saving and masking EXEC can never be a per-lane conversion.
        self.assertFalse(gcn.inventory(sop1(op=36, dst=8, src=10) + END)['per_lane_eligible'])
        i = one(mubuf(op=0))
        self.assertEqual((i.mnemonic, i.fields['components']), ('buffer_load_format_x', 1))
        # One component is the only row that may end on the last VGPR.
        self.assertEqual(one(mubuf(op=0, data=255)).fields['vdata'], 255)

    def test_every_supported_opcode(self):
        # Deliberately does not enumerate the decoder's OPCODES table.
        families = [
            (sop1, [(3, 's_mov_b32'), (4, 's_mov_b64'), (10, 's_wqm_b64'),
                    (36, 's_and_saveexec_b64')]),
            (sop2, [(0, 's_add_u32'), (14, 's_and_b32'), (38, 's_mul_i32'),
                    (42, 's_bfe_i64'), (44, 's_absdiff_i32')]),
            (sopc, [(0, 's_cmp_eq_i32'), (9, 's_cmp_ge_u32'), (12, 's_bitcmp0_b32'),
                    (15, 's_bitcmp1_b64')]),
            (sopp, [(12, 's_waitcnt'), (22, 's_ttracedata')]),
            (smrd, [(0, 's_load_dword'), (1, 's_load_dwordx2'), (2, 's_load_dwordx4'),
                    (3, 's_load_dwordx8'), (4, 's_load_dwordx16'),
                    (8, 's_buffer_load_dword'), (9, 's_buffer_load_dwordx2'),
                    (10, 's_buffer_load_dwordx4'), (11, 's_buffer_load_dwordx8'),
                    (12, 's_buffer_load_dwordx16')]),
            (vop1, [(1, 'v_mov_b32'), (36, 'v_floor_f32'), (46, 'v_rsq_f32')]),
            (vop2, [(3, 'v_add_f32'), (4, 'v_sub_f32'), (8, 'v_mul_f32'),
                    (22, 'v_lshrrev_b32'), (31, 'v_mac_f32'), (37, 'v_add_i32'),
                    (47, 'v_cvt_pkrtz_f16_f32')]),
            (vop3, [(264, 'v_mul_f32'), (303, 'v_cvt_pkrtz_f16_f32'), (321, 'v_mad_f32'),
                    (322, 'v_mad_i32_i24'), (349, 'v_sad_u32')]),
            (vopc, [(0, 'v_cmp_f_f32'), (15, 'v_cmp_t_f32'),
                    (16, 'v_cmpx_f_f32'), (31, 'v_cmpx_t_f32')]),
            (vintrp, [(0, 'v_interp_p1_f32'), (1, 'v_interp_p2_f32')]),
            (mubuf, [(0, 'buffer_load_format_x'), (1, 'buffer_load_format_xy'),
                     (2, 'buffer_load_format_xyz'),
                     (3, 'buffer_load_format_xyzw')]),
            (mimg, [(32, 'image_sample')]),
        ]
        for make, cases in families:
            for op, name in cases:
                with self.subTest(op=op, mnemonic=name):
                    kw = dict(src=8) if make is sop1 else {}
                    if make is smrd:
                        kw = dict(dst=0, base=0)
                    if make is vop3 and op in (322, 349):
                        kw = dict(abs_=0, clamp=0, omod=0, neg=0)
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
        # Scalar ALU/compare rows carry the same visibility rules as the other families.
        i = one(sop2(op=14, dst=126, s0=252, s1=10))
        self.assertEqual(i.requirements, ('reads_exec_mask', 'writes_exec_mask'))
        i = one(sopc(op=9, s0=10, s1=251))
        self.assertEqual(i.requirements, ('reads_vcc_mask', 'writes_scc'))
        # A VOPC mask write to VCC alone is not a quad/wave/LDS blocker; `cmpx` is, because
        # it overwrites EXEC (v_cmpx_* 16-31).
        self.assertTrue(gcn.inventory(vopc() + END)['per_lane_eligible'])
        self.assertFalse(gcn.inventory(vopc(op=16) + END)['per_lane_eligible'])
        self.assertFalse(gcn.inventory(vopc(src0=254) + END)['per_lane_eligible'])


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
        program = (sop1(src=255)+struct.pack('<I', 0x13579bdf)+sop2(s1=255, literal=9)
                   +sopc()+vopc()+smrd(imm=0, offset=255, literal=0x80)+smrd()
                   +vop3()+mubuf()+mimg()+exp()+END)
        for n in range(len(program)):
            with self.subTest(prefix=n):
                self.reject(program[:n])

    def test_fixed_64_bit_width(self):
        for p in (vop3(), mubuf(), mimg(), exp()):
            self.reject(p[:4], 'truncated')

    def test_literal_width(self):
        for p in (sop1(src=255), vop1(src=255), vop2(src0=255), vopc(src0=255),
                  sop2(s0=255), sop2(s1=255), sopc(s0=255), sopc(s1=255),
                  smrd(imm=0, offset=255)):
            self.reject(p, 'truncated')

    def test_unknown_opcode_each_family(self):
        for p in (sop1(255), sop2(12), sop2(43), sop2(45), sop2(127), sopc(16), sopc(127),
                  sopp(127), smrd(29), smrd(31), vop1(255), vopc(32), vopc(255),
                  vop2(63), vop3(511), vintrp(3), mubuf(127), mimg(127)):
            self.reject(p+END, 'unsupported')

    def test_unknown_encoding(self):
        # DS, MTBUF, VOP1-reserved and reserved high-prefix encodings are not guessed.
        # SOP2/VOPC/SOPC are no longer here: those encodings are now explicitly modelled.
        for p in (0xd8000000, 0xe8000000, 0x7a000000, 0xfc000000):
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

    def test_new_encoding_operand_limits(self):
        for p in (sop2(dst=125), sop2(s0=209), sop2(s1=254),
                  sop2(op=42, dst=7, s0=8, s1=10),
                  sop2(op=42, dst=8, s0=9, s1=10),
                  sop2(op=15, dst=8, s0=8, s1=11),
                  sopc(s0=254), sopc(s1=209), sopc(op=15, s0=9, s1=10),
                  vopc(src0=209), vopc(src0=104), vopc(src0=125),
                  smrd(op=12, dst=96), smrd(op=8, base=1), smrd(op=2, dst=13),
                  smrd(imm=0, offset=104), smrd(imm=0, offset=254)):
            self.reject(p+END)

    def test_integer_vop3_modifier_and_wide_operand_limits(self):
        # The ISA defines no ABS/CLAMP/OMOD/NEG on integer VOP3 rows, so a nonzero field is
        # rejected rather than folded away.
        for kw in (dict(abs_=1), dict(abs_=4), dict(clamp=1), dict(omod=1), dict(neg=2)):
            for op in (322, 349):
                with self.subTest(op=op, kw=kw):
                    self.reject(vop3(op=op, **kw)+END, 'modifier field')
        # Three distinct scalar sources exceed the GFX7 read ports on the new rows too, and
        # VOP3 still has no inline literal (selector 255 is not a source).
        self.reject(vop3(op=349, src0=4, src1=6, src2=8, abs_=0, clamp=0, omod=0, neg=0)+END,
                    'multiple distinct scalar sources')
        self.reject(vop3(op=322, src0=255, abs_=0, clamp=0, omod=0, neg=0)+END)
        self.reject(vop3(op=349, src1=254, abs_=0, clamp=0, omod=0, neg=0)+END)
        # s_and_saveexec_b64 keeps the 64-bit pair rules, and the one-format fetch keeps
        # the VGPR range rule.
        for p in (sop1(op=36, dst=7, src=8), sop1(op=36, dst=124, src=8),
                  sop1(op=36, dst=8, src=9)):
            self.reject(p+END)
        self.reject(mubuf(op=0, offen=1, idxen=1, addr=255)+END)

    def test_nearby_rows_stay_unsupported(self):
        # Neighbours that are NOT modelled keep rejecting: the integer VOP rows either side,
        # SOPP 5 `s_cbranch_scc1` (control transfer), the MUBUF/MIMG store rows.
        for p in (vop2(op=38), vop2(op=40), vop1(op=37),
                  vop3(op=323, abs_=0, clamp=0, omod=0, neg=0),
                  vop3(op=351, abs_=0, clamp=0, omod=0, neg=0),
                  sop1(op=35), sopp(op=2), sopp(op=5), sopp(op=9),
                  mubuf(op=4), mubuf(op=28), mimg(op=8)):
            self.reject(p+END, 'unsupported')

    def test_one_inline_literal_per_instruction(self):
        # Two 255 selectors cannot both consume the single following dword.
        self.reject(sop2(op=11, s0=255, s1=255, literal=0), 'multiple inline literals')
        self.reject(sopc(s0=255, s1=255, literal=0), 'multiple inline literals')

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
