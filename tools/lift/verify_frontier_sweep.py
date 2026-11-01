#!/usr/bin/env python3
"""Verify a constructor-sweep result and emit ready-to-paste table rows.

Reads the JSON produced by the sweep (`{site: {site, operands, global, slot, target, seed}}`),
re-verifies each chain against the pinned image (slot qword and type-8 relocation addend), and prints
one `vtable_resolved_entries.hpp` row per verified site plus a summary. Nothing is written to the
repository.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--sweep", default="local/validation/indirect-frontier/ctor-sweep.json")
    args = parser.parse_args()

    path = ROOT / args.sweep
    if not path.is_file():
        raise SystemExit(f"missing sweep result {path}")
    payload = json.loads(path.read_text())
    rows = payload.get("new", payload)
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    relocs = d.parse(elf)["relocs"]

    verified = []
    rejected = []
    for key, row in sorted(rows.items(), key=lambda item: int(item[1]["site"], 16)):
        site = int(row["site"], 16)
        target = int(row["target"], 16)
        global_address = row.get("global")
        slot = row.get("slot")
        if global_address is None or slot is None:
            rejected.append((site, "missing global/slot"))
            continue
        address = int(global_address) + int(slot)
        try:
            value = struct.unpack("<Q", img.read(address, 8))[0]
        except Exception:
            rejected.append((site, "slot unreadable"))
            continue
        matching = [r for r in relocs if r["target"] == address and r["addend"] == value]
        if value != target:
            rejected.append((site, "slot qword mismatch"))
            continue
        if not matching:
            rejected.append((site, "no matching relocation"))
            continue
        verified.append((site, address, slot, target))

    for site, address, slot, target in verified:
        print(f"    {{0x{site:x}, kResolvedVtable_<vtable>, {slot:#x}, "
              f"kResolvedVtableTarget_{target:x}Rva}},  // slot {address:#x}")
    print(f"verified {len(verified)} of {len(rows)}; rejected {len(rejected)}")
    for site, reason in rejected:
        print(f"  rejected 0x{site:x}: {reason}")
    return 0 if not rejected else 1


if __name__ == "__main__":
    raise SystemExit(main())
