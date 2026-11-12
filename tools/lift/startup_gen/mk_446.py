import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 446 (RVA 0x4c92a0), batch 21 scripting/debug page: four bool variables, a name holder, a name cell, the agroColor local-int variable, a group and a toggle item (menu rows), then two SCRIPTED-COMMAND REGISTRY ENTRIES (waitForAreaLoader, waitForGameToStart): each a 0x90 B object (three {obj+0x88, functor tag} pairs, zeros, the name SharedStr at +0x60 with an atomic control increment, the previous list head at +0x80), pushed on the global list at 0x2ef6400 and, when the registry object *0x2ef63f8 exists, registered through the red-black find-or-insert 0x3be2d0, plus the __cxa_atexit records.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). The entry run is evaluated symbolically by the generator (any unmodelled instruction is a hard error) and pinned byte for byte, together with the functor-tag slots, both literals, the registration body 0x3be2d0 and the destructor 0x3c15e0. The red-black callees of 0x3be2d0 are modelled and pinned by tools/lift/check_rb_registry_3be2d0.py. Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What the functors do at run time is NOT established (the names look like script wait commands: inference only).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(446,'Scripting/debug page (inference from the strings): bools, a holder, agroColor, a group, a toggle and two scripted-command registry entries (waitForAreaLoader, waitForGameToStart).','scripting page batch 21: bools, holder, agroColor, group, toggle and two registry entries','four bools, a holder, a local int, a group, a toggle and two registry entries',doc)
print(d['kinds'],d['nitems'],d['menu_less'])
