#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BBF
FRAG, FRAG_SIZE, END = 0x532BBF, 3, 0x532BC2
FRAG_BYTES = bytes.fromhex("4d85ed")
FRAG_SHA256 = "e10a2bc66203e957df52f8a9aed93e12244edee28cf26cd938a5b83aaac249d5"
NEXT_HEAD = bytes.fromhex("7408")
NATIVE = ROOT / "native/core/init_531d60_post_base_cmp_r13_r14_532bbf_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovR14MemRsp10_532bba_U1EndRva = 0x532bbf' in PREV.read_text(),'mov end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==FRAG_BYTES,'test r13,r13 4d85ed')
    require(img.read(END,2)==NEXT_HEAD,'je head @ 0x532bc2')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_cmp_r13_r14_532bbf_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in BODY.read_text(),'body merge')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bc7' in REG.read_text(),'pin')
    require('init_531d60_post_base_cmp_r13_r14_532bbf_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_cmp_r13_r14_532bbf_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
