#!/usr/bin/env python3
"""ELF-tie allocator bootstrap 0x13380 wiring under IAllocator.

Private: needs local/re/eboot.elf. Pins early-out, composite→slot 1, child slot-scan
start index 1 / bound 0x100, Bucket/Heap/Page ctor + 0x47dd0 registration sites, and
native allocator_bootstrap.hpp constants. Does NOT claim 0x18aa0 page-table build,
0x42b80, composite vt side-effects, or 0x17070(0x65690) post-wire alloc.
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
BOOT = 0x13380
BOOT_END = 0x136ca  # exclusive of trailing nops; ret at 0x136c9
BOOT_DIGEST = '81eca8f425031e642ccd174b41e1fdd1871bafac34341fbb1e095fecfb55829b'
TBL_BASE = 0x2dfaab0
COMPOSITE_SLOT1 = TBL_BASE + 8  # 0x2dfaab8


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


def call_target(img, rva):
    b = img.read(rva, 5)
    require(b[0] == 0xe8, f'missing call at {rva:#x}')
    return rva + 5 + struct.unpack('<i', b[1:5])[0]


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

body = img.read(BOOT, BOOT_END - BOOT)
require(hashlib.sha256(body).hexdigest() == BOOT_DIGEST, '0x13380 body digest changed')
require(body.startswith(bytes.fromhex('554889e5415653488d3d')), '0x13380 prologue')

# Early-out: testq %rax,%rax ; jne toward epilogue (composite already live).
early = img.read(0x1339c, 6)
require(early[:3] == bytes.fromhex('4885c0') and early[3] == 0x0f and early[4] == 0x85,
        'missing early-out jne after composite ptr test')

# Composite forced into tbl[1]: movl $1,%esi @ 0x133db; movq %rbx, disp(%rip) → 0x2dfaab8.
require(img.read(0x133db, 5) == bytes.fromhex('be01000000'), 'composite slot imm != 1')
store = img.read(0x133e0, 7)
require(store[0] == 0x48 and store[1] == 0x89 and store[2] == 0x1d, 'missing movq %rbx,disp(%rip)')
disp = struct.unpack('<i', store[3:7])[0]
require(0x133e0 + 7 + disp == COMPOSITE_SLOT1, f'store not tbl[1] ({0x133e0+7+disp:#x})')

# Bucket / Heap slot scans start at index 1; Page uses %ecx = 1.
require(img.read(0x1347f, 5) == bytes.fromhex('b801000000'), 'bucket scan start != 1')
require(img.read(0x1354c, 5) == bytes.fromhex('b801000000'), 'heap scan start != 1')
require(img.read(0x135dd, 5) == bytes.fromhex('b901000000'), 'page scan start != 1')
# Scan bound cmpq $0x100
require(bytes.fromhex('4881f800010000') in body or bytes.fromhex('4881f900010000') in body or
        bytes.fromhex('483d00010000') in body, 'missing scan bound 0x100')

# Key ctor / register calls.
require(call_target(img, 0x1347a) == 0x17990, 'Bucket init call != 0x17990')
require(call_target(img, 0x13547) == 0x488f0, 'Heap page-set call != 0x488f0')
require(call_target(img, 0x135a5) == 0x48910, 'Heap reserve call != 0x48910')
require(call_target(img, 0x1364a) == 0x47dd0, 'Bucket register != 0x47dd0')
require(call_target(img, 0x13662) == 0x47dd0, 'Heap register != 0x47dd0')
require(call_target(img, 0x13678) == 0x47dd0, 'Page register != 0x47dd0')

# Threshold immediates at registration sites (also pinned by check_f47e30.py).
require(img.read(0x13645, 5) == bytes.fromhex('ba00040000'), 'bucket thr != 0x400')
require(img.read(0x1365d, 5) == bytes.fromhex('ba00001000'), 'heap thr != 0x100000')
require(img.read(0x1366e, 7) == bytes.fromhex('48c7c2ffffffff'), 'page thr != ~0')

hdr = (ROOT / 'native/core/allocator_bootstrap.hpp').read_text()
require(re.search(r'kBootstrapCompositeHeapSlot\s*=\s*1\b', hdr), 'native composite slot')
require(re.search(r'kBootstrapBucketHeapSlot\s*=\s*2\b', hdr), 'native bucket slot')
require(re.search(r'kBootstrapHeapHeapSlot\s*=\s*3\b', hdr), 'native heap slot')
require(re.search(r'kBootstrapPageHeapSlot\s*=\s*4\b', hdr), 'native page slot')
require(re.search(r'kBootstrapHeapSlotCount\s*=\s*0x100\b', hdr), 'native scan bound')
require('0x13380' in hdr and 'bootstrap_allocator_hierarchy' in hdr, 'RVA/API markers')
require('first_free_heap_slot' in hdr, 'slot scan helper')

print('OK: bootstrap 0x13380 body + early-out + composite→slot1 + child scans '
      '(start 1 / bound 0x100) + Bucket/Heap/Page calls + 0x47dd0 thresholds; '
      'native allocator_bootstrap.hpp constants match; page-table/vt side-effects not lifted')
