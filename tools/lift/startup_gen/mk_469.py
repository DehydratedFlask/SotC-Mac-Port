import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 469 (RVA 0x394a20, 3658 bytes), batch 12 debug page (sound auto-ambient-grass tuning page).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page ends with a sentinel container (header stores [0]=[8]=[0x20]=this+0x28, [0x10]=this+0xa8; atexit 0x3941b0), pinned byte for byte as one instruction run. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(469,'Sound auto ambient grass page (inference from the strings): 5 bool and 3 bounded float sndx_autoGrass* variables, the Sound group, 6 explicit-text toggles and 4 sliders, and a trailing self-referential sentinel container whose destructor 0x3941b0 is recorded.','Menu: Sound group; Auto Ambient Grass toggles and sliders.','bool, bounded float, menu rows, sentinel container',doc)
print(d['kinds'],d['nitems'])
