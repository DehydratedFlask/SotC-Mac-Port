import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 571 (RVA 0xc780, 10644 bytes), batch 14 debug page (shadow_* player/horse debug variables, BP_Jump* commands and the launchReleaseElf command).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, the caller functor tags and the __cxa_atexit records. The page adds the 0x26f90 command item (parent / caption / console command name views, an opaque target pointer that another slot owns and the host supplies, flags 0x30); the callee body is hashed byte for byte.
Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the commands at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(571,'Player and horse debug page (inference from the strings): shadow_print*/shadow_mountHorse*/shadow_playerSlope variables, BP_JumpToCredits / BP_JumpEnding / BP_JumpToEnd and launchReleaseElf commands, and the CP10 Launch Release command item.','debug page batch 14: 20 bools, 11 floats, more bools and floats, five console commands and a 0x26f90 command item','bool and float variables, four console commands and one 0x26f90 command item',doc)
print(d['kinds'],d['nitems'])
