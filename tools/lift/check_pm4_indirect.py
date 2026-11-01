#!/usr/bin/env python3
"""Independent traversal oracle for native/platform/pm4_indirect.{hpp,cpp}.

Generates scenarios (policy table, nested buffers, cycles, truncations), traverses them here in
Python from the documented rules, runs the native `--traverse` mode over the same scenario and
requires identical draw lists and statistics. Mutation probe: a deliberately altered expectation
must be rejected.

Usage: SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/check_pm4_indirect.py
"""

from __future__ import annotations

import os
import random
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = Path(os.environ.get("SOTC_NATIVE_BUILD", "local/build/orch-r22"))
BINARY = BUILD / "test_pm4_indirect"

TYPE_NOP = 0
TYPE_REG = 1
TYPE_FILL = 2
TYPE_CMD = 3

KIND_NAMES = {
    0: "none",
    1: "index_auto",
    2: "index",
    3: "index_offset",
    4: "index_indirect",
    5: "dispatch",
    6: "dispatch_indirect",
}

MAX_DEPTH = 4
MAX_BUFFERS = 64
MAX_DRAWS = 4096


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def packet(opcode: int, payload: list[int]) -> list[int]:
    count = len(payload)
    return [(TYPE_CMD << 30) | (((count - 1) & 0x3FFF) << 16) | ((opcode & 0xFF) << 8)] + payload


def nop() -> list[int]:
    return [0]


def seq(*parts: list[int]) -> list[int]:
    """Flatten packet/nop fragments into one word stream."""
    words: list[int] = []
    for part in parts:
        words.extend(part)
    return words


def frame(words: list[int]) -> tuple[list[tuple[int, int, int, int]], str, int]:
    """Return (packets, status, stop) where packets are (opcode, count, payload_start, header_index)."""
    packets: list[tuple[int, int, int, int]] = []
    index = 0
    while index < len(words):
        header = words[index]
        ptype = header >> 30
        if ptype == TYPE_NOP:
            if (header >> 16) & 0x3FFF:
                return packets, "reserved_bits_set", index
            index += 1
            continue
        count = ((header >> 16) & 0x3FFF) + 1
        if index + 1 >= len(words):
            return packets, "truncated_header", index
        if count > len(words) - (index + 1):
            return packets, "truncated_payload", index
        if ptype == TYPE_CMD:
            packets.append(((header >> 8) & 0xFF, count, index + 1, index))
        index += 1 + count
    return packets, "ok", index


class Scenario:
    def __init__(self, policy: dict[int, dict], buffers: dict[int, list[int]], root_addr: int,
                 root: list[int]) -> None:
        self.policy = policy
        self.buffers = buffers
        self.root_addr = root_addr
        self.root = root

    def to_text(self) -> str:
        lines = [f"policy {len(self.policy)}"]
        for opcode, rule in self.policy.items():
            lines.append(
                f"{opcode} {rule['kind']} {rule['count_field']} {1 if rule['indirect'] else 0} "
                f"{rule['address_field']} {rule['size_field']}"
            )
        for address, words in self.buffers.items():
            lines.append(f"buffer {address:x} {len(words)}")
            lines.append(" ".join(f"{word:x}" for word in words))
        lines.append(f"root {self.root_addr:x} {len(self.root)}")
        lines.append(" ".join(f"{word:x}" for word in self.root))
        return "\n".join(lines) + "\n"


def traverse_py(scenario: Scenario) -> tuple[list[tuple[str, int, int, int]], dict, str]:
    draws: list[tuple[str, int, int, int]] = []
    stats = dict(buffers=1, indirects=0, draws=0, cycles=0, unreadable=0, maxdepth=0)
    status = "ok"

    def walk(words: list[int], address: int, depth: int, path: list[int]) -> bool:
        nonlocal status
        packets, decode_status, _ = frame(words)
        if decode_status != "ok":
            status = "decode_error"
            return False
        for opcode, count, payload_start, _header in packets:
            rule = scenario.policy.get(opcode)
            if rule is None:
                continue
            if rule["indirect"]:
                stats["indirects"] += 1
                addr_field = rule["address_field"]
                size_field = rule["size_field"]
                if addr_field == 0xFF or size_field == 0xFF:
                    status = "invalid_policy"
                    return False
                high = addr_field + 1
                if size_field >= count or high >= count:
                    status = "bad_indirect_payload"
                    return False
                if depth >= MAX_DEPTH:
                    status = "depth_exceeded"
                    return False
                target = (words[payload_start + addr_field] |
                          (words[payload_start + high] << 32))
                size = words[payload_start + size_field]
                if size == 0:
                    continue
                if target in path:
                    stats["cycles"] += 1
                    status = "cycle_detected"
                    return False
                if stats["buffers"] >= MAX_BUFFERS:
                    status = "depth_exceeded"
                    return False
                nested = scenario.buffers.get(target)
                if nested is None or len(nested) < size:
                    stats["unreadable"] += 1
                    status = "unreadable_indirect"
                    return False
                stats["buffers"] += 1
                child_depth = depth + 1
                stats["maxdepth"] = max(stats["maxdepth"], child_depth)
                if not walk(nested[:size], target, child_depth, path + [target]):
                    return False
                continue
            if rule["kind"] == KIND_NAMES[0]:
                continue
            if stats["draws"] >= MAX_DRAWS:
                status = "decode_error"
                return False
            count_field = rule["count_field"]
            draw_count = words[payload_start + count_field] if count_field != 0xFF and count_field < count else 0
            draws.append((KIND_NAMES[rule["kind"]], draw_count, depth, address))
            stats["draws"] += 1
        return True

    walk(scenario.root, scenario.root_addr, 0, [scenario.root_addr])
    return draws, stats, status


def run_native(scenario: Scenario) -> tuple[list[tuple[str, int, int, int]], dict, str]:
    with tempfile.NamedTemporaryFile("w", suffix=".pm4", delete=False) as handle:
        handle.write(scenario.to_text())
        path = handle.name
    try:
        completed = subprocess.run([str(BINARY), "--traverse", path], capture_output=True,
                                   text=True, check=False)
        require(completed.returncode == 0, f"traverse exit {completed.returncode}: {completed.stderr}")
        lines = completed.stdout.strip().splitlines()
    finally:
        os.unlink(path)

    require(lines and lines[0].startswith("draws "), "missing draws header")
    declared = int(lines[0].split()[1])
    draws: list[tuple[str, int, int, int]] = []
    for line in lines[1:-1]:
        parts = line.split()
        require(parts[0] == "draw", f"bad draw line {line!r}")
        draws.append((parts[1], int(parts[2]), int(parts[3]), int(parts[4])))
    require(len(draws) == declared, "declared draw count mismatch")
    stats_line = lines[-1]
    require(stats_line.startswith("stats "), "missing stats line")
    stats = {
        "buffers": int(stats_line.split("buffers=")[1].split()[0]),
        "indirects": int(stats_line.split("indirects=")[1].split()[0]),
        "draws": int(stats_line.split("draws=")[1].split()[0]),
        "cycles": int(stats_line.split("cycles=")[1].split()[0]),
        "unreadable": int(stats_line.split("unreadable=")[1].split()[0]),
        "maxdepth": int(stats_line.split("maxdepth=")[1].split()[0]),
    }
    status = stats_line.split("status=")[1].strip()
    return draws, stats, status


def rules_default() -> dict[int, dict]:
    return {
        0x10: {"kind": 0, "count_field": 0xFF, "indirect": True, "address_field": 0, "size_field": 2},
        0x2D: {"kind": 1, "count_field": 0, "indirect": False, "address_field": 0xFF,
               "size_field": 0xFF},
        0x2E: {"kind": 2, "count_field": 0, "indirect": False, "address_field": 0xFF,
               "size_field": 0xFF},
        0x31: {"kind": 5, "count_field": 0, "indirect": False, "address_field": 0xFF,
               "size_field": 0xFF},
    }


def scenarios() -> list[Scenario]:
    policy = rules_default()
    out: list[Scenario] = []

    # Flat stream with a nop between packets.
    out.append(Scenario(policy, {}, 0x1000,
                        seq(packet(0x2D, [3]), nop(), packet(0x2E, [4]), packet(0x31, [1, 1, 1]))))

    # One level of indirection, draw order preserved.
    out.append(Scenario(policy, {0x2000: packet(0x2E, [9])}, 0x1000,
                        seq(packet(0x2D, [1]), packet(0x10, [0x2000, 0, 2]), packet(0x2D, [2]))))

    # Two nested levels.
    out.append(Scenario(policy,
                        {0x2000: packet(0x10, [0x3000, 0, 2]), 0x3000: packet(0x2D, [7])},
                        0x1000, packet(0x10, [0x2000, 0, 4])))

    # Cycle: 0x2000 -> 0x3000 -> 0x2000.
    out.append(Scenario(policy,
                        {0x2000: packet(0x10, [0x3000, 0, 4]),
                         0x3000: packet(0x10, [0x2000, 0, 4])},
                        0x1000, packet(0x10, [0x2000, 0, 4])))

    # Self-cycle.
    out.append(Scenario(policy, {0x2000: packet(0x10, [0x2000, 0, 4])}, 0x1000,
                        packet(0x10, [0x2000, 0, 4])))

    # Unreadable nested buffer.
    out.append(Scenario(policy, {}, 0x1000, packet(0x10, [0xDEAD, 0, 2])))

    # Indirect payload too short for its own layout.
    out.append(Scenario(policy, {}, 0x1000, packet(0x10, [0x2000])))

    # Nested buffer that does not frame cleanly.
    out.append(Scenario(policy, {0x2000: [packet(0x2D, [1, 2, 3])[0]]}, 0x1000,
                        packet(0x10, [0x2000, 0, 1])))

    # Depth chain longer than the limit (default 4): root + 5 nested levels.
    deep_buffers = {}
    for level in range(5):
        address = 0x4000 + level * 0x100
        target = 0x4000 + (level + 1) * 0x100
        deep_buffers[address] = packet(0x10, [target, 0, 4])
    out.append(Scenario(policy, deep_buffers, 0x1000, packet(0x10, [0x4000, 0, 4])))

    # Randomised graphs.
    rng = random.Random(0x1D1EC7)
    for _ in range(10):
        buffers: dict[int, list[int]] = {}
        for index in range(rng.randint(1, 4)):
            address = 0x5000 + index * 0x100
            words: list[int] = []
            for _ in range(rng.randint(1, 3)):
                choice = rng.choice(["draw", "draw", "dispatch", "indirect", "nop"])
                if choice == "nop":
                    words += nop()
                elif choice == "draw":
                    words += packet(rng.choice([0x2D, 0x2E]), [rng.randint(0, 500)])
                elif choice == "dispatch":
                    words += packet(0x31, [rng.randint(1, 8), 1, 1])
                else:
                    target = rng.choice(list(buffers.keys()) + [0x5000 + index * 0x100])
                    words += packet(0x10, [target, 0, 2])
            buffers[address] = words
        root: list[int] = []
        for _ in range(rng.randint(1, 3)):
            if rng.random() < 0.5 and buffers:
                root += packet(0x10, [rng.choice(list(buffers.keys())), 0, 2])
            else:
                root += packet(rng.choice([0x2D, 0x2E]), [rng.randint(0, 500)])
        out.append(Scenario(policy, buffers, 0x1000, root))

    return out


def main() -> int:
    require(BINARY.is_file(), f"missing {BINARY}; build the pm4_indirect target first")
    source = (ROOT / "native/platform/pm4_indirect.cpp").read_text()
    require("cycle_detected" in source and "depth_exceeded" in source,
            "traversal statuses present in source")

    checked = 0
    for scenario in scenarios():
        expected = traverse_py(scenario)
        actual = run_native(scenario)
        require(actual == expected,
                "traversal mismatch for scenario with %d buffers:\n  py  %r\n  c++ %r"
                % (len(scenario.buffers), expected, actual))
        checked += 1

    # Mutation probes: altered expectations must be rejected.
    scenario = scenarios()[2]
    draws, stats, status = traverse_py(scenario)
    require((draws[:-1], stats, status) != run_native(scenario), "draw mutation not rejected")
    altered = dict(stats)
    altered["buffers"] += 1
    require((draws, altered, status) != run_native(scenario), "stat mutation not rejected")
    require((draws, stats, status + "x") != run_native(scenario), "status mutation not rejected")

    print(f"OK check_pm4_indirect: {checked} scenarios cross-checked (python traversal == native)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
