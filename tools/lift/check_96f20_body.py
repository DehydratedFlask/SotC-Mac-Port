#!/usr/bin/env python3
'''ELF-tie the 41 B native 0x96f20 wrapper.

Private: needs local/re/eboot.elf. Pins the wrapper body hash, its exact 0x97a50 call
site, the two fixed register arguments, and the unmodelled 2,835 B callee hash. The native
implementation is native/core/init_96f20_body.cpp; 0x97a50 remains an explicit host
allocation contract and is not claimed as a lifted body.
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
BODY = 0x96F20
BODY_SIZE = 41
BODY_SHA256 = '384d9d1f8dbb887de09c7f88fd338824d54c51fab56e9309447dc43a7f51eb8d'
CALLEE = 0x97A50
CALLEE_SIZE = 2835
CALLEE_SHA256 = '3e202117845a744be714d35680ef38ae27c4c06057ed3bef04f5b6e7ff9a34f8'
CALL_SITE = 0x96F3E


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit('FAIL ' + msg)


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, 'missing call at ' + hex(site))
    rel = struct.unpack('<i', b[1:5])[0]
    return site + 5 + rel


def main() -> int:
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(BODY, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, '0x96f20 body SHA256')
    require(body[0] == 0x55 and body[-1] == 0xC3, '0x96f20 prologue/ret')
    require(call_target(CALL_SITE, img) == CALLEE, '0x96f20 call site -> 0x97a50')
    require(b'\xba\x01\x00\x00\x00' in body, 'edx=1 immediate')
    require(b'\xb9\x01\x00\x00\x00' in body, 'ecx=1 immediate')

    callee = img.read(CALLEE, CALLEE_SIZE)
    require(hashlib.sha256(callee).hexdigest() == CALLEE_SHA256, '0x97a50 callee SHA256')
    require(callee[-1] == 0x90, '0x97a50 nop end')
    print('OK check_96f20_body: sha=' + BODY_SHA256[:12] + '... size=' + str(BODY_SIZE) +
          ' call=' + hex(CALL_SITE) + '->' + hex(CALLEE) +
          ' callee_sha=' + CALLEE_SHA256[:12] + '... host_contract')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
