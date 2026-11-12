import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 508 (RVA 0x2241e0, 3252 bytes), batch 12 debug page, physics debug variables.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The five physics* unbounded-float initial values are 4-byte loads of globals (not constants of the body): the model takes them as explicit input fields (default = ELF initial image) and the checker pins the load sites. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(508,'Physics debug variables (inference from the strings): drawRigidBodies / drawCollision / physics::drawTouch ranged ints, drawCollisionMeshMin/MaxTris unbounded ints, drawModifiers / drawConstraints bools and five physics* unbounded floats whose initial values are RUN-TIME LOADS of globals (four .data globals at 0x1604918..0x1604924 and the .bss global 0x2ee8820): modelled as explicit INPUT fields (Objects::source_bits), default = the pinned ELF initial image.','No menu rows (variables only).','ranged ints, unbounded ints, bools and unbounded floats with run-time loaded initial values (input fields)',doc)
print(d['kinds'],d['nitems'])
