#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BB3
FRAG, FRAG_SIZE, END = 0x532BB3, 7, 0x532BBA
FRAG_BYTES = bytes.fromhex("488d1da6a59c02")
FRAG_SHA256 = "8163125859f1d4162340a7a7a06772541f61c5f19b6918952884eca7a28904b7"
NEXT_HEAD = bytes.fromhex("4c8b74")
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovR13MemR12_40_532bae_U1EndRva = 0x532bb3' in PREV.read_text(),'mov r13 end')
    require(PREV_END == FRAG,'contiguous after mov r13 mem r12')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'lea rbx,[0x2efd160] 488d1da6a59c02')
    require(img.read(END,3)==NEXT_HEAD,'head @ 0x532bba')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_lea_rbx_2efd160_532bb3_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_lea_rbx_2efd160_532bb3_u1_ran' in BODY.read_text(),'body merge')
    require('merge_lea_rbx_2efd160_532bb3_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532c0f' in REG.read_text(),'pin')
    require('init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_lea_rbx_2efd160_532bb3_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
