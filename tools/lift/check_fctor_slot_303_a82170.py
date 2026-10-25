#!/usr/bin/env python3
"""ELF-tie startup constructor slot 303 (0xa82170): the `ProfileCPU` bool console variable and its
"Profile CPU" debug-menu toggle.

Offline only: opens the pinned private ELF, reads bytes, never loads, relocates or executes an
original RVA. It pins:
  * the table word for index 303, the 455-byte body (sha256 and extent: the byte after the final
    `call` is padding); full instruction bytes/disassembly are private evidence under local/;
  * the decoded operands the native lift relies on: the object / value cell / functor tag, every
    immediate store in order, the stack views, the call targets and the atexit registrations;
  * the shared callees it reuses (0x20af0, 0x20d60, 0x26ea0, destructors 0x20b0 / 0x272d0) by hash,
    and the functor vtable 0x15d2478 (RELATIVE relocations -> the four bool-functor bodies);
  * the native header / source / test / CMake / production-row tokens;
  * a mutation self-test (default on, `--no-mutate` to skip): the first and last byte of every pin is
    flipped in an in-memory overlay (whole-body hash disabled) and every native token / constant is
    perturbed; each mutant must be rejected.

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import (ATEXIT, DSO, RELEASE, ROOT, TABLE_START, Elf, const_value, digest,  # noqa: E402
                           require)

BASE = 0xA82170
SIZE = 455
BODY_SHA256 = "94b8d084c1313de9bb70a9735702f4b53c8ce639cb280b251dca1ab7f9cdb5e1"

VAR = 0x32FD228
ITEM = 0x32FD2F0
FN_TAG = 0x15D2478

# (name, rva, size, sha256) of the shared callees this slot reuses (hashes also pinned by
# check_fctor_slots_298_299_301.py).
CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "bool_fn_copy": (0xC2A0, 0x14, "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"),
    "bool_fn_clone": (0xC2C0, 0x14, "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"),
    "bool_fn_move": (0xC2E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "bool_fn_destroy": (0xC2F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
}


NATIVE_FILES = {
    "vars_h": "native/core/concmd_vars.hpp",
    "vars_c": "native/core/concmd_vars.cpp",
    "header": "native/core/ctor_slot_303_a82170.hpp",
    "source": "native/core/ctor_slot_303_a82170.cpp",
    "test": "native/tests/test_ctor_slot_303.cpp",
    "startup_c": "native/core/startup_ctors.cpp",
    "startup_h": "native/core/startup_ctors.hpp",
    "cmake": "native/CMakeLists.txt",
}

HEADER_CONSTANTS = {
    "kSlot303Rva": BASE, "kSlot303BodySize": SIZE, "kSlot303VarRva": VAR, "kSlot303ItemRva": ITEM,
    "kSlot303FnVtbl": FN_TAG, "kSlot303Value": 0, "kSlot303Default": 0, "kSlot303ItemFlags": 0x30,
}
HEADER_TOKENS = [
    'kSlot303Name[] = "ProfileCPU"', 'kSlot303Label[] = "Profile"', 'kSlot303Text[] = "Profile CPU"',
    "struct Slot303Objects", "ConCmdBoolVar var{};", "DebugMenuItem item{};",
]
SOURCE_ORDER = [   # store / call order of 0xa82188..0xa8230e
    "init_bool_var_static(objects.var, name, description, kSlot303FnVtbl, concmd,",
    "kSlot303Value, kSlot303Default",
    "debug_menu_target_for(objects.var)",
    "init_debug_menu_item_26ea0(objects.item, label, text, target, kSlot303ItemFlags)",
    "register_debug_menu_item_268b0(objects.item, menu, concmd.atexit_log)",
    "release_inert_view_f590(text)",
    "release_inert_view_f590(label)",
    "concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item)",
]
VARS_TOKENS = [
    "ConCmdRegistry& registry,\n                                                 std::uint8_t value = 1,\n"
    "                                                 std::uint8_t default_value = 1) noexcept;",
    "std::uint8_t value,\n                                   std::uint8_t default_value) noexcept {",
    "var.value = value;", "var.default_value = default_value;",
]
VARS_ORDER = ["var.base.vptr = kBoolVarMidVptr", "var.value_ref = &var.value", "std::memset(var.zero69",
              "std::memset(var.zero70", "var.fn_payload = 0", "var.fn_vptr = fn_vptr_tag", "var.fn_aux = 0",
              "release_inert_view_f590(description)", "var.value = value;", "var.default_value = default_value;",
              "register_concmd_20d60", "release_inert_view_f590(name)", "atexit_log.record(kBoolVarDtorRva"]
STARTUP_ROW_TOKENS = [
    "init_slot303_a82170(s.slot303, s.concmd, s.debug_menu).ok) s.init_failed = true;",
]
STARTUP_HEADER_TOKENS = ['#include "ctor_slot_303_a82170.hpp"', "Slot303Objects slot303{};"]
TEST_TOKENS = ["kSlot303Rva == 0xa82170", "section_params()", "section_after_299()", "section_failures()",
               "section_production()", "init_bool_var_static(z, view(\"zero\", 4), desc, 0x15d2478, r3, 0, 0)"]
CMAKE_TOKENS = ["core/ctor_slot_303_a82170.cpp", "add_executable(test_ctor_slot_303 tests/test_ctor_slot_303.cpp)",
                "add_test(NAME ctor_slot_303 COMMAND test_ctor_slot_303)"]


def run_elf_checks(e: Elf) -> None:
    require(e.u64(TABLE_START - 8 * 303) == BASE, "table word 303")
    e.body_hash("slot303", BASE, SIZE, BODY_SHA256)
    for name, (rva, size, sha) in CALLEES.items():
        e.body_hash(name, rva, size, sha, always=True)

    # ---- extent: complete-body hash above; final return/failure call followed by padding
    e.bytes_at(BASE + SIZE, "90" * 9, "nop padding after the body")
    e.bytes_at(0xA82331, "c3", "epilogue ret")

    # ---- prologue / stack canary
    e.bytes_at(BASE, "55" "4889e5" "4157" "4156" "4155" "4154" "53" "4883ec48", "prologue, frame 0x48")
    require(e.rip(0xA82181, 7, 3) == 0x15FE518 and e.rip(0xA82313, 7, 3) == 0x15FE518, "canary cell 0x15fe518")
    e.bytes_at(0xA821BF, "488945d0", "canary saved at [rbp-0x30]")
    e.bytes_at(0xA8231D, "483b45d0", "canary compared with [rbp-0x30]")
    require(e.call(0xA82332) == 0xF8F3A0, "canary failure path calls 0xf8f3a0 (stack_chk_fail PLT, inferred)")

    # ---- block A: the ProfileCPU variable
    require(e.lea(0xA82188) == 0x1594208, "A final vtable base 0x1594208 (+0x10 = 0x1594218)")
    e.bytes_at(0xA821BB, "4883c310", "A rbx += 0x10")
    require(e.lea(0xA8218F) == VAR, "A object 0x32fd228")
    require(e.lea(0xA82196) == 0x12C6B81, "A name string 0x12c6b81")
    e.cstring(0x12C6B81, b"ProfileCPU")
    require(e.lea(0xA8219D) == 0x12C6BEF, "A empty description 0x12c6bef")
    require(e.read(0x12C6BEF, 1) == b"\0", "A description points at a NUL byte")
    e.bytes_at(0xA821B0, "b901000000", "A flags ecx = 1")
    e.bytes_at(0xA821C3, "48897590" "488955b0", "A name view ptr [rbp-0x70], description view ptr [rbp-0x50]")
    e.bytes_at(0xA821D1, "48c745980a000000", "A name length 0xa")
    e.bytes_at(0xA821D9, "c5f81145b8" "c5f81145a0", "A description view zero words [rbp-0x48], name view zero words [rbp-0x60]")
    e.bytes_at(0xA821EA, "48c745c800000000", "A description view last word [rbp-0x38] = 0")
    e.bytes_at(0xA821AC, "c5f857c0", "A xmm0 = 0")
    e.bytes_at(0xA821A4, "4c8d7590" "4c8d65b0", "A r14 = &name view, r12 = &description view")
    e.bytes_at(0xA821CB, "4c89f6" "4c89e2", "A rsi = name view, rdx = description view")
    require(e.rip(0xA821E3, 7, 3) == VAR, "A [obj] = final vptr before the base ctor")
    e.bytes_at(0xA821E3, "48891d", "A pre-ctor vptr store is `mov [rip+d], rbx`")
    require(e.call(0xA821F2) == 0x20AF0, "A call base ctor 0x20af0")
    require(e.lea(0xA821F7) == 0x15940A0, "A mid vtable 0x15940a0 (+0x10 = 0x15940b0)")
    e.bytes_at(0xA82213, "4883c010", "A rax += 0x10")
    require(e.lea(0xA82201) == 0x32FD2E8 == VAR + 0xC0, "A value cell 0x32fd2e8 (= obj+0xc0)")
    require(e.lea(0xA8220C) == FN_TAG, "A functor vtable tag 0x15d2478")
    require(e.rip(0xA82217, 7, 3) == VAR, "A [obj] = mid vptr")
    require(e.rip(0xA8221E, 7, 3) == VAR + 0x60, "A [obj+0x60] = &value")
    require(e.rip(0xA82225, 10, 2) == VAR + 0x69, "A dword [obj+0x69] = 0")
    e.bytes_at(0xA82225, "c705", "A dword store opcode")
    require(e.read(0xA8222B + 0, 4) == b"\0\0\0\0", "A dword store immediate 0")
    require(e.rip(0xA8222F, 8, 4) == VAR + 0x70, "A ymm zero [obj+0x70]")
    require(e.rip(0xA82237, 11, 3) == VAR + 0xA0, "A qword [obj+0xa0] = 0")
    require(e.rip(0xA82242, 7, 3) == VAR + 0xB0, "A [obj+0xb0] = functor vtable")
    require(e.rip(0xA82249, 11, 3) == VAR + 0xB8, "A qword [obj+0xb8] = 0")
    e.bytes_at(0xA82237 + 7, "00000000", "A [obj+0xa0] immediate 0")
    e.bytes_at(0xA82249 + 7, "00000000", "A [obj+0xb8] immediate 0")
    e.bytes_at(0xA821FE, "4c89e7", "A rdi = description view for the first release")
    require(e.call(0xA82254) == RELEASE, "A release of the description view")
    require(e.rip(0xA8225C, 7, 3) == VAR, "A [obj] = final vptr after the description release")
    e.bytes_at(0xA82259, "4c89ff", "A rdi = obj for register")
    require(e.rip(0xA82263, 7, 2) == VAR + 0xC0 and e.read(0xA82269, 1) == b"\x00", "A byte [obj+0xc0] = 0 (value)")
    require(e.rip(0xA8226A, 7, 2) == VAR + 0x68 and e.read(0xA82270, 1) == b"\x00", "A byte [obj+0x68] = 0 (default)")
    e.bytes_at(0xA82263, "c605", "A value store opcode")
    e.bytes_at(0xA8226A, "c605", "A default store opcode")
    require(e.call(0xA82271) == 0x20D60, "A register 0x20d60")
    e.bytes_at(0xA82276, "4c89f7", "A rdi = name view for the second release")
    require(e.call(0xA82279) == RELEASE, "A release of the name view")
    require(e.lea(0xA8227E) == DSO and e.lea(0xA82285) == 0x20B0, "A atexit dso / destructor 0x20b0")
    e.bytes_at(0xA8228C, "4c89fe" "4c89f2", "A atexit rsi = obj, rdx = dso")
    require(e.call(0xA82292) == ATEXIT, "A __cxa_atexit")

    # ---- block B: the debug-menu toggle
    require(e.lea(0xA82297) == 0x12C6B94, "B text string 0x12c6b94")
    e.cstring(0x12C6B94, b"Profile CPU")
    require(e.lea(0xA822AD) == 0x12C6B8C, "B label string 0x12c6b8c")
    e.cstring(0x12C6B8C, b"Profile")
    require(e.lea(0xA8229E) == ITEM and ITEM == VAR + 0xC0 + 8, "B item 0x32fd2f0 directly after A's value byte")
    e.bytes_at(0xA822A5, "4c8d6db0" "488d5d90", "B r13 = &label view, rbx = &text view")
    e.bytes_at(0xA822B8, "41b830000000", "B flags r8d = 0x30")
    e.bytes_at(0xA822BE, "48894d90", "B text view ptr [rbp-0x70] = rcx")
    e.bytes_at(0xA822CE, "488945b0", "B label view ptr [rbp-0x50] = rax")
    e.bytes_at(0xA822D2, "48c745b807000000", "B label length 7")
    e.bytes_at(0xA822DA, "48c745980b000000", "B text length 0xb")
    e.bytes_at(0xA822E2, "c5f81145c0" "c5f81145a0", "B view zero words [rbp-0x40], [rbp-0x60]")
    e.bytes_at(0xA822C2, "4c89e7" "4c89ee" "4889da" "4c89f9", "B rdi=item, rsi=label, rdx=text, rcx=target(A)")
    require(e.call(0xA822EC) == 0x26EA0, "B call 0x26ea0")
    e.bytes_at(0xA822F1, "4889df", "B first release is the text view (rbx)")
    require(e.call(0xA822F4) == RELEASE, "B release of the text view")
    e.bytes_at(0xA822F9, "4c89ef", "B second release is the label view (r13)")
    require(e.call(0xA822FC) == RELEASE, "B release of the label view")
    require(e.lea(0xA82301) == 0x272D0, "B atexit destructor 0x272d0")
    e.bytes_at(0xA82308, "4c89e6" "4c89f2", "B atexit rsi = item, rdx = dso (r14)")
    require(e.call(0xA8230E) == ATEXIT, "B __cxa_atexit")

    # ---- shared bool class and the functor vtable 0x15d2478
    require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
    require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890 (returns 1)")
    require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930 (returns 0)")
    e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
    e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
    bodies = (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)
    for i, body in enumerate(bodies):
        require(e.reloc_target(FN_TAG + 8 * i) == body, f"functor vtable 0x15d2478 slot {i}")
    copy_body = e.read(bodies[0], 0x14)
    require(copy_body.startswith(bytes.fromhex("488b4610")) and copy_body.endswith(b"\xc3"), "functor copy shape")
    require(e.lea(bodies[0] + 4) == 0x1594280, "functor copy stores class vtable 0x1594280")
    require(e.lea(bodies[1]) == 0x1594280, "functor clone stores class vtable 0x1594280")
    e.bytes_at(bodies[2], "488b461048894710c3", "functor move body")
    e.bytes_at(bodies[3], "c3", "functor destroy is a bare ret")
    # The toggle constructor 0x26ea0 branches on virtual slots 3 / 6 of the target's class and keeps
    # the default range {1.0, 10.0} unless slot3 == 1 && slot6 == 3.
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041" "4489bba0000000", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
    require(e.jmp(t + 0xE8) == 0x268B0, "0x26ea0 tail-calls 0x268b0")


def read_native() -> dict[str, str]:
    return {key: (ROOT / path).read_text() for key, path in NATIVE_FILES.items()}


def check_native(t: dict[str, str]) -> None:
    for name, value in HEADER_CONSTANTS.items():
        require(const_value(t["header"], name) == value, f"native {name}")
    for token in HEADER_TOKENS:
        require(token in t["header"], f"header token {token!r}")
    pos = [t["source"].index(token) if token in t["source"] else -1 for token in SOURCE_ORDER]
    require(-1 not in pos and pos == sorted(pos) and len(set(pos)) == len(pos), "native slot 303 call order")
    for token in VARS_TOKENS:
        require(token in t["vars_h"] or token in t["vars_c"], f"concmd_vars token {token!r}")
    require(all(token in t["vars_c"] for token in VARS_ORDER), "concmd_vars store tokens present")
    pos = [t["vars_c"].index(token) for token in VARS_ORDER]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native bool shape store order")
    # slot 301 and 299-B keep the defaults: no explicit value arguments at their call sites.
    require("kSlot301FnVtbl, registry)" in (ROOT / "native/core/ctor_slot_301_ac9550.cpp").read_text(),
            "slot 301 still uses the defaulted value/default")
    require("kSlot299AdjustFnVtbl, concmd)" in (ROOT / "native/core/ctor_slot_299_b0c7a0.cpp").read_text(),
            "slot 299 block B still uses the defaulted value/default")
    # production row
    match = re.search(r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};", t["startup_c"], re.DOTALL)
    require(match is not None, "cannot find recovered constructor array")
    block = match.group(1)
    rows = [(int(rva, 16), int(i)) for rva, i in re.findall(
        r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*\[", block, re.MULTILINE)]
    require((BASE, 303) in rows, "missing registered row 303 / 0xa82170")
    indices = [i for _, i in rows]
    require(indices == sorted(indices) and len(indices) == len(set(indices)), "rows strictly increasing")
    require(indices.index(301) < indices.index(303) < indices.index(304), "row 303 sits between 301 and 304")
    for token in STARTUP_ROW_TOKENS:
        require(token in block, f"production row token {token!r}")
    for token in STARTUP_HEADER_TOKENS:
        require(token in t["startup_h"], f"startup header token {token!r}")
    for token in TEST_TOKENS:
        require(token in t["test"], f"test token {token!r}")
    for token in CMAKE_TOKENS:
        require(token in t["cmake"], f"cmake token {token!r}")


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


def mutation_self_test(e: Elf, texts: dict[str, str]) -> tuple[int, int]:
    pins = sorted(e.pins)
    e.skip_hash = True
    e.recording = False
    ran = 0
    survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                run_elf_checks(e)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    e.overlay = {}
    for slot in sorted(e.reloc_pins):
        e.reloc_flip = {slot}
        ran += 1
        try:
            run_elf_checks(e)
        except SystemExit:
            continue
        survivors += 1
        print(f"mutation survived: relocation addend at {slot:#x}", file=sys.stderr)
    e.reloc_flip = set()
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    # The whole-body hash catches changes outside the selected decoded semantic pins.
    e.overlay = {BASE + 0x20: e.raw(BASE + 0x20, 1)[0] ^ 0x80}
    expect_failure("body hash", lambda: run_elf_checks(e))
    e.overlay = {}

    native_ran = 0
    for key, tokens in (("header", HEADER_TOKENS), ("source", SOURCE_ORDER), ("vars_c", VARS_ORDER),
                        ("startup_c", STARTUP_ROW_TOKENS), ("startup_h", STARTUP_HEADER_TOKENS),
                        ("test", TEST_TOKENS), ("cmake", CMAKE_TOKENS), ("vars_h", VARS_TOKENS[:1])):
        for token in tokens:
            if token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(m))
            native_ran += 1
    for name, value in HEADER_CONSTANTS.items():
        pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    mutate = "--no-mutate" not in sys.argv[1:]
    e = Elf()
    texts = read_native()
    e.recording = True
    run_elf_checks(e)
    e.recording = False
    check_native(texts)
    note = ""
    if mutate:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(
        f"OK check_fctor_slot_303: body {BODY_SHA256[:12]}... 455 B hash/extent + selected operands, ProfileCPU 0x32fd228 "
        f"(value 0, default 0, tag 0x15d2478), toggle 0x32fd2f0 (flags 0x30), atexit 0x20b0 + 0x272d0; "
        f"guest bytes NOT executed{note}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
