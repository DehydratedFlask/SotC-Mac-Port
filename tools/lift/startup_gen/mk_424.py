import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 424 (RVA 0x551140, 28370 bytes), batch 18: one static table initialiser (three console command rows and a 0x6720 B object written by 2088 straight-line stores).

Generated from the ELF by tools/lift/startup_gen. The three command rows are pinned operand by operand like every other page. The table is handled by the STORE-REPLAY tracker: the 3577 instructions
between the last command row and the __cxa_atexit call (0x55141d..0x557fe4) are decoded twice (objdump text and the closed byte decoder tools/lift/store_bytes_replay.py, which must agree) and replayed
symbolically; every store becomes one record (instruction rva, offset, width, lane kinds and values). This checker re-derives the same table from the ELF BYTES (overlay aware) and requires it to equal the
NATIVE table of the generated header record for record, plus the 531 pooled .rodata strings (re-read at their original addresses), the identity tags (7 text functions, hash-pinned, and vtable-like relro
addresses) and the final __cxa_atexit arguments (destructor 0x54dbb0, object null, &__dso_handle). The whole range is also pinned byte for byte. What the table means (an array of records with a callable
at +0x20 whose destructor walks them last to first) is INFERENCE; bytes the stores do not cover are never written.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(424,'Static record table object (inference only): three console command rows and one 0x6720 B object of ~274 records of 0x60 B, initialised by 2088 straight-line stores; its destructor releases a pointer at +0x40 of each record.','static table batch 18: console commands plus a 0x6720 B record table written by one replayed store run','three console command rows and one ELF-tied static store table of 2088 stores',doc)
print(d['kinds'],d['nitems'])
