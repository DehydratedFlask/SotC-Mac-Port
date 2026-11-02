#!/usr/bin/env python3
"""Independent oracle for native/platform/gnm_opcode_table.{hpp,cpp}.

The table is a claim about the pinned `eboot.elf`: "our own code writes these type-3 PM4 packet
headers into a command buffer". This script re-derives that claim from the ELF bytes and fails when
the table drifts.

Derivation (no disassembler, no game module, no network):

  * Walk the PT_LOAD program headers. Instruction bytes live in the executable segment.
  * A *writer site* is one of two instruction shapes whose immediate is a type-3 header:
      - `movl $imm32, disp(reg)`  (opcode C7 /0) -- a complete header stored into a buffer;
      - `orl  $imm32, reg`        (opcode 81 /1) -- the payload count patched into a header that a
        preceding `shll $0x10` already built, so the payload size is runtime data.
    The base register may not be rsp/rbp (those are stack slots, not a packet cursor) and the
    displacement must be a small, dword-aligned, non-negative buffer offset.
  * The immediate must be a well-formed type-3 header: bits[31:30] == 3, bits[7:0] == 0 (reserved),
    payload dwords = ((imm >> 16) & 0x3FFF) + 1 <= 64, and the top byte must equal
    0xC0 | (payload >> 8) -- i.e. the count field may not bleed into the type bits.
  * `0xC0000000` (type 3, opcode 0x00, empty payload) is deliberately NOT derived: it is a
    ubiquitous constant elsewhere in the image (274+ immediates, e.g. masks and flags) and no store
    of it exists in the emitter region, so opcode 0x00 stays unknown rather than guessed.

Each row of the C++ table records opcode, payload bounds, size kind, family, site count and the
lowest/highest writer RVA. The script rebuilds those from the bytes and requires an exact match.

Skips cleanly (exit 0 with a notice) when local/re/eboot.elf is absent.

Usage:
    python3 tools/lift/check_gnm_opcode_table.py                 # verify the checked-in table
    python3 tools/lift/check_gnm_opcode_table.py --emit          # print the derived rows
    python3 tools/lift/check_gnm_opcode_table.py --elf PATH      # non-default binary
"""

from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_ELF = ROOT / "local/re/eboot.elf"
TABLE_CPP = ROOT / "native/platform/gnm_opcode_table.cpp"
TABLE_HPP = ROOT / "native/platform/gnm_opcode_table.hpp"

PT_LOAD = 1
PF_X = 0x1

# A header whose payload is larger than this is not emitted by any site we model.
MAX_MODELLED_PAYLOAD = 0x40

# Type 3 / opcode 0 / empty payload: a generic "high bits set" constant elsewhere in the image.
AMBIGUOUS_IMM = 0xC0000000
# Opcode 0 is never derived: see the module docstring.
OPCODE_ZERO = 0x00

STACK_REGS = (4, 5)  # rsp, rbp


class DeriveError(RuntimeError):
    pass


def load_exec_segments(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if data[:4] != b"\x7fELF" or data[4] != 2 or data[5] != 1:
        raise DeriveError("not a 64-bit little-endian ELF")
    phoff = struct.unpack_from("<Q", data, 32)[0]
    phentsize = struct.unpack_from("<H", data, 54)[0]
    phnum = struct.unpack_from("<H", data, 56)[0]
    if phentsize != 56:
        raise DeriveError(f"unexpected e_phentsize {phentsize}")
    out = []
    for i in range(phnum):
        base = phoff + i * phentsize
        p_type, _flags, off, _vaddr, _paddr, filesz, _memsz, _align = struct.unpack_from(
            "<IIQQQQQQ", data, base
        )
        if p_type == PT_LOAD and (_flags & PF_X) and filesz:
            out.append(data[off:off + filesz])
    if not out:
        raise DeriveError("no executable PT_LOAD segment")
    return out


def header_fields(imm: int) -> tuple[int, int] | None:
    """Return (opcode, payload_dwords) for a type-3 header immediate, else None."""
    if imm >> 30 != 3:
        return None
    if imm & 0xFF:  # bits[7:0] are reserved in a command header
        return None
    payload = ((imm >> 16) & 0x3FFF) + 1
    if payload > MAX_MODELLED_PAYLOAD:
        return None
    if (imm >> 24) != (0xC0 | (payload >> 8)):
        return None
    return (imm >> 8) & 0xFF, payload


def scan_writers(seg: bytes, va_base: int):
    """Yield (rva, form, immediate, payload_dwords) for every writer site in `seg`.

    `rva` is the address of the instruction opcode byte. For the REX-prefixed forms the writer
    starts one byte earlier; the opcode byte is used so the address is unambiguous.
    """
    n = len(seg)
    i = 0
    while i < n - 6:
        op = seg[i]
        if op == 0xC7:
            modrm = seg[i + 1]
            mod = modrm >> 6
            reg = (modrm >> 3) & 7
            rm = modrm & 7
            if reg == 0 and mod != 3:
                if mod == 0 and rm not in (4, 5):
                    imm_at, disp, ok = i + 2, 0, True
                elif mod == 1 and rm != 4:
                    imm_at, disp, ok = i + 3, seg[i + 2], True
                    if disp & 0x80:
                        disp -= 0x100
                else:
                    imm_at, disp, ok = i + 6, None, False
                if ok and disp is not None and 0 <= disp < 0x80 and disp % 4 == 0:
                    if rm not in STACK_REGS:
                        imm = struct.unpack_from("<I", seg, imm_at)[0]
                        fields = header_fields(imm)
                        if fields and fields[0] != OPCODE_ZERO and imm != AMBIGUOUS_IMM:
                            yield (va_base + i, "store", imm, fields[1])
        elif op == 0x81:
            modrm = seg[i + 1]
            if modrm >> 6 == 3 and ((modrm >> 3) & 7) == 1:
                imm = struct.unpack_from("<I", seg, i + 2)[0]
                if imm != AMBIGUOUS_IMM:
                    fields = header_fields(imm)
                    if fields and fields[0] != OPCODE_ZERO:
                        yield (va_base + i, "or", imm, fields[1])
        i += 1


def derive(path: Path) -> dict[int, dict]:
    rows: dict[int, dict] = {}
    for seg in load_exec_segments(path):
        for rva, form, imm, payload in scan_writers(seg, 0):
            opcode, _ = header_fields(imm)
            row = rows.setdefault(
                opcode,
                {
                    "opcode": opcode,
                    "sites": [],
                    "payloads": set(),
                    "forms": set(),
                },
            )
            row["sites"].append(rva)
            row["payloads"].add(payload)
            row["forms"].add(form)
    for row in rows.values():
        row["sites"].sort()
    return rows


ROW_RE = re.compile(
    r"\{\s*0x([0-9A-Fa-f]{2})\s*,\s*\"([^\"]*)\"\s*,\s*Family::(\w+)\s*,\s*SizeKind::(\w+)\s*,"
    r"\s*(\d+)\s*,\s*(\d+)\s*,\s*0x([0-9A-Fa-f]+)ULL\s*,\s*(\d+)\s*,"
    r"\s*0x([0-9A-Fa-f]+)\s*,\s*0x([0-9A-Fa-f]+)\s*,"
)


def parse_table(path: Path) -> dict[int, dict]:
    text = path.read_text()
    rows: dict[int, dict] = {}
    for match in ROW_RE.finditer(text):
        (opcode, name, family, kind, min_payload, max_payload, mask, sites,
         first, last) = match.groups()
        op = int(opcode, 16)
        if op in rows:
            raise DeriveError(f"duplicate row for opcode 0x{op:02x} in {path.name}")
        rows[op] = {
            "opcode": op,
            "name": name,
            "family": family,
            "kind": kind,
            "min": int(min_payload),
            "max": int(max_payload),
            "mask": int(mask, 16),
            "sites": int(sites),
            "first": int(first, 16),
            "last": int(last, 16),
        }
    return rows


def compare(derived: dict[int, dict], table: dict[int, dict]) -> list[str]:
    """Every field the table claims, re-derived from the bytes. Empty list means agreement."""
    problems: list[str] = []

    missing = sorted(set(derived) - set(table))
    extra = sorted(set(table) - set(derived))
    for op in missing:
        problems.append(f"0x{op:02x}: derived from the ELF but absent from the table")
    for op in extra:
        problems.append(f"0x{op:02x}: in the table but no writer site derives it")

    for op in sorted(set(derived) & set(table)):
        d = derived[op]
        t = table[op]
        payloads = sorted(d["payloads"])
        if t["sites"] != len(d["sites"]):
            problems.append(
                f"0x{op:02x}: table says {t['sites']} writer sites, ELF has {len(d['sites'])}"
            )
        if t["first"] != d["sites"][0]:
            problems.append(
                f"0x{op:02x}: first site 0x{t['first']:06x} != derived 0x{d['sites'][0]:06x}"
            )
        if t["last"] != d["sites"][-1]:
            problems.append(
                f"0x{op:02x}: last site 0x{t['last']:06x} != derived 0x{d['sites'][-1]:06x}"
            )
        if t["min"] != payloads[0]:
            problems.append(f"0x{op:02x}: min payload {t['min']} != derived {payloads[0]}")
        if t["max"] != payloads[-1]:
            problems.append(f"0x{op:02x}: max payload {t['max']} != derived {payloads[-1]}")
        want_kind = (
            "unbounded" if "or" in d["forms"] else ("fixed" if len(payloads) == 1 else "closed")
        )
        if t["kind"].lower() != want_kind:
            problems.append(
                f"0x{op:02x}: size kind {t['kind']} != derived {want_kind} "
                f"(forms {sorted(d['forms'])}, payloads {payloads})"
            )
        want_mask = 0 if want_kind == "unbounded" else sum(1 << (p - 1) for p in payloads)
        if t["mask"] != want_mask:
            problems.append(
                f"0x{op:02x}: stored-payload mask 0x{t['mask']:x} != derived 0x{want_mask:x} "
                f"(payloads {payloads})"
            )
    return problems


def mutation_probe(derived: dict[int, dict], table: dict[int, dict]) -> list[str]:
    """Plant three drifts in copies of the table and require each one to be caught."""
    if not derived:
        return ["no rows derived; the probe cannot run"]
    op = sorted(derived)[0]

    dropped = {k: dict(v) for k, v in table.items() if k != op}

    moved = {k: dict(v) for k, v in table.items()}
    moved[op]["sites"] = moved[op]["sites"] + 1

    widened = {k: dict(v) for k, v in table.items()}
    widened[op]["mask"] = widened[op]["mask"] | (1 << 20)

    problems: list[str] = []
    for name, mutated in (("drop a row", dropped), ("move a site count", moved),
                          ("widen the stored-payload mask", widened)):
        if not compare(derived, mutated):
            problems.append(f"mutation probe '{name}' was not detected")
    return problems


def check(elf: Path, emit: bool) -> int:
    derived = derive(elf)
    if emit:
        for op in sorted(derived):
            row = derived[op]
            payloads = sorted(row["payloads"])
            print(
                f"opcode 0x{op:02x} sites={len(row['sites']):3d} "
                f"payload={payloads[0]}..{payloads[-1]} "
                f"distinct={payloads} forms={sorted(row['forms'])} "
                f"first=0x{row['sites'][0]:06x} last=0x{row['sites'][-1]:06x}"
            )
        print(f"total opcodes={len(derived)} sites={sum(len(r['sites']) for r in derived.values())}")
        return 0

    table = parse_table(TABLE_CPP)
    problems = compare(derived, table)

    if problems:
        for line in problems:
            print(f"FAIL {line}")
        print(f"FAIL check_gnm_opcode_table: {len(problems)} drift(s)")
        return 1

    problems = mutation_probe(derived, table)
    if problems:
        for line in problems:
            print(f"FAIL {line}")
        print("FAIL check_gnm_opcode_table: the comparison did not catch a planted drift")
        return 1

    total_sites = sum(len(r["sites"]) for r in derived.values())
    print(
        f"OK check_gnm_opcode_table: {len(derived)} opcodes / {total_sites} writer sites "
        f"re-derived from {elf.name} and matched by the table"
    )
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--emit", action="store_true", help="print derived rows instead of checking")
    args = parser.parse_args()

    if not args.elf.is_file():
        print(
            f"SKIP check_gnm_opcode_table: {args.elf} absent; "
            "the table can only be re-derived on a machine that owns the pinned game file"
        )
        return 0
    if not TABLE_CPP.is_file() or not TABLE_HPP.is_file():
        print("FAIL check_gnm_opcode_table: gnm_opcode_table sources missing")
        return 1
    try:
        return check(args.elf, args.emit)
    except DeriveError as error:
        print(f"FAIL check_gnm_opcode_table: {error}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
