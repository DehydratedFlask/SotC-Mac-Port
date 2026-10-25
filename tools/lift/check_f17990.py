#!/usr/bin/env python3
"""ELF-tie BucketAllocator init 0x17990, allocate gate 0x179c0, pool setup 0x18750.

Private: needs local/re/eboot.elf. Pins bootstrap size-class geometry and that native
bucket_allocator.hpp / composite_allocator.hpp constants match. Does not claim the freelist
surgery body is lifted.
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
INIT = 0x17990
INIT_BODY = bytes.fromhex('c64708014889b7a80000004889978000000048898f88000000c3')
INIT_DIGEST = '2b140ea84dd891990a03ebe0d3195d14d127deb0ac29cbced791b39f76fc2b41'
ALLOC = 0x179c0
POOL = 0x18750
BOOT_LOOP = 0x13440


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

got = img.read(INIT, len(INIT_BODY))
require(got == INIT_BODY, f'0x17990 body changed: {got.hex()}')
require(hashlib.sha256(got).hexdigest() == INIT_DIGEST, '0x17990 digest changed')
# stores: +0x8=1, rsi→+0xa8 (page), rdx→+0x80, rcx→+0x88 (max class)

alloc = img.read(ALLOC, 0x50)
require(alloc.startswith(bytes.fromhex('554889e5')), '0x179c0 prologue')
require(bytes.fromhex('488b9a88000000') in alloc, '0x179c0 missing load max from +0x88')
require(bytes.fromhex('4839f3') in alloc, '0x179c0 missing cmp size vs max')

pool = img.read(POOL, 0x30)
require(pool.startswith(bytes.fromhex('554889e5')), '0x18750 prologue')
require(bytes.fromhex('4883c3c8') in pool, '0x18750 missing add page,-0x38 header')
require(bytes.fromhex('49f7f6') in pool, '0x18750 missing div by class size')

boot = img.read(BOOT_LOOP, 0x28)
require(boot[:5] == bytes.fromhex('be00000100'), 'bootstrap page size != 0x10000')
require(bytes.fromhex('4881fb10040000') in boot, 'bootstrap class loop end != 0x410')
# after loop: movl $0x10,%edx; movl $0x400,%ecx into 0x17990
ctor = img.read(0x13468, 16)
require(struct.unpack('<I', ctor[1:5])[0] == 0x10000, 'ctor page')
require(struct.unpack('<I', ctor[6:10])[0] == 0x10, 'ctor step')
require(struct.unpack('<I', ctor[11:15])[0] == 0x400, 'ctor max')

bucket = (ROOT / 'native/core/bucket_allocator.hpp').read_text()
comp = (ROOT / 'native/core/composite_allocator.hpp').read_text()
require(re.search(r'kBucketClassCount\s*=\s*64\b', bucket), 'native class count')
require(re.search(r'kBucketPageHeader\s*=\s*0x38\b', bucket), 'native page header')
require(re.search(r'kBucketClassStep\s*=\s*0x10\b', comp), 'step')
require(re.search(r'kBucketClassMin\s*=\s*0x10\b', comp), 'min class')
require(re.search(r'kBucketPageBytes\s*=\s*0x10000\b', comp), 'page bytes')
require(re.search(r'kCompositeBucketThreshold\s*=\s*0x400\b', comp), 'max class')
require('0x17990' in bucket and '0x18750' in bucket and '0x179c0' in bucket, 'RVA markers')
require('bucket_class_bytes' in bucket and 'bucket_slots_per_page' in bucket, 'helpers')

print('OK: BucketAllocator init 0x17990 + alloc gate 0x179c0 + pool setup 0x18750 '
      '(64 KiB pages, classes 0x10..0x400 step 0x10, header 0x38); '
      'native constants match; freelist surgery body not lifted')
