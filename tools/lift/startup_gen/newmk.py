"""usage: newmk.py <slot> <title> <what> <theme> <summary> [extra-doc-sentence]  -- writes mk_<slot>.py for a debug-page slot (batch-11 helper)."""
import sys
sys.path.insert(0,'.')
from rows import e,fde
import debug_page_batch_gen as g
n=int(sys.argv[1]); title,what,theme,summary=sys.argv[2:6]; extra=sys.argv[6] if len(sys.argv)>6 else ''
rva=e.u64(g.TABLE_TOP-8*n); size=fde(rva)
src=open('mk_398.py').read()
import re
head=src.split("doc='''")[0]
doc=f"""doc='''ELF byte-oracle for startup constructor slot {n} (RVA {rva:#x}, {size} bytes), batch 12 debug page{title}.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. {extra}Local ranged-int variables (if any) go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) or to a class-table alias of it, and its atexit destructor to slot 0 of the wrapper's final class (per-site tag
proof, never inherited from another slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build({n},{what!r},{theme!r},{summary!r},doc)
print(d['kinds'],d['nitems'])
"""
open(f'mk_{n}.py','w').write(head+doc)
print(n,hex(rva),size)
