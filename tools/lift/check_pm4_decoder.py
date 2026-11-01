#!/usr/bin/env python3
"""Independent framing oracle for native/platform/pm4_decoder.{hpp,cpp}.

Generates synthetic PM4 streams, frames them here in Python from the documented packet layout, runs
the native decoder's `--census` mode over the same bytes, and requires the two censuses to match.
Mutation probe: corrupt one generated stream's expectation and require a mismatch.

Usage: SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/check_pm4_decoder.py
"""

from __future__ import annotations

import os
import random
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BINARY = Path(os.environ.get("SOTC_NATIVE_BUILD", "local/build/orch-r22")) / "test_pm4_decoder"
HEADER = ROOT / "native/platform/pm4_decoder.hpp"
SOURCE = ROOT / "native/platform/pm4_decoder.cpp"

TYPE_NOP = 0
TYPE_REG = 1
TYPE_FILL = 2
TYPE_CMD = 3


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def frame(words: list[int], max_payload: int = 0x4000) -> tuple[str, int]:
    """Frame the stream exactly as the documented rules say; return (status, stop_index)."""
    stats = dict(packets=0, regs=0, cmds=0, fillers=0, nops=0, payload=0, cmd_dwords=0)
    if not words:
        return "empty_buffer", 0
    limit = 0x4000 if max_payload == 0 else max_payload
    index = 0
    while index < len(words):
        header = words[index]
        ptype = header >> 30
        if ptype == TYPE_NOP:
            if (header >> 16) & 0x3FFF:
                return "reserved_bits_set", index
            stats["nops"] += 1
            stats["packets"] += 1
            index += 1
            continue
        count = ((header >> 16) & 0x3FFF) + 1
        if count > limit:
            return "oversized_packet", index
        if index + 1 >= len(words):
            return "truncated_header", index
        if count > len(words) - (index + 1):
            return "truncated_payload", index
        if ptype == TYPE_REG:
            stats["regs"] += 1
        elif ptype == TYPE_FILL:
            stats["fillers"] += 1
        elif ptype == TYPE_CMD:
            stats["cmds"] += 1
            stats["cmd_dwords"] += count
        else:
            return "reserved_bits_set", index
        stats["payload"] += count
        stats["packets"] += 1
        index += 1 + count
    return "ok", index


def census_line(words: list[int]) -> str:
    status, stop = frame(words)
    stats = dict(packets=0, regs=0, cmds=0, fillers=0, nops=0, payload=0, cmd_dwords=0)
    # Recompute stats for the successful prefix only.
    if status in ("ok",):
        index = 0
        while index < len(words):
            header = words[index]
            ptype = header >> 30
            if ptype == TYPE_NOP:
                stats["nops"] += 1
                stats["packets"] += 1
                index += 1
                continue
            count = ((header >> 16) & 0x3FFF) + 1
            if ptype == TYPE_REG:
                stats["regs"] += 1
            elif ptype == TYPE_FILL:
                stats["fillers"] += 1
            else:
                stats["cmds"] += 1
                stats["cmd_dwords"] += count
            stats["payload"] += count
            stats["packets"] += 1
            index += 1 + count
    elif status in ("truncated_payload", "truncated_header", "oversized_packet", "reserved_bits_set"):
        index = 0
        while index < stop:
            header = words[index]
            ptype = header >> 30
            if ptype == TYPE_NOP:
                stats["nops"] += 1
                stats["packets"] += 1
                index += 1
                continue
            count = ((header >> 16) & 0x3FFF) + 1
            if ptype == TYPE_REG:
                stats["regs"] += 1
            elif ptype == TYPE_FILL:
                stats["fillers"] += 1
            else:
                stats["cmds"] += 1
                stats["cmd_dwords"] += count
            stats["payload"] += count
            stats["packets"] += 1
            index += 1 + count
    return (
        f"{stats['packets']} {stats['regs']} {stats['cmds']} {stats['fillers']} {stats['nops']} "
        f"{stats['payload']} {stats['cmd_dwords']} {status} {stop}"
    )


def run_native(words: list[int]) -> str:
    with tempfile.NamedTemporaryFile("w", suffix=".hex", delete=False) as handle:
        handle.write("\n".join(f"{word:08x}" for word in words))
        path = handle.name
    try:
        completed = subprocess.run([str(BINARY), "--census", path], capture_output=True, text=True,
                                   check=False)
        require(completed.returncode == 0, f"census exit {completed.returncode}: {completed.stderr}")
        return completed.stdout.strip()
    finally:
        os.unlink(path)


def packet(ptype: int, field: int, payload: list[int]) -> list[int]:
    count = len(payload)
    if ptype == TYPE_NOP:
        return [0]
    header = (ptype << 30) | (((count - 1) & 0x3FFF) << 16)
    if ptype == TYPE_CMD:
        header |= (field & 0xFF) << 8
    else:
        header |= field & 0xFFFF
    return [header] + payload


def generated_streams() -> list[list[int]]:
    rng = random.Random(0x504D34)  # "PM4"
    streams: list[list[int]] = [[], [0], [0, 0, 0]]
    for _ in range(12):
        words: list[int] = []
        for _ in range(rng.randint(1, 8)):
            kind = rng.choice([TYPE_NOP, TYPE_REG, TYPE_FILL, TYPE_CMD])
            if kind == TYPE_NOP:
                words += [0]
                continue
            payload = [rng.getrandbits(32) for _ in range(rng.randint(1, 5))]
            field = rng.getrandbits(16) if kind != TYPE_CMD else rng.getrandbits(8)
            words += packet(kind, field, payload)
        streams.append(words)
    # Truncated tail: a command header promising four dwords with only two present.
    streams.append(packet(TYPE_CMD, 0x10, [1, 2, 3, 4])[:-2])
    # Reserved bits in a type-0 header.
    streams.append([0x0001_0000, 0, 0])
    # Filler plus trailing NOP.
    streams.append(packet(TYPE_FILL, 0, [0, 0]) + [0])
    return streams


def main() -> int:
    require(BINARY.is_file(), f"missing {BINARY}; build the pm4_decoder target first")
    require("[29:16]" in HEADER.read_text() and "payload dwords minus one" in HEADER.read_text(),
            "count field position documented in header")
    require("0x3FFFu" in SOURCE.read_text(), "14-bit count mask present in the decoder")
    require("(header >> 8) & 0xFFu" in SOURCE.read_text(), "type-3 opcode extracted from bits 15:8")
    require("truncated_payload" in SOURCE.read_text(), "payload bounds check present")

    checked = 0
    for stream in generated_streams():
        expected = census_line(stream)
        actual = run_native(stream)
        require(actual == expected, f"census mismatch for {len(stream)} dwords:\n  py  {expected}\n  c++ {actual}")
        checked += 1

    # Mutation probe: a deliberately wrong expectation must not match.
    probe = generated_streams()[-1]
    require(run_native(probe) != census_line(probe)[:-1] + "1", "mutation probe was not rejected")

    print(f"OK check_pm4_decoder: {checked} streams cross-checked (python framing == native framing)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
