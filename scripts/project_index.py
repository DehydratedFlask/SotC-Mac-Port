#!/usr/bin/env python3
"""Index every project folder without reading game bytes; source symbols remain in Graft."""
import argparse
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GENERATED = {"docs/FOLDERS.md", "local/indexes/folders.json"}
ROLES = {
    ".": "Project entry points and agent state",
    "docs": "Plans, task/evidence registries, checkpoint and escalation documentation",
    "scripts": "Small project and offline analysis helpers",
    "tools": "Out-of-tree tool adapters and reproducible analysis recipes",
    "reference": "Pinned upstream source; read-only; symbols indexed by Graft",
    "graft": "Regenerable source graph; internal cache directories summarized",
    "local": "Private extracted assets, binary analysis, manifests, builds and evidence",
    "logs": "Private command/build/install/extraction logs",
    "Shadow of the Colossus": "Original user game inputs; read-only and private",
    "native": "Native game implementation; verify module README and task ownership",
    "launcher": "Native Swift macOS launcher",
    "tests": "Native fixtures and real-game validation scenarios",
}
PRIVATE = {"local", "logs", "Shadow of the Colossus"}


def inventory(root):
    records = []
    for current, directories, files in os.walk(root, followlinks=False):
        directories[:] = sorted(d for d in directories if d not in {".git", "__pycache__"})
        relative = Path(current).relative_to(root).as_posix()
        top = relative.split("/")[0]
        if top == "graft":
            directories[:] = []  # The source graph has its own folder/symbol index.
        visible = [name for name in files if name != ".DS_Store"
                   and (Path(relative) / name).as_posix() not in GENERATED]
        records.append({"path": relative, "files": len(visible),
                        "role": ROLES.get(top, "Project folder; add a contract to docs/INDEX.md"),
                        "private": top in PRIVATE})
    return sorted(records, key=lambda row: row["path"])


def render(records):
    rows = ["# Generated project folder index", "",
            "Regenerate: `python3 scripts/project_index.py`. Check: add `--check`.",
            "Every non-administrative folder is recorded in `local/indexes/folders.json`.",
            "Private game/analysis/log paths are summarized here; Graft indexes source symbols.",
            "`.git`, Finder metadata, Python caches and Graft internal caches are summarized by their owner.", "",
            "| Folder | Immediate files | Contract |", "|---|---:|---|"]
    for row in records:
        if row["private"] and "/" in row["path"]:
            continue
        rows.append(f'| `{row["path"]}` | {row["files"]} | {row["role"]} |')
    rows += ["", "Private recursive inventory: `local/indexes/folders.json` (excluded from publication).",
             "Canonical navigation and ownership: [INDEX](INDEX.md).", ""]
    return "\n".join(rows)


def generate(root, check=False):
    # Create output parents first so their existence is part of the stable inventory.
    if not check:
        (root / "local/indexes").mkdir(parents=True, exist_ok=True)
        (root / "docs").mkdir(exist_ok=True)
    records = inventory(root)
    outputs = {"docs/FOLDERS.md": render(records),
               "local/indexes/folders.json": json.dumps({"schema": 1, "folders": records}, indent=2) + "\n"}
    stale = []
    for name, content in outputs.items():
        path = root / name
        if check:
            if not path.exists() or path.read_text() != content:
                stale.append(name)
        else:
            path.write_text(content)
    if stale:
        raise ValueError("Stale indexes: " + ", ".join(stale))
    return len(records)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    try:
        print(f'{generate(ROOT, args.check)} folders indexed; ' + ("current" if args.check else "written"))
    except (OSError, ValueError) as error:
        parser.exit(1, str(error) + "\n")
