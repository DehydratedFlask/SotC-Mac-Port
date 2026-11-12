#!/usr/bin/env python3
"""ELF oracle for the native savedata family models.

Pins the five FDE bodies plus the four 0xa586a0 callees. Reused callees are proven, not assumed:
0x17090/0x170c0 are the allocator wrappers (jmp 0x12820/0x12af0) and 0xf8f470/0xf8f480 are the
memcpy/memmove PLT imports (GOT 0x1600028/0x1600030, JMPREL NIDs checked). The 0xa57d40 == 0x482a90
normalized-equivalence check remains.
"""
from __future__ import annotations

import argparse
import re
import shutil
import sys
import tempfile
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import trace_savedata_family as T  # noqa: E402
from slot_pin_util import Elf  # noqa: E402

PROJECT = Path(__file__).resolve().parents[2]
HEADER = PROJECT / "native" / "core" / "savedata_family.hpp"
CPP = PROJECT / "native" / "core" / "savedata_family.cpp"

DUPLICATE_A = 0xA57D40
DUPLICATE_B = 0x482A90
ASSERT_TEXT = b"invalid map/set<T> iterator"

EXPECTED = {
    0xA55050: {
        "instructions": 98, "direct": 0, "indirect": 0, "branches": 8, "stores": 1,
        "direct_histogram": {}, "branch_histogram": {"je": 8}, "rip": {0x32F88A0: 2},
    },
    0xA57C10: {
        "instructions": 81, "direct": 3, "indirect": 0, "branches": 12, "stores": 9,
        "direct_histogram": {0xA57D40: 1, 0xA58490: 1, 0xF8F3A0: 1},
        "branch_histogram": {"je": 6, "jmp": 3, "jne": 3}, "rip": {0x15FE518: 2},
    },
    0xA57D40: {
        "instructions": 340, "direct": 2, "indirect": 0, "branches": 77, "stores": 86,
        "direct_histogram": {0x170B0: 1, 0xF8FC20: 1},
        "branch_histogram": {"jne": 25, "je": 29, "jmp": 23}, "rip": {0x12C4741: 1},
    },
    0xA58490: {
        "instructions": 26, "direct": 2, "indirect": 0, "branches": 2, "stores": 0,
        "direct_histogram": {0xA58490: 1, 0x170B0: 1},
        "branch_histogram": {"jne": 1, "je": 1}, "rip": {},
    },
    0xA586A0: {
        "instructions": 108, "direct": 4, "indirect": 0, "branches": 7, "stores": 14,
        "direct_histogram": {0x17090: 1, 0x170C0: 1, 0xF8F470: 1, 0xF8F480: 1},
        "branch_histogram": {"jne": 2, "jb": 2, "jmp": 2, "je": 1}, "rip": {},
    },
    # Callee windows. 0x17090/0x170c0 are the reused wrapper models; the last two are the PLT
    # imports whose ISO C operations the grow model calls.
    0x17090: {
        "instructions": 4, "direct": 0, "indirect": 0, "branches": 1, "stores": 0,
        "direct_histogram": {}, "branch_histogram": {"jmp": 1}, "rip": {},
    },
    0x170C0: {
        "instructions": 2, "direct": 0, "indirect": 0, "branches": 1, "stores": 0,
        "direct_histogram": {}, "branch_histogram": {"jmp": 1}, "rip": {},
    },
    0xF8F470: {
        "instructions": 1, "direct": 0, "indirect": 0, "branches": 1, "stores": 0,
        "direct_histogram": {}, "branch_histogram": {"jmpq": 1}, "rip": {0x1600028: 1},
    },
    0xF8F480: {
        "instructions": 1, "direct": 0, "indirect": 0, "branches": 1, "stores": 0,
        "direct_histogram": {}, "branch_histogram": {"jmpq": 1}, "rip": {0x1600030: 1},
    },
}

CALLEES = {
    0x17090: {"size": 20, "sha256": "f26d9b99d1ad4395bc0dd389d33874229cce9e8ad4ec0dd359fabf714b97ad11"},
    0x170C0: {"size": 10, "sha256": "39cd562d64a27bc6e432aaf827156a2b3551b6d1ae1c44831c2a2f5feeace372"},
    0xF8F470: {"size": 6, "sha256": "bd719a31f451cadad5ba339791da6db9d6ead137e50a1a512755992099dadf75"},
    0xF8F480: {"size": 6, "sha256": "02101fcd50d9d2961c75acefd486bc3315a09ea38f7008eedef496da47a7cc51"},
}
T.BODIES.update(CALLEES)


def body_file_offset(elf: Path, rva: int) -> int:
    data = elf.read_bytes()
    for v, f, o in T._segments(data):
        if v <= rva < v + f:
            return o + rva - v
    raise SystemExit(f"0x{rva:x} is not mapped in {elf}")


def _normalized(ins: list[tuple[int, str]]) -> list[str]:
    return [re.sub(r"0x[0-9a-f]+", "IMM", text) for _, text in ins]


def _direct_histogram(ins: list[tuple[int, str]]) -> dict[int, int]:
    hist: Counter[int] = Counter()
    for _, text in ins:
        m = re.search(r"\bcallq?\s+(0x[0-9a-f]+)", text)
        if m:
            hist[int(m.group(1), 16)] += 1
    return dict(hist)


def check_duplicate(elf: Path, fails: list[str]) -> None:
    def req(cond: bool, msg: str) -> None:
        if not cond:
            fails.append(msg)

    a = T._objdump(DUPLICATE_A, T.BODIES[DUPLICATE_A]["size"], elf)
    b = T._objdump(DUPLICATE_B, 0x4B1, elf)
    req(len(a) == 340, f"0x{DUPLICATE_A:x} instructions {len(a)} != 340")
    req(len(b) == 340, f"0x{DUPLICATE_B:x} instructions {len(b)} != 340")
    if len(a) == len(b):
        diffs = [(i, x, y) for i, (x, y) in enumerate(zip(_normalized(a), _normalized(b))) if x != y]
        req(not diffs, f"0x{DUPLICATE_A:x} normalized body differs from 0x{DUPLICATE_B:x}: {diffs[:3]}")
    req(_direct_histogram(a) == _direct_histogram(b),
        f"call targets differ: {_direct_histogram(a)} vs {_direct_histogram(b)}")
    for rva in (0xFFB153, 0x12C4741):
        blob = T.body_bytes(rva, len(ASSERT_TEXT) + 1, elf)
        req(blob.startswith(ASSERT_TEXT + b"\x00"), f"assert text at 0x{rva:x} is {blob!r}")


def _rel32_jmp(elf: Path, rva: int, size: int) -> int:
    blob = T.body_bytes(rva, size, elf)
    off = blob.rfind(b"\xe9")
    if off < 0:
        raise SystemExit(f"no rel32 jmp in 0x{rva:x}")
    return rva + off + 5 + int.from_bytes(blob[off + 1:off + 5], "little", signed=True)


def check_call_sites(elf: Path, fails: list[str]) -> None:
    """Pin the instructions that establish the one deque base and the tree indirection."""
    def req(cond: bool, msg: str) -> None:
        if not cond:
            fails.append(msg)

    caller = dict(T._objdump(0xA54920, 0x730, elf))
    req(caller.get(0xA54FBC) == "leaq\t0x5e8(%rbx), %rdi",
        f"a54fbc base setup: {caller.get(0xA54FBC)!r}")
    req(caller.get(0xA54FCD, "").startswith("callq\t0xa586a0"),
        f"a54fcd grow call: {caller.get(0xA54FCD)!r}")
    req(caller.get(0xA54F78) == "movq\t0x618(%rbx), %rax",
        f"caller finish.cur load: {caller.get(0xA54F78)!r}")
    req(caller.get(0xA54F9B) == "movq\t0x630(%rbx), %r14",
        f"caller finish.node load: {caller.get(0xA54F9B)!r}")

    grow = dict(T._objdump(0xA586A0, 0x170, elf))
    req(grow.get(0xA586B1) == "movq\t%rdi, %r15",
        f"a586b1 does not retain the incoming base: {grow.get(0xA586B1)!r}")
    req(grow.get(0xA586BA) == "movq\t0x28(%r15), %r14",
        f"a586ba start.node load: {grow.get(0xA586BA)!r}")
    req(grow.get(0xA586BE) == "movq\t0x48(%r15), %rax",
        f"a586be finish.node load: {grow.get(0xA586BE)!r}")

    clear = dict(T._objdump(0xA57C10, 0x12F, elf))
    req(clear.get(0xA57C32) == "movq\t0x8(%r12), %r13",
        f"a57c32 header-pointer load: {clear.get(0xA57C32)!r}")
    req(clear.get(0xA57CEB) == "movq\t0x8(%r13), %rsi",
        f"a57ceb root load: {clear.get(0xA57CEB)!r}")
    req(clear.get(0xA57CF2, "").startswith("callq\t0xa58490"),
        f"a57cf2 release call: {clear.get(0xA57CF2)!r}")

    release = dict(T._objdump(0xA58490, 0x4C, elf))
    req(release.get(0xA5849A) == "movq\t%rsi, %rbx",
        f"a5849a node argument: {release.get(0xA5849A)!r}")
    req(release.get(0xA5849D) == "movq\t%rdi, %r14",
        f"a5849d self argument: {release.get(0xA5849D)!r}")


def check_callees(elf: Path, fails: list[str]) -> None:
    def req(cond: bool, msg: str) -> None:
        if not cond:
            fails.append(msg)

    req(_rel32_jmp(elf, 0x17090, 20) == 0x12820, "0x17090 does not jump to 0x12820")
    req(_rel32_jmp(elf, 0x170C0, 10) == 0x12AF0, "0x170c0 does not jump to 0x12af0")
    req(T.body_bytes(0xF8F470, 6, elf) == bytes.fromhex("ff25b20b6700"),
        "0xf8f470 is not `jmp qword [rip+0x670bb2]`")
    req(T.body_bytes(0xF8F480, 6, elf) == bytes.fromhex("ff25aa0b6700"),
        "0xf8f480 is not `jmp qword [rip+0x670baa]`")

    if elf.resolve() != T.ELF.resolve():
        return
    try:
        image = Elf()
        bp._bp_check_imports(image, (
            (0xF8F470, 0x1600028, "memcpy"),
            (0xF8F480, 0x1600030, "memmove"),
        ))
    except SystemExit as exc:
        fails.append(str(exc))


def check(elf: Path, header: Path) -> list[str]:
    fails: list[str] = []

    def req(cond: bool, msg: str) -> None:
        if not cond:
            fails.append(msg)

    for rva, meta in T.BODIES.items():
        r = T.trace(rva, elf)
        exp = EXPECTED[rva]
        req(r["sha256"] == meta["sha256"], f"0x{rva:x} body sha {r['sha256']} != {meta['sha256']}")
        req(r["rva"] == rva and r["size"] == meta["size"], f"0x{rva:x} rva/size drift")
        req(r["instructions"] == exp["instructions"], f"0x{rva:x} instructions {r['instructions']}")
        req(r["direct_calls"] == exp["direct"], f"0x{rva:x} direct calls {r['direct_calls']}")
        req(r["indirect_calls"] == exp["indirect"], f"0x{rva:x} indirect calls {r['indirect_calls']}")
        req(r["branches"] == exp["branches"], f"0x{rva:x} branches {r['branches']}")
        req(r["memory_stores"] == exp["stores"], f"0x{rva:x} stores {r['memory_stores']}")
        hist = {int(k, 16): v for k, v in r["direct_histogram"].items()}
        req(hist == exp["direct_histogram"], f"0x{rva:x} direct histogram {hist}")
        req(r["branch_histogram"] == exp["branch_histogram"],
            f"0x{rva:x} branch histogram {r['branch_histogram']}")
        rip = Counter(site["target"] for site in r["rip_sites"])
        req(dict(rip) == exp["rip"], f"0x{rva:x} RIP histogram {dict(rip)}")

    check_duplicate(elf, fails)
    check_call_sites(elf, fails)
    check_callees(elf, fails)

    text = header.read_text()
    tokens = [
        "kSavedataSlotA55050Rva = 0xA55050",
        "kSavedataSlotA55050Size = 347",
        "kSavedataTreeClearA57C10Rva = 0xA57C10",
        "kSavedataTreeClearA57C10Size = 303",
        "kSavedataTreeEraseA57D40Rva = 0xA57D40",
        "kSavedataTreeEraseA57D40Size = 1201",
        "kSavedataTreeReleaseA58490Rva = 0xA58490",
        "kSavedataTreeReleaseA58490Size = 76",
        "kSavedataRbEraseDuplicateA482A90Rva = 0x482A90",
        "kSavedataGrowA586A0Rva = 0xA586A0",
        "kSavedataGrowA586A0Size = 367",
        "kSavedataGrowAlloc17090Rva = 0x17090",
        "kSavedataGrowAlloc17090Size = 20",
        "kSavedataGrowFree170C0Rva = 0x170C0",
        "kSavedataGrowFree170C0Size = 10",
        "kSavedataGrowMemcpyF8F470Rva = 0xF8F470",
        "kSavedataGrowMemcpyF8F470Size = 6",
        "kSavedataGrowMemcpyF8F470GotRva = 0x1600028",
        "kSavedataGrowMemmoveF8F480Rva = 0xF8F480",
        "kSavedataGrowMemmoveF8F480Size = 6",
        "kSavedataGrowMemmoveF8F480GotRva = 0x1600030",
        "kSavedataBlockSize = 0x100",
        "kSavedataMapBaseOffset = 0x00",
        "kSavedataMapCapacityOffset = 0x08",
        "kSavedataMapStartCurOffset = 0x10",
        "kSavedataMapStartFirstOffset = 0x18",
        "kSavedataMapStartLastOffset = 0x20",
        "kSavedataMapStartNodeOffset = 0x28",
        "kSavedataMapFinishCurOffset = 0x30",
        "kSavedataMapFinishFirstOffset = 0x38",
        "kSavedataMapFinishLastOffset = 0x40",
        "kSavedataMapFinishNodeOffset = 0x48",
        "kSavedataSlotCursorRva = 0x32F88A0",
        "kSavedataSlotCount = 8",
        "kSavedataSlotStride = 0xB8",
        "kSavedataSlotBaseOffset = 0x28",
        "kSavedataSlotStatusOffset = 0x14",
        "kSavedataTreeHeaderOffset = 0x8",
        "kSavedataTreeFreeSlotOffset = 0x8",
        "kSavedataTreeSizeOffset = 0x10",
        "kSavedataTreeNodeFlagOffset = 0x19",
        "062b16fd7c32ca8f25c760e91c7f90e1855cd0ada958128706fee4e140a7456e",
        "6c03fcb0da5ce003b10b190f91c46e6c1e49e899c38d156920c3f64f9dfd029c",
        "4f54c1a69b06da222423c7f9ce724823be6698d3fa8acbb699e2ab611f1b3f2e",
        "7003816aa67ab301f6d65354bbc84182d442f44743ab3316123002631943c24b",
        "723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f",
        "f26d9b99d1ad4395bc0dd389d33874229cce9e8ad4ec0dd359fabf714b97ad11",
        "39cd562d64a27bc6e432aaf827156a2b3551b6d1ae1c44831c2a2f5feeace372",
        "bd719a31f451cadad5ba339791da6db9d6ead137e50a1a512755992099dadf75",
        "02101fcd50d9d2961c75acefd486bc3315a09ea38f7008eedef496da47a7cc51",
    ]
    for token in tokens:
        req(token in text, f"header token missing: {token}")
    req("SavedataTreeClearHooks" not in text, "stale release-hook struct still in header")
    req("SavedataGrowHooks" not in text, "stale grow-hook struct still in header")

    cpp = CPP.read_text()
    for token in ("kSavedataSlotStatusOffset", "kSavedataSlotCursorRva",
                  "savedata_tree_release_a58490", "savedata_tree_erase_a57d40",
                  "rb_erase_482a90", "free_170b0",
                  "SavedataDequeMap", "start_node", "finish_node", "start_first",
                  "finish_first", "savedata_grow_a586a0", "alloc_17090", "free_170c0",
                  "std::memcpy", "std::memmove", "kSavedataBlockSize"):
        req(token in cpp, f"model token missing: {token}")
    for stale in ("release_subtree", "hooks.grow", "hook_called"):
        req(stale not in cpp, f"stale seam still in model: {stale}")
    return fails


def run(elf: Path, header: Path) -> int:
    fails = check(elf, header)
    if fails:
        for f in fails:
            print(f"FAIL: {f}", file=sys.stderr)
        return 1
    print("OK savedata family: 9 windows pinned (5 bodies + 4 callees; 347/303/1201/76/367 B; "
          "98/81/340/26/108 insn; 0xa57d40 == 0x482a90 over 340 normalized instructions; "
          "0xa586a0 callees reused/proven: 0x17090->0x12820, 0x170c0->0x12af0, "
          "0xf8f470=memcpy, 0xf8f480=memmove)")
    return 0


def tamper_body(elf: Path, rva: int) -> Path:
    tmp = Path(tempfile.mkdtemp(prefix=f"{rva:x}-body-")) / "eboot.elf"
    shutil.copyfile(elf, tmp)
    data = bytearray(tmp.read_bytes())
    data[body_file_offset(elf, rva)] ^= 0x01
    tmp.write_bytes(bytes(data))
    return tmp


def tamper_header(header: Path, rva: int) -> Path:
    tmp = Path(tempfile.mkdtemp(prefix=f"{rva:x}-header-")) / "savedata_family.hpp"
    old = f"= 0x{rva:X};"
    new = f"= 0x{rva + 1:X};"
    text = header.read_text()
    if old not in text:
        raise SystemExit(f"no header pin for 0x{rva:x}")
    tmp.write_text(text.replace(old, new, 1))
    return tmp


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--elf", type=Path, default=T.ELF)
    ap.add_argument("--header", type=Path, default=HEADER)
    ap.add_argument("--self-test", action="store_true")
    ns = ap.parse_args(argv)

    if ns.self_test:
        clean = run(ns.elf, ns.header)
        if clean != 0:
            return clean
        for rva in T.BODIES:
            if run(tamper_body(ns.elf, rva), ns.header) == 0:
                print(f"FAIL: mutated body 0x{rva:x} was accepted", file=sys.stderr)
                return 1
            if run(ns.elf, tamper_header(ns.header, rva)) == 0:
                print(f"FAIL: mutated header 0x{rva:x} was accepted", file=sys.stderr)
                return 1
        print(f"OK tamper modes: all {len(T.BODIES)} body bytes and all {len(T.BODIES)} "
              "header RVAs rejected")
        return 0

    return run(ns.elf, ns.header)


if __name__ == "__main__":
    raise SystemExit(main())
