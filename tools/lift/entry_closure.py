#!/usr/bin/env python3
"""Type every guest address in the native-entry map against the pinned ELF (CPU-01-ENTRY-MAP-CLOSURE-01).

Offline, read-only and stdlib-only. The private ELF (`local/re/eboot.elf`, sha256 d0723afb...
5861d3) is read through `slot_pin_util.Elf`, which verifies the digest; nothing is loaded,
relocated, executed or written into the pinned image.

`docs/ENTRY_MAP.md` records which guest RVAs our native modules name, but not *what* each address
is. This tool closes that gap with the ELF's own unwind data:

  * parses `.eh_frame_hdr` (fixed VA 0x150c08c, encodings checked) into the 55,790 FDE ranges
    that define the function universe;
  * classifies every distinct mapped RVA as `function_start` (exact FDE start), `function_interior`
    (inside an FDE range: a branch/jump label or embedded constant), `plt` (the 0xf8f380..0xf91740
    import stubs), `executable_data` (R+X bytes with no FDE: literal pools and the read-only tail),
    `image_data` (another mapped PT_LOAD, including the OS-specific 0x1594000 read-only segment
    that hosts the vtables) or `outside_image`;
  * intersects the map with the 0x531d60 Init closure so the callee-closure gap is measured, not
    remembered;
  * writes `native/core/guest_entry_table.inc` so the native runtime can answer "is this RVA a
    function start?" by binary search, and `docs/ENTRY_CLOSURE.md` + `docs/entry_closure.json`.

Honesty labels: every number is STATIC_ANALYSIS_VERIFIED. An `function_interior` entry is not an
error: 381 mapped constants are branch targets or embedded literals inside a function (for example
`kSlot536BranchTarget` at 0x13455d). Only `function_start` entries are dispatchable call entries.
Nothing here proves a calling convention, resolves an indirect call, or executes the ELF.

Usage: entry_closure.py [--check]   (--check is what check_entry_closure.py runs)
"""

from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "scripts"))

from slot_pin_util import Elf  # noqa: E402

ENTRY_MAP = ROOT / "docs/entry_map.json"
OUT_JSON = ROOT / "docs/entry_closure.json"
OUT_DOC = ROOT / "docs/ENTRY_CLOSURE.md"
OUT_INC = ROOT / "native/core/guest_entry_table.inc"
CLOSURE_JSON = ROOT / "local/validation/aot-cfg/closure_531d60.json"
EH_FRAME_HDR = 0x150C08C
PLT_START = 0xF8F380
PLT_END = 0xF91740
CLOSURE_SEED = "0x531d60"

KIND_ORDER = ("function_start", "function_interior", "plt", "executable_data", "image_data",
              "outside_image")
KIND_ENUM = {
    "function_start": "FunctionStart",
    "function_interior": "FunctionInterior",
    "plt": "Plt",
    "executable_data": "ExecutableData",
    "image_data": "ImageData",
    "outside_image": "OutsideImage",
}
KIND_HELP = {
    "function_start": "exact FDE start: a real function entry, dispatchable",
    "function_interior": "inside an FDE range but not its start: branch/jump label or embedded literal",
    "plt": "import stub in 0xf8f380..0xf91740: callable only through the resolved import",
    "executable_data": "R+X bytes with no FDE: literal pool or the read-only text tail",
    "image_data": "mapped by a non-executable PT_LOAD (globals, vtables, BSS)",
    "outside_image": "not mapped by any PT_LOAD",
}


def die(message: str, code: int = 1) -> None:
    print(f"FAIL {message}", file=sys.stderr)
    raise SystemExit(code)


def parse_fdes(e: Elf) -> list[tuple[int, int]]:
    """(start, size) for every FDE in .eh_frame_hdr's binary-search table, sorted."""
    hdr = e.raw(EH_FRAME_HDR, 12)
    ver, fenc, cenc, tenc = hdr[0], hdr[1], hdr[2], hdr[3]
    if (ver, fenc, cenc, tenc) != (1, 0x1B, 0x03, 0x3B):
        die(f"unexpected .eh_frame_hdr encodings {(ver, fenc, cenc, tenc)}")
    count = struct.unpack_from("<i", hdr, 8)[0]
    table = e.raw(EH_FRAME_HDR + 12, 8 * count)
    out = []
    for i in range(count):
        _loc, fde = struct.unpack_from("<ii", table, 8 * i)
        fa = EH_FRAME_HDR + fde
        rec = e.raw(fa, 16)
        start = struct.unpack_from("<i", rec, 8)[0] + fa + 8
        size = struct.unpack_from("<I", rec, 12)[0]
        out.append((start, size))
    out.sort()
    return out


def parse_segments() -> list[tuple[int, int, int, int]]:
    """(type, flags, vaddr, memsz) for every program header with a non-zero memory size."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    entsize = struct.unpack_from("<H", elf, 0x36)[0]
    count = struct.unpack_from("<H", elf, 0x38)[0]
    segments = []
    for i in range(count):
        off = phoff + i * entsize
        p_type, p_flags = struct.unpack_from("<II", elf, off)
        _poff, vaddr, _pa, _fsz, memsz, _align = struct.unpack_from("<QQQQQQ", elf, off + 8)
        if memsz:
            segments.append((p_type, p_flags, vaddr, memsz))
    return segments


def mapped(rva: int, segments: list[tuple[int, int, int, int]]) -> tuple[int, int, int, int] | None:
    for seg in segments:
        if seg[2] <= rva < seg[2] + seg[3]:
            return seg
    return None


def classify(rva: int, fdes: list[tuple[int, int]], starts: list[int],
             segments: list[tuple[int, int, int, int]]):
    """(kind, function_start, function_size) for one RVA."""
    import bisect
    k = bisect.bisect_right(starts, rva) - 1
    if k >= 0 and fdes[k][0] <= rva < fdes[k][0] + fdes[k][1]:
        if fdes[k][0] == rva:
            return "function_start", rva, fdes[k][1]
        return "function_interior", fdes[k][0], fdes[k][1]
    if PLT_START <= rva < PLT_END:
        return "plt", None, None
    seg = mapped(rva, segments)
    if seg is not None:
        executable = seg[1] & 1
        return ("executable_data" if executable else "image_data"), None, None
    return "outside_image", None, None


def owner(rows: list[dict]) -> tuple[str, str]:
    row = sorted(rows, key=lambda r: (r["header"], r["constant"]))[0]
    return row["constant"], row["header"]


def build() -> tuple[dict, str]:
    e = Elf()
    fdes = parse_fdes(e)
    if len(fdes) != 55790:
        die(f"FDE count {len(fdes)} != 55790")
    if fdes[0][0] != 0x80 or fdes[-1][0] + fdes[-1][1] != 0xF8F321:
        die(f"unexpected FDE extent {fdes[0][0]:#x}..{fdes[-1][0] + fdes[-1][1]:#x}")
    total = sum(size for _, size in fdes)
    if total != 15839273:
        die(f"FDE total bytes {total} != 15839273")
    starts = [start for start, _ in fdes]
    segments = parse_segments()

    entry_map = json.loads(ENTRY_MAP.read_text())
    rows = entry_map["entries"]
    kinds: dict[int, tuple[str, int | None, int | None]] = {}
    for key in rows:
        rva = int(key, 16)
        kinds[rva] = classify(rva, fdes, starts, segments)
    if len(kinds) != entry_map["totals"]["distinct_rvas"]:
        die("entry count disagrees with entry_map totals")

    counts = {kind: 0 for kind in KIND_ORDER}
    samples: dict[str, list[dict]] = {kind: [] for kind in KIND_ORDER}
    interior = []
    for rva in sorted(kinds):
        kind, fn_start, fn_size = kinds[rva]
        counts[kind] += 1
        constant, header = owner(rows[f"0x{rva:x}"])
        if len(samples[kind]) < 8:
            samples[kind].append({"rva": f"0x{rva:x}", "constant": constant, "header": header})
        if kind == "function_interior":
            interior.append({
                "rva": f"0x{rva:x}",
                "function": f"0x{fn_start:x}",
                "size": fn_size,
                "offset": rva - fn_start,
                "constant": constant,
                "header": header,
            })

    target_named = {kind: 0 for kind in KIND_ORDER}
    target_entries = 0
    for rva in sorted(kinds):
        constant, _header = owner(rows[f"0x{rva:x}"])
        names = [r["constant"] for r in rows[f"0x{rva:x}"]]
        if not any("Target" in n or n.endswith("Rva") or n.endswith("RVA") for n in names):
            continue
        target_entries += 1
        target_named[kinds[rva][0]] += 1

    closure = None
    closure_missing: list[str] = []
    if CLOSURE_JSON.exists():
        reached = json.loads(CLOSURE_JSON.read_text())["closure"]["reached_list"]
        closure_rvas = sorted(int(x, 16) for x in reached)
        with_entry = [rva for rva in closure_rvas if rva in kinds and kinds[rva][0] == "function_start"]
        closure_missing = [f"0x{rva:x}" for rva in closure_rvas if rva not in kinds]
        closure = {
            "seed": CLOSURE_SEED,
            "functions": len(closure_rvas),
            "with_native_entry": len(with_entry),
            "without_native_entry": len(closure_rvas) - len(with_entry),
            "percent": round(100.0 * len(with_entry) / len(closure_rvas), 1),
            "missing_sample": closure_missing[:16],
        }
        if any(rva in kinds and kinds[rva][0] == "function_interior" for rva in closure_rvas):
            die("a closure function is mapped only as an interior address")

    data = {
        "schema": "sotc-entry-closure/1",
        "inputs": {
            "elf_sha256": "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3",
            "entry_map": "docs/entry_map.json",
            "closure": "local/validation/aot-cfg/closure_531d60.json" if closure else None,
            "eh_frame_hdr": f"0x{EH_FRAME_HDR:x}",
            "plt": f"0x{PLT_START:x}..0x{PLT_END:x}",
        },
        "fde": {
            "count": len(fdes),
            "min_start": f"0x{fdes[0][0]:x}",
            "max_end": f"0x{fdes[-1][0] + fdes[-1][1]:x}",
            "total_bytes": total,
        },
        "totals": dict({"entries": len(kinds)}, **counts),
        "target_named": dict({"entries": target_entries}, **target_named),
        "closure": closure,
        "samples": samples,
        "interior": interior,
    }

    lines = [
        "// Generated by tools/lift/entry_closure.py; do not edit.",
        "// Pinned ELF sha256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3,",
        "// .eh_frame_hdr @ 0x150c08c (55,790 FDE ranges) and docs/entry_map.json (1,957 RVAs).",
        "// kind: 1 function start, 2 interior label, 3 PLT stub, 4 data in R+X, 5 image data.",
        "static constexpr GuestEntry kGuestEntryTable[] = {",
    ]
    for rva in sorted(kinds):
        lines.append(f"    {{0x{rva:012x}ULL, GuestEntryKind::{KIND_ENUM[kinds[rva][0]]}}},")
    lines.append("};")
    return data, "\n".join(lines) + "\n"


def render_doc(data: dict) -> str:
    totals = data["totals"]
    fde = data["fde"]
    closure = data["closure"]
    total = totals["entries"]
    lines = [
        "# Entry-point typing for the guest-address map (CPU-01-ENTRY-MAP-CLOSURE-01)",
        "",
        "**Evidence label: STATIC_ANALYSIS_VERIFIED.** Generated by `tools/lift/entry_closure.py`; the",
        "checker `tools/lift/check_entry_closure.py` re-derives every number from the pinned ELF",
        "(`sha256 d0723afb...5861d3`) and `docs/entry_map.json`. Nothing here was executed.",
        "",
        "`ENTRY_MAP.md` answers *which* guest RVAs our native modules name. This document answers the",
        "next question the roadmap asks: **what is at that address** - a function entry, a branch label",
        "inside a function, an import stub, or data - and how much of the executed Init closure has a",
        "native entry at all.",
        "",
        "## Inputs",
        "",
        f"- FDE universe: `.eh_frame_hdr` at `{data['inputs']['eh_frame_hdr']}` -> **{fde['count']:,}** ranges, "
        f"`{fde['min_start']}..{fde['max_end']}`, {fde['total_bytes']:,} B.",
        f"- Mapped RVAs: **{total:,}** distinct addresses from `docs/entry_map.json`.",
        "- Import stubs: `" + data["inputs"]["plt"] + "` (the PLT range the CFG census established).",
        "",
        "## Classification",
        "",
        "| Kind | Entries | Share | Meaning |",
        "| --- | ---: | ---: | --- |",
    ]
    for kind in KIND_ORDER:
        lines.append(f"| `{kind}` | **{totals[kind]:,}** | {100.0 * totals[kind] / total:.1f} % | {KIND_HELP[kind]} |")
    lines += [
        "",
        "## Findings",
        "",
        f"- **{totals['function_start']:,} of {total:,} mapped addresses ({100.0 * totals['function_start'] / total:.1f} %) are real function entries.** "
        "Those are the only entries a native dispatcher may call without further evidence.",
        f"- **{totals['function_interior']:,} are inside a function but not at its start.** They are not map errors:",
        "  they are the branch/jump labels and embedded literals the models name (`kSlot536Branch`,",
        "  `kSlot536BranchTarget`, `kFn1290End`, `kAppInfoRunDispatchNext`, ...). A dispatcher must",
        "  reject them; the full list is in `entry_closure.json` under `interior`.",
        f"- **{totals['plt']:,} are PLT stubs**, including `__cxa_atexit` (`0xf8f390`), `strlen`, `memcpy`,",
        "  `memmove`, `strncpy`, the guard acquire/release pair and `__stack_chk_fail`.",
        f"- **{totals['executable_data']:,} are R+X bytes with no unwind range**: literal pools and the",
        "  read-only tail after the last `ret` (0xf8f321). Reading them as code would be wrong.",
        f"- **{totals['image_data']:,} are mapped by a non-executable PT_LOAD**, including the vtables in",
        "  the OS-specific `0x1594000` read-only segment (`0x1594d90`, `0x1594dc0`, ...).",
        f"- **{totals['outside_image']} addresses fall outside every PT_LOAD.**",
        "",
        "### Named call targets",
        "",
        f"Of the **{data['target_named']['entries']}** distinct addresses whose constants say `Target`/`Rva`,",
    ]
    tn = data["target_named"]
    lines += [
        f"**{tn['function_start']} are function starts**, **{tn['function_interior']} are interior labels**, "
        f"**{tn['plt']} is a PLT stub** and **{tn['image_data']} are data** (vtables/globals). The",
        "name convention alone therefore overstates callability for the interior and data rows; the",
        "type, not the constant name, is the dispatch contract.",
        "",
        "### Init closure coverage",
        "",
    ]
    if closure:
        lines += [
            f"The shared CFG census (`closure_531d60.json`) reaches **{closure['functions']:,} functions** from the",
            f"Init entry `0x531d60`. **{closure['with_native_entry']:,} of them ({closure['percent']} %) have an exact",
            f"native entry** in this map; **{closure['without_native_entry']:,} do not yet**. That is the measured",
            "callee-closure gap G1 refers to, and it is a list, not an estimate: the unmapped functions are",
            "recorded in `closure.missing_sample`/the closure JSON.",
        ]
    else:
        lines.append("The private closure census was not present, so closure coverage was not measured in this run.")
    lines += [
        "",
        "## What this changes",
        "",
        "- `ENTRY_MAP.md`'s open item 1 (\"entry point type\") is now answered for every mapped RVA.",
        "- The native runtime can answer `guest_entry_find(rva)` and `guest_entry_kind_name()`; the",
        "  generated table is `native/core/guest_entry_table.inc` and the test is",
        "  `test_guest_entry_table`.",
        "- The map is still not an ABI: a `function_start` says an FDE begins there, not that a thunk",
        "  with the retail calling convention exists. Indirect targets remain unresolved.",
        "",
        "## Limits",
        "",
        "- FDE-derived: a function with no unwind record is invisible, and an FDE range may cover",
        "  alignment or literal bytes (which is exactly why interior addresses are typed, not rejected).",
        "- `.eh_frame_hdr` is read at its fixed VA `0x150c08c`; the encodings are checked, so a different",
        "  image fails closed instead of misparsing.",
        "- No semantic or behavioural claim: presence in the table means the address has that ELF type,",
        "  nothing more. Every address claim still needs the owning module's checker.",
        "",
    ]
    return "\n".join(lines)


def main(argv: list[str]) -> int:
    check_only = "--check" in argv
    data, inc = build()
    if not check_only:
        OUT_JSON.write_text(json.dumps(data, indent=1) + "\n")
        OUT_DOC.write_text(render_doc(data))
        OUT_INC.write_text(inc)
    totals = data["totals"]
    closure = data["closure"]
    print(f"{'checked' if check_only else 'wrote'} entry closure: entries={totals['entries']} "
          f"start={totals['function_start']} interior={totals['function_interior']} plt={totals['plt']} "
          f"xdata={totals['executable_data']} data={totals['image_data']} outside={totals['outside_image']} "
          f"closure={closure['with_native_entry'] if closure else '?'}/"
          f"{closure['functions'] if closure else '?'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
