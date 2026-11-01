#!/usr/bin/env python3
"""Characterize the compressed `.csdr` container prefixes (8c06 / 8c0a / cc**).

E626 established that 19,320 corpus members are rejected as compressed-prefix and that the
container header is opaque. This probe does not guess a format: it measures the header across a
deterministic sample, tests standard codecs at plausible offsets, and reports either a decoded
format or an explicit "unknown" with the structure statistics.

It reads the pinned private archives through the validated PSARC `MD5(path)` pairing and writes
only metadata (sizes, digests, entropy, field values) under `local/`.

Usage:
  python3 -I tools/dev/csdr_compressed_probe.py --self-test
  python3 -I tools/dev/csdr_compressed_probe.py --sample 256 --out local/validation/csdr-compressed
"""

from __future__ import annotations

import argparse
import bz2
import hashlib
import json
import lzma
import math
import struct
import sys
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from psarc import Psarc  # noqa: E402

DEFAULT_ARCHIVES = (
    "local/extracted/CUSA08809/pak0.psarc",
    "local/extracted/CUSA08809/pak1.psarc",
    "local/extracted/CUSA08809/pakN.psarc",
)

MAGICS = {
    "zlib_78_01": bytes.fromhex("7801"),
    "zlib_78_9c": bytes.fromhex("789c"),
    "zlib_78_da": bytes.fromhex("78da"),
    "lz4_frame": bytes.fromhex("04224d18"),
    "zstd": bytes.fromhex("28b52ffd"),
    "xz": bytes.fromhex("fd377a585a00"),
    "bzip2": bytes.fromhex("425a68"),
    "lzma_alone": bytes.fromhex("5d0000"),
    "snappy_stream": bytes.fromhex("ff060000734e6150"),
}

ORB = b"ORB"


def entropy(data: bytes) -> float:
    if not data:
        return 0.0
    counts = [0] * 256
    for byte in data:
        counts[byte] += 1
    total = len(data)
    return -sum((count / total) * math.log2(count / total) for count in counts if count)


def codec_trials(data: bytes) -> dict:
    """Try standard codecs at a few offsets. Returns {label: True/False} plus first success."""
    attempts = {}
    success = None
    for offset in (0, 2, 4, 8, 12, 16, 20, 24, 32):
        payload = data[offset:]
        if len(payload) < 16:
            continue
        for label, fn in (
            ("zlib_15", lambda b: zlib.decompress(b, 15)),
            ("deflate_raw", lambda b: zlib.decompress(b, -15)),
            ("gzip_31", lambda b: zlib.decompress(b, 31)),
            ("bz2", bz2.decompress),
            ("lzma", lzma.decompress),
        ):
            key = f"{label}@{offset}"
            try:
                out = fn(payload)
            except Exception:
                attempts[key] = False
                continue
            attempts[key] = True
            if success is None and len(out) >= 16:
                success = {"codec": label, "offset": offset, "decoded_size": len(out),
                           "decoded_head": out[:16].hex(),
                           "decoded_starts_orb": out[:3] == ORB}
    return {"attempts": attempts, "success": success}


def header_fields(data: bytes) -> dict:
    return {
        "prefix": data[:2].hex(),
        "u16be_2_4": struct.unpack_from(">H", data, 2)[0],
        "u16be_4_6": struct.unpack_from(">H", data, 4)[0],
        "u16be_6_8": struct.unpack_from(">H", data, 6)[0],
        "u16le_2_4": struct.unpack_from("<H", data, 2)[0],
        "u32le_4_8": struct.unpack_from("<I", data, 4)[0],
        "u32le_8_12": struct.unpack_from("<I", data, 8)[0],
        "u32le_12_16": struct.unpack_from("<I", data, 12)[0],
        "header_hex": data[:16].hex(),
    }


def sample_members(archives, sample: int):
    rows = []
    for archive in archives:
        path = ROOT / archive
        if not path.is_file():
            raise SystemExit(f"missing archive {path}")
        psarc = Psarc(str(path))
        members = psarc.members()
        paths = sorted((member[0], index) for index, member in enumerate(members))
        rows.extend((archive, psarc, member_path, index) for member_path, index in paths)
    rows.sort(key=lambda row: (row[3 - 1], row[0]))  # path, then archive
    total = len(rows)
    if sample >= total:
        chosen = rows
    else:
        chosen = [rows[round(i * (total - 1) / (sample - 1))] for i in range(sample)]
    return chosen, total


def probe(archives, sample: int) -> dict:
    chosen, total_rows = sample_members(archives, sample)
    csdr = [row for row in chosen if row[2].endswith(".csdr")]
    records = []
    for archive, psarc, member_path, index in csdr:
        data = psarc.read(index)
        if not data:
            continue
        first = data[0]
        if first not in (0x8C, 0xCC):
            continue
        fields = header_fields(data)
        payload = data[16:] if len(data) > 16 else b""
        trials = codec_trials(data)
        record = {
            "path": member_path,
            "archive": Path(archive).name,
            "size": len(data),
            "sha256": hashlib.sha256(data).hexdigest(),
            "entropy_header": round(entropy(data[:16]), 4),
            "entropy_payload": round(entropy(payload), 4),
            "has_orb": ORB in data,
            "magics": [label for label, magic in MAGICS.items() if data[:16].startswith(magic)],
            "codec_success": trials["success"],
            **fields,
        }
        records.append(record)
    return {"total_rows": total_rows, "csdr_rows": len(csdr), "records": records}


def summarize(document: dict) -> dict:
    records = document["records"]
    by_prefix = {}
    for record in records:
        by_prefix.setdefault(record["prefix"], []).append(record)
    summary = {"sampled_compressed_members": len(records), "by_prefix": {}}
    for prefix, rows in sorted(by_prefix.items()):
        sizes = [row["size"] for row in rows]
        ratios = [row["u16be_2_4"] / row["size"] for row in rows if row["size"]]
        payload_entropy = [row["entropy_payload"] for row in rows]
        decoded = [row for row in rows if row["codec_success"]]
        summary["by_prefix"][prefix] = {
            "count": len(rows),
            "size_min": min(sizes),
            "size_median": sorted(sizes)[len(sizes) // 2],
            "size_max": max(sizes),
            "field_2_4_over_size_min": round(min(ratios), 6) if ratios else None,
            "field_2_4_over_size_median": round(sorted(ratios)[len(ratios) // 2], 6) if ratios else None,
            "field_2_4_over_size_max": round(max(ratios), 6) if ratios else None,
            "entropy_payload_min": round(min(payload_entropy), 4),
            "entropy_payload_median": round(sorted(payload_entropy)[len(payload_entropy) // 2], 4),
            "entropy_payload_max": round(max(payload_entropy), 4),
            "magic_hits": sorted({magic for row in rows for magic in row["magics"]}),
            "orb_hits": sum(1 for row in rows if row["has_orb"]),
            "codec_decoded": len(decoded),
        }
    return summary


def render_summary(summary: dict) -> str:
    lines = [f"sampled compressed members: {summary['sampled_compressed_members']}"]
    for prefix, data in summary["by_prefix"].items():
        lines.append(f"prefix {prefix}: {data['count']} members")
        lines.append(
            "  size %d..%d (median %d); field[2:4]/size min %.4f median %.4f max %.4f"
            % (data["size_min"], data["size_max"], data["size_median"],
               data["field_2_4_over_size_min"], data["field_2_4_over_size_median"],
               data["field_2_4_over_size_max"]))
        lines.append(
            "  payload entropy %.3f..%.3f (median %.3f); codec-decoded %d; ORB hits %d; magics %s"
            % (data["entropy_payload_min"], data["entropy_payload_max"],
               data["entropy_payload_median"], data["codec_decoded"], data["orb_hits"],
               data["magic_hits"] or "none"))
    decoded_total = sum(data["codec_decoded"] for data in summary["by_prefix"].values())
    lines.append(f"decoded by a standard codec: {decoded_total}")
    return "\n".join(lines)


def self_test() -> int:
    checks = 0

    def require(cond, msg):
        nonlocal checks
        if not cond:
            raise SystemExit(f"FAIL {msg}")
        checks += 1

    data = bytes.fromhex("8c0a001218881216") + b"\x00" * 8
    fields = header_fields(data)
    require(fields["prefix"] == "8c0a", "prefix")
    require(fields["u16be_2_4"] == 0x0012, "u16be 2:4")
    require(fields["u16be_6_8"] == 0x1216, "u16be 6:8")
    require(entropy(b"\x00\x00\x00\x00") == 0.0, "zero entropy")
    require(abs(entropy(bytes(range(256))) - 8.0) < 1e-9, "uniform entropy")
    ok = codec_trials(zlib.compress(b"ABCD" * 1024))
    require(ok["success"] is not None and ok["success"]["codec"].startswith("zlib"), "zlib trial")
    bad = codec_trials(data)
    require(bad["success"] is None, "no false positive on header bytes")
    summary = summarize({"records": [{"prefix": "8c0a", "size": 100, "u16be_2_4": 10,
                                      "entropy_payload": 7.9, "magics": [], "has_orb": False,
                                      "codec_success": None}]})
    require(summary["by_prefix"]["8c0a"]["count"] == 1, "summary count")
    print(f"csdr compressed probe self-test: {checks} checks passed (synthetic bytes only)")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--sample", type=int, default=256)
    parser.add_argument("--out", default="local/validation/csdr-compressed")
    parser.add_argument("--archives", nargs="*", default=list(DEFAULT_ARCHIVES))
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    out = Path(args.out)
    if not out.is_absolute():
        out = ROOT / out
    if ROOT not in out.parents and out != ROOT:
        raise SystemExit("output must stay inside the workspace")
    if "local" not in out.parts:
        raise SystemExit("output must stay under local/ (private evidence)")
    if out.exists() and any(out.iterdir()):
        raise SystemExit(f"{out} already exists; pass a fresh --out to keep earlier evidence")

    document = probe(list(args.archives), args.sample)
    summary = summarize(document)
    out.mkdir(parents=True, exist_ok=True)
    (out / "compressed_probe.json").write_text(json.dumps(
        {"summary": summary, "records": document["records"]}, indent=2, sort_keys=True) + "\n")
    (out / "SUMMARY.txt").write_text(render_summary(summary) + "\n")
    print(render_summary(summary))
    print(f"wrote {out}/compressed_probe.json")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
