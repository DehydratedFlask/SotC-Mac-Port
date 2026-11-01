#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B93
FRAG, FRAG_SIZE, END = 0x532B94, 4, 0x532B98
FRAG_BYTES = bytes.fromhex("498b5d10")
FRAG_SHA256 = "51cf1bb258add2501b662c9a241ee79e38a7cf232aaae570ce5e3ce40dd41e42"
DISP = 0x10
NEXT_HEAD = bytes.fromhex("4885db")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rbx_r13_10_532b94_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_jne_532b80_532b92_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseJne532b80_532b92_U1EndRva = 0x532b94' in PREV.read_text(),'jne end')
    require(PREV_END + 1 == FRAG,'contiguous after jne')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'mov rbx,[r13+0x10] 498b5d10')
    require(frag[3]==DISP,'disp 0x10')
    require(img.read(END,3)==NEXT_HEAD,'test rbx head @ 0x532b98')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rbx_r13_10_532b94_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in BODY.read_text(),'body merge')
    require('merge_mov_rbx_r13_10_532b94_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b9b' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_rbx_r13_10_532b94_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rbx_r13_10_532b94_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
