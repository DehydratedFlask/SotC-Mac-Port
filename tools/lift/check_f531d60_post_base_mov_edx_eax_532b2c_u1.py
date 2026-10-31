#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532B2C
FRAG, FRAG_SIZE, END = 0x532B2C, 2, 0x532B2E
FRAG_SHA256 = "253aa00c2dbbb117d146347fbacae9ee27e47b32bcdef68e72f244258330c351"
CALL_TARGET = 0x52A220
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"
PREV = ROOT / "native/core/init_531d60_post_base_mov_rsi_rbx_532b29_u1.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require('kInit531d60PostBaseMovRsiRbx_532b29_U1EndRva = 0x532b2c' in PREV.read_text(),'mov rsi end')
    require(PREV_END==FRAG,'contiguous')
    frag=img.read(FRAG,FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest()==FRAG_SHA256,'sha')
    require(frag==bytes.fromhex('89c2'),'mov edx,eax')
    b=img.read(END,5); require(b[0]==0xE8,'call head')
    rel=struct.unpack('<i',b[1:5])[0]; require(END+5+rel==CALL_TARGET,'call 0x52a220')
    nh=NATIVE.read_text()
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1' in nh,'api')
    require('NOT modelled' in nh,'omit')
    reg=REG.read_text()
    require('merge_mov_edx_eax_532b2c_u1_ran' in reg,'merge')
    require('kRegistryInit531d60FragmentPinnedRva = 0x532b65' in reg,'pin')
    require('init_531d60_post_base_mov_edx_eax_532b2c_u1.cpp' in CMAKE.read_text(),'cmake')
    print(f'OK check_f531d60_post_base_mov_edx_eax_532b2c_u1 end={END:#x}')
    return 0
if __name__=='__main__': raise SystemExit(main())
