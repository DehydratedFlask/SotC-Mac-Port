#!/usr/bin/env python3
"""Tie the guest TLS model to the evidence it claims.

The model documents three facts: every FS-segment access in real code is `%fs:0x0`, the thread
pointer is then dereferenced at 0x8/0x10/0x400, and the raw `%fs:` grep hits outside FDE ranges are
data bytes, not accesses. This checker re-derives all three from the pinned disassembly when it is
present, and always checks the module/CMake wiring.

Skips cleanly when the private inputs are absent (the same rule as the other private-input checkers).
"""

from __future__ import annotations

import importlib.util
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
HEADER = ROOT / "native/core/guest_tls.hpp"
SOURCE = ROOT / "native/core/guest_tls.cpp"
FIXTURE = ROOT / "native/tests/test_guest_tls.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"

LINE = re.compile(r"^\s*([0-9a-f]+):\s+([a-z][a-z0-9.]*)\s+(.*)$")
OBSERVED_OFFSETS = {0x8, 0x10, 0x400}


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_census():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = module
    spec.loader.exec_module(module)
    return module


def main() -> int:
    header = HEADER.read_text()
    source = SOURCE.read_text()
    fixture = FIXTURE.read_text()
    cmake = CMAKE.read_text()

    for needle in ("kBlockSize", "kMaxThreads", "thread_pointer", "no_thread",
                   "out_of_range", "already_exists", "too_many_threads"):
        require(needle in header, f"header missing {needle}")
    for needle in ("0x8", "0x10", "0x400", "%fs:0x0"):
        require(needle in header, f"header does not document the observed {needle}")
    require("kBlockSize = 0x800" in header, "block size changed without updating this checker")
    for needle in ("core/guest_tls.cpp", "tests/test_guest_tls.cpp", "test_guest_tls"):
        require(needle in cmake, f"cmake missing {needle}")
    for needle in ("two_threads_do_not_alias", "bounds_and_lifecycle", "observed_offsets"):
        require(needle in fixture, f"fixture missing {needle}")
    require("out_of_range" in source and "no_thread" in source, "source lacks the status paths")

    if not (DIS.is_file() and FDE.is_file()):
        print("SKIP check_guest_tls: private disassembly not present (module wiring checked)")
        return 0

    census = load_census()
    fdes, _duplicates = census.load_fde_ranges(FDE)

    fs_instructions = 0
    other_fs_offsets = set()
    pointer_offsets = set()
    rows = []
    for line in DIS.open(encoding="utf-8", errors="replace"):
        match = LINE.match(line)
        if match is None:
            continue
        address = int(match.group(1), 16)
        if fdes.find(address) is None:
            continue  # data bytes disassembled by -D are not code
        rows.append((address, match.group(3)))

    code_with_fs = [(address, ops) for address, ops in rows if "%fs:" in ops]
    for index, (address, ops) in enumerate(code_with_fs):
        fs_instructions += 1
        for offset in re.findall(r"%fs:(0x[0-9a-f]+)", ops):
            if int(offset, 16) != 0:
                other_fs_offsets.add(int(offset, 16))
        target = ops.split(",")[-1].strip()
        if not re.fullmatch(r"%r[a-z0-9]+", target):
            continue
        for later_address, later_ops in rows:
            if later_address <= address or later_address > address + 24:
                continue
            for offset in re.findall(rf"(0x[0-9a-f]+)\({re.escape(target)}\)", later_ops):
                pointer_offsets.add(int(offset, 16))

    require(fs_instructions > 0, "no FS-segment instruction found in FDE-covered code")
    require(not other_fs_offsets,
            f"real code uses FS offsets other than 0: {sorted(hex(o) for o in other_fs_offsets)}")
    require(pointer_offsets, "no thread-pointer dereference found after fs:0 loads")
    require(pointer_offsets <= OBSERVED_OFFSETS,
            f"undeclared thread-pointer offsets: {sorted(hex(o) for o in pointer_offsets - OBSERVED_OFFSETS)}")
    require(max(pointer_offsets) < 0x800, "an observed offset exceeds the modelled block size")

    print(f"OK check_guest_tls: {fs_instructions} fs:0 loads, thread-pointer offsets "
          f"{sorted(hex(o) for o in pointer_offsets)} inside the 0x800 block, no other FS offsets")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
