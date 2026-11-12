#!/usr/bin/env python3
"""Shared ELF proof for startup slots that call a member of the local ranged-int wrapper family (0x620e10 and its 25 twins, see check_helper_620e10.py).

The extra `lint_sites` proves, for every call site listed in SPEC["lint_sites"], the PER-SITE facts that make the native `LocalIntSiteSpec` admissible:
  * the callee is a byte twin of the hash-pinned wrapper 0x7ec160 outside its seven rel32 operands, and its inner constructor a twin of 0xc580;
  * the argument-functor tag the CALLER loads is the functor class the WRAPPER itself loads (+0x5a), or a class-table ALIAS of it (all four slot targets equal; the native twin list pins the pair);
  * the destructor the caller registers with __cxa_atexit is slot 0 of the wrapper's final vtable (+0x22, table + 0x10);
  * the functor class has the four int-body slots; the final class slots 3 / 6 are `mov eax,1; ret`;
  * the caller's description argument is the static empty string call 0xf730 (or, for a described site, a view released by 0xf590 right after the wrapper), the functor destroy is `call [tag+0x18]` and the name view is released with 0xf590 in between.
The instruction operands themselves (object, name, init / range / flags) are pinned by the generated checker's own pins; this extra adds the cross-function ties.

Offline only: reads the pinned private ELF (local/re/eboot.elf); nothing is loaded, relocated or executed.
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
from slot_pin_util import require  # noqa: E402

TEMPLATE_WRAPPER = 0x7EC160
WRAPPER_SIZE = 0x1B3
INNER_SIZE = 0x15F
RANGED_INT_CTOR = 0xC580
GUARD = 0x15FE518
WRAPPER_WINDOWS = ((0x1E, 4), (0x25, 4), (0x5D, 4), (0x158, 4), (0x188, 4), (0x18F, 4), (0x1AF, 4))
INNER_WINDOWS = ((0x14, 4), (0x37, 4), (0x3E, 4), (0x49, 4), (0x13E, 4), (0x15B, 4))
INT_FN_BODIES = ((0xC720, 20, ((7, 4),), 4), (0xC740, 20, ((3, 4),), 0), (0xC760, 9, (), None), (0xC770, 1, (), None))


def same_outside(e, a: int, b: int, size: int, windows, what: str) -> None:
    cursor = 0
    for off, length in tuple(windows) + ((size, 0),):
        if off > cursor:
            require(e.read(a + cursor, off - cursor) == e.read(b + cursor, off - cursor),
                    f"{what}: {a:#x} equals {b:#x} over [+{cursor:#x}, +{off:#x})")
        cursor = off + length


def x_lint_sites(e, spec) -> None:
    sites = spec["lint_sites"]
    require(len(sites) >= 1, "at least one local-int site")
    for s in sites:
        w = s["wrapper"]
        inner = w + 0x1C0
        require(e.call(s["call"]) == w, f"site {w:#x}: caller calls the wrapper")
        same_outside(e, TEMPLATE_WRAPPER, w, WRAPPER_SIZE, WRAPPER_WINDOWS, "wrapper")
        same_outside(e, RANGED_INT_CTOR, inner, INNER_SIZE, INNER_WINDOWS, "inner")
        require(e.call(w + 0x157) == inner, f"{w:#x}: inner constructor call")
        require(e.call(w + 0x187) == 0x20D60, f"{w:#x}: registration call")
        fn = e.rip(w + 0x5A, 7, 3)
        final = e.rip(w + 0x22, 7, 3) + 0x10
        require(fn == s["own"], f"{w:#x}: lint_sites own class is the wrapper's own functor class")
        if s["tag"] != fn:   # a caller twin: a different table with the same four slot targets as the wrapper's own class
            for index in range(4):
                require(e.reloc_target(s["tag"] + 8 * index) == e.reloc_target(fn + 8 * index), f"{w:#x}: caller tag {s['tag']:#x} slot {index} aliases the wrapper's own class {fn:#x}")
        require(e.rip(s["tag_lea"], 7, 3) == s["tag"], f"{w:#x}: caller loads that tag")
        require(e.rip(inner + 0x46, 7, 3) == fn, f"{inner:#x}: inner loads the same functor class")
        require(e.rip(w + 0x5A, 7, 3) == fn, f"{w:#x}: wrapper loads its own class")
        require(e.reloc_target(final) == s["dtor"], f"{w:#x}: final class slot 0 is the atexit destructor {s['dtor']:#x}")
        require(e.rip(s["dtor_lea"], 7, 3) == s["dtor"], f"{w:#x}: caller registers that destructor")
        require(e.rip(s["obj_lea"], 7, 3) == s["obj"], f"{w:#x}: object lea")
        if s.get("desc_f590"):   # described site (7ec160 order): wrapper ; f590(description) ; call [tag + 0x18] ; f590(name) ; atexit
            require(s["desc_f590"] == s["call"] + 0xC and e.call(s["desc_f590"]) == 0xF590, f"{w:#x}: f590 releases the description view right after the wrapper (add rsp,0x10 ; mov r12,rdi ; call f590)")
        else:
            require(e.call(s["f730"]) == 0xF730, f"{w:#x}: empty description is call 0xf730")
        e.bytes_at(s["ind"], "ff5018", f"{w:#x}: call [tag + 0x18] (functor destroy)")
        require(e.call(s["ind"] + 6) == 0xF590, f"{w:#x}: f590 releases the name view")
        require(e.call(s["atexit_call"]) == 0xF8F390, f"{w:#x}: __cxa_atexit")
        slot3, slot6 = e.reloc_target(final + 0x18), e.reloc_target(final + 0x30)
        e.bytes_at(slot3, "b801000000c3", f"{w:#x}: slot 3 is mov eax,1; ret")
        e.bytes_at(slot6, "b801000000c3", f"{w:#x}: slot 6 is mov eax,1; ret")
        for index, (int_body, size, windows, class_operand) in enumerate(INT_FN_BODIES):
            body = e.reloc_target(fn + 8 * index)
            same_outside(e, int_body, body, size, windows, f"{fn:#x} functor slot {index}")
            if class_operand is not None:
                require(e.rip(body + (4 if index == 0 else 0), 7, 3) == fn, f"{fn:#x} functor slot {index} stores its own class")


bp.lib.EXTRAS["lint_sites"] = x_lint_sites
