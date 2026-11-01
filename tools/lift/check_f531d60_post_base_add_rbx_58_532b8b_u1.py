#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B8B
FRAG, FRAG_SIZE, END = 0x532B8B, 4, 0x532B8F
FRAG_BYTES = bytes.fromhex("4883c358")
FRAG_SHA256 = "16dad117ec7bdabdccd3671227621b434a859b9e2dec0083125ab59e39ac380d"
IMM = 0x58
NEXT_HEAD = bytes.fromhex("4939df")
NATIVE = ROOT / "native/core/init_531d60_post_base_add_rbx_58_532b8b_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_vcall18_532b88_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseVcall18_532b88_U1EndRva = 0x532b8b' in PREV.read_text(),'vcall18 end')
    require(PREV_END == FRAG,'contiguous after vcall18')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'add rbx,0x58 4883c358')
    require(frag[3]==IMM,'imm 0x58')
    require(img.read(END,3)==NEXT_HEAD,'cmp r15 rbx head @ 0x532b8f')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_add_rbx_58_532b8b_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_add_rbx_58_532b8b_u1_ran' in BODY.read_text(),'body merge')
    require('merge_add_rbx_58_532b8b_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b92' in REG.read_text(),'pin')
    require('init_531d60_post_base_add_rbx_58_532b8b_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_add_rbx_58_532b8b_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
