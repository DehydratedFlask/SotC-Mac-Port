#!/usr/bin/env python3
"""ELF-tie lazy bootstrap latch byte 0x2dfaaa0 in AllocatorFront::alloc (0x12820).

Private: needs local/re/eboot.elf. Pins the alloc-path gate at 0x1284a..0x12873
(latch==0 && tbl[1]==0 → call 0x13380; then heap id := [rax+0x1c]; tbl[id]=rax),
the latch set at 0x110d3 ($1) and clear at 0x11f94 ($0), and native
allocator_front.hpp/cpp markers. Does NOT claim full 0x110a0 / 0x11e90 bodies or
free-path latch copies are lifted.
"""
import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
LATCH = 0x2dfaaa0
TBL1 = 0x2dfaab8
ALLOC = 0x12820
GATE = 0x1284a
GATE_END = 0x12873
BOOT = 0x13380
SET_LATCH = 0x110d3
CLR_LATCH = 0x11f94
GATE_BODY = bytes.fromhex(
    '8a055082de02'      # movb latch(%rip), %al
    '84c0'              # testb %al, %al
    '751f'              # jne 0x12873
    '488b055d82de02'    # movq tbl[1](%rip), %rax
    '4885c0'            # testq %rax, %rax
    '7513'              # jne 0x12873
    'e81b0b0000'        # call 0x13380
    '8b581c'            # movl 0x1c(%rax), %ebx
    '488d0d4182de02'    # leaq tbl(%rip), %rcx
    '488904d9'          # movq %rax, (%rcx,%rbx,8)
)
GATE_DIGEST = hashlib.sha256(GATE_BODY).hexdigest()


def require(cond, msg):
    if not cond:
        raise SystemExit(f'FAIL {msg}')


def rip_target(rva, insn_len, disp_off, raw):
    disp = struct.unpack_from('<i', raw, disp_off)[0]
    return rva + insn_len + disp


elf = (ROOT / 'local/re/eboot.elf').read_bytes()
require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)

gate = img.read(GATE, GATE_END - GATE)
require(gate == GATE_BODY, f'0x1284a..0x12873 changed: {gate.hex()}')
require(hashlib.sha256(gate).hexdigest() == GATE_DIGEST, 'gate digest')
require(GATE_DIGEST == 'fa92c572defa9d14c9cc1a5c9c84698aac7ef918a0816d731ce0601ecd01fed7',
        'unexpected frozen digest')

require(rip_target(GATE, 6, 2, gate) == LATCH, 'gate latch VA')
require(rip_target(0x12854, 7, 3, img.read(0x12854, 7)) == TBL1, 'gate tbl[1] VA')
call = img.read(0x12860, 5)
require(call[0] == 0xe8 and 0x12860 + 5 + struct.unpack('<i', call[1:5])[0] == BOOT,
        'lazy call != 0x13380')
require(img.read(ALLOC, 4) == bytes.fromhex('554889e5'), '0x12820 prologue')

setb = img.read(SET_LATCH, 7)
require(setb[0] == 0xc6 and setb[1] == 0x05 and setb[6] == 0x01, '0x110d3 not movb $1')
require(rip_target(SET_LATCH, 7, 2, setb) == LATCH, '0x110d3 target')
clrb = img.read(CLR_LATCH, 7)
require(clrb[0] == 0xc6 and clrb[1] == 0x05 and clrb[6] == 0x00, '0x11f94 not movb $0')
require(rip_target(CLR_LATCH, 7, 2, clrb) == LATCH, '0x11f94 target')

hdr = (ROOT / 'native/core/allocator_front.hpp').read_text()
src = (ROOT / 'native/core/allocator_front.cpp').read_text()
require('0x2dfaaa0' in hdr and 'bootstrap_latch' in hdr, 'native latch API')
require('set_lazy_bootstrap' in hdr and 'LazyBootstrap' in hdr, 'native lazy hook')
require('0x1284a' in hdr, 'native gate RVA')
require('bootstrap_latch_{false}' in hdr, 'default latch false')
require('maybe_lazy_bootstrap' in src, 'native alloc gate')

print('OK: lazy latch 0x2dfaaa0 gate 0x1284a..0x12873 sha256 fa92c572…fed7 '
      '(call 0x13380; id@[rax+0x1c]; tbl[id]=rax) + set 0x110d3=$1 + clear 0x11f94=$0; '
      'native AllocatorFront markers; 0x110a0/0x11e90 bodies and free-path copies not lifted')
