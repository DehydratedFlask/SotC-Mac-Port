import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 408 (RVA 0x5c7f50, 4750 bytes), batch 16 render-settings page: a perspective-projection prelude (0x1580a0), seven name holders of four classes, two vector headers,
fourteen console commands (CP10_* / shadow_* / boss*) and a closing vector header.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, the caller functor tags, the command handler identity tags and the __cxa_atexit records. The 0x1580a0 call is modelled natively (helper_1580a0_perspective; its six arguments are constants of this body:
the fov immediate and three .rodata dwords, edx = 1; libc tanf is a host hook). The holder destructors 0x8ef00 / 0x2c84f0 / 0x5b5470 / 0x5b5490 are byte twins (check_helper_8ef00.py) and the three vector destructors 0x5b5530 / 0x5b55c0 / 0x5c7c00
are pinned by check_helper_5b5530.py; here they stay IDENTITY TAGS (recorded, never run). Plain stores of the body that belong to no constructor row (the render-state globals around 0x2fda22c..0x2fda464 and the inline-built records at
0x2fdab00..0x2fecf18) are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(408,'Render settings page (inference from the strings): a perspective-projection matrix, seven named setting holders, vector headers and the CP10_* / shadow_* / boss* console commands.','render settings page batch 16: perspective prelude, seven holders of four classes, three vector headers and fourteen commands','a perspective prelude, seven name holders, vector headers and fourteen console commands',doc)
print(d['kinds'],d['nitems'])
