import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 399 (RVA 0x632ca0, 4642 bytes), batch 12 debug page (player/controls tuning).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page adds the 0x27050 slider row (explicit text, any target object, range from .rodata floats pinned byte for byte). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(399,'Player-controls page (inference from the strings): save-volume / persist-move-level / controller-assignment variables, four 0x27050 sliders, 0x26ea0 toggles and nine name-hash cells.','Menu: Shadow Misc / Shadow Player / Shadow Controls / Grip toggles and 0x27050 sliders.','bool, bounded float, local-int wrapper sites, 0x26ea0 toggles, four 0x27050 sliders and nine name cells',doc)
print(d['kinds'],d['nitems'])
