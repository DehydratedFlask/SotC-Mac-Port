import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 390 (RVA 0x691c50, 1035 bytes), batch 14 debug page (empty tree header, node reference, three name holders and two described bools).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The empty RB-tree header allocates its sentinel node through the host allocator input behind 0x17070 (fail-closed when missing); the node-reference row is the native 0x56b4b0 helper. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(390,'Legacy particle debug variables (inference from the strings): showLegacyParticle and showLegacyParticle_Overrides bools, with an empty RB-tree header, a node-reference record and three name holders around them','debug page batch 14: empty tree header (host allocator input) + node-reference row + name holders + described bools','Slot 390 builds an empty tree header (host allocator input), a node reference record, three name holders and two showLegacyParticle bools',doc)
print(d['kinds'],d['nitems'])
