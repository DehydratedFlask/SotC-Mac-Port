#!/usr/bin/env python3
"""Checker: docs/G1_EVIDENCE.md's measured block must match the live tree.

The G1 pack is an evidence-honesty document, so its numbers are regenerated, not remembered.
This wrapper runs `tools/lift/g1_evidence_pack.py --check`, which re-derives the startup coverage,
the indirect-frontier grade split and the graded-header count from `native/` and (when present)
the pinned ELF, and fails when the document disagrees.

A failure means one of two things, and the message says which:
  * a constructor row was registered or a frontier row added -> run
    `python3 tools/lift/g1_evidence_pack.py --write` and update the prose that quotes the number;
  * the document was edited by hand -> put the measurement back.
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GENERATOR = ROOT / "tools/lift/g1_evidence_pack.py"
DOC = ROOT / "docs/G1_EVIDENCE.md"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    require(DOC.is_file(), "docs/G1_EVIDENCE.md missing")
    require(GENERATOR.is_file(), "tools/lift/g1_evidence_pack.py missing")
    completed = subprocess.run([sys.executable, "-I", str(GENERATOR), "--check"],
                               capture_output=True, text=True, check=False)
    require(completed.returncode == 0, (completed.stdout + completed.stderr).strip())
    print(completed.stdout.strip() or "OK docs/G1_EVIDENCE.md matches the tree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
