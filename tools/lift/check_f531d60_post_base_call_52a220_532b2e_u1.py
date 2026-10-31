#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B2E
FRAG, FRAG_SIZE, END = 0x532B2E, 5, 0x532B33
FRAG_SHA256 = "3e05fe35c3d69687e249aabf44b562760e0c6d7f706a144223b0f7f9d22ab747"
CALL_TARGET = 0x52A220
NEXT_HEAD = bytes.fromhex("488d1d")
NATIVE = ROOT / "native/core/init_531d60_post_base_call_52a220_532b2e_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovEdxEax_532b2c_U1EndRva = 0x532b2e' in PREV.read_text(),'mov edx end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag[0]==0xE8,'call')
    rel=struct.unpack('<i',frag[1:5])[0]
    require(FRAG+5+rel==CALL_TARGET,'target 0x52a220')
    require(img.read(END,3)==NEXT_HEAD,'lea rbx head @ 0x532b33')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_call_52a220_532b2e_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_call_52a220_532b2e_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b3a' in reg,'pin')
    require('init_531d60_post_base_call_52a220_532b2e_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_call_52a220_532b2e_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
