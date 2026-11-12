import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 513 (RVA 0x1d0770, 4050 bytes), batch 12 debug page, Game Manager debug page.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The 13 named records go through the registrar 0xa81700 (byte-pinned in check_helper_a81700.py; whole-body hashes here). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(513,'Game Manager debug page (inference from the strings): 13 named records (0xa81700), draw/showObject variables (unbounded int, local ranged int, ranged int, 4 bool), 6 explicit-text toggles under Game Manager, and a trailing zero-filled container whose destructor 0x1c5090 is recorded.','Menu: Game Manager; explicit-text toggles.','named records, ints, bools, menu rows, zero container',doc)
print(d['kinds'],d['nitems'])
