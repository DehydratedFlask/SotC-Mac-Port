#!/usr/bin/env python3
"""Enumerate and classify the indirect call sites inside a direct-call closure.

`cfg_census.py` counts indirect calls but never looks at them. This tool lists every indirect
`call` inside the transitive direct-call closure of the given seeds and classifies each site by the
short instruction window before it:

  vtable-slot      `callq *0xNN(%rax)` where the window loads `%rax` from `0xMM(%reg)`
                   -> slot 0xNN of the object at +0xMM
  member-pointer   `callq *%rax` where the window loads `%rax` from `0xMM(%reg)`
  first-slot       `callq *(%rax)` (vtable slot 0)
  register         anything else; recorded, not guessed

Nothing is executed and no target is invented. A classification describes the call shape visible in
the window; it is not proof of the callee identity.

Usage:
  python3 tools/lift/indirect_frontier.py --seed 0x531d60 --json local/validation/indirect-frontier/closures.json
  python3 tools/lift/cfg_census.py --seed 0x531d60      # unchanged census
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
import re
import struct
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
DEFAULT_FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DEFAULT_ELF = ROOT / "local/re/eboot.elf"

_LINE = re.compile(r"^\s*([0-9a-f]+):\s+(.*)$")
_INSN = re.compile(r"^([0-9a-z.]+)\s*(.*)$")
# movq 0x48(%rbx), %rax  |  movl (%rax), %ecx  |  leaq 0x28(%rbx), %rdi
_MOV_LOAD = re.compile(r"^mov[a-z]*\s+(-?0x[0-9a-f]+)?\((%r[a-z0-9]+)\)\s*,\s*(%r[a-z0-9]+)$")
_MOV_OFF = re.compile(r"^mov[a-z]*\s+(-?0x[0-9a-f]+)\((%r[a-z0-9]+)\)\s*,\s*(%r[a-z0-9]+)$")
_LEA = re.compile(r"^lea[a-z]*\s+(-?0x[0-9a-f]+)?\((%r[a-z0-9]+)\)\s*,\s*(%r[a-z0-9]+)$")
_CALL_IND_OFF = re.compile(r"^\*0x([0-9a-f]+)\((%r[a-z0-9]+)\)$")
_CALL_IND_DEREF = re.compile(r"^\*\((%r[a-z0-9]+)\)$")
_CALL_IND_REG = re.compile(r"^\*(%r[a-z0-9]+)$")
_RIP_LOAD = re.compile(r"^(mov[a-z]*|lea[a-z]*)\s+(-?0x[0-9a-f]+)?\(%rip\)\s*,\s*(%r[a-z0-9]+)")
_RIP_COMMENT = re.compile(r"#\s*(0x[0-9a-f]+)")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def parse_line(line: str):
    match = _LINE.match(line)
    if match is None:
        return None
    try:
        address = int(match.group(1), 16)
    except ValueError:
        return None
    insn = _INSN.match(match.group(2).strip())
    if insn is None:
        return None
    return address, insn.group(1), insn.group(2).strip()


def rip_global_for(register: str, window):
    """(address, form) for a RIP-relative global feeding `register`.

    form == "address" when the instruction is `lea`, so the register holds the global's address;
    form == "value" when it is `mov`, so the register holds the qword stored at the global.
    """
    for _addr, mnem, ops in reversed(window):
        match = _RIP_LOAD.match(f"{mnem} {ops}")
        if match is None or match.group(3) != register:
            continue
        comment = _RIP_COMMENT.search(ops)
        if comment is None:
            return None
        # objdump already prints the resolved effective address in the comment; the displacement
        # in the operand must not be added again.
        form = "address" if mnem.startswith("lea") else "value"
        return (int(comment.group(1), 16), form)
    return None


def classify(operands: str, window):
    """Return (class, detail) for one indirect call, using the preceding window."""
    match = _CALL_IND_OFF.match(operands)
    if match:
        slot = int(match.group(1), 16)
        register = match.group(2)
        load = None
        for _addr, mnem, ops in reversed(window):
            hit = _MOV_OFF.match(f"{mnem} {ops}")
            if hit and hit.group(3) == register:
                load = hit
                break
        if load is not None:
            object_off = int(load.group(1), 16)
            detail = {
                "slot": slot,
                "vtable_register": register,
                "object_offset": object_off,
                "object_register": load.group(2),
            }
            global_hit = rip_global_for(load.group(2), window)
            if global_hit is not None:
                detail["global_address"], detail["global_form"] = global_hit
            return "vtable-slot", detail
        global_hit = rip_global_for(register, window)
        if global_hit is not None:
            detail = {"slot": slot, "vtable_register": register,
                      "global_address": global_hit[0], "global_form": global_hit[1],
                      "object_offset": 0, "vtable_is_register": True}
            return "vtable-slot", detail
        return "register-offset", {"slot": slot, "register": register}

    match = _CALL_IND_DEREF.match(operands)
    if match:
        register = match.group(1)
        for _addr, mnem, ops in reversed(window):
            hit = _MOV_OFF.match(f"{mnem} {ops}")
            if hit and hit.group(3) == register:
                detail = {"vtable_register": register,
                          "object_offset": int(hit.group(1), 16),
                          "object_register": hit.group(2)}
                global_hit = rip_global_for(hit.group(2), window)
                if global_hit is not None:
                    detail["global_address"], detail["global_form"] = global_hit
                return "first-slot", detail
            hit = _MOV_LOAD.match(f"{mnem} {ops}")
            if hit and hit.group(3) == register:
                return "first-slot", {"vtable_register": register}
        return "first-slot", {"vtable_register": register}

    match = _CALL_IND_REG.match(operands)
    if match:
        register = match.group(1)
        for _addr, mnem, ops in reversed(window):
            hit = _MOV_OFF.match(f"{mnem} {ops}")
            if hit and hit.group(3) == register:
                return "member-pointer", {"register": register,
                                          "object_offset": int(hit.group(1), 16),
                                          "object_register": hit.group(2)}
            hit = _LEA.match(f"{mnem} {ops}")
            if hit and hit.group(3) == register:
                return "member-pointer", {"register": register,
                                          "object_offset": int(hit.group(1) or "0", 16),
                                          "object_register": hit.group(2)}
        return "register", {"register": register}

    return "register", {"operand": operands}


def scan(path: Path, fdes, window_size: int):
    edges = []
    sites = []
    current_owner = None
    window = []
    try:
        handle = open(path, "r", encoding="utf-8", errors="replace")
    except OSError as exc:
        raise SystemExit(f"cannot read disassembly {path}: {exc}")

    with handle:
        for line in handle:
            if "call" not in line and "jmp" not in line and "mov" not in line and "lea" not in line:
                continue
            parsed = parse_line(line)
            if parsed is None:
                continue
            address, mnemonic, operands = parsed
            owner = fdes.find(address)
            owner_start = None if owner is None else owner[0]
            if owner_start != current_owner:
                current_owner = owner_start
                window = []
            if mnemonic.startswith("call"):
                if operands.startswith("*"):
                    sites.append({"function": owner_start, "address": address,
                                  "kind": "call", "operands": operands,
                                  "window": [(a, m, o) for a, m, o in window]})
                else:
                    head = operands.split()[0]
                    try:
                        target = int(head, 16)
                    except ValueError:
                        target = None
                    if owner_start is not None and target is not None:
                        # Match cfg_census: an edge goes to the containing FDE range start, and a
                        # mid-range target is attributed to that function, never to a bare address.
                        containing = fdes.find(target)
                        if containing is not None:
                            edges.append((owner_start, containing[0]))
            elif mnemonic.startswith("jmp") and operands.startswith("*"):
                sites.append({"function": owner_start, "address": address,
                              "kind": "jump", "operands": operands,
                              "window": [(a, m, o) for a, m, o in window]})
            window.append((address, mnemonic, operands))
            if len(window) > window_size:
                window.pop(0)
    return edges, sites


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--dis", type=Path, default=DEFAULT_DIS)
    parser.add_argument("--fde", type=Path, default=DEFAULT_FDE)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--seed", action="append", default=None)
    parser.add_argument("--window", type=int, default=4)
    parser.add_argument("--json", type=Path, default=None)
    parser.add_argument("--summary-json", type=Path, default=None,
                        help="publishable summary: counts and resolved targets, no instruction text")
    args = parser.parse_args(argv)

    for path in (args.dis, args.fde, args.elf):
        if not path.is_file():
            raise SystemExit(f"missing input {path}")

    census = load_module("cfg_census", ROOT / "tools/lift/cfg_census.py")
    fdes, _duplicates = census.load_fde_ranges(args.fde)
    seeds = [int(seed, 16) for seed in (args.seed or ["0x531d60"])]

    edges, sites = scan(args.dis, fdes, args.window)
    adjacency = census.build_adjacency(edges)
    reachable = census.closure(seeds, adjacency)
    reachable_set = set(reachable)

    inside = [site for site in sites if site["function"] in reachable_set]
    rows = []
    for site in inside:
        kind, detail = classify(site["operands"], site["window"])
        if site["kind"] == "jump":
            kind = "indirect-jump"
        rows.append({
            "function": f"0x{site['function']:x}",
            "address": f"0x{site['address']:x}",
            "operands": site["operands"],
            "class": kind,
            "detail": detail,
            "window": [f"0x{a:x} {m} {o}" for a, m, o in site["window"]],
        })
    # Resolve what the window actually pins down: a RIP-relative global plus a vtable offset and
    # slot can be read straight out of the pinned image. Anything else stays unknown, by design.
    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as dyn  # noqa: E402

    elf_bytes = args.elf.read_bytes()
    _segments, headers = dyn.program_headers(elf_bytes)
    image = dyn.Image(elf_bytes, headers)

    def read_u64(address: int):
        try:
            if address <= 0 or address + 8 > (1 << 40):
                return None
            raw = image.read(address, 8)
        except Exception:
            return None
        if len(raw) != 8:
            return None
        return struct.unpack("<Q", raw)[0]

    resolved = 0
    for row in rows:
        detail = row["detail"]
        global_address = detail.get("global_address")
        if global_address is None:
            row["resolution"] = "unknown (no RIP-relative base in window)"
            continue
        form = detail.get("global_form", "address")
        object_base = read_u64(global_address) if form == "value" else global_address
        if object_base is None or object_base == 0:
            row["resolution"] = "unknown (global holds no pointer)"
            continue
        if detail.get("vtable_is_register"):
            vtable = object_base
        else:
            vtable = read_u64(object_base + detail.get("object_offset", 0))
        if vtable is None or vtable == 0:
            row["resolution"] = "unknown (object yields no vtable)"
            continue
        slot = detail.get("slot", 0)
        target = read_u64(vtable + slot)
        if target is None or target == 0:
            row["resolution"] = "unknown (vtable slot unreadable)"
            continue
        row["resolved_target"] = f"0x{target:x}"
        row["resolution"] = "resolved (global -> vtable -> slot)"
        resolved += 1

    rows.sort(key=lambda row: int(row["address"], 16))

    counts = Counter(row["class"] for row in rows)
    frontier_functions = len({row["function"] for row in rows})
    document = {
        "schema": "sotc-indirect-frontier/1",
        "inputs": {
            "dis_sha256": sha256_file(args.dis),
            "fde_sha256": sha256_file(args.fde),
            "elf_sha256": sha256_file(args.elf),
        },
        "seeds": [f"0x{seed:x}" for seed in seeds],
        "closure_functions": len(reachable_set),
        "indirect_sites_in_closure": len(rows),
        "indirect_calls_in_closure": sum(1 for row in rows if row["class"] != "indirect-jump"),
        "frontier_functions": frontier_functions,
        "sites_by_class": dict(sorted(counts.items())),
        "resolved_targets": resolved,
        "unknown_targets": len(rows) - resolved,
        "sites": rows,
    }

    print(f"seeds: {', '.join(document['seeds'])}")
    print(f"closure functions: {document['closure_functions']}")
    print(f"indirect sites in closure: {document['indirect_sites_in_closure']} "
          f"({document['indirect_calls_in_closure']} calls)")
    print(f"frontier functions (containing at least one indirect site): {frontier_functions}")
    print(f"targets resolved from the image: {resolved} / {len(rows)}")
    for name, count in document["sites_by_class"].items():
        print(f"  {name:15s} {count}")
    for row in rows[:10]:
        print(f"  {row['address']:>10s} {row['class']:15s} {row['operands']:<24s} fn={row['function']}")
    if len(rows) > 10:
        print(f"  ... {len(rows) - 10} more")

    if args.summary_json:
        summary = {
            "schema": "sotc-indirect-frontier-summary/1",
            "inputs": document["inputs"],
            "seeds": document["seeds"],
            "closure_functions": document["closure_functions"],
            "frontier_functions": document["frontier_functions"],
            "indirect_sites_in_closure": document["indirect_sites_in_closure"],
            "indirect_calls_in_closure": document["indirect_calls_in_closure"],
            "sites_by_class": document["sites_by_class"],
            "resolved_targets": document["resolved_targets"],
            "unknown_targets": document["unknown_targets"],
            "resolved_rows": [
                {"address": row["address"], "class": row["class"], "operands": row["operands"],
                 "target": row["resolved_target"],
                 "global_address": row["detail"].get("global_address"),
                 "slot": row["detail"].get("slot")}
                for row in rows if "resolved_target" in row
            ],
        }
        args.summary_json.parent.mkdir(parents=True, exist_ok=True)
        args.summary_json.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
        print(f"wrote {args.summary_json}")

    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(document, indent=2, sort_keys=True) + "\n")
        print(f"wrote {args.json}")
    print("STATIC_ANALYSIS_VERIFIED only: nothing was executed; classifications describe call shape.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
