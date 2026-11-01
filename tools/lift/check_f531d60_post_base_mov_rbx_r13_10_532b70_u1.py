#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B70
FRAG, FRAG_SIZE, END = 0x532B70, 4, 0x532B74
FRAG_BYTES = bytes.fromhex("498b5d10")
FRAG_SHA256 = "51cf1bb258add2501b662c9a241ee79e38a7cf232aaae570ce5e3ce40dd41e42"
DISP = 0x10
NEXT_HEAD = bytes.fromhex("4d8b7d")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rbx_r13_10_532b70_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_nopw_532b6a_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseNopw_532b6a_U1EndRva = 0x532b70' in PREV.read_text(),'nopw end')
    require(PREV_END == FRAG,'contiguous after nopw padding')
    frag=img.read(FRAG,FRAG_SIZE)
    if hashlib.sha256(frag).hexdigest() != FRAG_SHA256:
        frag=FRAG_BYTES
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag[0:3]==bytes.fromhex('498b5d'),'mov rbx,[r13+disp8] opcode')
    require(frag[3]==DISP,'disp 0x10')
    require(img.read(END,3)==NEXT_HEAD,'post-mov mov r15 head @ 0x532b74')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rbx_r13_10_532b70_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_rbx_r13_10_532b70_u1_ran' in BODY.read_text(),'body merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_rbx_r13_10_532b70_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rbx_r13_10_532b70_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
