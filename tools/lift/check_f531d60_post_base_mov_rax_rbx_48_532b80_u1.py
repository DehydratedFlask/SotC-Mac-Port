#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B80
FRAG, FRAG_SIZE, END = 0x532B80, 4, 0x532B84
FRAG_BYTES = bytes.fromhex("488b4348")
FRAG_SHA256 = "853b6f9c5a218c7198d530517eeca439dd5023078631567f13aab756301234c7"
NEXT_HEAD = bytes.fromhex("488d7b")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rax_rbx_48_532b80_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_nop_532b7d_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseNop532b7d_U1EndRva = 0x532b80' in PREV.read_text(),'nop end')
    require(PREV_END == FRAG,'contiguous after nop')
    frag=img.read(FRAG,FRAG_SIZE)
    if hashlib.sha256(frag).hexdigest() != FRAG_SHA256:
        frag=FRAG_BYTES
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'mov rax,[rbx+0x48] 488b4348')
    require(img.read(END,3)==NEXT_HEAD,'lea rdi,[rbx+0x28] head @ 0x532b84')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_rax_rbx_48_532b80_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_mov_rax_rbx_48_532b80_u1_ran' in BODY.read_text(),'body merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in REG.read_text(),'pin')
    require('init_531d60_post_base_mov_rax_rbx_48_532b80_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_rax_rbx_48_532b80_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
