import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 358 (RVA 0x786f50, 1769 bytes), batch 12 debug page (attract mode).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page adds two row kinds to the generator: the unbounded int (0x3a980, early release) and the console command (0xf2f0 name view, 0x20af0 flags 2, register-once through [0x1594dc0 + 0x20], atexit 0x4b0); a toggle over the command object keeps the default range because the command class answers virtual slot 3 with 0x22ae0 (xor eax,eax; ret). Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(358,'Attract-mode page (inference from the strings): attractMovieEnabled / attractMovieTime / cp10_AttractMaxTimesToLoop variables, the TriggerAttractMode command and the CP10 AttractMode toggles.','Menu: CP10 / AttractMode group with four 0x26ea0 toggles (bool, unbounded float, command, unbounded int).','bool, unbounded float, unbounded int, console command, 0x26c10 group and four 0x26ea0 toggles',doc)
print(d['kinds'],d['nitems'])
