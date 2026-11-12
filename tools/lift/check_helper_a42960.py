#!/usr/bin/env python3
"""ELF-tie checker for the availability entry constructor 0xa42960 (CPU-01-AOT-CFG-SCALE-01-BATCH19).

Pins instruction by instruction: 0xa42960 (590 B, FDE 590): two 0xf180 empty-string initialisers (entry, entry + 0x20), `+0x40 = 0`, the two guarded
0xf590 + 32 B copies with `lock inc`, the 0xc8 B 0x17070 allocation, the 0x10a90 Format of "entitlement_available_%s" (edx 0x18, rcx = entry->a.data),
the bool var construction (0x20af0 flags 1, mid vptr 0x15940a0 + 0x10, value_ref = var + 0xc0, zeros, fn tag 0x15d17b0, 0xf590 on the description, final
vptr 0x1594208 + 0x10, value 0, default 0, 0x20d60), the unreachable `call [rax+8]` delete of a previous var, the 0xf590 release of the name, and the
inline doubling push on V2 (0x32f6f38 / 0x32f6f40 / 0x32f6f48; 0x17090 / memmove / 0x170c0). The format string and the empty description are read
from the ELF. memmove and __stack_chk_fail are NID-proven. The whole image is scanned for E8 callers: exactly 6, all inside the hash-pinned slot 428.

NOT established: what the strings mean; the body of 0x10a90 (variadic Format; the native model is a snprintf stand-in for this ONE format string); the
virtual delete target (unreachable: +0x40 is zeroed first); the exit-time teardown of V2.

Usage: python3 -I tools/lift/check_helper_a42960.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import check_helper_a59620 as a59  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_g as pins_g  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_g.PINS)


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F480, 0x1600030, "memmove"), (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail")))
    base, size, sha = a59.SLOT_428
    fde = lib.fde_ranges()
    if fde is not None:
        require(fde.get(hex(base)) == size, "slot 428: FDE length")
    e.body_hash("slot 428 (0x52ec10)", base, size, sha, always=True)
    found = a59.callers(e, 0xA42960)
    require(len(found) == 6, f"0xa42960 has exactly 6 E8 callers in the image, got {len(found)}")
    require(all(base <= c < base + size for c in found), "every caller of 0xa42960 lies inside slot 428")
    require(e.raw(0x12C2D57, 25) == b"entitlement_available_%s\x00", "the Format string at 0x12c2d57")
    require(len(b"entitlement_available_%s") == 0x18, "the Format length hint (edx 0x18) is the string length")
    require(e.raw(0x12C2E23, 1) == b"\x00", "the empty description string at 0x12c2e23")


SPEC = {
    "name": "check_helper_a42960",
    "summary": ("0xa42960 (590 B) pinned per instruction (2 x f180, +0x40 = 0, 2 guarded copies, 0xc8 B var, Format \"entitlement_available_%s\", bool var "
                "shape without atexit, 0x20d60, name release, V2 doubling push); memmove / __stack_chk_fail NID-proven; 6 callers, all inside the "
                "hash-pinned slot 428"),
    "families": [],
    "bodies": [
        {"name": "0xa42960 availability entry ctor", "rva": 0xA42960, "size": 590, "fde": 590, "pins": "avail_entry_a42960",
         "sha": "5b1e59677243c0238fb7da1f616b5e49d63e55c0a0de4a713d84798fbe81f456"},
    ],
    "rips": [
        ("stack protector load", 0xA42971, 7, 3, 0x15FE518), ("stack protector reload", 0xA42B89, 7, 3, 0x15FE518),
        ("Format string", 0xA42A11, 7, 3, 0x12C2D57), ("final vptr base", 0xA42A28, 7, 3, 0x1594208),
        ("empty description", 0xA42A33, 7, 3, 0x12C2E23), ("mid vptr base", 0xA42A71, 7, 3, 0x15940A0),
        ("functor tag", 0xA42A7F, 7, 3, 0x15D17B0),
        ("V2 end load", 0xA42AF6, 7, 3, 0x32F6F40), ("V2 cap compare", 0xA42AFD, 7, 3, 0x32F6F48),
        ("V2 end store", 0xA42B0A, 7, 3, 0x32F6F40), ("grow: begin load", 0xA42B16, 7, 3, 0x32F6F38),
        ("grow: begin store", 0xA42B74, 7, 3, 0x32F6F38), ("grow: end store", 0xA42B7B, 7, 3, 0x32F6F40),
        ("grow: cap store", 0xA42B82, 7, 3, 0x32F6F48),
    ],
    "calls": [
        ("f180 on the entry", 0xA42988, 0xF180), ("f180 on entry + 0x20", 0xA42994, 0xF180),
        ("release entry.a", 0xA429A9, 0xF590), ("release entry.b", 0xA429D8, 0xF590),
        ("var allocation 0x17070", 0xA42A06, 0x17070), ("Format 0x10a90", 0xA42A23, 0x10A90),
        ("bool var base ctor 0x20af0", 0xA42A6C, 0x20AF0), ("release the description", 0xA42ABA, 0xF590),
        ("register 0x20d60", 0xA42AD0, 0x20D60), ("release the name", 0xA42AF1, 0xF590),
        ("grow allocate 0x17090", 0xA42B44, 0x17090), ("grow memmove", 0xA42B55, 0xF8F480), ("grow free 0x170c0", 0xA42B6B, 0x170C0),
        ("stack_chk_fail", 0xA42BA8, 0xF8F3A0),
    ],
    "jmps": [],
    "relocs": [],
    "bytes": [
        ("entry.var = 0", 0xA42999, "49c7464000000000"),
        ("delete-old: call [rax+8] (unreachable, +0x40 was zeroed)", 0xA42AE6, "ff5008"),
        ("push fast path: jmp short to the stack-protector epilogue", 0xA42B14, "eb73"),
        ("Format length hint edx = 0x18", 0xA42A1C, "ba18000000"),
        ("var allocation size edi = 0xc8", 0xA42A01, "bfc8000000"),
        ("value byte +0xc0 = 0", 0xA42AC5, "c683c000000000"),
        ("default byte +0x68 = 0", 0xA42ACC, "c6436800"),
        ("base ctor flags ecx = 1", 0xA42A3E, "b901000000"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_a42960_avail_entry.hpp", "helper_c": "native/core/helper_a42960_avail_entry.cpp",
        "test": "native/tests/test_helper_a42960.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kAvailEntryCtorRva": 0xA42960, "kAvailEntryCtorSize": 590, "kAvailEntryVecBeginRva": 0x32F6F38,
                     "kAvailEntryStride": 0x48, "kAvailVarAllocSize": 0xC8, "kAvailVarFnVtbl": 0x15D17B0, "kAvailNameLength": 0x18},
    },
    "tokens": {
        "helper_h": ["static_assert(sizeof(ConCmdBoolVar) == kAvailVarAllocSize);",
                     "static_assert(sizeof(AvailEntry) == kAvailEntryStride && offsetof(AvailEntry, b) == 0x20 && offsetof(AvailEntry, var) == 0x40);"],
        "helper_c": ["init_slot_f180(&entry.a);", "init_slot_f180(&entry.b);", "entry.var = nullptr;", "if (&entry.a == &a) {",
                     "if (&entry.b == &b) {", "copy_shared_str_inc(entry.a, a);", "copy_shared_str_inc(entry.b, b);",
                     "alloc_17070(kAvailVarAllocSize, hooks.alloc)", "\"entitlement_available_%s\"",
                     "init_bool_var_vector(*var, name, description, kAvailVarFnVtbl, registry, hooks.name_dealloc, 0, 0)",
                     "entry.var = var;", "push_ptr_doubling(vec, &entry, hooks.alloc)"],
        "test": ["CHECK(var.value == 0 && var.default_value == 0 && var.value_ref == &var.value);",
                 "CHECK(std::memcmp(var.base.name.data, \"entitlement_available_FullGame\", 30) == 0);",
                 "CHECK(registry.atexit_log.size() == 0);", "r.stage == AvailEntryStage::var_alloc_failed",
                 "r.push_refusal == PtrVecRefusal::allocation_failed"],
        "cmake": ["core/helper_a42960_avail_entry.cpp", "add_test(NAME helper_a42960 COMMAND test_helper_a42960)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
