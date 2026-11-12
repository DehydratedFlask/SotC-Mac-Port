import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 511 (RVA 0x1d90c0, 1550 bytes), batch 15 debug page: Profile think/render time debug page with two empty trees.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(511,'Profile debug variables (inference from the strings): showThinkTimes / showThinkTimesThreshold / showRenderTimes / showRenderTimesThreshold unbounded ints, four Profile toggles and two empty tree headers.','debug page batch 15: four unbounded ints, four 0x26db0 toggles and two empty trees','four unbounded int variables, four toggles and two empty RB-tree headers (dtor 0x1d6210)',doc)
print(d['kinds'],d['nitems'])
