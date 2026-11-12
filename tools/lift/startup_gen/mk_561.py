import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 561 (RVA 0x3ac00, 1808 bytes), batch 12 debug page (js threading stats).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page mixes a ranged int (0xc580) and an unbounded int (0x3a980) that share the caller tag 0x1595660; toggles over the int classes keep the default range (int class slot 3 = 1, slot 6 = 2). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(561,'Threading/JS-stats page (inference from the strings): js_PrintStats / js_EnableTracking bools, js_StackCheckerStride ranged int, js_WipeStackMemory unbounded int and the Systems / Threading toggles.','Menu: Systems / Threading group with four 0x26ea0 toggles (two bool, one ranged int, one unbounded int).','two bool vars, ranged int, unbounded int, 0x26c10 group and four 0x26ea0 toggles',doc)
print(d['kinds'],d['nitems'])
