#!/usr/bin/env python3
"""Verify private raw .csdr inputs with the independent native reader and Python probe.

Uses scripts/psarc.py Psarc.members() (validated MD5(path) mapping), never positional entries.
One csdr_inspect --stream process handles the corpus. Only metadata/digests are written, all
under local/. Hash/CRC fields are compared as metadata, never claimed to be authenticated.
This is importer evidence only: no instruction execution, conversion, Metal frame or G3 result.

  python3 -I tools/dev/verify_csdr_corpus.py --native local/build/orch-r22/csdr_inspect --self-test
  python3 -I tools/dev/verify_csdr_corpus.py --native local/build/orch-r22/csdr_inspect \
      --output local/validation/csdr-reader-r30 --expect-main-corpus \
      local/extracted/CUSA08809/pak0.psarc local/extracted/CUSA08809/pak1.psarc \
      local/extracted/CUSA08809/pakN.psarc
"""
from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
sys.path.insert(0, str(ROOT / "tools/dev"))
from psarc import Psarc  # noqa: E402
from gpu_asset_probe import ContainerError, _synthetic_csdr, parse_raw_csdr  # noqa: E402

PROBE_KEYS = ("info_offset", "code_offset", "version", "type", "source_type", "length",
              "shader_hash", "crc32")
EXPECTED = {
    "pak0.psarc": {"members": 3309, "raw": 1039, "ok": 1039, "gs": 0, "compressed": 2270},
    "pak1.psarc": {"members": 2512, "raw": 1070, "ok": 1070, "gs": 0, "compressed": 1442},
    "pakN.psarc": {"members": 28470, "raw": 12862, "ok": 12858, "gs": 4, "compressed": 15608},
}


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(4 * 1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def line(stream, value):
    stream.write(json.dumps(value, sort_keys=True, separators=(",", ":")) + "\n")


def has_compressed_prefix(data: bytes) -> bool:
    # Diagnostic fallback only. A raw count can collide, so full structural parsing must win.
    return len(data) >= 2 and (data[0] == 0xcc or data[:2] in (b"\x8c\x06", b"\x8c\x0a"))


class NativeReader:
    def __init__(self, executable: Path, stderr=None):
        self.process = subprocess.Popen([str(executable), "--stream"], stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE, stderr=stderr)

    def parse(self, data: bytes) -> dict:
        if len(data) > 64 * 1024 * 1024:
            raise ValueError("member exceeds native diagnostic 64 MiB limit")
        self.process.stdin.write(struct.pack("<I", len(data)))
        self.process.stdin.write(data)
        self.process.stdin.flush()
        output = self.process.stdout.readline()
        if not output:
            raise ValueError(f"native reader stopped (exit {self.process.poll()})")
        return json.loads(output)

    def close(self):
        self.process.stdin.close()
        try:
            result = self.process.wait(timeout=30)
        except subprocess.TimeoutExpired:
            self.process.kill()
            self.process.wait()
            raise ValueError("native reader did not exit after EOF") from None
        finally:
            self.process.stdout.close()
        if result != 0:
            raise ValueError(f"native reader exit {result}")


def self_test(executable: Path):
    native = NativeReader(executable)
    checks = 0
    try:
        good = _synthetic_csdr([(1, 20), (0, 12)])
        parsed = native.parse(good)
        assert parsed["status"] == "ok"
        assert [{key: p[key] for key in PROBE_KEYS} for p in parsed["programs"]] == parse_raw_csdr(good)
        checks += 1
        for count in (140, 204, 1676, 2700, 1740, 2764):
            many = _synthetic_csdr([(1, 20)] * count)
            parsed_many = native.parse(many)
            assert has_compressed_prefix(many) == (count != 140)
            assert parsed_many["status"] == "ok" and parsed_many["declared_count"] == count
            assert [{key: p[key] for key in PROBE_KEYS} for p in parsed_many["programs"]] == \
                parse_raw_csdr(many)
            checks += 1
        for payload, expected in ((b"\x8c\x06", "unsupported_compression"),
                                  (b"\x8c\x0a", "unsupported_compression"),
                                  (b"\xcc\x06", "unsupported_compression"),
                                  (b"\xcc\x7f", "unsupported_compression"),
                                  (b"\xcc\x0a", "unsupported_compression"),
                                  (b"", "too_short"),
                                  (good[:-1], "truncated_binary_info"),
                                  (_synthetic_csdr([(1, 20)], 2), "count_mismatch"),
                                  (_synthetic_csdr([(5, 20), (6, 12)], 1),
                                   "unsupported_gs_double_program")):
            rejected = native.parse(payload)
            assert rejected["status"] == expected, rejected
            assert rejected["programs"] == []
            checks += 1
        # Framing must not silently accept a truncated or excessive frame.
        for transport in (b"\x01", struct.pack("<I", 10) + b"x",
                          struct.pack("<I", 64 * 1024 * 1024 + 1)):
            run = subprocess.run([str(executable), "--stream"], input=transport,
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=30)
            assert run.returncode == 2 and not run.stdout, run
            checks += 1
    finally:
        native.close()
    print(f"csdr corpus verifier self-test: {checks} checks passed")


def verify(args) -> int:
    output = args.output.resolve()
    if not output.is_relative_to((ROOT / "local").resolve()):
        raise ValueError("private corpus output must be under this workspace's local/ directory")
    # Do not overwrite evidence from an earlier run.
    output.mkdir(parents=True, exist_ok=False)
    start = time.monotonic()
    summary = {
        "status": "FAILED", "started": datetime.now().astimezone().isoformat(),
        "native": str(args.native.resolve()), "native_sha256": digest(args.native),
        "archives": [], "exceptions": [], "errors": [],
        "limits": ["raw structural parsing only; opaque header/trailer fields unmodelled",
                   "hash and CRC retained as metadata; checksum algorithms unverified",
                   "usage table/resource ABI unparsed; no shader semantics or Metal execution",
                   "known 8C/CC prefixes classify compression only after full raw validation fails",
                   "no decompression; G3 NOT RUN"],
    }
    paths = [ROOT / name for name in (
        "native/core/csdr_container.hpp", "native/core/csdr_container.cpp",
        "tools/dev/csdr_inspect.cpp", "tools/dev/verify_csdr_corpus.py",
        "tools/dev/gpu_asset_probe.py", "scripts/psarc.py")]
    summary["source_sha256"] = {str(path.relative_to(ROOT)): digest(path) for path in paths}
    stage_counts, versions, sources = Counter(), Counter(), Counter()
    with (output / "native.stderr.log").open("w") as stderr, \
            (output / "members.jsonl").open("w") as members_file, \
            (output / "programs.jsonl").open("w") as programs_file:
        native = NativeReader(args.native.resolve(), stderr)
        try:
            for archive_path in args.archives:
                archive_path = archive_path.resolve()
                archive = Psarc(archive_path)
                identity = {"path": str(archive_path), "size": archive_path.stat().st_size,
                            "sha256": digest(archive_path)}
                counts = dict.fromkeys(("members", "raw", "ok", "gs", "compressed"), 0)
                try:
                    for path, index in archive.members():
                        if not path.endswith(".csdr"):
                            continue
                        counts["members"] += 1
                        row = {"archive": str(archive_path), "path": path, "entry_index": index,
                               "entry_path_md5": archive.entries[index]["md5"]}
                        # Full raw validation must precede prefix classification: a raw count can
                        # equal any known two-byte prefix. Never skip a structurally valid member.
                        data = archive.read(index)
                        result = native.parse(data)
                        row.update(size=len(data), sha256=hashlib.sha256(data).hexdigest(),
                                   native=result, input_scope="complete member")
                        if has_compressed_prefix(data) and \
                                result["status"] == "unsupported_compression":
                            counts["compressed"] += 1
                            row["input_scope"] = "complete compressed-prefix member; raw structure rejected"
                            line(members_file, row)
                            continue
                        counts["raw"] += 1
                        row["input_scope"] = "complete raw member"
                        try:
                            reference = parse_raw_csdr(data)
                            probe_error = None
                        except ContainerError as exc:
                            reference, probe_error = None, str(exc)
                        row["probe_error"] = probe_error
                        if result["status"] == "ok":
                            native_fields = [{key: p[key] for key in PROBE_KEYS}
                                             for p in result["programs"]]
                            if reference != native_fields:
                                raise ValueError(f"native/Python metadata disagreement: {path}")
                            counts["ok"] += 1
                            for ordinal, program in enumerate(result["programs"]):
                                code = data[program["code_offset"]:
                                            program["code_offset"] + program["length"]]
                                line(programs_file, {
                                    "archive": str(archive_path), "path": path, "ordinal": ordinal,
                                    "member_sha256": row["sha256"],
                                    "code_sha256": hashlib.sha256(code).hexdigest(), **program})
                                stage_counts[str(program["type"])] += 1
                                versions[str(program["version"])] += 1
                                sources[str(program["source_type"])] += 1
                        elif result["status"] == "unsupported_gs_double_program":
                            if probe_error != "header count 1 != 2 OrbShdr blobs" or \
                                    result["declared_count"] != 1 or result["observed_count"] != 2 or \
                                    result["programs"]:
                                raise ValueError(f"unconfirmed GS exception: {path}")
                            counts["gs"] += 1
                            summary["exceptions"].append(row)
                        else:
                            raise ValueError(f"unexpected native rejection: {path}: {result}")
                        line(members_file, row)
                finally:
                    archive.close()
                identity["counts"] = counts
                summary["archives"].append(identity)
                print(f"{archive_path.name}: {counts}", flush=True)
                if args.expect_main_corpus and counts != EXPECTED.get(archive_path.name):
                    raise ValueError(f"counts differ from the pinned main-corpus expectation: {counts}")
            if args.expect_main_corpus and sorted(p.name for p in args.archives) != sorted(EXPECTED):
                raise ValueError("--expect-main-corpus requires exactly pak0, pak1 and pakN once each")
            summary["status"] = "ARTIFACT_VERIFIED"
        except (OSError, ValueError, AssertionError) as exc:
            summary["errors"].append(str(exc))
        finally:
            try:
                native.close()
            except (OSError, ValueError) as exc:
                summary["status"] = "FAILED"
                summary["errors"].append(str(exc))
    summary["program_counts_by_type"] = dict(stage_counts)
    summary["versions"] = dict(versions)
    summary["source_types"] = dict(sources)
    summary["elapsed_seconds"] = round(time.monotonic() - start, 3)
    summary["finished"] = datetime.now().astimezone().isoformat()
    summary["members_manifest_sha256"] = digest(output / "members.jsonl")
    summary["programs_manifest_sha256"] = digest(output / "programs.jsonl")
    (output / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
    print(f"{summary['status']}: {output / 'summary.json'}", flush=True)
    return 0 if summary["status"] == "ARTIFACT_VERIFIED" else 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--native", type=Path, required=True)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--expect-main-corpus", action="store_true")
    parser.add_argument("archives", type=Path, nargs="*")
    args = parser.parse_args()
    try:
        if args.self_test:
            self_test(args.native.resolve())
            return 0
        if not args.output or not args.archives:
            parser.error("corpus verification requires --output and one or more archive paths")
        return verify(args)
    except (OSError, ValueError, AssertionError) as exc:
        print(f"FAIL {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
