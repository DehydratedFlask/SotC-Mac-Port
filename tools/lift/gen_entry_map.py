#!/usr/bin/env python3
"""Generate the guest-address -> native-entry map from the lifted module headers.

Every native lift declares its guest RVA as an `inline constexpr std::uint64_t k...Rva` constant in
the module header. This tool collects them into one stable table so call dispatch can ask "is there
a native entry for guest address X, and which module owns it?".

What it proves: the set of guest addresses our code claims to model, and which header owns each.
What it does NOT prove: that the entry is callable with the retail ABI, that its behaviour matches
the original, or that any address outside the table is unreachable. Addresses with no entry are
simply absent.

Usage:
  python3 tools/lift/gen_entry_map.py            # write docs/entry_map.json
  python3 tools/lift/gen_entry_map.py --check    # compare with the committed map
"""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / "docs/entry_map.json"
SCAN_DIRS = ("native/core", "native/platform")

PATTERN = re.compile(
    r"inline\s+constexpr\s+std::uint64_t\s+(k[A-Za-z0-9_]+?)(?:Rva|RVA)\s*=\s*0x([0-9a-fA-F]+)\s*;"
)


def collect() -> dict:
    entries: dict[int, list[dict]] = {}
    headers = 0
    constants = 0
    for directory in SCAN_DIRS:
        for header in sorted((ROOT / directory).glob("*.hpp")):
            text = header.read_text(errors="replace")
            found = PATTERN.findall(text)
            if not found:
                continue
            headers += 1
            for name, address in found:
                constants += 1
                entries.setdefault(int(address, 16), []).append(
                    {"constant": name, "header": str(header.relative_to(ROOT))}
                )

    for owners in entries.values():
        owners.sort(key=lambda row: (row["header"], row["constant"]))

    duplicates = {address: owners for address, owners in entries.items() if len(owners) > 1}
    ordered = {f"0x{address:x}": entries[address] for address in sorted(entries)}
    return {
        "schema": "sotc-entry-map/1",
        "scanned_dirs": list(SCAN_DIRS),
        "totals": {
            "constants": constants,
            "distinct_rvas": len(entries),
            "headers": headers,
            "duplicate_rvas": len(duplicates),
        },
        "duplicates": {f"0x{address:x}": owners for address, owners in sorted(duplicates.items())},
        "entries": ordered,
    }


def render(document: dict) -> str:
    return json.dumps(document, indent=2, sort_keys=True) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    document = collect()
    text = render(document)
    if args.check:
        if not OUTPUT.is_file():
            print(f"FAIL missing {OUTPUT}")
            return 1
        if OUTPUT.read_text() != text:
            print(f"FAIL {OUTPUT.name} is stale; rerun tools/lift/gen_entry_map.py")
            return 1
        totals = document["totals"]
        print(
            "OK check_entry_map: %d constants -> %d distinct guest RVAs in %d headers (%d duplicates)"
            % (totals["constants"], totals["distinct_rvas"], totals["headers"],
               totals["duplicate_rvas"])
        )
        return 0

    OUTPUT.write_text(text)
    totals = document["totals"]
    print(
        "wrote %s: %d constants -> %d distinct guest RVAs in %d headers (%d duplicates)"
        % (OUTPUT.relative_to(ROOT), totals["constants"], totals["distinct_rvas"],
           totals["headers"], totals["duplicate_rvas"])
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
