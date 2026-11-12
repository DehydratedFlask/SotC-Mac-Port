import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 398 (RVA 0x63cf60, 1408 bytes), batch 11 debug page (CP10-Anim and Shadow Misc toggles).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The local ranged-int variables go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) and its atexit destructor to slot 0 of the wrapper's final class (per-site tag proof, never inherited from another
slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (the readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(398,'CP10 animation display toggles plus two local-int spew variables (shadow_spewMotShift / shadow_spewSubMotShift; inference from the strings).','Menu: CP10-Anim and Shadow Misc toggles.','two bool vars + two local-int wrapper sites (0x529160 family) + 0x26db0 / 0x26ea0 toggles',doc)
print(d['kinds'],d['nitems'])
