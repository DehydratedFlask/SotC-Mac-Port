#!/usr/bin/env python3
"""Verify the raw default material's shader path and replay its pinned shader pair.

This is an offline, asset-only fixture. It resolves exact PSARC paths through the
MD5(path) map, validates the complete raw members against fixed digests, and checks
that the selected CSDR programs reproduce the E627 MSL and CPU/Metal oracle result.
It does not decode the material constants or establish that the game consumes them.
All extracted bytes and generated outputs must stay under this workspace's local/.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
sys.path.insert(0, str(ROOT / "tools" / "dev"))

from psarc import Psarc, PsarcError, path_md5  # noqa: E402
from gpu_asset_probe import ContainerError, parse_raw_csdr  # noqa: E402


MATERIAL_PATH = "coredata/enginesupport/materials/_ps4/default.cmat"
SHADER_PATH = (
    "materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/"
    "standard_4030788106.csdr"
)
SELECTOR_BYTES = ("$/" + SHADER_PATH).encode("ascii")
MATERIAL_SIZE = 171
MATERIAL_SHA256 = "0c2233c1aab20cfc1da914de96cfd1cbd0a20e37964024d54e10e36b260c6d29"
SHADER_SIZE = 1988
SHADER_SHA256 = "c4214e4ee0852a962ce56e0cab0befbe3169698cb354bc62afa7eab4d1f3f356"

EXPECTED_PROGRAMS = (
    {
        "name": "default-vs",
        "code_offset": 564,
        "length": 272,
        "info_offset": 972,
        "code_sha256": "5a4f11f29a8d46a34240a0d144ff1e5fbdb4a6cb9a2f309a18fcfdc06b67e7f7",
    },
    {
        "name": "default-ps",
        "code_offset": 1368,
        "length": 240,
        "info_offset": 1744,
        "code_sha256": "4af7cb6973d4743a98f406eb34a60d8c5cdd73837fb650eb16050be0d700f00b",
    },
)

BASELINE = ROOT / "local" / "fixtures" / "gpu-r30"
ABI_PATH = BASELINE / "default-abi.json"
ABI_SHA256 = "dd6908b1f5cfea1509c99b739abfdd52f4a4b79f6312147d2c5598d197b45734"
BASELINE_MSL = BASELINE / "default-pair.metal"
MSL_SHA256 = "5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e"
BASELINE_VS = BASELINE / "default-vs.bin"
BASELINE_PS = BASELINE / "default-ps.bin"
METAL_VERIFY_SOURCE = ROOT / "tools" / "dev" / "metal_shader_verify.mm"


class VerificationError(ValueError):
    """Input or replay data does not match the closed, supported fixture."""


class UnknownMaterialSelector(VerificationError):
    """The material bytes do not prove one unambiguous shader path."""


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def validate_pinned_bytes(data: bytes, *, label: str, size: int, digest: str) -> None:
    if len(data) != size:
        raise VerificationError(f"{label} size {len(data)} != pinned {size}")
    actual = sha256(data)
    if actual != digest:
        raise VerificationError(f"{label} SHA-256 {actual} != pinned {digest}")


def find_material_shader(cmat: bytes) -> dict:
    """Recognize only the single pinned CSDR path literal; do not parse constants."""
    suffixes = list(re.finditer(re.escape(b".csdr"), cmat))
    if len(suffixes) != 1:
        raise UnknownMaterialSelector(f"material has {len(suffixes)} .csdr suffixes; expected one")
    count = cmat.count(SELECTOR_BYTES)
    offset = cmat.find(SELECTOR_BYTES)
    if count != 1:
        raise UnknownMaterialSelector(f"pinned shader path occurs {count} times; expected one")
    if suffixes[0].start() + len(b".csdr") != offset + len(SELECTOR_BYTES):
        raise UnknownMaterialSelector("the only .csdr suffix is not the pinned shader path")
    if suffixes[0].start() < offset:
        raise UnknownMaterialSelector("shader path suffix precedes the pinned path")
    return {"path": SHADER_PATH, "literal": SELECTOR_BYTES.decode("ascii"), "offset": offset}


def validate_program_spans(csdr: bytes) -> tuple[list[dict], list[bytes]]:
    try:
        programs = parse_raw_csdr(csdr)
    except ContainerError as exc:
        raise VerificationError(f"raw CSDR parser rejected pinned member: {exc}") from exc
    if len(programs) != len(EXPECTED_PROGRAMS):
        raise VerificationError(f"CSDR has {len(programs)} programs; expected two")

    codes = []
    rows = []
    for ordinal, (program, expected) in enumerate(zip(programs, EXPECTED_PROGRAMS, strict=True)):
        for key in ("code_offset", "length", "info_offset"):
            if program[key] != expected[key]:
                raise VerificationError(
                    f"program {ordinal} {key} {program[key]} != pinned {expected[key]}"
                )
        start = program["code_offset"]
        end = start + program["length"]
        if start < 0 or end > len(csdr):
            raise VerificationError(f"program {ordinal} code span is outside the CSDR")
        code = csdr[start:end]
        code_hash = sha256(code)
        if code_hash != expected["code_sha256"]:
            raise VerificationError(f"program {ordinal} code SHA-256 mismatch")
        codes.append(code)
        rows.append({
            "ordinal": ordinal,
            "name": expected["name"],
            "type": program["type"],
            "source_type": program["source_type"],
            "version": program["version"],
            "code_offset": start,
            "length": len(code),
            "info_offset": program["info_offset"],
            "shader_hash": program["shader_hash"],
            "crc32": program["crc32"],
            "code_sha256": code_hash,
        })
    return rows, codes


def _workspace_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def _input_file(path: Path, label: str) -> Path:
    candidate = _workspace_path(path)
    if candidate.is_symlink():
        raise VerificationError(f"{label} archive must not be a symlink")
    try:
        resolved = candidate.resolve(strict=True)
    except OSError as exc:
        raise VerificationError(f"{label} archive is unavailable: {candidate}") from exc
    if not resolved.is_file():
        raise VerificationError(f"{label} archive is not a regular file: {resolved}")
    if not resolved.is_relative_to(ROOT):
        raise VerificationError(f"{label} archive resolves outside the workspace: {resolved}")
    local_path = ROOT / "local"
    if local_path.is_symlink():
        raise VerificationError("workspace local/ must not be a symlink")
    try:
        local_root = local_path.resolve(strict=True)
    except OSError as exc:
        raise VerificationError("workspace local/ is unavailable") from exc
    if not local_root.is_dir():
        raise VerificationError("workspace local/ is not a directory")
    if not resolved.is_relative_to(local_root):
        raise VerificationError(f"{label} archive resolves outside private local/: {resolved}")
    return resolved


def _output_path(path: Path) -> Path:
    local_root = (ROOT / "local").resolve(strict=True)
    if (ROOT / "local").is_symlink():
        raise VerificationError("workspace local/ must not be a symlink")
    candidate = _workspace_path(path)
    if candidate.name in ("", ".", ".."):
        raise VerificationError("output must name a new directory under local/")
    if candidate.exists() or candidate.is_symlink():
        raise VerificationError(f"output already exists; refusing to overwrite: {candidate}")
    try:
        parent = candidate.parent.resolve(strict=True)
    except OSError as exc:
        raise VerificationError(f"output parent must already exist: {candidate.parent}") from exc
    if candidate.parent.absolute() != parent:
        raise VerificationError("output parent must not traverse a symlink or unresolved path")
    if not parent.is_relative_to(local_root):
        raise VerificationError("private outputs must be under this workspace's local/")
    return parent / candidate.name


def read_member(archive_path: Path, member_path: str, *, label: str,
                size: int, digest: str) -> tuple[bytes, dict]:
    archive = Psarc(archive_path)
    try:
        index = archive.entry_index(member_path)
        entry = archive.entries[index]
        expected_md5 = path_md5(member_path)
        if entry["md5"] != expected_md5:
            raise VerificationError(f"{label} entry does not match MD5(exact path)")
        if entry["size"] != size:
            raise VerificationError(f"{label} TOC size {entry['size']} != pinned {size}")
        data = archive.read(index)
    except PsarcError as exc:
        raise VerificationError(f"{label} PSARC lookup/read failed: {exc}") from exc
    finally:
        archive.close()

    validate_pinned_bytes(data, label=label, size=size, digest=digest)
    return data, {
        "archive": str(archive_path.relative_to(ROOT)),
        "path": member_path,
        "entry_index": index,
        "entry_md5": entry["md5"],
        "path_md5": expected_md5,
        "toc_size": entry["size"],
        "size": len(data),
        "sha256": sha256(data),
    }


def _require_baseline_file(path: Path, expected_sha256: str, label: str) -> bytes:
    try:
        data = path.read_bytes()
    except OSError as exc:
        raise VerificationError(f"{label} baseline is unavailable: {path}") from exc
    actual = sha256(data)
    if actual != expected_sha256:
        raise VerificationError(f"{label} baseline SHA-256 {actual} != pinned {expected_sha256}")
    return data


def _run(command: list[str], *, cwd: Path, log_path: Path, env: dict | None = None) -> None:
    merged_env = os.environ.copy()
    if env:
        merged_env.update(env)
    with log_path.open("x", encoding="utf-8") as log:
        completed = subprocess.run(command, cwd=cwd, env=merged_env, stdout=log,
                                   stderr=subprocess.STDOUT, text=True, check=False)
    if completed.returncode != 0:
        raise VerificationError(
            f"command exited {completed.returncode}; see {log_path.relative_to(ROOT)}"
        )


def run(args: argparse.Namespace) -> dict:
    cachedfiles = _input_file(args.cachedfiles, "cachedFiles")
    pak0 = _input_file(args.pak0, "pak0")
    output = _output_path(args.output)

    try:
        cmat, cmat_info = read_member(
            cachedfiles, MATERIAL_PATH, label="default.cmat", size=MATERIAL_SIZE,
            digest=MATERIAL_SHA256,
        )
    except VerificationError as exc:
        raise UnknownMaterialSelector(f"pinned material could not be proved: {exc}") from exc
    selector = find_material_shader(cmat)
    if selector["path"] != SHADER_PATH:
        raise VerificationError("material selected an unexpected shader path")
    csdr, csdr_info = read_member(
        pak0, selector["path"], label="default CSDR", size=SHADER_SIZE,
        digest=SHADER_SHA256,
    )
    program_rows, codes = validate_program_spans(csdr)

    baseline_vs = _require_baseline_file(BASELINE_VS, EXPECTED_PROGRAMS[0]["code_sha256"], "VS")
    baseline_ps = _require_baseline_file(BASELINE_PS, EXPECTED_PROGRAMS[1]["code_sha256"], "PS")
    if codes != [baseline_vs, baseline_ps]:
        raise VerificationError("selected CSDR program bytes differ from the E627 stage inputs")
    abi = _require_baseline_file(ABI_PATH, ABI_SHA256, "default ABI")
    baseline_msl = _require_baseline_file(BASELINE_MSL, MSL_SHA256, "default MSL")
    if not METAL_VERIFY_SOURCE.is_file() or METAL_VERIFY_SOURCE.is_symlink():
        raise VerificationError(f"Metal verifier source is unavailable or a symlink: {METAL_VERIFY_SOURCE}")
    metal_source_sha256 = sha256(METAL_VERIFY_SOURCE.read_bytes())

    # All pins and structural checks pass before this exclusive output directory is created.
    output.mkdir(parents=False, exist_ok=False)
    (output / "default.cmat").write_bytes(cmat)
    (output / "default.csdr").write_bytes(csdr)
    (output / "default-vs.bin").write_bytes(codes[0])
    (output / "default-ps.bin").write_bytes(codes[1])

    metal_binary = output / "metal_shader_verify"
    metal_build_command = [
        "xcrun", "clang++", "-arch", "arm64", "-std=c++20", "-fobjc-arc",
        "-framework", "Foundation", "-framework", "Metal",
        str(METAL_VERIFY_SOURCE), "-o", str(metal_binary),
    ]
    _run(metal_build_command, cwd=ROOT, log_path=output / "metal-build.log")
    if not metal_binary.is_file() or metal_binary.is_symlink():
        raise VerificationError("freshly compiled arm64 Metal verifier is missing or a symlink")

    lower_command = [
        sys.executable, "-I", str(ROOT / "tools/dev/gcn_to_msl.py"),
        "--vs", str(output / "default-vs.bin"),
        "--ps", str(output / "default-ps.bin"),
        "--abi", str(ABI_PATH),
        "--output", str(output / "default-pair.metal"),
        "--report", str(output / "default-lowering.json"),
    ]
    _run(lower_command, cwd=ROOT, log_path=output / "lowerer.log")
    msl = (output / "default-pair.metal").read_bytes()
    lowering = json.loads((output / "default-lowering.json").read_text())
    actual_msl_hash = sha256(msl)
    if actual_msl_hash != MSL_SHA256 or msl != baseline_msl:
        raise VerificationError(f"generated MSL differs from the E627 baseline ({actual_msl_hash})")
    if lowering.get("msl_sha256") != MSL_SHA256:
        raise VerificationError("lowerer report does not match the pinned MSL digest")

    fixture_command = [
        sys.executable, "-I", "-W", "error",
        str(ROOT / "tools/dev/default_shader_oracle.py"), "fixture", str(output),
    ]
    _run(fixture_command, cwd=ROOT, log_path=output / "oracle-fixture.log")
    metal_command = [
        str(metal_binary), str(output / "default-pair.metal"),
        str(output / "harness-inputs.bin"), str(output / "metal-output.bin"),
        str(output / "offscreen.rgba32f"),
    ]
    _run(metal_command, cwd=ROOT, log_path=output / "metal-execution.log",
         env={"MTL_DEBUG_LAYER": "1", "MTL_SHADER_VALIDATION": "1"})
    compare_command = [
        sys.executable, "-I", "-W", "error",
        str(ROOT / "tools/dev/default_shader_oracle.py"), "compare", str(output),
    ]
    _run(compare_command, cwd=ROOT, log_path=output / "oracle-comparison.log")
    comparison = json.loads((output / "oracle-comparison.json").read_text())
    if (comparison.get("status") != "PASSED" or comparison.get("cases") != 2141
            or comparison.get("components") != 34256
            or comparison.get("max_absolute_error") != 0):
        raise VerificationError("Metal/oracle output did not reproduce the pinned E627 result")

    summary = {
        "status": "FIXTURE_VERIFIED",
        "task": "GPU-02-RAW-MATERIAL-SELECTOR-01",
        "scope": "raw material path selection + exact E627 shader-pair replay",
        "members": {"material": cmat_info, "shader": csdr_info},
        "selector": selector,
        "programs": program_rows,
        "e627": {
            "abi_sha256": sha256(abi),
            "msl_sha256": actual_msl_hash,
            "oracle_status": comparison["status"],
            "oracle_cases": comparison["cases"],
            "oracle_components": comparison["components"],
            "oracle_max_absolute_error": comparison["max_absolute_error"],
            "oracle_finite_alpha_exact": comparison["finite_alpha_exact"],
        },
        "metal_verifier": {
            "source": str(METAL_VERIFY_SOURCE.relative_to(ROOT)),
            "source_sha256": metal_source_sha256,
            "build_command": metal_build_command,
            "build_log": str((output / "metal-build.log").relative_to(ROOT)),
            "binary": str(metal_binary.relative_to(ROOT)),
            "execution_environment": {
                "MTL_DEBUG_LAYER": "1",
                "MTL_SHADER_VALIDATION": "1",
            },
        },
        "commands": [metal_build_command, lower_command, fixture_command,
                     metal_command, compare_command],
        "limitations": [
            "literal path bytes are observed; engine consumption of the material is not established",
            "material constants/layout, retail descriptors, bindings and runtime use remain UNKNOWN",
            "no 0x8C decode, mesh/texture interpretation, game frame, or G3 claim",
        ],
    }
    (output / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    return summary


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cachedfiles", type=Path,
                        default=Path("local/extracted/CUSA08809/cachedFiles.psarc"))
    parser.add_argument("--pak0", type=Path,
                        default=Path("local/extracted/CUSA08809/pak0.psarc"))
    parser.add_argument("--output", type=Path, required=True,
                        help="new private output directory under local/")
    args = parser.parse_args(argv)
    try:
        result = run(args)
    except UnknownMaterialSelector as exc:
        print(f"UNKNOWN: {exc}", file=sys.stderr)
        return 2
    except (OSError, PsarcError, VerificationError, ContainerError, json.JSONDecodeError) as exc:
        print(f"FAILED: {exc}", file=sys.stderr)
        return 1
    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
