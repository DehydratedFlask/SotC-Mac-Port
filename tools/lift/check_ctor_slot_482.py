#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 482 (RVA 0x2fb2a0, 323 bytes), lane M4.

One bool variable `network_WarnAboutServerObjectsOnClient` (tag 0x15a52f8, value 1, default 1, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. The stored
value/default bytes are tied to the native constants. A mutation self-test flips the first/last byte of every
pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {'slot': 482,
 'base': 3125920,
 'size': 323,
 'body_sha256': '5f0835617997e211a67d1084aadd8dd5cce6809da8f8337f80c8aede37f16ff0',
 'pad': '90909090909090909090909090',
 'callees': ['concmd_base_ctor',
             'concmd_register',
             'bool_dtor',
             'release_f590',
             'bool_fn_copy',
             'bool_fn_clone',
             'bool_fn_move',
             'bool_fn_destroy',
             'bool_slot3',
             'bool_slot6'],
 'bool_tags': [22696696],
 'float_tags': [],
 'strings': [(16600361, b'network_WarnAboutServerObjectsOnClient'), (16600717, b'')],
 'lengths': [('network_WarnAboutServerObjectsOnClient', 3126018, '48c7459826000000')],
 'pins': [
        ("rip", 0x2fb2b1, 7, 3, 0x15fe518),
        ("rip", 0x2fb2b8, 7, 3, 0x1594208),
        ("rip", 0x2fb2bf, 7, 3, 0x2eefd30),
        ("rip", 0x2fb2c6, 7, 3, 0xfd4d29),
        ("rip", 0x2fb2cd, 7, 3, 0xfd4e8d),
        ("imm", 0x2fb2e0, "b901000000"),
        ("imm", 0x2fb2ec, "4883c310"),
        ("imm", 0x2fb302, "48c7459826000000"),
        ("rip", 0x2fb314, 7, 3, 0x2eefd30),
        ("imm", 0x2fb31b, "48c745c800000000"),
        ("call", 0x2fb323, 0x20af0),
        ("rip", 0x2fb328, 7, 3, 0x15940a0),
        ("rip", 0x2fb332, 7, 3, 0x2eefdf0),
        ("rip", 0x2fb33d, 7, 3, 0x15a52f8),
        ("imm", 0x2fb344, "4883c010"),
        ("rip", 0x2fb348, 7, 3, 0x2eefd30),
        ("rip", 0x2fb34f, 7, 3, 0x2eefd90),
        ("rip", 0x2fb356, 10, 2, 0x2eefd99),
        ("imm", 0x2fb356, "c705394abf0200000000"),
        ("rip", 0x2fb360, 8, 4, 0x2eefda0),
        ("rip", 0x2fb368, 11, 3, 0x2eefdd0),
        ("imm", 0x2fb368, "48c7055d4abf0200000000"),
        ("rip", 0x2fb373, 7, 3, 0x2eefde0),
        ("rip", 0x2fb37a, 11, 3, 0x2eefde8),
        ("imm", 0x2fb37a, "48c705634abf0200000000"),
        ("call", 0x2fb385, 0xf590),
        ("rip", 0x2fb38d, 7, 3, 0x2eefd30),
        ("rip", 0x2fb394, 7, 2, 0x2eefdf0),
        ("imm", 0x2fb394, "c605554abf0201"),
        ("rip", 0x2fb39b, 7, 2, 0x2eefd98),
        ("imm", 0x2fb39b, "c605f649bf0201"),
        ("call", 0x2fb3a2, 0x20d60),
        ("call", 0x2fb3aa, 0xf590),
        ("rip", 0x2fb3af, 7, 3, 0x20b0),
        ("rip", 0x2fb3b6, 7, 3, 0x1604218),
        ("call", 0x2fb3c0, 0xf8f390),
        ("call", 0x2fb3de, 0xf8f3a0),
    ],
 'native': {'header': 'native/core/ctor_slot_482_2fb2a0.hpp',
            'source': 'native/core/ctor_slot_482_2fb2a0.cpp',
            'test': 'native/tests/test_ctor_slot_482.cpp',
            'cmake': 'native/lanes/m4.cmake',
            'consts': {'kSlot482Rva': 3125920, 'kSlot482Size': 323, 'kSlot482ObjectRva': 49216816},
            'header_tokens': ['kSlot482FnTag = 0x15a52f8'],
            'source_order': ['init_bool_var_site(objects.var, kSlot482Name, kSlot482FnTag, concmd, kSlot482Value, '
                             'kSlot482Default)'],
            'test_tokens': ['kSlot482Rva == 0x2fb2a0', 'section_alone()', 'section_failures()'],
            'cmake_tokens': ['core/ctor_slot_482_2fb2a0.cpp',
                             'add_executable(test_ctor_slot_482 tests/test_ctor_slot_482.cpp)',
                             'add_test(NAME ctor_slot_482 COMMAND test_ctor_slot_482)']},
 'extra': {'stores': [(3126170, 1, 'kSlot482Value'), (3126177, 1, 'kSlot482Default')], 'int': False, 'string': False}}


# ---- lane-M4 additions (self-contained; the shared driver l4_slot_lib is reused unmodified) ----------------
import re  # noqa: E402

from slot_pin_util import const_value, require  # noqa: E402

lib.COMMON_CALLEES.update({
    "int_var_worker": (0x3A980, 0x142, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
    "int_dtor": (0x18A60, 0x36, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
    "int_fn_copy": (0xC720, 0x14, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
    "int_fn_clone": (0xC740, 0x14, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    "int_fn_move": (0xC760, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "int_fn_destroy": (0xC770, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "string_ctor": (0x257A0, 0x14C, "9ac816cf6bebd628d2a9b2941571869d4f1d08994081f5c61db7d165270115f2"),
    "string_inner": (0x258F0, 0x157, "5555aff241162d36f3c442755b6342ac872e92bbdf296839033b3a2b086d6950"),
    "empty_ctor_f180": (0xF180, 0x1C, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c"),
    "sfn_copy": (0x25A50, 0x14, "73be203a7605ec0af252ac3df43ce59dc067315828552214dff5a1160c06fb6e"),
    "sfn_clone": (0x25A70, 0x14, "5400d3e5266832c0e4cde2131420856e018aeac1fa816f68d2e5487d98496897"),
    "sfn_move": (0x25A90, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "sfn_destroy": (0x25AA0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "str_slot3": (0x246A0, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "str_slot6": (0x24780, 0x6, "e36623525b7c35b277173e56616b2d865cf6515aa2be1d12d6814ef725cecffb"),
})

_state: dict = {}
_orig_run_elf_checks = lib.run_elf_checks
_orig_check_native = lib.check_native
_orig_mutation = lib.mutation_self_test


def _run_elf_checks(e, spec):
    _orig_run_elf_checks(e, spec)
    _state["e"] = e
    x = spec["extra"]
    if x.get("int"):
        # The argument-functor tag and the slot-body-identical table the native worker model is given as a surrogate.
        for tag in sorted({x["arg_tag"], 0x15CE8C0}):
            for i, body in enumerate((0xC720, 0xC740, 0xC760, 0xC770)):
                require(e.reloc_target(tag + 8 * i) == body, f"int functor table {tag:#x} slot {i}")
        e.bytes_at(0xC770, "c3", "int functor destroy is a bare ret")
        require(e.rip(0xC724, 7, 3) == 0x15944A0, "int functor copy stores class tag 0x15944a0")
        require(e.rip(0xC740, 7, 3) == 0x15944A0, "int functor clone stores class tag 0x15944a0")
        e.bytes_at(0xC760, "488b461048894710c3", "int functor move body")
        require(e.reloc_target(0x1594C08) == 0x18A60, "int class slot 0 = 0x18a60")
        require(e.reloc_target(0x1594C08 + 0x10) == 0x20D60, "int class slot 2 = 0x20d60")
    if x.get("string"):
        require(e.reloc_target(0x1594EF0) == 0x23C90, "string class slot 0 = 0x23c90")
        require(e.reloc_target(0x1594EF0 + 0x10) == 0x20D60, "string class slot 2 = 0x20d60")
        require(e.reloc_target(0x1594EF0 + 0x18) == 0x246A0, "string class slot 3 = 0x246a0 (returns 1)")
        require(e.reloc_target(0x1594EF0 + 0x30) == 0x24780, "string class slot 6 = 0x24780 (returns 4)")
        e.bytes_at(0x246A0, "b801000000c3", "string slot 3: mov eax,1; ret")
        e.bytes_at(0x24780, "b804000000c3", "string slot 6: mov eax,4; ret")
        for i, body in enumerate((0x25A50, 0x25A70, 0x25A90, 0x25AA0)):
            require(e.reloc_target(0x1595068 + 8 * i) == body, f"string functor class 0x1595068 slot {i}")
        require(e.rip(0x25A54, 7, 3) == 0x1595068 and e.rip(0x25A70, 7, 3) == 0x1595068, "functor stores its class tag")
        require(e.rip(0x257BF, 7, 3) == 0x1594EE0 and e.rip(0x25921, 7, 3) == 0x1594F58, "string constructor vtables")


def _check_native(spec, t):
    _orig_check_native(spec, t)
    x = spec["extra"]
    e = _state["e"]
    for site, nbytes, name in x.get("stores", ()):
        stored = int.from_bytes(e.raw(site, nbytes), "little")
        require(stored == const_value(t["header"], name), f"ELF store at {site:#x} == native {name}")
    for name, value in x.get("shapes_h_consts", {}).items():
        require(const_value(t["shapes_h"], name) == value, f"shapes constant {name}")
    src = t.get("shapes_c", "")
    pos = [src.index(tok) if tok in src else -1 for tok in x.get("shapes_order", ())]
    require(-1 not in pos and pos == sorted(pos) and len(set(pos)) == len(pos), "shapes call order")


def _bump(text, name, value):
    pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
    out = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", text, count=1)
    require(out != text, f"constant {name} pattern did not match")
    return out


def _mutation_self_test(e, spec, texts):
    ran, native_ran = _orig_mutation(e, spec, texts)
    x = spec["extra"]
    extra = 0
    for site, nbytes, name in x.get("stores", ()):
        mutated = dict(texts)
        mutated["header"] = _bump(texts["header"], name, const_value(texts["header"], name))
        lib.expect_failure(f"stored value {name}", lambda m=mutated: lib.check_native(spec, m))
        extra += 1
    for name, value in x.get("shapes_h_consts", {}).items():
        mutated = dict(texts)
        mutated["shapes_h"] = _bump(texts["shapes_h"], name, value)
        lib.expect_failure(f"shapes constant {name}", lambda m=mutated: lib.check_native(spec, m))
        extra += 1
    for tok in x.get("shapes_order", ()):
        mutated = dict(texts)
        mutated["shapes_c"] = texts["shapes_c"].replace(tok, "\0MUTATED\0")
        lib.expect_failure(f"shapes order token {tok!r}", lambda m=mutated: lib.check_native(spec, m))
        extra += 1
    return ran, native_ran + extra


lib.run_elf_checks = _run_elf_checks
lib.check_native = _check_native
lib.mutation_self_test = _mutation_self_test


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'one bool variable `network_WarnAboutServerObjectsOnClient` (tag 0x15a52f8, value 1, default 1, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0)'))
