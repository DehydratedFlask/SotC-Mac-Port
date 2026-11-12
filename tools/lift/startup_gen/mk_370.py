import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 370 (RVA 0x720b50, 52949 bytes), batch 12 debug page (head tracking and player tuning).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page adds a 0x257a0 string variable (borrowed name and empty initial view; two f590 then atexit 0x23c90) and a name holder with its own class (0x15a0a48, dtor 0x2b5050). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(370,'Head-tracking and player tuning page (inference from the strings): 131 bounded floats and 36 bools (shadow_headTrack_*), 5 unbounded floats, 2 local-int wrapper sites, 3 console commands, the startupArea string variable, a name holder, 10 stat cells and 43 menu rows.','Menu: Shadow Player / Shadow Controls / Hair / Pat / Swim / Collectibles toggles and 14 sliders.','bool, bounded float, unbounded float, local-int wrapper sites, string variable, console commands, name holder, name cells and menu rows',doc)
print(d['kinds'],d['nitems'])
