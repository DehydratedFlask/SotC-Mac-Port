#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BCC
FRAG, FRAG_SIZE, END = 0x532BCC, 5, 0x532BD1
FRAG_BYTES = bytes.fromhex("498b7c2420")
FRAG_SHA256 = "32218b472edbb3cded85cbe397df6d6972f153b7f576d5e0b4658a2c0474711c"
DISP = 0x20
NEXT_HEAD = bytes.fromhex("4885ff")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_call_170c0_532bc7_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseCall170c0_532bc7_U1EndRva = 0x532bcc' in PREV.read_text(),'call end')
    require(PREV_END == FRAG,'contiguous after call 170c0 532bc7')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'mov rdi,[r12+0x20]')
    require(frag[4]==DISP,'disp 0x20')
    require(img.read(END,3)==NEXT_HEAD,'test rdi head @ 0x532bd1')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in BODY.read_text(),'body merge')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
