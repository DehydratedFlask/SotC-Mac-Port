#!/usr/bin/env python3
'''ELF-tie the 1,143 B native 0xa5cd90 SoundSystemSndx::Init body.

Private: needs local/re/eboot.elf. Pins the body hash and extent, all 44 direct call
sites (including the never-called stack-check trap), both indirect call sites, the two
mutually exclusive config strings, and the two callback/model table words the body reads.

The native implementation is native/core/init_a5cd90_body.cpp. Every unmodelled callee is
an explicit host seam; the 0x994e70 config argument is an identity tag (an original-image
string RVA), never a dereferenced host pointer. That distinction is what the 2026-11-12
segfault at 0x12c4f51 was: the body passed the RVA as `const char*` and the host called
strlen on it.
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
BODY = 0xA5CD90
BODY_SIZE = 0x477
BODY_SHA256 = '5d554780bc47358511278e0adad41d4d9bf2aca46902afc5976167dc41becf23'

CALLS = (
    (0xA5CDC0, 0x9854F0),   # callback registration 0xa5d210
    (0xA5CDD3, 0x994DD0),   # callback pair 0xa5c630 / 0xa5c440
    (0xA5CDF8, 0x994E70),   # config string (identity tag only)
    (0xA5CE41, 0x991C70),   # five-pointer callback/model block
    (0xA5CE5B, 0x12820),    # 2 MiB first arena
    (0xA5CE72, 0x992210),   # arena zero-fill / size class
    (0xA5CEA5, 0x14C2F0),   # resource key
    (0xA5CEBE, 0x14CCB0),   # addref
    (0xA5CEDD, 0xF8F3D0),   # once state
    (0xA5CEE6, 0xF730),     # thread
    (0xA5CEFA, 0x14C4A0),   # mutex
    (0xA5CF10, 0xF8F390),   # once key create
    (0xA5CF1C, 0xF8F3E0),   # once key end
    (0xA5CF30, 0x14CF20),   # named resource
    (0xA5CF4F, 0xF8F3D0),   # second once state
    (0xA5CF58, 0xF730),     # second thread
    (0xA5CF6C, 0x14C4A0),   # second mutex
    (0xA5CF82, 0xF8F390),   # second key create
    (0xA5CF8E, 0xF8F3E0),   # second key end
    (0xA5CFA6, 0x14CF20),   # second named resource
    (0xA5CFB9, 0x14D880),   # current field
    (0xA5CFD3, 0x14D2D0),   # destroy current
    (0xA5CFE6, 0x14CCB0),   # addref
    (0xA5CFEE, 0x14CCC0),   # release
    (0xA5D006, 0x14CCB0),   # addref
    (0xA5D00E, 0x95580),    # notify
    (0xA5D016, 0x14D2D0),   # destroy
    (0xA5D01E, 0x14CCC0),   # release
    (0xA5D02D, 0x14D2D0),   # destroy
    (0xA5D03A, 0x14CCC0),   # release
    (0xA5D04E, 0xF590),     # key release
    (0xA5D056, 0x95580),    # notify
    (0xA5D05F, 0x14D7B0),   # parameter query
    (0xA5D06E, 0x9892D0),   # status query
    (0xA5D08B, 0x946590),   # size query
    (0xA5D0A7, 0x12820),    # 0x100000 second arena
    (0xA5D0BA, 0x946650),   # region query
    (0xA5D0D2, 0x994070),   # descriptor mode words
    (0xA5D0DE, 0xF901E0),   # UserID query
    (0xA5D151, 0x3E7E0),    # error log (gated)
    (0xA5D160, 0x994640),   # success log
    (0xA5D16A, 0x17070),    # 0x70 task object
    (0xA5D1BE, 0x3B5C0),    # task init
    (0xA5D201, 0xF8F3A0),   # stack-check trap, pinned never called
)

INDIRECT = (0xA5CEB5, 0xA5D1D7)   # ff /2 sites: [0x2ee2cf8+0x18] resolver, [old]+0x8 release

CONSTANTS = (
    (0x15FEB80, 0xA5D5A0),   # model 0 word read by the callback block
    (0x15FED98, 0xA5D5F0),   # model 1 word read by the callback block
)

CONFIG_TEXT = ('maxResources=128 cpuEnableFlags={} maxControllerSpeakers=1 '
               'maxXfaderShortcuts=300')

STRINGS = (
    (0x12C4EFA, CONFIG_TEXT.format('0000001'), 'config, flag set'),
    (0x12C4F51, CONFIG_TEXT.format('0000011'), 'config, flag zero'),
    (0x12C4EC7, 'void SoundSystemSndx::Init(const SndxInitParams &)', 'function name'),
    (0x12C4E79, 'd:\\dev\\cp10\\Main\\Code\\BPE\\Source\\Engine\\Sound\\SoundSystemSndx.cpp',
     'source path'),
    (0x12C4FA8, 'Failed to obtain UserID from the UserService\n', 'error message'),
    (0x12C4FD6, 'SoundSystemSndx', 'task name'),
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
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, '0xa5cd90 body SHA256')
    require(body[0] == 0x55, '0xa5cd90 prologue')
    # The body ends with `call 0xf8f3a0; nop`.
    require(body[-6] == 0xE8 and body[-1] == 0x90, '0xa5cd90 cold tail')

    require(len(CALLS) == 44, 'call site count')
    for site, target in CALLS:
        require(call_target(site, img) == target,
                'call ' + hex(site) + ' -> ' + hex(target))
    for site in INDIRECT:
        require(body[site - BODY + 1] & 0x38 == 0x10,
                'indirect call at ' + hex(site))

    for va, expected, label in STRINGS:
        require(read_cstr(img, va) == expected, label + ' at ' + hex(va))
    require(len(read_cstr(img, 0x12C4EFA)) == 0x56, 'config set length')
    require(len(read_cstr(img, 0x12C4F51)) == 0x56, 'config zero length')

    for va, expected in CONSTANTS:
        actual = struct.unpack('<Q', img.read(va, 8))[0]
        require(actual == expected, 'constant ' + hex(va) + ' expected ' +
                hex(expected) + ' got ' + hex(actual))

    print('OK check_a5cd90_body: sha=' + BODY_SHA256[:12] + '... size=' +
          str(BODY_SIZE) + ' calls=' + str(len(CALLS)) + ' indirect=' +
          str(len(INDIRECT)) + ' trap=0xf8f3a0')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
