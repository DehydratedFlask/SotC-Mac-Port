import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 498 (RVA 0x252ae0, 2379 bytes), batch 16 debug page: two bool variables, an unbounded int, a ranged int, four menu items, one 0x26b60 separator and a table of
24 member-hash records built from 48 six-byte getters (helper_498_member_hashes).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The 24 getter pairs are pinned body by body here (each `mov eax, imm32 ; ret`, 6 B) and the whole call / store run is
pinned byte for byte; the native table copies the ELF-derived dwords (helper_498_member_hashes getters) and does not interpret them (INFERENCE only: a type id and a per-member hash). The separator builds "MenuSeparator" through the native
0xf1a0 (the allocator is a HOST input). Plain stores of the body that belong to no constructor row are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT
established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(498,'Debug page (inference from the strings): console variables, four menu items, a separator and a 24-record member-hash table.','debug page batch 16: variables, four items, a separator and a member-hash table','console variables, menu items, a separator and a table of 24 member-hash records',doc)
print(d['kinds'],d['nitems'])
