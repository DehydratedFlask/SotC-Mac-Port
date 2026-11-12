#!/usr/bin/env python3
"""Pin the E1095 vptr-skew correction of the indirect-call frontier table.

Nine `vtable_resolved_entries` rows were found by the store-to-load pass to read
`TABLE + 0x10 + slot`, not the recorded `TABLE + slot`. Each site is
`movq (%X),%rax; call *disp(%rax)` where the nearest store of `(%X)` is
`lea TABLE(%rip),%R; add $0x10,%R; movq %R,(%X)`. The Itanium ABI address point is
`TABLE + 0x10`, so the call reads `TABLE + 0x10 + disp`.

This checker re-derives, from the pinned ELF and (when present) the private disassembly, every
field of `kCorrectedVptrReads` in `native/core/vtable_resolved_entries.hpp`, then checks the table
against it:

  * layer 1 (image + code): the old qword is a real slot, the read address is `old_vtable + 0x10 +
    old_slot`, its qword is the real target, each address carries exactly one type-8 RELA with that
    addend, the real target is an FDE start, and the disassembly shows the call, the vptr store and
    the `lea TABLE; add $0x10` shape;
  * layer 2 (table): a kept row sits at the read address with the real target and carries no old
    pair; a withdrawn site has no row; no table row names a withdrawn target;
  * layer 3 (pin): the header's audit array equals the nine tuples pinned below.

It then runs mutation probes: for every one of the nine rows, each probe breaks one fact and the
matching layer must reject it. A probe that is accepted fails the checker.

What this does NOT prove: that nothing overwrites `(%X)` between the store and the call. That is a
hand argument over callee write-sets (recorded in the checkpoint), which is why the corrected row
stays `table-level` and no row is promoted.

Usage:
  python3 -I tools/lift/check_frontier_vptr_correction.py
"""

from __future__ import annotations

import copy
import hashlib
import importlib.util
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
ENTRY_MAP = ROOT / "docs/entry_map.json"
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
FORWARDING = ROOT / "tools/lift/store_to_load_forwarding.py"
TABLE_HEADER_NAME = "native/core/vtable_resolved_entries.hpp"

# (site, old_vtable, old_slot, old_target, read_address, real_target, row_kept)
PINNED = [
    (0x113c5, 0x1595b60, 0x18, 0x488d0, 0x1595b88, 0x13d20, True),
    (0x113d6, 0x1595b60, 0x50, 0x13d60, 0x1595bc0, 0x47860, False),
    (0x11462, 0x1594ae8, 0x50, 0x13d60, 0x1594b48, 0x47860, False),
    (0x312ce, 0x15953f8, 0xa8, 0x318b0, 0x15954b0, 0x31900, False),
    (0x312de, 0x15953f8, 0xa0, 0x31870, 0x15954a8, 0x318f0, False),
    (0x312f0, 0x15953f8, 0x18, 0x31310, 0x1595420, 0x31520, False),
    (0x3133e, 0x15953f8, 0xa8, 0x318b0, 0x15954b0, 0x31900, False),
    (0x3134e, 0x15953f8, 0xa0, 0x31870, 0x15954a8, 0x318f0, False),
    (0x31360, 0x15953f8, 0x18, 0x31310, 0x1595420, 0x31520, False),
]
WITHDRAWN_TARGETS = (0x13d60, 0x488d0, 0x318b0, 0x31870, 0x31310)
FIELDS = ("site", "old_vtable", "old_slot", "old_target", "read_address", "real_target", "row_kept")

AUDIT_RE = re.compile(
    r"\{0x([0-9a-f]+), 0x([0-9a-f]+), 0x([0-9a-f]+), 0x([0-9a-f]+), 0x([0-9a-f]+), "
    r"0x([0-9a-f]+), (true|false)\}")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def parse_audit(text: str):
    """Rows of `kCorrectedVptrReads` as dicts, in file order."""
    start = text.index("kCorrectedVptrReads[] = {")
    end = text.index("};", start)
    rows = []
    for match in AUDIT_RE.finditer(text[start:end]):
        values = [int(group, 16) for group in match.groups()[:6]] + [match.group(7) == "true"]
        rows.append(dict(zip(FIELDS, values)))
    return rows


class Context:
    def __init__(self):
        elf = (ROOT / "local/re/eboot.elf").read_bytes()
        if hashlib.sha256(elf).hexdigest() != ELF_SHA256:
            raise SystemExit("FAIL unexpected ELF SHA256")
        _, headers = dyn.program_headers(elf)
        self.image = dyn.Image(elf, headers)
        self.relocs = {}
        for reloc in dyn.parse(elf)["relocs"]:
            self.relocs.setdefault(reloc["target"], []).append(reloc)
        self.entry_map = json.loads(ENTRY_MAP.read_text())["entries"]
        self.forwarding = load_module("store_to_load_forwarding", FORWARDING)
        self.fdes = None
        self.bodies = None
        if FDE_JSON.is_file():
            census = load_module("cfg_census", ROOT / "tools/lift/cfg_census.py")
            self.fdes, _ = census.load_fde_ranges(FDE_JSON)
            if DIS.is_file():
                self.bodies = self.forwarding.parse_disassembly(DIS, self.fdes)

    def qword(self, address: int):
        try:
            raw = self.image.read(address, 8)
        except Exception:
            return None
        return struct.unpack("<Q", raw)[0] if len(raw) == 8 else None

    def type8(self, address: int):
        return [r for r in self.relocs.get(address, []) if r["type"] == 8]


# ---------------------------------------------------------------------------------------------
# Layer 1: image and code facts of one audit row
# ---------------------------------------------------------------------------------------------


def shape_failure(row, ctx: Context):
    """Why the disassembly does not show the skewed-vptr shape for this row, else None."""
    if ctx.bodies is None:
        return None
    fw = ctx.forwarding
    owner = ctx.fdes.find(row["site"])
    body = ctx.bodies.get(owner[0] if owner else None, [])
    index = next((i for i, insn in enumerate(body) if insn.address == row["site"]), None)
    if index is None:
        return f"site {row['site']:#x} is not an instruction of its FDE"
    call = re.fullmatch(r"\*0x([0-9a-f]+)\((%r[a-z0-9]+)\)", fw.strip_comment(body[index].operands))
    if call is None or not fw.mnemonic_base(body[index].mnemonic) == "call":
        return f"site {row['site']:#x} is not `call *disp(%reg)`"
    if int(call.group(1), 16) != row["old_slot"]:
        return f"site {row['site']:#x} displacement {call.group(1)} != slot {row['old_slot']:#x}"
    register = call.group(2)

    def nearest_def(reg, start):
        want = fw.family(reg)
        for i in range(start - 1, -1, -1):
            if want in fw.written_families(body[i]):
                return i
        return None

    load_at = nearest_def(register, index)
    if load_at is None:
        return "call register has no definition in the FDE"
    load = fw.operands_of(body[load_at])
    location = fw.memory_operand(load[0]) if len(load) == 2 else None
    if not fw.is_mov_form(body[load_at].mnemonic) or location is None or location[0] != 0 or location[2]:
        return f"call register is not reloaded from `(%X)` (found `{body[load_at].text}`)"
    base = location[1]
    for i in range(load_at - 1, -1, -1):
        parts = fw.operands_of(body[i])
        if not (fw.is_mov_form(body[i].mnemonic) and len(parts) == 2):
            continue
        destination = fw.memory_operand(parts[1])
        if destination is None or destination[0] != 0 or destination[2] or destination[1] != base:
            continue
        stored = parts[0]
        add_at = nearest_def(stored, i)
        if add_at is None:
            return f"vptr store at {body[i].address:#x} stores an undefined register"
        step = fw.operands_of(body[add_at])
        base_op = fw.mnemonic_base(body[add_at].mnemonic)
        if base_op == "add" and len(step) == 2 and step[0] == "$0x10" and step[1] == stored:
            source = stored                      # lea TABLE,%R; add $0x10,%R; mov %R,(%X)
        elif base_op == "lea" and len(step) == 2:
            inner = fw.memory_operand(step[0])   # lea TABLE,%A; lea 0x10(%A),%R; mov %R,(%X)
            if inner is None or inner[0] != 0x10 or inner[2]:
                return f"vptr store at {body[i].address:#x} does not store `reg + 0x10`"
            source = inner[1]
        else:
            return f"vptr store at {body[i].address:#x} does not store `reg + 0x10`"
        lea_at = nearest_def(source, add_at)
        if lea_at is None or fw.mnemonic_base(body[lea_at].mnemonic) != "lea" or \
                fw.comment_address(body[lea_at]) != row["old_vtable"]:
            return f"`lea` feeding the vptr store is not table {row['old_vtable']:#x}"
        return None
    return "no store to `(%X)` before the reload"


def layer1(rows, ctx: Context):
    failures = []
    for row in rows:
        tag = f"site {row['site']:#x}"
        if row["read_address"] != row["old_vtable"] + 0x10 + row["old_slot"]:
            failures.append(f"{tag}: read address {row['read_address']:#x} is not old_vtable + 0x10 "
                            f"+ old_slot")
        if row["real_target"] == row["old_target"]:
            failures.append(f"{tag}: the correction did not move the target")
        old_address = row["old_vtable"] + row["old_slot"]
        if ctx.qword(old_address) != row["old_target"]:
            failures.append(f"{tag}: image qword at {old_address:#x} is not old target "
                            f"{row['old_target']:#x}")
        if ctx.qword(row["read_address"]) != row["real_target"]:
            failures.append(f"{tag}: image qword at {row['read_address']:#x} is not real target "
                            f"{row['real_target']:#x}")
        for address, target in ((old_address, row["old_target"]),
                                (row["read_address"], row["real_target"])):
            rela = ctx.type8(address)
            if len(rela) != 1 or rela[0]["addend"] != target:
                failures.append(f"{tag}: {address:#x} lacks exactly one type-8 RELA with addend "
                                f"{target:#x}")
        if ctx.fdes is not None:
            found = ctx.fdes.find(row["real_target"])
            if found is None or found[0] != row["real_target"]:
                failures.append(f"{tag}: real target {row['real_target']:#x} is not an FDE start")
        shape = shape_failure(row, ctx)
        if shape is not None:
            failures.append(f"{tag}: {shape}")
    return failures


# ---------------------------------------------------------------------------------------------
# Layer 2: the table against the audit
# ---------------------------------------------------------------------------------------------


def layer2(rows, table, ctx: Context):
    failures = []
    by_site = {entry["site"]: entry for entry in table}
    targets = {entry["target"] for entry in table}
    for row in rows:
        tag = f"site {row['site']:#x}"
        entry = by_site.get(row["site"])
        if row["row_kept"]:
            if entry is None:
                failures.append(f"{tag}: kept row is missing from the table")
                continue
            if entry["vtable"] + entry["slot"] != row["read_address"] or \
                    entry["target"] != row["real_target"]:
                failures.append(f"{tag}: table row reads {entry['vtable'] + entry['slot']:#x} -> "
                                f"{entry['target']:#x}, expected {row['read_address']:#x} -> "
                                f"{row['real_target']:#x}")
            if entry["grade"] != "table-level":
                failures.append(f"{tag}: corrected row must stay table-level (found {entry['grade']})")
            owners = {e["header"] for e in ctx.entry_map.get(f"0x{row['real_target']:x}", [])}
            if TABLE_HEADER_NAME not in owners:
                failures.append(f"{tag}: entry map does not tie {row['real_target']:#x} to the table")
        else:
            if entry is not None:
                failures.append(f"{tag}: withdrawn row is still in the table")
            owners = {e["header"] for e in ctx.entry_map.get(f"0x{row['real_target']:x}", [])}
            if TABLE_HEADER_NAME in owners:
                failures.append(f"{tag}: unmodelled real target {row['real_target']:#x} leaked into "
                                f"the table's entry map")
    for target in WITHDRAWN_TARGETS:
        if target in targets:
            failures.append(f"withdrawn target {target:#x} is a table target again")
    return failures


# ---------------------------------------------------------------------------------------------
# Layer 3: the header equals the pinned tuples
# ---------------------------------------------------------------------------------------------


def layer3(rows):
    if [tuple(row[f] for f in FIELDS) for row in rows] != PINNED:
        return ["kCorrectedVptrReads differs from the nine pinned tuples in this checker"]
    return []


# ---------------------------------------------------------------------------------------------
# Mutation probes
# ---------------------------------------------------------------------------------------------


def probes(rows, table, ctx: Context):
    """[(label, accepted?)] - accepted?=True means a broken fact slipped through."""
    results = []

    def run(label, mutated_rows, mutated_table, layers):
        failures = []
        if 1 in layers:
            failures += layer1(mutated_rows, ctx)
        if 2 in layers:
            failures += layer2(mutated_rows, mutated_table, ctx)
        if 3 in layers:
            failures += layer3(mutated_rows)
        results.append((label, not failures))

    for position, row in enumerate(rows):
        site = f"{row['site']:#x}"

        def with_row(**changes):
            copied = copy.deepcopy(rows)
            copied[position].update(changes)
            return copied

        run(f"{site} undo the vptr skew (read = old_vtable + old_slot)",
            with_row(read_address=row["old_vtable"] + row["old_slot"]), table, (1,))
        run(f"{site} keep the old target as the real one",
            with_row(real_target=row["old_target"]), table, (1,))
        run(f"{site} claim the real target is the next function",
            with_row(real_target=row["real_target"] + 0x10), table, (1,))
        run(f"{site} shift the displacement by 8",
            with_row(old_slot=row["old_slot"] + 8), table, (1,))
        run(f"{site} point at a different table",
            with_row(old_vtable=row["old_vtable"] + 0x38), table, (1,))
        if ctx.bodies is not None:
            run(f"{site} move the site off its call",
                with_row(site=row["site"] + 1), table, (1,))
        run(f"{site} flip row_kept", with_row(row_kept=not row["row_kept"]), table, (2,))
        run(f"{site} edit one tuple field", with_row(old_target=row["old_target"] + 1), table, (3,))

        if row["row_kept"]:
            for mutation, entry_changes in (
                ("revert the table row to the old pair", {"slot": row["old_slot"],
                                                          "target": row["old_target"]}),
                ("keep the old slot, new target", {"slot": row["old_slot"]}),
                ("promote to site-provable", {"grade": "site-provable"}),
            ):
                mutated = copy.deepcopy(table)
                for entry in mutated:
                    if entry["site"] == row["site"]:
                        entry.update(entry_changes)
                run(f"{site} {mutation}", rows, mutated, (2,))
            run(f"{site} drop the kept row", rows,
                [e for e in table if e["site"] != row["site"]], (2,))
        else:
            mutated = copy.deepcopy(table)
            mutated.append({"site": row["site"], "vtable": row["old_vtable"], "slot": row["old_slot"],
                            "target": row["old_target"], "grade": "table-level"})
            run(f"{site} reinstate the withdrawn row with its old pair", rows, mutated, (2,))
            mutated = copy.deepcopy(table)
            mutated.append({"site": row["site"], "vtable": row["old_vtable"],
                            "slot": row["read_address"] - row["old_vtable"],
                            "target": row["real_target"], "grade": "table-level"})
            run(f"{site} reinstate the withdrawn row with the corrected pair", rows, mutated, (2,))
    return results


def main() -> int:
    text = HEADER.read_text()
    rows = parse_audit(text)
    if len(rows) != 9:
        print(f"FAIL expected 9 audit rows in the header, found {len(rows)}")
        return 1
    ctx = Context()
    table = ctx.forwarding.parse_header_rows(text)
    if len(table) != 83:
        print(f"FAIL expected 83 table rows, found {len(table)}")
        return 1

    failures = layer1(rows, ctx) + layer2(rows, table, ctx) + layer3(rows)
    for message in failures:
        print(f"  {message}")
    if failures:
        print(f"FAIL check_frontier_vptr_correction: {len(failures)} failure(s)")
        return 1

    results = probes(rows, table, ctx)
    accepted = [label for label, slipped in results if slipped]
    for label in accepted:
        print(f"  probe ACCEPTED a broken fact: {label}")
    if accepted:
        print(f"FAIL check_frontier_vptr_correction: {len(accepted)} of {len(results)} probes slipped")
        return 1

    mode = "ELF + disassembly" if ctx.bodies is not None else "ELF only (private disassembly absent)"
    print(f"OK check_frontier_vptr_correction: 9 audit rows ({mode}), 1 kept / 8 withdrawn, "
          f"{len(results)} mutation probes all rejected")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
