#!/usr/bin/env python3
"""ELF byte-oracle for slot 554 batch 2b (prefix body 0x6fc29..0x7036d).

Covers the 0xd0 `XPR Occluder Debug Mode` object built by the 0x6abb0 wrapper (inner 0x6ad40)
and the seven 0xc0 bools at 0x2eaa4a0..0x2eaa9e0. Slot 554 remains PARTIAL; the remainder of
the body [0x7036d,0x718bb) is a separate open unit. Offline only: reads the pinned private ELF,
never loads or executes an original RVA.
"""
from __future__ import annotations
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main  # noqa: E402
from slot_pin_util import ROOT  # noqa: E402,F401

PINS = [
    # object 0 / 0x6abb0 wrapper and its post-wrapper releases
    ('rip', 0x6fc29, 7, 3, 0x2eaa3d0, 'obj0_6fc29'),
    ('rip', 0x6fc30, 7, 3, 0xf9a8bb, 'name0_6fc30'),
    ('bytes', 0x6fc47, '498d5618', 'spec0_6fc47'),
    ('bytes', 0x6fc59, 'c744243c00000000', 'low0_6fc59'),
    ('bytes', 0x6fc61, 'c744243802000000', 'high0_6fc61'),
    ('bytes', 0x6fc8e, '48c744246817000000', 'len0_6fc8e'),
    ('call', 0x6fcdf, 0x6abb0, 'call0_6fcdf'),
    ('call', 0x6fceb, 0xf590, 'call0_6fceb'),
    ('bytes', 0x6fd00, 'ff5018', 'destroy_view_6fd00'),
    ('call', 0x6fd06, 0xf590, 'call0_6fd06'),
    ('rip', 0x6fd0b, 7, 3, 0x54b60, 'atexit0_6fd0b'),
    ('call', 0x6fd18, 0xf8f390, 'call0_6fd18'),
    # bool 1
    ('rip', 0x6fd1d, 7, 3, 0x2eaa4a0, 'obj1_6fd1d'),
    ('rip', 0x6fd2c, 7, 3, 0xf9a8d4, 'name1_6fd2c'),
    ('bytes', 0x6fd97, '498d471c', 'page1_6fd97'),
    ('call', 0x6fd78, 0x20af0, 'call1_6fd78'),
    ('call', 0x6fdcb, 0xf590, 'call1_6fdcb'),
    ('call', 0x6fde2, 0x20d60, 'call1_6fde2'),
    ('call', 0x6fdec, 0xf590, 'call1_6fdec'),
    ('call', 0x6fe05, 0xf8f390, 'call1_6fe05'),
    # bool 2
    ('rip', 0x6fe0a, 7, 3, 0x2eaa560, 'obj2_6fe0a'),
    ('rip', 0x6fe11, 7, 3, 0xf9a8f2, 'name2_6fe11'),
    ('bytes', 0x6fe74, '498d471d', 'page2_6fe74'),
    ('call', 0x6fe6c, 0x20af0, 'call2_6fe6c'),
    ('call', 0x6feb3, 0xf590, 'call2_6feb3'),
    ('call', 0x6fec5, 0x20d60, 'call2_6fec5'),
    ('call', 0x6fecf, 0xf590, 'call2_6fecf'),
    ('call', 0x6fee5, 0xf8f390, 'call2_6fee5'),
    # bool 3
    ('rip', 0x6feea, 7, 3, 0x2eaa620, 'obj3_6feea'),
    ('rip', 0x6fef1, 7, 3, 0xf9a903, 'name3_6fef1'),
    ('bytes', 0x6ff54, '498d471e', 'page3_6ff54'),
    ('call', 0x6ff4c, 0x20af0, 'call3_6ff4c'),
    ('call', 0x6ff96, 0xf590, 'call3_6ff96'),
    ('call', 0x6ffad, 0x20d60, 'call3_6ffad'),
    ('call', 0x6ffb7, 0xf590, 'call3_6ffb7'),
    ('call', 0x6ffcd, 0xf8f390, 'call3_6ffcd'),
    # bool 4
    ('rip', 0x6ffd2, 7, 3, 0x2eaa6e0, 'obj4_6ffd2'),
    ('rip', 0x6ffd9, 7, 3, 0xf9a917, 'name4_6ffd9'),
    ('bytes', 0x7003c, '498d471f', 'page4_7003c'),
    ('call', 0x70034, 0x20af0, 'call4_70034'),
    ('call', 0x7007b, 0xf590, 'call4_7007b'),
    ('call', 0x70090, 0x20d60, 'call4_70090'),
    ('call', 0x7009a, 0xf590, 'call4_7009a'),
    ('call', 0x700b0, 0xf8f390, 'call4_700b0'),
    # bool 5
    ('rip', 0x700b5, 7, 3, 0x2eaa7a0, 'obj5_700b5'),
    ('rip', 0x700bc, 7, 3, 0xf9a92c, 'name5_700bc'),
    ('bytes', 0x70124, '498d4720', 'page5_70124'),
    ('call', 0x70117, 0x20af0, 'call5_70117'),
    ('call', 0x70163, 0xf590, 'call5_70163'),
    ('call', 0x70175, 0x20d60, 'call5_70175'),
    ('call', 0x7017f, 0xf590, 'call5_7017f'),
    ('call', 0x70195, 0xf8f390, 'call5_70195'),
    # bool 6
    ('rip', 0x7019a, 7, 3, 0x2eaa860, 'obj6_7019a'),
    ('rip', 0x701a1, 7, 3, 0xf9a94a, 'name6_701a1'),
    ('bytes', 0x70204, '498d4721', 'page6_70204'),
    ('call', 0x701fc, 0x20af0, 'call6_701fc'),
    ('call', 0x70246, 0xf590, 'call6_70246'),
    ('call', 0x7025d, 0x20d60, 'call6_7025d'),
    ('call', 0x70267, 0xf590, 'call6_70267'),
    ('call', 0x7027d, 0xf8f390, 'call6_7027d'),
    # bool 7
    ('rip', 0x70282, 7, 3, 0x2eaa920, 'obj7_70282'),
    ('rip', 0x70289, 7, 3, 0xf9a971, 'name7_70289'),
    ('bytes', 0x70304, '498d4622', 'page7_70304'),
    ('call', 0x702e7, 0x20af0, 'call7_702e7'),
    ('call', 0x70335, 0xf590, 'call7_70335'),
    ('call', 0x70347, 0x20d60, 'call7_70347'),
    ('call', 0x7034f, 0xf590, 'call7_7034f'),
    ('call', 0x70368, 0xf8f390, 'call7_70368'),
    # 0x6abb0 outer wrapper / 0x6ad40 inner
    ('bytes', 0x6abb0, '554889e541574156', 'wrapper_head_6abb0'),
    ('bytes', 0x6ad40, '554889e541574156', 'inner_head_6ad40'),
    ('rip', 0x6abee, 8, 3, 0x15968b9, 'wrapper_view_vptr_6abee'),
    ('call', 0x6ace3, 0x6ad40, 'wrapper_inner_call_6ace3'),
    ('rip', 0x6acf9, 7, 3, 0x15968d8, 'wrapper_final_table_6acf9'),
    ('bytes', 0x6ad03, '4883c010', 'wrapper_final_plus10_6ad03'),
    ('call', 0x6ad10, 0x20d60, 'wrapper_register_6ad10'),
    ('call', 0x6ad37, 0xf8f3a0, 'wrapper_canary_fail_6ad37'),
    ('call', 0x6ad76, 0x20af0, 'inner_base_6ad76'),
    ('rip', 0x6ad7b, 7, 3, 0x1595e80, 'inner_mid_table_6ad7b'),
    ('bytes', 0x6ad8d, '4883c010', 'inner_mid_plus10_6ad8d'),
    ('rip', 0x6ad86, 8, 3, 0x15968b9, 'inner_tag_6ad86'),
    ('call', 0x6ae9a, 0xf8f3a0, 'inner_canary_fail_6ae9a'),
]

def elf_extra(e) -> None:
    e.cstring(0xf9a8bb, b'XPR Occluder Debug Mode')
    e.cstring(0xf9a8d4, b'enableNonPower2DepthOccluders')
    e.cstring(0xf9a8f2, b'dumpXPRInfoStats')
    e.cstring(0xf9a903, b'useSunShadowIndices')
    e.cstring(0xf9a917, b'doIndexBufferCulling')
    e.cstring(0xf9a92c, b'doSunShadowIndexBufferCulling')
    e.cstring(0xf9a94a, b'enableSmallPrimitiveIndexBufferCulling')
    e.cstring(0xf9a971, b'forceUseLastIndexBufferCullingResults')
    e.cstring(0xf9a8d3, b'')

SPEC = Spec(
    name='slot 554 batch 2b', index=554, base=0x6fc29, size=0x744,
    sha='9244a7d9ba3a205fcbe095c324106ac32c3dc2ab9f362257a1d2def3f4066d9b',
    pins=PINS,
    native_files={
        'header': 'native/core/ctor_slot_554_2b_6fc29.hpp',
        'source': 'native/core/ctor_slot_554_2b_6fc29.cpp',
        'test': 'native/tests/test_ctor_slot_554_batch2b.cpp',
        'cmake': 'native/CMakeLists.txt',
    },
    callees={
        'wrapper 0x6abb0': (0x6abb0, 0x18d,
                            '242aa9fe5b26a4dca9060f681303fc14a074cb9f08299e264452c207e6ed39ad'),
        'inner 0x6ad40': (0x6ad40, 0x160,
                         'e4e6ec135d1f3af7fa2163fc8b0d5657b0d5aa404d494d0533d569289dcf8c9b'),
        'base ctor 0x20af0': (0x20af0, 0x5a,
                              'e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45'),
        'register 0x20d60': (0x20d60, 0x1a,
                             'f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403'),
        'release 0xf590': (0xf590, 0x49,
                           '55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47'),
        'atexit 0xf8f390': (0xf8f390, 0x1b,
                            '4314f548f230d5ca2b9d5193e0a00a493cb8e13d59335d852b80a1a514bbbbab'),
    },
    header_constants={
        'kSlot554B2BRva': 0x6fc29,
        'kSlot554B2BBodySize': 0x744,
        'kSlot554B2BObjectBaseRva': 0x2eaa3d0,
        'kSlot554B2BObjectCount': 8,
        'kSlot554B2BFunctorSize': 0xd0,
        'kSlot554B2BBoolSize': 0xc0,
        'kSlot554B2BBlockSize': 0x610,
        'kSlot554B2BWritten': 1329,
        'kSlot554B2BHoles': 223,
        'kSlot554B2BInstructionCount': 328,
        'kSlot554B2BDirectCallCount': 39,
        'kSlot554B2BIndirectCallCount': 1,
        'kSlot554B2BObjectStoreCount': 63,
        'kSlot554B2BStackStoreCount': 54,
        'kSlot554B2BFunctorWritten': 195,
        'kSlot554B2BFunctorHoles': 13,
        'kSlot554B2BWrapperRva': 0x6abb0,
        'kSlot554B2BWrapperSize': 0x18d,
        'kSlot554B2BInnerRva': 0x6ad40,
        'kSlot554B2BInnerSize': 0x160,
        'kSlot554B2BInnerMidVptr': 0x1595e90,
        'kSlot554B2BFinalVptr': 0x15968e8,
        'kSlot554B2BTagVptr': 0x15968b9,
        'kSlot554B2BViewVptr': 0x15968b9,
        'kSlot554B2BDtorRva': 0x54b60,
        'kSlot554B2BCanaryCell': 0x1593968,
        'kSlot554B2BOccluderLow': 0,
        'kSlot554B2BOccluderHigh': 2,
        'kSlot554B2BOccluderPageOffset': 0x18,
    },
    header_tokens=['Slot554B2BFunctor', 'slot554_batch2b_written',
                   'XPR Occluder Debug Mode', 'enableNonPower2DepthOccluders',
                   '0x6abb0', '0x2eaa3d0'],
    source_order=['init_functor', 'init_bool', 'slot554_batch2b_object_written',
                  'slot554_batch2b_written', 'default_slot554_batch2b_hooks',
                  'init_slot554_batch2b'],
    test_tokens=['kSentinel', 'slot554_batch2b_written', 'count_transfer', 'check_replay',
                 'XPR Occluder Debug Mode', 'kSlot554B2BFinalVptr'],
    cmake_tokens=['core/ctor_slot_554_2b_6fc29.cpp',
                  'add_test(NAME ctor_slot_554_batch2b COMMAND test_ctor_slot_554_batch2b)'],
    elf_extra=elf_extra,
    pad_byte=None,
    summary='0xd0 0x6abb0 occluder wrapper + seven bools; 1329/223 bytes; batch 2b prefix of PARTIAL slot 554',
    table_word=0x6efe0,
)

if __name__ == '__main__':
    raise SystemExit(main(SPEC))
