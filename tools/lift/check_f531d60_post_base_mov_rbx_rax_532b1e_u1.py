#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B1E
FRAG, FRAG_SIZE, END = 0x532B1E, 3, 0x532B21
FRAG_SHA256 = "51d71a01be52beeddb3cb945fc70488f8c47dae061ccc4ec1439ad84698b0d88"
CALL_TARGET = 0x54DC20
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_call_54dc10_532b19_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseCall54dc10_532b19_U1EndRva = 0x532b1e' in PREV.read_text(),'call end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('4889c3'),'mov rbx,rax')
    require(img.read(END,1)[0]==0xE8,'call head')
    b=img.read(END,5); rel=struct.unpack('<i',b[1:5])[0]
    require(END+5+rel==CALL_TARGET,'target 0x54dc20')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_mov_rbx_rax_532b1e_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b29' in reg,'pin')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rbx_rax_532b1e_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
