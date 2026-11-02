#!/usr/bin/env python3
"""Shared ELF tie for the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster checkers.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3) are
read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Every check skips cleanly
(exit 0) when the private inputs are absent.

The four slots of the batch (361 / 467 / 379 / 536) are one byte-for-byte statement sequence with
per-slot data, so the pinning is the same work done four times. This module holds it once:

  * the constructor-table word (index i at 0x2dd70e0 - 8*i) must hold the body RVA;
  * the body digest over the FDE extent, the FDE record itself (its declared initial_location and
    address_range), and the eh_frame_hdr table entry that points at it;
  * every direct `callq` site and its target (so 0xc300 / 0x20d60 / 0xf590 / 0x27050 / 0x26ea0 /
    __cxa_atexit / __stack_chk_fail are pinned as a *set of sites*, not just as callees);
  * every rip-relative operand: (site, instruction length, displacement offset, target);
  * the literal instruction bytes that carry the shape (prologue, frame, canary store/compare, the
    `callq *0x18(%rax)` = ff 50 18, the epilogue, the single branch and its __stack_chk_fail target);
  * the `__cxa_atexit` rows in body order (site, destructor, object), with each destructor and object
    reachable as a rip operand inside its own step window;
  * the caller argument-functor tags: each tag's four .data.rel.ro slots resolve to the float bodies
    0xc4a0 / 0xc4c0 / 0xc4e0 / 0xc4f0 and the last is a bare `ret`;
  * the float class 0x15942b0's gate slots (0xbba0 `mov eax,1; ret`, 0xbbf0 `mov eax,3; ret`);
  * the strings of the block, byte for byte;
  * the native side: the header constants, the data rows, the thin adapter, the shared model's step
    order, the fixture tokens and the CMake stanzas.

A mutation self-test (`--no-mutate` skips it) flips the first and last byte of every recorded pin and
requires every mutant to be rejected. `--tamper elf|native` runs the check on a deliberately tampered
input and therefore exits 1: a checker that cannot fail proves nothing.
"""

from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, const_value, digest, require  # noqa: E402

TABLE_TOP = 0x2DD70E0
EH_FRAME_HDR = 0x150C08C
ATEXIT = 0xF8F390
STACK_CHK_FAIL = 0xF8F3A0
RELEASE = 0xF590
REGISTER = 0x20D60
FLOAT_DTOR = 0x20F0
ITEM_DTOR = 0x272D0
SLIDER = 0x27050
TOGGLE = 0x26EA0
FLOAT_CTOR = 0xC300
FLOAT_CLASS = 0x15942B0
FLOAT_CLASS_SLOT3 = (0x18, 0xBBA0)   # mov eax,1; ret
FLOAT_CLASS_SLOT6 = (0x30, 0xBBF0)   # mov eax,3; ret
FLOAT_FN_BODIES = [0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0]   # copy / clone / move / bare-ret destroy
MODEL_SOURCE = "native/core/debug_page_var_item.cpp"
MODEL_HEADER = "native/core/debug_page_var_item.hpp"

# The shared model's step order (each token must appear exactly once, in this order).
MODEL_ORDER = [
    "return Stage::var_ctor;",
    "if (!init_bounded_float_var_c300(",
    "const bool description_inert = release_inert_view_f590(description);",
    "result.functor_destroy_ok = true;",
    "var.value = row.value_bits;",
    "result.registration = register_concmd_20d60(",
    "const bool name_inert = release_inert_view_f590(name);",
    "result.atexit_recorded = concmd.atexit_log.record(kFloatVarDtorRva",
    "if (!init_debug_menu_item_27050(",
    "if (!init_debug_menu_item_26ea0(",
    "result.registration = register_debug_menu_item_268b0(",
    "const bool text_inert = release_inert_view_f590(text);",
    "const bool label_inert = release_inert_view_f590(label);",
    "result.atexit_recorded = concmd.atexit_log.record(kMenuItemDtorRva",
    "out.canary_checked = true;",
]

MODEL_TOKENS = [
    "inline constexpr std::uint64_t kBoundedFloatCtorRva = 0xc300;",
    "inline constexpr std::uint64_t kConCmdRegisterRva = 0x20d60;",
    "inline constexpr std::uint64_t kViewReleaseRva = 0xf590;",
    "inline constexpr std::uint64_t kFloatVarDtorRva = 0x20f0;",
    "inline constexpr std::uint64_t kSliderItemCtorRva = 0x27050;",
    "inline constexpr std::uint64_t kToggleItemCtorRva = 0x26ea0;",
    "inline constexpr std::uint64_t kMenuItemRegisterRva = 0x268b0;",
    "inline constexpr std::uint64_t kMenuItemDtorRva = 0x272d0;",
    "inline constexpr std::uint64_t kFloatClassVptr = 0x15942b0;",
    "inline constexpr std::uint64_t kFloatClassSlot3Rva = 0xbba0;",
    "inline constexpr std::uint64_t kFloatClassSlot6Rva = 0xbbf0;",
    "inline constexpr std::size_t kVarStride = 0xd8;",
    "inline constexpr std::size_t kItemStride = 0xa8;",
    "inline constexpr std::uint32_t kVarFlags = 1;",
    "inline constexpr std::uint32_t kItemFlags = 0x30;",
    "inline constexpr std::size_t kMaxScriptVars = 2;",
    "inline constexpr std::size_t kMaxScriptItems = 1;",
    # the only hook of the batch: the stack guard, and it is a host decision
    "const std::uint64_t* stack_guard = nullptr;",
    "const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;",
    "bool canary_checked = false;",
]


def fde_range(e: Elf, initial: int, fde_addr: int, record_length: int) -> int:
    """address_range of the FDE at `fde_addr`, plus a check of the eh_frame_hdr entry that points at it."""
    header = e.raw(EH_FRAME_HDR, 12)
    require(header[0] == 1 and header[1] == 0x1B and header[2] == 0x03 and header[3] == 0x3B,
            "eh_frame_hdr version/encodings")
    count = struct.unpack("<I", header[8:12])[0]
    table = e.img.read(EH_FRAME_HDR + 12, count * 8)
    found = None
    for i in range(count):
        loc, fde = struct.unpack_from("<ii", table, 8 * i)
        if EH_FRAME_HDR + loc == initial:
            found = EH_FRAME_HDR + fde
            break
    require(found is not None, f"no eh_frame_hdr entry for 0x{initial:x}")
    require(found == fde_addr, f"FDE record 0x{found:x} != pinned 0x{fde_addr:x}")
    record = e.read(fde_addr, 16)   # pinned: the FDE record
    require(struct.unpack("<I", record[0:4])[0] == record_length, "FDE record length")
    location = fde_addr + 8 + struct.unpack("<i", record[8:12])[0]
    require(location == initial, f"FDE initial_location 0x{location:x}")
    return struct.unpack("<I", record[12:16])[0]


def check_elf(e: Elf, spec: dict) -> None:
    index, rva, size = spec["index"], spec["rva"], spec["size"]
    table_word = TABLE_TOP - 8 * index
    require(table_word == spec["table_word"], "table row arithmetic")
    require(e.u64(table_word) == rva, f"constructor-table entry {index} -> 0x{rva:x}")

    body = e.read(rva, size)
    if not e.skip_hash:
        require(digest(body) == spec["sha"], "body sha256")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(fde_range(e, rva, spec["fde_addr"], spec["fde_reclen"]) == size, f"FDE address_range == {size}")

    # every direct call site and target
    require(len(spec["calls"]) == spec["direct_calls"], "pinned call-site count")
    require(len({site for site, _ in spec["calls"]}) == len(spec["calls"]), "distinct call sites")
    for site, target in spec["calls"]:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, target in spec["calls"]}) == spec["callees"], "distinct callees")
    require(sum(1 for _, target in spec["calls"] if target == ATEXIT) == len(spec["atexit"]),
            "atexit call-site count")

    # every rip-relative operand
    require(len(spec["rips"]) == spec["rip_operands"], "pinned rip-operand count")
    for site, length, disp_off, target in spec["rips"]:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, _, _, target in spec["rips"]}) == spec["rip_targets"], "distinct rip targets")

    # literal bytes of the shape
    for site, want, what in spec["bytes"]:
        e.bytes_at(site, want, what)

    # the single branch and its __stack_chk_fail target
    branch_site, branch_target = spec["branch"]
    branch = e.read(branch_site, 2)
    require(branch[0] == 0x75, f"the body's only branch is jne rel8 at 0x{branch_site:x}")
    require(branch_site + 2 + struct.unpack("<b", branch[1:2])[0] == branch_target,
            f"jne target 0x{branch_target:x}")
    require(e.call(branch_target) == STACK_CHK_FAIL, "the branch target calls __stack_chk_fail")

    # the functor sites: `callq *0x18(%rax)` at the pinned rvas
    for site in spec["functor_sites"]:
        e.bytes_at(site, "ff5018", f"callq *0x18(%rax) at 0x{site:x}")

    # __cxa_atexit rows in body order
    atexit_sites = [site for site, target in spec["calls"] if target == ATEXIT]
    require(atexit_sites == [site for site, _, _ in spec["atexit"]], "the atexit sites in body order")
    rip_at = {site: target for site, _, _, target in spec["rips"]}
    known_dtors = {dtor for _, dtor, _ in spec["atexit"]}
    first_object = {}
    for site, target in sorted(rip_at.items()):
        first_object.setdefault(target, site)
    previous = -1
    for site, dtor, obj in spec["atexit"]:
        where = first_object.get(obj)
        require(where is not None, f"object 0x{obj:x} is loaded by a rip operand")
        require(previous < where <= site, f"object 0x{obj:x} is loaded inside its own step window")
        previous = site
        dtor_sites = [s for s, t in rip_at.items() if t in known_dtors and s <= site]
        require(dtor_sites, f"step at 0x{site:x} loads a destructor constant before the call")
        require(rip_at[max(dtor_sites)] == dtor, f"step at 0x{site:x} registers dtor 0x{dtor:x}")

    # the caller argument-functor tags and the float class gate
    for tag in spec["functor_tags"]:
        for offset, body_rva in zip((0x0, 0x8, 0x10, 0x18), FLOAT_FN_BODIES):
            require(e.reloc_target(tag + offset) == body_rva,
                    f"arg tag 0x{tag:x} slot {offset:#x} -> 0x{body_rva:x}")
        e.bytes_at(FLOAT_FN_BODIES[3], "c3", "the destroy slot is a bare ret")
    for offset, body_rva, body_hex, what in (
            (*FLOAT_CLASS_SLOT3, "b801000000c3", "gate slot 3 returns 1"),
            (*FLOAT_CLASS_SLOT6, "b803000000c3", "gate slot 6 returns 3")):
        require(e.reloc_target(FLOAT_CLASS + offset) == body_rva,
                f"float class slot {offset:#x} -> 0x{body_rva:x}")
        e.bytes_at(body_rva, body_hex, what)

    # strings
    for addr, text in spec["strings"]:
        e.cstring(addr, text)


def check_native(texts: dict[str, str], spec: dict) -> None:
    header = texts["header"]
    for name, value in spec["constants"].items():
        require(const_value(header, name) == value, f"native constant {name}")
    for token in spec["header_tokens"]:
        require(token in header, f"header token {token!r}")
    positions = []
    for token in spec["source_order"]:
        require(token in texts["source"], f"source token {token!r}")
        positions.append(texts["source"].index(token))
    require(positions == sorted(positions) and len(set(positions)) == len(positions),
            "the adapter's statements are in body order")
    for token in spec["test_tokens"]:
        require(token in texts["test"], f"test token {token!r}")
    for token in spec["cmake_tokens"]:
        require(token in texts["cmake"], f"cmake token {token!r}")
    # The shared model: constants, the hook wiring, and the step order this shape demands.
    for token in MODEL_TOKENS:
        require(token in texts["model_header"], f"model header token {token!r}")
    positions = []
    for token in MODEL_ORDER:
        require(token in texts["model_source"], f"model source token {token!r}")
        positions.append(texts["model_source"].index(token))
    require(positions == sorted(positions) and len(set(positions)) == len(positions),
            "the shared model's steps are in the original's order")


def read_native(spec: dict) -> dict[str, str]:
    paths = {"header": spec["header"], "source": spec["source"], "test": spec["test"], "cmake": "native/CMakeLists.txt",
             "model_header": MODEL_HEADER, "model_source": MODEL_SOURCE}
    return {key: (ROOT / path).read_text() for key, path in paths.items()}


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutant {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutant survived: {label}")


def mutation_self_test(e: Elf, texts: dict[str, str], spec: dict) -> tuple[int, int]:
    e.recording = True
    check_elf(e, spec)
    e.recording = False
    pins = sorted(e.pins)
    e.skip_hash = True
    ran = survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                check_elf(e, spec)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    native_ran = 0
    for key, tokens in (("header", list(spec["constants"]) + spec["header_tokens"]), ("source", spec["source_order"]),
                        ("test", spec["test_tokens"]), ("cmake", spec["cmake_tokens"]),
                        ("model_header", MODEL_TOKENS), ("model_source", MODEL_ORDER)):
        for token in tokens:
            if token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(m, spec))
            native_ran += 1
    for name, value in spec["constants"].items():
        pattern = re.compile(rf"({re.escape(name)}\s*=\s*)(0x[0-9A-Fa-f]+|\d+)(u?\s*;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m, spec))
        native_ran += 1
    return ran, native_ran


def main(spec: dict, argv: list[str]) -> int:
    if not (ROOT / "local/re/eboot.elf").is_file():
        print(f"SKIP check_ctor_slot_{spec['index']}: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native(spec)
    if "--tamper" in argv:
        mode = argv[argv.index("--tamper") + 1] if argv.index("--tamper") + 1 < len(argv) else "elf"
        if mode == "elf":
            e.overlay = {spec["rva"] + 0x10: e.raw(spec["rva"] + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace(spec["first_constant"], "kTampered", 1)
    e.recording = True
    check_elf(e, spec)
    check_native(texts, spec)
    e.recording = False
    note = ""
    if "--no-mutate" not in argv and "--tamper" not in argv:
        elf_runs, native_runs = mutation_self_test(e, texts, spec)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_{spec['index']}: slot {spec['index']} (0x{spec['rva']:x}, {spec['size']} B, "
          f"sha {spec['sha'][:12]}..., FDE range {spec['size']}) is row {spec['index']} of the constructor "
          f"table at 0x{spec['table_word']:x}; {spec['direct_calls']} direct calls over {spec['callees']} "
          f"callees, {len(spec['atexit'])} __cxa_atexit rows, {len(spec['functor_sites'])} "
          f"`callq *0x18(%rax)` sites, {spec['rip_operands']} rip operands, {len(spec['strings'])} strings; "
          f"the batch model serves it with these data rows; guest bytes NOT executed{note}")
    return 0
