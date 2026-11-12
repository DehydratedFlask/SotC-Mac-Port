#!/usr/bin/env python3
"""Re-derive the typed/callable entry map and indirect frontier from the pinned ELF.

Runs `entry_closure.build()` (ELF sha256 checked; `.eh_frame_hdr` parsed; every RVA typed; every
function start decoded; every closure indirect site resolved or blocked) and compares the result with
`docs/entry_closure.json` and `native/core/guest_entry_table.inc`.

`--mutate` additionally runs an in-memory mutation self-test: a tampered table row and a tampered
total must both be detected by the same predicate that checks the real artifacts. No file is changed.

Prints SKIP (exit 0) when the private ELF is absent, like the other private-image checkers.
"""

from __future__ import annotations

import hashlib
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))

import entry_closure as ec  # noqa: E402


def die(message: str) -> None:
    print(f"FAIL check_entry_closure: {message}", file=sys.stderr)
    raise SystemExit(1)


def artifacts_match(data: dict, inc: str, stored_json: str, stored_inc: str) -> bool:
    return stored_json == data and stored_inc == inc


def native_thunk_claims_ok(data: dict) -> bool:
    """Re-derive the thunk bodies from the pinned ELF and check the C++ registry rows."""
    block = data.get("native_thunks", {})
    thunks = block.get("thunks", [])
    if block.get("count") != 2 or len(thunks) != 2:
        return False
    source = (ROOT / "native/core/guest_thunks.cpp").read_text()
    elf = ec.Elf()
    for thunk in thunks:
        rva = int(thunk["rva"], 16)
        if f"0x{rva:x}ULL" not in source or thunk["name"] not in source:
            return False
        body = elf.raw(rva, thunk["size"])
        if hashlib.sha256(body).hexdigest() != thunk["sha256"]:
            return False
    return True


def main(argv: list[str]) -> int:
    if not (ROOT / "local/re/eboot.elf").exists():
        print("SKIP check_entry_closure: private ELF absent")
        return 0
    data, inc = ec.build()
    stored_json = json.loads(ec.OUT_JSON.read_text())
    stored_inc = ec.OUT_INC.read_text()
    if not artifacts_match(data, inc, stored_json, stored_inc):
        if stored_json != data:
            die("docs/entry_closure.json is stale; rerun tools/lift/entry_closure.py")
        die("native/core/guest_entry_table.inc is stale; rerun tools/lift/entry_closure.py")

    totals = data["totals"]
    if sum(totals[k] for k in ec.KIND_ORDER) != totals["entries"]:
        die("kind counts do not sum to the entry count")
    if totals["entries"] != json.loads(ec.ENTRY_MAP.read_text())["totals"]["distinct_rvas"]:
        die("typed entries disagree with docs/entry_map.json")
    callable_counts = data["callable"]["counts"]
    if sum(callable_counts.values()) != totals["entries"]:
        die("callable counts do not sum to the entry count")
    if (callable_counts["retail_entry"] + callable_counts["jmp_thunk"] +
            callable_counts["ret_stub"]) != totals["function_start"]:
        die("callable shapes do not sum to the function starts")
    indirect = data["indirect"]
    if indirect and indirect["resolved"] + indirect["blocked"] != indirect["sites"]:
        die("indirect resolution counts do not sum to the site count")
    if indirect:
        if indirect.get("analysis") != "cfg_forward":
            die("indirect analysis is not the CFG dataflow")
        sites = {record["site"] for record in indirect["sites_detail"]}
        if len(sites) != indirect["sites"]:
            die("indirect site records are not unique")
        for record in indirect["sites_detail"]:
            if record["status"] == "blocked" and not record.get("reason"):
                die(f"blocked site {record['site']} has no reason")
            if record["status"] == "resolved" and not record.get("target"):
                die(f"resolved site {record['site']} has no target")
    if not native_thunk_claims_ok(data):
        die("native thunk registry or pinned body hash mismatch")

    if "--mutate" in argv:
        bad_inc = inc.replace("GuestEntryKind::FunctionStart, GuestEntryCallable::RetailEntry",
                              "GuestEntryKind::FunctionStart, GuestEntryCallable::NotCallable", 1)
        tampered = json.loads(json.dumps(data))
        tampered["indirect"]["resolved"] += 1
        if bad_inc == inc:
            die("mutation seed did not change the table")
        if artifacts_match(data, inc, data, bad_inc):
            die("tampered table row was not detected")
        if artifacts_match(data, inc, tampered, inc):
            die("tampered total was not detected")
        bad_thunk = json.loads(json.dumps(data))
        bad_thunk["native_thunks"]["thunks"][0]["sha256"] = "0" * 64
        if native_thunk_claims_ok(bad_thunk):
            die("tampered native-thunk body hash was not detected")
        print("OK mutation probe: tampered row, total and thunk hash all detected")

    closure = data["closure"]
    print(f"OK check_entry_closure: entries={totals['entries']} start={totals['function_start']} "
          f"interior={totals['function_interior']} plt={totals['plt']} "
          f"xdata={totals['executable_data']} data={totals['image_data']} "
          f"outside={totals['outside_image']} "
          f"retail={callable_counts['retail_entry']} thunks={callable_counts['jmp_thunk']} "
          f"stubs={callable_counts['ret_stub']} "
          f"indirect={indirect['resolved'] if indirect else '?'}/"
          f"{indirect['sites'] if indirect else '?'} "
          f"closure={closure['with_native_entry'] if closure else '?'}/"
          f"{closure['functions'] if closure else '?'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
