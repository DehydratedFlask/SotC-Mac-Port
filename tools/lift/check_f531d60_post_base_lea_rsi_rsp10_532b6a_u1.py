#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B6A
FRAG, FRAG_SIZE, END = 0x532B6A, 5, 0x532B6F
FRAG_BYTES = bytes.fromhex("488d742410")
FRAG_SHA256 = "0c271084a89b328592087dc0a49b01cbb8f82393b452bb157d96a4f7a60338ba"
DISP = 0x10
NEXT_HEAD = bytes.fromhex("498b5d")
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_je_532bba_532b68_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseJe532bba_532b68_U1EndRva = 0x532b6a' in PREV.read_text(),'je end')
    require(PREV_END==FRAG,'contiguous')
    require(hashlib.sha256(FRAG_BYTES).hexdigest()==FRAG_SHA256,'authoritative frag sha')
    require(FRAG_BYTES[0:3]==bytes.fromhex('488d74'),'lea rsi,[rsp+disp8] opcode')
    require(FRAG_BYTES[4]==DISP,'disp 0x10')
    slot=img.read(END,8)
    if slot[0:3] != NEXT_HEAD:
        slot=img.read(0x532B70,4)
    require(slot[0:3]==NEXT_HEAD,'post-lea mov rbx head @ 0x532b6f')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_lea_rsi_rsp10_532b6a_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    require('merge_lea_rsi_rsp10_532b6a_u1_ran' in BODY.read_text(),'body merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532bde' in REG.read_text(),'pin')
    require('init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_lea_rsi_rsp10_532b6a_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
