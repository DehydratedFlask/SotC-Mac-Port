import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 402 (RVA 0x610430, 2663 bytes), batch 14 debug page (CP10 animation debug variables: shadow_gridBlendDefaultBoundary floats, an inline-built ranged enum, bools, a string variable, toggles and a slider).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The inline-built enum variable is a 0xd8 byte ranged var whose class pair (0x15cc0a8 / 0x15cc140), functor tag (0x15cc1a8) and destructor (0x60d5d0) are identity tags; the final class slots 3 and 6 are pinned as mov eax,1. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(402,'CP10 animation debug page (inference from the strings): two shadow_gridBlendDefaultBoundary floats, the CP10_AnimMode ranged enum (range 0..2, inline-built), CP10_Anim* bools and floats, the CP10_NewAnimationFilter string variable, toggles, a slider and the r_enableAnimations name holder','debug page batch 14: inline-built ranged enum variable (identity class pair) + floats, bools, a string variable, toggles and a slider','Slot 402 builds the CP10 animation debug variables, including the inline-built CP10_AnimMode ranged enum, and their menu rows',doc)
print(d['kinds'],d['nitems'])
