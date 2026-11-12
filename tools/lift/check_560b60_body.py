#!/usr/bin/env python3
'''ELF-tie the 803 B native 0x560b60 collision/descriptor initializer.

Private: needs local/re/eboot.elf. Pins the body hash, all 20 direct call sites (including
the never-called stack-check trap), the two entry strings, the three stack callback RVAs,
and the three 0x20-byte descriptor constants. The native implementation is
native/core/init_560b60_body.cpp; the unmodelled callees are named host seams.
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
BODY = 0x560B60
BODY_SIZE = 0x323
BODY_SHA256 = '9f67f04fe33b637bf34ad25a4912f245a5f6904c484f55370833bb1d5679fa56'

CALLS = (
    (0x560B90, 0x11BC0),  # descriptor lookup
    (0x560BB1, 0x17070),  # object allocation, 0x136f0
    (0x560C21, 0x17090),  # 0x2000 buffer
    (0x560C46, 0x17090),  # 0x400 buffer
    (0x560C6B, 0x17090),  # 0x200 buffer
    (0x560CC4, 0x170C0),  # previous +0x136c8
    (0x560CD5, 0x170C0),  # previous +0x10080
    (0x560CE6, 0x170C0),  # previous +0x10060
    (0x560CF7, 0x170C0),  # previous +0x10040
    (0x560CFF, 0x170B0),  # previous object
    (0x560D76, 0x3CA330),  # three stack descriptors
    (0x560D90, 0x273890),  # callback registration
    (0x560DFB, 0x14C0F0),  # format/output
    (0x560E1B, 0xF590),    # release 0x2f08930
    (0x560E39, 0xF590),    # release -0x48 block
    (0x560E41, 0xF590),    # release -0xd0 input
    (0x560E49, 0x56B540),  # probe
    (0x560E59, 0x95580),   # conditional follow-up
    (0x560E5E, 0x56EFB0),  # final initializer
    (0x560E7D, 0xF8F3A0),  # stack-check trap, pinned never called
)

CONSTANTS = (
    (0x15FFC58, 0x560580),
    (0x15FFC60, 0x5605F0),
    (0x15FF4A0, 0x560600),
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit('FAIL ' + msg)


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, 'missing call at ' + hex(site))
    rel = struct.unpack('<i', b[1:5])[0]
    return site + 5 + rel


def read_cstr(img: d.Image, va: int, limit: int = 128) -> str:
    out = bytearray()
    for i in range(limit):
        b = img.read(va + i, 1)
        if b == b'\x00':
            return out.decode('ascii')
        out += b
    raise SystemExit('FAIL unterminated string at ' + hex(va))


def main() -> int:
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(BODY, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, '0x560b60 body SHA256')
    require(body[0] == 0x55 and body[-1] == 0x90, '0x560b60 prologue/nop end')
    require(len(CALLS) == 20, 'call site count')
    for site, target in CALLS:
        require(call_target(site, img) == target,
                'call ' + hex(site) + ' -> ' + hex(target))

    require(read_cstr(img, 0x101CE8A) == 'Collision', 'Collision descriptor string')
    require(read_cstr(img, 0x101CE94) ==
            '$/models/shadow/collision/****/collision_map.crmp',
            'collision_map format string')
    require(len(read_cstr(img, 0x101CE94)) == 0x31, 'format string length')
    for va, expected in CONSTANTS:
        actual = struct.unpack('<Q', img.read(va, 8))[0]
        require(actual == expected, 'constant ' + hex(va) + ' expected ' +
                hex(expected) + ' got ' + hex(actual))

    print('OK check_560b60_body: sha=' + BODY_SHA256[:12] + '... size=' +
          str(BODY_SIZE) + ' calls=' + str(len(CALLS)) + ' constants=3 trap=0xf8f3a0')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
