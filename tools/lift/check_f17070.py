#!/usr/bin/env python3
"""ELF-tie the four allocator wrapper trampolines (0x17070/90/b0/c0) to AllocatorFront.

Private: needs local/re/eboot.elf. These are not full body lifts of 0x12820/0x12af0 — they are
the thin wrappers the contract models as new_/new_array/delete_/delete_array:

  0x17070: mov esi,1; mov edx,8; mov ecx,1; jmp 0x12820   // new  (heap1, align8, flags1)
  0x17090: mov esi,1; mov edx,8; mov ecx,2; jmp 0x12820   // new[] (flags2)
  0x170b0: mov esi,1; jmp 0x12af0                         // delete  (kind1)
  0x170c0: mov esi,2; jmp 0x12af0                         // delete[] (kind2)

Pins body digests and that native/core/{allocator_front,iallocator}.hpp carry matching constants.
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
ALLOC = 0x12820
FREE = 0x12af0

NEW = (0x17070, bytes.fromhex('be01000000ba08000000b901000000e99cb7ffff'),
       '9c9cdfa9ec1727f5e2903512bc08086d93c439323bee3a9212eb4aabfe4ba148')
NEW_ARRAY = (0x17090, bytes.fromhex('be01000000ba08000000b902000000e97cb7ffff'), None)
DELETE = (0x170b0, bytes.fromhex('be01000000e936baffff'), None)
DELETE_ARRAY = (0x170c0, bytes.fromhex('be02000000e926baffff'), None)


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


def jmp_target(rva, body, e9_at):
    require(body[e9_at] == 0xe9, f'{rva:#x}: missing jmp at +{e9_at}')
    return rva + e9_at + 5 + struct.unpack('<i', body[e9_at + 1:e9_at + 5])[0]


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

rva, body, digest = NEW
got = img.read(rva, len(body))
require(got == body, f'0x17070 body changed: {got.hex()}')
require(hashlib.sha256(got).hexdigest() == digest, '0x17070 body sha256 changed')
require(jmp_target(rva, got, 15) == ALLOC, '0x17070 does not jmp to alloc 0x12820')
require(struct.unpack('<I', got[1:5])[0] == 1, '0x17070 heap != 1')
require(struct.unpack('<I', got[6:10])[0] == 8, '0x17070 align != 8')
require(struct.unpack('<I', got[11:15])[0] == 1, '0x17070 flags != 1')

rva, body, _ = NEW_ARRAY
got = img.read(rva, len(body))
require(got == body, f'0x17090 body changed: {got.hex()}')
require(jmp_target(rva, got, 15) == ALLOC, '0x17090 does not jmp to alloc 0x12820')
require(struct.unpack('<I', got[11:15])[0] == 2, '0x17090 flags != 2')

rva, body, _ = DELETE
got = img.read(rva, len(body))
require(got == body, f'0x170b0 body changed: {got.hex()}')
require(jmp_target(rva, got, 5) == FREE, '0x170b0 does not jmp to free 0x12af0')
require(struct.unpack('<I', got[1:5])[0] == 1, '0x170b0 kind != 1')

rva, body, _ = DELETE_ARRAY
got = img.read(rva, len(body))
require(got == body, f'0x170c0 body changed: {got.hex()}')
require(jmp_target(rva, got, 5) == FREE, '0x170c0 does not jmp to free 0x12af0')
require(struct.unpack('<I', got[1:5])[0] == 2, '0x170c0 kind != 2')

front = (ROOT / 'native/core/allocator_front.hpp').read_text()
ialloc = (ROOT / 'native/core/iallocator.hpp').read_text()
require(re.search(r'kDefaultHeap\s*=\s*1\b', front), 'AllocatorFront::kDefaultHeap != 1')
require(re.search(r'kDefaultAlign\s*=\s*8\b', front), 'AllocatorFront::kDefaultAlign != 8')
require(re.search(r'kAllocFlagNew\s*=\s*1\b', ialloc), 'kAllocFlagNew != 1')
require(re.search(r'kAllocFlagNewArray\s*=\s*2\b', ialloc), 'kAllocFlagNewArray != 2')
require('0x17070' in front and '0x17090' in front and '0x170b0' in front and '0x170c0' in front,
        'allocator_front.hpp missing wrapper RVA comments')
require('void* new_(std::size_t size)' in front and 'new_array' in front, 'native wrappers missing')

print('OK: wrappers 0x17070/0x17090 -> alloc 0x12820 (heap=1,align=8,flags=1/2); '
      '0x170b0/0x170c0 -> free 0x12af0 (kind=1/2); native AllocatorFront constants match; '
      '0x12820/0x12af0 bodies remain contract-only (not byte-lifted here)')
