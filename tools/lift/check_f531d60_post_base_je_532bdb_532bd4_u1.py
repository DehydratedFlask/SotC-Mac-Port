#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BD4
FRAG, FRAG_SIZE, END = 0x532BD4, 2, 0x532BD6
FRAG_SHA256 = "58b821595e7051b1409a38f7c48f7b1153019dce5f9e8b78241111e6dd4a67cb"
JE_TARGET = 0x532BDB
NEXT_HEAD = bytes.fromhex("e8e544aeff")
NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bdb_532bd4_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_test_rdi_rdi_532bd1_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseTestRdiRdi_532bd1_U1EndRva = 0x532bd4' in PREV.read_text(),'test end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('7405'),'je rel8 +0x05')
    rel=struct.unpack('b', bytes([frag[1]]))[0]
    require(FRAG+FRAG_SIZE+rel==JE_TARGET,'je target 0x532bdb')
    require(img.read(END,5)==NEXT_HEAD,'call head @ 0x532bd6')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_je_532bdb_532bd4_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_je_532bdb_532bd4_u1_ran' in BODY.read_text(),'body merge')
    require('merge_je_532bdb_532bd4_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bd9' in REG.read_text(),'pin')
    require('init_531d60_post_base_je_532bdb_532bd4_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_je_532bdb_532bd4_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
