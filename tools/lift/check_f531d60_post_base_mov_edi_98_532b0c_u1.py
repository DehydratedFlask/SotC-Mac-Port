#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG, FRAG_SIZE, END = 0x532B0C, 5, 0x532B11
FRAG_SHA256 = "8701905b6331edca312a4b173aa2f603ccfd80a39c1382f4aa36467e2a5a091f"
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CONT = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require("kInit531d60PostBaseContinueEEndRva = 0x532b0c" in CONT.read_text(),'cont_e end')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('bf98000000'),'mov edi')
    require(img.read(END,1)[0]==0xE8,'call head')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_edi_98_532b0c_u1' in nh,'api')
    reg=REG.read_text()
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b60' in reg,'pin')
    require('merge_mov_edi_98_532b0c_u1_ran' in reg,'merge mov')
    require('merge_call_17070_532b11_u1_ran' in reg,'merge call')
    require('merge_mov_r14_rax_532b16_u1_ran' in reg,'merge mov r14')
    require('merge_call_54dc10_532b19_u1_ran' in reg,'merge call 54dc10')
    require('init_531d60_post_base_mov_edi_98_532b0c_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_edi_98_532b0c_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
