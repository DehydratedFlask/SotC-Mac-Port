#!/usr/bin/env python3
"""ELF byte-oracle for the first-class RB erase 0x482a90 (1,201 B) and the clear body 0x482960.

Batch 26. The erase is `_Rb_tree_rebalance_for_erase` with explicit nil nodes (node +0/+8/+0x10
links, +0x18 colour, +0x19 nil; header +0 leftmost, +8 root, +0x10 rightmost). It is reached from
the tree destructor 0x47e0f0 through 0x482960, whose guard at 0x48298b compares the header's left
link with itself and therefore always takes the 0x483240 destroy path; the erase call at 0x482a28
is dead in this image. Both facts are pinned here.

Offline only: reads the pinned private ELF, never loads or executes an original RVA.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, require  # noqa: E402

BASE = 0x482A90
SIZE = 0x4B1
SHA = "c04855e76cf037e74c3e2e5ae5b2b130eb49eb292475a28d4332eaf5ed679703"
CLEAR = 0x482960
ATEXIT = 0xF8F390

IMM_PINS = [
    (BASE, 1, "55", "push rbp"),
    (0x482987, 4, "498b5d00", "rbx = header->left"),
    (0x48298B, 4, "493b5d00", "cmp rbx,[r13]: self-compare, always equal"),
    (0x48298F, 6, "0f84a6000000", "je 0x482a3b (taken)"),
    (0x482AA5, 4, "807f1900", "cmpb $0,0x19(node) — nil rejection"),
    (0x482C27, 4, "807f1801", "cmpb $1,0x18(node) — black check"),
    (0x482F41, 15, "90" * 15, "pad to 0x482f50"),
    (CLEAR, 1, "55", "clear push rbp"),
]

CALL_PINS = [
    (0x482A28, BASE, "the only erase call site (inside the unreachable loop)"),
    (0x482A42, 0x483240, "taken path: destroy root"),
    (0x482F08, 0x170B0, "operator delete(node)"),
    (0x482F3A, 0xF8FC20, "invalid map/set<T> iterator abort"),
]

CONSTS = {
    "kRbEraseRva": 0x482A90,
    "kRbEraseSize": 0x4B1,
    "kRbClearRva": 0x482960,
    "kRbDestroyRva": 0x483240,
    "kRbAssertTextRva": 0xFFB153,
    "kRbTreeDtorA": 0x47E0F0,
    "kRbTreeDtorB": 0x482F50,
}

TOKENS = ["invalid map/set<T> iterator", "0x482a28", "INFERENCE", "rb_erase_all_loop",
          "kRbRed", "kRbBlack", "leftmost", "rightmost"]


def native_ok(header: str) -> None:
    for name, value in CONSTS.items():
        match = re.search(rf"\b{name}\s*=\s*(0x[0-9A-Fa-f]+|\d+)", header)
        require(match is not None, f"native constant {name} missing")
        require(int(match.group(1), 0) == value, f"native constant {name} == {value:#x}")
    for token in TOKENS:
        require(token in header, f"native token {token!r}")


def run_elf(elf: Elf, skip_hash: bool = False) -> None:
    if not skip_hash:
        elf.body_hash("rb_erase_482a90", BASE, SIZE, SHA)
    for addr, size, hexbytes, label in IMM_PINS:
        elf.bytes_at(addr, hexbytes, label)
        require(len(elf.raw(addr, size)) == size, f"{label}: extent")
    for site, target, label in CALL_PINS:
        require(elf.call(site) == target, f"{label}: call at {site:#x} -> {target:#x}")
    elf.cstring(0xFFB153, b"invalid map/set<T> iterator")


def mutation_self_test(elf: Elf) -> int:
    ran = 0
    for addr, size, _, label in IMM_PINS:
        elf.overlay = {addr: elf.raw(addr, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf, skip_hash=True)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL mutation survived at {addr:#x} ({label})")
    for site, _, label in CALL_PINS:
        elf.overlay = {site: elf.raw(site, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf, skip_hash=True)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL mutation survived at {site:#x} ({label})")
    elf.overlay = {}
    return ran


def main() -> int:
    args = sys.argv[1:]
    elf = Elf()
    header = (ROOT / "native/core/rb_tree_482a90.hpp").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kRbEraseSize = 0x4b1;", "kRbEraseSize = 0x4b2;", 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        native_ok(mutated)   # must raise
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" in args:
        elf.overlay = {BASE + 0x20: elf.raw(BASE + 0x20, 1)[0] ^ 0x80}
        run_elf(elf)   # must raise
        print("tamper NOT detected", file=sys.stderr)
        return 0

    elf.recording = True
    run_elf(elf)
    elf.recording = False
    native_ok(header)
    if "--no-mutate" not in args:
        ran = mutation_self_test(elf)
        print(f"OK check_rb_tree_482a90: {SIZE} B hash + {len(IMM_PINS)} imm pins + "
              f"{len(CALL_PINS)} call pins; mutation self-test {ran} mutants rejected; guest bytes NOT executed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
