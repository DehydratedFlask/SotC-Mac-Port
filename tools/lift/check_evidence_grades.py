#!/usr/bin/env python3
"""Checker: the evidence-grade split of `native/core/vtable_resolved_entries.hpp`.

Every row in that table carries a trailing `// site-provable` or `// table-level` comment. This
checker re-derives each row's grade from the pinned ELF and, when the private disassembly is
present, from the site itself:

  * **table side** (always): the qword at `vtable + slot` equals the recorded target, exactly one
    type-8 dynamic relocation sits at that address with the same addend, the target is an FDE
    function start, and the target appears in `docs/entry_map.json` under this header.
  * **site side** (needs `local/validation/registry-analysis-agent/all.dis`): the *strict*
    nearest-definition predicate. The call register's nearest preceding write must be the
    RIP-relative load of the row's own table - `lea TABLE(%rip)` or `mov G(%rip)` - or a member
    load whose base is one of those two. Then `register_value + slot` must be the row's slot
    address and its qword must be the row's target. A row the forwarding pass
    (`tools/lift/store_to_load_forwarding.py`) proves also counts as site-provable.

The derived split must equal the table in `docs/INDIRECT_FRONTIER.md` ("Evidence grades"):
rows, distinct targets and distinct table bases for each grade. Counts drifting in either place
fails the check.

Skips the site side cleanly when the private disassembly is absent, like the other private-input
checkers.
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
ENTRY_MAP = ROOT / "docs/entry_map.json"
DOC = ROOT / "docs/INDIRECT_FRONTIER.md"
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
FORWARDING = ROOT / "tools/lift/store_to_load_forwarding.py"

SITE_PROVABLE = "site-provable"
TABLE_LEVEL = "table-level"

# The audit that produced the grades ran `indirect_frontier.scan` with its 32-instruction window and
# resolved the call register from that window alone. The strict predicate below uses the same bound
# so this checker re-derives the documented artifact rather than a wider one.
AUDIT_WINDOW = 32

GRADE_TABLE_RE = re.compile(
    r"\|\s*site-provable\s*\|\s*\*\*(\d+)\*\*\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|")
TABLE_LEVEL_RE = re.compile(
    r"\|\s*table-level only\s*\|\s*\*\*(\d+)\*\*\s*\|\s*(\d+)\s*\|\s*(\d+) of (\d+)\s*\|")
TOTAL_RE = re.compile(
    r"\|\s*\*\*total\*\*\s*\|\s*\*\*(\d+)\*\*\s*\|\s*\*\*(\d+)\*\*\s*\|\s*\*\*(\d+)\*\*\s*\|")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def split_of(rows):
    """(rows, targets, bases) for each grade, derived from the row list."""
    result = {}
    for grade in (SITE_PROVABLE, TABLE_LEVEL):
        selected = [row for row in rows if row["grade"] == grade]
        result[grade] = (len(selected), len({row["target"] for row in selected}),
                         len({row["vtable"] for row in selected}))
    result["total"] = (len(rows), len({row["target"] for row in rows}),
                       len({row["vtable"] for row in rows}))
    return result


def main() -> int:
    for path in (HEADER, DOC, ENTRY_MAP, FORWARDING):
        require(path.is_file(), f"missing {path}")
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256,
            "unexpected ELF SHA256; the tier facts are pinned to d0723afb...61d3")

    forwarding = load_module("store_to_load_forwarding", FORWARDING)
    rows = forwarding.parse_header_rows(HEADER.read_text())
    require(len(rows) == 91, f"expected 91 rows in the header, found {len(rows)}")
    require(len({row["site"] for row in rows}) == len(rows), "duplicate call site in the header")
    require(all(row["grade"] in forwarding.GRADES for row in rows),
            "every row must carry a `// site-provable` or `// table-level` comment")

    _segments, headers = dyn.program_headers(elf)
    image = dyn.Image(elf, headers)
    reloc_at = defaultdict(list)
    for row in dyn.parse(elf)["relocs"]:
        reloc_at[row["target"]].append(row)
    entry_map = json.loads(ENTRY_MAP.read_text())["entries"]

    fdes = None
    if FDE_JSON.is_file():
        census = load_module("cfg_census", ROOT / "tools/lift/cfg_census.py")
        fdes, _duplicates = census.load_fde_ranges(FDE_JSON)

    def read_u64(address: int):
        try:
            raw = image.read(address, 8)
        except Exception:
            return None
        return struct.unpack("<Q", raw)[0] if len(raw) == 8 else None

    # ---------------------------------------------------------------------------------------------
    # Table side: facts both grades share.
    # ---------------------------------------------------------------------------------------------
    for row in rows:
        address = row["vtable"] + row["slot"]
        value = read_u64(address)
        require(value == row["target"],
                f"slot qword at {address:#x} is {value and hex(value) or 'unreadable'}, "
                f"expected {row['target']:#x} (site {row['site']:#x})")
        type8 = [reloc for reloc in reloc_at.get(address, []) if reloc["type"] == 8]
        require(len(type8) == 1,
                f"expected exactly one type-8 relocation at {address:#x}, found {len(type8)}")
        require(type8[0]["addend"] == row["target"],
                f"relocation addend at {address:#x} is {type8[0]['addend']:#x}, "
                f"expected {row['target']:#x}")
        if fdes is not None:
            found = fdes.find(row["target"])
            require(found is not None and found[0] == row["target"],
                    f"target {row['target']:#x} is not an FDE function start")
        key = f"0x{row['target']:x}"
        require(key in entry_map, f"entry map is missing {key}")
        require(any(entry["header"] == "native/core/vtable_resolved_entries.hpp"
                    for entry in entry_map[key]),
                f"{key} entry-map record does not name this header")

    declared = split_of(rows)

    # ---------------------------------------------------------------------------------------------
    # Site side: re-derive every grade.
    # ---------------------------------------------------------------------------------------------
    if not DIS.is_file():
        document = DOC.read_text()
        require(GRADE_TABLE_RE.search(document) is not None,
                "docs/INDIRECT_FRONTIER.md has no site-provable row in its Evidence-grades table")
        match = GRADE_TABLE_RE.search(document)
        require(int(match.group(1)) == declared[SITE_PROVABLE][0],
                f"doc says {match.group(1)} site-provable rows, header says "
                f"{declared[SITE_PROVABLE][0]}")
        print("SKIP check_evidence_grades: private disassembly not present "
              f"(header split {declared[SITE_PROVABLE][0]} site-provable / "
              f"{declared[TABLE_LEVEL][0]} table-level is self-consistent)")
        return 0

    bodies = forwarding.parse_disassembly(DIS, fdes)

    def strict_predicate(row, body):
        """The audit's nearest-definition-only tie, restricted to this row.

        Returns the `(vtable, slot)` the site forces, or None. This never scans past the nearest
        definition of the call register, which is exactly the difference from the published
        tool's `rip_global_for` fallback.
        """
        index = next((i for i, insn in enumerate(body) if insn.address == row["site"]), None)
        if index is None:
            return None
        match = re.fullmatch(r"\*0x([0-9a-f]+)\((%r[a-z0-9]+)\)",
                             forwarding.strip_comment(body[index].operands))
        if match is None:
            return None
        slot = int(match.group(1), 16)

        def static_value(register, start, allow_object):
            """(kind, value) for a register defined by a RIP table load, else None."""
            definition = forwarding.nearest_definition(body, start, register, AUDIT_WINDOW)
            if definition is None:
                return None
            insn = body[definition]
            parts = forwarding.operands_of(insn)
            if len(parts) != 2:
                return None
            base = forwarding.mnemonic_base(insn.mnemonic)
            if forwarding.rip_operand(parts[0]) and (base == "lea" or
                                                     forwarding.is_mov_form(insn.mnemonic)):
                address = forwarding.comment_address(insn)
                if address is None:
                    return None
                if base == "lea":
                    return ("table", address)
                value = read_u64(address)
                if not value:
                    return None
                return ("object", value) if allow_object else ("table", value)
            if not allow_object:
                return None
            location = forwarding.memory_operand(parts[0])
            if location is None or location[2] or not forwarding.is_mov_form(insn.mnemonic):
                return None
            inner = static_value(location[1], definition, allow_object=False)
            if inner is None or inner[0] != "object":
                return None
            vtable = read_u64(inner[1] + location[0])
            return ("table", vtable) if vtable else None

        hit = static_value(match.group(2), index, allow_object=True)
        return None if hit is None else (hit[1], slot)

    def forwarding_proves(row, body):
        def reloc_at_address(address):
            return reloc_at.get(address, [])

        def fde_owner(address):
            found = fdes.find(address) if address else None
            return found if found and found[0] == address else None

        associated = forwarding.Associated(reloc_at_address, fde_owner, fdes.find)
        return forwarding.analyze_site(row, body, associated, read_u64).proven

    mismatches = []
    derived_rows = []
    for row in rows:
        owner = fdes.find(row["site"])
        body = bodies.get(owner[0] if owner else None, [])
        hit = strict_predicate(row, body)
        site_provable = (hit is not None and hit[0] + hit[1] == row["vtable"] + row["slot"]
                         and read_u64(hit[0] + hit[1]) == row["target"])
        if not site_provable:
            site_provable = forwarding_proves(row, body)
        grade = SITE_PROVABLE if site_provable else TABLE_LEVEL
        derived_rows.append({"site": row["site"], "vtable": row["vtable"], "slot": row["slot"],
                             "target": row["target"], "grade": grade})
        if grade != row["grade"]:
            mismatches.append((row["site"], row["grade"], grade, hit))

    for site, declared_grade, derived_grade, hit in mismatches:
        print(f"  site {site:#x}: header says {declared_grade}, the site says {derived_grade}"
              + ("" if hit is None else f" (strict tie {hit[0]:#x}+{hit[1]:#x})"))
    require(not mismatches, f"{len(mismatches)} row(s) carry the wrong evidence grade")

    derived = split_of(derived_rows)

    # Both grades are non-empty, and both really occur: a pass that re-labels everything is a bug.
    require(derived[SITE_PROVABLE][0] > 0 and derived[TABLE_LEVEL][0] > 0,
            "one of the two grades became empty; the split is not a split")

    # ---------------------------------------------------------------------------------------------
    # The documented split must equal the derived one.
    # ---------------------------------------------------------------------------------------------
    document = DOC.read_text()
    for regex, label in ((GRADE_TABLE_RE, "site-provable"), (TABLE_LEVEL_RE, "table-level only"),
                         (TOTAL_RE, "total")):
        match = regex.search(document)
        require(match is not None,
                f"docs/INDIRECT_FRONTIER.md Evidence-grades table has no {label} row")
        numbers = [int(group) for group in match.groups()]
        key = SITE_PROVABLE if label == "site-provable" else (
            TABLE_LEVEL if label == "table-level only" else "total")
        expected = list(derived[key])
        if label == "table-level only":
            expected.append(derived["total"][2])
        if numbers != expected:
            print(f"FAIL docs/INDIRECT_FRONTIER.md {label}: doc {numbers}, derived {expected}")
            return 1

    counts = Counter(row["grade"] for row in rows)
    print(f"OK check_evidence_grades: {counts[SITE_PROVABLE]} site-provable "
          f"({derived[SITE_PROVABLE][1]} targets, {derived[SITE_PROVABLE][2]} table bases) / "
          f"{counts[TABLE_LEVEL]} table-level "
          f"({derived[TABLE_LEVEL][1]} targets, {derived[TABLE_LEVEL][2]} of "
          f"{derived['total'][2]} table bases) / {len(rows)} rows across "
          f"{derived['total'][1]} targets; the doc matches")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
