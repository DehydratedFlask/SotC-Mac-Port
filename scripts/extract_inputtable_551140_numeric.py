#!/usr/bin/env python3
"""Regenerate the publishable numeric-only payload for ctor 0x551140.

Reads the pinned private ELF at local/re/eboot.elf (never published). Decodes only the
immediate stores for entry fields a/b/c (u32 @ +0x10/+0x14/+0x18) and f50 (u64 @ +0x50)
from the straight-line region 0x55141d..0x557fe4 via system objdump. Name pointers and
callback/vtable pointers are intentionally omitted — those stay private.

Writes (allowlisted, no game strings):
  native/core/input_table_551140_numeric.inc.hpp

Exit non-zero on ELF / region / payload digest mismatch.
"""

from __future__ import annotations

import hashlib
import re
import struct
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
REGION_START = 0x55141D
REGION_END = 0x557FE4
REGION_SHA256 = "5ecf2186b5a7f35b384a8ec65b69ea2304ace415a19de9b1429212f78e9a3068"
TABLE_BASE = 0x2EFF8E0
COUNT = 275
STRIDE = 0x60
PAYLOAD_SHA256 = "61c85093797bb62b47f6299c320a3d05d7cf17577434f25360b5ba2c298b9dcb"
OUT = ROOT / "native/core/input_table_551140_numeric.inc.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def parse_imm(tok: str) -> int:
    s = tok[1:]  # drop leading '$'
    if s.startswith("-0x"):
        return -int(s[3:], 16)
    if s.startswith("0x"):
        return int(s, 16)
    return int(s, 16)


def decode_numeric_rows(elf_path: Path) -> list[tuple[int, int, int, int]]:
    out = subprocess.check_output(
        [
            "objdump",
            "-d",
            f"--start-address={REGION_START}",
            f"--stop-address={REGION_END}",
            str(elf_path),
        ],
        text=True,
        stderr=subprocess.STDOUT,
    )
    gpr: dict[str, int] = {}
    imm: dict[int, dict[int, int]] = defaultdict(dict)
    line_re = re.compile(
        r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+[0-9a-f]{2})\s+(\S+)\s*(.*)$"
    )
    for line in out.splitlines():
        m = line_re.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        nbytes = len(m.group(2).split())
        mn = m.group(3)
        rest = m.group(4).split("#")[0].strip()
        ops = [o.strip() for o in rest.split(",")] if rest else []
        if mn == "leaq" and len(ops) == 2:
            mo = re.match(r"(-?0x[0-9a-f]+)\(%rip\)", ops[0])
            if mo:
                disp = (
                    int(mo.group(1), 16)
                    if not mo.group(1).startswith("-")
                    else -int(mo.group(1)[1:], 16)
                )
                gpr[ops[1].lstrip("%")] = addr + nbytes + disp
            continue
        if mn not in ("movl", "movq") or len(ops) != 2:
            continue
        if not ops[0].startswith("$") or not ops[1].endswith("(%rax)"):
            continue
        if gpr.get("rax") != TABLE_BASE:
            continue
        off_s = ops[1].split("(")[0]
        off = int(off_s, 16) if off_s else 0
        sz = 4 if mn == "movl" else 8
        val = parse_imm(ops[0]) & ((1 << (8 * sz)) - 1)
        entry, eo = divmod(off, STRIDE)
        if eo in (0x10, 0x14, 0x18, 0x50):
            imm[entry][eo] = val
    rows: list[tuple[int, int, int, int]] = []
    for idx in range(COUNT):
        cell = imm.get(idx, {})
        require(0x10 in cell and 0x14 in cell and 0x18 in cell, f"missing a/b/c at entry {idx}")
        rows.append((cell[0x10], cell[0x14], cell[0x18], cell.get(0x50, 0)))
    return rows


def write_header(rows: list[tuple[int, int, int, int]], sha: str) -> None:
    lines = [
        "// GENERATED — do not edit by hand.",
        "// Regenerate: python3 -I scripts/extract_inputtable_551140_numeric.py",
        "// Numeric-only payload for ctor 0x551140 (275 x stride 0x60). No name strings,",
        "// no callback/vtable pointers. ELF region 0x55141d..0x557fe4 sha256",
        f"// {REGION_SHA256}.",
        f"// Payload sha256 {sha} (275 x {{u32 a,u32 b,u32 c,u64 f50}} LE).",
        "#pragma once",
        "#include <array>",
        "#include <cstdint>",
        "namespace sotc {",
        "namespace input_table_551140_detail {",
        "struct NumericRow {",
        "  std::uint32_t a;",
        "  std::uint32_t b;",
        "  std::uint32_t c;",
        "  std::uint64_t f50;",
        "};",
        f"inline constexpr std::size_t kCount = {COUNT};",
        f'inline constexpr char kPayloadSha256[] = "{sha}";',
        "inline constexpr std::array<NumericRow, kCount> kRows = {{",
    ]
    for a, b, c, f50 in rows:
        lines.append(f"  {{{a}u, {b}u, {c}u, {f50}ull}},")
    lines.extend(
        [
            "}};",
            "}  // namespace input_table_551140_detail",
            "}  // namespace sotc",
            "",
        ]
    )
    OUT.write_text("\n".join(lines))


def main() -> int:
    elf_path = ROOT / "local/re/eboot.elf"
    require(elf_path.is_file(), f"missing private ELF {elf_path}")
    elf = elf_path.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    region = img.read(REGION_START, REGION_END - REGION_START)
    require(len(region) == REGION_END - REGION_START, "region length")
    require(hashlib.sha256(region).hexdigest() == REGION_SHA256, "region sha256 changed")

    rows = decode_numeric_rows(elf_path)
    require(len(rows) == COUNT, f"expected {COUNT} rows, got {len(rows)}")
    for a, b, c, f50 in rows:
        require(0 <= a <= 5, f"a out of range {a}")
        require(0 <= b <= 3, f"b out of range {b}")
        require(0 <= c <= 3, f"c out of range {c}")
        require(0 <= f50 <= 15, f"f50 out of range {f50}")
    blob = b"".join(struct.pack("<IIIQ", *r) for r in rows)
    sha = hashlib.sha256(blob).hexdigest()
    require(sha == PAYLOAD_SHA256, f"payload sha256 changed: {sha}")

    write_header(rows, sha)
    print(f"OK wrote {OUT.relative_to(ROOT)} ({OUT.stat().st_size} B) sha256={sha}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
