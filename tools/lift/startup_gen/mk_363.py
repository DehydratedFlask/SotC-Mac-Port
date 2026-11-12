import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 363 (RVA 0x751c70, 701 bytes), batch 11 debug page (menu-less cp3_animal* variables).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page has NO menu rows. The local ranged-int variables go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) and its atexit destructor to slot 0 of the wrapper's final class (per-site tag proof, never inherited from another
slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (the readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(363,'cp3 animal debug page (menu-less): three local ranged-int variables cp3_animalDebugType / DisplayType / DebugID (inference from the strings).','No menu rows.','three local-int wrapper sites (0x751190 / 0x751550 / 0x751910), menu-less',doc)
print(d['kinds'],d['nitems'])
