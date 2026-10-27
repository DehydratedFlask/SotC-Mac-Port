"""Shared harness for the lane-L2 single-slot startup-constructor ELF checkers (slots 307..318).

Offline and read-only: bytes of the pinned private ELF are read through slot_pin_util.Elf (which wraps
ps4_dynlink.Image); nothing is loaded, relocated or executed. A per-slot checker supplies a `Spec`:

  * the constructor-table word, the exact body extent and sha256, the padding byte after the body;
  * `pins`: decoded operands of every call / jmp / rip-relative operand / immediate in the body
    ("call" site target, "jmp" site target, "rip" site length disp_off target, "bytes" site hex);
  * `callees`: shared callee bodies by sha256 (hash stays active during the mutation run);
  * `elf_extra(e)`: slot specific checks (strings, vtable relocations, class slot bodies);
  * native tokens: header constants, header tokens, ordered source tokens, test and CMake tokens.

Modes: default = full check + mutation self-test (first and last byte of every pin flipped, every
relocation addend perturbed, every native token and constant perturbed; all mutants must be rejected);
`--no-mutate` skips the self-test; `--tamper elf|native` runs the check on a deliberately tampered input
and therefore EXITS 1 (the tamper sanity: a checker that cannot fail proves nothing).
"""

from __future__ import annotations

import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import (ATEXIT, DSO, RELEASE, ROOT, TABLE_START, Elf, const_value,  # noqa: E402,F401
                           digest, require)


@dataclass
class Spec:
    name: str
    index: int
    base: int
    size: int
    sha: str
    pins: list
    native_files: dict[str, str]
    header_constants: dict[str, int]
    header_tokens: list[str]
    source_order: list[str]
    test_tokens: list[str]
    cmake_tokens: list[str]
    callees: dict[str, tuple[int, int, str]] = field(default_factory=dict)
    elf_extra: Callable[[Elf], None] | None = None
    pad_byte: int | None = 0x90   # first byte after the body (nop padding)
    summary: str = ""


def run_pins(e: Elf, pins: list) -> None:
    for p in pins:
        kind, site = p[0], p[1]
        if kind == "call":
            require(e.call(site) == p[2], f"call at {site:#x} -> {p[2]:#x}")
        elif kind == "jmp":
            require(e.jmp(site) == p[2], f"jmp at {site:#x} -> {p[2]:#x}")
        elif kind == "rip":
            require(e.rip(site, p[2], p[3]) == p[4], f"rip operand at {site:#x} -> {p[4]:#x}")
        elif kind == "bytes":
            e.bytes_at(site, p[2], f"bytes {p[2]}")
        else:
            raise SystemExit(f"FAIL unknown pin kind {kind!r}")


def run_elf_checks(spec: Spec, e: Elf) -> None:
    require(e.u64(TABLE_START - 8 * spec.index) == spec.base, f"table word {spec.index}")
    recording, e.recording = e.recording, False   # the whole-body hash read is not an operand pin
    e.body_hash(spec.name, spec.base, spec.size, spec.sha)
    e.recording = recording
    for name, (rva, size, sha) in spec.callees.items():
        e.body_hash(name, rva, size, sha, always=True)
    if spec.pad_byte is not None:
        require(e.read(spec.base + spec.size, 1)[0] == spec.pad_byte, "padding byte after the body")
    run_pins(e, spec.pins)
    if spec.elf_extra is not None:
        spec.elf_extra(e)


def read_native(spec: Spec) -> dict[str, str]:
    return {key: (ROOT / path).read_text() for key, path in spec.native_files.items()}


def check_native(spec: Spec, t: dict[str, str]) -> None:
    for name, value in spec.header_constants.items():
        require(const_value(t["header"], name) == value, f"native {name}")
    for token in spec.header_tokens:
        require(token in t["header"], f"header token {token!r}")
    pos = [t["source"].index(token) if token in t["source"] else -1 for token in spec.source_order]
    require(-1 not in pos and pos == sorted(pos) and len(set(pos)) == len(pos),
            f"native slot {spec.index} call order")
    for token in spec.test_tokens:
        require(token in t["test"], f"test token {token!r}")
    for token in spec.cmake_tokens:
        require(token in t["cmake"], f"cmake token {token!r}")


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


def mutation_self_test(spec: Spec, e: Elf, texts: dict[str, str]) -> tuple[int, int]:
    pins = sorted(e.pins)
    e.skip_hash = True
    e.recording = False
    ran = survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                run_elf_checks(spec, e)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    e.overlay = {}
    for slot in sorted(e.reloc_pins):
        e.reloc_flip = {slot}
        ran += 1
        try:
            run_elf_checks(spec, e)
        except SystemExit:
            continue
        survivors += 1
        print(f"mutation survived: relocation addend at {slot:#x}", file=sys.stderr)
    e.reloc_flip = set()
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    # The whole-body hash catches changes outside the decoded semantic pins.
    e.overlay = {spec.base + 0x10: e.raw(spec.base + 0x10, 1)[0] ^ 0x80}
    expect_failure("body hash", lambda: run_elf_checks(spec, e))
    e.overlay = {}

    native_ran = 0
    token_groups = (("header", spec.header_tokens), ("source", spec.source_order),
                    ("test", spec.test_tokens), ("cmake", spec.cmake_tokens))
    for key, tokens in token_groups:
        for token in tokens:
            if token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(spec, m))
            native_ran += 1
    for name, value in spec.header_constants.items():
        pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(spec, m))
        native_ran += 1
    return ran, native_ran


def main(spec: Spec) -> int:
    args = sys.argv[1:]
    mutate = "--no-mutate" not in args
    e = Elf()
    texts = read_native(spec)
    if "--tamper" in args:
        mode = args[args.index("--tamper") + 1] if len(args) > args.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {spec.base + 0x10: e.raw(spec.base + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace(next(iter(spec.header_constants)), "kTampered", 1)
    e.recording = True
    run_elf_checks(spec, e)
    e.recording = False
    check_native(spec, texts)
    note = ""
    if mutate and "--tamper" not in args:
        elf_runs, native_runs = mutation_self_test(spec, e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_{spec.index}: body {spec.sha[:12]}... {spec.size} B hash/extent + "
          f"{len(spec.pins)} operand pins; {spec.summary}; guest bytes NOT executed{note}")
    return 0
