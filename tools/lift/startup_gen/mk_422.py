import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 422 (RVA 0x56b980, 2682 bytes), batch 15 debug page (collision / debug-draw variables with an out-of-line bool constructor).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The out-of-line bool constructor 0x274730 is byte-pinned in check_helper_274730.py; the per-site argument-functor class tag is tied here. The zero pointer slot records __cxa_atexit(0x565e80), an identity tag. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(422,'A debug page of bool variables (one built by the out-of-line constructor 0x274730), local ranged-int wrapper variables, a command item, a node-ref row and a zero pointer slot.','collision debug page','bool, local-int and out-of-line bool variables, items, a node-ref row and a zero pointer slot',doc)
print(d['kinds'],d['nitems'])
