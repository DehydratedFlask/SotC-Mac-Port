#!/usr/bin/env python3
"""ELF-tie checker for the dword ranged console-variable constructors 0x49ca60 and 0x49cc60 (CPU-01-HELPER-CALLEE-MODELS-04; startup slot 451).

Pins instruction by instruction both 352 B bodies, the eight class slot bodies (A = 0x15bfce8: 0x49cc00 copy / 0x49cc20 clone / 0x49cc40 move / 0x49cc50 destroy;
B = 0x15bfd80: 0x49ce00 / 0x49ce20 / 0x49ce40 / 0x49ce50), the already-native float twin 0xc300, and both call sites inside slot 451 (0x49d230, 5126 B, hash-pinned).
Equivalence with 0xc300: the three bodies have the same instruction count and offsets and differ ONLY in the six rip- or call-relative instructions
(stack guard load x2, call 0x20af0, vptr lea, class lea, call __stack_chk_fail); every other instruction is byte-identical. So the layout, the min/max
engaged flags and the install sequence of init_ranged_float_var_c300 apply unchanged. The whole text is scanned: exactly one direct caller of each body
(0x49dbad and 0x49df7f), no jmp tail call, never address-taken. The class tables' relocation words are tied to the slot bodies and each slot copy / clone stores
the table's own constant. Site facts (min / max / value / default / atexit) are pinned from the bytes for the later wiring worker.

NOT established: the meaning of the dword (int vs unsigned); the consumers; the q[1..3] callbacks (zero at both sites); the slot-451 rows outside the two
sites; the final vptr of the object (rbx / r15 + 0x10, an immediate input of the site, pinned as a lea only).

Usage: python3 -I tools/lift/check_helper_49ca60.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins as pins_base  # noqa: E402
import helper_callee_pins_e as pins_e  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_e.PINS)

TEXT_END = 0xF8F000
A, B = 0x49CA60, 0x49CC60
SLOT451 = (0x49D230, 5126, "f6655116c4174f7d116b24f8f54f4beb69727ef64b920df63eaf59e45c504b59")
CLASS_A, CLASS_B = 0x15BFCE8, 0x15BFD80
SITE_A, SITE_B = 0x49DAEB, 0x49DEC0
# offsets (inside the 352 B bodies) of the only instructions that differ between 0xc300, 0x49ca60 and 0x49cc60
DIFF_OFFSETS = (0x11, 0x36, 0x3B, 0x46, 0x13B, 0x15A)


def x_equivalence() -> None:
    base, a, b = pins_base.PINS["ctor_c300"], pins_e.PINS["rdword_ctor_49ca60"], pins_e.PINS["rdword_ctor_49cc60"]
    require(len(base) == len(a) == len(b) == 90, "0xc300, 0x49ca60 and 0x49cc60 decode to the same 90 instructions")
    diff = []
    for (o1, h1), (o2, h2), (o3, h3) in zip(base, a, b):
        require(o1 == o2 == o3 and len(h1) == len(h2) == len(h3), f"instruction layout matches at {o1:#x}")
        if h1 != h2 or h2 != h3:
            diff.append(o1)
            lead = 1 if h1.startswith("e8") else 3     # opcode bytes of call rel32 / mov-lea rip-relative
            require(h1[:lead * 2] == h2[:lead * 2] == h3[:lead * 2], f"differing instruction at {o1:#x} keeps its opcode bytes")
    require(tuple(diff) == DIFF_OFFSETS, f"only the six rip / call relative instructions differ, got {[hex(d) for d in diff]}")


def x_scans(e) -> None:
    """Callers and address-taken-ness. Whole-image scans behave like the body hashes: skipped in the byte-mutation run."""
    if e.skip_hash:
        return
    text = e.raw(0, TEXT_END)
    for target, caller in ((A, 0x49DBAD), (B, 0x49DF7F)):
        calls, jmps = [], []
        for opcode, found in ((0xE8, calls), (0xE9, jmps)):
            pos = text.find(bytes([opcode]))
            while pos != -1:
                if pos + 5 <= len(text) and pos + 5 + struct.unpack("<i", text[pos + 1:pos + 5])[0] == target:
                    found.append(pos)
                pos = text.find(bytes([opcode]), pos + 1)
        require(calls == [caller], f"exactly one direct caller of {target:#x} at {caller:#x}, got {[hex(c) for c in calls]}")
        require(jmps == [], f"no jmp tail call to {target:#x}")
        require(not [x for x, r in e.relocs.items() if r["type"] == 8 and r["addend"] in (target, target ^ 1)],
                f"no relocation addend names {target:#x}")


def x_classes(e) -> None:
    for cls, slots in ((CLASS_A, (0x49CC00, 0x49CC20, 0x49CC40, 0x49CC50)), (CLASS_B, (0x49CE00, 0x49CE20, 0x49CE40, 0x49CE50))):
        for i, rva in enumerate(slots):
            require(e.reloc_target(cls + 8 * i) == rva, f"class {cls:#x} slot {i} -> {rva:#x}")
        require(e.lea(slots[0] + 4) == cls, f"class {cls:#x}: the copy slot stores its own constant")
        require(e.lea(slots[1]) == cls, f"class {cls:#x}: the clone slot stores its own constant")
        e.bytes_at(slots[3], "c3", f"class {cls:#x}: slot 3 is a bare ret")
    # nothing else relocates to the eight slot bodies (a table reaching them would widen the admitted family)
    if not e.skip_hash:
        wanted = {0x49CC00, 0x49CC20, 0x49CC40, 0x49CC50, 0x49CE00, 0x49CE20, 0x49CE40, 0x49CE50}
        owners = sorted(x for x, r in e.relocs.items() if r["type"] == 8 and r["addend"] in wanted)
        require(owners == sorted([CLASS_A + 8 * i for i in range(4)] + [CLASS_B + 8 * i for i in range(4)]),
                f"exactly the two class tables reach the eight slot bodies, got {[hex(o) for o in owners]}")


def x_slot451(e) -> None:
    base, size, sha = SLOT451
    fde = lib.fde_ranges()
    if fde is not None:
        require(fde.get(hex(base)) == size, "slot 451: FDE length")
    e.body_hash("slot 451 (0x49d230)", base, size, sha, always=True)
    body = e.raw(base, size)
    for target, site in ((A, 0x49DBAD), (B, 0x49DF7F)):
        found = [base + i for i in range(size - 4) if body[i] == 0xE8 and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == target]
        require(found == [site], f"slot 451: exactly one call to {target:#x}, at {site:#x}")
    for cls, site in ((CLASS_A, 0x49DB73), (CLASS_B, 0x49DF45)):
        leas = [base + i for i in range(size - 6) if body[i] in (0x48, 0x4C) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05
                and base + i + 7 + struct.unpack("<i", body[i + 3:i + 7])[0] == cls]
        require(leas == [site], f"slot 451: class {cls:#x} is loaded exactly once, at {site:#x}")
    # site A: min 0 / max 10 / flags 1 / value 1 / default 1 / atexit 0x498b20 ; site B: min 0 / max 4 / flags 1 / value 0 / default 0 / atexit 0x498b60
    for key, start, size_ in (("site451_a_range_49daeb", SITE_A, 0x120), ("site451_b_range_49dec0", SITE_B, 0x11D)):
        table = pins_e.PINS[key]
        require(sum(len(h) // 2 for _, h in table) == size_, f"{key}: pin table covers the range")
        for off, hexstr in table:
            e.bytes_at(start + off, hexstr, f"{key}+{off:#x}")
    require(e.lea(0x49DAEB) == 0x15BFC70, "site A: final vptr table 0x15bfc70 (+0x10)")
    require(e.lea(0x49DB73) == CLASS_A and e.call(0x49DBAD) == A, "site A: argument functor is class A, call 0x49ca60")
    e.bytes_at(0x49DB09, "c744241c00000000", "site A: min = 0 at [rsp+0x1c]")
    e.bytes_at(0x49DB11, "c74424180a000000", "site A: max = 10 at [rsp+0x18]")
    e.bytes_at(0x49DB37, "41b901000000", "site A: flags r9d = 1")
    e.bytes_at(0x49DBC1, "ff5018", "site A: call [class + 0x18] (bare ret destroy)")
    e.bytes_at(0x49DBCA, "41c786d000000001000000", "site A: value = 1 at +0xd0")
    e.bytes_at(0x49DBD5, "41c7466801000000", "site A: default = 1 at +0x68")
    require(e.call(0x49DBDD) == 0x20D60, "site A: register 0x20d60")
    require(e.call(0x49DBE5) == 0xF590 and e.call(0x49DBED) == 0xF590, "site A: two f590 releases (description, name)")
    require(e.lea(0x49DBF9) == 0x498B20 and e.lea(0x49DBF2) == 0x1604218 and e.call(0x49DC06) == 0xF8F390, "site A: __cxa_atexit(0x498b20, obj, dso)")
    require(e.lea(0x49DEC0) == 0x15BFD08, "site B: final vptr table 0x15bfd08 (+0x10)")
    require(e.lea(0x49DF45) == CLASS_B and e.call(0x49DF7F) == B, "site B: argument functor is class B, call 0x49cc60")
    e.bytes_at(0x49DEDE, "c744241c00000000", "site B: min = 0 at [rsp+0x1c]")
    e.bytes_at(0x49DEE6, "c744241804000000", "site B: max = 4 at [rsp+0x18]")
    e.bytes_at(0x49DF04, "41b901000000", "site B: flags r9d = 1")
    e.bytes_at(0x49DF98, "ff5018", "site B: call [class + 0x18] (bare ret destroy)")
    e.bytes_at(0x49DFA1, "c783d000000000000000", "site B: value = 0 at +0xd0")
    e.bytes_at(0x49DFAB, "c7436800000000", "site B: default = 0 at +0x68")
    require(e.call(0x49DFB2) == 0x20D60, "site B: register 0x20d60")
    require(e.call(0x49DFBA) == 0xF590 and e.call(0x49DFC2) == 0xF590, "site B: two f590 releases")
    require(e.lea(0x49DFC7) == 0x498B60 and e.lea(0x49DFCE) == 0x1604218 and e.call(0x49DFD8) == 0xF8F390, "site B: __cxa_atexit(0x498b60, obj, dso)")


def extra(e, fams) -> None:
    x_equivalence()
    x_classes(e)
    x_scans(e)
    x_slot451(e)


def ctor_rips(base: int, vtable: int, cls: int) -> list:
    return [(f"{base:#x} stack guard load", base + 0x11, 7, 3, 0x15FE518), (f"{base:#x} vptr table", base + 0x3B, 7, 3, vtable),
            (f"{base:#x} functor class", base + 0x46, 7, 3, cls), (f"{base:#x} stack guard reload", base + 0x13B, 7, 3, 0x15FE518)]


SPEC = {
    "name": "check_helper_49ca60",
    "summary": ("0x49ca60 / 0x49cc60 (352 B dword ranged console-variable ctors) pinned per instruction and proven the same body as the native float twin 0xc300 "
                "(six rip / call relative instructions differ); class A / B slot bodies pinned; one caller each re-derived by a whole-text scan; "
                "slot 451's two sites pinned from the vptr lea to __cxa_atexit"),
    "families": [],
    "bodies": [
        {"name": "0x49ca60 dword ranged ctor A", "rva": A, "size": 352, "fde": 352, "pins": "rdword_ctor_49ca60",
         "sha": "6303d909ca38d401e4149fe8ba455b7acbcb7bf237f6228e2a29f7f3c011dbe7"},
        {"name": "0x49cc60 dword ranged ctor B", "rva": B, "size": 352, "fde": 352, "pins": "rdword_ctor_49cc60",
         "sha": "43bfdc45715681411e9a81936761d817e9b718046fa798f8e86d8943f6c6216c"},
        {"name": "0xc300 float ranged ctor (twin)", "rva": 0xC300, "size": 352, "fde": 352, "pins": "ctor_c300",
         "sha": "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"},
        {"name": "0x49cc00 class A copy", "rva": 0x49CC00, "size": 20, "fde": 20, "pins": "rdword_a_copy_49cc00",
         "sha": "5a53a9acc65b8464c661212f81eb2bc8498e60c81db963d35382b9b8862d0563"},
        {"name": "0x49cc20 class A clone", "rva": 0x49CC20, "size": 20, "fde": 20, "pins": "rdword_a_clone_49cc20",
         "sha": "1b0a183a6009975d675831beba11993566d5939ea3a450305319c17158841dbf"},
        {"name": "0x49cc40 class A move", "rva": 0x49CC40, "size": 9, "fde": 9, "pins": "rdword_a_move_49cc40",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "0x49cc50 class A destroy", "rva": 0x49CC50, "size": 1, "fde": 1, "pins": "rdword_a_destroy_49cc50",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
        {"name": "0x49ce00 class B copy", "rva": 0x49CE00, "size": 20, "fde": 20, "pins": "rdword_b_copy_49ce00",
         "sha": "98cd6ab5305711f0f43482e3289042eb1353293a1b7cec2a2c8dd0e414ed9814"},
        {"name": "0x49ce20 class B clone", "rva": 0x49CE20, "size": 20, "fde": 20, "pins": "rdword_b_clone_49ce20",
         "sha": "03def06c6b69baf5e332a0dc72853559a9c65eb97c8808ca977b614aa3b1859f"},
        {"name": "0x49ce40 class B move", "rva": 0x49CE40, "size": 9, "fde": 9, "pins": "rdword_b_move_49ce40",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "0x49ce50 class B destroy", "rva": 0x49CE50, "size": 1, "fde": 1, "pins": "rdword_b_destroy_49ce50",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
    ],
    "rips": ctor_rips(A, 0x15BFB20, CLASS_A) + ctor_rips(B, 0x15BFB98, CLASS_B),
    "calls": [("0x49ca60 base ctor 0x20af0", A + 0x36, 0x20AF0), ("0x49ca60 __stack_chk_fail", A + 0x15A, 0xF8F3A0),
              ("0x49cc60 base ctor 0x20af0", B + 0x36, 0x20AF0), ("0x49cc60 __stack_chk_fail", B + 0x15A, 0xF8F3A0)],
    "jmps": [],
    "relocs": [],
    "bytes": [
        ("A: byte [+0x70] = 1 (min engaged)", A + 0x5C, "c6437001"), ("A: byte [+0x78] = 1 (max engaged)", A + 0x6A, "c6437801"),
        ("B: byte [+0x70] = 1 (min engaged)", B + 0x5C, "c6437001"), ("B: byte [+0x78] = 1 (max engaged)", B + 0x6A, "c6437801"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_49ca60_ranged_dword_var.hpp", "helper_c": "native/core/helper_49ca60_ranged_dword_var.cpp",
        "test": "native/tests/test_helper_49ca60.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kRangedDwordACtorRva": A, "kRangedDwordBCtorRva": B, "kRangedDwordCtorSize": 352,
                     "kRangedDwordAVptrTable": 0x15BFB20, "kRangedDwordBVptrTable": 0x15BFB98,
                     "kRangedDwordAClass": CLASS_A, "kRangedDwordBClass": CLASS_B,
                     "kRangedDwordAFnCopyRva": 0x49CC00, "kRangedDwordAFnCloneRva": 0x49CC20, "kRangedDwordAFnMoveRva": 0x49CC40,
                     "kRangedDwordAFnDestroyRva": 0x49CC50, "kRangedDwordBFnCopyRva": 0x49CE00, "kRangedDwordBFnCloneRva": 0x49CE20,
                     "kRangedDwordBFnMoveRva": 0x49CE40, "kRangedDwordBFnDestroyRva": 0x49CE50},
    },
    "tokens": {
        "helper_h": ["enum class RangedDwordVariant : std::uint8_t { a_49ca60, b_49cc60 };", "return (v == RangedDwordVariant::a_49ca60 ? kRangedDwordAVptrTable : kRangedDwordBVptrTable) + 0x10;",
                     "return v == RangedDwordVariant::a_49ca60 ? kRangedDwordAClass : kRangedDwordBClass;"],
        "helper_c": ["if (args.fn_vptr != cls) {", "out.refusal = HelperRefusal::unadmitted_arg_functor;", "init_concmd_base_20af0(var.base, name, description, flags);",
                     "var.base.vptr = ranged_dword_vptr(variant);", "var.value_ref = value_ref;", "var.min_engaged = 1;", "std::memcpy(var.min_value, min_value, 4);",
                     "var.max_engaged = 1;", "std::memcpy(var.max_value, max_value, 4);", "var.fn_vptr = cls;", "install_ranged_fn_from_args(var, args, cls);",
                     "out.refusal = HelperRefusal::wrong_helper;"],
        "test": ["CHECK(v.fn_payload == 0x1234 && v.fn_vptr == cls && v.fn_aux == 0x77);", "CHECK(cell == 0);",
                 "CHECK(w.args[0] == reinterpret_cast<std::uint64_t>(&w.args[0]) && w.args[3] == 0x49ca60 && w.args[1] == 0x1111);",
                 "CHECK(!rr.ok && rr.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(x));"],
        "cmake": ["core/helper_49ca60_ranged_dword_var.cpp", "add_test(NAME helper_49ca60 COMMAND test_helper_49ca60)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
