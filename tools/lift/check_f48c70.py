#!/usr/bin/env python3
"""ELF-tie HeapAllocator usable-size 0x48c70, allocate 0x48c80, page 0x488f0, reserve 0x48910.

Private: needs local/re/eboot.elf. Pins bootstrap page/cap immediates and native
heap_allocator.hpp / composite_allocator.hpp constants. Does not claim freelist surgery
(0x49030 / 0x48a70) is lifted.
"""
import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
USABLE = 0x48c70
USABLE_BODY = bytes.fromhex('488b46f84883e0f8c3')
USABLE_DIGEST = hashlib.sha256(USABLE_BODY).hexdigest()
ALLOC = 0x48c80
PAGE_SET = 0x488f0
PAGE_SET_BODY = bytes.fromhex('4881c6ffff0000c64708014881e60000ffff4889b7d0000000c3')
RESERVE = 0x48910
VT = 0x1595b70  # HeapAllocator vptr after bootstrap lea+0x10
BOOT_PAGE = 0x134f6  # movl $0x100000, %esi
BOOT_CAP = 0x1359d   # movl $0x9600000, %esi


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

got = img.read(USABLE, len(USABLE_BODY))
require(got == USABLE_BODY, f'0x48c70 body changed: {got.hex()}')
require(hashlib.sha256(got).hexdigest() == USABLE_DIGEST, '0x48c70 digest')

alloc = img.read(ALLOC, 0x30)
require(alloc.startswith(bytes.fromhex('554889e5')), '0x48c80 prologue')
# replenish path: movl $0x10000,%edx @ 0x48db4; call 0x479a0 @ 0x48db9
require(img.read(0x48db4, 5) == bytes.fromhex('ba00000100'), 'replenish align != 0x10000')
call_bytes = img.read(0x48db9, 5)
require(call_bytes[0] == 0xe8, 'missing call at 0x48db9')
require(0x48db9 + 5 + struct.unpack('<i', call_bytes[1:5])[0] == 0x479a0,
        'replenish call not 0x479a0')

page = img.read(PAGE_SET, len(PAGE_SET_BODY))
require(page == PAGE_SET_BODY, f'0x488f0 body changed: {page.hex()}')
# stores rounded page at +0xd0

res = img.read(RESERVE, 0x20)
require(res.startswith(bytes.fromhex('554889e5')), '0x48910 prologue')
require(bytes.fromhex('ba00000100') in res, '0x48910 align edx != 0x10000')
# call 0x479a0 from 0x4892a
cb = img.read(0x4892a, 5)
require(cb[0] == 0xe8 and 0x4892a + 5 + struct.unpack('<i', cb[1:5])[0] == 0x479a0,
        '0x48910 does not call 0x479a0')

def u64(rva):
    return struct.unpack('<Q', img.read(rva, 8))[0]

require(u64(VT + 0x30) == USABLE, 'vt+0x30 != usable-size 0x48c70')
require(u64(VT + 0xa0) == ALLOC, 'vt+0xa0 != allocate 0x48c80')
require(u64(VT + 0xa8) == 0x493d0, 'vt+0xa8 != deallocate 0x493d0')

boot_page = img.read(BOOT_PAGE, 5)
require(boot_page[0] == 0xbe and struct.unpack('<I', boot_page[1:5])[0] == 0x100000,
        'bootstrap page != 1 MiB')
boot_cap = img.read(BOOT_CAP, 5)
require(boot_cap[0] == 0xbe and struct.unpack('<I', boot_cap[1:5])[0] == 0x9600000,
        'bootstrap cap != 150 MiB')

hdr = (ROOT / 'native/core/heap_allocator.hpp').read_text()
comp = (ROOT / 'native/core/composite_allocator.hpp').read_text()
require(re.search(r'kHeapPageBytes\s*=\s*0x100000\b', comp), 'native page quantum')
require(re.search(r'kHeapAllocatorCapBytes\s*=\s*0x9600000\b', comp), 'native cap')
require(re.search(r'kHeapReserveAlign\s*=\s*0x10000\b', comp), 'native reserve align')
require('0x48c70' in hdr and '0x48c80' in hdr and '0x488f0' in hdr and '0x48910' in hdr,
        'RVA markers')
require('heap_usable_size' in hdr and 'heap_round_page_quantum' in hdr, 'helpers')
require('heap_request_bytes' in hdr, 'request helper')

print('OK: HeapAllocator usable 0x48c70 + alloc 0x48c80 + page-set 0x488f0 (+0xd0) + '
      'reserve 0x48910→0x479a0 (align 64 KiB); vt slots; bootstrap 1 MiB / 150 MiB; '
      'native constants match; freelist surgery not lifted')
