#!/usr/bin/env python3
"""Checker: docs/entry_map.json must match a fresh scan of the lifted module headers."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GENERATOR = ROOT / "tools/lift/gen_entry_map.py"
MAP = ROOT / "docs/entry_map.json"
DOC = ROOT / "docs/ENTRY_MAP.md"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    require(MAP.is_file(), "docs/entry_map.json missing; run tools/lift/gen_entry_map.py")
    require(DOC.is_file(), "docs/ENTRY_MAP.md missing")
    completed = subprocess.run([sys.executable, "-I", str(GENERATOR), "--check"],
                               capture_output=True, text=True, check=False)
    require(completed.returncode == 0, (completed.stdout + completed.stderr).strip())
    document_text = MAP.read_text()
    require('"distinct_rvas"' in document_text, "map missing totals")
    print(completed.stdout.strip())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
