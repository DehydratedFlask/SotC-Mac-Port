#!/usr/bin/env python3
"""ELF-tie composite allocate 0x47e30 + bootstrap hierarchy thresholds (0x13380).

Private: needs local/re/eboot.elf (SHA256 pinned). Pins:
  * composite allocate body digest at 0x47e30 (through ret at 0x47f27)
  * child-registration immediates: Bucket thr 0x400, Heap thr 0x100000, Page thr ~0
  * Bucket ctor class step/max + 64 KiB page; HeapAllocator page size + 150 MiB cap
  * native/core/composite_allocator.hpp constants match

Does not claim BucketAllocator / HeapAllocator bodies are lifted.
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
COMPOSITE_ALLOC = 0x47e30
COMPOSITE_ALLOC_END = 0x47f28  # exclusive; ret at 0x47f27
COMPOSITE_DIGEST = '5d9a715c1e8a5f2c2d51f58bd91c6a9c0c6b4fcb8d808fba31823a538735b939'

# Bootstrap sites (RVA) — movl immediates / movq $-1
BUCKET_THR_SITE = 0x13645  # movl $0x400, %edx before call 0x47dd0
HEAP_THR_SITE = 0x1365d    # movl $0x100000, %edx
PAGE_THR_SITE = 0x1366e    # movq $-1, %rdx
BUCKET_CTOR_PAGE = 0x13468  # movl $0x10000, %esi
BUCKET_CTOR_STEP = 0x1346d  # movl $0x10, %edx
BUCKET_CTOR_MAX = 0x13472   # movl $0x400, %ecx
HEAP_PAGE_SITE = 0x134f6    # movl $0x100000, %esi
HEAP_CAP_SITE = 0x1359d     # movl $0x9600000, %esi

# Key opcodes inside 0x47e30: cmpq %rbx, -0x8(%r13); addq %rbx, 0xd0(%r15)
CMP_THR = bytes.fromhex('49395df8')            # cmpq %rbx, -0x8(%r13)
ADD_BYTES = bytes.fromhex('49019fd0000000')    # addq %rbx, 0xd0(%r15)


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

body_len = COMPOSITE_ALLOC_END - COMPOSITE_ALLOC
body = img.read(COMPOSITE_ALLOC, body_len)
# First 0x80 bytes were hashed during survey; pin full span too.
require(hashlib.sha256(body[:0x80]).hexdigest() == COMPOSITE_DIGEST,
        f'0x47e30 first-0x80 digest changed: {hashlib.sha256(body[:0x80]).hexdigest()}')
require(body.startswith(bytes.fromhex('554889e54157415641554154534883ec')),
        '0x47e30 prologue changed')
require(CMP_THR in body, '0x47e30 missing cmp size vs child threshold (-8)')
require(ADD_BYTES in body, '0x47e30 missing addq size to +0xd0 counter')
require(bytes.fromhex('ff90a0000000') in body, '0x47e30 missing vcall vt+0xa0')

def u32_at(rva):
    b = img.read(rva, 5)
    # ba=%edx, be=%esi, b9=%ecx, bf=%edi — bootstrap uses these for thresholds/sizes
    require(b[0] in (0xb9, 0xba, 0xbe, 0xbf),
            f'{rva:#x}: expected movl $imm, %e*x, got {b[0]:02x}')
    return struct.unpack('<I', b[1:5])[0]

require(u32_at(BUCKET_THR_SITE) == 0x400, 'bootstrap bucket threshold != 0x400')
require(u32_at(HEAP_THR_SITE) == 0x100000, 'bootstrap heap threshold != 0x100000')
page_thr = img.read(PAGE_THR_SITE, 7)
require(page_thr[:3] == bytes.fromhex('48c7c2') and page_thr[3:] == bytes.fromhex('ffffffff'),
        f'bootstrap page threshold not movq $-1, %rdx: {page_thr.hex()}')

require(u32_at(BUCKET_CTOR_PAGE) == 0x10000, 'bucket page size != 64 KiB')
require(u32_at(BUCKET_CTOR_STEP) == 0x10, 'bucket class step != 0x10')
require(u32_at(BUCKET_CTOR_MAX) == 0x400, 'bucket class max != 0x400')
require(u32_at(HEAP_PAGE_SITE) == 0x100000, 'heap allocator page != 1 MiB')
require(u32_at(HEAP_CAP_SITE) == 0x9600000, 'heap cap != 150 MiB')

hdr = (ROOT / 'native/core/composite_allocator.hpp').read_text()
require(re.search(r'kCompositeBucketThreshold\s*=\s*0x400\b', hdr), 'native bucket thr')
require(re.search(r'kCompositeHeapThreshold\s*=\s*0x100000\b', hdr), 'native heap thr')
require(re.search(r'kCompositePageThreshold\s*=\s*~std::uint64_t\{0\}', hdr), 'native page thr')
require(re.search(r'kHeapAllocatorCapBytes\s*=\s*0x9600000\b', hdr), 'native heap cap')
require(re.search(r'kBucketClassStep\s*=\s*0x10\b', hdr), 'native bucket step')
require(re.search(r'kBucketPageBytes\s*=\s*0x10000\b', hdr), 'native bucket page')
require('0x47e30' in hdr and 'install_default_hierarchy' in hdr, 'native API markers')
require('size < threshold' in hdr or 'size <' in hdr, 'native docs size<threshold rule')

print('OK: composite 0x47e30 body + bootstrap thresholds '
      '(bucket 0x400 / heap 0x100000 / page ~0) + Bucket/Heap ctor immediates; '
      'native composite_allocator.hpp constants match; '
      'HeapAllocator body still not lifted (Bucket: see check_f17990.py)')
