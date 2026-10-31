#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B50
FRAG, FRAG_SIZE, END = 0x532B50, 6, 0x532B56
FRAG_SHA256 = "08410fa0f64c8db2820a0317c4713b6fad3bfd7b6b814b06cff5d9b409b50b90"
JE_TARGET = 0x532BF8
NEXT_HEAD = bytes.fromhex("4c8974")
NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_test_r12_r12_532b4d_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseTestR12R12_532b4d_U1EndRva = 0x532b50' in PREV.read_text(),'test end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('0f84a2000000'),'je rel32 +0xa2')
    rel=struct.unpack('<i',frag[2:6])[0]
    require(FRAG+FRAG_SIZE+rel==JE_TARGET,'je target 0x532bf8')
    require(img.read(END,3)==NEXT_HEAD,'mov r12,[rsp+..] head @ 0x532b56')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_je_532bf8_532b50_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_je_532bf8_532b50_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b5b' in reg,'pin')
    require('init_531d60_post_base_je_532bf8_532b50_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_je_532bf8_532b50_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
