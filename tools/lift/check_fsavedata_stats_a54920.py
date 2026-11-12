#!/usr/bin/env python3
"""Byte-oracle for the native 0xa54920 model.

Pins the pinned-ELF body (size/sha/counts/calls/stores), the seven string literals, the header's
constants, and the one new functor-table admission. Tamper modes mutate a body byte and a header
RVA and require the oracle to reject both.
"""
from __future__ import annotations

import argparse
import shutil
import struct
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import trace_a54920 as T  # noqa: E402

PROJECT = Path(__file__).resolve().parents[2]
HEADER = PROJECT / "native" / "core" / "savedata_stats_a54920.hpp"
CPP = PROJECT / "native" / "core" / "savedata_stats_a54920.cpp"
CTOR_CPP = PROJECT / "native" / "core" / "ctor_slots_322_334.cpp"
CTOR_HPP = PROJECT / "native" / "core" / "ctor_slots_322_334.hpp"

EXPECTED_BRANCH = {"je": 10, "jmp": 1, "jl": 1, "jne": 1}
EXPECTED_RIP = {
    0x15FE518: 2,   # __stack_chk_guard cell, loaded at 0xa54938 and 0xa55023
    0x12C4360: 1,
    0x1594BF8: 1,
    0x15D1D80: 3,
    0x12C41FE: 3,
    0x12C4372: 1,
    0x12C4385: 1,
    0x12C412D: 1,
    0x12C4396: 1,
    0x12C43A3: 1,
    0x12C43B1: 1,
}
EXPECTED_STRINGS = {
    0x12C412D: b"Save System",
    0x12C41FE: b"",
    0x12C4360: b"save_%s_max_count",
    0x12C4372: b"save_%s_saves_used",
    0x12C4385: b"save_%s_mem_used",
    0x12C4396: b"%s Max Count",
    0x12C43A3: b"%s Saves Used",
    0x12C43B1: b"%s Save Mem Used (MB)",
}


def read_va(elf: Path, va: int, n: int = 64) -> bytes:
    data = elf.read_bytes()
    for v, f, o in T._segments(data):
        if v <= va < v + f:
            return data[o + va - v : o + va - v + n]
    raise SystemExit(f"0x{va:x} is not mapped in {elf}")


def body_file_offset(elf: Path, rva: int) -> int:
    data = elf.read_bytes()
    for v, f, o in T._segments(data):
        if v <= rva < v + f:
            return o + rva - v
    raise SystemExit(f"0x{rva:x} is not mapped in {elf}")


def check(elf: Path, header: Path) -> list[str]:
    fails: list[str] = []

    def req(cond: bool, msg: str) -> None:
        if not cond:
            fails.append(msg)

    r = T.trace(elf)
    req(r["sha256"] == T.SHA256, f"body sha {r['sha256']} != {T.SHA256}")
    req(r["rva"] == T.RVA and r["size"] == T.SIZE, "rva/size drift")
    req(r["instructions"] == 363, f"instructions {r['instructions']} != 363")
    req(r["direct_calls"] == 42, f"direct calls {r['direct_calls']} != 42")
    req(r["indirect_calls"] == 11, f"indirect calls {r['indirect_calls']} != 11")
    req(r["branches"] == 13, f"branches {r['branches']} != 13")
    req(r["memory_stores"] == 73, f"memory stores {r['memory_stores']} != 73")

    histogram = {int(k, 16): v for k, v in r["direct_histogram"].items()}
    req(histogram == T.EXPECTED_DIRECT,
        f"direct histogram drift: {histogram} != {T.EXPECTED_DIRECT}")
    req(r["branch_histogram"] == EXPECTED_BRANCH,
        f"branch histogram drift: {r['branch_histogram']}")

    rip = Counter(site["target"] for site in r["rip_sites"])
    req(dict(rip) == EXPECTED_RIP, f"RIP operand histogram drift: {dict(rip)}")
    req([s["text"] for s in r["indirect_sites"]] == [
        "callq\t*0x18(%rax)", "callq\t*0x8(%rax)", "callq\t*0x18(%rax)", "callq\t*0x8(%rax)",
        "callq\t*0x18(%rax)", "callq\t*0x8(%rax)", "callq\t*0x8(%rax)", "callq\t*(%rax)",
        "callq\t*(%rax)", "callq\t*(%rax)", "callq\t*0x18(%rax)"],
        "indirect-site sequence drift")

    for va, expected in EXPECTED_STRINGS.items():
        actual = read_va(elf, va, len(expected) + 1).split(b"\0")[0]
        req(actual == expected, f"string 0x{va:x}: {actual!r} != {expected!r}")

    text = header.read_text()
    tokens = [
        "kSavedataStatsA54920Rva = 0xA54920",
        "kSavedataStatsA54920Size = 1840",
        "kSavedataStatsCounterCount = 3",
        "kSavedataStatsConVarCount = 3",
        "kSavedataStatsCounterVptr = 0x1594C08",
        "kSavedataStatsArgFnTable = 0x15D1D80",
        "kSavedataStatsListSentinel = 0x100000001",
        "kSavedataStatsListType = 2",
        "kSavedataStatsDescriptorSize = 0xB8",
        "kSavedataStatsListChildSize = 0x30",
        "kSavedataStatsParentMapOffset = 0x5E8",
        'kSavedataStatsLabel[] = "Save System"',
        'kSavedataStatsFormatMaxCount[] = "save_%s_max_count"',
        'kSavedataStatsFormatSavesUsed[] = "save_%s_saves_used"',
        'kSavedataStatsFormatMemUsed[] = "save_%s_mem_used"',
        'kSavedataStatsConVarMaxCount[] = "%s Max Count"',
        'kSavedataStatsConVarSavesUsed[] = "%s Saves Used"',
        'kSavedataStatsConVarMemUsed[] = "%s Save Mem Used (MB)"',
        "27c76ceb08e4aac8d9e67b1dfcdeb87c8833fa8e73ae4f56de2481c475167810",
    ]
    for token in tokens:
        req(token in text, f"header token missing: {token}")

    cpp = CPP.read_text()
    for token in ("init_unbounded_int_var_worker_3a980", "kSavedataStatsArgFnTable",
                  "register_concmd_20d60", "init_debug_menu_item_26ea0",
                  "register_debug_menu_item_268b0", "kDebugMenuItemDtorRva",
                  "kSavedataStatsListSentinel", "kSavedataStatsListIndex",
                  "savedata_slot_alloc_a55050", "savedata_tree_clear_a57c10",
                  "savedata_grow_a586a0", "alloc_17090", "free_170b0"):
        req(token in cpp, f"model token missing: {token}")
    for stale in ("sibling_ctor", "list_node_dtor", "parent_vector_full"):
        req(stale not in cpp, f"stale seam still in model: {stale}")
    req("hooks.vector_grow" not in cpp, "stale vector_grow hook still in model")

    ctor = CTOR_CPP.read_text()
    req("table == kUnboundedIntFnTableSavedata" in ctor,
        "ctor_slots_322_334.cpp does not admit the 0x15d1d80 table")
    req("kUnboundedIntFnTableSavedata = 0x15d1d80" in CTOR_HPP.read_text(),
        "ctor_slots_322_334.hpp lacks the 0x15d1d80 pin")

    return fails


def run(elf: Path, header: Path) -> int:
    fails = check(elf, header)
    if fails:
        for f in fails:
            print(f"FAIL: {f}", file=sys.stderr)
        return 1
    print(f"OK 0xa54920: 1840 B sha {T.SHA256[:16]}…, 363 instructions, 42 direct / 11 indirect "
          f"calls, 13 branches, 73 stores; 7 literals and 10 RIP targets pinned")
    return 0


def tamper_body(elf: Path) -> Path:
    tmp = Path(tempfile.mkdtemp(prefix="a54920-body-")) / "eboot.elf"
    shutil.copyfile(elf, tmp)
    data = bytearray(tmp.read_bytes())
    data[body_file_offset(elf, T.RVA)] ^= 0x01
    tmp.write_bytes(bytes(data))
    return tmp


def tamper_header(header: Path) -> Path:
    tmp = Path(tempfile.mkdtemp(prefix="a54920-header-")) / "savedata_stats_a54920.hpp"
    tmp.write_text(header.read_text().replace("0xA54920", "0xA54921", 1))
    return tmp


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--elf", type=Path, default=T.ELF)
    ap.add_argument("--header", type=Path, default=HEADER)
    ap.add_argument("--tamper", choices=("body", "header", "none"), default="none")
    ap.add_argument("--self-test", action="store_true")
    ns = ap.parse_args(argv)

    if ns.self_test:
        clean = run(ns.elf, ns.header)
        if clean != 0:
            return clean
        body_tmp = tamper_body(ns.elf)
        if run(body_tmp, ns.header) == 0:
            print("FAIL: mutated body was accepted", file=sys.stderr)
            return 1
        header_tmp = tamper_header(ns.header)
        if run(ns.elf, header_tmp) == 0:
            print("FAIL: mutated header was accepted", file=sys.stderr)
            return 1
        print("OK tamper modes: body byte and header RVA both rejected")
        return 0

    if ns.tamper == "body":
        return run(tamper_body(ns.elf), ns.header)
    if ns.tamper == "header":
        return run(ns.elf, tamper_header(ns.header))
    return run(ns.elf, ns.header)


if __name__ == "__main__":
    raise SystemExit(main())
