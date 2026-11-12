import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 352 (RVA 0x7a2390, 2290 bytes), batch 11 debug page.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses,
name string addresses and their length immediates, flags, value and range immediates, the caller functor tags (float tag 0x15ce1e0 and int tag 0x15ce200, whose bodies are pinned), and the
__cxa_atexit records. The page has NO menu rows. Vtables, functor tags and destructors (0x20f0, 0x18a60) are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established
(the camera dolly / rewind / bow-on-horseback reading is inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(352,'Camera debug page (menu-less): six bounded floats, one ranged int and one unbounded float (dolly spring, camera rewind, bow-on-horseback look-at scale; inference from the strings).','No menu rows: the page only registers console variables.','6 bounded floats + ranged int (0xc580) + unbounded float (0x6aa00), menu-less',doc)
print(d['kinds'],d['nitems'])
