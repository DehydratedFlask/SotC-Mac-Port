#!/usr/bin/env python3
from __future__ import annotations
import hashlib, struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START, SIZE, END = 0x532B0C, 6, 0x532B12
REGION_SHA256 = "274b64f92199cbef9472ff9bcec30176c06836f2185a3d345a2a079c2b139454"
CALL_RVA, CALL_TARGET = 0x532B11, 0x17070
UNITS=[(0x532B0C,5,"8701905b6331edca312a4b173aa2f603ccfd80a39c1382f4aa36467e2a5a091f")]
NATIVE=ROOT/"native/core/init_531d60_post_base_registry_tail.hpp"
NCPP=ROOT/"native/core/init_531d60_post_base_registry_tail.cpp"
MOV=ROOT/"native/core/init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
SL=ROOT/"native/core/appmain_init_ctor_98a90_slice.hpp"
SC=ROOT/"native/core/appmain_init_ctor_98a90_slice.cpp"
TEST=ROOT/"native/tests/test_appmain_init_ctor_98a90_slice.cpp"
CMAKE=ROOT/"native/CMakeLists.txt"

def require(c,m):
    if not c: raise SystemExit('FAIL '+m)

def main():
    elf=(ROOT/"local/re/eboot.elf").read_bytes(); require(hashlib.sha256(elf).hexdigest()==ELF_SHA256,'elf')
    _,ph=d.program_headers(elf); img=d.Image(elf,ph)
    require(len(UNITS)==1,'units')
    require(hashlib.sha256(img.read(START,SIZE)).hexdigest()==REGION_SHA256,'region')
    b=img.read(CALL_RVA,5); require(b[0]==0xE8,'call')
    rel=struct.unpack('<i',b[1:5])[0]; require(CALL_RVA+5+rel==CALL_TARGET,'target')
    nh=NATIVE.read_text()
    require('kInit531d60PostBaseRegistryTailRva = 0x532b11' in nh,'rva')
    require('kInit531d60PostBaseRegistryTailSize = 0' in nh,'size0')
    require('merge_mov_edi_98_532b0c_u1_ran' in nh,'merge')
    require('init_531d60_post_base_mov_edi_98_532b0c_u1' in NCPP.read_text(),'cpp')
    require('post_base_registry_tail_hooks' in SL.read_text(),'slice hpp')
    require('init_531d60_post_base_registry_tail' in SC.read_text(),'slice cpp')
    require('init_531d60_post_base_registry_tail.cpp' in CMAKE.read_text(),'cmake')
    require('merge_mov_edi_98_532b0c_u1_ran' in TEST.read_text(),'test merge')
    print('OK check_f531d60_post_base_registry_tail post-peel=0x532b11+0B')
    return 0
if __name__=='__main__': raise SystemExit(main())
