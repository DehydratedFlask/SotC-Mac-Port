#!/usr/bin/env python3
"""ELF byte-oracle for the functor-command helper pair 0x6af60 (397 B) and 0x6b0f0 (352 B).

Batch 27. Slot 462 calls 0x6af60 at 0x3e2c45 for the 0xd0-byte command at 0x2ef76a0. 0x6b0f0
builds the ConCmdBase, the +0x60 spec, the two bounded dwords, the 32-byte view copy and the
type-erased +0xa0 tail through five indirect calls; 0x6af60 adds vptr 0x1596980, copies [spec] into
+0x68 and registers through 0x20d60. The 397 B body ends with three nops before 0x6b0f0.

Offline only: reads the pinned private ELF, never executes an original RVA.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, require  # noqa: E402

OUTER = 0x6AF60
OUTER_SIZE = 397
OUTER_SHA = "2a30b19b2337a0c9084a414610ac8733192fe5180a86060258d5997dfbb8d878"
INNER = 0x6B0F0
INNER_SIZE = 352
INNER_SHA = "62da759b877c899c9bce25654ad40555981490251fb63d281b4d03cc72d3c9cb"

IMM_PINS = [
    (OUTER, 1, "55", "outer push rbp"),
    (0x6B0A9, 7, "488d05c0b85201", "lea 0x1596970 (outer vtable)"),
    (0x6B0B7, 3, "488903", "outer vptr store"),
    (0x6B0BA, 3, "418b06", "outer reads [spec]"),
    (0x6B0BD, 3, "894368", "outer +0x68 store"),
    (0x6B0ED, 3, "909090", "outer pad to 0x6b0f0"),
    (INNER, 1, "55", "inner push rbp"),
    (0x6B12B, 7, "488d05c6ad5201", "lea 0x1595ef8 (inner vtable)"),
    (0x6B136, 7, "488d0d13b85201", "lea 0x1596950 (functor tag)"),
    (0x6B144, 4, "4c896360", "inner +0x60 spec"),
    (0x6B14C, 4, "c6437001", "inner +0x70 = 1"),
    (0x6B164, 8, "c5fc118380000000", "inner zero +0x80..0x9f"),
    (0x6B16C, 7, "48898bc0000000", "inner +0xc0 functor tag"),
    (0x6B1B9, 3, "488900", "inner self pointer"),
    (0x6B1FD, 2, "ff10", "inner construct +0xa0"),
    (0x6B219, 7, "48898bc8000000", "inner +0xc8 = view tag"),
    (0x6B228, 3, "ff5018", "inner destroy temp"),
]

CALL_PINS = [
    (0x6B093, INNER, "outer -> 0x6b0f0"),
    (0x6B0C0, 0x20D60, "outer register"),
    (0x6B0E7, 0xF8F3A0, "outer stack check fail"),
    (0x6B126, 0x20AF0, "inner base ctor"),
    (0x6B24A, 0xF8F3A0, "inner stack check fail"),
]

CONSTS = {
    "kConCmdFunctorInnerRva": 0x6B0F0,
    "kConCmdFunctorInnerSize": 352,
    "kConCmdFunctorOuterRva": 0x6AF60,
    "kConCmdFunctorOuterSize": 397,
    "kConCmdFunctorInnerVptr": 0x1595F08,
    "kConCmdFunctorFinalVptr": 0x1596980,
    "kConCmdFunctorTagVptr": 0x1596950,
    "kConCmdFunctorSpecRva": 0x2EA8EE8,
    "kConCmdFunctorSpecWord": 0,
    "kConCmdFunctorRegisterRva": 0x20D60,
    "kConCmdFunctorCanaryCell": 0x15FE518,
    "kConCmdFunctorStackChkFailRva": 0xF8F3A0,
}

TOKENS = ["0x6af60", "0x6b0f0", "0x3e2c45", "INFERENCE", "0x1596980", "0x20d60"]


def native_ok(header: str) -> None:
    for name, value in CONSTS.items():
        match = re.search(rf"\b{name}\s*=\s*(0x[0-9A-Fa-f]+|\d+)", header)
        require(match is not None, f"native constant {name} missing")
        require(int(match.group(1), 0) == value, f"native constant {name} == {value:#x}")
    for token in TOKENS:
        require(token in header, f"native token {token!r}")


def run_elf(elf: Elf, skip_hash: bool = False) -> None:
    if not skip_hash:
        elf.body_hash("concmd_functor_outer", OUTER, OUTER_SIZE, OUTER_SHA)
        elf.body_hash("concmd_functor_inner", INNER, INNER_SIZE, INNER_SHA)
    for addr, size, hexbytes, label in IMM_PINS:
        elf.bytes_at(addr, hexbytes, label)
        require(len(elf.raw(addr, size)) == size, f"{label}: extent")
    for site, target, label in CALL_PINS:
        require(elf.call(site) == target, f"{label}: call at {site:#x} -> {target:#x}")


def mutation_self_test(elf: Elf) -> int:
    ran = 0
    for base, size in ((OUTER, OUTER_SIZE), (INNER, INNER_SIZE)):
        for offset in (0, 1, 0x20, size - 2):
            elf.overlay = {base + offset: elf.raw(base + offset, 1)[0] ^ 0x01}
            ran += 1
            try:
                run_elf(elf, skip_hash=False)
            except SystemExit:
                continue
            raise SystemExit(f"FAIL hash mutant survived at {base + offset:#x}")
    for addr, _, hexbytes, label in IMM_PINS:
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
    header = (ROOT / "native/core/concmd_functor_6af60.hpp").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kConCmdFunctorOuterSize = 397;", "kConCmdFunctorOuterSize = 398;", 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        native_ok(mutated)
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" in args:
        elf.overlay = {OUTER + 0x20: elf.raw(OUTER + 0x20, 1)[0] ^ 0x80}
        run_elf(elf)
        print("tamper NOT detected", file=sys.stderr)
        return 0

    elf.recording = True
    run_elf(elf)
    elf.recording = False
    native_ok(header)
    if "--no-mutate" not in args:
        ran = mutation_self_test(elf)
        print(f"OK check_concmd_functor_6af60: 2 body hashes + {len(IMM_PINS)} imm pins + "
              f"{len(CALL_PINS)} call pins; mutation self-test {ran} mutants rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
