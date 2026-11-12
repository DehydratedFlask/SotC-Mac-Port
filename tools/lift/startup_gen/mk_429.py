import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 429 (RVA 0x52e2b0, 1734 bytes), batch 19 debug page (Stat System: stats_clear / stats_dump commands, Stat On Screen bool, two groups and two command items).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page also holds one zero pointer slot (atexit destructor 0x52a010) and three zero-header vectors (begin / end / cap zeroed; atexit destructors 0x52a0d0, 0x52a140, 0x52a1b0): their destructors are IDENTITY TAGS, hash-pinned by FDE range, never run. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(429,'Debug page (inference from the strings): the Stat System page: stats_clear and stats_dump commands, the Stat On Screen bool, two groups and two command items, one zero pointer slot and three zero-header vectors.','debug page batch 19: stat system commands, a bool, groups, command items, a zero pointer slot and three zero-header vectors','stat system commands, a bool, two groups, two command items, one zero pointer slot and three zero vectors',doc)
print(d['kinds'],d['nitems'])
