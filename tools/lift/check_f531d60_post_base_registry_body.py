#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B19, 218, 0x532BF3
REGION_SHA256 = "75608c15391991ecc64eea342d239c0d6c0f04deab0d336858f301f479ce5be5"
PEEL_MOV_RAX_RDI = bytes.fromhex("488b07")
PEEL_MOV_R13_8_RAX = bytes.fromhex("48894508")
PEEL_LEA = bytes.fromhex("488d742410")
PEEL_MOV = bytes.fromhex("498b5d10")
PEEL_MOV15 = bytes.fromhex("4d8b7d18")
PEEL_CMP15 = bytes.fromhex("4c39fb")
PEEL_JE = bytes.fromhex("741b")
PEEL_NOP = bytes.fromhex("0f1f00")
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
    (0x532B7D,3,"019f07bfe6bd5747f9f3b65f5a095db04a0418d5f11c6feec58eaff709860b66"),
    (0x532B80,4,"853b6f9c5a218c7198d530517eeca439dd5023078631567f13aab756301234c7"),
    (0x532B84,4,"aad5e1d021e7586cfdfbfaa72a2576217c02e8c17b80e6eaacdeb0f529487780"),
    (0x532B88,3,"22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96"),
    (0x532B8B,4,"16dad117ec7bdabdccd3671227621b434a859b9e2dec0083125ab59e39ac380d"),
    (0x532B8F,3,"164f94ed8cad6f9ed0fd1a687fcf323408d9c843015d258e5bbcc11874154017"),
    (0x532B92,2,"437b263352f51d6dc454cc85816e0ab7b8c797936e198534e1790a004efdb56f"),
    (0x532B94,4,"51cf1bb258add2501b662c9a241ee79e38a7cf232aaae570ce5e3ce40dd41e42"),
    (0x532B98,3,"2cf4a4a2a0eead8a720a70b7cde1047cbe8bd81262905504c08d4484e120b121"),
    (0x532B9B,2,"ce7b207f44bdfcb4050755ac2969bf23f44a92ef9c1139df06caf97b9b3aedd1"),
    (0x532B9D,3,"0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab"),
    (0x532BA0,5,"ba50bb61223083644ce96ef494e8c15caeb546653d72a138cf09727a8771622c"),
    (0x532BA5,4,"87f6500906a263197172aa44232fecf534880a8a2de0ece2f7b80cd8759cb743"),
    (0x532BA9,3,"e1fc26b5bab7014dc3b5f125c9dbd3f780ec07a8ab595b815acdb4537e241a62"),
    (0x532BAC,2,"4532ba8a8cb1ac213964c95496a5a8535246ca3b933cd8cf2b9c368d826e78e2"),
    (0x532BAE,5,"5161bd7ca4697196a7ed118df5923758c9c21e074bbe80a1e68063cc42b866e5"),
    (0x532BB3,7,"8163125859f1d4162340a7a7a06772541f61c5f19b6918952884eca7a28904b7"),
    (0x532BBA,5,"0832b50b1127d182d74804dfe1d0e7c47ba24a7959ec75952e83dac10ea85388"),
    (0x532BBF,3,"e10a2bc66203e957df52f8a9aed93e12244edee28cf26cd938a5b83aaac249d5"),
    (0x532BC2,2,"ce7b207f44bdfcb4050755ac2969bf23f44a92ef9c1139df06caf97b9b3aedd1"),
    (0x532BC4,3,"93a4c731bcc86650db8525737b9ea22d2b79229d2c48381bf6f461720afd0e4e"),
    (0x532BC7,5,"b8dc75fbe64aecd9dc63b6c2dc6714aff57f7699f8427fa7f9e9bcb0a1c7ad57"),
    (0x532BCC,5,"32218b472edbb3cded85cbe397df6d6972f153b7f576d5e0b4658a2c0474711c"),
    (0x532BD1,3,"78126bd6ec0824c8cdd3ca1d994e12bfa11fcc75666a04538c9c74e495761e6c"),
    (0x532BD4,2,"58b821595e7051b1409a38f7c48f7b1153019dce5f9e8b78241111e6dd4a67cb"),
    (0x532BD6,3,"81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409"),
    (0x532BD9,5,"28db90ec932a09df02988910700a21d31cb6658818352c4e25c89c68dc8e7542"),
    (0x532BDE,3,"81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409"),
    (0x532BE1,5,"7f367300693a91508777eb984165a5ee775ab703f8ddd95389d20f185269b165"),
    (0x532BE6,5,"201be5646f38f1b81ff370448d95d65ff330b608b531be7015cc5d3764f5e6b7"),
    (0x532BEB,5,"a4830c6c94910c61fa171c7aa1e9beca321fb35d44fa9cbc9d7346842e828bc2"),
    (0x532BF0,3,"83281286e4ef4d5b4f67e1d5b6134a722648eaacfe7e10238a648847273f37c8"),
]
MOVRAXRDI=ROOT/"native/core/init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"
MOVR138=ROOT/"native/core/init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"
MOVRAXBDE=ROOT/"native/core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp"
SUBEAX=ROOT/"native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"
JE532BF0=ROOT/"native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp"
CALL170C0BEB=ROOT/"native/core/init_531d60_post_base_call_170c0_532beb_u1.hpp"
MOVRDIR12=ROOT/"native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp"
MOVRDIM12=ROOT/"native/core/init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.hpp"
TESTRDI=ROOT/"native/core/init_531d60_post_base_test_rdi_rdi_532bd1_u1.hpp"
JE532BDB=ROOT/"native/core/init_531d60_post_base_je_532bdb_532bd4_u1.hpp"
CALL170C0BC7=ROOT/"native/core/init_531d60_post_base_call_170c0_532bc7_u1.hpp"
MOVR14RSP532BBF=ROOT/"native/core/init_531d60_post_base_cmp_r13_r14_532bbf_u1.hpp"
JEBCC=ROOT/"native/core/init_531d60_post_base_je_532bcc_532bc2_u1.hpp"
MOVR13R15=ROOT/"native/core/init_531d60_post_base_mov_r13_r15_532bc4_u1.hpp"
CMP532BA9=ROOT/"native/core/init_531d60_post_base_cmp_r13_r14_532ba9_u1.hpp"
JNE532BAC=ROOT/"native/core/init_531d60_post_base_jne_532b70_532bac_u1.hpp"
MOVR13M12=ROOT/"native/core/init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1.hpp"
LEARBX532BB3=ROOT/"native/core/init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.hpp"
MOVR14RSP532BBA=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.hpp"
ADDR1330=ROOT/"native/core/init_531d60_post_base_add_r13_30_532ba5_u1.hpp"
CALL170C0=ROOT/"native/core/init_531d60_post_base_call_170c0_532ba0_u1.hpp"
MOVRDIRBX532=ROOT/"native/core/init_531d60_post_base_mov_rdi_rbx_532b9d_u1.hpp"
JE532BA5=ROOT/"native/core/init_531d60_post_base_je_532ba5_532b9b_u1.hpp"
JNE532=ROOT/"native/core/init_531d60_post_base_jne_532b80_532b92_u1.hpp"
MOVRBXR1394=ROOT/"native/core/init_531d60_post_base_mov_rbx_r13_10_532b94_u1.hpp"
TESTRBX=ROOT/"native/core/init_531d60_post_base_test_rbx_rbx_532b98_u1.hpp"
VCALL18=ROOT/"native/core/init_531d60_post_base_vcall18_532b88_u1.hpp"
ADDRBX=ROOT/"native/core/init_531d60_post_base_add_rbx_58_532b8b_u1.hpp"
CMPC15RBX=ROOT/"native/core/init_531d60_post_base_cmp_r15_rbx_532b8f_u1.hpp"
LEARDI=ROOT/"native/core/init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.hpp"
MOVRAX=ROOT/"native/core/init_531d60_post_base_mov_rax_rbx_48_532b80_u1.hpp"
NOP532=ROOT/"native/core/init_531d60_post_base_nop_532b7d_u1.hpp"
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
    data[0x532B7B-START:0x532B7D-START]=PEEL_JE
    data[0x532B7D-START:0x532B80-START]=PEEL_NOP
    data[0x532BD6-START:0x532BD9-START]=PEEL_MOV_RAX_RDI
    data[0x532BD9-START:0x532BDE-START]=PEEL_MOV_R13_8_RAX
    data[0x532BDE-START:0x532BE1-START]=PEEL_MOV_RAX_RDI
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
    if rva==0x532B7D and sz==3:
        return PEEL_NOP
    if rva==0x532BD6 and sz==3:
        return PEEL_MOV_RAX_RDI
    if rva==0x532BDE and sz==3:
        return PEEL_MOV_RAX_RDI
    return img.read(rva,sz)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes(); require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require(len(UNITS)==56,'units')
    require(hashlib.sha256(patched_region(img)).hexdigest()==REGION_SHA256,'region')
    for rva,sz,sha in UNITS:
        require(hashlib.sha256(unit_bytes(img,rva,sz)).hexdigest()==sha,f'unit@{rva:#x}')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    b=img.read(FILL_RVA,5); require(b[0]==0xE8,'fill call')
    rel=struct.unpack('<i',b[1:5])[0]; require(FILL_RVA+5+rel==FILL_TARGET,'fill 0x52a220')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryBodyEndRva = 0x532bf3' in nh,'end')
    require('call_54dc10_532b19_u1' in nh and 'mov_rbx_rax_532b1e_u1' in nh and 'call_54dc20_532b21_u1' in nh and 'mov_r15_rax_532b26_u1' in nh and 'mov_rsi_rbx_532b29_u1' in nh and 'mov_edx_eax_532b2c_u1' in nh and 'call_52a220_532b2e_u1' in nh and 'lea_rbx_2efd160_532b33_u1' in nh and 'mov_r13_15fe518_532b3a_u1' in nh and 'mov_r12_rbx_532b41_u1' in nh and 'cmp_r12_r14_532b44_u1' in nh and 'je_532bfb_532b47_u1' in nh and 'test_r12_r12_532b4d_u1' in nh and 'je_532bf8_532b50_u1' in nh and 'mov_r14_mem_rsp10_532b56_u1' in nh and 'mov_r13_mem_rsp40_532b5b_u1' in nh and 'mov_r14_mem_rsp48_532b60_u1' in nh and 'cmp_r13_r14_532b65_u1' in nh and 'je_532bba_532b68_u1' in nh and 'lea_rsi_rsp10_532b6a_u1' in nh and 'mov_rbx_r13_10_532b70_u1' in nh and 'mov_r15_r13_18_532b74_u1' in nh and 'cmp_r15_r11_532b78_u1' in nh and 'je_532b98_532b7b_u1' in nh and 'nop_532b7d_u1' in nh and 'mov_rax_rbx_48_532b80_u1' in nh and 'lea_rdi_rbx_28_532b84_u1' in nh and 'vcall18_532b88_u1' in nh and 'add_rbx_58_532b8b_u1' in nh and 'cmp_r15_rbx_532b8f_u1' in nh and 'jne_532b80_532b92_u1' in nh and 'mov_rbx_r13_10_532b94_u1' in nh and 'test_rbx_rbx_532b98_u1' in nh and 'je_532ba5_532b9b_u1' in nh and 'mov_rdi_rbx_532b9d_u1' in nh and 'cmp_r13_r14_532bbf_u1' in nh and 'je_532bcc_532bc2_u1' in nh and 'mov_r13_r15_532bc4_u1' in nh and 'call_170c0_532bc7_u1' in nh and 'mov_rdi_mem_r12_20_532bcc_u1' in nh and 'test_rdi_rdi_532bd1_u1' in nh and 'je_532bdb_532bd4_u1' in nh and 'mov_rax_rdi_mem_532bd6_u1' in nh and 'call_170c0_532beb_u1' in nh and 'mov_rdi_r12_532bf0_u1' in nh,'unit enum')
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
    require('merge_nop_532b7d_u1_ran' in nh,'merge nop 532b7d')
    require('merge_mov_rax_rbx_48_532b80_u1_ran' in nh,'merge mov rax rbx 48')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in nh,'merge lea rdi rbx 28')
    require('merge_vcall18_532b88_u1_ran' in nh,'merge vcall18 532b88')
    require('merge_add_rbx_58_532b8b_u1_ran' in nh,'merge add rbx 58 532b8b')
    require('merge_cmp_r15_rbx_532b8f_u1_ran' in nh,'merge cmp r15 rbx 532b8f')
    require('merge_jne_532b80_532b92_u1_ran' in nh,'merge jne 532b80 532b92')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in nh,'merge mov rbx r13 532b94')
    require('merge_test_rbx_rbx_532b98_u1_ran' in nh,'merge test rbx rbx 532b98')
    require('merge_je_532ba5_532b9b_u1_ran' in nh,'merge je 532ba5 532b9b')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in nh,'merge mov rdi rbx 532b9d')
    require('merge_mov_r14_mem_rsp10_532bba_u1_ran' in nh,'merge mov r14 rsp10 532bba')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in nh,'merge cmp r13 r14 532bbf')
    require('merge_je_532bcc_532bc2_u1_ran' in nh,'merge je 532bcc 532bc2')
    require('merge_mov_r13_r15_532bc4_u1_ran' in nh,'merge mov r13 r15 532bc4')
    require('merge_call_170c0_532bc7_u1_ran' in nh,'merge call 170c0 532bc7')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in nh,'merge mov rdi mem r12 20 532bcc')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in nh,'merge test rdi rdi 532bd1')
    require('merge_je_532bdb_532bd4_u1_ran' in nh,'merge je 532bdb 532bd4')
    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in nh,'merge mov rax rdi mem 532bd6')
    require('merge_mov_r13_8_rax_532bd9_u1_ran' in nh,'merge mov r13 8 rax 532bd9')
    require('merge_mov_rax_rdi_mem_532bde_u1_ran' in nh,'merge mov rax rdi mem 532bde')
    require('merge_sub_eax_010cb932_532be1_u1_ran' in nh,'merge sub eax 532be1')
    require('merge_je_532bf0_532be6_u1_ran' in nh,'merge je 532bf0 532be6')
    require('merge_call_170c0_532beb_u1_ran' in nh,'merge call 170c0 532beb')
    require('merge_mov_rdi_r12_532bf0_u1_ran' in nh,'merge mov rdi r12 532bf0')
    require('mov_rax_rdi_mem_532bd6_u1' in nh,'unit enum mov rax rdi mem 532bd6')
    require('mov_r13_8_rax_532bd9_u1' in nh,'unit enum mov r13 8 rax 532bd9')
    require('mov_rax_rdi_mem_532bde_u1' in nh,'unit enum mov rax rdi mem 532bde')
    require('sub_eax_010cb932_532be1_u1' in nh,'unit enum sub eax 532be1')
    require('je_532bf0_532be6_u1' in nh,'unit enum je 532bf0 532be6')
    require('call_170c0_532beb_u1' in nh,'unit enum call 170c0 532beb')
    require('mov_rdi_r12_532bf0_u1' in nh,'unit enum mov rdi r12 532bf0')
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
    require('init_531d60_post_base_nop_532b7d_u1' in NCPP.read_text(),'cpp nop 532b7d')
    require('init_531d60_post_base_mov_rax_rbx_48_532b80_u1' in NCPP.read_text(),'cpp mov rax rbx 48')
    require('init_531d60_post_base_lea_rdi_rbx_28_532b84_u1' in NCPP.read_text(),'cpp lea rdi rbx 28')
    require('init_531d60_post_base_vcall18_532b88_u1' in NCPP.read_text(),'cpp vcall18 532b88')
    require('init_531d60_post_base_add_rbx_58_532b8b_u1' in NCPP.read_text(),'cpp add rbx 58 532b8b')
    require('init_531d60_post_base_cmp_r15_rbx_532b8f_u1' in NCPP.read_text(),'cpp cmp r15 rbx 532b8f')
    require('init_531d60_post_base_jne_532b80_532b92_u1' in NCPP.read_text(),'cpp jne 532b80 532b92')
    require('init_531d60_post_base_mov_rbx_r13_10_532b94_u1' in NCPP.read_text(),'cpp mov rbx r13 532b94')
    require('init_531d60_post_base_test_rbx_rbx_532b98_u1' in NCPP.read_text(),'cpp test rbx rbx 532b98')
    require('init_531d60_post_base_je_532ba5_532b9b_u1' in NCPP.read_text(),'cpp je 532ba5 532b9b')
    require('init_531d60_post_base_mov_rdi_rbx_532b9d_u1' in NCPP.read_text(),'cpp mov rdi rbx 532b9d')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1' in NCPP.read_text(),'cpp mov r14 rsp10 532bba')
    require('init_531d60_post_base_cmp_r13_r14_532bbf_u1' in NCPP.read_text(),'cpp cmp r13 r14 532bbf')
    require('init_531d60_post_base_je_532bcc_532bc2_u1' in NCPP.read_text(),'cpp je 532bcc 532bc2')
    require('init_531d60_post_base_mov_r13_r15_532bc4_u1' in NCPP.read_text(),'cpp mov r13 r15 532bc4')
    require('init_531d60_post_base_call_170c0_532bc7_u1' in NCPP.read_text(),'cpp call 170c0 532bc7')
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
    require('init_531d60_post_base_nop_532b7d_u1' in NOP532.read_text(),'nop 532b7d hpp')
    require('init_531d60_post_base_mov_rax_rbx_48_532b80_u1' in MOVRAX.read_text(),'mov rax rbx 48 hpp')
    require('init_531d60_post_base_lea_rdi_rbx_28_532b84_u1' in LEARDI.read_text(),'lea rdi rbx 28 hpp')
    require('init_531d60_post_base_vcall18_532b88_u1' in VCALL18.read_text(),'vcall18 532b88 hpp')
    require('init_531d60_post_base_add_rbx_58_532b8b_u1' in ADDRBX.read_text(),'add rbx 58 532b8b hpp')
    require('init_531d60_post_base_cmp_r15_rbx_532b8f_u1' in CMPC15RBX.read_text(),'cmp r15 rbx 532b8f hpp')
    require('init_531d60_post_base_jne_532b80_532b92_u1' in JNE532.read_text(),'jne 532b80 532b92 hpp')
    require('init_531d60_post_base_mov_rbx_r13_10_532b94_u1' in MOVRBXR1394.read_text(),'mov rbx r13 532b94 hpp')
    require('init_531d60_post_base_test_rbx_rbx_532b98_u1' in TESTRBX.read_text(),'test rbx rbx 532b98 hpp')
    require('init_531d60_post_base_je_532ba5_532b9b_u1' in JE532BA5.read_text(),'je 532ba5 532b9b hpp')
    require('init_531d60_post_base_mov_rdi_rbx_532b9d_u1' in MOVRDIRBX532.read_text(),'mov rdi rbx 532b9d hpp')
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
    require('merge_nop_532b7d_u1_ran' in SC.read_text(),'slice merge nop 532b7d')
    require('merge_mov_rax_rbx_48_532b80_u1_ran' in SC.read_text(),'slice merge mov rax rbx 48')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in SC.read_text(),'slice merge lea rdi rbx 28')
    require('merge_vcall18_532b88_u1_ran' in SC.read_text(),'slice merge vcall18 532b88')
    require('merge_add_rbx_58_532b8b_u1_ran' in SC.read_text(),'slice merge add rbx 58 532b8b')
    require('merge_cmp_r15_rbx_532b8f_u1_ran' in SC.read_text(),'slice merge cmp r15 rbx 532b8f')
    require('merge_jne_532b80_532b92_u1_ran' in SC.read_text(),'slice merge jne 532b80 532b92')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in SC.read_text(),'slice merge mov rbx r13 532b94')
    require('merge_test_rbx_rbx_532b98_u1_ran' in SC.read_text(),'slice merge test rbx rbx 532b98')
    require('merge_je_532ba5_532b9b_u1_ran' in SC.read_text(),'slice merge je 532ba5 532b9b')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in SC.read_text(),'slice merge mov rdi rbx 532b9d')
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
    require('init_531d60_post_base_nop_532b7d_u1.cpp' in CMAKE.read_text(),'cmake nop 532b7d')
    require('init_531d60_post_base_mov_rax_rbx_48_532b80_u1.cpp' in CMAKE.read_text(),'cmake mov rax rbx 48')
    require('init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.cpp' in CMAKE.read_text(),'cmake lea rdi rbx 28')
    require('init_531d60_post_base_vcall18_532b88_u1.cpp' in CMAKE.read_text(),'cmake vcall18 532b88')
    require('init_531d60_post_base_add_rbx_58_532b8b_u1.cpp' in CMAKE.read_text(),'cmake add rbx 58 532b8b')
    require('init_531d60_post_base_cmp_r15_rbx_532b8f_u1.cpp' in CMAKE.read_text(),'cmake cmp r15 rbx 532b8f')
    require('init_531d60_post_base_jne_532b80_532b92_u1.cpp' in CMAKE.read_text(),'cmake jne 532b80 532b92')
    require('init_531d60_post_base_mov_rbx_r13_10_532b94_u1.cpp' in CMAKE.read_text(),'cmake mov rbx r13 532b94')
    require('init_531d60_post_base_test_rbx_rbx_532b98_u1.cpp' in CMAKE.read_text(),'cmake test rbx rbx 532b98')
    require('init_531d60_post_base_je_532ba5_532b9b_u1.cpp' in CMAKE.read_text(),'cmake je 532ba5 532b9b')
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
    require('merge_nop_532b7d_u1_ran' in TEST.read_text(),'test merge nop 532b7d')
    require('merge_mov_rax_rbx_48_532b80_u1_ran' in TEST.read_text(),'test merge mov rax rbx 48')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in TEST.read_text(),'test merge lea rdi rbx 28')
    require('merge_vcall18_532b88_u1_ran' in TEST.read_text(),'test merge vcall18 532b88')
    require('merge_add_rbx_58_532b8b_u1_ran' in TEST.read_text(),'test merge add rbx 58 532b8b')
    require('merge_cmp_r15_rbx_532b8f_u1_ran' in TEST.read_text(),'test merge cmp r15 rbx 532b8f')
    require('merge_jne_532b80_532b92_u1_ran' in TEST.read_text(),'test merge jne 532b80 532b92')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in TEST.read_text(),'test merge mov rbx r13 532b94')
    require('merge_test_rbx_rbx_532b98_u1_ran' in TEST.read_text(),'test merge test rbx rbx 532b98')
    require('merge_je_532ba5_532b9b_u1_ran' in TEST.read_text(),'test merge je 532ba5 532b9b')
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
    require('nop_532b7d_count' in TEST.read_text(),'test nop 532b7d field')
    require('mov_rax_rbx_48_count' in TEST.read_text(),'test mov rax rbx 48 field')
    require('lea_rdi_rbx_28_count' in TEST.read_text(),'test lea rdi rbx 28 field')
    require('call_vslot18_532b88_count' in TEST.read_text(),'test vcall18 532b88 field')
    require('add_rbx_58_count' in TEST.read_text(),'test add rbx 58 field')
    require('cmp_r15_rbx_count' in TEST.read_text(),'test cmp r15 rbx field')
    require('jne_532b80_count' in TEST.read_text(),'test jne 532b80 field')
    require('mov_rbx_r13_10_532b94_count' in TEST.read_text(),'test mov rbx r13 532b94 field')
    require('test_rbx_rbx_count' in TEST.read_text(),'test test rbx rbx field')
    require('je_532ba5_count' in TEST.read_text(),'test je 532ba5 field')
    require('rsi_from_rsp10_lea' in TEST.read_text(),'test rsi from lea field')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in REG.read_text(),'reg merge cmp r13 r14 532bbf')
    require('merge_cmp_r15_r11_532b78_u1_ran' in REG.read_text(),'reg merge cmp r15 r11')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in REG.read_text(),'reg merge mov rbx r13')
    require('merge_mov_r14_mem_rsp10_532b56_u1_ran' in REG.read_text(),'reg merge mov rsp10')
    require('merge_mov_r13_mem_rsp40_532b5b_u1_ran' in REG.read_text(),'reg merge mov rsp40')
    require('merge_mov_r14_mem_rsp48_532b60_u1_ran' in REG.read_text(),'reg merge mov rsp48')
    require('merge_cmp_r13_r14_532b65_u1_ran' in REG.read_text(),'reg merge cmp r13')
    require('merge_je_532bba_532b68_u1_ran' in REG.read_text(),'reg merge je')
    require('merge_je_532b98_532b7b_u1_ran' in REG.read_text(),'reg merge je 532b98')
    require('merge_nop_532b7d_u1_ran' in REG.read_text(),'reg merge nop 532b7d')
    require('merge_mov_rax_rbx_48_532b80_u1_ran' in REG.read_text(),'reg merge mov rax rbx 48')
    require('merge_lea_rdi_rbx_28_532b84_u1_ran' in REG.read_text(),'reg merge lea rdi rbx 28')
    require('merge_vcall18_532b88_u1_ran' in REG.read_text(),'reg merge vcall18 532b88')
    require('merge_add_rbx_58_532b8b_u1_ran' in REG.read_text(),'reg merge add rbx 58 532b8b')
    require('merge_cmp_r15_rbx_532b8f_u1_ran' in REG.read_text(),'reg merge cmp r15 rbx 532b8f')
    require('merge_jne_532b80_532b92_u1_ran' in REG.read_text(),'reg merge jne 532b80 532b92')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in REG.read_text(),'reg merge mov rbx r13 532b94')
    require('merge_test_rbx_rbx_532b98_u1_ran' in REG.read_text(),'reg merge test rbx rbx 532b98')
    require('merge_je_532ba5_532b9b_u1_ran' in REG.read_text(),'reg merge je 532ba5 532b9b')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in REG.read_text(),'reg merge mov rdi rbx 532b9d')
    require('merge_call_170c0_532ba0_u1_ran' in REG.read_text(),'reg merge call 170c0 532ba0')
    require('merge_add_r13_30_532ba5_u1_ran' in REG.read_text(),'reg merge add r13 30 532ba5')
    require('merge_cmp_r13_r14_532ba9_u1_ran' in REG.read_text(),'reg merge cmp r13 r14 532ba9')
    require('merge_mov_rdi_rbx_532b9d_u1_ran' in TEST.read_text(),'test merge mov rdi rbx 532b9d')
    require('merge_mov_r14_mem_rsp10_532bba_u1_ran' in TEST.read_text(),'test merge mov r14 rsp10 532bba')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in TEST.read_text(),'test merge cmp r13 r14 532bbf')
    require('cmp_r13_r14_532bbf_count' in TEST.read_text(),'test cmp r13 r14 532bbf field')
    require('merge_call_170c0_532ba0_u1_ran' in TEST.read_text(),'test merge call 170c0 532ba0')
    require('merge_add_r13_30_532ba5_u1_ran' in TEST.read_text(),'test merge add r13 30 532ba5')
    require('merge_cmp_r13_r14_532ba9_u1_ran' in TEST.read_text(),'test merge cmp r13 r14 532ba9')
    require('mov_rdi_rbx_532b9d_count' in TEST.read_text(),'test mov rdi rbx 532b9d field')
    require('call_170c0_count' in TEST.read_text(),'test call 170c0 field')
    require('add_r13_30_532ba5_count' in TEST.read_text(),'test add r13 30 532ba5 field')
    require('cmp_r13_r14_532ba9_count' in TEST.read_text(),'test cmp r13 r14 532ba9 field')
    require('init_531d60_post_base_mov_rdi_rbx_532b9d_u1.cpp' in CMAKE.read_text(),'cmake mov rdi rbx 532b9d')
    require('init_531d60_post_base_call_170c0_532ba0_u1.cpp' in CMAKE.read_text(),'cmake call 170c0 532ba0')
    require('init_531d60_post_base_add_r13_30_532ba5_u1.cpp' in CMAKE.read_text(),'cmake add r13 30 532ba5')
    require('init_531d60_post_base_cmp_r13_r14_532ba9_u1.cpp' in CMAKE.read_text(),'cmake cmp r13 r14 532ba9')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.cpp' in CMAKE.read_text(),'cmake mov r14 rsp10 532bba')
    require('init_531d60_post_base_cmp_r13_r14_532bbf_u1.cpp' in CMAKE.read_text(),'cmake cmp r13 r14 532bbf')
    require('init_531d60_post_base_call_170c0_532ba0_u1' in CALL170C0.read_text(),'call 170c0 hpp')
    require('init_531d60_post_base_add_r13_30_532ba5_u1' in ADDR1330.read_text(),'add r13 30 532ba5 hpp')
    require('init_531d60_post_base_cmp_r13_r14_532ba9_u1' in CMP532BA9.read_text(),'cmp r13 r14 532ba9 hpp')
    require('init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1' in MOVR14RSP532BBA.read_text(),'mov r14 rsp10 532bba hpp')
    require('init_531d60_post_base_cmp_r13_r14_532bbf_u1' in MOVR14RSP532BBF.read_text(),'cmp r13 r14 532bbf hpp')
    require('init_531d60_post_base_je_532bcc_532bc2_u1' in JEBCC.read_text(),'je 532bcc 532bc2 hpp')
    require('init_531d60_post_base_mov_r13_r15_532bc4_u1' in MOVR13R15.read_text(),'mov r13 r15 532bc4 hpp')
    require('init_531d60_post_base_call_170c0_532bc7_u1' in CALL170C0BC7.read_text(),'call 170c0 532bc7 hpp')
    require('init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1' in MOVRDIM12.read_text(),'mov rdi mem r12 20 532bcc hpp')
    require('init_531d60_post_base_test_rdi_rdi_532bd1_u1' in TESTRDI.read_text(),'test rdi rdi 532bd1 hpp')
    require('init_531d60_post_base_je_532bdb_532bd4_u1' in JE532BDB.read_text(),'je 532bdb 532bd4 hpp')
    require('init_531d60_post_base_call_170c0_532ba0_u1' in NCPP.read_text(),'cpp call 170c0 532ba0')
    require('init_531d60_post_base_add_r13_30_532ba5_u1' in NCPP.read_text(),'cpp add r13 30 532ba5')
    require('init_531d60_post_base_cmp_r13_r14_532ba9_u1' in NCPP.read_text(),'cpp cmp r13 r14 532ba9')
    require('merge_call_170c0_532ba0_u1_ran' in nh,'merge call 170c0 532ba0')
    require('merge_add_r13_30_532ba5_u1_ran' in nh,'merge add r13 30 532ba5')
    require('merge_cmp_r13_r14_532ba9_u1_ran' in nh,'merge cmp r13 r14 532ba9')
    require('merge_call_170c0_532ba0_u1_ran' in SC.read_text(),'slice merge call 170c0 532ba0')
    require('merge_add_r13_30_532ba5_u1_ran' in SC.read_text(),'slice merge add r13 30 532ba5')
    require('merge_cmp_r13_r14_532ba9_u1_ran' in SC.read_text(),'slice merge cmp r13 r14 532ba9')
    require('merge_mov_r14_mem_rsp10_532bba_u1_ran' in SC.read_text(),'slice merge mov r14 rsp10 532bba')
    require('merge_cmp_r13_r14_532bbf_u1_ran' in SC.read_text(),'slice merge cmp r13 r14 532bbf')
    require('merge_je_532bcc_532bc2_u1_ran' in SC.read_text(),'slice merge je 532bcc 532bc2')
    require('merge_mov_r13_r15_532bc4_u1_ran' in SC.read_text(),'slice merge mov r13 r15 532bc4')
    require('merge_call_170c0_532bc7_u1_ran' in SC.read_text(),'slice merge call 170c0 532bc7')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in SC.read_text(),'slice merge mov rdi mem r12 20 532bcc')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in SC.read_text(),'slice merge test rdi rdi 532bd1')
    require('merge_je_532bdb_532bd4_u1_ran' in SC.read_text(),'slice merge je 532bdb 532bd4')
    require('init_531d60_post_base_je_532bcc_532bc2_u1.cpp' in CMAKE.read_text(),'cmake je 532bcc 532bc2')
    require('init_531d60_post_base_mov_r13_r15_532bc4_u1.cpp' in CMAKE.read_text(),'cmake mov r13 r15 532bc4')
    require('init_531d60_post_base_call_170c0_532bc7_u1.cpp' in CMAKE.read_text(),'cmake call 170c0 532bc7')
    require('init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.cpp' in CMAKE.read_text(),'cmake mov rdi mem r12 20 532bcc')
    require('init_531d60_post_base_test_rdi_rdi_532bd1_u1.cpp' in CMAKE.read_text(),'cmake test rdi rdi 532bd1')
    require('init_531d60_post_base_je_532bdb_532bd4_u1.cpp' in CMAKE.read_text(),'cmake je 532bdb 532bd4')
    require('merge_je_532bcc_532bc2_u1_ran' in TEST.read_text(),'test merge je 532bcc 532bc2')
    require('merge_mov_r13_r15_532bc4_u1_ran' in TEST.read_text(),'test merge mov r13 r15 532bc4')
    require('merge_call_170c0_532bc7_u1_ran' in TEST.read_text(),'test merge call 170c0 532bc7')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in TEST.read_text(),'test merge mov rdi mem r12 20 532bcc')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in TEST.read_text(),'test merge test rdi rdi 532bd1')
    require('merge_je_532bdb_532bd4_u1_ran' in TEST.read_text(),'test merge je 532bdb 532bd4')
    require('je_532bcc_count' in TEST.read_text(),'test je 532bcc field')
    require('mov_r13_r15_532bc4_count' in TEST.read_text(),'test mov r13 r15 532bc4 field')
    require('call_170c0_532bc7_count' in TEST.read_text(),'test call 170c0 532bc7 field')
    require('mov_rdi_mem_r12_20_532bcc_count' in TEST.read_text(),'test mov rdi mem r12 20 532bcc field')
    require('test_rdi_rdi_count' in TEST.read_text(),'test test rdi rdi field')
    require('je_532bdb_count' in TEST.read_text(),'test je 532bdb field')
    require('kInit531d60PostBaseRegistryBodyUnitCount == 56' in TEST.read_text(),'test unit count 56')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bf3' in REG.read_text(),'reg pin')
    require('merge_je_532bcc_532bc2_u1_ran' in REG.read_text(),'reg merge je 532bcc 532bc2')
    require('merge_mov_r13_r15_532bc4_u1_ran' in REG.read_text(),'reg merge mov r13 r15 532bc4')
    require('merge_call_170c0_532bc7_u1_ran' in REG.read_text(),'reg merge call 170c0 532bc7')
    require('merge_mov_rdi_mem_r12_20_532bcc_u1_ran' in REG.read_text(),'reg merge mov rdi mem r12 20 532bcc')
    require('merge_test_rdi_rdi_532bd1_u1_ran' in REG.read_text(),'reg merge test rdi rdi 532bd1')
    require('merge_je_532bdb_532bd4_u1_ran' in REG.read_text(),'reg merge je 532bdb 532bd4')
    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')
    require('merge_mov_r13_8_rax_532bd9_u1_ran' in REG.read_text(),'reg merge mov r13 8 rax 532bd9')
    require('merge_mov_rax_rdi_mem_532bde_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bde')
    require('merge_sub_eax_010cb932_532be1_u1_ran' in REG.read_text(),'reg merge sub eax 532be1')
    require('merge_je_532bf0_532be6_u1_ran' in REG.read_text(),'reg merge je 532bf0 532be6')
    require('merge_call_170c0_532beb_u1_ran' in REG.read_text(),'reg merge call 170c0 532beb')
    require('merge_mov_rdi_r12_532bf0_u1_ran' in REG.read_text(),'reg merge mov rdi r12 532bf0')
    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')
    require('init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1' in NCPP.read_text(),'cpp mov rdi mem r12 20 532bcc')
    require('init_531d60_post_base_test_rdi_rdi_532bd1_u1' in NCPP.read_text(),'cpp test rdi rdi 532bd1')
    require('init_531d60_post_base_je_532bdb_532bd4_u1' in NCPP.read_text(),'cpp je 532bdb 532bd4')
    require('init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1' in NCPP.read_text(),'cpp mov rax rdi mem 532bd6')
    require('init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.cpp' in CMAKE.read_text(),'cmake mov rax rdi mem 532bd6')
    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in SC.read_text(),'slice merge mov rax rdi mem 532bd6')
    require('merge_mov_r13_8_rax_532bd9_u1_ran' in SC.read_text(),'slice merge mov r13 8 rax 532bd9')
    require('merge_mov_rax_rdi_mem_532bde_u1_ran' in SC.read_text(),'slice merge mov rax rdi mem 532bde')
    require('merge_sub_eax_010cb932_532be1_u1_ran' in SC.read_text(),'slice merge sub eax 532be1')
    require('merge_je_532bf0_532be6_u1_ran' in SC.read_text(),'slice merge je 532bf0 532be6')
    require('merge_call_170c0_532beb_u1_ran' in SC.read_text(),'slice merge call 170c0 532beb')
    require('merge_mov_rdi_r12_532bf0_u1_ran' in SC.read_text(),'slice merge mov rdi r12 532bf0')
    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')
    require('mov_rax_rdi_mem_532bd6_count' in TEST.read_text(),'test mov rax rdi mem 532bd6 field')
    require('end_rva == 0x532bf3' in TEST.read_text(),'test end rva 532bf3')
    require('mov_r13_8_rax_532bd9_count' in TEST.read_text(),'test mov r13 8 rax field')
    require('mov_rax_rdi_mem_532bde_count' in TEST.read_text(),'test mov rax rdi mem 532bde field')
    require('merge_mov_r13_8_rax_532bd9_u1_ran' in TEST.read_text(),'test merge mov r13 8 rax')
    require('merge_mov_rax_rdi_mem_532bde_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bde')
    require('merge_sub_eax_010cb932_532be1_u1_ran' in TEST.read_text(),'test merge sub eax 532be1')
    require('merge_je_532bf0_532be6_u1_ran' in TEST.read_text(),'test merge je 532bf0 532be6')
    require('merge_call_170c0_532beb_u1_ran' in TEST.read_text(),'test merge call 170c0 532beb')
    require('merge_mov_rdi_r12_532bf0_u1_ran' in TEST.read_text(),'test merge mov rdi r12 532bf0')
    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')
    require('je_532bf0_532be6_count' in TEST.read_text(),'test je 532bf0 532be6 field')
    require('call_170c0_532beb_count' in TEST.read_text(),'test call 170c0 532beb field')
    require('mov_rdi_r12_532bf0_count' in TEST.read_text(),'test mov rdi r12 532bf0 field')
    require('init_531d60_post_base_mov_r13_8_rax_532bd9_u1' in MOVR138.read_text(),'mov r13 8 rax hpp')
    require('init_531d60_post_base_mov_rax_rdi_mem_532bde_u1' in MOVRAXBDE.read_text(),'mov rax rdi mem 532bde hpp')
    require('init_531d60_post_base_mov_r13_8_rax_532bd9_u1.cpp' in CMAKE.read_text(),'cmake mov r13 8 rax')
    require('init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp' in CMAKE.read_text(),'cmake mov rax rdi mem 532bde')
    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in SUBEAX.read_text(),'sub eax 532be1 hpp')
    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp' in CMAKE.read_text(),'cmake sub eax 532be1')
    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in NCPP.read_text(),'cpp sub eax 532be1')
    require('init_531d60_post_base_je_532bf0_532be6_u1' in NCPP.read_text(),'cpp je 532bf0 532be6')
    require('init_531d60_post_base_je_532bf0_532be6_u1' in JE532BF0.read_text(),'je 532bf0 532be6 hpp')
    require('init_531d60_post_base_je_532bf0_532be6_u1.cpp' in CMAKE.read_text(),'cmake je 532bf0 532be6')
    require('init_531d60_post_base_call_170c0_532beb_u1' in NCPP.read_text(),'cpp call 170c0 532beb')
    require('init_531d60_post_base_call_170c0_532beb_u1' in CALL170C0BEB.read_text(),'call 170c0 532beb hpp')
    require('init_531d60_post_base_call_170c0_532beb_u1.cpp' in CMAKE.read_text(),'cmake call 170c0 532beb')
    require('init_531d60_post_base_mov_rdi_r12_532bf0_u1' in NCPP.read_text(),'cpp mov rdi r12 532bf0')
    require('init_531d60_post_base_mov_rdi_r12_532bf0_u1' in MOVRDIR12.read_text(),'mov rdi r12 532bf0 hpp')
    require('init_531d60_post_base_mov_rdi_r12_532bf0_u1.cpp' in CMAKE.read_text(),'cmake mov rdi r12 532bf0')
    print('OK check_f531d60_post_base_registry_body end=0x532bf3 units=56')
    return 0
if __name__=='__main__': raise SystemExit(main())
