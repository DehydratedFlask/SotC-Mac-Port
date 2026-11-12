import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 445 (RVA 0x4cfe80), batch 20 UI option page: four name holders (two holder classes, destructors 0x4cb9e0 / 0x4cba00), the uiForceAllowExtendedVideoModes bool and 79 hash-to-dword stores (74 distinct rodata UI strings hashed by the path-folding FNV 0xa2ddc0, stored one dword each into a 0x164 B window at 0x2efb1e0), plus the __cxa_atexit records.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every holder, the bool and each atexit record is pinned operand by operand. The hash-store run is replayed from the raw bytes by a closed decoder (tools/lift/hash_store_bytes.py) that must agree store for store with the objdump-text decoder of the generator; the hash itself is checked against an independent Python port and the native hash (fnv_cstr_a2ddc0). The callee-saved register contract of 0xa2ddc0 / 0x162020 is pinned by its prologue, epilogue and body hash. Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the hashed dwords at run time is NOT established (the strings look like UI option / button ids: inference only).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(445,'UI option page (inference from the strings): four name holders, the uiForceAllowExtendedVideoModes bool and 79 hashed UI option string ids stored as dwords.','UI option page batch 20: four name holders, a bool and a hash-to-dword table','four name holders, one bool and 79 hash-to-dword stores',doc)
print(d['kinds'],d['nitems'],d['menu_less'])
