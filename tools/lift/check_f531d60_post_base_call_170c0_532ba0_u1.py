#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BA0
FRAG, FRAG_SIZE, END = 0x532BA0, 5, 0x532BA5
FRAG_SHA256 = "ba50bb61223083644ce96ef494e8c15caeb546653d72a138cf09727a8771622c"
CALL_TARGET = 0x170C0
NATIVE = ROOT / "native/core/init_531d60_post_base_call_170c0_532ba0_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_rdi_rbx_532b9d_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovRdiRbx_532b9d_U1EndRva = 0x532ba0' in PREV.read_text(),'mov end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('e81b45aeff'),'call 0x170c0')
    require(frag[0]==0xE8,'call')
    rel=struct.unpack('<i',frag[1:5])[0]
    require(FRAG+5+rel==CALL_TARGET,'target 0x170c0')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_call_170c0_532ba0_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_call_170c0_532ba0_u1_ran' in BODY.read_text(),'body merge')
    require('merge_call_170c0_532ba0_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532ba5' in REG.read_text(),'pin')
    require('init_531d60_post_base_call_170c0_532ba0_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_call_170c0_532ba0_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
