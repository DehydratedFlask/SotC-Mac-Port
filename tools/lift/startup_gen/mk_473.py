import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 473 (RVA 0x355a00, 6487 bytes), batch 16 debug page: one name holder, bool / unbounded-float / string / ranged-bool variables, 0x26ea0 toggles, 0x26c10 group,
five 0x26b60 separators, one console command and a 0x2628-byte ring / vector header (0x1f220, atexit 0x354470).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, the caller functor tags, the command handler identity tag and the __cxa_atexit records. The separators (0x26b60 builds "MenuSeparator" through the native 0xf1a0; the allocator is a HOST input),
the ring header (helper_1f220_ring_header: exactly four qword stores) and the holder destructor 0x34e820 (check_helper_8ef00.py) are tied to their own byte-pinned models; the ring destructor 0x354470 (check_helper_354470.py) stays an
IDENTITY TAG here (recorded, never run). Plain stores of the body that belong to no constructor row are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT
established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(473,'Debug page (inference from the strings): bool / float / string variables, toggles, a group, separators, one console command and an inline-storage ring header.','debug page batch 16: holder, variables, toggles, group, five separators, command and a ring header','a name holder, console variables, menu toggles, a group, five separators, a command and a ring header',doc)
print(d['kinds'],d['nitems'])
