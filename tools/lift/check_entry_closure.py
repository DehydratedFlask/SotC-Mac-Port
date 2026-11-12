#!/usr/bin/env python3
"""Re-derive the guest entry closure from the pinned ELF and verify the committed artifacts.

Runs `entry_closure.build()` (which checks the ELF sha256, parses `.eh_frame_hdr`, re-types every
RVA in `docs/entry_map.json` and intersects the 0x531d60 closure) and compares the result with
`docs/entry_closure.json` and `native/core/guest_entry_table.inc`. No execution, no network.

Prints SKIP (exit 0) when the private ELF is absent, like the other private-image checkers.
"""

from __future__ import annotations

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


def main() -> int:
    if not (ROOT / "local/re/eboot.elf").exists():
        print("SKIP check_entry_closure: private ELF absent")
        return 0
    data, inc = ec.build()

    stored = json.loads(ec.OUT_JSON.read_text())
    if stored != data:
        die("docs/entry_closure.json is stale; rerun tools/lift/entry_closure.py")
    if ec.OUT_INC.read_text() != inc:
        die("native/core/guest_entry_table.inc is stale; rerun tools/lift/entry_closure.py")

    totals = data["totals"]
    if sum(totals[k] for k in ec.KIND_ORDER) != totals["entries"]:
        die("kind counts do not sum to the entry count")
    if totals["entries"] != json.loads(ec.ENTRY_MAP.read_text())["totals"]["distinct_rvas"]:
        die("typed entries disagree with docs/entry_map.json")

    closure = data["closure"]
    print(f"OK check_entry_closure: entries={totals['entries']} start={totals['function_start']} "
          f"interior={totals['function_interior']} plt={totals['plt']} "
          f"xdata={totals['executable_data']} data={totals['image_data']} "
          f"outside={totals['outside_image']} "
          f"closure={closure['with_native_entry'] if closure else '?'}/"
          f"{closure['functions'] if closure else '?'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
