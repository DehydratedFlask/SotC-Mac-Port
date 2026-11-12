import sys
import pathlib as _pl
sys.path.insert(0, str(_pl.Path(__file__).resolve().parent))
from mk_page import *
doc='''ELF byte-oracle for startup constructor slot 501 (RVA 0x24db80, 3244 bytes), batch 20 debug page (Animation: r_showBounds / r_showSkeleton / r_showJointNames / r_showSkeletonScale / r_showSkeletonWorldSpace / r_highlightSkeletonJoint / r_enableAnimations variables, a group, a kind-0 label item built by 0x26800 and eight toggle items).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The string variable r_highlightSkeletonJoint takes its initial view from the RETURN VALUE of 0xf730 (the function-local static empty string; the generator checks that `movq %rax, %rdx` is the only write to rax / rdx between the call and 0x257a0, so the initial view is exactly that static and it is never released); 0xf730 is hash-pinned. The label item (0x26800, 173 B, hash-pinned) reuses the native model init_slot326_label_item_26800 (kind 3 stored, then overwritten with 0). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inferences from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
'''
d=build(501,'Animation debug page (inference from the strings): skeleton / bounds / joint-name display variables, a highlighted-joint string variable with an empty default, one group, one label item and eight toggles.','debug page batch 20: animation display variables, a string variable with an empty default, a group, a label item and toggles','seven variables, a string variable with an empty initial view, a group, a label item and eight toggles',doc)
print(d['kinds'],d['nitems'],d['menu_less'])
