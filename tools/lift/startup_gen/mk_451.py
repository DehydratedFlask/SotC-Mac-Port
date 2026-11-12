import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 451 (RVA 0x49d230, 5126 bytes), batch 17 debug page: the cp10_testUI test console page. Rows in constructor order: bool variables, unbounded
dword variables (with descriptions), unbounded float variables, two DWORD-RANGED variables (0x49ca60 / 0x49cc60, class A / B), two critical-section objects (0x3b310 plus field stores, atexit 0x499e50 / 0x499e60),
one zero pointer slot whose atexit destructor is the 0x499e90 node holder, and one console command.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags, the final vtable pointers and the __cxa_atexit records. The critical-section stores are pinned byte for byte over the whole
init + store run; the native model replays the observed offsets / widths / values (the 0x3b310 handle is the host sidecar token, NOT a Sony pthread). Plain stores of the body that belong to no row are pinned only by the body
hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(451,'Debug page (inference from the strings): the cp10_testUI test console variables, two dword-ranged variables, two critical-section objects, a node-holder slot and one command.','debug page batch 17: test console variables, dword-ranged variables, critical-section objects and a node holder','console variables, dword-ranged variables, critical-section objects and a node-holder slot',doc)
print(d['kinds'],d['nitems'])
