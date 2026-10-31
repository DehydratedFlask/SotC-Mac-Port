#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B5B
FRAG, FRAG_SIZE, END = 0x532B5B, 5, 0x532B60
FRAG_SHA256 = "5161bd7ca4697196a7ed118df5923758c9c21e074bbe80a1e68063cc42b866e5"
NEXT_HEAD = bytes.fromhex("4d8b74")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovR14MemRsp10_532b56_U1EndRva = 0x532b5b' in PREV.read_text(),'mov rsp10 end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('4d8b6c2440'),'mov r13,[rsp+0x40]')
    require(img.read(END,3)==NEXT_HEAD,'mov r14,[rsp+0x48] head @ 0x532b60')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b60' in reg,'pin')
    require('init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_r13_mem_rsp40_532b5b_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
