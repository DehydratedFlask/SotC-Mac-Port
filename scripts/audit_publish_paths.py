#!/usr/bin/env python3
"""Fail if staged (or listed) paths look like forbidden game / private content.

Usage:
  python3 -I scripts/audit_publish_paths.py           # audit `git diff --cached --name-only`
  python3 -I scripts/audit_publish_paths.py --all     # audit every tracked file
  python3 -I scripts/audit_publish_paths.py PATH...   # audit explicit paths
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

FORBIDDEN_PREFIXES = (
    "Shadow of the Colossus/",
    "local/",
    "reference/",
    "logs/",
    "graft/",
    ".graft/",
    "AgentStores/",
)

FORBIDDEN_NAME_RE = re.compile(
    r"(?i)("
    r"eboot\.bin$"
    r"|\.pkg$"
    r"|\.psarc$"
    r"|\.self$"
    r"|\.sprx$"
    r"|\.elf$"
    r"|(^|/)libSce[^/]*\.sprx$"
    r"|(^|/)credentials"
    r"|(^|/)\.env(\.|$)"
    r")"
)

# Directory names forbidden at ANY depth (case-insensitive component match).
FORBIDDEN_DIR_COMPONENTS = frozenset(
    {
        "shadow of the colossus",
        "local",
        "agentstores",
        ".graft",
    }
)

# Allowlisted top-level roots for a publish tree.
ALLOWED_TOP = frozenset(
    {
        ".gitignore",
        ".ignore",
        "README.md",
        "ROADMAP.md",
        "HANDOFF.md",
        "AGENTS.md",
        "INSTALL_LOG.md",
        "LICENSE",
        "LICENSE.md",
        "native",
        "launcher",
        "docs",
        "scripts",
        "tools",
    }
)


def git_names(mode: str) -> list[str]:
    if mode == "staged":
        cmd = ["git", "diff", "--cached", "--name-only", "-z"]
    else:
        cmd = ["git", "ls-files", "-z"]
    out = subprocess.check_output(cmd, cwd=ROOT)
    if not out:
        return []
    return [p.decode() for p in out.split(b"\0") if p]


def violations(paths: list[str]) -> list[str]:
    bad: list[str] = []
    for raw in paths:
        p = raw.replace("\\", "/")
        while p.startswith("./"):
            p = p[2:]
        if not p:
            continue
        if ".." in p.split("/"):
            bad.append(f"path traversal: {p}")
            continue
        top = p.split("/", 1)[0]
        if top not in ALLOWED_TOP and p not in ALLOWED_TOP:
            bad.append(f"outside allowlist: {p}")
            continue
        nested = next(
            (c for c in p.split("/")[:-1] if c.lower() in FORBIDDEN_DIR_COMPONENTS),
            None,
        )
        if nested is not None:
            bad.append(f"forbidden nested directory {nested!r}: {p}")
            continue
        for pref in FORBIDDEN_PREFIXES:
            if p == pref.rstrip("/") or p.startswith(pref):
                bad.append(f"forbidden prefix: {p}")
                break
        else:
            if FORBIDDEN_NAME_RE.search(p):
                bad.append(f"forbidden name: {p}")
            # Block accidental binary dumps under tools/docs even if extension slips through.
            if p.startswith("local/"):
                bad.append(f"forbidden local/: {p}")
    return bad


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--all", action="store_true", help="audit all tracked files")
    ap.add_argument("paths", nargs="*", help="explicit paths to audit")
    args = ap.parse_args()

    if args.paths:
        paths = args.paths
    elif args.all:
        paths = git_names("tracked")
    else:
        paths = git_names("staged")
        if not paths:
            print("audit_publish_paths: nothing staged; pass --all or PATH...", file=sys.stderr)
            return 2

    bad = violations(paths)
    if bad:
        print("PUBLISH PATH AUDIT FAILED:", file=sys.stderr)
        for line in bad:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print(f"PUBLISH PATH AUDIT OK ({len(paths)} path(s))")
    for p in sorted(paths):
        print(f"  {p}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
