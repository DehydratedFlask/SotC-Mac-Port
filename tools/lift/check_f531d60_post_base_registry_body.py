#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B19, 26, 0x532B33
REGION_SHA256 = "b1a899150d297b60caa81c95edd1e14228dddb950065efd4158aaecd0d0da134"
CALL_RVA, CALL_TARGET = 0x532B19, 0x54DC10
FILL_RVA, FILL_TARGET = 0x532B2E, 0x52A220
UNITS=[
    (0x532B19,5,"41cb14749a699157e6b6ec1b94b6aac68ab165c466cb18b06db3bba19e3ef0c3"),
    (0x532B1E,3,"51d71a01be52beeddb3cb945fc70488f8c47dae061ccc4ec1439ad84698b0d88"),
    (0x532B21,5,"30b15fffa8fcb8433b00b5f77a62c1d027be6b9a5e9e9689205a84adcdcc4fbe"),
    (0x532B26,3,"34eb50fdb7685f0af0775bc2ce9660fd5573393e1dac2a8c90d2ff1cb877d999"),
    (0x532B29,3,"1357a4dffac35070330d8a4a9dd8130f8ae30beb05cd49402be087ba39c3a4c6"),
    (0x532B2C,2,"253aa00c2dbbb117d146347fbacae9ee27e47b32bcdef68e72f244258330c351"),
    (0x532B2E,5,"3e05fe35c3d69687e249aabf44b562760e0c6d7f706a144223b0f7f9d22ab747"),
]
NATIVE=ROOT/"native/core/init_531d60_post_base_registry_body.hpp"
NCPP=ROOT/"native/core/init_531d60_post_base_registry_body.cpp"
CALL=ROOT/"native/core/init_531d60_post_base_call_54dc10_532b19_u1.hpp"
CALL20=ROOT/"native/core/init_531d60_post_base_call_54dc20_532b21_u1.hpp"
CALL52=ROOT/"native/core/init_531d60_post_base_call_52a220_532b2e_u1.hpp"
MOVRBX=ROOT/"native/core/init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
MOVR15=ROOT/"native/core/init_531d60_post_base_mov_r15_rax_532b26_u1.hpp"
MOVRSI=ROOT/"native/core/init_531d60_post_base_mov_rsi_rbx_532b29_u1.hpp"
MOVEDX=ROOT/"native/core/init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"
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
    require(len(UNITS)==7,'units')
    require(hashlib.sha256(img.read(START,SIZE)).hexdigest()==REGION_SHA256,'region')
    for rva,sz,sha in UNITS:
        require(hashlib.sha256(img.read(rva,sz)).hexdigest()==sha,f'unit@{rva:#x}')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    b=img.read(FILL_RVA,5); require(b[0]==0xE8,'fill call')
    rel=struct.unpack('<i',b[1:5])[0]; require(FILL_RVA+5+rel==FILL_TARGET,'fill 0x52a220')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryBodyEndRva = 0x532b33' in nh,'end')
    require('call_54dc10_532b19_u1' in nh and 'mov_rbx_rax_532b1e_u1' in nh and 'call_54dc20_532b21_u1' in nh and 'mov_r15_rax_532b26_u1' in nh and 'mov_rsi_rbx_532b29_u1' in nh and 'mov_edx_eax_532b2c_u1' in nh and 'call_52a220_532b2e_u1' in nh,'unit enum')
    require('kInit531d60PostBaseRegistryBodyUnitCount' in nh,'unit count')
    require('merge_call_54dc10_532b19_u1_ran' in nh,'merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in nh,'merge mov rbx')
    require('merge_call_54dc20_532b21_u1_ran' in nh,'merge call count')
    require('merge_mov_r15_rax_532b26_u1_ran' in nh,'merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in nh,'merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in nh,'merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in nh,'merge fill call')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in NCPP.read_text(),'cpp mov rbx')
    require('init_531d60_post_base_call_54dc20_532b21_u1' in NCPP.read_text(),'cpp call count')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1' in NCPP.read_text(),'cpp mov r15')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1' in NCPP.read_text(),'cpp mov rsi')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1' in NCPP.read_text(),'cpp mov edx')
    require('init_531d60_post_base_call_52a220_532b2e_u1' in NCPP.read_text(),'cpp fill call')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in CALL.read_text(),'call hpp')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in MOVRBX.read_text(),'mov rbx hpp')
    require('init_531d60_post_base_call_54dc20_532b21_u1' in CALL20.read_text(),'call count hpp')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1' in MOVR15.read_text(),'mov r15 hpp')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1' in MOVRSI.read_text(),'mov rsi hpp')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1' in MOVEDX.read_text(),'mov edx hpp')
    require('init_531d60_post_base_call_52a220_532b2e_u1' in CALL52.read_text(),'fill call hpp')
    require('kInit531d60PostBaseMovR14Rax_532b16_U1EndRva = 0x532b19' in MOV14.read_text(),'mov r14 end')
    require('post_base_registry_body_hooks' in SL.read_text(),'slice hpp')
    require('init_531d60_post_base_registry_body' in SC.read_text(),'slice cpp')
    require('merge_call_54dc20_532b21_u1_ran' in SC.read_text(),'slice merge count')
    require('merge_mov_r15_rax_532b26_u1_ran' in SC.read_text(),'slice merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in SC.read_text(),'slice merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in SC.read_text(),'slice merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in SC.read_text(),'slice merge fill call')
    require('init_531d60_post_base_registry_body.cpp' in CMAKE.read_text(),'cmake body')
    require('init_531d60_post_base_call_54dc10_532b19_u1.cpp' in CMAKE.read_text(),'cmake call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1.cpp' in CMAKE.read_text(),'cmake mov rbx')
    require('init_531d60_post_base_call_54dc20_532b21_u1.cpp' in CMAKE.read_text(),'cmake call count')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1.cpp' in CMAKE.read_text(),'cmake mov r15')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1.cpp' in CMAKE.read_text(),'cmake mov rsi')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1.cpp' in CMAKE.read_text(),'cmake mov edx')
    require('init_531d60_post_base_call_52a220_532b2e_u1.cpp' in CMAKE.read_text(),'cmake fill call')
    require('merge_call_54dc10_532b19_u1_ran' in TEST.read_text(),'test merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in TEST.read_text(),'test merge mov rbx')
    require('merge_call_54dc20_532b21_u1_ran' in TEST.read_text(),'test merge call count')
    require('merge_mov_r15_rax_532b26_u1_ran' in TEST.read_text(),'test merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in TEST.read_text(),'test merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in TEST.read_text(),'test merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in TEST.read_text(),'test merge fill call')
    require('count_from_54dc20' in TEST.read_text(),'test count field')
    require('r15_from_54dc20' in TEST.read_text(),'test r15 field')
    require('rsi_from_rbx' in TEST.read_text(),'test rsi field')
    require('edx_from_eax' in TEST.read_text(),'test edx field')
    require('call_52a220_count' in TEST.read_text(),'test fill call field')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b33' in REG.read_text(),'reg pin')
    print('OK check_f531d60_post_base_registry_body post-peel=0x532b33+0B')
    return 0
if __name__=='__main__': raise SystemExit(main())
