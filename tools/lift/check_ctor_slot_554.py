#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 554, batch 1 (RVA 0x6efe0).

Batch 1 is the page header 0x2ea8ee8..0x2ea99b0 (0xac8 B, built by 0x53fc0) plus the first eight
variables at 0x2ea99b0..0x2eaa010.  The whole slot is 112,184 B and remains PARTIAL.  Offline only:
reads the pinned private ELF, never loads or executes an original RVA.
"""
from __future__ import annotations
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402
from slot_pin_util import ROOT  # noqa: E402

PINS = [
    ('rip', 454655, 7, 3, 48926440, 'obj_6efff'),
    ('call', 454676, 344000, 'call_6f014'),
    ('rip', 454681, 7, 3, 48929200, 'obj_6f019'),
    ('call', 454842, 240000, 'call_6f0ba'),
    ('call', 454850, 62864, 'call_6f0c2'),
    ('call', 454898, 134496, 'call_6f0f2'),
    ('call', 454906, 62864, 'call_6f0fa'),
    ('call', 454928, 16315280, 'call_6f110'),
    ('rip', 454933, 7, 3, 48929408, 'obj_6f115'),
    ('call', 455086, 240000, 'call_6f1ae'),
    ('call', 455094, 62864, 'call_6f1b6'),
    ('call', 455134, 134496, 'call_6f1de'),
    ('call', 455142, 62864, 'call_6f1e6'),
    ('call', 455164, 16315280, 'call_6f1fc'),
    ('rip', 455169, 7, 3, 48929616, 'obj_6f201'),
    ('call', 455322, 436736, 'call_6f29a'),
    ('call', 455330, 62864, 'call_6f2a2'),
    ('call', 455381, 134496, 'call_6f2d5'),
    ('call', 455389, 62864, 'call_6f2dd'),
    ('call', 455411, 16315280, 'call_6f2f3'),
    ('rip', 455416, 7, 3, 48929824, 'obj_6f2f8'),
    ('call', 455569, 436736, 'call_6f391'),
    ('call', 455577, 62864, 'call_6f399'),
    ('call', 455614, 134496, 'call_6f3be'),
    ('call', 455622, 62864, 'call_6f3c6'),
    ('call', 455644, 16315280, 'call_6f3dc'),
    ('rip', 455649, 7, 3, 48930032, 'obj_6f3e1'),
    ('call', 455802, 436736, 'call_6f47a'),
    ('call', 455810, 62864, 'call_6f482'),
    ('call', 455852, 134496, 'call_6f4ac'),
    ('call', 455860, 62864, 'call_6f4b4'),
    ('call', 455882, 16315280, 'call_6f4ca'),
    ('rip', 455887, 7, 3, 48930240, 'obj_6f4cf'),
    ('call', 456040, 436736, 'call_6f568'),
    ('call', 456048, 62864, 'call_6f570'),
    ('call', 456085, 134496, 'call_6f595'),
    ('call', 456093, 62864, 'call_6f59d'),
    ('call', 456115, 16315280, 'call_6f5b3'),
    ('rip', 456120, 7, 3, 48930448, 'obj_6f5b8'),
    ('call', 456218, 133872, 'call_6f61a'),
    ('call', 456300, 62864, 'call_6f66c'),
    ('call', 456334, 134496, 'call_6f68e'),
    ('call', 456344, 62864, 'call_6f698'),
    ('call', 456366, 16315280, 'call_6f6ae'),
    ('rip', 456371, 7, 3, 48930640, 'obj_6f6b3'),
    ('call', 456469, 133872, 'call_6f715'),
    ('call', 456540, 62864, 'call_6f75c'),
    ('call', 456558, 134496, 'call_6f76e'),
    ('call', 456568, 62864, 'call_6f778'),
    ('call', 456590, 16315280, 'call_6f78e'),
]

def elf_extra(e) -> None:
    e.cstring(0xf9a7e4, b'backgroundColor')
    e.cstring(0xf9a7f4, b'gpuMarkerLevel')
    e.cstring(0xf9a803, b'globalRenderVarX')
    e.cstring(0xf9a814, b'globalRenderVarY')
    e.cstring(0xf9a825, b'globalRenderVarZ')
    e.cstring(0xf9a836, b'globalRenderVarW')
    e.cstring(0xf9a847, b'profileEnableGpu')
    e.cstring(0xf9a858, b'groupCulling')
    e.cstring(0xf9a8d3, b'')

SPEC = Spec(
    name='slot 554 batch 1', index=554, base=0x6efe0, size=0x7b3,
    sha='483de6397f25b7f9f5038b4a00e61191e3399acdcec76d1ff98700dd804a3e47',
    pins=PINS,
    native_files={
        'header': 'native/core/ctor_slot_554_6efe0.hpp',
        'source': 'native/core/ctor_slot_554_6efe0.cpp',
        'test': 'native/tests/test_ctor_slot_554.cpp',
        'cmake': 'native/CMakeLists.txt',
    },
    callees={
        'page init 0x53fc0': (0x53fc0, 0xb9e, 'f05331226f093fbfb54ddd7ba3066ea490f001793ffa049217fb0f8c2d379ad4'),
        'int ctor 0x3a980': (0x3a980, 0x142, '2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca'),
        'float ctor 0x6aa00': (0x6aa00, 0x142, 'ee90dea5d0e2ed59ce83ff52c5a3732419635622c7b422f3eff54fd1f5384ebb'),
        'base ctor 0x20af0': (0x20af0, 0x5a, 'e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45'),
        'register 0x20d60': (0x20d60, 0x1a, 'f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403'),
    },
    header_constants={
        'kSlot554Rva': 0x6efe0,
        'kSlot554Batch1BodyBytes': 0x7b3,
        'kSlot554PageSize': 0xac8,
        'kSlot554VariableCount': 8,
        'kSlot554PageStoreCount': 187,
        'kSlot554PageWritten': 2113,
        'kSlot554PageHoles': 647,
        'kSlot554BlockWritten': 3427,
        'kSlot554BlockHoles': 965,
        'kSlot554IntFinalVptr': 0x1594438,
        'kSlot554FloatFinalVptr': 0x15943c0,
        'kSlot554BoolFinalVptr': 0x1594348,
        'kSlot554TableWordRva': 0x2dd5f90,
    },
    header_tokens=['Slot554Objects', 'slot554_written', 'backgroundColor', 'groupCulling',
                   '0x6efe0', '0x2ea8ee8'],
    source_order=['slot554_init_page', 'slot554_page_written', 'slot554_object_written',
                  'default_slot554_hooks', 'hooks.atexit', 'init_slot554_batch1'],
    test_tokens=['kSentinel', 'slot554_written', 'refuse_register', 'check_replay',
                 'default_slot554_hooks'],
    cmake_tokens=['core/ctor_slot_554_6efe0.cpp',
                  'add_test(NAME ctor_slot_554 COMMAND test_ctor_slot_554)'],
    elf_extra=elf_extra,
    pad_byte=None,
    summary='page 0xac8 (187 stores, 2113/647) + 8 vars (0x660, 1314/318); PARTIAL slot',
)

if __name__ == '__main__':
    raise SystemExit(main(SPEC))
