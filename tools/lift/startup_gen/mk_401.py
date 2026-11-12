import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 401 (RVA 0x621530, 3799 bytes), batch 11 debug page "CP10-Anim" / "Shadow IK".

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). The page builds bool variables (0x20af0, caller tag 0x15cc2f8), TWO local ranged-int
variables through the wrapper family 0x620e10 / 0x6211d0 (cp10_phaseBlendUse [0,2], cp10_phaseBlendTypeOverride [1,4]) and the 0x26db0 / 0x26ea0 toggles over them. The per-site tag proof (lint_sites extra) ties each
caller's functor tag to the wrapper's own class (+0x5a) and the atexit destructor to slot 0 of the wrapper's final class, with the wrapper a byte twin of 0x7ec160 outside seven rel32 operands (the family is proven in
check_helper_620e10.py). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (the character animation reading is inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(401,'CP10 character animation debug page: bool toggles and two local ranged-int variables (phase blend), plus the Shadow IK toggle (inference from the strings).','Menu: CP10-Anim and Shadow IK toggles.','bool vars + two local-int wrapper sites (0x620e10 family) + 0x26db0 toggles',doc)
print(d['kinds'],d['nitems'])
