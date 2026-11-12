import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 427 (RVA 0x5388f0, 4362 bytes), batch 17 debug page.
Rows in constructor order: console variables (string, bool, unsigned, float), menu toggles and sliders over them, two SINGLE named records (0xa81700 registrar + atexit 0xa81950 on the record;
the first use initialises the process record registry and records atexit 0xa81b80 before it), and one zero cell whose atexit destructor is 0x534580 (frees the pointer at +0, keeps the cell).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every row is pinned operand by operand: object addresses, name string addresses and
length immediates, flags, init / range immediates, functor tags, final vtable pointers and the __cxa_atexit records. The zero-cell store run is pinned byte for byte. Plain stores of the body that belong to no row
are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(427,'Debug page (inference from the strings): console variables, menu toggles, two single named records and a zero cell.','debug page batch 17: console variables, menu items, single named records and a zero cell','console variables, menu items, single named records and a zero cell',doc)
print(d['kinds'],d['nitems'])
