#!/usr/bin/env python3
"""Bounded front-end coverage census of the private `.csdr` corpus; decode only, never run.

`gcn_to_msl.py` lowers a GCN program only after a private ABI manifest supplies the exact
program digest, named scalar views and stream bindings. No such manifest exists for retail
shaders, so this tool measures the *front end* instead: how many sampled members can even be
read as a raw container, and how many of those programs `gcn_inventory.py` decodes.

Classification is exclusive and honest:
  lowered                never produced here (no ABI manifest exists; nothing is invented)
  blocked-no-abi-manifest every program decoded; lowering stops at the missing manifest
  decode-error           the container parsed but at least one program failed gcn_inventory
  not-a-shader           no raw shader body this reader can see (compressed block/container)

Member paths are paired through `scripts/psarc.py` `Psarc.members()`, i.e. MD5(path) only.
Container structure comes from the same validated Python probe (`parse_raw_csdr`) that
`verify_csdr_corpus.py` checks against the native reader. Opcode coverage imports
`gcn_inventory.OPCODES`; nothing here re-lists encodings. Only paths, sizes, digests, counters
and decoder strings are recorded - never game bytes, code spans or instruction listings - and
inside this repo output must stay under private `local/`. Shader code is never executed.

Evidence: the census is STATIC_ANALYSIS_VERIFIED. `--self-test` is FIXTURE_VERIFIED and uses
only synthetic spans, so the tool is testable without private input.

  python3 -I tools/dev/msl_corpus_coverage.py --self-test
  python3 -I tools/dev/msl_corpus_coverage.py            # default sample 64, pinned archives
  python3 -I tools/dev/msl_corpus_coverage.py --sample 512 \\
      --out local/validation/msl-coverage/sample-512
"""
from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import importlib.util
import json
from pathlib import Path
import platform
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
for injected in (HERE, ROOT / "scripts"):
    if str(injected) not in sys.path:
        sys.path.insert(0, str(injected))

from gpu_asset_probe import ContainerError, parse_raw_csdr  # noqa: E402
from psarc import Psarc  # noqa: E402

VERSION = "msl-corpus-coverage-v2"
DEFAULT_ARCHIVES = ("local/extracted/CUSA08809/pak0.psarc",
                    "local/extracted/CUSA08809/pak1.psarc",
                    "local/extracted/CUSA08809/pakN.psarc")
DEFAULT_OUT = "local/validation/msl-coverage"
DEFAULT_SAMPLE = 64
# Rejection diagnostics only; a structurally valid raw member always wins over the prefix.
OODLE_LIKE_PREFIXES = (b"\x8c\x06", b"\x8c\x0a")
# INF (filename co-occurrence, docs/METAL_FRAME_PATH.md 4.3): a nibble label, not stage semantics.
STAGE_BY_TYPE = {0: "ps", 1: "vs", 2: "gs", 4: "cs", 7: "hs", 8: "ds"}
CLASSIFICATIONS = ("lowered", "blocked-no-abi-manifest", "decode-error", "not-a-shader")


def load_gcn():
    """Load the sibling decoder by path, exactly as gcn_to_msl.py does."""
    spec = importlib.util.spec_from_file_location("gcn_inventory_for_coverage",
                                                  HERE / "gcn_inventory.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


gcn = load_gcn()


def digest_file(path: Path, chunk_size: int = 1 << 22) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(chunk_size), b""):
            h.update(chunk)
    return h.hexdigest()


def sample_indices(population: int, sample: int) -> list[int]:
    """Deterministic even spacing: round(i*(N-1)/(k-1)) over the sorted population."""
    if population <= 0:
        raise ValueError("coverage census needs a nonempty member population")
    if sample <= 0:
        raise ValueError("sample size must be positive")
    if sample >= population:
        return list(range(population))
    if sample == 1:
        return [0]
    return [round(i * (population - 1) / (sample - 1)) for i in range(sample)]


def classify_program(code: bytes, ordinal: int, info: dict) -> dict:
    """Decode one exact code span; report only counts, digests and the decoder's own string."""
    row = {
        "ordinal": ordinal,
        "header_type_code": info["type"],
        "stage": STAGE_BY_TYPE.get(info["type"]),
        "stage_evidence": "INF filename co-occurrence (docs/METAL_FRAME_PATH.md 4.3); "
                          "the header nibble itself does not establish stage semantics",
        "version": info["version"],
        "source_type": info["source_type"],
        "code_bytes": len(code),
        "code_sha256": hashlib.sha256(code).hexdigest(),
        "decoded": False,
        "decoder_error": None,
        "instructions": None,
        "supported_opcode_share": None,
        "unsupported_opcodes": [],
        "encodings": None,
        "per_lane_eligible": None,
        "per_lane_blocker_instructions": None,
        "lowering": "decode-error",
        "block_decode": None,
    }
    # The CFG-aware decode mode lives in gcn_inventory; this records its counters only.
    try:
        block_program = gcn.decode_blocks(code)
    except gcn.DecodeError as exc:
        row["block_decode"] = {
            "ok": False, "blocks": 0, "instructions": 0, "branches": 0,
            "branches_in_span": 0, "branches_on_instruction": 0, "branch_mnemonics": [],
            "error_offset": exc.offset, "error_reason": exc.reason,
        }
    else:
        row["block_decode"] = {
            "ok": True,
            "blocks": len(block_program.blocks),
            "instructions": len(block_program.instructions),
            "branches": len(block_program.branches),
            "branches_in_span": sum(1 for b in block_program.branches if b.target_in_program),
            "branches_on_instruction": sum(1 for b in block_program.branches
                                           if b.target_on_instruction),
            "branch_mnemonics": sorted({b.mnemonic for b in block_program.branches}),
            "error_offset": None,
            "error_reason": None,
        }
    try:
        rows = gcn.decode(code)
    except gcn.DecodeError as exc:
        row["decoder_error"] = str(exc)
        return row
    supported = sum(1 for i in rows
                    if i.encoding in gcn.OPCODES and i.opcode in gcn.OPCODES[i.encoding])
    blockers = sum(1 for i in rows if gcn.PER_LANE_BLOCKERS.intersection(i.requirements))
    row.update(
        decoded=True,
        instructions=len(rows),
        supported_opcode_share=round(supported / len(rows), 6),
        unsupported_opcodes=sorted({i.mnemonic for i in rows
                                    if i.opcode not in gcn.OPCODES.get(i.encoding, {})}),
        encodings=dict(sorted(Counter(i.encoding for i in rows).items())),
        per_lane_eligible=blockers == 0,
        per_lane_blocker_instructions=blockers,
        lowering="blocked-no-abi-manifest",
    )
    return row


def split_stages(code: bytes) -> tuple[list[bytes], int | None]:
    """Split a blob that holds more than one program back to back.

    Some members carry a VS/PS pair: the declared span continues past the first `s_endpgm`, and
    `decode_blocks` reports `bytes follow s_endpgm`. When a cut point exists where **both** halves
    decode, the pair is returned as two spans; otherwise the span is returned whole so the failure
    stays visible instead of being papered over.
    """
    try:
        gcn.decode_blocks(code)
    except gcn.DecodeError as exc:
        if "bytes follow s_endpgm" not in exc.reason or len(code) > 4096:
            return [code], None
        for cut in range(4, len(code) - 3, 4):
            try:
                gcn.decode(code[:cut], allow_branches=True)
                gcn.decode(code[cut:], allow_branches=True)
            except gcn.DecodeError:
                continue
            return [code[:cut], code[cut:]], cut
        return [code], None
    return [code], None


def classify_member(archive: str, path: str, entry_index: int, data: bytes) -> dict:
    """Classify one complete member: container readability first, then per-program decode."""
    row = {"archive": archive, "path": path, "entry_index": entry_index,
           "member_bytes": len(data), "member_sha256": hashlib.sha256(data).hexdigest(),
           "classification": "not-a-shader", "reason": None,
           "compressed_prefix": False, "programs": []}
    try:
        blobs = parse_raw_csdr(data)
    except ContainerError as exc:
        if data[:2] in OODLE_LIKE_PREFIXES:
            row["compressed_prefix"] = True
            row["reason"] = f"compressed-prefix {data[:2].hex()}: {exc}"
        else:
            row["reason"] = f"container: {exc}"
        return row
    split_offsets = []
    for ordinal, blob in enumerate(blobs):
        code = data[blob["code_offset"]:blob["code_offset"] + blob["length"]]
        spans, split = split_stages(code)
        if split is not None:
            split_offsets.append(split)
        for span_index, span in enumerate(spans):
            record = classify_program(span, ordinal, blob)
            record["span_index"] = span_index
            record["span_count"] = len(spans)
            record["member_split"] = split
            row["programs"].append(record)
    row["split_offsets"] = split_offsets
    if not row["programs"]:
        row["reason"] = "container declared no programs"
        return row
    row["classification"] = ("decode-error" if any(not p["decoded"] for p in row["programs"])
                             else "blocked-no-abi-manifest")
    return row


def read_failure(archive: str, path: str, entry_index: int, exc: Exception) -> dict:
    return {"archive": archive, "path": path, "entry_index": entry_index,
            "member_bytes": None, "member_sha256": None, "classification": "not-a-shader",
            "reason": f"psarc-read: {exc}", "compressed_prefix": False, "programs": []}


def reason_without_offset(text: str) -> str:
    """Keep the decoder's own wording but drop the per-span byte offset for counting."""
    return text.split(": ", 1)[1] if text.startswith("at byte ") else text


def distribution(values: list[float]) -> dict:
    if not values:
        return {"count": 0, "min": None, "median": None, "max": None}
    ordered = sorted(values)
    n = len(ordered)
    median = ordered[n // 2] if n % 2 else (ordered[n // 2 - 1] + ordered[n // 2]) / 2
    return {"count": n, "min": ordered[0], "median": median, "max": ordered[-1]}


def summarize(rows: list[dict], population: list[tuple], sample_info: dict,
              archives: list[dict]) -> dict:
    classes = Counter(r["classification"] for r in rows)
    programs = [p for r in rows for p in r["programs"]]
    decoded = [p for p in programs if p["decoded"]]
    errors = [p["decoder_error"] for p in programs if p["decoder_error"]]
    container_reasons = Counter(r["reason"] for r in rows
                                if r["classification"] == "not-a-shader")
    container_classes = Counter(r["reason"].split(": ", 1)[0] for r in rows
                                if r["classification"] == "not-a-shader")
    by_archive = {a["name"]: dict(sorted(Counter(
        r["classification"] for r in rows if r["archive"] == a["name"]).items()))
        for a in archives}
    totals = {
        "sampled_members": len(rows),
        "classifications": {name: classes.get(name, 0) for name in CLASSIFICATIONS},
        "programs_attempted": len(programs),
        "programs_decoded": len(decoded),
        "programs_decode_error": len(errors),
        "programs_block_decodable": sum(1 for p in programs
                                        if (p.get("block_decode") or {}).get("ok")),
        "members_split_into_stages": sum(1 for r in rows if r.get("split_offsets")),
        "programs_from_split_stages": sum(1 for p in programs
                                          if (p.get("span_count") or 1) > 1),
        "stage_split_offsets": sorted({offset for r in rows
                                       for offset in (r.get("split_offsets") or [])}),
        "programs_linear_only": sum(1 for p in programs
                                    if not p["decoded"]
                                    and (p.get("block_decode") or {}).get("ok")),
        "branch_rows_total": sum((p.get("block_decode") or {}).get("branches") or 0
                                 for p in programs),
        "branch_targets_in_span": sum((p.get("block_decode") or {}).get("branches_in_span") or 0
                                      for p in programs),
        "branch_targets_on_instruction": sum(
            (p.get("block_decode") or {}).get("branches_on_instruction") or 0 for p in programs),
        "instructions_total": sum(p["instructions"] for p in decoded),
        "supported_opcode_share": distribution([p["supported_opcode_share"] for p in decoded]),
        "programs_with_unsupported_opcodes": sum(1 for p in decoded if p["unsupported_opcodes"]),
        "per_lane_eligible_programs": sum(1 for p in decoded if p["per_lane_eligible"]),
        "per_lane_blocker_instructions": sum(p["per_lane_blocker_instructions"] for p in decoded),
        "decode_error_reasons": dict(sorted(Counter(errors).items())),
        "decode_error_reasons_without_offset": dict(sorted(
            Counter(reason_without_offset(e) for e in errors).items())),
        "not_a_shader_reasons": dict(sorted(container_reasons.items())),
        "not_a_shader_reason_classes": dict(sorted(container_classes.items())),
        "header_type_codes": dict(sorted(Counter(
            str(p["header_type_code"]) for p in programs).items(), key=lambda kv: int(kv[0]))),
        "stages_inferred": dict(sorted(Counter(
            str(p["stage"]) for p in programs).items())),
        "by_archive": by_archive,
        "population_members": len(population),
        "population_unique_paths": len({p for p, _, _ in population}),
    }
    totals["coverage"] = {
        "sampled_of_population": (round(len(rows) / len(population), 6)
                                  if population else None),
        "decoded_program_share_of_sampled_members": (round(len(decoded) / len(rows), 6)
                                                    if rows else None),
        "lowered_programs": 0,
        "why_zero_lowered": "no per-shader ABI manifest exists; inventing bindings is forbidden",
    }
    return {"schema": VERSION,
            "generated": datetime.now(timezone.utc).isoformat(timespec="seconds"),
            "sample": sample_info, "archives": archives, "totals": totals, "members": rows}


def human_summary(summary: dict, out_path: Path) -> list[str]:
    t = summary["totals"]
    s = summary["sample"]
    share = t["supported_opcode_share"]
    classes = t["classifications"]
    lines = [
        "== MSL corpus coverage (front-end only; nothing lowered) ==",
        f"sample         : {s['selected']} of {t['population_members']} .csdr members "
        f"({t['population_unique_paths']} unique paths); {s['rule']}",
        "archives       : " + ", ".join(f"{a['name']} ({a['csdr_members']} .csdr)"
                                       for a in summary["archives"]),
        "classifications: " + " | ".join(f"{k} {v}" for k, v in classes.items()),
        f"programs       : {t['programs_attempted']} attempted, {t['programs_decoded']} decoded, "
        f"{t['programs_decode_error']} decode-error",
        f"instructions   : {t['instructions_total']} total across decoded programs",
        f"supported share: min {share['min']}  median {share['median']}  max {share['max']}  "
        "(degenerate: decode() rejects any opcode outside the imported table)",
        f"per-lane       : {t['per_lane_eligible_programs']}/{t['programs_decoded']} decoded "
        f"programs pass the PER_LANE_BLOCKERS gate "
        f"({t['per_lane_blocker_instructions']} blocking instructions)",
        "lowered        : 0 (no ABI manifest for any real shader)",
    ]
    if t["decode_error_reasons_without_offset"]:
        top = sorted(t["decode_error_reasons_without_offset"].items(),
                     key=lambda kv: (-kv[1], kv[0]))[:8]
        lines.append("decode errors  : " + " | ".join(f"{r} x{n}" for r, n in top))
    compressed = sum(1 for r in summary["members"] if r["compressed_prefix"])
    if compressed:
        lines.append(f"not-a-shader   : {compressed} compressed-prefix members "
                     f"(no decompressor; G3 NOT RUN)")
    if t.get("not_a_shader_reason_classes"):
        lines.append("not-a-shader by class: " + " | ".join(
            f"{k} {v}" for k, v in t["not_a_shader_reason_classes"].items()))
    lines.append(f"wrote          : {out_path}")
    return lines


def self_test() -> int:
    """Synthetic spans only: no private archive is opened or referenced."""
    checks = 0

    def check(name: str, condition: bool) -> None:
        nonlocal checks
        if not condition:
            raise AssertionError(f"self-test failed: {name}")
        checks += 1

    good = (0xBE800300).to_bytes(4, "little") + (0xBF810000).to_bytes(4, "little")
    good_row = classify_program(good, 0, {"type": 1, "version": 7, "source_type": 1})
    check("decodable synthetic span", good_row["decoded"] is True)
    check("two instructions", good_row["instructions"] == 2)
    check("supported share is 1", good_row["supported_opcode_share"] == 1.0)
    check("blocked without manifest", good_row["lowering"] == "blocked-no-abi-manifest")

    trailing = good + (0xBF800000).to_bytes(4, "little")
    bad_row = classify_program(trailing, 0, {"type": 1, "version": 7, "source_type": 1})
    check("trailing bytes rejected", bad_row["decoded"] is False)
    check("exact decoder string",
          bad_row["decoder_error"] == "at byte 0x8: bytes follow s_endpgm inside the declared program")
    check("decode-error class", bad_row["lowering"] == "decode-error")

    from gpu_asset_probe import _synthetic_csdr
    # The repo fixture pads to an 8-byte boundary, so an even dword count keeps its token rule.
    # Its placeholder SOPP opcode 0 is unsupported; substitute s_waitcnt vmcnt=0 so the program
    # decodes while the container layout stays the validated one.
    placeholder = (0xBF800000).to_bytes(4, "little")
    supported = (0xBF8C0000).to_bytes(4, "little")
    pair = classify_member("synthetic", "synthetic/pair.csdr", 0,
                           _synthetic_csdr([(1, 4), (0, 4)]).replace(placeholder, supported))
    check("synthetic container class", pair["classification"] == "blocked-no-abi-manifest")
    check("synthetic programs", [p["header_type_code"] for p in pair["programs"]] == [1, 0])
    check("inferred stages", [p["stage"] for p in pair["programs"]] == ["vs", "ps"])
    check("synthetic instruction total",
          sum(p["instructions"] for p in pair["programs"]) == 6)

    broken = classify_member("synthetic", "synthetic/broken.csdr", 0, _synthetic_csdr([(1, 4)]))
    check("undecodable container is decode-error", broken["classification"] == "decode-error")
    check("undecodable program reason",
          broken["programs"][0]["decoder_error"] == "at byte 0x8: unsupported SOPP opcode 0")

    compressed = classify_member("synthetic", "synthetic/blocked.csdr", 0, b"\x8c\x0a" + b"\0" * 40)
    check("compressed member is not-a-shader", compressed["classification"] == "not-a-shader")
    check("compressed reason recorded", compressed["reason"].startswith("compressed-prefix 8c0a"))
    check("compressed programs empty", compressed["programs"] == [])

    first = sample_indices(34291, 64)
    check("sampler deterministic", first == sample_indices(34291, 64))
    check("sampler size", len(first) == 64)
    check("sampler spans population", (first[0], first[-1]) == (0, 34290))
    check("sampler monotone", first == sorted(set(first)))
    check("sampler single", sample_indices(1000, 1) == [0])
    check("sampler clamps", sample_indices(5, 10) == list(range(5)))

    summary = summarize([], [], {"selected": 0, "rule": "synthetic"}, [])
    check("empty summary has zero lowered", summary["totals"]["classifications"]["lowered"] == 0)
    print(f"msl corpus coverage self-test: {checks} checks passed (synthetic spans only)")
    return 0


def resolve_output(out: Path) -> Path:
    """Resolve and bound the destination: private local/ only inside this repo."""
    resolved = out.resolve()
    local = (ROOT / "local").resolve()
    if resolved.is_relative_to(ROOT.resolve()) and not resolved.is_relative_to(local):
        raise ValueError("private coverage output inside this repo must stay under local/")
    return resolved


def write_output(out: Path, summary: dict, overwrite: bool) -> Path:
    resolved = resolve_output(out)
    resolved.mkdir(parents=True, exist_ok=True)
    target = resolved / "coverage.json"
    if target.exists() and not overwrite:
        raise ValueError(f"{target} already exists; pass --overwrite to replace earlier evidence")
    target.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
    return target


def run(args) -> int:
    out = args.out if args.out is not None else ROOT / DEFAULT_OUT
    resolve_output(out)  # fail on a forbidden destination before reading any archive
    archive_paths = args.archives or [ROOT / name for name in DEFAULT_ARCHIVES]
    archive_paths = [p.resolve() for p in archive_paths]
    missing = [str(p) for p in archive_paths if not p.is_file()]
    if missing:
        print("FAILED: private archive(s) missing, no census produced:\n  " + "\n  ".join(missing),
              file=sys.stderr)
        return 2
    start = time.monotonic()

    identities, population = [], []
    for path in archive_paths:
        archive = Psarc(path)
        try:
            members = archive.members()  # MD5(path) pairing only, never positional entries
            csdr = [(name, index) for name, index in members if name.endswith(".csdr")]
        finally:
            archive.close()
        relative = str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path)
        identity = {"name": path.name, "path": relative, "bytes": path.stat().st_size,
                    "mtime_ns": path.stat().st_mtime_ns, "members": len(members),
                    "csdr_members": len(csdr)}
        if args.hash_archives:
            identity["sha256"] = digest_file(path)
        identities.append(identity)
        population.extend((name, index, path) for name, index in csdr)
        print(f"{path.name}: {len(members)} members, {len(csdr)} .csdr", flush=True)
    population.sort(key=lambda item: (item[0], item[2].name))

    indices = sample_indices(len(population), args.sample)
    sample_info = {
        "requested": args.sample, "selected": len(indices),
        "population": len(population),
        "rule": "deterministic even spacing round(i*(N-1)/(k-1)) over the sorted "
                "[(path, archive)] list; first and last entries included",
        "indices": indices,
        "implied_stride": round(len(population) / len(indices), 3),
    }

    handles, rows = {}, []
    try:
        for position in indices:
            name, index, path = population[position]
            archive = handles.get(path)
            if archive is None:
                archive = handles[path] = Psarc(path)
            try:
                rows.append(classify_member(path.name, name, index, archive.read(index)))
            except (OSError, ValueError) as exc:
                rows.append(read_failure(path.name, name, index, exc))
    finally:
        for archive in handles.values():
            archive.close()
    rows.sort(key=lambda r: (r["path"], r["archive"]))

    summary = summarize(rows, population, sample_info, identities)
    summary["evidence"] = {"census": "STATIC_ANALYSIS_VERIFIED",
                           "self_test": "FIXTURE_VERIFIED"}
    summary["tool"] = {
        "path": str(Path(__file__).resolve().relative_to(ROOT)),
        "sha256": digest_file(Path(__file__).resolve()),
        "inputs": {str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p): digest_file(p)
                   for p in (HERE / "gcn_inventory.py", ROOT / "scripts/psarc.py",
                             HERE / "gpu_asset_probe.py")},
        "python": sys.version.split()[0], "host": platform.platform(),
        "elapsed_seconds": round(time.monotonic() - start, 3),
    }
    summary["limits"] = [
        "front-end coverage only: container readability and gcn_inventory decode, no lowering",
        "zero real shaders lowered because no per-shader ABI manifest exists",
        "supported-opcode share is 1.0 for every decoded program by decoder contract",
        "compressed (8C/CC) members are classified, never decompressed",
        "stage is an INF nibble label from filename co-occurrence, not verified semantics",
        "no shader execution, no Metal compile, no pipeline state, G3 NOT RUN",
    ]
    target = write_output(out, summary, args.overwrite)
    for line in human_summary(summary, target):
        print(line)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("archives", nargs="*", type=Path,
                        help="PSARC paths; default: the three pinned pak archives")
    parser.add_argument("--sample", type=int, default=DEFAULT_SAMPLE,
                        help=f"members to sample deterministically (default {DEFAULT_SAMPLE})")
    parser.add_argument("--out", type=Path, default=None,
                        help=f"private output directory (default {DEFAULT_OUT})")
    parser.add_argument("--self-test", action="store_true", help="run the synthetic-span tests")
    parser.add_argument("--overwrite", action="store_true", help="replace an earlier coverage.json")
    parser.add_argument("--hash-archives", action="store_true",
                        help="also record full archive SHA-256 (reads every archive byte)")
    args = parser.parse_args(argv)
    try:
        if args.self_test:
            return self_test()
        return run(args)
    except (OSError, ValueError) as exc:
        print(f"FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
