import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 534 (RVA 0x1418d0, 6207 bytes), batch 17 debug page (texture streaming debug variables: enableTextureStreaming, showTextures*, the texture pool sizes in MiBs, one UNBOUNDED inline-built enum
variable textureUnloadHighMode, one 1-qword zero cell (atexit 0x13a0d0) and the two commands tx_unload_high / tx_dump_stats).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags, the final vtable pointers and the __cxa_atexit records. The inline-built enum variable is a 0xd8 byte ranged var whose class pair
(0x15982c8 / 0x1598438), functor tag (0x15984a0) and destructor (0x140010) are identity tags; the final class slots 3 and 6 are pinned as mov eax,1. Unlike the bounded enum of slot 402 it stores ONE zero byte at +0x6c / +0x74 and
clears both engaged bytes (+0x70 / +0x78); the other bound bytes are never written. The zero cell is a single qword zeroed by the body (pinned byte for byte); its destructor 0x13a0d0 deletes through the pointer and clears the cell
(identity tag, never run). Plain stores of the body that belong to no row are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT established.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(534,'Debug page (inference from the strings): texture streaming variables, one unbounded inline enum, a zero cell and two commands.','debug page batch 17: texture streaming variables, an unbounded inline enum, a zero cell and two commands','texture streaming variables, an unbounded inline enum, a zero cell and two commands',doc)
print(d['kinds'],d['nitems'])
