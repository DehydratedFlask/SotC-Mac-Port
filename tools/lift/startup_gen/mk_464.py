import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 464 (RVA 0x3db070, 697 bytes), batch 14, a bool plus records.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The 5 named records go through the registrar 0xa81700 (byte-pinned in check_helper_a81700.py; whole-body hashes here). The group destructor 0x3d9b40 unregisters the block's records last first (shape checked, hashed, never run). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(464,'A bool variable and five named records (0xa81700 registrar) with a group destructor 0x3d9b40 recorded (inference: a name table; the consumer is not established).','bool plus records','one bool, five named records and a group destructor',doc)
print(d['kinds'],d['nitems'])