import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 381 (RVA 0x6cd5b0, 3059 bytes), batch 12 debug page (weapon multipliers, menu-less).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page has NO menu rows. Three console commands (0xf2f0 name view, 0x20af0 flags 2, register-once through [0x1594dc0 + 0x20], atexit 0x4b0) and six fnv1a name cells follow the variables. Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(381,'Combat-tuning page (inference from the strings): five shadow_weaponAttackMultiplier_* bounded floats, shadow_InstantKillPercentage / EasyMode damage multipliers, three EasyMode console commands and six NameHashCell achievement counters.','No menu rows.','five bounded floats, three unbounded floats (one late release), three console commands, six name cells (menu-less)',doc)
print(d['kinds'],d['nitems'])
