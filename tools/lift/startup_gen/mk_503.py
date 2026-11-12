import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 503 (RVA 0x247f00, 3185 bytes), batch 15 debug page: UI debug page (scene stack commands) with a null-name record.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The null-name record is the native store_null_name_cell_247f47 (ISO C strncpy; check_helper_f8f630.py). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(503,'UI scene debug page (inference from the strings): a NULL-named record, three bool/float override variables, nine ui* console commands and a Render draw toggle.','debug page batch 15: null-name record, three variables, nine commands and a 0x26ea0 toggle','a null-name record (strncpy), UI override variables, console commands and one toggle',doc)
print(d['kinds'],d['nitems'])
