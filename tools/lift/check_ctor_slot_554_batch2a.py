#!/usr/bin/env python3
"""ELF byte-oracle for slot 554 batch 2a (body 0x6f793..0x6fc29, five bool variables).

The batch is the clean 0xc0-byte bool prefix of object range 0x2eaa010..0x2eaa3d0.  Slot 554 remains
PARTIAL; this checker covers batch 2a only and never loads or executes an original RVA.
"""
from __future__ import annotations
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main  # noqa: E402
from slot_pin_util import ROOT  # noqa: E402,F401

PINS = [
    ('call', 0x6f7f5, 0x20af0, 'call_6f7f5'),
    ('call', 0x6f83f, 0xf590, 'call_6f83f'),
    ('call', 0x6f856, 0x20d60, 'call_6f856'),
    ('call', 0x6f860, 0xf590, 'call_6f860'),
    ('call', 0x6f876, 0xf8f390, 'call_6f876'),
    ('rip', 0x6f793, 7, 3, 0x2eaa010, 'obj_6f793'),
    ('call', 0x6f8dd, 0x20af0, 'call_6f8dd'),
    ('call', 0x6f92e, 0xf590, 'call_6f92e'),
    ('call', 0x6f946, 0x20d60, 'call_6f946'),
    ('call', 0x6f950, 0xf590, 'call_6f950'),
    ('call', 0x6f966, 0xf8f390, 'call_6f966'),
    ('rip', 0x6f87b, 7, 3, 0x2eaa0d0, 'obj_6f87b'),
    ('call', 0x6f9cd, 0x20af0, 'call_6f9cd'),
    ('call', 0x6fa14, 0xf590, 'call_6fa14'),
    ('call', 0x6fa26, 0x20d60, 'call_6fa26'),
    ('call', 0x6fa30, 0xf590, 'call_6fa30'),
    ('call', 0x6fa46, 0xf8f390, 'call_6fa46'),
    ('rip', 0x6f96b, 7, 3, 0x2eaa190, 'obj_6f96b'),
    ('call', 0x6faad, 0x20af0, 'call_6faad'),
    ('call', 0x6fafc, 0xf590, 'call_6fafc'),
    ('call', 0x6fb13, 0x20d60, 'call_6fb13'),
    ('call', 0x6fb1d, 0xf590, 'call_6fb1d'),
    ('call', 0x6fb33, 0xf8f390, 'call_6fb33'),
    ('rip', 0x6fa4b, 7, 3, 0x2eaa250, 'obj_6fa4b'),
    ('call', 0x6fb9d, 0x20af0, 'call_6fb9d'),
    ('call', 0x6fbec, 0xf590, 'call_6fbec'),
    ('call', 0x6fc03, 0x20d60, 'call_6fc03'),
    ('call', 0x6fc0b, 0xf590, 'call_6fc0b'),
    ('call', 0x6fc24, 0xf8f390, 'call_6fc24'),
    ('rip', 0x6fb38, 7, 3, 0x2eaa310, 'obj_6fb38'),
]

def elf_extra(e) -> None:
    e.cstring(0xf9a865, b'threadedCulling')
    e.cstring(0xf9a875, b'threadedRendering')
    e.cstring(0xf9a887, b'threadedSumbit')
    e.cstring(0xf9a896, b'enableAsyncCompute')
    e.cstring(0xf9a8a9, b'enableGPUCounters')
    e.cstring(0xf9a8d3, b'')

SPEC = Spec(
    name='slot 554 batch 2a', index=554, base=0x6f793, size=0x496,
    sha='adfcc16736d8440b779af9cf4b80b44e1acbf380aba41184fdd0d5faab94ec6c',
    pins=PINS,
    native_files={
        'header': 'native/core/ctor_slot_554_2a_6f793.hpp',
        'source': 'native/core/ctor_slot_554_2a_6f793.cpp',
        'test': 'native/tests/test_ctor_slot_554_batch2a.cpp',
        'cmake': 'native/CMakeLists.txt',
    },
    callees={
        'base ctor 0x20af0': (0x20af0, 0x5a, 'e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45'),
        'register 0x20d60': (0x20d60, 0x1a, 'f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403'),
        'release 0xf590': (0xf590, 0x49, '55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47'),
        'atexit 0xf8f390': (0xf8f390, 0x1b, '4314f548f230d5ca2b9d5193e0a00a493cb8e13d59335d852b80a1a514bbbbab'),
    },
    header_constants={
        'kSlot554B2ARva': 0x6f793,
        'kSlot554B2ABodySize': 0x496,
        'kSlot554B2AObjectBaseRva': 0x2eaa010,
        'kSlot554B2AObjectCount': 5,
        'kSlot554B2AObjectSize': 0xc0,
        'kSlot554B2ABlockSize': 0x3c0,
        'kSlot554B2AWritten': 810,
        'kSlot554B2AHoles': 150,
        'kSlot554B2ATableWordRva': 0x2dd5f90,
        'kSlot554B2AFinalVptr': 0x1594348,
    },
    header_tokens=['Slot554B2ABool', 'slot554_batch2a_written', 'threadedCulling',
                   'enableGPUCounters', '0x6f793', '0x2eaa010'],
    source_order=['slot554_batch2a_object_written', 'slot554_batch2a_written',
                  'default_slot554_batch2a_hooks', 'init_slot554_batch2a', 'out.atexit_records'],
    test_tokens=['kSentinel', 'slot554_batch2a_written', 'refuse_register', 'check_replay',
                 'default_slot554_batch2a_hooks', 'threadedCulling'],
    cmake_tokens=['core/ctor_slot_554_2a_6f793.cpp',
                  'add_test(NAME ctor_slot_554_batch2a COMMAND test_ctor_slot_554_batch2a)'],
    elf_extra=elf_extra,
    pad_byte=None,
    summary='five bools 0x3c0 (810/150) at 0x2eaa010; batch 2a of PARTIAL slot 554',
    table_word=0x6efe0,
)

if __name__ == '__main__':
    raise SystemExit(main(SPEC))
