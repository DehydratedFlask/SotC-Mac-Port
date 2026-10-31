#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B47
FRAG, FRAG_SIZE, END = 0x532B47, 6, 0x532B4D
FRAG_SHA256 = "4e6ccfb65620d791cdb7ac0ce5025f52a7d1cdcd1409819b1f8b84b4a61d8d64"
JE_TARGET = 0x532BFB
NEXT_HEAD = bytes.fromhex("4d85e4")
NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bfb_532b47_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_cmp_r12_r14_532b44_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseCmpR12R14_532b44_U1EndRva = 0x532b47' in PREV.read_text(),'cmp end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('0f84ae000000'),'je rel32 +0xae')
    rel=struct.unpack('<i',frag[2:6])[0]
    require(FRAG+FRAG_SIZE+rel==JE_TARGET,'je target 0x532bfb')
    require(img.read(END,3)==NEXT_HEAD,'test r12,r12 head @ 0x532b4d')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_je_532bfb_532b47_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_je_532bfb_532b47_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b60' in reg,'pin')
    require('init_531d60_post_base_je_532bfb_532b47_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_je_532bfb_532b47_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
