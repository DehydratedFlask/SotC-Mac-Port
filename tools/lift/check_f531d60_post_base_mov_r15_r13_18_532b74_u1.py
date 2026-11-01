#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B73
FRAG, FRAG_SIZE, END = 0x532B74, 4, 0x532B78
FRAG_BYTES = bytes.fromhex("4d8b7d18")
FRAG_SHA256 = "bcb886a3b4697acd64e109a054f3fd7018a1a54cab8159cc6a20a54b074bcccb"
DISP = 0x18
NEXT_HEAD = bytes.fromhex("4c39fb")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r15_r13_18_532b74_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_rbx_r13_10_532b70_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovRbxR13_10_532b70_U1EndRva = 0x532b74' in PREV.read_text(),'mov rbx end')
    require(PREV_END + 1 == FRAG,'contiguous after mov rbx')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag[0:3]==bytes.fromhex('4d8b7d'),'mov r15,[r13+disp8] opcode')
    require(frag[3]==DISP,'disp 0x18')
    require(img.read(END,3)==NEXT_HEAD,'post-mov cmp r15,r11 head @ 0x532b78')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_r15_r13_18_532b74_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_r15_r13_18_532b74_u1_ran' in BODY.read_text(),'body merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_r15_r13_18_532b74_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_r15_r13_18_532b74_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
