#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B9D
FRAG, FRAG_SIZE, END = 0x532B9D, 3, 0x532BA0
FRAG_SHA256 = "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab"
CALL_TARGET = 0x170C0
NEXT_HEAD = bytes.fromhex("e81b45aeff")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rbx_532b9d_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_je_532ba5_532b9b_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseJe532ba5_532b9b_U1EndRva = 0x532b9d' in PREV.read_text(),'je end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('4889df'),'mov rdi,rbx')
    require(img.read(END,5)==NEXT_HEAD[:5],'call head @ 0x532ba0')
    b=img.read(END,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(END+5+rel==CALL_TARGET,'call 0x170c0')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rdi_rbx_532b9d_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in BODY.read_text(),'body merge')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532ba0' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_rdi_rbx_532b9d_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rdi_rbx_532b9d_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
