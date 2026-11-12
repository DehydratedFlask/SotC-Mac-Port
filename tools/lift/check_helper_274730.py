#!/usr/bin/env python3
"""ELF-tie checker for the out-of-line bool console-variable constructor 0x274730 (661 B) and the bool functor family it admits (CPU-01-HELPER-CALLEE-MODELS-04).

Pins instruction by instruction: 0x274730 (the stack protector, the two staging functor blocks, the clone / copy / copy / copy / aux-swap install through the caller's
class and the staging class 0x159ca60, the 0x20af0 base constructor with the fixed empty description 0xfc521a and flags 1, the object stores [+0x60] [+0x69] [+0xa0] [+0xb0]
[+0xb8] and the ymm copy to [+0x70] with the q[3] self-reference rule, f590 on the description, the bare-ret destroy, the final vptr 0x1594218, the byte stores to [+0xc0] /
[+0x68], 0x20d60) and the four family slot bodies 0xc2a0 (copy), 0xc2c0 (clone), 0xc2e0 (move), 0xc2f0 (destroy) whose class constant is 0x1594280. The bool functor family is
recomputed from the relocation table (every aligned group of four RELATIVE words = {0xc2a0, 0xc2c0, 0xc2e0, 0xc2f0}: 143 tables) and the native array
`kBoolFunctorTags` must equal it in both directions. The 14 direct callers are re-derived by a whole-text scan (no jmp tail call, never address-taken); the five owning bodies
(startup slots 493 / 473 / 427 / 422 and the non-slot body 0x523bd0) are hash-pinned, each must contain exactly its listed calls and load only the listed bool-family table(s).
The first site of slot 493 (0x2777a4) is pinned instruction by instruction from the argument block to the __cxa_atexit record.

NOT established: which tag each of the 13 other sites passes (only the per-body bool-family table set is proven, not the register flow); the meaning of the callbacks q[1] / q[3]
(0x274710, 0x26fcf0, identity only); that the destroy `call [argfn + 0x18]` at the other sites is the same bare ret beyond the family membership; the debug-tool use of the object.

Usage: python3 -I tools/lift/check_helper_274730.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_e as pins_e  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_e.PINS)
BOOL_SLOTS = (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)
lib.FAMILY_SLOTS["bool"] = BOOL_SLOTS
lib.FAMILY_ARRAY["bool"] = "kBoolFunctorTags"

TARGET = 0x274730
TEXT_END = 0xF8F000
CALLS = (0x2777A4, 0x355D19, 0x355DCD, 0x356042, 0x3560F1, 0x35619D, 0x356249, 0x3562F5, 0x3563A1, 0x35644D, 0x524CB4, 0x5390C4, 0x5392AD, 0x56C248)
# (name, base, size, sha256, expected calls to 0x274730, expected bool-family tables loaded by lea)
OWNERS = (
    ("slot 493 (0x277560)", 0x277560, 1088, "83320f9aa5a6a7bc9d5586978b9b764cb5557f00fbf87adbd6c49eaa9ff6058b", (0x2777A4,), {0x159CA60}),
    ("slot 473 (0x355a00)", 0x355A00, 6487, "2ec7a932dd8bed1a4b7cfc24091e926e6068bcd295a6124e9aaa3a86dd11fcf8",
     (0x355D19, 0x355DCD, 0x356042, 0x3560F1, 0x35619D, 0x356249, 0x3562F5, 0x3563A1, 0x35644D), {0x15AA860}),
    ("slot 427 (0x5388f0)", 0x5388F0, 4362, "aa202ea2138463351225cbf3bbf3a4b3367ada23597fd87b87bcbeb7b586747e", (0x5390C4, 0x5392AD), {0x15C8DB8}),
    ("slot 422 (0x56b980)", 0x56B980, 2682, "8e29b66eb02ad4d10fbfedb164c42dceff120de8c5d640c598f10fbf5a35c541", (0x56C248,), {0x15CA808}),
    ("non-slot body 0x523bd0", 0x523BD0, 4626, "e41d67bb20a27129b52742e364f3be386bf53407fbcafd93bb29008d044c2053", (0x524CB4,), {0x15C8248}),
)
SITE493_RANGE = 0x277723


def x_callers(e) -> None:
    for site in CALLS:
        require(e.call(site) == TARGET, f"caller at {site:#x} reaches 0x274730")
    if e.skip_hash:                         # whole-image scans behave like the body hashes: skipped in the byte-mutation run
        return
    text = e.raw(0, TEXT_END)
    calls, jmps = [], []
    for opcode, found in ((0xE8, calls), (0xE9, jmps)):
        pos = text.find(bytes([opcode]))
        while pos != -1:
            if pos + 5 <= len(text) and pos + 5 + struct.unpack("<i", text[pos + 1:pos + 5])[0] == TARGET:
                found.append(pos)
            pos = text.find(bytes([opcode]), pos + 1)
    require(calls == sorted(CALLS), f"exactly the 14 listed direct callers of 0x274730, got {[hex(c) for c in calls]}")
    require(jmps == [], "no jmp rel32 tail call to 0x274730")
    require(not [a for a, r in e.relocs.items() if r["type"] == 8 and r["addend"] in (TARGET, TARGET ^ 1)],
            "no relocation addend names 0x274730 (never address-taken)")


def x_owners(e, fams) -> None:
    bool_family = set(fams["bool"])
    fde = lib.fde_ranges()
    seen = []
    for name, base, size, sha, calls, tables in OWNERS:
        if fde is not None:
            require(fde.get(hex(base)) == size, f"{name}: FDE length")
        e.body_hash(name, base, size, sha, always=True)
        body = e.raw(base, size)
        found = tuple(base + i for i in range(size - 4) if body[i] == 0xE8
                      and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == TARGET)
        require(found == calls, f"{name}: exactly the listed calls to 0x274730, got {[hex(x) for x in found]}")
        seen.extend(found)
        loaded = set()
        for i in range(size - 6):
            if body[i] in (0x48, 0x4C) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05:
                target = base + i + 7 + struct.unpack("<i", body[i + 3:i + 7])[0]
                if target in bool_family:
                    loaded.add(target)
        require(loaded == tables, f"{name}: the bool-family tables loaded by lea are {sorted(hex(t) for t in tables)}, got "
                                  f"{sorted(hex(t) for t in loaded)}")
    require(sorted(seen) == sorted(CALLS), "the five owning bodies cover exactly the 14 call sites")


def x_site493(e, fams) -> None:
    """Slot 493's first site: argument block, tag, callbacks, call, destroy, f590(name), atexit(0x20b0)."""
    for off, hexstr in pins_e.PINS["site493_range_277723"]:
        e.bytes_at(SITE493_RANGE + off, hexstr, f"site 493 pin +{off:#x}")
    e.bytes_at(0x277756, "c6855fffffff01", "site 493: initial byte = 1")
    base = SITE493_RANGE
    require(e.rip(base, 8, 4) == 0x15FEC90, "site 493: xmm1 loaded from 0x15fec90 (q[1], q[2])")
    require(e.reloc_target(0x15FEC90) == 0x274710, "site 493: q[1] is the callback 0x274710 (RELATIVE word)")
    require(e.lea(base + 0x0F) == 0x2EEF1E8, "site 493: this = 0x2eef1e8")
    require(e.lea(base + 0x1D) == 0x159CA60 and 0x159CA60 in fams["bool"], "site 493: the argument functor class is the bool table 0x159ca60")
    require(e.lea(base + 0x41) == 0x26FCF0, "site 493: q[3] is the callback 0x26fcf0")
    require(e.call(base + 0x81) == TARGET, "site 493: call 0x274730")
    require(e.lea(base + 0x99) == 0x20B0, "site 493: atexit destructor 0x20b0")
    require(e.lea(base + 0xA0) == 0x1604218, "site 493: atexit DSO handle")
    require(e.call(base + 0xAA) == 0xF8F390, "site 493: __cxa_atexit")
    require(e.call(base + 0x94) == 0xF590, "site 493: f590(name)")
    # the family table's slot 3 is the bare ret the `call [tag + 0x18]` reaches
    require(e.reloc_target(0x159CA60 + 0x18) == BOOL_SLOTS[3], "site 493: [tag + 0x18] is the bare-ret destroy 0xc2f0")
    require(e.reloc_target(0x159CA60 + 0x08) == BOOL_SLOTS[1], "site 493: [tag + 0x08] is the clone 0xc2c0")


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),))
    for table in (0x1594280, 0x159CA60):
        require(table in fams["bool"], f"{table:#x} is a bool functor table")
    x_callers(e)
    x_owners(e, fams)
    x_site493(e, fams)


SPEC = {
    "name": "check_helper_274730",
    "summary": ("0x274730 (661 B bool-variable ctor) + the four bool functor slot bodies 0xc2a0 / 0xc2c0 / 0xc2e0 / 0xc2f0 pinned per instruction; the 143-table bool functor "
                "family recomputed from relocations and tied to the native array both ways; 14 direct callers re-derived; five owning bodies hash-pinned; "
                "slot 493's first site pinned from argument block to atexit"),
    "families": ["bool"],
    "family_count": {"bool": 143},
    "bodies": [
        {"name": "0x274730 bool variable ctor", "rva": TARGET, "size": 661, "fde": 661, "pins": "bool_var_274730",
         "sha": "824c83fdb9d2ed82d7589f4ae86e7dac1d0f95743c5760d7f7443a93aa437106"},
        {"name": "0xc2a0 bool functor copy", "rva": 0xC2A0, "size": 20, "fde": 20, "pins": "bool_copy_c2a0",
         "sha": "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"},
        {"name": "0xc2c0 bool functor clone", "rva": 0xC2C0, "size": 20, "fde": 20, "pins": "bool_clone_c2c0",
         "sha": "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"},
        {"name": "0xc2e0 bool functor move", "rva": 0xC2E0, "size": 9, "fde": 9, "pins": "bool_move_c2e0",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "0xc2f0 bool functor destroy", "rva": 0xC2F0, "size": 1, "fde": 1, "pins": "bool_destroy_c2f0",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
    ],
    "rips": [
        ("stack guard load", 0x274744, 7, 3, 0x15FE518), ("pre-store vptr table", 0x27474B, 7, 3, 0x1594208),
        ("staging class (first)", 0x274763, 7, 3, 0x159CA60, "bool"), ("empty description string", 0x274844, 7, 3, 0xFC521A),
        ("mid vptr table", 0x27487D, 7, 3, 0x15940A0), ("staging class (second)", 0x274884, 7, 3, 0x159CA60, "bool"),
        ("stack guard reload", 0x27499D, 7, 3, 0x15FE518),
        ("copy slot class constant", 0xC2A4, 7, 3, 0x1594280, "bool"), ("clone slot class constant", 0xC2C0, 7, 3, 0x1594280, "bool"),
    ],
    "calls": [
        ("0x20af0 base ctor", 0x274878, 0x20AF0), ("f590 on the description", 0x274968, 0xF590), ("0x20d60 register", 0x274998, 0x20D60),
        ("__stack_chk_fail", 0x2749BF, 0xF8F3A0),
    ],
    "jmps": [],
    "relocs": [("class table 0x159ca60 slot 0", 0x159CA60, 0xC2A0), ("class table 0x159ca60 slot 1", 0x159CA68, 0xC2C0),
               ("class table 0x159ca60 slot 2", 0x159CA70, 0xC2E0), ("class table 0x159ca60 slot 3", 0x159CA78, 0xC2F0)],
    "bytes": [("flags = 1 before 0x20af0", 0x274856, "b901000000")],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_274730_bool_var.hpp", "helper_c": "native/core/helper_274730_bool_var.cpp",
        "tags_h": "native/core/helper_bool_functor_tags.hpp", "tags_c": "native/core/helper_bool_functor_tags.cpp",
        "test": "native/tests/test_helper_274730.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kBoolVarCtorRva": TARGET, "kBoolVarCtorSize": 661, "kBoolVarCtorStagingClass": 0x159CA60,
                     "kBoolVarCtorEmptyDescRva": 0xFC521A, "kBoolVarCtorFlags": 1, "kBoolVarCtorPreVptr": 0x1594218,
                     "kBoolVarCtorMidVptr": 0x15940B0},
        "tags_h": {"kBoolFunctorCopyRva": 0xC2A0, "kBoolFunctorCloneRva": 0xC2C0, "kBoolFunctorMoveRva": 0xC2E0,
                   "kBoolFunctorDestroyRva": 0xC2F0, "kBoolFunctorClassTag": 0x1594280, "kBoolFunctorTagCount": 143},
    },
    "tokens": {
        "helper_h": ["inline constexpr std::uint64_t kBoolVarCtorFinalFnClass = kBoolFunctorClassTag;", "struct BoolVar274730SiteSpec {",
                     "[[nodiscard]] BoolVar274730Result helper_274730_bool_var("],
        "helper_c": ["if (!is_bool_functor_tag(args.fn_vptr)) {", "var.base.vptr = kBoolVarCtorPreVptr;", "init_concmd_base_20af0(var.base, name, description, kBoolVarCtorFlags);",
                     "var.base.vptr = kBoolVarMidVptr;", "var.value_ref = &var.value;", "var.fn_vptr = kBoolVarCtorStagingClass;",
                     "std::memcpy(var.zero70, args.q, sizeof var.zero70);", "if (args.q[3] != 0) {", "var.fn_payload = args.fn_payload;",
                     "var.fn_vptr = kBoolVarCtorFinalFnClass;", "var.fn_aux = args.fn_aux;", "var.base.vptr = kBoolVarFinalVptr;",
                     "var.value = initial_value;", "var.default_value = initial_value;", "register_concmd_20d60(var.base, registry);",
                     "registry.atexit_log.record(kBoolVarDtorRva, &var);", "out.refusal = HelperRefusal::unadmitted_arg_functor;"],
        "tags_c": ["std::binary_search(kBoolFunctorTags.begin(), kBoolFunctorTags.end(), tag)"],
        "test": ["CHECK(kBoolFunctorTagCount == 143 && kBoolFunctorTags.size() == 143);", "CHECK(v.fn_payload == 0 && v.fn_vptr == 0x1594280 && v.fn_aux == 0);",
                 "CHECK(q[0] == reinterpret_cast<std::uint64_t>(v.zero70));", "CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.ctor_ok);",
                 "for (std::uint64_t tag : kBoolFunctorTags) {"],
        "cmake": ["core/helper_274730_bool_var.cpp", "core/helper_bool_functor_tags.cpp", "add_test(NAME helper_274730 COMMAND test_helper_274730)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
