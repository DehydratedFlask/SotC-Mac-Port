#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BBA
FRAG, FRAG_SIZE, END = 0x532BBA, 5, 0x532BBF
FRAG_BYTES = bytes.fromhex("4c8b742410")
FRAG_SHA256 = "0832b50b1127d182d74804dfe1d0e7c47ba24a7959ec75952e83dac10ea85388"
DISP = 0x10
NEXT_HEAD = bytes.fromhex("4d85ed")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseLeaRbx2efd160_532bb3_U1EndRva = 0x532bba' in PREV.read_text(),'lea end')
    require(PREV_END == FRAG,'contiguous after lea 532bb3')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'mov r14,[rsp+0x10]')
    require(frag[4]==DISP,'disp 0x10')
    require(img.read(END,3)==NEXT_HEAD,'head @ 0x532bbf')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_r14_mem_rsp10_532bba_u1_ran' in BODY.read_text(),'body merge')
    require('merge_mov_r14_mem_rsp10_532bba_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bde' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_r14_mem_rsp10_532bba_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
