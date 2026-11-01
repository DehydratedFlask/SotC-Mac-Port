#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B3A
FRAG, FRAG_SIZE, END = 0x532B3A, 7, 0x532B41
FRAG_SHA256 = "bdd7f670e8d62797e4c60b6970c53a006e7136517bff55a0d59ca0d80a952cdd"
TARGET = 0x15FE518
NEXT_HEAD = bytes.fromhex("4c8b23")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r13_15fe518_532b3a_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseLeaRbx2efd160_532b33_U1EndRva = 0x532b3a' in PREV.read_text(),'lea end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag[0:3]==bytes.fromhex('4c8b2d'),'mov r13,[rip+disp32]')
    rel=struct.unpack('<i',frag[3:7])[0]
    require(FRAG+7+rel==TARGET,'target 0x15fe518')
    require(img.read(END,3)==NEXT_HEAD,'mov r12 head @ 0x532b41')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_r13_15fe518_532b3a_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_mov_r13_15fe518_532b3a_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x' in reg,'pin')
    require('init_531d60_post_base_mov_r13_15fe518_532b3a_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_r13_15fe518_532b3a_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
