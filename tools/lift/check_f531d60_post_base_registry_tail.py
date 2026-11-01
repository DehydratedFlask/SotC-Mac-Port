#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B0C, 13, 0x532B19
REGION_SHA256 = "79df3d04a513f1c47a11acb015503e4d2f96d95322ad22cc4026c11d4d5d3fbd"
CALL_RVA, CALL_TARGET = 0x532B11, 0x17070
UNITS=[
    (0x532B0C,5,"8701905b6331edca312a4b173aa2f603ccfd80a39c1382f4aa36467e2a5a091f"),
    (0x532B11,5,"aff4e9c4992db897d411fab1b12fce279b2e1a7772ba6cb3ee9b3d4c6a95b3f1"),
    (0x532B16,3,"cd7c2f6278caef866d72542e1d57ec0ebaa91b3fd681894c8ae87bdf769afc30"),
]
NATIVE=ROOT/"native/core/init_531d60_post_base_registry_tail.hpp"
NCPP=ROOT/"native/core/init_531d60_post_base_registry_tail.cpp"
CALL=ROOT/"native/core/init_531d60_post_base_call_17070_532b11_u1.hpp"
MOV14=ROOT/"native/core/init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"
MOV=ROOT/"native/core/init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
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
    require(len(UNITS)==3,'units')
    require(hashlib.sha256(img.read(START,SIZE)).hexdigest()==REGION_SHA256,'region')
    for rva,sz,sha in UNITS:
        require(hashlib.sha256(img.read(rva,sz)).hexdigest()==sha,f'unit@{rva:#x}')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    require(img.read(END,1)[0]==0xE8,'next call 0x54dc10 head')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryTailEndRva = 0x532b19' in nh,'end')
    require('RegistryTailUnit : int { call_17070_532b11_u1, mov_r14_rax_532b16_u1, count }' in nh,'unit enum')
    require('merge_mov_edi_98_532b0c_u1_ran' in nh,'merge mov')
    require('merge_call_17070_532b11_u1_ran' in nh,'merge call')
    require('merge_mov_r14_rax_532b16_u1_ran' in nh,'merge mov r14')
    require('init_531d60_post_base_call_17070_532b11_u1' in NCPP.read_text(),'cpp call')
    require('init_531d60_post_base_mov_r14_rax_532b16_u1' in NCPP.read_text(),'cpp mov r14')
    require('init_531d60_post_base_mov_edi_98_532b0c_u1' in NCPP.read_text(),'cpp mov')
    require('init_531d60_post_base_call_17070_532b11_u1' in CALL.read_text(),'call hpp')
    require('init_531d60_post_base_mov_r14_rax_532b16_u1' in MOV14.read_text(),'mov r14 hpp')
    require('post_base_registry_tail_hooks' in SL.read_text(),'slice hpp')
    require('init_531d60_post_base_registry_tail' in SC.read_text(),'slice cpp')
    require('init_531d60_post_base_registry_tail.cpp' in CMAKE.read_text(),'cmake tail')
    require('init_531d60_post_base_call_17070_532b11_u1.cpp' in CMAKE.read_text(),'cmake call')
    require('init_531d60_post_base_mov_r14_rax_532b16_u1.cpp' in CMAKE.read_text(),'cmake mov r14')
    require('merge_call_17070_532b11_u1_ran' in TEST.read_text(),'test merge call')
    require('merge_mov_r14_rax_532b16_u1_ran' in TEST.read_text(),'test merge mov r14')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in REG.read_text(),'reg pin')
    print('OK check_f531d60_post_base_registry_tail post-peel=0x532b19+0B (body next)')
    return 0
if __name__=='__main__': raise SystemExit(main())
