#!/usr/bin/env python3
'''ELF-tie the 656 B 0x7f6f90 staffroll text loader.

Private: needs local/re/eboot.elf. Pins the body hash, all 19 direct call sites, the
path/mode/fallback formats, the three staffroll basenames, and the 37-entry token table.
The native implementation is native/core/init_7f6f90_body.cpp; E1430 claims this body.
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
BODY = 0x7F6F90
BODY_SIZE = 656
BODY_SHA256 = '6b68dce8a6d7e449e5228659a617755f99f78e39813dd0e5ef26f68d4f66a2bd'
TOKEN_TABLE = 0x15CE750

TOKENS = (
    'CENTER',
    'CENTER_BR',
    'RIGHT',
    'RIGHT_BR',
    'LEFT',
    'LEFT_BR',
    'BR',
    'SPACE',
    'SPACE_SCREEN',
    'SPACE_HALFSCREEN',
    'LAYOUTAREA',
    'FADEAREA',
    'LAYOUTBOX',
    'LINEHEIGHT',
    'FONTCOLOR',
    'ROLL_TIME',
    'FADE_TIME',
    'WAIT_BEFORE_FADE',
    'SHADOWFONT_OFFSET',
    'CENTER_FULL_SCREEN',
    'MAIN_HEADING',
    'SUB_HEADING',
    'SUB_HEADING_2',
    'COLUMN_HEADING_1',
    'COLUMN_HEADING_2',
    'COLUMN_HEADING_3',
    'COLUMN_HEADING_4',
    'COLUMN_1',
    'COLUMN_2',
    'COLUMN_3',
    'COLUMN_4',
    'COLUMN_END',
    'COPYRIGHT',
    'SPECIAL_THANKS',
    'BP_COMMAND_FORMAT',
    'STYLE',
    'TARGET',
)

CALLS = (
    (0x7F6FCE, 0x57CB70),  # vsnprintf wrapper, absolute path
    (0x7F6FDD, 0xF902A0),  # fopen
    (0x7F6FF4, 0xF90390),  # fseek end
    (0x7F6FFC, 0xF903A0),  # ftell
    (0x7F700B, 0xF90390),  # fseek set
    (0x7F7015, 0xF90210),  # malloc
    (0x7F702B, 0xF903D0),  # fread
    (0x7F7039, 0xF90270),  # fclose
    (0x7F7056, 0x57CB70),  # vsnprintf wrapper, fallback path
    (0x7F7063, 0x56CDB0),  # fallback load
    (0x7F706E, 0x56CEB0),  # fallback size
    (0x7F707B, 0xF90210),  # malloc
    (0x7F708C, 0x56CEC0),  # fallback copy
    (0x7F709A, 0x56CE60),  # fallback close
    (0x7F7123, 0xF90220),  # ctype lookup, mask 0x144
    (0x7F7140, 0xF8F3B0),  # strlen remainder
    (0x7F7168, 0xF902C0),  # strchr comma
    (0x7F7186, 0xF8F500),  # strcasecmp token
    (0x7F721A, 0xF8F3A0),  # stack-check trap, pinned never called
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
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, '0x7f6f90 body SHA256')
    require(body[0] == 0x55, '0x7f6f90 push rbp')
    require(len(CALLS) == 19, 'call site count')
    for site, target in CALLS:
        require(call_target(site, img) == target,
                'call ' + hex(site) + ' -> ' + hex(target))

    require(read_cstr(img, 0x128B85F) == '/app0/CP3Shadow/%s', 'absolute path format')
    require(read_cstr(img, 0x128B872) == 'rb', 'open mode')
    require(read_cstr(img, 0x128B875) == '%s', 'fallback format')
    require(read_cstr(img, 0x128B878) == 'cp10_shadow_staffroll.txt', 'shadow basename')
    require(read_cstr(img, 0x128B892) == 'staffroll_original_us.txt', 'original basename')
    require(read_cstr(img, 0x128B8AC) == 'staffroll_remaster_us.txt', 'remaster basename')

    table = img.read(TOKEN_TABLE, 8 * len(TOKENS))
    for i, expected in enumerate(TOKENS):
        ptr = struct.unpack_from('<Q', table, i * 8)[0]
        actual = read_cstr(img, ptr)
        require(actual == expected, 'token ' + str(i) + ' expected ' + expected + ' got ' + actual)

    print('OK check_7f6f90_body: sha=' + BODY_SHA256[:12] + '... size=' +
          str(BODY_SIZE) + ' calls=' + str(len(CALLS)) + ' tokens=' + str(len(TOKENS)) +
          ' trap=0xf8f3a0 pinned not called')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
