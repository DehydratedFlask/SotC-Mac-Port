#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B19, 8, 0x532B21
REGION_SHA256 = "d029007db957fefc282370fd2fdc835d76eab2df7165edaa3441c2adabfa4840"
CALL_RVA, CALL_TARGET = 0x532B19, 0x54DC10
UNITS=[
    (0x532B19,5,"41cb14749a699157e6b6ec1b94b6aac68ab165c466cb18b06db3bba19e3ef0c3"),
    (0x532B1E,3,"51d71a01be52beeddb3cb945fc70488f8c47dae061ccc4ec1439ad84698b0d88"),
]
NATIVE=ROOT/"native/core/init_531d60_post_base_registry_body.hpp"
NCPP=ROOT/"native/core/init_531d60_post_base_registry_body.cpp"
CALL=ROOT/"native/core/init_531d60_post_base_call_54dc10_532b19_u1.hpp"
MOVRBX=ROOT/"native/core/init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
MOV14=ROOT/"native/core/init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"
SL=ROOT/"native/core/appmain_init_ctor_98a90_slice.hpp"
SC=ROOT/"native/core/appmain_init_ctor_98a90_slice.cpp"
TEST=ROOT/"native/tests/test_appmain_init_ctor_98a90_slice.cpp"
CMAKE=ROOT/"native/CMakeLists.txt"
REG=ROOT/"native/core/registry_init_531d60.hpp"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes(); require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require(len(UNITS)==2,'units')
    require(hashlib.sha256(img.read(START,SIZE)).hexdigest()==REGION_SHA256,'region')
    for rva,sz,sha in UNITS:
        require(hashlib.sha256(img.read(rva,sz)).hexdigest()==sha,f'unit@{rva:#x}')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    require(img.read(END,1)[0]==0xE8,'call 54dc20 head')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryBodyEndRva = 0x532b21' in nh,'end')
    require('RegistryBodyUnit : int { call_54dc10_532b19_u1, mov_rbx_rax_532b1e_u1, count }' in nh,'unit enum')
    require('merge_call_54dc10_532b19_u1_ran' in nh,'merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in nh,'merge mov rbx')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in NCPP.read_text(),'cpp mov rbx')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in CALL.read_text(),'call hpp')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in MOVRBX.read_text(),'mov rbx hpp')
    require('kInit531d60PostBaseMovR14Rax_532b16_U1EndRva = 0x532b19' in MOV14.read_text(),'mov r14 end')
    require('post_base_registry_body_hooks' in SL.read_text(),'slice hpp')
    require('init_531d60_post_base_registry_body' in SC.read_text(),'slice cpp')
    require('init_531d60_post_base_registry_body.cpp' in CMAKE.read_text(),'cmake body')
    require('init_531d60_post_base_call_54dc10_532b19_u1.cpp' in CMAKE.read_text(),'cmake call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1.cpp' in CMAKE.read_text(),'cmake mov rbx')
    require('merge_call_54dc10_532b19_u1_ran' in TEST.read_text(),'test merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in TEST.read_text(),'test merge mov rbx')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b21' in REG.read_text(),'reg pin')
    print('OK check_f531d60_post_base_registry_body post-peel=0x532b21+0B')
    return 0
if __name__=='__main__': raise SystemExit(main())
