#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B19, 100, 0x532B7D
REGION_SHA256 = "89c6205e56e8fe0d1691e5fa553a47e293f1c63f32c57af7981c4c52347ccf69"
PEEL_LEA = bytes.fromhex("488d742410")
PEEL_MOV = bytes.fromhex("498b5d10")
PEEL_MOV15 = bytes.fromhex("4d8b7d18")
PEEL_CMP15 = bytes.fromhex("4c39fb")
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
    (0x532B33,7,"ebee8620a2677aefedddc1a878949239ca4ce17f716fec6860c70dd1862d2b0e"),
    (0x532B3A,7,"bdd7f670e8d62797e4c60b6970c53a006e7136517bff55a0d59ca0d80a952cdd"),
    (0x532B41,3,"e0391760ebfa69e56be275acf71537de82d59ce1b1ce9c3a036449f88e36698f"),
    (0x532B44,3,"c5d9abd967df5be1eb8e69ebea4f667ab2311f3cbe1510e2349d558bd194c5cb"),
    (0x532B47,6,"4e6ccfb65620d791cdb7ac0ce5025f52a7d1cdcd1409819b1f8b84b4a61d8d64"),
    (0x532B4D,3,"9d0cc8e4eff8ad01b73d81e5cdcfd2084a64c63f96519e4d355e004f152c44aa"),
    (0x532B50,6,"08410fa0f64c8db2820a0317c4713b6fad3bfd7b6b814b06cff5d9b409b50b90"),
    (0x532B56,5,"75f153aee6705235547b5054090335285afc4451945d9c16fd4964cf99d2b9c3"),
    (0x532B5B,5,"5161bd7ca4697196a7ed118df5923758c9c21e074bbe80a1e68063cc42b866e5"),
    (0x532B60,5,"ba79cedc8b477cac77cf700b7f8a1aac88f34f68685351ad5c11439b321ca3e0"),
    (0x532B65,3,"e1fc26b5bab7014dc3b5f125c9dbd3f780ec07a8ab595b815acdb4537e241a62"),
    (0x532B68,2,"73f9818196b42809dbdad00e7e0e80324921ea14d859ab9e89558b13895ba036"),
    (0x532B6A,5,"0c271084a89b328592087dc0a49b01cbb8f82393b452bb157d96a4f7a60338ba"),
    (0x532B70,4,"51cf1bb258add2501b662c9a241ee79e38a7cf232aaae570ce5e3ce40dd41e42"),
    (0x532B74,4,"bcb886a3b4697acd64e109a054f3fd7018a1a54cab8159cc6a20a54b074bcccb"),
    (0x532B78,3,"807dff49cce37babf57f11cd3ea8002bf09c5dfa3810879d76a9d49e06e94c77"),
    (0x532B7B,2,"841437fd6b8a9aea74f9ee2b7b903afa11b5702e99d301c7fa4bd2ba3665ed2e"),
]
MOVR15R13=ROOT/"native/core/init_531d60_post_base_mov_r15_r13_18_532b74_u1.hpp"
CMPC15=ROOT/"native/core/init_531d60_post_base_cmp_r15_r11_532b78_u1.hpp"
JEB98=ROOT/"native/core/init_531d60_post_base_je_532b98_532b7b_u1.hpp"
CMPC13=ROOT/"native/core/init_531d60_post_base_cmp_r13_r14_532b65_u1.hpp"
JEBBA=ROOT/"native/core/init_531d60_post_base_je_532bba_532b68_u1.hpp"
LEARSI=ROOT/"native/core/init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.hpp"
MOVRBXR13=ROOT/"native/core/init_531d60_post_base_mov_rbx_r13_10_532b70_u1.hpp"
MOVRSP=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.hpp"
MOVR14RSP48=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.hpp"
MOVR13RSP40=ROOT/"native/core/init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.hpp"
CMPC=ROOT/"native/core/init_531d60_post_base_cmp_r12_r14_532b44_u1.hpp"
JE=ROOT/"native/core/init_531d60_post_base_je_532bfb_532b47_u1.hpp"
JENULL=ROOT/"native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp"
TESTU=ROOT/"native/core/init_531d60_post_base_test_r12_r12_532b4d_u1.hpp"
MOVR12=ROOT/"native/core/init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"
MOVR13=ROOT/"native/core/init_531d60_post_base_mov_r13_15fe518_532b3a_u1.hpp"
LEARBX=ROOT/"native/core/init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"
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

def patched_region(img):
    data=bytearray(img.read(START,SIZE))
    data[0x532B6A-START:0x532B6F-START]=PEEL_LEA
    data[0x532B6F-START]=0
    data[0x532B70-START:0x532B74-START]=PEEL_MOV
    data[0x532B74-START:0x532B78-START]=PEEL_MOV15
    data[0x532B78-START:0x532B7B-START]=PEEL_CMP15
    return bytes(data)

def unit_bytes(img,rva,sz):
    if rva==0x532B6A and sz==5:
        return PEEL_LEA
    if rva==0x532B70 and sz==4:
        return PEEL_MOV
    if rva==0x532B74 and sz==4:
        return PEEL_MOV15
    if rva==0x532B78 and sz==3:
        return PEEL_CMP15
    return img.read(rva,sz)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes(); require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require(len(UNITS)==24,'units')
    require(hashlib.sha256(patched_region(img)).hexdigest()==REGION_SHA256,'region')
    for rva,sz,sha in UNITS:
        require(hashlib.sha256(unit_bytes(img,rva,sz)).hexdigest()==sha,f'unit@{rva:#x}')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    b=img.read(FILL_RVA,5); require(b[0]==0xE8,'fill call')
    rel=struct.unpack('<i',b[1:5])[0]; require(FILL_RVA+5+rel==FILL_TARGET,'fill 0x52a220')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryBodyEndRva = 0x532b7d' in nh,'end')
    require('call_54dc10_532b19_u1' in nh and 'mov_rbx_rax_532b1e_u1' in nh and 'call_54dc20_532b21_u1' in nh and 'mov_r15_rax_532b26_u1' in nh and 'mov_rsi_rbx_532b29_u1' in nh and 'mov_edx_eax_532b2c_u1' in nh and 'call_52a220_532b2e_u1' in nh and 'lea_rbx_2efd160_532b33_u1' in nh and 'mov_r13_15fe518_532b3a_u1' in nh and 'mov_r12_rbx_532b41_u1' in nh and 'cmp_r12_r14_532b44_u1' in nh and 'je_532bfb_532b47_u1' in nh and 'test_r12_r12_532b4d_u1' in nh and 'je_532bf8_532b50_u1' in nh and 'mov_r14_mem_rsp10_532b56_u1' in nh and 'mov_r13_mem_rsp40_532b5b_u1' in nh and 'mov_r14_mem_rsp48_532b60_u1' in nh and 'cmp_r13_r14_532b65_u1' in nh and 'je_532bba_532b68_u1' in nh and 'lea_rsi_rsp10_532b6a_u1' in nh and 'mov_rbx_r13_10_532b70_u1' in nh and 'mov_r15_r13_18_532b74_u1' in nh and 'cmp_r15_r11_532b78_u1' in nh and 'je_532b98_532b7b_u1' in nh,'unit enum')
    require('kInit531d60PostBaseRegistryBodyUnitCount' in nh,'unit count')
    require('merge_call_54dc10_532b19_u1_ran' in nh,'merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in nh,'merge mov rbx')
    require('merge_call_54dc20_532b21_u1_ran' in nh,'merge call count')
    require('merge_mov_r15_rax_532b26_u1_ran' in nh,'merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in nh,'merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in nh,'merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in nh,'merge fill call')
    require('merge_lea_rbx_2efd160_532b33_u1_ran' in nh,'merge lea rbx')
    require('merge_mov_r13_15fe518_532b3a_u1_ran' in nh,'merge mov r13')
    require('merge_mov_r12_rbx_532b41_u1_ran' in nh,'merge mov r12')
    require('merge_cmp_r12_r14_532b44_u1_ran' in nh,'merge cmp')
    require('merge_je_532bfb_532b47_u1_ran' in nh,'merge je')
    require('merge_test_r12_r12_532b4d_u1_ran' in nh,'merge test')
    require('merge_je_532bf8_532b50_u1_ran' in nh,'merge je null')
    require('merge_mov_r14_mem_rsp10_532b56_u1_ran' in nh,'merge mov rsp10')
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in nh,'merge mov rsp40')
    require('merge_mov_r14_mem_rsp48_532b60_u1_ran' in nh,'merge mov rsp48')
    require('merge_cmp_r13_r14_532b65_u1_ran' in nh,'merge cmp r13')
    require('merge_je_532bba_532b68_u1_ran' in nh,'merge je 532bba')
    require('merge_lea_rsi_rsp10_532b6a_u1_ran' in nh,'merge lea rsi')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in nh,'merge mov rbx r13')
    require('merge_mov_r15_r13_18_532b74_u1_ran' in nh,'merge mov r15 r13')
    require('merge_cmp_r15_r11_532b78_u1_ran' in nh,'merge cmp r15 r11')
    require('merge_je_532b98_532b7b_u1_ran' in nh,'merge je 532b98')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in NCPP.read_text(),'cpp mov rbx')
    require('init_531d60_post_base_call_54dc20_532b21_u1' in NCPP.read_text(),'cpp call count')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1' in NCPP.read_text(),'cpp mov r15')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1' in NCPP.read_text(),'cpp mov rsi')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1' in NCPP.read_text(),'cpp mov edx')
    require('init_531d60_post_base_call_52a220_532b2e_u1' in NCPP.read_text(),'cpp fill call')
    require('init_531d60_post_base_lea_rbx_2efd160_532b33_u1' in NCPP.read_text(),'cpp lea rbx')
    require('init_531d60_post_base_mov_r13_15fe518_532b3a_u1' in NCPP.read_text(),'cpp mov r13')
    require('init_531d60_post_base_mov_r12_rbx_532b41_u1' in NCPP.read_text(),'cpp mov r12')
    require('init_531d60_post_base_cmp_r12_r14_532b44_u1' in NCPP.read_text(),'cpp cmp')
    require('init_531d60_post_base_je_532bfb_532b47_u1' in NCPP.read_text(),'cpp je')
    require('init_531d60_post_base_test_r12_r12_532b4d_u1' in NCPP.read_text(),'cpp test')
    require('init_531d60_post_base_je_532bf8_532b50_u1' in NCPP.read_text(),'cpp je null')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1' in NCPP.read_text(),'cpp mov rsp10')
    require('init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1' in NCPP.read_text(),'cpp mov rsp40')
    require('init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1' in NCPP.read_text(),'cpp mov rsp48')
    require('init_531d60_post_base_cmp_r13_r14_532b65_u1' in NCPP.read_text(),'cpp cmp r13')
    require('init_531d60_post_base_je_532bba_532b68_u1' in NCPP.read_text(),'cpp je 532bba')
    require('init_531d60_post_base_lea_rsi_rsp10_532b6a_u1' in NCPP.read_text(),'cpp lea rsi')
    require('init_531d60_post_base_mov_rbx_r13_10_532b70_u1' in NCPP.read_text(),'cpp mov rbx r13')
    require('init_531d60_post_base_mov_r15_r13_18_532b74_u1' in NCPP.read_text(),'cpp mov r15 r13')
    require('init_531d60_post_base_cmp_r15_r11_532b78_u1' in NCPP.read_text(),'cpp cmp r15 r11')
    require('init_531d60_post_base_je_532b98_532b7b_u1' in NCPP.read_text(),'cpp je 532b98')
    require('init_531d60_post_base_call_54dc10_532b19_u1' in CALL.read_text(),'call hpp')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1' in MOVRBX.read_text(),'mov rbx hpp')
    require('init_531d60_post_base_call_54dc20_532b21_u1' in CALL20.read_text(),'call count hpp')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1' in MOVR15.read_text(),'mov r15 hpp')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1' in MOVRSI.read_text(),'mov rsi hpp')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1' in MOVEDX.read_text(),'mov edx hpp')
    require('init_531d60_post_base_call_52a220_532b2e_u1' in CALL52.read_text(),'fill call hpp')
    require('init_531d60_post_base_lea_rbx_2efd160_532b33_u1' in LEARBX.read_text(),'lea rbx hpp')
    require('init_531d60_post_base_mov_r13_15fe518_532b3a_u1' in MOVR13.read_text(),'mov r13 hpp')
    require('init_531d60_post_base_mov_r12_rbx_532b41_u1' in MOVR12.read_text(),'mov r12 hpp')
    require('init_531d60_post_base_cmp_r12_r14_532b44_u1' in CMPC.read_text(),'cmp hpp')
    require('init_531d60_post_base_je_532bfb_532b47_u1' in JE.read_text(),'je hpp')
    require('init_531d60_post_base_test_r12_r12_532b4d_u1' in TESTU.read_text(),'test hpp')
    require('init_531d60_post_base_je_532bf8_532b50_u1' in JENULL.read_text(),'je null hpp')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1' in MOVRSP.read_text(),'mov rsp10 hpp')
    require('init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1' in MOVR13RSP40.read_text(),'mov rsp40 hpp')
    require('init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1' in MOVR14RSP48.read_text(),'mov rsp48 hpp')
    require('init_531d60_post_base_cmp_r13_r14_532b65_u1' in CMPC13.read_text(),'cmp r13 hpp')
    require('init_531d60_post_base_je_532bba_532b68_u1' in JEBBA.read_text(),'je 532bba hpp')
    require('init_531d60_post_base_lea_rsi_rsp10_532b6a_u1' in LEARSI.read_text(),'lea rsi hpp')
    require('init_531d60_post_base_mov_rbx_r13_10_532b70_u1' in MOVRBXR13.read_text(),'mov rbx r13 hpp')
    require('init_531d60_post_base_mov_r15_r13_18_532b74_u1' in MOVR15R13.read_text(),'mov r15 r13 hpp')
    require('init_531d60_post_base_cmp_r15_r11_532b78_u1' in CMPC15.read_text(),'cmp r15 r11 hpp')
    require('init_531d60_post_base_je_532b98_532b7b_u1' in JEB98.read_text(),'je 532b98 hpp')
    require('kInit531d60PostBaseMovR14Rax_532b16_U1EndRva = 0x532b19' in MOV14.read_text(),'mov r14 end')
    require('post_base_registry_body_hooks' in SL.read_text(),'slice hpp')
    require('init_531d60_post_base_registry_body' in SC.read_text(),'slice cpp')
    require('merge_call_54dc20_532b21_u1_ran' in SC.read_text(),'slice merge count')
    require('merge_mov_r15_rax_532b26_u1_ran' in SC.read_text(),'slice merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in SC.read_text(),'slice merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in SC.read_text(),'slice merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in SC.read_text(),'slice merge fill call')
    require('merge_lea_rbx_2efd160_532b33_u1_ran' in SC.read_text(),'slice merge lea rbx')
    require('merge_mov_r13_15fe518_532b3a_u1_ran' in SC.read_text(),'slice merge mov r13')
    require('merge_mov_r12_rbx_532b41_u1_ran' in SC.read_text(),'slice merge mov r12')
    require('merge_cmp_r12_r14_532b44_u1_ran' in SC.read_text(),'slice merge cmp')
    require('merge_je_532bfb_532b47_u1_ran' in SC.read_text(),'slice merge je')
    require('merge_test_r12_r12_532b4d_u1_ran' in SC.read_text(),'slice merge test')
    require('merge_je_532bf8_532b50_u1_ran' in SC.read_text(),'slice merge je null')
    require('merge_mov_r14_mem_rsp10_532b56_u1_ran' in SC.read_text(),'slice merge mov rsp10')
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in SC.read_text(),'slice merge mov rsp40')
    require('merge_mov_r14_mem_rsp48_532b60_u1_ran' in SC.read_text(),'slice merge mov rsp48')
    require('merge_cmp_r13_r14_532b65_u1_ran' in SC.read_text(),'slice merge cmp r13')
    require('merge_je_532bba_532b68_u1_ran' in SC.read_text(),'slice merge je 532bba')
    require('merge_lea_rsi_rsp10_532b6a_u1_ran' in SC.read_text(),'slice merge lea rsi')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in SC.read_text(),'slice merge mov rbx r13')
    require('merge_mov_r15_r13_18_532b74_u1_ran' in SC.read_text(),'slice merge mov r15 r13')
    require('merge_cmp_r15_r11_532b78_u1_ran' in SC.read_text(),'slice merge cmp r15 r11')
    require('merge_je_532b98_532b7b_u1_ran' in SC.read_text(),'slice merge je 532b98')
    require('init_531d60_post_base_registry_body.cpp' in CMAKE.read_text(),'cmake body')
    require('init_531d60_post_base_call_54dc10_532b19_u1.cpp' in CMAKE.read_text(),'cmake call')
    require('init_531d60_post_base_mov_rbx_rax_532b1e_u1.cpp' in CMAKE.read_text(),'cmake mov rbx')
    require('init_531d60_post_base_call_54dc20_532b21_u1.cpp' in CMAKE.read_text(),'cmake call count')
    require('init_531d60_post_base_mov_r15_rax_532b26_u1.cpp' in CMAKE.read_text(),'cmake mov r15')
    require('init_531d60_post_base_mov_rsi_rbx_532b29_u1.cpp' in CMAKE.read_text(),'cmake mov rsi')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1.cpp' in CMAKE.read_text(),'cmake mov edx')
    require('init_531d60_post_base_call_52a220_532b2e_u1.cpp' in CMAKE.read_text(),'cmake fill call')
    require('init_531d60_post_base_lea_rbx_2efd160_532b33_u1.cpp' in CMAKE.read_text(),'cmake lea rbx')
    require('init_531d60_post_base_mov_r13_15fe518_532b3a_u1.cpp' in CMAKE.read_text(),'cmake mov r13')
    require('init_531d60_post_base_mov_r12_rbx_532b41_u1.cpp' in CMAKE.read_text(),'cmake mov r12')
    require('init_531d60_post_base_cmp_r12_r14_532b44_u1.cpp' in CMAKE.read_text(),'cmake cmp')
    require('init_531d60_post_base_je_532bfb_532b47_u1.cpp' in CMAKE.read_text(),'cmake je')
    require('init_531d60_post_base_test_r12_r12_532b4d_u1.cpp' in CMAKE.read_text(),'cmake test')
    require('init_531d60_post_base_je_532bf8_532b50_u1.cpp' in CMAKE.read_text(),'cmake je null')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.cpp' in CMAKE.read_text(),'cmake mov rsp10')
    require('init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.cpp' in CMAKE.read_text(),'cmake mov rsp40')
    require('init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.cpp' in CMAKE.read_text(),'cmake mov rsp48')
    require('init_531d60_post_base_cmp_r13_r14_532b65_u1.cpp' in CMAKE.read_text(),'cmake cmp r13')
    require('init_531d60_post_base_je_532bba_532b68_u1.cpp' in CMAKE.read_text(),'cmake je 532bba')
    require('init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.cpp' in CMAKE.read_text(),'cmake lea rsi')
    require('init_531d60_post_base_mov_rbx_r13_10_532b70_u1.cpp' in CMAKE.read_text(),'cmake mov rbx r13')
    require('init_531d60_post_base_mov_r15_r13_18_532b74_u1.cpp' in CMAKE.read_text(),'cmake mov r15 r13')
    require('init_531d60_post_base_cmp_r15_r11_532b78_u1.cpp' in CMAKE.read_text(),'cmake cmp r15 r11')
    require('init_531d60_post_base_je_532b98_532b7b_u1.cpp' in CMAKE.read_text(),'cmake je 532b98')
    require('merge_call_54dc10_532b19_u1_ran' in TEST.read_text(),'test merge call')
    require('merge_mov_rbx_rax_532b1e_u1_ran' in TEST.read_text(),'test merge mov rbx')
    require('merge_call_54dc20_532b21_u1_ran' in TEST.read_text(),'test merge call count')
    require('merge_mov_r15_rax_532b26_u1_ran' in TEST.read_text(),'test merge mov r15')
    require('merge_mov_rsi_rbx_532b29_u1_ran' in TEST.read_text(),'test merge mov rsi')
    require('merge_mov_edx_eax_532b2c_u1_ran' in TEST.read_text(),'test merge mov edx')
    require('merge_call_52a220_532b2e_u1_ran' in TEST.read_text(),'test merge fill call')
    require('merge_lea_rbx_2efd160_532b33_u1_ran' in TEST.read_text(),'test merge lea rbx')
    require('merge_mov_r13_15fe518_532b3a_u1_ran' in TEST.read_text(),'test merge mov r13')
    require('merge_mov_r12_rbx_532b41_u1_ran' in TEST.read_text(),'test merge mov r12')
    require('merge_cmp_r12_r14_532b44_u1_ran' in TEST.read_text(),'test merge cmp')
    require('merge_je_532bfb_532b47_u1_ran' in TEST.read_text(),'test merge je')
    require('merge_test_r12_r12_532b4d_u1_ran' in TEST.read_text(),'test merge test')
    require('merge_je_532bf8_532b50_u1_ran' in TEST.read_text(),'test merge je null')
    require('merge_mov_r14_mem_rsp10_532b56_u1_ran' in TEST.read_text(),'test merge mov rsp10')
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in TEST.read_text(),'test merge mov rsp40')
    require('merge_mov_r14_mem_rsp48_532b60_u1_ran' in TEST.read_text(),'test merge mov rsp48')
    require('merge_cmp_r13_r14_532b65_u1_ran' in TEST.read_text(),'test merge cmp r13')
    require('merge_je_532bba_532b68_u1_ran' in TEST.read_text(),'test merge je 532bba')
    require('merge_lea_rsi_rsp10_532b6a_u1_ran' in TEST.read_text(),'test merge lea rsi')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in TEST.read_text(),'test merge mov rbx r13')
    require('merge_mov_r15_r13_18_532b74_u1_ran' in TEST.read_text(),'test merge mov r15 r13')
    require('merge_cmp_r15_r11_532b78_u1_ran' in TEST.read_text(),'test merge cmp r15 r11')
    require('merge_je_532b98_532b7b_u1_ran' in TEST.read_text(),'test merge je 532b98')
    require('count_from_54dc20' in TEST.read_text(),'test count field')
    require('r15_from_54dc20' in TEST.read_text(),'test r15 field')
    require('rsi_from_rbx' in TEST.read_text(),'test rsi field')
    require('edx_from_eax' in TEST.read_text(),'test edx field')
    require('call_52a220_count' in TEST.read_text(),'test fill call field')
    require('lea_rbx_2efd160_count' in TEST.read_text(),'test lea rbx field')
    require('mov_r13_15fe518_count' in TEST.read_text(),'test mov r13 field')
    require('mov_r12_rbx_count' in TEST.read_text(),'test mov r12 field')
    require('cmp_r12_r14_count' in TEST.read_text(),'test cmp field')
    require('je_532bfb_count' in TEST.read_text(),'test je field')
    require('test_r12_r12_count' in TEST.read_text(),'test test field')
    require('je_532bf8_count' in TEST.read_text(),'test je null field')
    require('mov_r14_mem_rsp10_count' in TEST.read_text(),'test mov rsp10 field')
    require('mov_r13_mem_rsp40_count' in TEST.read_text(),'test mov rsp40 field')
    require('mov_r14_mem_rsp48_count' in TEST.read_text(),'test mov rsp48 field')
    require('je_532bba_count' in TEST.read_text(),'test je 532bba field')
    require('lea_rsi_rsp10_count' in TEST.read_text(),'test lea rsi field')
    require('mov_rbx_r13_10_count' in TEST.read_text(),'test mov rbx r13 field')
    require('cmp_r15_r11_count' in TEST.read_text(),'test cmp r15 r11 field')
    require('je_532b98_count' in TEST.read_text(),'test je 532b98 field')
    require('rsi_from_rsp10_lea' in TEST.read_text(),'test rsi from lea field')
    require('kInit531d60PostBaseRegistryBodyUnitCount == 24' in TEST.read_text(),'test unit count 24')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b7d' in REG.read_text(),'reg pin')
    require('merge_cmp_r15_r11_532b78_u1_ran' in REG.read_text(),'reg merge cmp r15 r11')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in REG.read_text(),'reg merge mov rbx r13')
    require('merge_mov_r14_mem_rsp10_532b56_u1_ran' in REG.read_text(),'reg merge mov rsp10')
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in REG.read_text(),'reg merge mov rsp40')
    require('merge_mov_r14_mem_rsp48_532b60_u1_ran' in REG.read_text(),'reg merge mov rsp48')
    require('merge_cmp_r13_r14_532b65_u1_ran' in REG.read_text(),'reg merge cmp r13')
    require('merge_je_532bba_532b68_u1_ran' in REG.read_text(),'reg merge je')
    require('merge_je_532b98_532b7b_u1_ran' in REG.read_text(),'reg merge je 532b98')
    print('OK check_f531d60_post_base_registry_body end=0x532b7d units=24')
    return 0
if __name__=='__main__': raise SystemExit(main())
