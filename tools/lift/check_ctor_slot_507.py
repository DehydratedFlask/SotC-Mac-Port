#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 507.

The pinned body is the inclusive range 0x22f3c0..0x22f5f6 (567 bytes). Its
final branch calls the stack-check-failure PLT thunk; the following nine NOPs
at 0x22f5f7..0x22f5ff are padding, before the next routine at 0x22f600.

Offline only: reads the pinned private ELF and relocation metadata; it never
loads, relocates, or executes original code. Pins the table word, body hash,
branch/call/RIP operands, strings, static data relocations, shared callees,
atexit import NIDs, and the native model/test contract. The inherited M6
driver runs a fail-closed byte/relocation/native-token mutation suite and
supports an explicit --tamper rejection probe.
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
def _slot507_import_context():
    """Parse the hash-pinned ELF's dynamic imports without executing game code."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "slot-507 import parser ELF SHA256")
    parsed = dyn.parse(elf)
    return parsed, dyn.Image(elf, parsed["ph"])


def x_slot507_atexit_imports(e, spec) -> None:
    """Tie the two imported PLT calls to their unique JMPREL NID records."""
    parsed, image = _slot507_import_context()
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
        # JMPREL symbol metadata is parsed from the pinned ELF; the addend
        # mutation hook must still make this assertion fail closed.
        e.reloc_pins.add(got)
        effective_addend = reloc["addend"] ^ (1 if got in e.reloc_flip else 0)
        require(effective_addend == 0, f"{plt:#x} JMPREL addend mutation")

        symbol = parsed["symbols"][reloc["symbol"]]
        expected_nid = dyn.nid(expected_name)
        require(symbol["type"] == "FUNC" and not symbol["defined"], f"{plt:#x} external function symbol")
        require(symbol["nid"] == expected_nid, f"{plt:#x} NID hash for {expected_name}")
        require(tiny.import_name(parsed, image, plt) == expected_nid,
                f"{plt:#x} PLT import decoder agrees for {expected_name}")


def x_slot507_object_vtables(e, spec) -> None:
    """Pin the base and final bool-variable vtable identities written by the body."""
    tables = (
        (0x15940B0, (0xB810, 0xB850, 0x20D60, 0xB890, 0xB8A0)),
        (0x1594218, (0x20B0, 0xC260, 0x20D60, 0xB890, 0xB8A0, 0x223C0, 0xB930)),
    )
    for table, targets in tables:
        for index, target in enumerate(targets):
            require(e.reloc_target(table + 8 * index) == target,
                    f"slot-507 object vtable {table:#x} entry {index}")


lib.EXTRAS["slot507_atexit_imports"] = x_slot507_atexit_imports
lib.EXTRAS["slot507_object_vtables"] = x_slot507_object_vtables


SPEC = {
    "slot": 507,
    "base": 0x22F3C0,
    "size": 567,
    "body_sha256": "6bd2f21d8a38c90130d48714808ab721bbba669d0cfc4fe8288afb827515714b",
    "pad": "909090909090909090",
    "callees": [
        "concmd_base_ctor",
        "concmd_register",
        "bool_dtor",
        "release_f590",
        "bool_fn_copy",
        "bool_fn_clone",
        "bool_fn_move",
        "bool_fn_destroy",
        "bool_slot3",
        "bool_slot6",
    ],
    "bool_tags": [0x159B500],
    "float_tags": [],
    "strings": [
        (0xFBFF20, b"ExhaustiveDegenerateSplineSegmentLookahead"),
        (0xFBFF4B, b"BruteForcePathSmoothing"),
        (0xFC00FD, b""),
    ],
    "lengths": [
        ("ExhaustiveDegenerateSplineSegmentLookahead", 0x22F421, "48c745982a000000"),
        ("BruteForcePathSmoothing", 0x22F522, "48c7459817000000"),
    ],
    "pins": [
        # Prologue, stack size, two enabled-value arguments, string lengths,
        # final guard branch, and exact return/stack-canary path.
        ("imm", 0x22F3C0, "55"),
        ("imm", 0x22F3C1, "4889e5"),
        ("imm", 0x22F3C4, "4157"),
        ("imm", 0x22F3C6, "4156"),
        ("imm", 0x22F3C8, "4155"),
        ("imm", 0x22F3CA, "4154"),
        ("imm", 0x22F3CC, "53"),
        ("imm", 0x22F3CD, "4883ec48"),
        ("rip", 0x22F3D1, 7, 3, 0x15FE518),
        ("rip", 0x22F3D8, 7, 3, 0x1594208),
        ("rip", 0x22F3DF, 7, 3, 0x2EE92C0),
        ("rip", 0x22F3E6, 7, 3, 0xFBFF20),
        ("rip", 0x22F3ED, 7, 3, 0xFC00FD),
        ("imm", 0x22F400, "b901000000"),
        ("rip", 0x22F433, 7, 3, 0x2EE92C0),
        ("imm", 0x22F43A, "48c745c800000000"),
        ("call", 0x22F442, 0x20AF0),
        ("rip", 0x22F447, 7, 3, 0x15940A0),
        ("rip", 0x22F451, 7, 3, 0x2EE9380),
        ("rip", 0x22F45C, 7, 3, 0x159B500),
        ("rip", 0x22F467, 7, 3, 0x2EE92C0),
        ("rip", 0x22F46E, 7, 3, 0x2EE9320),
        ("rip", 0x22F475, 10, 2, 0x2EE9329),
        ("rip", 0x22F47F, 8, 4, 0x2EE9330),
        ("rip", 0x22F487, 11, 3, 0x2EE9360),
        ("rip", 0x22F492, 7, 3, 0x2EE9370),
        ("rip", 0x22F499, 11, 3, 0x2EE9378),
        ("call", 0x22F4A4, 0xF590),
        ("rip", 0x22F4AC, 7, 3, 0x2EE92C0),
        ("rip", 0x22F4B3, 7, 2, 0x2EE9380),
        ("imm", 0x22F4B3, "c605c69ecb0201"),
        ("rip", 0x22F4BA, 7, 2, 0x2EE9328),
        ("imm", 0x22F4BA, "c605679ecb0201"),
        ("call", 0x22F4C1, 0x20D60),
        ("call", 0x22F4C9, 0xF590),
        ("rip", 0x22F4CE, 7, 3, 0x20B0),
        ("rip", 0x22F4D5, 7, 3, 0x1604218),
        ("call", 0x22F4DF, 0xF8F390),
        ("rip", 0x22F4E4, 7, 3, 0x2EE9388),
        ("rip", 0x22F4EB, 7, 3, 0xFC00FD),
        ("rip", 0x22F4FA, 7, 3, 0xFBFF4B),
        ("imm", 0x22F505, "b901000000"),
        ("rip", 0x22F50A, 7, 3, 0x2EE9388),
        ("imm", 0x22F534, "48c745c800000000"),
        ("call", 0x22F53C, 0x20AF0),
        ("rip", 0x22F544, 7, 3, 0x2EE9448),
        ("rip", 0x22F54F, 7, 3, 0x159B500),
        ("rip", 0x22F556, 7, 3, 0x2EE9388),
        ("rip", 0x22F55D, 7, 3, 0x2EE93E8),
        ("rip", 0x22F564, 10, 2, 0x2EE93F1),
        ("rip", 0x22F56E, 8, 4, 0x2EE93F8),
        ("rip", 0x22F576, 11, 3, 0x2EE9428),
        ("rip", 0x22F581, 7, 3, 0x2EE9438),
        ("rip", 0x22F588, 11, 3, 0x2EE9440),
        ("call", 0x22F593, 0xF590),
        ("rip", 0x22F59B, 7, 3, 0x2EE9388),
        ("rip", 0x22F5A2, 7, 2, 0x2EE9448),
        ("imm", 0x22F5A2, "c6059f9ecb0201"),
        ("rip", 0x22F5A9, 7, 2, 0x2EE93F0),
        ("imm", 0x22F5A9, "c605409ecb0201"),
        ("call", 0x22F5B0, 0x20D60),
        ("call", 0x22F5B8, 0xF590),
        ("rip", 0x22F5BD, 7, 3, 0x20B0),
        ("rip", 0x22F5C4, 7, 3, 0x1604218),
        ("call", 0x22F5CE, 0xF8F390),
        ("rip", 0x22F5D3, 7, 3, 0x15FE518),
        ("imm", 0x22F5E1, "750f"),
        ("call", 0x22F5F2, 0xF8F3A0),
    ],
    "m6_bytes": [
        (0xF8F390, "ff25220c6700", "__cxa_atexit PLT thunk"),
        (0xF8F3A0, "ff251a0c6700", "__stack_chk_fail PLT thunk"),
    ],
    "m6_extras": ["slot507_atexit_imports", "slot507_object_vtables"],
    "native": {
        "header": "native/core/ctor_slot_507_22f3c0.hpp",
        "source": "native/core/ctor_slot_507_22f3c0.cpp",
        "test": "native/tests/test_ctor_slot_507.cpp",
        "consts": {
            "kSlot507Rva": 0x22F3C0,
            "kSlot507Size": 567,
            "kSlot507BoolFnTag": 0x159B500,
            "kSlot507Var1NameLength": 0x2A,
            "kSlot507Var2NameLength": 0x17,
        },
        "header_tokens": [
            'IDENTITY TAGS, recorded and never run.',
        ],
        "source_order": [
            "out.first = lane_m6::init_bool_var_literal(objects.lookahead, kSlot507Var1Name,",
            "out.second = lane_m6::init_bool_var_literal(objects.smoothing, kSlot507Var2Name,",
            "out.ok = out.first.ok && out.second.ok;",
        ],
        "test_tokens": [
            "r.ok && r.first.ok && r.second.ok && r.first.views_inert && r.second.views_inert",
            "check_bool_common(o.lookahead, 0x159b500, 1)",
            "check_bool_common(o.smoothing, 0x159b500, 1)",
            "CHECK(r.first.registration.ok && r.first.registration.path == ConCmdRegisterPath::pending_list)",
            "CHECK(r.second.registration.ok && r.second.registration.path == ConCmdRegisterPath::pending_list)",
            "creg.pending.size() == 2",
            "creg.atexit_log.size() == 2",
            "check_atexit(creg, 0, 0x20b0, &o.lookahead)",
            "check_atexit(creg, 1, 0x20b0, &o.smoothing)",
            "CHECK(!r.ok && !r.first.ok && !r.second.ok && !r.first.registration.ok && !r.second.registration.ok",
        ],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(
        SPEC,
        "two bool variables (`ExhaustiveDegenerateSplineSegmentLookahead` and "
        "`BruteForcePathSmoothing`, each enabled/default 1; empty borrowed descriptions; "
        "two `__cxa_atexit(0x20b0, object, DSO)` registrations",
    ))
