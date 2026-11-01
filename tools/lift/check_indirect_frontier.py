#!/usr/bin/env python3
"""Checker: docs/indirect_frontier.json must match a fresh indirect-frontier run.

The full report needs the private disassembly (`local/validation/registry-analysis-agent/all.dis`),
so this checker skips cleanly when that input is absent, like the other private-input checkers.
"""

from __future__ import annotations

import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOL = ROOT / "tools/lift/indirect_frontier.py"
COMMITTED = ROOT / "docs/indirect_frontier.json"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
DOC = ROOT / "docs/INDIRECT_FRONTIER.md"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    require(COMMITTED.is_file(), "docs/indirect_frontier.json missing")
    require(DOC.is_file(), "docs/INDIRECT_FRONTIER.md missing")
    if not DIS.is_file():
        print("SKIP check_indirect_frontier: private disassembly not present")
        return 0

    with tempfile.TemporaryDirectory() as temp:
        target = Path(temp) / "summary.json"
        completed = subprocess.run(
            [sys.executable, "-I", str(TOOL), "--seed", "0x531d60",
             "--summary-json", str(target), "--json", str(Path(temp) / "full.json")],
            capture_output=True, text=True, check=False, cwd=ROOT, env=os.environ.copy())
        require(completed.returncode == 0,
                (completed.stdout + completed.stderr).strip()[:400])
        require(target.is_file(), "tool produced no summary")
        if target.read_text() != COMMITTED.read_text():
            print("FAIL summary drift: rerun tools/lift/indirect_frontier.py --summary-json "
                  "docs/indirect_frontier.json")
            return 1

    import json
    document = json.loads(COMMITTED.read_text())
    require(document["closure_functions"] == 608, "closure function count changed")
    require(len(document["seeds"]) == 1, "seed list changed")
    print(
        "OK check_indirect_frontier: %d closure functions, %d frontier functions, %d sites, "
        "%d resolved" % (document["closure_functions"], document["frontier_functions"],
                         document["indirect_sites_in_closure"], document["resolved_targets"]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
