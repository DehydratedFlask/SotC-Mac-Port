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


@lru_cache(maxsize=8)
def direct_call_evidence(targets: tuple[int, ...]) -> dict:
    """Re-derive full-FDE direct calls and liveness for the wanted targets."""
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    return ec._scan_full_fde_direct_calls(elf, fdes, set(targets))


@lru_cache(maxsize=4)
def indirect_liveness(targets: tuple[int, ...]) -> dict[int, tuple[list[str], list[str]]]:
    """Re-derive rax liveness at every resolved indirect site for the wanted targets."""
    data = json.loads(ec.OUT_JSON.read_text())["indirect"]["sites_detail"]
    wanted = set(targets)
    by_function: dict[int, list[dict]] = {}
    for record in data:
        if record["status"] == "resolved" and int(record["target"], 16) in wanted:
            by_function.setdefault(int(record["function"], 16), []).append(record)
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    sizes = dict(fdes)
    intervals = [(fn, fn + sizes[fn]) for fn in by_function if sizes.get(fn, 0)]
    if not intervals:
        return {}
    all_ins = ec.disassemble(intervals)
    result: dict[int, tuple[list[str], list[str]]] = {}
    for fn, records in by_function.items():
        size = sizes.get(fn, 0)
        if not size:
            continue
        window = [a for a in sorted(all_ins) if fn <= a < fn + size]
        if not window or window[0] != fn:
            continue
        live = ec._rax_live_after(all_ins, window, {int(r["site"], 16) for r in records})
        for record in records:
            target = int(record["target"], 16)
            sites, live_sites = result.setdefault(target, ([], []))
            if record["site"] not in sites:
                sites.append(record["site"])
            if live[int(record["site"], 16)]:
                live_sites.append(record["site"])
    return result


def known_concmd_slot_site() -> tuple[int, str, bool] | None:
    """Re-decode the one fixed 0x207d0 vtable caller from the pinned ELF."""
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    fn, site, slot, target = 0x207E0, 0x20A9E, 0x1594D78, 0x207D0
    size = dict(fdes).get(fn, 0)
    if not size or elf.u64(slot) != target or elf.raw(target, 1) != b"\xc3":
        return None
    ins = ec.disassemble([(fn, fn + size)])
    expect = ((0x20883, "leaq", "0x1594d60"),
              (0x2088a, "movq", "%rdx, 0xd0(%rsp)"),
              (0x20a88, "movq", "0xd0(%rsp), %rbx"),
              (0x20a9e, "callq", "*0x18(%rbx)"))
    for addr, mnemonic, operand in expect:
        actual = ins.get(addr)
        if actual is None or actual[0] != mnemonic or operand not in actual[1]:
            return None
    window = [a for a in sorted(ins) if fn <= a < fn + size]
    if not window or window[0] != fn:
        return None
    live = ec._rax_live_after(ins, window, {site})
    return target, f"0x{site:x}", live[site]


def known_comm_slot_site() -> tuple[int, str, bool] | None:
    """Re-decode the one fixed 0x25c90 vtable caller from the pinned ELF."""
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    fn, site, slot, target = 0x25DE0, 0x26199, 0x1595118, 0x25C90
    size = dict(fdes).get(fn, 0)
    if not size or elf.u64(slot) != target or elf.raw(target, 1) != b"\xc3":
        return None
    ins = ec.disassemble([(fn, fn + size)])
    expect = ((0x2612A, "leaq", "0x1595100"),
              (0x2614C, "movq", "%rax, 0xa0(%rsp)"),
              (0x2618E, "movq", "0xa0(%rsp), %rax"),
              (0x26199, "callq", "*0x18(%rax)"))
    for addr, mnemonic, operand in expect:
        actual = ins.get(addr)
        if actual is None or actual[0] != mnemonic or operand not in actual[1]:
            return None
    window = [a for a in sorted(ins) if fn <= a < fn + size]
    if not window or window[0] != fn:
        return None
    live = ec._rax_live_after(ins, window, {site})
    return target, f"0x{site:x}", live[site]


def native_thunk_claims_ok(data: dict, thunk_inc: str) -> bool:
    """Re-derive every registered thunk body and the dispatch slot from the pinned ELF."""
    block = data.get("native_thunks", {})
    thunks = block.get("thunks", [])
    if block.get("count") != 96 or len(thunks) != 96:
        return False
    source = (ROOT / "native/core/guest_thunks.cpp").read_text()
    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    starts = [start for start, _ in fdes]
    segments = ec.parse_segments()
    custom: set[int] = set()
    for thunk in thunks:
        rva = int(thunk["rva"], 16)
        kind, fn_start, size = ec.classify(rva, fdes, starts, segments)
        if kind != "function_start" or fn_start != rva or size != thunk["size"]:
            return False
        body = elf.raw(rva, thunk["size"])
        if hashlib.sha256(body).hexdigest() != thunk["sha256"]:
            return False
        if thunk.get("custom"):
            custom.add(rva)
            if f"0x{rva:x}ULL, \"{thunk['name']}\"" not in source:
                return False
        elif f"0x{rva:x}ULL, \"{thunk['name']}\"" not in thunk_inc:
            return False
        if rva == 0x9844B0:
            if thunk.get("callee") != "0xf8fed0" or thunk.get("format_text") != "%s":
                return False
            if elf.raw(0x12B57F4, 3) != b"%s\0":
                return False
            if ec._plt_import_nid(elf, 0x1600558) != "hcuQgD53UxM":
                return False
        if rva == 0x20D80:
            expected = {"container": "0x2dfcbc8", "tree_offset": "0x18", "size_offset": "0x28",
                        "lock_offset": "0x58", "inserter": "0x211d0", "comparator": "0xf8f500",
                        "comparator_import": "strcasecmp", "value_offset": "0x40",
                        "tree_color_offset": "0x18", "null_branch": "0x20e7f",
                        "global_sentinel": "0x2dfce50", "global_tail": "0x2dfce58",
                        "global_guard": "0x2dfce60"}
            for key, value in expected.items():
                if thunk.get(key) != value:
                    return False
            for site, target in ((0x20DE2, 0x3B390), (0x20E1A, 0xF8F500),
                                 (0x20E4C, 0xF8F500), (0x20E61, 0x211D0),
                                 (0x20E71, 0x3B3C0)):
                raw = elf.raw(site, 5)
                if raw[0] != 0xE8 or site + 5 + int.from_bytes(raw[1:5], "little", signed=True) != target:
                    return False
    if custom != {0x20D60, 0x20D80, 0x9844B0}:
        return False
    guard = block.get("candidate_guard", {})
    if guard.get("bounded_to_campaign_candidates") is not True:
        return False
    if guard.get("unbounded_one_byte_rows_rejected") != 570:
        return False
    if block.get("blocked") is None or len(block["blocked"]) != 20:
        return False
    if any(int(entry["rva"], 16) == 0x207D0 for entry in block["blocked"]):
        return False
    if len(thunks) + len(block["blocked"]) != guard.get("candidate_rows"):
        return False
    for entry in block["blocked"]:
        if not entry.get("reason"):
            return False
        rva = int(entry["rva"], 16)
        if ec.classify(rva, fdes, starts, segments)[0] not in ("function_start", "plt"):
            return False
        if "reference_resolution" in entry and rva != 0x20D80:
            expected = ec.classify_image_reference(elf, rva)
            if entry["reference_resolution"] != expected["resolution"]:
                return False
            if entry.get("image_qword_reference_sites", []) != [f"0x{s:x}" for s in expected["image_sites"]]:
                return False
            if entry.get("relocation_slots", []) != [f"0x{s:x}" for s in expected["relocation_slots"]]:
                return False
        if rva == 0x20D80:
            return False  # must be a registered row, not blocked

    elf = ec.Elf()
    fdes = ec.parse_fdes(elf)
    sizes = dict(fdes)
    starts = [start for start, _ in fdes]
    segments = ec.parse_segments()
    entries = json.loads(ec.ENTRY_MAP.read_text())["entries"]
    mapped = {int(key, 16) for key in entries}
    start_set = {rva for rva in mapped
                 if ec.classify(rva, fdes, starts, segments)[0] == "function_start"}
    small = {rva for rva in start_set if sizes.get(rva, 0) in (1, 4, 5)}
    resolved = {int(record["target"], 16) for record in data["indirect"]["sites_detail"]
                if record["status"] == "resolved"}
    target_set = tuple(sorted(small | resolved | {0x20D80, 0x9844B0}))
    scan = direct_call_evidence(target_set)
    if block.get("scan") != scan["summary"]:
        return False
    direct_sites: dict[int, list[dict]] = {}
    for entry in scan["sites"]:
        direct_sites.setdefault(int(entry["target"]), []).append(entry)
    indirect_sites: dict[int, int] = {}
    for record in data["indirect"]["sites_detail"]:
        if record["status"] == "resolved":
            target = int(record["target"], 16)
            indirect_sites[target] = indirect_sites.get(target, 0) + 1
    evidence_targets = tuple(sorted({int(t["rva"], 16) for t in thunks + block["blocked"]}))
    indirect_live = indirect_liveness(evidence_targets)
    for known in (known_concmd_slot_site(), known_comm_slot_site()):
        if known is None:
            continue
        known_target, known_site, known_is_live = known
        indirect_sites[known_target] = indirect_sites.get(known_target, 0) + 1
        sites, live_sites = indirect_live.get(known_target, ([], []))
        if known_site not in sites:
            sites.append(known_site)
        if known_is_live:
            live_sites.append(known_site)
        indirect_live[known_target] = (sites, live_sites)
    for thunk in list(thunks) + list(block["blocked"]):
        if "call_sites" not in thunk:
            continue
        rva = int(thunk["rva"], 16)
        sites = direct_sites.get(rva, [])
        live = sorted({f"0x{int(site['site']):x}" for site in sites if site["live"]} |
                      set(indirect_live.get(rva, ([], []))[1]))
        expected_indirect = indirect_sites.get(rva, 0)
        if thunk["direct_call_sites"] != len(sites):
            return False
        if thunk.get("indirect_call_sites", 0) != expected_indirect:
            return False
        if thunk["call_sites"] != len(sites) + expected_indirect:
            return False
        if "rax_live_sites" in thunk and thunk["rax_live_sites"] != live:
            return False
        if live:
            return False
        if "image_qword_references" in thunk:
            expected_image = ec.classify_image_reference(elf, rva)
            if thunk["image_qword_references"] != len(expected_image["image_sites"]):
                return False
            if thunk.get("image_qword_reference_sites", []) != [f"0x{s:x}" for s in expected_image["image_sites"]]:
                return False
            if thunk.get("relocation_slots", []) != [f"0x{s:x}" for s in expected_image["relocation_slots"]]:
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
        bad_slot = json.loads(json.dumps(data))
        for row in bad_slot["native_thunks"]["thunks"]:
            if row["rva"] == "0x25c90":
                row["indirect_call_sites"] = 0
                row["call_sites"] = 0
                break
        else:
            die("second fixed vtable row is missing")
        if native_thunk_claims_ok(bad_slot, thunk_inc):
            die("tampered 0x25c90 indirect-call count was not detected")
        bad_scan = json.loads(json.dumps(data))
        bad_scan["native_thunks"]["scan"]["validated_sites"] += 1
        if native_thunk_claims_ok(bad_scan, thunk_inc):
            die("tampered full-FDE scan summary was not detected")
        bad_dispatch = json.loads(json.dumps(data))
        bad_dispatch["native_thunks"]["dispatch"]["target"] = "0x1234"
        if native_thunk_claims_ok(bad_dispatch, thunk_inc):
            die("tampered dispatch target was not detected")
        bad_reference = json.loads(json.dumps(data))
        for entry in bad_reference["native_thunks"]["blocked"]:
            if "reference_resolution" in entry:
                entry["reference_resolution"] = "hand_edited"
                break
        else:
            die("no blocked row carries reference evidence to mutate")
        if native_thunk_claims_ok(bad_reference, thunk_inc):
            die("tampered reference resolution was not detected")
        bad_guard = json.loads(json.dumps(data))
        bad_guard["native_thunks"]["candidate_guard"]["candidate_rows"] += 1
        if native_thunk_claims_ok(bad_guard, thunk_inc):
            die("tampered candidate-set guard was not detected")
        print("OK mutation probe: row, total, thunk hash, direct count, scan summary, "
                  "dispatch target, reference resolution and candidate guard detected")

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
