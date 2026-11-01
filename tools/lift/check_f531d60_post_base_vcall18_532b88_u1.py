#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B84
FRAG, FRAG_SIZE, END = 0x532B88, 3, 0x532B8B
FRAG_BYTES = bytes.fromhex("ff5018")
FRAG_SHA256 = "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96"
NEXT_HEAD = bytes.fromhex("4883c3")
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532b88_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseLeaRdiRbx_28_532b84_U1EndRva = 0x532b88' in PREV.read_text(),'lea rdi end')
    require(PREV_END + 4 == FRAG,'contiguous after lea rdi')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'call [rax+0x18] ff5018')
    require(img.read(END,3)==NEXT_HEAD,'add rbx head @ 0x532b8b')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_vcall18_532b88_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_vcall18_532b88_u1_ran' in BODY.read_text(),'body merge')
    require('merge_vcall18_532b88_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b94' in REG.read_text(),'pin')
    require('init_531d60_post_base_vcall18_532b88_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_vcall18_532b88_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
