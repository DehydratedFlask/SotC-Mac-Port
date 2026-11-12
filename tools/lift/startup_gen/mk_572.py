import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 572 (RVA 0x1a20, 1668 bytes), batch 14 debug page (player/camera position commands).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, the caller functor tags and the __cxa_atexit records. The page adds the 0x26f90 command item (parent / caption / console command name views, an opaque target pointer that another slot owns and the host supplies, flags 0x30); the callee body is hashed byte for byte.
Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the commands at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(572,'Player-position debug console commands (inference from the strings): printPlayerPos / printPlayerAndCameraPos / setPlayerPos / Pos / toggleDebugCam plus the CP10 menu rows (two toggles and a Kill the active Boss command item).','debug page batch 14: five console commands, two 0x26ea0 toggles and a 0x26f90 command item','five console commands, two 0x26ea0 toggles and one 0x26f90 command item',doc)
print(d['kinds'],d['nitems'])
