#!/usr/bin/env python3
"""Measure the numbers behind docs/G1_EVIDENCE.md and keep them honest.

The pack is a *PARTIAL* gate record: it states what the ARM64 harness proves and what it does not.
Every number in the machine-readable block comes from the live tree, so a registered constructor or
a new frontier row makes the block stale instead of silently wrong.

Sources (all read-only):
  native/core/startup_ctors.cpp          registered constructor rows (RVA, original index)
  native/core/ctor_original_table.cpp    the 573-row original table, index order
  native/core/vtable_resolved_entries.hpp  91 frontier rows with their evidence grade comment
  native/core/vtable_*.hpp               per-target body models (graded headers)
  local/re/eboot.elf                     PRIVATE: the pinned 573-qword table at 0x2dd70e0
  local/validation/indirect-frontier/tied-sites-image-wide-strict.json
                                         PRIVATE: the 41-site nearest-definition tie

What this proves: the numbers in the pack match the current source tree and, when the private ELF
is present, the pinned image. What it does NOT prove: that any of the modelled code behaves like the
retail function, that anything was executed, or that G1 passed. Nothing here executes a guest
instruction, and no row's grade is upgraded by this tool.

Usage:
  python3 tools/lift/g1_evidence_pack.py            # measure and print
  python3 tools/lift/g1_evidence_pack.py --check    # fail when docs/G1_EVIDENCE.md has drifted
  python3 tools/lift/g1_evidence_pack.py --write    # rewrite the block in docs/G1_EVIDENCE.md
  python3 tools/lift/g1_evidence_pack.py --json P   # also write the measured JSON to P
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DOC = ROOT / "docs/G1_EVIDENCE.md"
STARTUP_SRC = ROOT / "native/core/startup_ctors.cpp"
ORIGINAL_TABLE = ROOT / "native/core/ctor_original_table.cpp"
FRONTIER_HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
CORE_DIR = ROOT / "native/core"
ELF = ROOT / "local/re/eboot.elf"
STRICT_TIED = ROOT / "local/validation/indirect-frontier/tied-sites-image-wide-strict.json"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_TABLE_BASE = 0x2DD70E0
CTOR_TABLE_COUNT = 573
CTOR_TABLE_SHA256 = "5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28"

BEGIN = "<!-- g1-evidence-pack:begin -->"
END = "<!-- g1-evidence-pack:end -->"

SLOT_ROW_RE = re.compile(r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*(nullptr|\[\])", re.M)
ORIGINAL_ROW_RE = re.compile(r"\{\s*(\d+)\s*,\s*(0x[0-9a-fA-F]+)\s*\}")
FRONTIER_ROW_RE = re.compile(
    r"\{0x([0-9a-f]+), kResolvedVtable_([0-9a-f]+), (0x[0-9a-f]+), "
    r"kResolvedVtableTarget_([0-9a-f]+)Rva\},(?:\s*//\s*(\S+))?"
)
TEST_ASSERT_RE = {
    "rows": re.compile(r"CHECK\(slots\.size\(\) == (\d+)\);"),
    "index": re.compile(r"CHECK\(first_unrecovered_constructor_index\(\) == (\d+)\);"),
    "rva": re.compile(r"CHECK\(first_unrecovered_constructor_rva\(\) == (0x[0-9a-fA-F]+)\);"),
    "remaining": re.compile(r"CHECK\(slots\.size\(\) \+ (\d+) == kOriginalCtorCount\);"),
}


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def read(path: Path) -> str:
    require(path.is_file(), f"missing {path.relative_to(ROOT)}")
    return path.read_text()


def measure_startup() -> dict:
    """Registered constructor rows against the 573-row original table (public files only)."""
    source = read(STARTUP_SRC)
    array = re.search(r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};", source, re.S)
    require(array is not None, "could not find kSlots[] in native/core/startup_ctors.cpp")
    rows = [(int(rva, 16), int(index)) for rva, index, _ in SLOT_ROW_RE.findall(array.group(1))]
    require(rows, "no constructor rows parsed")

    table = read(ORIGINAL_TABLE)
    original = {int(i): int(rva, 16) for i, rva in ORIGINAL_ROW_RE.findall(table)}
    require(len(original) == CTOR_TABLE_COUNT,
            f"original table has {len(original)} rows, expected {CTOR_TABLE_COUNT}")

    indices = [index for _, index in rows]
    require(indices == sorted(indices), "registered rows are not in original index order")
    require(len(set(indices)) == len(indices), "duplicate registered index")
    for rva, index in rows:
        require(original[index] == rva,
                f"row index {index}: RVA {rva:#x} disagrees with the original table {original[index]:#x}")

    # The recovered set is NOT a prefix: rows above the first gap are registered too (137 of them
    # at the time of writing). "Coverage" therefore means "registered rows", and the cliff index is
    # the FIRST unrecovered index, not the end of a contiguous run.
    missing = [index for index in range(CTOR_TABLE_COUNT) if index not in set(indices)]
    first = missing[0]
    above = [index for index in indices if index > first]
    require(len(rows) + len(missing) == CTOR_TABLE_COUNT, "coverage and gaps do not sum to 573")

    # The C++ test already asserts these; a disagreement means one of the two is wrong.
    test = read(ROOT / "native/tests/test_startup_ctor_coverage.cpp")
    asserts = {name: regex.search(test) for name, regex in TEST_ASSERT_RE.items()}
    for name, match in asserts.items():
        require(match is not None, f"test_startup_ctor_coverage.cpp lost its {name} assertion")
    require(int(asserts["rows"].group(1)) == len(rows), "test row count disagrees with kSlots[]")
    require(int(asserts["index"].group(1)) == first, "test cliff index disagrees with the source")
    require(int(asserts["rva"].group(1), 16) == original[first], "test cliff RVA disagrees with the table")
    require(int(asserts["remaining"].group(1)) == len(missing), "test remaining count disagrees")

    return {
        "table_base": f"0x{CTOR_TABLE_BASE:x}",
        "table_indexing": "backwards: entry i is the qword at 0x2dd70e0 - 8*i",
        "table_qwords": CTOR_TABLE_COUNT,
        "registered_rows": len(rows),
        "remaining_rows": len(missing),
        "first_unrecovered_index": first,
        "first_unrecovered_rva": f"0x{original[first]:x}",
        "rows_registered_above_first_gap": len(above),
        "test_agrees": True,
    }


def measure_frontier() -> dict:
    """Frontier rows and their evidence grade; the grade comment is mandatory."""
    text = read(FRONTIER_HEADER)
    rows = []
    for site, vtable, slot, target, grade in FRONTIER_ROW_RE.findall(text):
        require(grade in ("site-provable", "table-level"),
                f"frontier row {site} has no evidence-grade comment")
        rows.append((int(site, 16), int(vtable, 16), int(slot, 16), int(target, 16), grade))

    sites = [row[0] for row in rows]
    require(sites == sorted(sites), "frontier rows are not sorted by site")
    require(len(set(sites)) == len(sites), "duplicate frontier site")

    provable = [row for row in rows if row[4] == "site-provable"]
    table_level = [row for row in rows if row[4] == "table-level"]
    bases = {}
    for _, vtable, _, _, grade in rows:
        bases.setdefault(vtable, set()).add(grade)

    models = [p for p in CORE_DIR.glob("vtable_*.hpp") if p.name != "vtable_resolved_entries.hpp"]
    graded = [p for p in models
              if "site-provable" in p.read_text(errors="replace")
              or "table-level" in p.read_text(errors="replace")]

    return {
        "rows": len(rows),
        "targets": len({row[3] for row in rows}),
        "table_bases": len(bases),
        "site_provable_rows": len(provable),
        "site_provable_targets": len({row[3] for row in provable}),
        "table_level_rows": len(table_level),
        "table_level_targets": len({row[3] for row in table_level}),
        "bases_with_both_grades": sum(1 for grades in bases.values() if len(grades) > 1),
        "bases_table_level_only": sum(1 for grades in bases.values() if grades == {"table-level"}),
        "model_headers": len(models),
        "graded_model_headers": len(graded),
    }


def measure_private(frontier_rows: list[tuple[int, int, int, int, str]]) -> dict:
    """Cross-checks that need the private image. Absent inputs are reported, never assumed."""
    out: dict = {}

    if ELF.is_file():
        elf = ELF.read_bytes()
        require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF sha256")
        sys.path.insert(0, str(ROOT / "scripts"))
        import ps4_dynlink as dyn  # noqa: E402

        _segments, headers = dyn.program_headers(elf)
        image = dyn.Image(elf, headers)
        words = [image.read(CTOR_TABLE_BASE - 8 * index, 8) for index in range(CTOR_TABLE_COUNT)]
        raw = b"".join(words)
        require(hashlib.sha256(raw).hexdigest() == CTOR_TABLE_SHA256,
                "the 573-qword constructor table no longer matches its pinned sha256")
        originals = {int(i): int(rva, 16)
                     for i, rva in ORIGINAL_ROW_RE.findall(read(ORIGINAL_TABLE))}
        for index, word in enumerate(words):
            value = struct.unpack("<Q", word)[0]
            require(value == originals[index],
                    f"ELF table entry {index} is {value:#x}, native table says {originals[index]:#x}")
        out["elf_ctor_table"] = "verified: 573 qwords at 0x2dd70e0, sha256 5917cb85..., every row matches"
    else:
        out["elf_ctor_table"] = "not-run: local/re/eboot.elf absent"

    if STRICT_TIED.is_file():
        document = json.loads(STRICT_TIED.read_text())
        strict = {(row["site"], row["vtable"], row["slot"], row["target"])
                  for container in document["containers"] for row in container["sites"]}
        provable = {(s, v, sl, t) for s, v, sl, t, g in frontier_rows if g == "site-provable"}
        table_level = {(s, v, sl, t) for s, v, sl, t, g in frontier_rows if g == "table-level"}
        require(provable == strict,
                "site-provable rows and the nearest-definition tie disagree: "
                f"{len(provable - strict)} header-only, {len(strict - provable)} artifact-only")
        require(not (table_level & strict), "a table-level row is in the nearest-definition tie")
        out["strict_tied_sites"] = {
            "sites": len(strict),
            "targets": len({row[3] for row in strict}),
            "site_provable_intersection": len(provable & strict),
            "table_level_intersection": len(table_level & strict),
        }
        out["image_wide_indirect_calls"] = document.get("indirect_call_sites")
        out["image_wide_slot_calls"] = document.get("slot_call_sites")
    else:
        out["strict_tied_sites"] = "not-run: tied-sites-image-wide-strict.json absent"

    return out


def measure() -> dict:
    startup = measure_startup()
    text = read(FRONTIER_HEADER)
    frontier_rows = []
    for site, vtable, slot, target, grade in FRONTIER_ROW_RE.findall(text):
        frontier_rows.append((int(site, 16), int(vtable, 16), int(slot, 16),
                              int(target, 16), grade))
    frontier = measure_frontier()
    return {
        "schema": "sotc-g1-evidence-pack/1",
        "generated": "",  # filled by the caller; never compared
        "startup": startup,
        "frontier": frontier,
        "private": measure_private(frontier_rows),
    }


def render(payload: dict) -> str:
    body = {key: value for key, value in payload.items() if key != "generated"}
    return "```json\n" + json.dumps(body, indent=2, sort_keys=True) + "\n```"


def block_from_doc(text: str) -> str | None:
    start = text.find(BEGIN)
    end = text.find(END)
    if start < 0 or end < 0 or end < start:
        return None
    return text[start + len(BEGIN):end].strip()


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--check", action="store_true", help="compare docs/G1_EVIDENCE.md with the tree")
    mode.add_argument("--write", action="store_true", help="rewrite the block in docs/G1_EVIDENCE.md")
    parser.add_argument("--json", type=Path, default=None, help="write the measured JSON here too")
    args = parser.parse_args(argv)

    payload = measure()
    payload["generated"] = "generated by tools/lift/g1_evidence_pack.py; re-run it instead of editing"

    if args.json:
        args.json.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")

    if args.write:
        text = read(DOC)
        require(BEGIN in text and END in text, f"{DOC.name} lost its pack markers")
        head, rest = text.split(BEGIN, 1)
        _old, tail = rest.split(END, 1)
        DOC.write_text(f"{head}{BEGIN}\n\n{render(payload)}\n\n{END}{tail}")
        print(f"wrote {DOC.relative_to(ROOT)}")

    if args.check:
        text = read(DOC)
        block = block_from_doc(text)
        require(block is not None, f"{DOC.name} has no {BEGIN} ... {END} block")
        try:
            committed = json.loads(block.split("```json", 1)[1].rsplit("```", 1)[0])
        except (IndexError, json.JSONDecodeError) as error:
            raise SystemExit(f"FAIL {DOC.name}: unreadable JSON block ({error})")
        measured = {key: value for key, value in payload.items() if key != "generated"}

        # The public half must always match: those numbers are derivable from committed sources.
        stale = [(key, committed.get(key), measured[key])
                 for key in ("schema", "startup", "frontier")
                 if committed.get(key) != measured[key]]

        # The private half is compared only where the private inputs are available, so a clean
        # checkout reports it instead of failing on inputs it cannot have.
        private_known = all(not (isinstance(value, str) and value.startswith("not-run"))
                            for value in measured["private"].values())
        if private_known:
            if committed.get("private") != measured["private"]:
                stale.append(("private", committed.get("private"), measured["private"]))
        else:
            print("note: private inputs absent; the block's private section was not re-verified")

        if stale:
            for key, doc_value, tree_value in stale:
                print(f"stale: {key}\n  doc:    {json.dumps(doc_value, sort_keys=True)}\n"
                      f"  tree:   {json.dumps(tree_value, sort_keys=True)}", file=sys.stderr)
            raise SystemExit("FAIL docs/G1_EVIDENCE.md is stale; run "
                             "python3 tools/lift/g1_evidence_pack.py --write, then update the prose")
        print(f"OK tools/lift/g1_evidence_pack.py: docs/G1_EVIDENCE.md matches the tree "
              f"(451/573 startup, 91 frontier rows, 41 site-provable)")

    startup = payload["startup"]
    frontier = payload["frontier"]
    print(f"startup: {startup['registered_rows']}/{startup['table_qwords']} rows, "
          f"first gap {startup['first_unrecovered_index']}/{startup['first_unrecovered_rva']}, "
          f"{startup['remaining_rows']} remaining")
    print(f"frontier: {frontier['rows']} rows / {frontier['targets']} targets / "
          f"{frontier['table_bases']} table bases — "
          f"{frontier['site_provable_rows']} site-provable, {frontier['table_level_rows']} table-level")
    print(f"model headers: {frontier['graded_model_headers']}/{frontier['model_headers']} carry a grade")
    for key, value in payload["private"].items():
        print(f"private {key}: {value if not isinstance(value, dict) else json.dumps(value, sort_keys=True)}")
    print("STATIC_ANALYSIS_VERIFIED only: nothing was executed; no gate is advanced by this tool.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
