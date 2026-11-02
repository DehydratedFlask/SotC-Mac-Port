#!/usr/bin/env python3
"""Pin how startup slot 309 (`0xa505e0`) is reached, and decode the two save-data jump tables.

The open question in `CPU-01-SLOT309-SAVE-DISPATCH-JUMPTABLES-01` was whether some case of the
computed jumps at `0xa4869b` / `0xa4afba` reaches the slot-309 initializer `0xa505e0`.  The answer
turns out not to be a call-graph question at all:

  * `0xa505e0` has **no direct near call or jump anywhere in the image** (checked by a complete raw
    byte scan for `e8`/`e9` + disp32, so it does not depend on instruction decoding);
  * exactly **one** RELA row references it: a type-8 row at `0x2dd6738`, which is
    `0x2dd70e0 - 309 * 8` - entry 309 of the backwards-indexed startup constructor table.

So the only edge to slot 309 is data, which is why every call-graph trace (including the Ghidra one
that timed out) failed to find an order.  The two jump tables are still pinned here because the
task asked for them, and because they do reach `0xa4a600` / `0xa4a940`.

Skips cleanly without the private inputs.
"""

from __future__ import annotations

import hashlib
import importlib.util
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

SLOT309_RVA = 0xA505E0
SLOT309_TABLE_ROW = 0x2DD6738
# The original constructor table is 573 qwords at 0x2dd70e0, indexed BACKWARDS: entry i is the qword
# at `kCtorTableTop - 8*i` (see native/core/startup_ctors.hpp).  Entry 309 is therefore
# 0x2dd70e0 - 309*8 = 0x2dd6738.  An earlier draft of this checker invented a forward "base"
# 0x2dd5d90 = top - 618*8; that lands on entry 309 only because 618 == 2*309, and it mislabelled
# every neighbouring entry by six positions.  The top/index form below is the real model.
SLOT309_TABLE_TOP = 0x2DD70E0
SLOT309_TABLE_COUNT = 573
SLOT309_INDEX = 309

# Neighbouring startup entries, so the table model is pinned by more than the slot-309 row alone.
NEIGHBOURS = {306: 0xA633B0, 307: 0xA5B090, 308: 0xA58F10, 310: 0xA479D0, 311: 0xA46B70}
# The two ends of the 573-entry span.
TABLE_ENDS = {0: 0xF6CAD0, 572: 0x1A20}

# The two computed jumps and their PIC-relative tables.
JUMP_A = {"site": 0xA4869B, "load": 0xA4868D, "table": 0x12C3280, "count": 17, "bound": b"\x83\xf8\x10"}
JUMP_B = {"site": 0xA4AFBA, "load": 0xA4AFAC, "table": 0x12C3324, "count": 19, "bound": b"\x83\xf8\x12"}

# Which dispatch case owns which `0xa4a600` / `0xa4a940` call site.
CASE_CALLS = (
    ("A", 2, 0xA4890C, 0xA4A600),
    ("A", 14, 0xA4947B, 0xA4A940),
    ("B", 2, 0xA4B23A, 0xA4A600),
    ("B", 19, 0xA4BF6D, 0xA4A940),
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    if not (ELF.is_file() and DIS.is_file()):
        print("SKIP check_startup_slot309_table: private inputs not present")
        return 0

    elf = ELF.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "pinned ELF digest changed")
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)
    relocs = d.parse(elf)["relocs"]

    def as_int(value) -> int:
        return value if isinstance(value, int) else int(str(value), 16)

    # --- the table row is the only reference to slot 309 -------------------------------------------------
    type8 = [r for r in relocs if as_int(r["type"]) == 8]
    pointing = [r for r in type8 if as_int(r["addend"]) == SLOT309_RVA]
    require(len(pointing) == 1, f"expected exactly one type-8 row at 0x{SLOT309_RVA:x}, found {len(pointing)}")
    require(as_int(pointing[0]["target"]) == SLOT309_TABLE_ROW,
            f"slot 309 row moved to 0x{as_int(pointing[0]['target']):x}")

    any_ref = [r for r in relocs
               if as_int(r["addend"]) == SLOT309_RVA or as_int(r.get("symbol", 0)) == SLOT309_RVA]
    require(len(any_ref) == 1, f"a second relocation now references 0x{SLOT309_RVA:x}: {len(any_ref)}")

    # The table model is pinned by the row index, by five neighbours and by both ends of the span.
    require(SLOT309_TABLE_TOP - SLOT309_INDEX * 8 == SLOT309_TABLE_ROW,
            "slot 309 row/index arithmetic no longer holds")
    by_target = {}
    for row in type8:
        by_target[as_int(row["target"])] = as_int(row["addend"])
    missing = [i for i in range(SLOT309_TABLE_COUNT) if SLOT309_TABLE_TOP - i * 8 not in by_target]
    require(not missing, f"constructor table has {len(missing)} gaps, first {missing[:5]}")
    for index, expected in {**NEIGHBOURS, **TABLE_ENDS}.items():
        got = by_target.get(SLOT309_TABLE_TOP - index * 8)
        require(got == expected, f"startup entry {index} is 0x{got:x}, expected 0x{expected:x}")

    # --- no code edge, checked without trusting a disassembler -------------------------------------------
    # A near call/jmp to `0xa505e0` must be `e8`/`e9` followed by a disp32 that lands on it.  Scanning
    # every file-backed byte of every loadable segment finds all such encodings whether or not they sit
    # at an instruction boundary, so the negative result does not depend on a decoder.
    hits = []
    for seg in image.loads:
        base, length = seg["offset"], seg["filesz"]
        chunk = elf[base:base + length]
        for off, byte in enumerate(chunk[:-4]):
            if byte not in (0xE8, 0xE9):
                continue
            if seg["vaddr"] + off + 5 + struct.unpack_from("<i", chunk, off + 1)[0] == SLOT309_RVA:
                hits.append(seg["vaddr"] + off)
    require(not hits, f"a near call/jmp to 0x{SLOT309_RVA:x} appeared at {[hex(h) for h in hits]}")

    # The pinned disassembly must agree: the address appears only as its own entry.
    dis = DIS.read_text(errors="replace")
    require(dis.count("a505e0") == 1, f"disassembly mentions a505e0 {dis.count('a505e0')} times")
    require(not re.search(rf"callq\s+0x{SLOT309_RVA:x}\b", dis), "disassembly shows a call to slot 309")
    require(not re.search(rf"jmp\s+0x{SLOT309_RVA:x}\b", dis), "disassembly shows a jump to slot 309")
    body_starts = {int(m.group(1), 16) for m in re.finditer(r"^\s+([0-9a-f]+):", dis, re.MULTILINE)}

    # --- the two jump tables ----------------------------------------------------------------------------
    for name, spec in (("A", JUMP_A), ("B", JUMP_B)):
        table = image.read(spec["table"], spec["count"] * 4)
        require(table is not None and len(table) == spec["count"] * 4, f"table {name} unreadable")
        # The bound is loaded from the case index (`-1`) and compared before the jump.
        window = image.read(spec["site"] - 0x1B, 0x1B + 3)
        require(spec["bound"] in window, f"table {name} bound check {spec['bound'].hex()} changed")
        # The load is `movslq (%rcx,%rax,4), %rax; addq %rcx, %rax; jmpq *%rax`.
        load = image.read(spec["load"], 15)
        require(load is not None and load.endswith(bytes.fromhex("486304814801c8ff")),
                f"table {name} load sequence changed")
        targets = [spec["table"] + struct.unpack_from("<i", table, 4 * i)[0]
                   for i in range(spec["count"])]
        require(len(set(targets)) == spec["count"], f"table {name} has duplicate targets")
        for t in targets:
            require(t in body_starts, f"table {name} target 0x{t:x} is not an instruction boundary")

    # --- which case owns each dispatch call --------------------------------------------------------------
    tables = {}
    for name, spec in (("A", JUMP_A), ("B", JUMP_B)):
        raw = image.read(spec["table"], spec["count"] * 4)
        tables[name] = [spec["table"] + struct.unpack_from("<i", raw, 4 * i)[0]
                        for i in range(spec["count"])]
    for name, case, call, target in CASE_CALLS:
        cases = tables[name]
        frozen = image.read(call, 5)
        require(frozen is not None, f"call site 0x{call:x} unreadable")
        disp = struct.unpack("<i", frozen[1:])[0]
        require(frozen[0] == 0xE8 and call + 5 + disp == target,
                f"call at 0x{call:x} no longer targets 0x{target:x}")
        starts = [t for t in cases if t <= call]
        require(starts, f"call 0x{call:x} precedes every case of table {name}")
        # The case blocks are not laid out in case order, so the owner is the highest start address
        # below the call, not the last element of the filtered list.
        owner = max(starts)
        require(owner == cases[case - 1], f"call 0x{call:x} is owned by case {cases.index(owner) + 1}, not {case}")

    print(f"OK check_startup_slot309_table: slot 309 is a data-only edge - 0x{SLOT309_RVA:x} is row "
          f"{SLOT309_INDEX} of the backwards-indexed {SLOT309_TABLE_COUNT}-entry table at "
          f"0x{SLOT309_TABLE_TOP:x} (single type-8 RELA row at "
          f"0x{SLOT309_TABLE_ROW:x}), with no near call/jmp to it anywhere in the image; "
          f"tables A/B decode {JUMP_A['count']}/{JUMP_B['count']} entries, all on instruction "
          f"boundaries, and cases A2/A14/B2/B19 own the four 0xa4a600/0xa4a940 calls")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
