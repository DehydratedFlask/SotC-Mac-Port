#!/usr/bin/env python3
"""Report which bytes of Init 0x531d60 are modelled by native modules.

Reads the ELF-pinned RVA/size constants declared in `native/core/*.hpp`, intersects them with
the requested address window, and prints the merged coverage plus every gap. This is a
*bookkeeping* check: a covered byte means some native module claims that RVA, not that the
module implements the instruction's semantics.

Usage:
  python3 -I tools/lift/report_init_coverage.py                 # default window
  python3 -I tools/lift/report_init_coverage.py --start 0x531d60 --end 0x532b0c
  python3 -I tools/lift/report_init_coverage.py --json
"""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CORE = ROOT / "native/core"

RVA_RE = re.compile(r"\bk([A-Za-z0-9_]+)Rva\s*=\s*(0x[0-9A-Fa-f]+)")
SIZE_RE = re.compile(r"\bk([A-Za-z0-9_]+)Size\s*=\s*(\d+)")
DEFAULT_START = 0x531D60
DEFAULT_END = 0x532B0C  # start of the already-recovered registry fragment


def collect_spans() -> list[dict]:
    spans: list[dict] = []
    for path in sorted(CORE.glob("*.hpp")):
        text = path.read_text(errors="ignore")
        rvas = dict(RVA_RE.findall(text))
        sizes = dict(SIZE_RE.findall(text))
        for key, rva_hex in rvas.items():
            if key not in sizes:
                continue
            size = int(sizes[key])
            if size <= 0:
                continue
            spans.append(
                {
                    "name": key,
                    "file": path.name,
                    "start": int(rva_hex, 16),
                    "size": size,
                }
            )
    return spans


def merge(spans: list[dict], start: int, end: int) -> list[tuple[int, int]]:
    clipped = []
    for s in spans:
        a = max(s["start"], start)
        b = min(s["start"] + s["size"], end)
        if a < b:
            clipped.append((a, b))
    clipped.sort()
    merged: list[tuple[int, int]] = []
    for a, b in clipped:
        if merged and a <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(merged[-1][1], b))
        else:
            merged.append((a, b))
    return merged


def gaps(merged: list[tuple[int, int]], start: int, end: int) -> list[tuple[int, int]]:
    out: list[tuple[int, int]] = []
    cursor = start
    for a, b in merged:
        if a > cursor:
            out.append((cursor, a))
        cursor = max(cursor, b)
    if cursor < end:
        out.append((cursor, end))
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--start", default=hex(DEFAULT_START))
    ap.add_argument("--end", default=hex(DEFAULT_END))
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()
    start = int(args.start, 0)
    end = int(args.end, 0)
    if end <= start:
        raise SystemExit("FAIL end must be greater than start")

    spans = collect_spans()
    merged = merge(spans, start, end)
    hole = gaps(merged, start, end)
    covered = sum(b - a for a, b in merged)
    total = end - start
    report = {
        "window": {"start": hex(start), "end": hex(end), "bytes": total},
        "covered_bytes": covered,
        "coverage_percent": round(100.0 * covered / total, 2),
        "merged_spans": [[hex(a), hex(b)] for a, b in merged],
        "gaps": [[hex(a), hex(b), b - a] for a, b in hole],
        "declared_spans_considered": len(
            [s for s in spans if start <= s["start"] < end]
        ),
    }
    if args.json:
        print(json.dumps(report, indent=2))
    else:
        print(
            f"Init coverage {report['window']['start']}..{report['window']['end']} "
            f"({total} B): {covered} B modelled, {report['coverage_percent']}%"
        )
        print(f"merged spans: {len(merged)}; gaps: {len(hole)}")
        for gap in report["gaps"]:
            print(f"  GAP {gap[0]}..{gap[1]} ({gap[2]} B)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
