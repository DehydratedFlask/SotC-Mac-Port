#!/usr/bin/env python3
"""Use the pinned reference's plaintext SELF reader; never decrypt or run game code."""
import argparse
import hashlib
import json
from pathlib import Path
import runpy


def unwrap(source, destination):
    reader = Path(__file__).resolve().parents[1] / "reference/ShadowOfTheColossus-PC/scripts/prepare.py"
    data = source.read_bytes()
    elf, header, programs, segments, missing = runpy.run_path(str(reader), run_name="sotc_reader")["parse_self"](data)
    if destination.exists() and destination.read_bytes() != elf:
        raise ValueError(f"refusing to replace a different existing artifact: {destination}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(elf)
    record = {
        "source": str(source.resolve()), "source_bytes": len(data),
        "source_sha256": hashlib.sha256(data).hexdigest(),
        "elf": str(destination.resolve()), "elf_bytes": len(elf),
        "elf_sha256": hashlib.sha256(elf).hexdigest(),
        "entry": hex(header[4]), "elf_type": hex(header[1]), "machine": header[2],
        "program_headers": programs, "self_segments": segments, "missing_segments": missing,
        "reader": str(reader), "limitation": "Plaintext segment reconstruction only; no code execution or source recovery."
    }
    destination.with_suffix(".identity.json").write_text(json.dumps(record, indent=2) + "\n")
    return record


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    args = parser.parse_args()
    try:
        print(json.dumps(unwrap(args.source, args.destination), indent=2))
    except (ValueError, OSError, KeyError, IndexError) as error:
        parser.exit(1, f"SELF reconstruction failed: {error}\n")
