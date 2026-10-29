#!/usr/bin/env python3
"""Offline byte-oracle for startup constructor slot 545 (RVA 0xb2ec0).

The pinned parent body is the inclusive range 0xb2ec0..0xb3154 (661 bytes),
followed by eleven alignment NOPs. The checker reads the hash-pinned private ELF
and relocation metadata only; it never loads, relocates, or executes an original RVA.
It pins the constructor-table word, full body, helper/callback/release body hashes,
imports, static source bytes, call graph, and the native fixture contract.
"""

from __future__ import annotations

from functools import lru_cache
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402
from slot_pin_util import ELF_SHA256, ROOT, TABLE_START, digest, require  # noqa: E402

sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402
import tiny_slot_decode as tiny  # noqa: E402


@lru_cache(maxsize=1)
def _slot545_import_context():
    """Parse imports from the pinned ELF without executing game code."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "slot-545 import parser ELF SHA256")
    parsed = dyn.parse(elf)
    return parsed, dyn.Image(elf, parsed["ph"])


def x_slot545_atexit_imports(e, spec) -> None:
    """Bind both parent PLT thunks to their unique JMPREL NID records."""
    parsed, image = _slot545_import_context()
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
        e.reloc_pins.add(got)
        effective_addend = reloc["addend"] ^ (1 if got in e.reloc_flip else 0)
        require(effective_addend == 0, f"{plt:#x} JMPREL addend mutation")
        symbol = parsed["symbols"][reloc["symbol"]]
        expected_nid = dyn.nid(expected_name)
        require(symbol["type"] == "FUNC" and not symbol["defined"], f"{plt:#x} external function symbol")
        require(symbol["nid"] == expected_nid, f"{plt:#x} NID hash for {expected_name}")
        require(tiny.import_name(parsed, image, plt) == expected_nid,
                f"{plt:#x} PLT import decoder agrees for {expected_name}")


def x_slot545_parent(e, spec) -> None:
    """Pin table identity, static stores, helper order, imports, and canary path."""
    e.body_hash("slot545_complete_parent", 0xB2EC0, 0x295,
                "e623b2b5df411115cdf2eee644646d9cda3fa3a3059f978cd8bc967dde65e06c", always=True)
    require(e.u64(TABLE_START - 8 * 545) == 0xB2EC0, "constructor table slot 545 -> 0xb2ec0")

    for site in (0xB2F63, 0xB2FF1, 0xB3083):
        require(e.call(site) == 0xAB860, f"matrix helper call at {site:#x} -> 0xab860")
    for site, target in ((0xB2ED7, 0x2ED37F8), (0xB2F6C, 0x2ED3838), (0xB300B, 0x2ED3878)):
        require(e.lea(site) == target, f"matrix output at {site:#x} -> {target:#x}")

    require(e.rip(0xB3088, 8, 4) == 0xFA0260, "32-byte constant source at 0xfa0260")
    require(e.lea(0xB3090) == 0xFAB6B0, "64-byte object prefix source at 0xfab6b0")
    require(e.lea(0xB3097) == 0x2ED3918, "static object at global image offset 0x120")
    require(e.lea(0xB309E) == 0xB0A10, "atexit callback identity 0xb0a10")
    require(e.lea(0xB30A5) == 0x1604218, "atexit DSO handle identity 0x1604218")
    require(e.rip(0xB30AC, 8, 4) == 0x2ED38C0, "constant copy destination at global offset 0xc8")
    require(e.rip(0xB30C8, 10, 2) == 0x2ED38E0, "scalar store destination at global offset 0xe8")
    require(e.call(0xB3138) == 0xF8F390, "one __cxa_atexit call after data writes")

    e.bytes_at(0xB3144, "750a", "stack-cookie mismatch branch")
    branch = e.read(0xB3144, 2)
    require(0xB3146 + int.from_bytes(branch[1:2], "little", signed=True) == 0xB3150,
            "stack-cookie branch targets __stack_chk_fail call")
    e.bytes_at(0xB3146, "4881c4880000005b5dc3", "normal stack restore and return")
    require(e.call(0xB3150) == 0xF8F3A0, "stack-cookie failure call -> __stack_chk_fail")


def x_slot545_cleanup(e, spec) -> None:
    """Pin the cleanup callback's two pointer predicates and ordered tag-2 wrapper paths."""
    e.bytes_at(0xB0A19, "488bbb58010000", "first cleanup pointer load at +0x158")
    e.bytes_at(0xB0A23, "740e", "first cleanup null skip")
    e.bytes_at(0xB0A25, "4839bb78010000", "first cleanup peer compare at +0x178")
    e.bytes_at(0xB0A2C, "7405", "first cleanup equal-pointer skip")
    require(e.call(0xB0A2E) == 0x170C0, "first cleanup release -> 0x170c0")
    e.bytes_at(0xB0A33, "488b7b30", "second cleanup pointer load at +0x30")
    e.bytes_at(0xB0A3A, "7411", "second cleanup null skip")
    e.bytes_at(0xB0A3C, "48397b50", "second cleanup peer compare at +0x50")
    e.bytes_at(0xB0A40, "740b", "second cleanup equal-pointer skip")
    require(e.jmp(0xB0A48) == 0x170C0, "second cleanup tail release -> 0x170c0")
    e.bytes_at(0x170C0, "be02000000", "0x170c0 forwards allocator tag 2")
    require(e.jmp(0x170C5) == 0x12AF0, "0x170c0 tail-dispatches to the host allocator contract site")


lib.EXTRAS["slot545_atexit_imports"] = x_slot545_atexit_imports
lib.EXTRAS["slot545_parent"] = x_slot545_parent
lib.EXTRAS["slot545_cleanup"] = x_slot545_cleanup


_base_read_native = lib.base.read_native
_base_check_native = lib.base.check_native


def _slot545_read_native(spec):
    texts = _base_read_native(spec)
    texts["startup_h"] = (ROOT / "native/core/startup_ctors.hpp").read_text()
    texts["startup_cpp"] = (ROOT / "native/core/startup_ctors.cpp").read_text()
    return texts


def _slot545_check_native(spec, texts):
    _base_check_native(spec, texts)
    for path in ("startup_h", "startup_cpp"):
        require("init_slot545_b2ec0" not in texts[path] and "kSlot545Rva" not in texts[path],
                f"slot 545 remains absent from {path}")


lib.base.read_native = _slot545_read_native
lib.base.check_native = _slot545_check_native


SPEC = {
    "slot": 545,
    "base": 0xB2EC0,
    "size": 0x295,
    "body_sha256": "e623b2b5df411115cdf2eee644646d9cda3fa3a3059f978cd8bc967dde65e06c",
    "pad": "9090909090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "pins": [
        ("call", 0xB2F63, 0xAB860),
        ("call", 0xB2FF1, 0xAB860),
        ("call", 0xB3083, 0xAB860),
        ("call", 0xB3138, 0xF8F390),
        ("call", 0xB3150, 0xF8F3A0),
        ("rip", 0xB3088, 8, 4, 0xFA0260),
        ("rip", 0xB30AC, 8, 4, 0x2ED38C0),
        ("rip", 0xB30C8, 10, 2, 0x2ED38E0),
        ("imm", 0xB3144, "750a"),
    ],
    "m6_callees": {
        "slot545_complete_parent": (0xB2EC0, 0x295,
                                     "e623b2b5df411115cdf2eee644646d9cda3fa3a3059f978cd8bc967dde65e06c"),
        "slot545_matrix_helper": (0xAB860, 0xFF,
                                  "c8d08501b4200a25b9d919d8a09721789d96fc8095987ccfcd3b23d1f8ee871a"),
        "slot545_cleanup_callback": (0xB0A10, 0x44,
                                     "d6b73ebcf6f957bf83cbe12fe465f4de77e8a83736101cb188c2735eee5f1a21"),
        "slot545_free_wrapper": (0x170C0, 0x0A,
                                 "39cd562d64a27bc6e432aaf827156a2b3551b6d1ae1c44831c2a2f5feeace372"),
    },
    "m6_bytes": [
        (0xF8F390, "ff25220c6700", "__cxa_atexit PLT thunk"),
        (0xF8F3A0, "ff251a0c6700", "__stack_chk_fail PLT thunk"),
        (0xFA0260, "0000803f0000000000000000000000000000803f000000000000000000000000",
         "slot-545 32-byte constant source"),
        (0xFAB6B0,
         "0000803f0000000000000000000000000000803f000000000000000000000000"
         "0000803f00000000000000000000000068641bff16641bff0c641bff02641bff",
         "slot-545 64-byte object prefix source"),
    ],
    "m6_extras": ["slot545_atexit_imports", "slot545_parent", "slot545_cleanup"],
    "native": {
        "header": "native/core/ctor_slot_545_b2ec0.hpp",
        "source": "native/core/ctor_slot_545_b2ec0.cpp",
        "test": "native/tests/test_ctor_slot_545.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot545Rva": 0xB2EC0,
            "kSlot545Size": 0x295,
            "kSlot545MatrixHelperRva": 0xAB860,
            "kSlot545DtorRva": 0xB0A10,
            "kSlot545StateSize": 0x2F8,
            "kSlot545ObjectOffset": 0x120,
            "kSlot545ObjectSize": 0x1D0,
            "kSlot545BasisOffset": 0xC8,
            "kSlot545ScalarOffset": 0xE8,
        },
        "header_tokens": [
            "never back into the original ELF image",
            "Inputs are snapshotted before output stores",
            "A missing or false host allocator hook stops at the failing release stage.",
        ],
        "source_order": [
            "slot545_matrix_multiply(output, first_a, first_b);",
            "store_matrix(0x00, output);",
            "slot545_matrix_multiply(output, second_a, second_b);",
            "store_matrix(0x40, output);",
            "slot545_matrix_multiply(output, third_a, second_b);",
            "store_matrix(0x80, output);",
            "std::memcpy(state.bytes.data() + kSlot545BasisOffset,",
            "store_u32(state.bytes, kSlot545ScalarOffset, 0x3F800000);",
            "std::memcpy(object, kObjectPrefixWords.data(), sizeof(kObjectPrefixWords));",
            "result.atexit_recorded = log->record(kSlot545DtorRva, object);",
        ],
        "test_tokens": [
            "actual_basis == basis",
            "state.bytes.data() + sotc::kSlot545ObjectOffset",
            "cleanup.release_attempts == 0 && release_count == 0",
            "first_failure.failure == sotc::Slot545DtorFailure::first_release",
            "second_failure.failure == sotc::Slot545DtorFailure::second_release",
            "guard.error == sotc::Slot545Error::stack_guard_mismatch",
        ],
        "cmake_tokens": [
            "core/ctor_slot_545_b2ec0.cpp",
            "COMPILE_OPTIONS \"-ffp-contract=off\"",
            "add_executable(test_ctor_slot_545 tests/test_ctor_slot_545.cpp)",
            "add_test(NAME ctor_slot_545 COMMAND test_ctor_slot_545)",
        ],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(
        SPEC,
        "slot-545 matrix/static-state writes, one atexit identity, and two conditional tag-2 release stages; "
        "native model only, no startup registration or game-runtime claim",
    ))
