#!/usr/bin/env python3
from __future__ import annotations
import hashlib, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B92
FRAG, FRAG_SIZE, END = 0x532B92, 2, 0x532B94
FRAG_BYTES = bytes.fromhex("75ec")
FRAG_SHA256 = "437b263352f51d6dc454cc85816e0ab7b8c797936e198534e1790a004efdb56f"
NEXT_HEAD = bytes.fromhex("498b")
NATIVE = ROOT / "native/core/init_531d60_post_base_jne_532b80_532b92_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_cmp_r15_rbx_532b8f_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseCmpR15Rbx_532b8f_U1EndRva = 0x532b92' in PREV.read_text(),'cmp r15 rbx end')
    require(PREV_END == FRAG,'contiguous after cmp r15 rbx')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(frag==FRAG_BYTES,'jne 75ec')
    require(img.read(END,2)==NEXT_HEAD,'mov head @ 0x532b94')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_jne_532b80_532b92_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_jne_532b80_532b92_u1_ran' in BODY.read_text(),'body merge')
    require('merge_jne_532b80_532b92_u1_ran' in REG.read_text(),'reg merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b94' in REG.read_text(),'pin')
    require('init_531d60_post_base_jne_532b80_532b92_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_jne_532b80_532b92_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
