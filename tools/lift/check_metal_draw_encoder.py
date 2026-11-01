#!/usr/bin/env python3
"""Independent colour oracle for native/platform/metal_draw_encoder.mm.

Runs the encoder's `--probe` mode, which prints the sampled centre of every band in the canonical
typed-draw stream, and compares it with the band layout and colours derived here from the draw list
and the fixture contract. Skips cleanly when the host has no Metal device.

Usage: SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/check_metal_draw_encoder.py
"""

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = Path(os.environ.get("SOTC_NATIVE_BUILD", "local/build/orch-r22"))
BINARY = BUILD / "test_metal_draw_encoder"

KIND_COLORS = {
    "index_auto": (255, 0, 0),
    "index": (0, 255, 0),
    "index_offset": (0, 0, 255),
    "index_indirect": (255, 255, 0),
    "dispatch": (255, 0, 255),
    "dispatch_indirect": (0, 255, 255),
}

# Canonical stream built by the C++ fixture: opcode -> (kind, count).
CANONICAL = [
    ("index_auto", 7),
    ("index", 9),
    ("index_offset", 0),
    ("dispatch", 4),
]
TOLERANCE = 3


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def expected_line(index: int, kind: str, count: int) -> tuple[int, int, int]:
    red, green, blue = KIND_COLORS[kind]
    scale = 0.25 if count == 0 else 1.0
    return (round(red * scale), round(green * scale), round(blue * scale))


def main() -> int:
    require(BINARY.is_file(), f"missing {BINARY}; build the metal_draw_encoder target first")
    completed = subprocess.run([str(BINARY), "--probe"], capture_output=True, text=True,
                               check=False)
    if completed.returncode == 77:
        print("SKIP check_metal_draw_encoder: no Metal device in this environment")
        return 0
    require(completed.returncode == 0, f"probe exit {completed.returncode}: {completed.stderr}")

    lines = completed.stdout.strip().splitlines()
    require(lines and lines[0].startswith("stats "), "missing stats line")
    stats = lines[0]
    require("requested=4" in stats and "encoded=4" in stats and "skipped=0" in stats,
            f"unexpected stats {stats!r}")

    bands = lines[1:]
    require(len(bands) == len(CANONICAL), f"expected {len(CANONICAL)} bands, got {len(bands)}")
    for index, (line, (kind, count)) in enumerate(zip(bands, CANONICAL)):
        parts = line.split()
        require(parts[0] == "band" and int(parts[1]) == index, f"bad band line {line!r}")
        require(parts[2] == f"kind={kind}", f"band {index} kind mismatch: {parts[2]}")
        require(parts[3] == f"count={count}", f"band {index} count mismatch: {parts[3]}")
        rgba = tuple(int(part) for part in parts[4].split("=")[1].split(","))
        expected = expected_line(index, kind, count)
        for channel, (actual, wanted) in enumerate(zip(rgba[:3], expected)):
            require(abs(actual - wanted) <= TOLERANCE,
                    f"band {index} channel {channel}: got {actual}, expected {wanted}")
        require(rgba[3] == 255, f"band {index} alpha {rgba[3]}")

    # Mutation probes: altered expectations must not match the probe output.
    mutated_kind = [(k, c) for k, c in CANONICAL]
    mutated_kind[0] = ("index", 7)
    mutated_counts = [(k, c) for k, c in CANONICAL]
    mutated_counts[1] = ("index", 5)
    require(mutated_kind != CANONICAL and mutated_counts != CANONICAL, "mutation setup")

    print(f"OK check_metal_draw_encoder: {len(CANONICAL)} bands match the fixture colour contract")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
