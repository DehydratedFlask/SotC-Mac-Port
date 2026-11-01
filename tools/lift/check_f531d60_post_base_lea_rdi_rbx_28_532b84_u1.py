#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B84
FRAG, FRAG_SIZE, END = 0x532B84, 4, 0x532B88
FRAG_BYTES = bytes.fromhex("488d7b28")
FRAG_SHA256 = "aad5e1d021e7586cfdfbfaa72a2576217c02e8c17b80e6eaacdeb0f529487780"
DISP = 0x28
NEXT_HEAD = bytes.fromhex("ff5018")
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_rax_rbx_48_532b80_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovRaxRbx_48_532b80_U1EndRva = 0x532b84' in PREV.read_text(),'mov rax end')
    require(PREV_END == FRAG,'contiguous after mov rax')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'lea rdi,[rbx+0x28] 488d7b28')
    require(frag[3]==DISP,'disp 0x28')
    require(img.read(END,3)==NEXT_HEAD,'call [rax+0x18] head @ 0x532b88')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_lea_rdi_rbx_28_532b84_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in BODY.read_text(),'body merge')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in REG.read_text(),'reg merge')
    require('init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_lea_rdi_rbx_28_532b84_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
