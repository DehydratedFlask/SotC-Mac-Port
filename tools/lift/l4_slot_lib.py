"""Shared driver for the lane-L4 single-slot ELF checkers (startup constructor slots 343..356).

Each `check_ctor_slot_<N>.py` supplies a declarative SPEC; this module runs the byte-level checks against
the pinned private ELF (`local/re/eboot.elf`, read-only; nothing is loaded, relocated or executed), then
the native-source token checks, then a mutation self-test that must reject every mutant.

SPEC keys
  slot, base, size, body_sha256     constructor-table word, body extent and whole-body hash
  pad                               hex bytes that follow the body (alignment padding)
  pins                              list of operand pins read from the body (see PIN KINDS)
  strings                           [(rva, bytes)] NUL-terminated strings the body references
  lengths                           [(text, imm_site, imm_hex)] explicit string-length immediates
  bool_tags / float_tags            functor vtable tags whose four reloc slots are checked
  callees                           names from COMMON_CALLEES whose bodies are hash-pinned
  native                            dict(header=..., source=..., test=..., consts={}, header_tokens=[],
                                    source_order=[], test_tokens=[], cmake_tokens=[])

PIN KINDS (every one reads bytes; the mutation test flips the first and last byte of each)
  ("rip", site, length, disp_off, target)   rip-relative operand resolves to `target`
  ("call", site, target) / ("jmp", site, target)   rel32 branch target
  ("imm", site, hex)                        exact instruction bytes (immediate stores, indirect calls)
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import (ATEXIT, DSO, RELEASE, ROOT, TABLE_START, Elf, const_value, digest,  # noqa: E402
                           require)

# (rva, size, sha256) of the shared callees the nine slots reuse.
COMMON_CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "float_dtor": (0x20F0, 0x36, "e93fb896a676df342779b680fc43ca88aa08aaab78eece0b260f1b6b93df5579"),
    "cmd_dtor": (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_toggle_from_var_ctor": (0x26DB0, 0xEF, "903ee574f90468eb5c2ed01f6d299b010dbafd8ed3d90f18ed9a283754a62c6e"),
    "unbounded_float_ctor": (0x6AA00, 0x142, "ee90dea5d0e2ed59ce83ff52c5a3732419635622c7b422f3eff54fd1f5384ebb"),
    "bounded_float_ctor": (0xC300, 0x160, "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"),
    "release_f590": (0xF590, 0x49, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47"),
    "float_fn_copy": (0xC4A0, 0x14, "0137c6226a7dcb380747d73f16a87fab37c63e9c2b8f67d0d1a8f875f2d44a08"),
    "float_fn_clone": (0xC4C0, 0x14, "5f93b13fd6577d64a7242762148d8d994a40ebd9480009297cce40f30caea597"),
    "float_fn_move": (0xC4E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "float_fn_destroy": (0xC4F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "bool_fn_copy": (0xC2A0, 0x14, "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"),
    "bool_fn_clone": (0xC2C0, 0x14, "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"),
    "bool_fn_move": (0xC2E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "bool_fn_destroy": (0xC2F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "zero_vector_dtor": (0x7D3AF0, 0xE, "4d71313522e028c75b9c793a78cc4ce00cbca9600d874c7e13d83677b4da88e9"),
}

FLOAT_FN_BODIES = (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)
BOOL_FN_BODIES = (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)
BOOL_CLASS_FN_VTBL = 0x1594280      # stored by the bool functor copy/clone bodies
FLOAT_CLASS_FN_VTBL = 0x1594318     # stored by the float functor copy/clone bodies


def run_pin(e: Elf, pin: tuple) -> None:
    kind = pin[0]
    if kind == "rip":
        _, site, length, disp_off, target = pin
        require(e.rip(site, length, disp_off) == target, f"rip operand at {site:#x} -> {target:#x}")
    elif kind == "call":
        require(e.call(pin[1]) == pin[2], f"call at {pin[1]:#x} -> {pin[2]:#x}")
    elif kind == "jmp":
        require(e.jmp(pin[1]) == pin[2], f"jmp at {pin[1]:#x} -> {pin[2]:#x}")
    elif kind == "imm":
        e.bytes_at(pin[1], pin[2], f"instruction bytes {pin[2]}")
    else:  # pragma: no cover
        raise SystemExit(f"FAIL unknown pin kind {kind}")


def run_functor_checks(e: Elf, spec: dict) -> None:
    for tag in spec.get("bool_tags", ()):
        for i, body in enumerate(BOOL_FN_BODIES):
            require(e.reloc_target(tag + 8 * i) == body, f"bool functor vtable {tag:#x} slot {i}")
    for tag in spec.get("float_tags", ()):
        for i, body in enumerate(FLOAT_FN_BODIES):
            require(e.reloc_target(tag + 8 * i) == body, f"float functor vtable {tag:#x} slot {i}")
    if spec.get("bool_tags"):
        require(e.lea(0xC2A0 + 4) == BOOL_CLASS_FN_VTBL, "bool functor copy stores class vtable 0x1594280")
        require(e.lea(0xC2C0) == BOOL_CLASS_FN_VTBL, "bool functor clone stores class vtable 0x1594280")
        require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
        require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890")
        require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930")
        e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
        e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
    if spec.get("float_tags"):
        require(e.lea(0xC4A0 + 4) == FLOAT_CLASS_FN_VTBL, "float functor copy stores class vtable 0x1594318")
        require(e.lea(0xC4C0) == FLOAT_CLASS_FN_VTBL, "float functor clone stores class vtable 0x1594318")
        require(e.reloc_target(0x15942B0) == 0x20F0, "float class slot 0 = 0x20f0")


def run_elf_checks(e: Elf, spec: dict) -> None:
    base, size = spec["base"], spec["size"]
    if not spec.get("no_table"):
        require(e.u64(TABLE_START - 8 * spec["slot"]) == base, f"table word {spec['slot']}")
    e.body_hash(f"slot{spec['slot']}", base, size, spec["body_sha256"])
    for name in spec.get("callees", ()):
        rva, sz, sha = COMMON_CALLEES[name]
        e.body_hash(name, rva, sz, sha, always=True)
    e.bytes_at(base + size, spec["pad"], "padding after the body")
    for pin in spec["pins"]:
        run_pin(e, pin)
    for rva, text in spec["strings"]:
        e.cstring(rva, text)
    for text, site, imm_hex in spec.get("lengths", ()):
        e.bytes_at(site, imm_hex, f"length immediate for {text!r}")
        require(int.from_bytes(bytes.fromhex(imm_hex)[-4:], "little") == len(text), f"length of {text!r}")
    run_functor_checks(e, spec)


def read_native(spec: dict) -> dict[str, str]:
    nat = spec["native"]
    out = {}
    for key in ("header", "source", "test", "shapes_h", "shapes_c", "cmake"):
        path = nat.get(key)
        if path:
            out[key] = (ROOT / path).read_text()
    return out


def check_native(spec: dict, t: dict[str, str]) -> None:
    nat = spec["native"]
    for name, value in nat.get("consts", {}).items():
        require(const_value(t["header"], name) == value, f"native {name}")
    for token in nat.get("header_tokens", ()):
        require(token in t["header"], f"header token {token!r}")
    src = t["source"]
    pos = [src.index(token) if token in src else -1 for token in nat.get("source_order", ())]
    require(-1 not in pos and pos == sorted(pos) and len(set(pos)) == len(pos), "native call order")
    for token in nat.get("test_tokens", ()):
        require(token in t["test"], f"test token {token!r}")
    for token in nat.get("shapes_tokens", ()):
        require(token in t["shapes_c"] or token in t["shapes_h"], f"shapes token {token!r}")
    for token in nat.get("cmake_tokens", ()):
        require(token in t["cmake"], f"cmake token {token!r}")
    # Every ELF string the body references is a literal of the native header (borrowed views of the same text).
    for _, text in spec["strings"]:
        if text:
            require(f'"{text.decode()}"' in t["header"], f"native string {text!r}")
    for tag in list(spec.get("bool_tags", ())) + list(spec.get("float_tags", ())):
        require(f"{tag:#x}" in t["header"] + t.get("shapes_h", "") + t["source"], f"native tag {tag:#x}")


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


def mutation_self_test(e: Elf, spec: dict, texts: dict[str, str]) -> tuple[int, int]:
    pins = sorted(e.pins)
    e.skip_hash = True
    e.recording = False
    ran = survivors = 0
    for addr, size in pins:
        if (addr, size) == (spec["base"], spec["size"]):
            continue   # whole-body extent: covered by the body-hash mutation below
        for byte_addr in sorted({addr, addr + size - 1}):
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                run_elf_checks(e, spec)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    e.overlay = {}
    for slot in sorted(e.reloc_pins):
        e.reloc_flip = {slot}
        ran += 1
        try:
            run_elf_checks(e, spec)
        except SystemExit:
            continue
        survivors += 1
        print(f"mutation survived: relocation addend at {slot:#x}", file=sys.stderr)
    e.reloc_flip = set()
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    e.overlay = {spec["base"] + 0x20: e.raw(spec["base"] + 0x20, 1)[0] ^ 0x80}
    expect_failure("body hash", lambda: run_elf_checks(e, spec))
    e.overlay = {}

    nat = spec["native"]
    native_ran = 0
    for key, tokens in (("header", nat.get("header_tokens", ())), ("source", nat.get("source_order", ())),
                        ("test", nat.get("test_tokens", ())), ("cmake", nat.get("cmake_tokens", ())),
                        ("shapes_c", nat.get("shapes_tokens", ()))):
        for token in tokens:
            if key not in texts or token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(spec, m))
            native_ran += 1
    for name, value in nat.get("consts", {}).items():
        pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(spec, m))
        native_ran += 1
    for _, text in spec["strings"]:
        if text:
            mutated = dict(texts)
            mutated["header"] = texts["header"].replace(f'"{text.decode()}"', '"MUTATED"')
            expect_failure(f"string {text!r}", lambda m=mutated: check_native(spec, m))
            native_ran += 1
    return ran, native_ran


def main(spec: dict, summary: str) -> int:
    mutate = "--no-mutate" not in sys.argv[1:]
    e = Elf()
    if "--tamper" in sys.argv[1:]:
        # Demonstration of the fail-closed path: flip one body byte in the in-memory overlay, expect exit 1.
        e.overlay = {spec["base"] + 0x20: e.raw(spec["base"] + 0x20, 1)[0] ^ 0x80}
        run_elf_checks(e, spec)
        print("tamper NOT detected", file=sys.stderr)
        return 0
    texts = read_native(spec)
    e.recording = True
    run_elf_checks(e, spec)
    e.recording = False
    check_native(spec, texts)
    note = ""
    if mutate:
        elf_runs, native_runs = mutation_self_test(e, spec, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_{spec.get('label', spec['slot'])}: body {spec['body_sha256'][:12]}... {spec['size']} B hash/extent + "
          f"{len(spec['pins'])} operand pins, {summary}; guest bytes NOT executed{note}")
    return 0
