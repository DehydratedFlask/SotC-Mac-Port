#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 539 (RVA 0x121940), lane M6.

The pinned inclusive body is 0x121940..0x121b7a (571 bytes). Five NOPs at
0x121b7b..0x121b7f are alignment padding before the next routine at 0x121b80.

Offline only: reads the hash-pinned private ELF and relocation metadata. It
never loads, relocates, or executes an original RVA. The byte pins include each
body instruction plus semantic RIP/call operands, three literal lengths, two
atexit import identities, both name-holder vtable address points, and shared
callee/destructor body hashes. The shared M6 driver runs fail-closed byte,
relocation, and native-token mutation checks and provides --tamper rejection.
"""

from __future__ import annotations

from functools import lru_cache
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402
from slot_pin_util import ELF_SHA256, ROOT, digest, require  # noqa: E402

sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402
import tiny_slot_decode as tiny  # noqa: E402


@lru_cache(maxsize=1)
def _slot539_import_context():
    """Parse dynamic imports from the pinned ELF without executing game code."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "slot-539 import parser ELF SHA256")
    parsed = dyn.parse(elf)
    return parsed, dyn.Image(elf, parsed["ph"])


def x_slot539_atexit_imports(e, spec) -> None:
    """Bind both PLT thunks used by the constructor to their JMPREL NIDs."""
    parsed, image = _slot539_import_context()
    imports = (
        (0xF8F390, 0x15FFFB8, "__cxa_atexit"),
        (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
    )
    for plt, expected_got, expected_name in imports:
        stub = e.read(plt, 6)
        require(stub[:2] == b"\xff\x25", f"PLT thunk {plt:#x} opcode")
        got = e.rip(plt, 6, 2)
        require(got == expected_got, f"PLT thunk {plt:#x} GOT target")
        rows = [r for r in parsed["relocs"] if r["table"] == "JMPREL" and r["target"] == got]
        require(len(rows) == 1, f"one JMPREL row for {plt:#x} GOT {got:#x}")
        reloc = rows[0]
        require(reloc["type"] == 7, f"{plt:#x} relocation is R_X86_64_JUMP_SLOT")
        require(reloc["addend"] == 0, f"{plt:#x} JMPREL addend is zero")

        # Let the shared mutation driver perturb this import relocation too.
        e.reloc_pins.add(got)
        effective_addend = reloc["addend"] ^ (1 if got in e.reloc_flip else 0)
        require(effective_addend == 0, f"{plt:#x} JMPREL addend mutation")

        symbol = parsed["symbols"][reloc["symbol"]]
        expected_nid = dyn.nid(expected_name)
        require(symbol["type"] == "FUNC" and not symbol["defined"], f"{plt:#x} external function symbol")
        require(symbol["nid"] == expected_nid, f"{plt:#x} NID hash for {expected_name}")
        require(tiny.import_name(parsed, image, plt) == expected_nid,
                f"{plt:#x} PLT import decoder agrees for {expected_name}")


def x_slot539_holder_vtables(e, spec) -> None:
    """Pin the address-point relocations carried by the two recorded name-holder identities."""
    address_points = (
        (0x15976B0, (0x91210, 0, 0, 0x90420, 0x91270, 0x13D10, 0x13D20)),
        (0x1597658, (0x911C0, 0x358A0, 0x358C0, 0x358E0, 0x358F0, 0xC2A0, 0xC2C0)),
    )
    for address_point, targets in address_points:
        for index, target in enumerate(targets):
            slot = address_point + 8 * index
            if target:
                require(e.reloc_target(slot) == target,
                        f"name-holder vtable {address_point:#x} slot {index} -> {target:#x}")
            else:
                require(e.u64(slot) == 0, f"name-holder vtable {address_point:#x} slot {index} is null")


def x_slot539_cell_dtor_contract(e, spec) -> None:
    """Pin the zero-cell fast path and both conditional helper-call identities."""
    e.bytes_at(0x112D90, "7417", "cell destructor skips cleanup when its cell is zero")
    require(0x112D90 + 2 + 0x17 == 0x112DA9, "cell destructor zero-cell branch target")
    require(e.call(0x112D95) == 0x113690, "cell destructor helper call 0")
    require(e.call(0x112D9D) == 0x170B0, "cell destructor helper call 1")


def x_slot539_stack_canary_branch(e, spec) -> None:
    """Decode the short branch to the terminal stack-check-failure call."""
    branch = e.read(0x121B65, 2)
    require(branch[0] == 0x75, "constructor stack-canary branch is jne rel8")
    target = 0x121B67 + int.from_bytes(branch[1:2], "little", signed=True)
    require(target == 0x121B76, "constructor stack-canary branch targets failure call")


lib.EXTRAS["slot539_atexit_imports"] = x_slot539_atexit_imports
lib.EXTRAS["slot539_holder_vtables"] = x_slot539_holder_vtables
lib.EXTRAS["slot539_cell_dtor_contract"] = x_slot539_cell_dtor_contract
lib.EXTRAS["slot539_stack_canary_branch"] = x_slot539_stack_canary_branch


# Complete raw instruction encodings from xcrun llvm-objdump -d over the
# inclusive body. This pins every instruction byte (including every encoded
# displacement and immediate); semantic RIP/call pins below also state targets.
INSTRUCTIONS = [
    (0x121940, "55"),
    (0x121941, "4889e5"),
    (0x121944, "4157"),
    (0x121946, "4156"),
    (0x121948, "4155"),
    (0x12194A, "4154"),
    (0x12194C, "53"),
    (0x12194D, "4883ec48"),
    (0x121951, "488b05c0cb4d01"),
    (0x121958, "488d15b9284e01"),
    (0x12195F, "488d35dad7db02"),
    (0x121966, "488d3d1314ffff"),
    (0x12196D, "488b00"),
    (0x121970, "4989d7"),
    (0x121973, "488945d0"),
    (0x121977, "48c70600000000"),
    (0x12197E, "e80ddae600"),
    (0x121983, "4c8d35165d4701"),
    (0x12198A, "488d1db7d7db02"),
    (0x121991, "488d05484be800"),
    (0x121998, "488d7db0"),
    (0x12199C, "c5e857d2"),
    (0x1219A0, "488945b0"),
    (0x1219A4, "c4e1f96ec0"),
    (0x1219A9, "b80c000000"),
    (0x1219AE, "48c745b80c000000"),
    (0x1219B6, "c5f81155c0"),
    (0x1219BB, "c4e1f96ec8"),
    (0x1219C0, "c5f96cc1"),
    (0x1219C4, "4983c610"),
    (0x1219C8, "c5fa7f4308"),
    (0x1219CD, "c5f8115318"),
    (0x1219D2, "48c7432800000000"),
    (0x1219DA, "4c8933"),
    (0x1219DD, "e8aedbeeff"),
    (0x1219E2, "488d3d17d5f6ff"),
    (0x1219E9, "4889de"),
    (0x1219EC, "4c89fa"),
    (0x1219EF, "e89cd9e600"),
    (0x1219F4, "4c8d3d0d284701"),
    (0x1219FB, "488d1d76d7db02"),
    (0x121A02, "488d05e44ae800"),
    (0x121A09, "4c8d6590"),
    (0x121A0D, "4c8d6db0"),
    (0x121A11, "c5f9efc0"),
    (0x121A15, "b901000000"),
    (0x121A1A, "48894590"),
    (0x121A1E, "488d051852e800"),
    (0x121A25, "4c89e6"),
    (0x121A28, "4c89ea"),
    (0x121A2B, "48c7459811000000"),
    (0x121A33, "c5fa7f45a0"),
    (0x121A38, "488945b0"),
    (0x121A3C, "c5fa7f45b8"),
    (0x121A41, "48c745c800000000"),
    (0x121A49, "4983c710"),
    (0x121A4D, "4889df"),
    (0x121A50, "4c893b"),
    (0x121A53, "e898f0efff"),
    (0x121A58, "488d0541264701"),
    (0x121A5F, "4c89ef"),
    (0x121A62, "488d93c0000000"),
    (0x121A69, "c5fc57c0"),
    (0x121A6D, "488d0dac654701"),
    (0x121A74, "4883c010"),
    (0x121A78, "488903"),
    (0x121A7B, "48895360"),
    (0x121A7F, "c7436900000000"),
    (0x121A86, "c5fc114370"),
    (0x121A8B, "48c783a000000000000000"),
    (0x121A96, "48898bb0000000"),
    (0x121A9D, "48c783b800000000000000"),
    (0x121AA8, "e8e3daeeff"),
    (0x121AAD, "4889df"),
    (0x121AB0, "4c893b"),
    (0x121AB3, "c683c000000000"),
    (0x121ABA, "c6436800"),
    (0x121ABE, "e89df2efff"),
    (0x121AC3, "4c89e7"),
    (0x121AC6, "e8c5daeeff"),
    (0x121ACB, "4889de"),
    (0x121ACE, "488d1d43274e01"),
    (0x121AD5, "488d3dd405eeff"),
    (0x121ADC, "4889da"),
    (0x121ADF, "e8acd8e600"),
    (0x121AE4, "488d05ca4ae800"),
    (0x121AEB, "488d7db0"),
    (0x121AEF, "c5e857d2"),
    (0x121AF3, "4c8d3d46d7db02"),
    (0x121AFA, "488945b0"),
    (0x121AFE, "c4e1f96ec0"),
    (0x121B03, "b819000000"),
    (0x121B08, "48c745b819000000"),
    (0x121B10, "c5f81155c0"),
    (0x121B15, "c4e1f96ec8"),
    (0x121B1A, "c5f96cc1"),
    (0x121B1E, "c5fa7f0522d7db02"),
    (0x121B26, "c5f811152ad7db02"),
    (0x121B2E, "48c7052fd7db0200000000"),
    (0x121B39, "4c893500d7db02"),
    (0x121B40, "e84bdaeeff"),
    (0x121B45, "488d3db4d3f6ff"),
    (0x121B4C, "4c89fe"),
    (0x121B4F, "4889da"),
    (0x121B52, "e839d8e600"),
    (0x121B57, "488b05bac94d01"),
    (0x121B5E, "488b00"),
    (0x121B61, "483b45d0"),
    (0x121B65, "750f"),
    (0x121B67, "4883c448"),
    (0x121B6B, "5b"),
    (0x121B6C, "415c"),
    (0x121B6E, "415d"),
    (0x121B70, "415e"),
    (0x121B72, "415f"),
    (0x121B74, "5d"),
    (0x121B75, "c3"),
    (0x121B76, "e825d8e600"),
]


SPEC = {
    "slot": 539,
    "base": 0x121940,
    "size": 571,
    "body_sha256": "3567c7ba88fbdcebee5591499f2524e9a2c8cec015505d905ba1a521e779a9c9",
    "pad": "9090909090",
    "callees": ["concmd_base_ctor", "concmd_register", "bool_dtor", "release_f590"],
    "bool_tags": [0x1598020],
    "float_tags": [],
    "strings": [
        (0xFA64E0, b"r_showBounds"),
        (0xFA64ED, b"r_showObjectCount"),
        (0xFA65B5, b"r_threadRObjectNewAndFree"),
        (0xFA6C3D, b""),
    ],
    "lengths": [
        ("r_showBounds", 0x1219AE, "48c745b80c000000"),
        ("r_showObjectCount", 0x121A2B, "48c7459811000000"),
        ("r_threadRObjectNewAndFree", 0x121B08, "48c745b819000000"),
    ],
    "pins": [
        *(('imm', address, encoding) for address, encoding in INSTRUCTIONS),
        # All RIP-relative references, with their computed targets.
        ("rip", 0x121951, 7, 3, 0x15FE518),
        ("rip", 0x121958, 7, 3, 0x1604218),
        ("rip", 0x12195F, 7, 3, 0x2EDF140),
        ("rip", 0x121966, 7, 3, 0x112D80),
        ("rip", 0x121983, 7, 3, 0x15976A0),
        ("rip", 0x12198A, 7, 3, 0x2EDF148),
        ("rip", 0x121991, 7, 3, 0xFA64E0),
        ("rip", 0x1219E2, 7, 3, 0x8EF00),
        ("rip", 0x1219F4, 7, 3, 0x1594208),
        ("rip", 0x1219FB, 7, 3, 0x2EDF178),
        ("rip", 0x121A02, 7, 3, 0xFA64ED),
        ("rip", 0x121A1E, 7, 3, 0xFA6C3D),
        ("rip", 0x121A58, 7, 3, 0x15940A0),
        ("rip", 0x121A6D, 7, 3, 0x1598020),
        ("rip", 0x121ACE, 7, 3, 0x1604218),
        ("rip", 0x121AD5, 7, 3, 0x20B0),
        ("rip", 0x121AE4, 7, 3, 0xFA65B5),
        ("rip", 0x121AF3, 7, 3, 0x2EDF240),
        ("rip", 0x121B1E, 8, 4, 0x2EDF248),
        ("rip", 0x121B26, 8, 4, 0x2EDF258),
        ("rip", 0x121B2E, 11, 3, 0x2EDF268),
        ("rip", 0x121B39, 7, 3, 0x2EDF240),
        ("rip", 0x121B45, 7, 3, 0x8EF00),
        ("rip", 0x121B57, 7, 3, 0x15FE518),
        # Direct call targets; the short stack-canary branch is byte-pinned above.
        ("call", 0x12197E, 0xF8F390),
        ("call", 0x1219DD, 0xF590),
        ("call", 0x1219EF, 0xF8F390),
        ("call", 0x121A53, 0x20AF0),
        ("call", 0x121AA8, 0xF590),
        ("call", 0x121ABE, 0x20D60),
        ("call", 0x121AC6, 0xF590),
        ("call", 0x121ADF, 0xF8F390),
        ("call", 0x121B40, 0xF590),
        ("call", 0x121B52, 0xF8F390),
        ("call", 0x121B76, 0xF8F3A0),
    ],
    "m6_callees": {
        "slot539_cell_dtor": (0x112D80, 46, "3528c49b11da93d90c3b40e58ce898f6c1b3149c3fd9ac82b10efc9fc234859b"),
        "slot539_name_holder_dtor": (0x8EF00, 23, "32799b7bccf74b604c28f179e15a8bfb5b928f37bfacc3efa336f33d022e6c86"),
        "slot539_cell_cleanup_helper": (0x113690, 710, "e317680fa8b023a43e012b0f63336ffcade955299e341da3650fc64c3f7527db"),
        "slot539_cell_release_helper": (0x170B0, 10, "bd6d6a49a0a57e9f8f358cd4c02a743796b0e138a0098a26091b0c4c95df3928"),
    },
    "m6_bytes": [
        (0xF8F390, "ff25220c6700", "__cxa_atexit PLT thunk"),
        (0xF8F3A0, "ff251a0c6700", "__stack_chk_fail PLT thunk"),
    ],
    "m6_extras": ["slot539_atexit_imports", "slot539_holder_vtables", "slot539_cell_dtor_contract",
                   "slot539_stack_canary_branch"],
    "native": {
        "header": "native/core/ctor_slot_539_121940.hpp",
        "source": "native/core/ctor_slot_539_121940.cpp",
        "test": "native/tests/test_ctor_slot_539.cpp",
        "consts": {
            "kSlot539Rva": 0x121940,
            "kSlot539Size": 571,
            "kSlot539CellDtorRva": 0x112D80,
            "kSlot539HolderDtorRva": 0x8EF00,
            "kSlot539HolderVptr": 0x15976B0,
            "kSlot539BoolFnTag": 0x1598020,
            "kSlot539Holder1NameLength": 0xC,
            "kSlot539VarNameLength": 0x11,
            "kSlot539Holder2NameLength": 0x19,
        },
        "header_tokens": [
            "IDENTITY TAGS, recorded and never run.",
            "control == nullptr, borrowed",
            "struct NameHolder30",
            "offsetof(Slot539Objects, holder1) == 0x8",
            "offsetof(Slot539Objects, var) == 0x38",
            "offsetof(Slot539Objects, holder2) == 0x100",
            "sizeof(NameHolder30) == 0x30",
        ],
        "source_order": [
            "const SharedStr view = l2::measured_view(text, length);",
            "holder.name = view;",
            "holder.tail = 0;",
            "holder.vptr = kSlot539HolderVptr;",
            "const bool inert = release_inert_view_f590(view);",
            "const bool recorded = registry.atexit_log.record(kSlot539HolderDtorRva, &holder);",
            "objects.cell = 0;",
            "out.cell_atexit = concmd.atexit_log.record(kSlot539CellDtorRva, &objects.cell);",
            "out.holder1_ok = build_holder(objects.holder1, kSlot539Holder1Name,",
            "out.var = lane_m6::init_bool_var_literal(objects.var, kSlot539VarName, kSlot539VarNameLength, \"\", 0,",
            "out.holder2_ok = build_holder(objects.holder2, kSlot539Holder2Name,",
            "out.ok = out.cell_atexit && out.holder1_ok && out.var.ok && out.holder2_ok;",
        ],
        "test_tokens": [
            "sizeof(Slot539Objects) == 0x130",
            "section_alone();",
            "section_failures();",
            "r.ok && r.cell_atexit && r.holder1_ok && r.holder2_ok && r.var.ok && r.var.views_inert",
            "same_view(o.var.base.name, \"r_showObjectCount\", 0x11)",
            "check_bool_common(o.var, 0x1598020, 0)",
            "creg.atexit_log.size() == 4",
            "check_atexit(creg, 0, 0x112d80, &o.cell)",
            "check_atexit(creg, 1, 0x8ef00, &o.holder1)",
            "check_atexit(creg, 2, 0x20b0, &o.var)",
            "check_atexit(creg, 3, 0x8ef00, &o.holder2)",
            "!r.ok && r.cell_atexit && r.holder1_ok && !r.var.ok && !r.var.registration.ok && r.holder2_ok",
        ],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(
        SPEC,
        "zero cell + two borrowed name holders (`r_showBounds`, `r_threadRObjectNewAndFree`) + "
        "bool console variable `r_showObjectCount` (0/0, tag 0x1598020); four ordered atexit records",
    ))
