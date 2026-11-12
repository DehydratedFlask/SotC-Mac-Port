#!/usr/bin/env python3
"""ELF-tie checker for the unbounded-int constructor twins 0x140b00 (startup slot 534) and 0x25ab0 (startup slot 566) (CPU-01-HELPER-CALLEE-MODELS-04).

Each twin is the already-native 0x3a980 body (322 B, FDE 323) with a private class pair. The checker hash-pins and instruction-pins both twins, compares the three
bodies byte for byte OUTSIDE the same five rel32 operand windows as the 0x233ec0 twin (stack guard load, call 0x20af0, mid vptr lea, seed class lea, call
__stack_chk_fail; recorded reads, so a changed byte in any of the three bodies is rejected) and resolves the operands on all three. The private seed classes
(A = 0x15983c8, B = 0x1595100) are four-slot tables whose copy / clone bodies equal the int family's 0xc720 / 0xc740 outside the stored class constant, whose move
is 0xc760 and whose destroy is a bare ret. The mid vptr tables are tied by relocation. The callers are re-derived by a whole-text scan: 0x140b00 has exactly three
(inside slot 534's body 0x1418d0), 0x25ab0 exactly one (inside slot 566's body 0x25de0); every call site's argument functor is the twin's OWN seed class.

NOT established: what the variables mean at run time; the final vptr classes and the value / default / atexit of the slots (slot-side wiring, not a callee fact).

Usage: python3 -I tools/lift/check_helper_140b00.py [--no-mutate] [--tamper elf|native]
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

TEXT_END = 0xF8F000
REF = 0x3A980
REF_SHA = "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"
SIZE = 0x142
WINDOWS = ((0x14, 4), (0x2D, 4), (0x34, 4), (0x3F, 4), (0x13E, 4))
# variant: (ctor, mid table, seed class, seed slot bodies, mid vptr relocation words, slot body, callers)
TWINS = (
    (0x140B00, 0x1598240, 0x15983C8, (0x140C90, 0x140CB0, 0x140CD0, 0x140CE0),
     (0x13EF80, 0x13EFC0, 0x20D60, 0x13F000, 0x13F010, 0x223C0, 0x13F380, 0x13F390),
     (0x1418D0, 6207, "4eedf09f4d802ac8cbbd255206623722c2bea3f484ed3081cfc622c5581b968c"), (0x141BD1, 0x141CD6, 0x142406), (0x141B76, 0x141C4A, 0x1423B4)),
    (0x25AB0, 0x1594FD0, 0x1595100, (0x25C40, 0x25C60, 0x25C80, 0x25C90),
     (0x24A50, 0x24A90, 0x20D60, 0x24AD0, 0x24AE0, 0x223C0, 0x24D70, 0x24D80),
     (0x25DE0, 1194, "9a520d7e1eb8f77bb73d50e0b61f60c340e28ace39adbdd70e3f444b7150d2b0"), (0x26181,), (0x2612A,)),
)
INT_SLOTS = (0xC720, 0xC740, 0xC760, 0xC770)


def x_equivalence(e) -> None:
    """Each twin == 0x3a980 outside five rel32 windows; the operands pin the class pair."""
    e.body_hash("0x3a980 reference body", REF, SIZE, REF_SHA, always=True)
    for ctor, mid, seed, *_ in TWINS:
        cursor = 0
        for off, length in WINDOWS + ((SIZE, 0),):
            if off > cursor:
                require(e.read(REF + cursor, off - cursor) == e.read(ctor + cursor, off - cursor),
                        f"{ctor:#x} equals 0x3a980 over [+{cursor:#x}, +{off:#x})")
            cursor = off + length
        require(e.rip(ctor + 0x11, 7, 3) == 0x15FE518, f"{ctor:#x}: stack guard load")
        require(e.call(ctor + 0x2C) == 0x20AF0, f"{ctor:#x}: call 0x20af0")
        require(e.rip(ctor + 0x31, 7, 3) == mid, f"{ctor:#x}: mid vptr table {mid:#x}")
        require(e.rip(ctor + 0x3C, 7, 3) == seed, f"{ctor:#x}: seed class {seed:#x}")
        require(e.call(ctor + 0x13D) == 0xF8F3A0, f"{ctor:#x}: __stack_chk_fail")
        e.bytes_at(ctor + 0x43, "4883c010", f"{ctor:#x}: mid vptr = table + 0x10")
    require(e.rip(REF + 0x31, 7, 3) == 0x1594190 and e.rip(REF + 0x3C, 7, 3) == 0x1595660, "0x3a980 class pair 0x1594190 / 0x1595660")


def x_classes(e) -> None:
    for ctor, mid, seed, slots, mid_words, *_ in TWINS:
        for i, rva in enumerate(slots):
            require(e.reloc_target(seed + 8 * i) == rva, f"seed class {seed:#x} slot {i} -> {rva:#x}")
        for i, rva in enumerate(mid_words):
            require(e.reloc_target(mid + 0x10 + 8 * i) == rva, f"mid vptr {mid + 0x10:#x} word {i} -> {rva:#x}")
        require(e.rip(slots[0] + 4, 7, 3) == seed, f"seed {seed:#x}: the copy slot stores its own constant")
        require(e.rip(slots[1], 7, 3) == seed, f"seed {seed:#x}: the clone slot stores its own constant")
        # outside the 4 byte class operand the seed bodies are the int bodies (0xc720 / 0xc740 / 0xc760 / 0xc770)
        require(e.read(0xC720, 7) == e.read(slots[0], 7) and e.read(0xC72B, 9) == e.read(slots[0] + 0xB, 9), f"seed {seed:#x}: copy == int copy outside the class operand")
        require(e.read(0xC740, 3) == e.read(slots[1], 3) and e.read(0xC747, 13) == e.read(slots[1] + 7, 13), f"seed {seed:#x}: clone == int clone outside the class operand")
        require(e.read(0xC760, 9) == e.read(slots[2], 9), f"seed {seed:#x}: move == int move")
        e.bytes_at(slots[3], "c3", f"seed {seed:#x}: destroy is a bare ret")
    if not e.skip_hash:
        wanted = {r for t in TWINS for r in t[3]}
        owners = sorted(x for x, r in e.relocs.items() if r["type"] == 8 and r["addend"] in wanted)
        require(owners == sorted(t[2] + 8 * i for t in TWINS for i in range(4)),
                f"exactly the two seed class tables reach the eight seed slot bodies, got {[hex(o) for o in owners]}")
    require(e.rip(0xC720 + 4, 7, 3) == 0x15944A0 and e.rip(0xC740, 7, 3) == 0x15944A0, "int copy / clone store class 0x15944a0")


def x_sites(e) -> None:
    fde = lib.fde_ranges()
    for ctor, mid, seed, slots, words, (base, size, sha), callers, leas_at in TWINS:
        if fde is not None:
            require(fde.get(hex(base)) == size, f"slot body {base:#x}: FDE length")
        e.body_hash(f"slot body {base:#x}", base, size, sha, always=True)
        body = e.raw(base, size)
        found = [base + i for i in range(size - 4) if body[i] == 0xE8 and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == ctor]
        require(found == list(callers), f"slot body {base:#x}: exactly the listed calls to {ctor:#x}, got {[hex(x) for x in found]}")
        loaded = [base + i for i in range(size - 6) if body[i] in (0x48, 0x4C) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05
                  and base + i + 7 + struct.unpack("<i", body[i + 3:i + 7])[0] == seed]
        require(loaded == list(leas_at), f"slot body {base:#x}: seed class {seed:#x} is loaded exactly at {[hex(x) for x in leas_at]}")
        for call in callers:
            require(e.call(call) == ctor, f"call at {call:#x} reaches {ctor:#x}")
        for lea in leas_at:
            require(e.lea(lea) == seed, f"the argument functor class loaded at {lea:#x} is the twin's own seed class {seed:#x}")
        if e.skip_hash:
            continue
        text = e.raw(0, TEXT_END)
        calls, jmps = [], []
        for opcode, bucket in ((0xE8, calls), (0xE9, jmps)):
            pos = text.find(bytes([opcode]))
            while pos != -1:
                if pos + 5 <= len(text) and pos + 5 + struct.unpack("<i", text[pos + 1:pos + 5])[0] == ctor:
                    bucket.append(pos)
                pos = text.find(bytes([opcode]), pos + 1)
        require(calls == list(callers), f"exactly the listed direct callers of {ctor:#x}, got {[hex(c) for c in calls]}")
        require(jmps == [], f"no jmp tail call to {ctor:#x}")
        require(not [x for x, r in e.relocs.items() if r["type"] == 8 and r["addend"] in (ctor, ctor ^ 1)], f"no relocation addend names {ctor:#x}")


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),))
    x_equivalence(e)
    x_classes(e)
    x_sites(e)


SPEC = {
    "name": "check_helper_140b00",
    "summary": ("0x140b00 / 0x25ab0 (unbounded-int ctor twins, 322 B) pinned per instruction and compared with the native 0x3a980 outside five rel32 windows; "
                "private seed classes 0x15983c8 / 0x1595100 tied to the int functor bodies; mid vptr tables by relocation; callers re-derived by a whole-text "
                "scan; every call site passes the twin's own seed class (slot 534: 3 sites, slot 566: 1 site)"),
    "families": [],
    "bodies": [
        {"name": "0x140b00 unbounded-int twin A", "rva": 0x140B00, "size": SIZE, "fde": 323, "pins": "twin_ctor_140b00",
         "sha": "6787738b3afa99663019912a9356fa08f689ea73b1062080832dcda1540d99bd"},
        {"name": "0x25ab0 unbounded-int twin B", "rva": 0x25AB0, "size": SIZE, "fde": 323, "pins": "twin_ctor_25ab0",
         "sha": "87aaa663ddaa01f8ac164d133bcf888d122b37089f3c83f4e10fb15d2d816ade"},
        {"name": "0x140c90 seed A copy", "rva": 0x140C90, "size": 20, "fde": 20, "pins": "twin_a_seed_copy_140c90",
         "sha": "953bdaa4d771100c170bc9b480281cdbee80b3b728a9539d9daef2f6e93d1e9f"},
        {"name": "0x140cb0 seed A clone", "rva": 0x140CB0, "size": 20, "fde": 20, "pins": "twin_a_seed_clone_140cb0",
         "sha": "1096115572266827793e9ea41040d65ebb740dcddb53b3d42000a1a3ce78c6fd"},
        {"name": "0x140cd0 seed A move", "rva": 0x140CD0, "size": 9, "fde": 9, "pins": "twin_a_seed_move_140cd0",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "0x140ce0 seed A destroy", "rva": 0x140CE0, "size": 1, "fde": 1, "pins": "twin_a_seed_destroy_140ce0",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
        {"name": "0x25c40 seed B copy", "rva": 0x25C40, "size": 20, "fde": 20, "pins": "twin_b_seed_copy_25c40",
         "sha": "84fc1099379817b10d817ea8bc0bd5341eb00e148d29105784858670ef91e529"},
        {"name": "0x25c60 seed B clone", "rva": 0x25C60, "size": 20, "fde": 20, "pins": "twin_b_seed_clone_25c60",
         "sha": "1ce359d9a094b8d66d7c8975cbc618e9baf073173ac66c3bcf3872302d9d0b04"},
        {"name": "0x25c80 seed B move", "rva": 0x25C80, "size": 9, "fde": 9, "pins": "twin_b_seed_move_25c80",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "0x25c90 seed B destroy", "rva": 0x25C90, "size": 1, "fde": 1, "pins": "twin_b_seed_destroy_25c90",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
    ],
    "rips": [],
    "calls": [],
    "jmps": [],
    "relocs": [],
    "bytes": [],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_140b00_unbounded_int_twin.hpp", "helper_c": "native/core/helper_140b00_unbounded_int_twin.cpp",
        "test": "native/tests/test_helper_140b00.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kUnboundedIntTwinARva": 0x140B00, "kUnboundedIntTwinBRva": 0x25AB0, "kUnboundedIntTwinABSize": SIZE,
                     "kUnboundedIntTwinAMidTable": 0x1598240, "kUnboundedIntTwinBMidTable": 0x1594FD0,
                     "kUnboundedIntTwinASeedClass": 0x15983C8, "kUnboundedIntTwinBSeedClass": 0x1595100,
                     "kUnboundedIntTwinASeedCopyRva": 0x140C90, "kUnboundedIntTwinASeedCloneRva": 0x140CB0,
                     "kUnboundedIntTwinASeedMoveRva": 0x140CD0, "kUnboundedIntTwinASeedDestroyRva": 0x140CE0,
                     "kUnboundedIntTwinBSeedCopyRva": 0x25C40, "kUnboundedIntTwinBSeedCloneRva": 0x25C60,
                     "kUnboundedIntTwinBSeedMoveRva": 0x25C80, "kUnboundedIntTwinBSeedDestroyRva": 0x25C90},
    },
    "tokens": {
        "helper_h": ["enum class UnboundedIntTwinVariant : std::uint8_t { a_140b00, b_25ab0 };",
                     "return (v == UnboundedIntTwinVariant::a_140b00 ? kUnboundedIntTwinAMidTable : kUnboundedIntTwinBMidTable) + 0x10;",
                     "return v == UnboundedIntTwinVariant::a_140b00 ? kUnboundedIntTwinASeedClass : kUnboundedIntTwinBSeedClass;"],
        "helper_c": ["const bool private_class = block.fn_vptr == seed;", "if (private_class) lifted.fn_vptr = kIntFunctorClassTag;",
                     "helper_3a980_unbounded_int(var, name, flags, lifted, description);", "if (!out.ok) return out;",
                     "var.base.vptr = unbounded_int_twin_mid_vptr(variant);", "if (private_class) var.fn_vptr = seed;"],
        "test": ["CHECK(v.fn_vptr == seed && v.fn_payload == 0x55 && v.fn_aux == 0x66 && v.min_engaged == 0 && v.max_engaged == 0);",
                 "CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok && all_poison(v));",
                 "CHECK(same_bytes(a, b));"],
        "cmake": ["core/helper_140b00_unbounded_int_twin.cpp", "add_test(NAME helper_140b00 COMMAND test_helper_140b00)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
