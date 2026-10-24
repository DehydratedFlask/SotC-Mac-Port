#!/usr/bin/env python3
"""Protocol/exit-code checker for the native SoCGameCore auxiliary executable.

APP-01-SOCGAMECORE-EXE-01. Spawns the built arm64 Mach-O `SoCGameCore` and pins the v1 line
protocol: schemas, key order, exit codes, the honest `--start` refusal, `--serve` framing, and the
fail-visible slot-23 fixture path. It reads no game executable and executes no original RVA.

Usage:
  python3 -I tools/lift/check_fcore_status.py                 # every check, default build dir
  SOTC_NATIVE_BUILD=local/build/orch-r22 python3 -I tools/lift/check_fcore_status.py
  python3 -I tools/lift/check_fcore_status.py --exe <path> --only serve

Exit 0 on success, 1 on a protocol violation, 2 when the executable is not built.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ARCHIVES = ("cachedFiles.psarc", "pak0.psarc", "pak1.psarc", "pak2.psarc", "pak3.psarc", "pakN.psarc")
STATUS_SCHEMA = "sotc.core.status.v1"
COVERAGE_SCHEMA = "sotc.core.coverage.v1"
VALIDATE_SCHEMA = "sotc.core.validate.v1"
START_SCHEMA = "sotc.core.start.v1"
START_LINE = (
    '{"schema":"sotc.core.start.v1","ok":false,"error":4,'
    '"error_name":"SOTC_ERR_ENGINE_NOT_IMPLEMENTED"}'
)
SUBSYSTEMS = ("startup_constructors", "engine_boot", "metal_renderer", "audio", "save_load")
SLOT23_RVA = 0xE71990


def fail(message: str) -> None:
    raise AssertionError(message)


def check(condition: bool, message: str) -> None:
    if not condition:
        fail(message)


def default_exe() -> Path:
    build = Path(os.environ.get("SOTC_NATIVE_BUILD", "local/build/orch-r22"))
    if not build.is_absolute():
        build = ROOT / build
    return build / "SoCGameCore"


def run(exe: Path, args: list[str], stdin: str | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [str(exe), *args],
        input=stdin,
        text=True,
        capture_output=True,
        cwd=str(ROOT),
        timeout=120,
        check=False,
    )


def one_json_line(proc: subprocess.CompletedProcess[str], schema: str) -> tuple[str, dict]:
    lines = proc.stdout.splitlines()
    check(len(lines) == 1, f"expected exactly one stdout line, got {len(lines)}: {proc.stdout!r}")
    line = lines[0]
    check(line.startswith('{"schema":"%s",' % schema), f"schema is not the first field: {line!r}")
    try:
        obj = json.loads(line)
    except json.JSONDecodeError as exc:  # pragma: no cover - explicit message on corrupt bytes
        fail(f"stdout is not JSON: {line!r} ({exc})")
    check(obj["schema"] == schema, f"schema mismatch: {obj['schema']!r}")
    check(proc.stderr == "", f"clean command wrote to stderr: {proc.stderr!r}")
    return line, obj


def check_status(exe: Path) -> str:
    proc = run(exe, ["--status"])
    check(proc.returncode == 0, f"--status exit {proc.returncode}, stderr {proc.stderr!r}")
    line, obj = one_json_line(proc, STATUS_SCHEMA)
    check(list(obj) == ["schema", "abi", "state", "last_error", "last_error_name", "assets_validated",
                        "assets_ready", "engine_implemented", "ctor"], f"status key order changed: {line!r}")
    check(obj["abi"] == (1 << 16), f"abi {obj['abi']} != {1 << 16}")
    check(obj["engine_implemented"] is False, "status claims an engine")
    check(obj["state"] in {"idle", "validating", "ready", "starting", "running", "stopping", "failed"},
          f"state {obj['state']!r} is not an ABI state name")
    check(obj["last_error_name"].startswith("SOTC_"), f"error name {obj['last_error_name']!r}")
    ctor = obj["ctor"]
    check(list(ctor) == ["linked", "original_slots", "slots_registered", "slots_with_callback",
                         "completed", "run_ok", "callback_failed", "blocked_on_rva",
                         "first_unrecovered_index"], f"ctor key order changed: {ctor!r}")
    check(ctor["linked"] is True, "constructor table is not linked")
    check(ctor["original_slots"] == 573, f"original_slots {ctor['original_slots']} != 573")
    check(0 < ctor["slots_registered"] <= 573, f"slots_registered {ctor['slots_registered']}")
    check(ctor["slots_with_callback"] <= ctor["slots_registered"], "callbacks exceed registered slots")
    check(ctor["completed"] <= ctor["slots_registered"], "completed exceeds registered slots")
    check(ctor["run_ok"] == (ctor["blocked_on_rva"] == 0 and not ctor["callback_failed"]),
          "run_ok disagrees with blocked_on_rva/callback_failed")
    index = ctor["first_unrecovered_index"]
    check(index is None or 0 <= index <= 572, f"first_unrecovered_index {index!r} outside 0..572")
    fixture = ROOT / "local/fixtures/slot23_e71990_names.bin"
    measured = (f"state={obj['state']} slots={ctor['slots_registered']} completed={ctor['completed']} "
                f"run_ok={ctor['run_ok']} first_unrecovered={index}")
    if fixture.is_file():
        # The repo-root probe must find the private fixture; a full run must not fail a callback.
        check(ctor["run_ok"] is True, "status with the private fixture present reports run_ok=false")
        check(ctor["callback_failed"] is False, "status with the private fixture present failed a callback")

    missing = ROOT / "local/fixtures/this-fixture-does-not-exist.bin"
    bad = run(exe, ["--fixture", str(missing), "--status"])
    check(bad.returncode == 0, f"missing-fixture --status exit {bad.returncode}")
    bad_obj = json.loads(bad.stdout)
    bad_ctor = bad_obj["ctor"]
    check(bad_obj["engine_implemented"] is False, "missing-fixture status claims an engine")
    check(bad_ctor["run_ok"] is False, "missing fixture did not surface as run_ok=false")
    check(bad_ctor["callback_failed"] is True, "missing fixture did not surface as callback_failed=true")
    check(bad_ctor["completed"] == 23, f"missing fixture completed={bad_ctor['completed']} (want slot 23 stop)")
    check(bad_ctor["blocked_on_rva"] == SLOT23_RVA,
          f"missing fixture blocked_on_rva=0x{bad_ctor['blocked_on_rva']:x} (want 0xe71990)")
    return (measured + f"; missing fixture -> run_ok=false callback_failed=true "
            f"blocked_on_rva=0x{bad_ctor['blocked_on_rva']:x} completed={bad_ctor['completed']}")


def check_coverage(exe: Path) -> str:
    proc = run(exe, ["--coverage"])
    check(proc.returncode == 0, f"--coverage exit {proc.returncode}, stderr {proc.stderr!r}")
    line, obj = one_json_line(proc, COVERAGE_SCHEMA)
    check(list(obj) == ["schema", "abi", "engine_implemented", "subsystems"], f"coverage keys: {line!r}")
    check(obj["abi"] == (1 << 16), f"abi {obj['abi']}")
    check(obj["engine_implemented"] is False, "coverage claims an engine")
    rows = obj["subsystems"]
    check([row["name"] for row in rows] == list(SUBSYSTEMS), f"subsystem order {[r['name'] for r in rows]}")
    for row in rows:
        check(row["status"] in {"not_implemented", "partial", "implemented", "not_linked"},
              f"unknown status {row['status']!r}")
        check(row["status"] != "implemented", f"{row['name']} claims implemented")
        check(row["detail"] != "", f"{row['name']} has an empty detail")
    by_name = {row["name"]: row for row in rows}
    startup = by_name["startup_constructors"]
    check(startup["status"] == "partial", f"startup_constructors status {startup['status']!r}")
    match = re.search(r"^(\d+) of 573 original constructor slots recovered; first unrecovered index (\d+|none); ",
                      startup["detail"])
    check(match is not None, f"startup_constructors detail not the computed form: {startup['detail']!r}")
    recovered = int(match.group(1))
    check(0 < recovered <= 573, f"coverage recovered count {recovered}")
    status_proc = run(exe, ["--status"])
    status_obj = json.loads(status_proc.stdout)
    check(status_proc.returncode == 0, f"--status for cross-check exit {status_proc.returncode}")
    expected_index = status_obj["ctor"]["first_unrecovered_index"]
    if match.group(2) == "none":
        check(expected_index is None, "coverage says no unrecovered slot but status reports one")
    else:
        check(int(match.group(2)) == expected_index,
              f"coverage index {match.group(2)} != status index {expected_index}")
    check(recovered <= status_obj["ctor"]["slots_registered"],
          "coverage recovered count exceeds the registered slots")
    check(by_name["save_load"]["status"] == "partial", "save_load is not PARTIAL")
    check("no platform save or real PS4 acceptance" in by_name["save_load"]["detail"],
          f"save_load detail: {by_name['save_load']['detail']!r}")
    for name in ("engine_boot", "metal_renderer", "audio"):
        check(by_name[name]["status"] == "not_implemented", f"{name} is not NOT_IMPLEMENTED")
    return (f"5 rows, startup_constructors=partial ({recovered} of 573, first unrecovered "
            f"{match.group(2)}), save_load=partial, no implemented rows")


def check_start(exe: Path) -> str:
    proc = run(exe, ["--start"])
    check(proc.returncode == 3, f"--start exit {proc.returncode} (want 3)")
    check(proc.stdout.strip() == START_LINE, f"--start line changed: {proc.stdout!r}")
    obj = json.loads(proc.stdout)
    check(obj["ok"] is False, "start claims ok")
    check(obj["error"] == 4 and obj["error_name"] == "SOTC_ERR_ENGINE_NOT_IMPLEMENTED",
          f"start error {obj['error']} {obj['error_name']!r}")
    with tempfile.TemporaryDirectory(prefix="sotc_core_assets_") as assets:
        for name in ARCHIVES:
            (Path(assets) / name).write_bytes(b"PSAR0000")
        with_assets = run(exe, ["--assets", assets, "--start"])
        check(with_assets.returncode == 3, f"--assets --start exit {with_assets.returncode} (want 3)")
        check(with_assets.stdout.strip() == START_LINE, f"--assets --start line: {with_assets.stdout!r}")
    return "exit 3, exact ok=false error=4 line, also with a validated assets directory"


def check_validate(exe: Path) -> str:
    with tempfile.TemporaryDirectory(prefix="sotc_core_empty_") as empty:
        proc = run(exe, ["--validate", empty])
        check(proc.returncode == 0, f"validate(empty) exit {proc.returncode} (want 0)")
        _, obj = one_json_line(proc, VALIDATE_SCHEMA)
        check(obj["ok"] is False and obj["error"] == 3, f"validate(empty) verdict {obj['error']}")
        check(obj["error_name"] == "SOTC_ERR_ASSETS_NOT_READY", obj["error_name"])
        check(obj["assets_ready"] is False and obj["engine_ready"] is False
              and obj["start_would_succeed"] is False, "validate(empty) claims readiness")
        check([row["status"] for row in obj["archives"]] == ["missing"] * 6,
              f"validate(empty) archive statuses {[r['status'] for r in obj['archives']]}")
        check([row["name"] for row in obj["archives"]] == list(ARCHIVES), "archive order/names changed")
        check(all(row["size_bytes"] == 0 for row in obj["archives"]), "missing archives report a size")
        empty_detail = obj["detail"]

    with tempfile.TemporaryDirectory(prefix="sotc_core_good_") as good:
        for name in ARCHIVES:
            (Path(good) / name).write_bytes(b"PSAR01234567")
        proc = run(exe, ["--validate", good])
        check(proc.returncode == 0, f"validate(good) exit {proc.returncode} (want 0)")
        _, obj = one_json_line(proc, VALIDATE_SCHEMA)
        check(obj["ok"] is True and obj["error"] == 0, f"validate(good) error {obj['error']}")
        check(obj["assets_ready"] is True, "validate(good) assets_ready=false")
        check(obj["engine_ready"] is False, "validate(good) claims engine_ready")
        check(obj["start_would_succeed"] is False, "validate(good) claims start would succeed")
        check([row["status"] for row in obj["archives"]] == ["ok"] * 6, "validate(good) statuses")
        check(all(row["size_bytes"] == 12 for row in obj["archives"]),
              f"validate(good) sizes {[r['size_bytes'] for r in obj['archives']]}")
        check("engine not implemented" in obj["detail"], f"validate(good) detail: {obj['detail']!r}")

    with tempfile.TemporaryDirectory(prefix="sotc_core_file_") as parent:
        regular = Path(parent) / "not-a-directory"
        regular.write_bytes(b"x")
        proc = run(exe, ["--validate", str(regular)])
        check(proc.returncode == 5, f"validate(regular file) exit {proc.returncode} (want 5)")
        _, obj = one_json_line(proc, VALIDATE_SCHEMA)
        check(obj["ok"] is False and obj["error"] == 5 and obj["error_name"] == "SOTC_ERR_IO",
              f"validate(file) verdict {obj['error']} {obj['error_name']!r}")
        check(obj["assets_ready"] is False, "validate(file) assets_ready=true")

    usage = run(exe, ["--validate"])
    check(usage.returncode == 2, f"--validate without a directory exit {usage.returncode} (want 2)")
    check(usage.stdout == "", f"usage error wrote stdout: {usage.stdout!r}")
    return (f"empty=exit 0 6x missing ({empty_detail!r}); good=exit 0 6x ok assets_ready=true "
            f"start_would_succeed=false; regular-file=exit 5 SOTC_ERR_IO; no-arg=exit 2")


def serve_lines(exe: Path, requests: str, *, allow_stderr: bool = False) -> list[dict]:
    proc = run(exe, ["--serve"], stdin=requests)
    check(proc.returncode == 0, f"--serve exit {proc.returncode}, stderr {proc.stderr!r}")
    lines = proc.stdout.splitlines()
    check(len(lines) == len(requests.splitlines()),
          f"--serve emitted {len(lines)} lines for {len(requests.splitlines())} requests: {proc.stdout!r}")
    objects = []
    for line in lines:
        check(line.startswith('{"id":'), f"serve line does not start with id: {line!r}")
        objects.append(json.loads(line))
    if not allow_stderr:
        check(proc.stderr == "", f"clean serve session wrote stderr: {proc.stderr!r}")
    return objects


def write_assets(directory: Path) -> None:
    for name in ARCHIVES:
        (directory / name).write_bytes(b"PSAR01234567")


def check_serve(exe: Path) -> str:
    objects = serve_lines(exe, "1 status\n2 quit\n")
    check([obj["id"] for obj in objects] == [1, 2], f"serve ids {[o['id'] for o in objects]}")
    check(objects[0]["schema"] == STATUS_SCHEMA and objects[0]["engine_implemented"] is False,
          "serve status row")
    check(objects[1] == {"id": 2, "ok": True, "bye": True}, f"quit reply {objects[1]!r}")

    malformed = serve_lines(exe, "nope\n5 bogus\n6 validate\n7\n\n8 coverage\n9 quit\n")
    check([obj["id"] for obj in malformed] == [0, 5, 6, 7, 0, 8, 9],
          f"bad-request ids {[o['id'] for o in malformed]}")
    for index in (0, 1, 2, 3, 4):
        check(malformed[index]["ok"] is False and malformed[index]["error"] == "bad_request",
              f"request {index} not refused: {malformed[index]!r}")
    check(malformed[5]["schema"] == COVERAGE_SCHEMA, "coverage after bad requests")

    eof = run(exe, ["--serve"], stdin="")
    check(eof.returncode == 0 and eof.stdout == "", f"EOF session exit {eof.returncode} out {eof.stdout!r}")
    two_then_eof = run(exe, ["--serve"], stdin="1 status\n2 status\n")
    check(two_then_eof.returncode == 0 and len(two_then_eof.stdout.splitlines()) == 2,
          "EOF after two requests did not exit 0 with two lines")

    with tempfile.TemporaryDirectory(prefix="sotc_core_serve_") as empty:
        sequence = serve_lines(exe, f"1 validate {empty}\n2 status\n3 start\n4 status\n5 stop\n6 quit\n",
                               allow_stderr=True)
    check([obj["id"] for obj in sequence] == [1, 2, 3, 4, 5, 6], "validate/start sequence ids")
    check(sequence[0]["schema"] == VALIDATE_SCHEMA and sequence[0]["ok"] is False, "serve validate row")
    check(sequence[1]["assets_validated"] is True and sequence[1]["assets_ready"] is False,
          "serve status after failed validation")
    check(sequence[2]["error"] == 4 and sequence[2]["error_name"] == "SOTC_ERR_ENGINE_NOT_IMPLEMENTED",
          "serve start is not the engine refusal")
    check(sequence[3]["engine_implemented"] is False and sequence[3]["state"] in {"idle", "failed"},
          f"serve status after start: {sequence[3]['state']!r}")
    check(sequence[4]["ok"] is True, "serve stop not ok")

    with tempfile.TemporaryDirectory(prefix="sotc_core_ready_") as good:
        write_assets(Path(good))
        ready = serve_lines(exe, f"1 validate {good}\n2 status\n3 start\n4 status\n5 quit\n")
    check(ready[0]["ok"] is True and ready[0]["assets_ready"] is True, "serve good validation")
    check(ready[1]["state"] == "ready" and ready[1]["assets_ready"] is True, "serve status not ready")
    check(ready[2]["error"] == 4 and ready[2]["ok"] is False, "serve start not refused")
    check(ready[3]["state"] == "failed" and ready[3]["last_error"] == 4
          and ready[3]["engine_implemented"] is False, f"serve status after refusal {ready[3]!r}")
    return ("status+quit=2 lines; malformed ids [0,5,6,7,0]; EOF exit 0; "
            "validate/start/status/stop sequence honest; ready-assets start still error 4, state failed")


CHECKS = {
    "status": check_status,
    "coverage": check_coverage,
    "start": check_start,
    "validate": check_validate,
    "serve": check_serve,
}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--exe", type=Path, default=default_exe(), help="path to the built SoCGameCore")
    parser.add_argument("--only", choices=sorted(CHECKS), action="append", help="run only these checks")
    args = parser.parse_args()
    exe = args.exe if args.exe.is_absolute() else (ROOT / args.exe)
    if not exe.is_file() or not os.access(exe, os.X_OK):
        print(f"SKIP: SoCGameCore not built in {exe.parent} (build the soc_game_core target)")
        return 2
    names = args.only or list(CHECKS)
    for name in names:
        try:
            summary = CHECKS[name](exe)
        except AssertionError as exc:
            print(f"FAILED: {name}: {exc}", file=sys.stderr)
            return 1
        print(f"OK: {name}: {summary}")
    print(f"OK: {exe} passed {len(names)} protocol check(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
