#!/usr/bin/env python3
"""Read-only inspector/decoder for a Sony `SShd` sample bank (`.hd` header + `.bd` body).

Scope. The container relations below held for all 25 `.hd`/`.bd` pairs audited in AUD-01 (E1009
corpus): a 16-byte wrapper `{u32 file_size, u32 body_size, u32 0, 'SShd'}`, then a 2,026-byte (or
larger) header whose sample directory is **not** decoded here. The body is 16-byte PSX/PS2 SPU
(VAG) ADPCM frames: `byte0 = filter<<4 | shift`, `byte1` = predictor high byte, `bytes 2..15` =
28 signed 4-bit nibbles, high nibble first. All 1,620,083 frames of the audited corpus satisfy
`shift <= 12` and `filter <= 4`, and the decode agrees with FFmpeg's independent `adpcm_psx`
decoder at correlation 0.99861 over 57,344 samples (not bit-exact; see docs/AUDIO_FORMAT.md).

This tool is READ-ONLY and keeps decoded samples in memory. It never writes PCM, never copies game
data into the repository, and refuses (`BankError`) on any structural mismatch:

  * `SShd` at offset 0x0C, `u32@0 == len(.hd)`, `u32@4 == len(.bd)`, `len(.bd) % 16 == 0`
  * every frame header within the PSX rule (`shift <= 12`, `filter <= 4`)

It does NOT know the sample rate (no rate field was proven in `.hd`), does not parse the sample
directory, does not decode `*.sxd`/`*.sxd1`/`*.sxd2`, and makes no playback claim.

Usage:
    python3 -I tools/dev/se_bank_inspect.py --self-test
    python3 -I tools/dev/se_bank_inspect.py --hd PATH.hd --bd PATH.bd [--max-frames N] [--json]

Exit codes: 0 all checks passed, 1 a check failed, 2 usage/IO.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

SHD_MAGIC = b"SShd"
SHD_MAGIC_OFFSET = 0x0C
FRAME_BYTES = 16
SAMPLES_PER_FRAME = 28
MAX_SHIFT = 12
MAX_FILTER = 4
FILTER_COEFFS = {
    0: (0, 0),
    1: (60, 0),
    2: (115, -52),
    3: (98, -55),
    4: (122, -60),
}


class BankError(ValueError):
    pass


def _u32(data: bytes, offset: int) -> int:
    if offset + 4 > len(data):
        raise BankError(f"truncated u32 at {offset:#x}")
    return struct.unpack_from("<I", data, offset)[0]


def _sgn4(nibble: int) -> int:
    return nibble - 16 if nibble >= 8 else nibble


def _clip16(value: int) -> int:
    return -32768 if value < -32768 else (32767 if value > 32767 else value)


def parse_header(hd: bytes, body_size: int) -> dict:
    """Validate the 16-byte `SShd` wrapper. Returns the declared sizes."""
    if len(hd) < 0x10:
        raise BankError(f"header too short: {len(hd)} bytes")
    if hd[SHD_MAGIC_OFFSET:SHD_MAGIC_OFFSET + 4] != SHD_MAGIC:
        raise BankError(
            f"missing SShd magic at {SHD_MAGIC_OFFSET:#x}: {hd[SHD_MAGIC_OFFSET:SHD_MAGIC_OFFSET + 4]!r}"
        )
    declared_file = _u32(hd, 0)
    declared_body = _u32(hd, 4)
    if declared_file != len(hd):
        raise BankError(f"header size field {declared_file} != actual {len(hd)}")
    if declared_body != body_size:
        raise BankError(f"body size field {declared_body} != actual {body_size}")
    return {"file_size": declared_file, "body_size": declared_body,
            "flags": _u32(hd, 8)}


def decode_frame(frame: bytes, hist1: int, hist2: int) -> tuple[list[int], int, int]:
    """Decode one 16-byte SPU ADPCM frame. Returns (28 samples, hist1, hist2)."""
    if len(frame) != FRAME_BYTES:
        raise BankError(f"frame is {len(frame)} bytes, expected {FRAME_BYTES}")
    header = frame[0]
    shift = header & 0x0F
    filt = header >> 4
    if shift > MAX_SHIFT:
        raise BankError(f"invalid ADPCM shift {shift} (max {MAX_SHIFT})")
    if filt > MAX_FILTER:
        raise BankError(f"invalid ADPCM filter {filt} (max {MAX_FILTER})")
    c0, c1 = FILTER_COEFFS[filt]
    out: list[int] = []
    for i in range(SAMPLES_PER_FRAME):
        byte = frame[2 + (i >> 1)]
        nibble = (byte >> 4) if (i & 1) == 0 else (byte & 0x0F)
        sample = (_sgn4(nibble) << 12) >> shift
        sample += (hist1 * c0 + hist2 * c1 + 32) >> 6
        sample = _clip16(sample)
        hist2, hist1 = hist1, sample
        out.append(sample)
    return out, hist1, hist2


def decode_body(body: bytes, max_frames: int | None = None) -> dict:
    """Decode a whole body. Fails closed on any invalid frame header."""
    if len(body) % FRAME_BYTES:
        raise BankError(f"body length {len(body)} is not a multiple of {FRAME_BYTES}")
    total = len(body) // FRAME_BYTES
    frames = total if max_frames is None else min(total, max_frames)
    hist1 = hist2 = 0
    peak = 0
    nonzero = 0
    shifts = [0] * (MAX_SHIFT + 1)
    filters = [0] * (MAX_FILTER + 1)
    samples: list[int] = []
    for index in range(frames):
        frame = body[index * FRAME_BYTES:(index + 1) * FRAME_BYTES]
        shift = frame[0] & 0x0F
        filt = frame[0] >> 4
        if shift > MAX_SHIFT or filt > MAX_FILTER:
            raise BankError(
                f"frame {index}: invalid header {frame[0]:#04x} (shift {shift}, filter {filt})"
            )
        shifts[shift] += 1
        filters[filt] += 1
        decoded, hist1, hist2 = decode_frame(frame, hist1, hist2)
        samples.extend(decoded)
        peak = max(peak, max(abs(v) for v in decoded))
        nonzero += sum(1 for v in decoded if v)
    return {
        "frames_total": total,
        "frames_decoded": frames,
        "samples": len(samples),
        "peak": peak,
        "nonzero": nonzero,
        "shift_histogram": {k: v for k, v in enumerate(shifts) if v},
        "filter_histogram": {k: v for k, v in enumerate(filters) if v},
        "samples_data": samples,
    }


class Report:
    def __init__(self) -> None:
        self.checks: list[dict[str, object]] = []

    def check(self, name: str, ok: bool, detail: str) -> bool:
        self.checks.append({"name": name, "ok": ok, "detail": detail})
        return ok

    @property
    def ok(self) -> bool:
        return all(c["ok"] for c in self.checks)


def inspect_pair(hd: bytes, body: bytes, report: Report, max_frames: int | None = None) -> dict:
    try:
        header = parse_header(hd, len(body))
        report.check("shd_magic", True,
                     f"SShd at {SHD_MAGIC_OFFSET:#x}; file {header['file_size']}, body {header['body_size']}")
        report.check("body_tiling", len(body) % FRAME_BYTES == 0,
                     f"{len(body)} bytes = {len(body) // FRAME_BYTES} x {FRAME_BYTES}")
    except BankError as exc:
        report.check("container", False, str(exc))
        return {"error": str(exc)}

    try:
        decoded = decode_body(body, max_frames)
        report.check("frame_headers", True,
                     f"{decoded['frames_decoded']} frames decoded; shift<=12, filter<=4")
    except BankError as exc:
        report.check("frame_headers", False, str(exc))
        return {"error": str(exc)}

    summary = {k: v for k, v in decoded.items() if k != "samples_data"}
    summary["codec"] = "spu_adpcm_16B"
    summary["sample_rate"] = None
    summary["sample_rate_note"] = "not stored in the proven .hd fields; not claimed"
    return summary


def self_test() -> int:
    report = Report()

    def build_hd(size: int, body: int) -> bytes:
        hd = bytearray(size)
        struct.pack_into("<I", hd, 0, size)
        struct.pack_into("<I", hd, 4, body)
        hd[SHD_MAGIC_OFFSET:SHD_MAGIC_OFFSET + 4] = SHD_MAGIC
        return bytes(hd)

    def frame(header: int, nibbles: list[int]) -> bytes:
        out = bytearray(FRAME_BYTES)
        out[0] = header
        out[1] = 0
        for i, n in enumerate(nibbles[:28]):
            if i & 1:
                out[2 + (i >> 1)] |= n & 0x0F
            else:
                out[2 + (i >> 1)] = (n & 0x0F) << 4
        return bytes(out)

    # filter 0 means "no prediction": each sample is the shifted nibble.
    nibbles = [1, 2, 3, 4, 15, 14, 2, 0] + [0] * 20                      # 15 == -1
    body = frame(0x0C, nibbles) + frame(0x08, [1, 2, 3] + [0] * 25)
    hd = build_hd(0x40, len(body))
    summary = inspect_pair(hd, body, report)
    expected = [1, 2, 3, 4, -1, -2, 2, 0] + [0] * 20 + [16, 32, 48] + [0] * 25
    report.check("synthetic_samples", summary.get("samples") == len(expected),
                 f"{summary.get('samples')} samples, expected {len(expected)}")

    decoded = decode_body(body)
    report.check("synthetic_values", decoded["samples_data"] == expected,
                 f"first 6 {decoded['samples_data'][:6]}, expected {expected[:6]}")

    # filter 1 feeds the previous sample back through 60/64 (no rounding drift check)
    carried = decode_body(frame(0x1C, [1] * 28))["samples_data"]
    report.check("filter1_feedback", carried[:6] == [1, 2, 3, 4, 5, 6],
                 f"filter-1 first six {carried[:6]}, expected [1, 2, 3, 4, 5, 6]")

    # filter path: filter 2 with explicit history
    filt_frame = frame(0x2C, [0] * 28)
    out, _, _ = decode_frame(filt_frame, 1000, 2000)
    report.check("filter_coeffs", out[0] == 172 and out[1] == -503,
                 f"filter-2 first two samples {out[:2]}, expected [172, -503]")

    # negative cases
    for label, data in (("bad_magic", b"XXXX" + hd[4:]),
                        ("short_header", b"SShd")):
        try:
            parse_header(data, len(body))
        except BankError:
            report.check(f"reject_{label}", True, "rejected")
        else:
            report.check(f"reject_{label}", False, "accepted")

    try:
        parse_header(hd, len(body) + 1)
    except BankError:
        report.check("reject_body_size", True, "rejected")
    else:
        report.check("reject_body_size", False, "accepted")

    for label, header in (("filter", 0x50), ("shift", 0x0D)):
        bad = bytearray(frame(0x0C, [0] * 28))
        bad[0] = header
        try:
            decode_body(bytes(bad))
        except BankError:
            report.check(f"reject_frame_{label}", True, f"header {header:#04x} rejected")
        else:
            report.check(f"reject_frame_{label}", False, f"header {header:#04x} accepted")

    try:
        decode_body(body + b"\x00")
    except BankError:
        report.check("reject_ragged_body", True, "not a multiple of 16 rejected")
    else:
        report.check("reject_ragged_body", False, "accepted")

    ok = report.ok
    for c in report.checks:
        print(f"{'PASS' if c['ok'] else 'FAIL'} {c['name']}: {c['detail']}")
    print(f"self-test {'PASS' if ok else 'FAIL'} ({len(report.checks)} checks)")
    return 0 if ok else 1


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--hd", type=Path, help="SShd header member (.hd) read from disk")
    parser.add_argument("--bd", type=Path, help="ADPCM body member (.bd) read from disk")
    parser.add_argument("--max-frames", type=int, default=None,
                        help="decode only the first N frames (default: all)")
    parser.add_argument("--json", action="store_true", help="print the summary as JSON")
    parser.add_argument("--self-test", action="store_true", help="run the synthetic fixture test")
    args = parser.parse_args(argv)
    if args.self_test:
        return self_test()
    if not args.hd or not args.bd:
        parser.error("--hd and --bd are required unless --self-test is used")
    if args.max_frames is not None and args.max_frames <= 0:
        parser.error("--max-frames must be positive")
    try:
        hd = args.hd.expanduser().read_bytes()
        body = args.bd.expanduser().read_bytes()
    except OSError as exc:
        print(f"FAIL io: {exc}", file=sys.stderr)
        return 2
    report = Report()
    summary = inspect_pair(hd, body, report, args.max_frames)
    if args.json:
        print(json.dumps({"ok": report.ok, "summary": summary, "checks": report.checks}, indent=2))
    else:
        for c in report.checks:
            print(f"{'PASS' if c['ok'] else 'FAIL'} {c['name']}: {c['detail']}")
        if "error" not in summary:
            print(f"frames {summary['frames_decoded']}/{summary['frames_total']}, "
                  f"samples {summary['samples']}, peak {summary['peak']}, "
                  f"nonzero {summary['nonzero']}")
            print(f"shifts {summary['shift_histogram']} filters {summary['filter_histogram']}")
            print("codec SPU/VAG ADPCM (16-byte frames); sample rate not claimed; "
                  "no playback, no SXD2 decode")
        print(f"VERDICT: {'PASS' if report.ok else 'FAIL'} ({len(report.checks)} checks)")
    return 0 if report.ok else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
