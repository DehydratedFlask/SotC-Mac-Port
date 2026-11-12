import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 566 (RVA 0x25de0, 1194 bytes), batch 18 debug page (build info: RetailBuild, BuildVersion, ToolServerIP, MirrorWorld and the memoryToStringDisplayUnits variable with its Debug menu row).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, the caller functor tags, the final vtable pointers and the __cxa_atexit records. The page contains the first TWIN-B unbounded uint variable (0x25ab0): unlike twin A (0x140b00,
slot 534) its object is only 0xd0 bytes (the next object, the toggle item at 0x2dfd370, starts at +0xd0), its value is the EXTERNAL dword at obj-8 (0x2dfd298), and the site neither pre-stores the final vptr nor writes a value:
it loads that dword into [obj+0x68] (the value dword is an INPUT; default = the ELF initial image of the global, .bss has none). The native Objects struct reserves the cell and the 0xd0 B window address-tied, and places the 0xd8 B
ConCmdRangedVar model right after the window (its +0xd0 tail is not part of the original and is never used). Final class 0x1595098, atexit destructor 0x23d20, seed class 0x1595100 are identity tags. Plain stores of the body
that belong to no row are pinned only by the body hash and the per-instruction pins: they are NOT modelled. What reads the variables at run time is NOT established.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(566,'Debug page (inference from the strings): build info variables (RetailBuild, BuildVersion, ToolServerIP, MirrorWorld), the twin-B memoryToStringDisplayUnits variable with an external value cell and its Debug menu row.','debug page batch 18: build info variables, one twin-B uint with an external value cell and a menu row','build info variables, a twin-B uint with an external value cell and one menu row',doc)
print(d['kinds'],d['nitems'])
