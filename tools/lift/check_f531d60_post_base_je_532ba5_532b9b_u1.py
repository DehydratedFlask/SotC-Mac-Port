#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B9B
FRAG, FRAG_SIZE, END = 0x532B9B, 2, 0x532B9D
FRAG_SHA256 = "ce7b207f44bdfcb4050755ac2969bf23f44a92ef9c1139df06caf97b9b3aedd1"
JE_TARGET = 0x532BA5
NEXT_HEAD = bytes.fromhex("4889df")
NATIVE = ROOT / "native/core/init_531d60_post_base_je_532ba5_532b9b_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_test_rbx_rbx_532b98_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseTestRbxRbx_532b98_U1EndRva = 0x532b9b' in PREV.read_text(),'test end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('7408'),'je rel8 +0x08')
    rel=struct.unpack('b', bytes([frag[1]]))[0]
    require(FRAG+FRAG_SIZE+rel==JE_TARGET,'je target 0x532ba5')
    require(img.read(END,3)==NEXT_HEAD,'mov rdi,rbx head @ 0x532b9d')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_je_532ba5_532b9b_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_je_532ba5_532b9b_u1_ran' in BODY.read_text(),'body merge')
    require('merge_je_532ba5_532b9b_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bde' in REG.read_text(),'pin')
    require('init_531d60_post_base_je_532ba5_532b9b_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_je_532ba5_532b9b_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
