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

import bisect
import hashlib
from functools import lru_cache
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


def artifacts_match(data: dict, inc: str, thunk_inc: str, stored_json: str,
                    stored_inc: str, stored_thunk_inc: str) -> bool:
    return (stored_json == data and stored_inc == inc and
            stored_thunk_inc == thunk_inc)


@lru_cache(maxsize=1)
def direct_call_evidence() -> dict[int, dict]:
    """Re-derive direct `call rel32` sites and their rax liveness from the pinned ELF."""
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    starts = [start for start, _ in fdes]
    sizes = dict(fdes)
    segments = ec.parse_segments()
    entries = json.loads(ec.ENTRY_MAP.read_text())["entries"]
    mapped = {int(k, 16) for k in entries}
    start_set = {rva for rva in mapped if ec.classify(rva, fdes, starts, segments)[0] == "function_start"}
    ins = ec.disassemble([(rva, rva + sizes[rva]) for rva in start_set if sizes.get(rva)])
    addrs = sorted(ins)
    caller_starts = sorted(start_set)
    by_target: dict[int, list[tuple[int, int]]] = {}
    for site in addrs:
        mnemonic, operands = ins[site]
        if mnemonic not in ("call", "callq"):
            continue
        match = __import__("re").match(r"^(0x[0-9a-f]+)(?: <[^>]*>)?$", operands.strip())
        if match is None:
            continue
        target = int(match.group(1), 16)
        if target not in start_set:
            continue
        k = bisect.bisect_right(caller_starts, site) - 1
        if k < 0:
            continue
        fn = caller_starts[k]
        if not (fn <= site < fn + sizes.get(fn, 0)):
            continue
        by_target.setdefault(target, []).append((fn, site))
    out: dict[int, dict] = {}
    for target, sites in by_target.items():
        by_caller: dict[int, set[int]] = {}
        for fn, site in sites:
            by_caller.setdefault(fn, set()).add(site)
        live_sites = []
        for fn, fn_sites in by_caller.items():
            window = [a for a in addrs if fn <= a < fn + sizes.get(fn, 0)]
            result = ec._rax_live_after(ins, window, fn_sites)
            live_sites += [f"0x{site:x}" for site in fn_sites if result[site]]
        out[target] = {"count": len(sites), "live": sorted(live_sites)}
    return out


def native_thunk_claims_ok(data: dict, thunk_inc: str) -> bool:
    """Re-derive every registered thunk body and the dispatch slot from the pinned ELF."""
    block = data.get("native_thunks", {})
    thunks = block.get("thunks", [])
    if block.get("count") != 91 or len(thunks) != 91:
        return False
    source = (ROOT / "native/core/guest_thunks.cpp").read_text()
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    starts = [start for start, _ in fdes]
    segments = ec.parse_segments()
    custom = 0
    for thunk in thunks:
        rva = int(thunk["rva"], 16)
        kind, fn_start, size = ec.classify(rva, fdes, starts, segments)
        if kind != "function_start" or fn_start != rva or size != thunk["size"]:
            return False
        body = elf.raw(rva, thunk["size"])
        if hashlib.sha256(body).hexdigest() != thunk["sha256"]:
            return False
        if thunk.get("custom"):
            custom += 1
            if f"0x{rva:x}ULL, \"{thunk['name']}\"" not in source:
                return False
        elif f"0x{rva:x}ULL, \"{thunk['name']}\"" not in thunk_inc:
            return False
    if custom != 1:
        return False
    if block.get("blocked") is None or len(block["blocked"]) != 25:
        return False
    for entry in block["blocked"]:
        if not entry.get("reason"):
            return False
        rva = int(entry["rva"], 16)
        if ec.classify(rva, fdes, starts, segments)[0] not in ("function_start", "plt"):
            return False

    direct = direct_call_evidence()
    for thunk in thunks:
        if "direct_call_sites" not in thunk:
            continue
        rva = int(thunk["rva"], 16)
        evidence = direct.get(rva, {"count": 0, "live": []})
        if thunk["direct_call_sites"] != evidence["count"]:
            return False
        if thunk.get("rax_live_sites") != evidence["live"]:
            return False
        if evidence["live"]:
            return False

    dispatch = block.get("dispatch", {})
    site = int(dispatch.get("site", "0x0"), 16)
    target = int(dispatch.get("target", "0x0"), 16)
    vtable = int(dispatch.get("vtable", "0x0"), 16)
    slot = int(dispatch.get("slot", "0x0"), 16)
    if elf.u64(vtable + slot) != target:
        return False
    if hashlib.sha256(elf.raw(vtable, 0x28)).hexdigest() != dispatch.get("vtable_sha256"):
        return False
    record = next((r for r in data["indirect"]["sites_detail"] if r["site"] == dispatch["site"]),
                  None)
    if record is None or record["status"] != "resolved" or record["target"] != dispatch["target"]:
        return False
    if record["function"] != dispatch["function"] or record["operand"] != f"*0x{slot:x}(%rax)":
        return False
    instruction = elf.raw(site, 3)
    if instruction[0] != 0xFF or instruction[1] != 0x50 or instruction[2] != slot:
        return False
    return True


def main(argv: list[str]) -> int:
    if not (ROOT / "local/re/eboot.elf").exists():
        print("SKIP check_entry_closure: private ELF absent")
        return 0
    data, inc, thunk_inc = ec.build()
    stored_json = json.loads(ec.OUT_JSON.read_text())
    stored_inc = ec.OUT_INC.read_text()
    stored_thunk_inc = ec.OUT_THUNK_INC.read_text()
    if not artifacts_match(data, inc, thunk_inc, stored_json, stored_inc, stored_thunk_inc):
        if stored_json != data:
            die("docs/entry_closure.json is stale; rerun tools/lift/entry_closure.py")
        if stored_inc != inc:
            die("native/core/guest_entry_table.inc is stale; rerun tools/lift/entry_closure.py")
        die("native/core/guest_thunk_table.inc is stale; rerun tools/lift/entry_closure.py")

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
    if not native_thunk_claims_ok(data, thunk_inc):
        die("native thunk registry, dispatch slot or pinned body hash mismatch")

    if "--mutate" in argv:
        bad_inc = inc.replace("GuestEntryKind::FunctionStart, GuestEntryCallable::RetailEntry",
                              "GuestEntryKind::FunctionStart, GuestEntryCallable::NotCallable", 1)
        tampered = json.loads(json.dumps(data))
        tampered["indirect"]["resolved"] += 1
        if bad_inc == inc:
            die("mutation seed did not change the table")
        if artifacts_match(data, inc, thunk_inc, data, bad_inc, thunk_inc):
            die("tampered table row was not detected")
        if artifacts_match(data, inc, thunk_inc, tampered, inc, thunk_inc):
            die("tampered total was not detected")
        bad_thunk = json.loads(json.dumps(data))
        bad_thunk["native_thunks"]["thunks"][0]["sha256"] = "0" * 64
        if native_thunk_claims_ok(bad_thunk, thunk_inc):
            die("tampered native-thunk body hash was not detected")
        bad_direct = json.loads(json.dumps(data))
        for row in bad_direct["native_thunks"]["thunks"]:
            if row.get("direct_call_sites"):
                row["direct_call_sites"] = 0
                break
        else:
            die("no row carries direct-call evidence to mutate")
        if native_thunk_claims_ok(bad_direct, thunk_inc):
            die("tampered direct-call count was not detected")
        bad_dispatch = json.loads(json.dumps(data))
        bad_dispatch["native_thunks"]["dispatch"]["target"] = "0x1234"
        if native_thunk_claims_ok(bad_dispatch, thunk_inc):
            die("tampered dispatch target was not detected")
        print("OK mutation probe: row, total, thunk hash, direct count and dispatch target detected")

    closure = data["closure"]
    print(f"OK check_entry_closure: entries={totals['entries']} start={totals['function_start']} "
          f"interior={totals['function_interior']} plt={totals['plt']} "
          f"xdata={totals['executable_data']} data={totals['image_data']} "
          f"outside={totals['outside_image']} "
          f"retail={callable_counts['retail_entry']} thunks={callable_counts['jmp_thunk']} "
          f"stubs={callable_counts['ret_stub']} "
          f"indirect={indirect['resolved'] if indirect else '?'}/"
          f"{indirect['sites'] if indirect else '?'} "
          f"native_thunks={data['native_thunks']['count']} "
          f"closure={closure['with_native_entry'] if closure else '?'}/"
          f"{closure['functions'] if closure else '?'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
