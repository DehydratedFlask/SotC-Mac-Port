#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BD1
FRAG, FRAG_SIZE, END = 0x532BD1, 3, 0x532BD4
FRAG_BYTES = bytes.fromhex("4885ff")
FRAG_SHA256 = "78126bd6ec0824c8cdd3ca1d994e12bfa11fcc75666a04538c9c74e495761e6c"
NEXT_JE_RVA = 0x532BD4
NEXT_HEAD = bytes.fromhex("7405")
NATIVE = ROOT / "native/core/init_531d60_post_base_test_rdi_rdi_532bd1_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1EndRva = 0x532bd1' in PREV.read_text(),'mov end')
    require(PREV_END == FRAG,'contiguous after mov rdi mem r12 20 532bcc')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'test rdi,rdi 4885ff')
    je=img.read(NEXT_JE_RVA,2)
    require(je==NEXT_HEAD,'je head @ 0x532bd4')
    rel=struct.unpack('b',bytes([je[1]]))[0]
    require(NEXT_JE_RVA+2+rel==0x532BDB,'je target 0x532bdb')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_test_rdi_rdi_532bd1_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in BODY.read_text(),'body merge')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bde' in REG.read_text(),'pin')
    require('init_531d60_post_base_test_rdi_rdi_532bd1_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_test_rdi_rdi_532bd1_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
