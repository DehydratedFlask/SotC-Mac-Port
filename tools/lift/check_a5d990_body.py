#!/usr/bin/env python3
'''ELF-tie the 2,709 B native 0xa5d990 SoundSystemSndx category/menu body.

Private: needs local/re/eboot.elf. Pins the body hash and extent, all 64 direct call sites
(the body has no indirect call), the cold stack-check trap, the dead "Unnamed Categories"
gate byte, and every string the static menu cluster uses.

The native composition is native/core/init_a5d990_body.cpp: region 1 reuses the already
native engine_category_vars loop, region 2 is the dead gate block and is never executed,
region 3 is the two separators + four console command items built by the existing
fmenu_ctors_slot306 models.
'''

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BODY = 0xA5D990
BODY_SIZE = 0xA95   # 2709
BODY_SHA256 = '2e6a87f41290146d2c9aae776af1e30c49f4c8f63af9096da33f58ca1ca4488f'

CALLS = (
    (0xA5DA3A, 0x10A90),   # "sndxCategory_Mute_%d"
    (0xA5DAAC, 0x20AF0),   # bool-var construct (mute)
    (0xA5DB03, 0xF590),    # name release
    (0xA5DB24, 0x20D60),   # register (mute)
    (0xA5DB4E, 0xA62BC0),  # vector grow (mute)
    (0xA5DB59, 0xF590),
    (0xA5DB72, 0x10A90),   # "sndxCategory_Filter_%d"
    (0xA5DBE0, 0x20AF0),
    (0xA5DC32, 0xF590),
    (0xA5DC4E, 0x20D60),   # register (filter)
    (0xA5DC7E, 0xA62BC0),  # vector grow (filter)
    (0xA5DC89, 0xF590),
    (0xA5DCC9, 0x10A90),   # "%d - %s" toggle caption
    (0xA5DD2E, 0x26EA0),   # toggle item (mute)
    (0xA5DD36, 0xF590),
    (0xA5DD91, 0x26EA0),   # toggle item (filter)
    (0xA5DD99, 0xF590),
    (0xA5DDA8, 0x272D0),   # toggle item destroy (filter)
    (0xA5DDB4, 0x272D0),   # toggle item destroy (mute)
    (0xA5DDC0, 0xF590),
    (0xA5DE52, 0x26C10),   # dead block: "Unnamed Categories" group
    (0xA5DE5A, 0xF590),
    (0xA5DE62, 0xF590),
    (0xA5DECD, 0x26C10),   # dead block: second group
    (0xA5DED5, 0xF590),
    (0xA5DEDD, 0xF590),
    (0xA5DF17, 0x10A90),   # dead block: "Category %d"
    (0xA5DF44, 0x10A90),   # dead block: unnamed caption
    (0xA5DFA5, 0x26EA0),   # dead block: toggle
    (0xA5DFB0, 0xF590),
    (0xA5E008, 0x26EA0),   # dead block: toggle
    (0xA5E010, 0xF590),
    (0xA5E018, 0x272D0),
    (0xA5E024, 0x272D0),
    (0xA5E02C, 0xF590),
    (0xA5E04C, 0x272D0),
    (0xA5E058, 0x272D0),
    (0xA5E09D, 0x26B60),   # separator, parent "Category Mute"
    (0xA5E0A5, 0xF590),
    (0xA5E143, 0x26F90),   # "Mute All"
    (0xA5E14B, 0xF590),
    (0xA5E153, 0xF590),
    (0xA5E15B, 0xF590),
    (0xA5E1EE, 0x26F90),   # "Unmute All"
    (0xA5E1F6, 0xF590),
    (0xA5E1FE, 0xF590),
    (0xA5E206, 0xF590),
    (0xA5E248, 0x26B60),   # separator, parent "Category Filters"
    (0xA5E250, 0xF590),
    (0xA5E2EA, 0x26F90),   # "Filter All"
    (0xA5E2F2, 0xF590),
    (0xA5E2FA, 0xF590),
    (0xA5E302, 0xF590),
    (0xA5E39C, 0x26F90),   # "Unfilter All"
    (0xA5E3A4, 0xF590),
    (0xA5E3AC, 0xF590),
    (0xA5E3B4, 0xF590),
    (0xA5E3BC, 0x272D0),   # six direct item destroys
    (0xA5E3C8, 0x272D0),
    (0xA5E3D4, 0x272D0),
    (0xA5E3E0, 0x272D0),
    (0xA5E3EC, 0x272D0),
    (0xA5E3F8, 0x272D0),
    (0xA5E41F, 0xF8F3A0),  # cold stack-check trap, pinned never called
)

# movb %al,-0x731(%rbp) is the gate byte the dead block tests; the only writer is the loop
# head, where eax is zero.
GATE_WRITE = bytes.fromhex('8885cff8ffff')
GATE_TEST = bytes.fromhex('a801')          # testb $0x1,%al

STRINGS = (
    (0x12C4FE6, 'sndxCategory_Mute_%d', 0x14),
    (0x12C4FFB, 'sndxCategory_Filter_%d', 0x16),
    (0x12C5012, '%d - %s', 0x7),
    (0x12C4D2F, 'Category Mute', 0xD),
    (0x12C4DDD, 'Category Filters', 0x10),
    (0x12C501A, 'Unnamed Categories', 0x12),
    (0x12C5041, 'Category %d', 0xB),
    (0x12C504D, 'Mute All', 0x8),
    (0x12C5056, 'sndx_categoryMuteAll=true', 0x19),
    (0x12C5070, 'Unmute All', 0xA),
    (0x12C507B, 'sndx_categoryMuteAll=false', 0x1A),
    (0x12C5096, 'Filter All', 0xA),
    (0x12C50A1, 'sndx_categoryFilterAll=true', 0x1B),
    (0x12C50BD, 'Unfilter All', 0xC),
    (0x12C50CA, 'sndx_categoryFilterAll=false', 0x1C),
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit('FAIL ' + msg)


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, 'missing call at ' + hex(site))
    rel = struct.unpack('<i', b[1:5])[0]
    return site + 5 + rel


def read_cstr(img: d.Image, va: int, limit: int = 256) -> str:
    out = bytearray()
    for i in range(limit):
        b = img.read(va + i, 1)
        if b == b'\x00':
            return out.decode('latin-1')
        out += b
    raise SystemExit('FAIL unterminated string at ' + hex(va))


def main() -> int:
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(BODY, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, '0xa5d990 body SHA256')
    require(body[0] == 0x55, '0xa5d990 prologue')
    require(body[-6] == 0xE8 and body[-1] == 0x90, '0xa5d990 cold tail')

    require(len(CALLS) == 64, 'call site count')
    for site, target in CALLS:
        require(call_target(site, img) == target,
                'call ' + hex(site) + ' -> ' + hex(target))
    # The body has no indirect call: the disassembly lists 64 `callq` sites and no `callq *`.
    # A raw FF/2 byte scan is not instruction-aligned (it hits immediate bytes), so the call
    # surface is bound by the 64 pinned sites above rather than by an opcode sweep.

    require(body.count(GATE_WRITE) == 1, 'single dead-gate writer')
    require(body.count(GATE_TEST) == 1, 'single dead-gate test')
    gate = body.index(GATE_TEST)
    writer = body.index(GATE_WRITE)
    require(writer < gate, 'gate writer precedes the test')

    for va, expected, length in STRINGS:
        require(read_cstr(img, va) == expected, 'string at ' + hex(va))
        require(len(expected) == length, 'pinned length at ' + hex(va))

    print('OK check_a5d990_body: sha=' + BODY_SHA256[:12] + '... size=' +
          str(BODY_SIZE) + ' calls=' + str(len(CALLS)) + ' dead_gate=1 trap=0xf8f3a0')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
