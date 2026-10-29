"""Lane-M6 extension of the lane-L4 single-slot ELF checker driver (l4_slot_lib).

`check_ctor_slot_<N>.py` for the lane-M6 slots (544 557 558 559 569 448 379 518 507 539) import this module
instead of l4_slot_lib. It re-uses the L4 driver unchanged (pins, strings, lengths, bool/float functor tags,
native token checks, mutation self-test, --tamper) and adds, through two optional SPEC keys, only what the L4
driver cannot express:

  m6_callees   {name: (rva, size, sha256)}  extra shared callee / destructor / helper bodies, hash-pinned
                                            (read with always=True, exactly like COMMON_CALLEES)
  m6_extras    [names]                      named byte checks from EXTRAS below
  m6_int_tags  [tags]                       int-functor caller tags (used by the "int_site" extra)
  m6_bytes     [(rva, hex, what)]           exact byte runs (float constants in .rodata, initial values in .data)

Every extra check reads the original bytes only through the recording `Elf` accessors, so the L4 mutation
self-test flips them like any other pin: a mutant that survives is a failure of this checker.

Offline only: reads the pinned private ELF (local/re/eboot.elf); nothing is loaded, relocated or executed.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as base  # noqa: E402
from slot_pin_util import require  # noqa: E402

INT_FN_BODIES = (0xC720, 0xC740, 0xC760, 0xC770)
INT_CLASS_FN_VTBL = 0x15944A0        # stored by the int functor copy / clone bodies
INT_FINAL_CLASS = 0x1594C08          # 0x1594bf8 + 0x10, stored by the caller after 0xc580
FLOAT_FINAL_CLASS = 0x15942B0
MEMSET_THUNK = 0xF8F4F0
MEMSET_GOT = 0x1600068


def x_memset_thunk(e, spec) -> None:
    """0xf8f4f0 is the 6-byte `jmp [rip+disp]` PLT thunk of `memset` (NID equality: tools/lift/check_ff8f4f0.py)."""
    e.bytes_at(MEMSET_THUNK, "ff25", "memset thunk is jmp [rip+disp]")
    require(e.rip(MEMSET_THUNK, 6, 2) == MEMSET_GOT, "memset thunk GOT slot 0x1600068")


def x_fcmd_vtable(e, spec) -> None:
    """The console-command class vtable 0x1594dc0 slot 4 (`call [vtbl + 0x20]`) is the registration 0x20d60."""
    require(e.reloc_target(0x1594DC0 + 0x20) == 0x20D60, "console command vtable slot 4 = 0x20d60")


def x_int_site(e, spec) -> None:
    """Caller tags of the ranged-int (0xc580) site have the same four slot bodies as the lifted class 0x15944a0."""
    for tag in list(spec.get("m6_int_tags", ())) + [INT_CLASS_FN_VTBL]:
        for i, body in enumerate(INT_FN_BODIES):
            require(e.reloc_target(tag + 8 * i) == body, f"int functor vtable {tag:#x} slot {i}")
    e.bytes_at(0xC770, "c3", "int functor destroy is a bare ret")
    require(e.rip(0xC724, 7, 3) == INT_CLASS_FN_VTBL, "int functor copy stores the class tag 0x15944a0")
    require(e.rip(0xC740, 7, 3) == INT_CLASS_FN_VTBL, "int functor clone stores the class tag 0x15944a0")
    require(e.rip(0xC5BB, 7, 3) == 0x1594190, "0xc580 mid vtable 0x1594190")
    require(e.rip(0xC5C6, 7, 3) == INT_CLASS_FN_VTBL, "0xc580 functor class 0x15944a0")
    e.bytes_at(0xC5CD, "4883c010", "0xc580: mid vptr = table + 0x10")
    require(e.reloc_target(INT_FINAL_CLASS) == 0x18A60, "int class slot 0 = 0x18a60")
    require(e.reloc_target(INT_FINAL_CLASS + 0x10) == 0x20D60, "int class slot 2 = 0x20d60")
    require(e.reloc_target(INT_FINAL_CLASS + 0x18) == 0xBF70, "int class slot 3 = 0xbf70")
    require(e.reloc_target(INT_FINAL_CLASS + 0x30) == 0xBFC0, "int class slot 6 = 0xbfc0")
    e.bytes_at(0xBF70, "b801000000c3", "int slot 3: mov eax,1; ret")
    e.bytes_at(0xBFC0, "b802000000c3", "int slot 6: mov eax,2; ret")


def x_float_menu(e, spec) -> None:
    """The float class 0x15942b0 answers the toggle's virtual reads with slot 3 = 1 and slot 6 = 3."""
    require(e.reloc_target(FLOAT_FINAL_CLASS + 0x18) == 0xBBA0, "float class slot 3 = 0xbba0")
    require(e.reloc_target(FLOAT_FINAL_CLASS + 0x30) == 0xBBF0, "float class slot 6 = 0xbbf0")
    e.bytes_at(0xBBA0, "b801000000c3", "float slot 3: mov eax,1; ret")
    e.bytes_at(0xBBF0, "b803000000c3", "float slot 6: mov eax,3; ret")


def x_toggle_gate(e, spec) -> None:
    """0x26ea0: default range {1.0f, 10.0f} and the `slot3 == 1 && slot6 == 3` narrowing gate."""
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")


EXTRAS = {
    "memset_thunk": x_memset_thunk,
    "fcmd_vtable": x_fcmd_vtable,
    "int_site": x_int_site,
    "float_menu": x_float_menu,
    "toggle_gate": x_toggle_gate,
}

_l4_run_elf_checks = base.run_elf_checks


def run_elf_checks(e, spec: dict) -> None:
    _l4_run_elf_checks(e, spec)
    for name, (rva, size, sha) in spec.get("m6_callees", {}).items():
        e.body_hash(name, rva, size, sha, always=True)
    for rva, hexstr, what in spec.get("m6_bytes", ()):
        e.bytes_at(rva, hexstr, what)
    for name in spec.get("m6_extras", ()):
        EXTRAS[name](e, spec)


# The L4 driver looks `run_elf_checks` up as a module global (record run, mutation run, --tamper), so replacing
# it here makes every phase include the lane-M6 extras.
base.run_elf_checks = run_elf_checks

main = base.main
