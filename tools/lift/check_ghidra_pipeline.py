#!/usr/bin/env python3
"""Pin the Ghidra headless decompilation pipeline and its output.

Two layers of assertions:

1. **Always** (public tree): the tool paths, the runner/dumper/probe scripts and the pipeline doc
   exist and still name the pinned Ghidra installation, the pinned project and the pinned SHA-256.
   The dossier's expected RVA set is compared with the list in `docs/GHIDRA_PIPELINE.md` so the doc
   and the checker cannot drift apart.
2. **Private inputs** (`local/re/eboot.elf`, the Ghidra project, the dossier):
   - the ELF hashes to the pinned SHA-256;
   - the project contains the `SotC.gpr` marker and the `SotC.rep` database;
   - every RVA in the dossier resolved to a function with the recorded body size, a non-empty
     decompiled body, a plausible C block, and a callee list.

Skips cleanly (exit 0, "SKIP ...") when the private inputs are absent, exactly like the other
private-input checkers. A *present* dossier that is wrong is a failure, never a skip.

Why the sizes are pinned exactly: the guest image is pinned by SHA-256, so Ghidra's function body
size for a given address is deterministic. The recorded sizes differ from the FDE lengths quoted in
`AGENTS.md` (e.g. 0x2569d0 is 12626 here vs 12621 there) because Ghidra's body is the union of
instruction bytes it owns, not the unwind range; both numbers are recorded rather than reconciled.
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GHIDRA_INSTALL = Path("/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC")
GHIDRA_VERSION = "12.1.4"
HEADLESS = GHIDRA_INSTALL / "support/analyzeHeadless"
PROJECT_DIR = ROOT / "local/ghidra/persistent-20261020"
PROJECT_NAME = "SotC"
ELF = ROOT / "local/re/eboot.elf"
PINNED_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

RUNNER = ROOT / "tools/re/ghidra/run_dump.py"
DUMPER = ROOT / "tools/re/ghidra/dump_functions.py"
PROBE = ROOT / "tools/re/ghidra/probe_project.py"
DOC = ROOT / "docs/GHIDRA_PIPELINE.md"
OUT_DIR = ROOT / "local/validation/ghidra-decompile-20261101"
DOSSIER = OUT_DIR / "dossier-20261101.txt"

# entry -> (Ghidra body size in bytes, minimum decompiled line count)
# The minimum line count is a floor, not a pin: it only proves the body is real C, not a stub.
DOSSIER_RVAS = {
    0x2569D0: (12626, 400),
    0x1BDF30: (1479, 40),
    0x1BDB10: (56, 3),
    0xA2AFB0: (441, 10),
    0x531D60: (3889, 100),
}

SECTION = re.compile(r"^=== (0x[0-9a-f]+) ===$")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def check_public_surface() -> None:
    for path in (RUNNER, DUMPER, PROBE, DOC):
        require(path.is_file(), f"missing {path.relative_to(ROOT)}")

    doc = DOC.read_text()
    for needle in (
        str(GHIDRA_INSTALL),
        f"Ghidra {GHIDRA_VERSION}",
        PINNED_SHA256,
        "local/ghidra/persistent-20261020",
        "tools/re/ghidra/run_dump.py",
        "tools/re/ghidra/dump_functions.py",
        "local/validation/ghidra-decompile-20261101",
    ):
        require(needle in doc, f"{DOC.name} no longer documents {needle!r}")

    # The doc is the human-facing list of RVAs; the checker pins the same five.
    for rva in DOSSIER_RVAS:
        require(f"0x{rva:x}" in doc, f"{DOC.name} no longer lists 0x{rva:x}")

    # The runner is the only entry point (analyzeHeadless cannot start Python), and it must keep
    # pointing at the pinned installation rather than inheriting whatever GHIDRA_INSTALL_DIR is set.
    runner = RUNNER.read_text()
    require("import pyghidra" in runner, "run_dump.py no longer imports pyghidra")
    require(str(GHIDRA_INSTALL) in runner, "run_dump.py no longer defaults to the pinned Ghidra installation")
    require("--project-dir" in runner and "--project-name" in runner,
            "run_dump.py no longer takes the project on the command line")

    # The dumper's output format is a contract: this checker parses those exact markers.
    dumper = DUMPER.read_text()
    for marker in ("=== 0x%x ===", "--- decompiled C ---", "--- end decompiled C ---",
                   "DECOMPILE_STATUS=", "CALLEE_COUNT=", "SIZE=", "SIGNATURE="):
        require(marker in dumper, f"dump_functions.py no longer emits {marker!r}")
    probe = PROBE.read_text()
    require('"PROBE %s=%s"' in probe, "probe_project.py no longer emits PROBE key=value lines")
    for key in ("name", "language", "image_base", "function_count", "executable_sha256"):
        require(f'_emit("{key}"' in probe, f"probe_project.py no longer reports {key}")


def check_ghidra_install() -> str:
    if not GHIDRA_INSTALL.is_dir():
        return "ghidra install absent"
    properties = GHIDRA_INSTALL / "Ghidra/application.properties"
    require(properties.is_file(), f"{properties} missing")
    version = ""
    for line in properties.read_text().splitlines():
        if line.startswith("application.version="):
            version = line.split("=", 1)[1].strip()
    require(version == GHIDRA_VERSION, f"Ghidra version is {version!r}, pinned {GHIDRA_VERSION!r}")
    require(HEADLESS.is_file(), f"{HEADLESS} missing")
    return f"Ghidra {version} at {GHIDRA_INSTALL}"


def check_project() -> str:
    if not PROJECT_DIR.is_dir():
        return "project absent"
    require((PROJECT_DIR / f"{PROJECT_NAME}.gpr").is_file(), f"{PROJECT_NAME}.gpr missing")
    require((PROJECT_DIR / f"{PROJECT_NAME}.rep").is_dir(), f"{PROJECT_NAME}.rep missing")
    database = PROJECT_DIR / f"{PROJECT_NAME}.rep/idata"
    require(database.is_dir(), "project database directory missing")
    return f"project {PROJECT_DIR.relative_to(ROOT)}"


def check_elf() -> str:
    if not ELF.is_file():
        return "elf absent"
    digest = hashlib.sha256()
    with ELF.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    require(digest.hexdigest() == PINNED_SHA256,
            f"local/re/eboot.elf is not the pinned image ({digest.hexdigest()})")
    return "elf sha256 pinned"


def parse_dossier(path: Path):
    """Return {rva: {field: value, 'c': text}} from the line-oriented dossier."""
    sections = {}
    current = None
    collecting = False
    lines_out = []
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = SECTION.match(line)
        if match:
            if current is not None:
                current["c"] = "\n".join(lines_out)
            current = {}
            sections[int(match.group(1), 16)] = current
            collecting = False
            lines_out = []
            continue
        if current is None:
            continue
        if line == "--- decompiled C ---":
            collecting = True
            continue
        if line == "--- end decompiled C ---":
            collecting = False
            current["c"] = "\n".join(lines_out)
            continue
        if collecting:
            lines_out.append(line)
            continue
        if "=" in line:
            key, value = line.split("=", 1)
            current.setdefault(key, []).append(value)
    if current is not None and "c" not in current:
        current["c"] = "\n".join(lines_out)
    return sections


def check_dossier() -> str:
    if not DOSSIER.is_file():
        return "dossier absent"
    sections = parse_dossier(DOSSIER)
    require(sections, f"{DOSSIER.name} has no RVA sections")

    body_lines = 0
    for rva, (size, floor) in DOSSIER_RVAS.items():
        require(rva in sections, f"dossier is missing 0x{rva:x}")
        section = sections[rva]
        require(section.get("PRESENT") == ["yes"], f"0x{rva:x} is not a function in the program")
        require(section.get("DECOMPILE_STATUS") == ["ok"], f"0x{rva:x} decompile status is not ok")
        recorded = int(section["SIZE"][0])
        require(recorded == size, f"0x{rva:x} body size {recorded} != pinned {size}")
        require(size > 0, f"0x{rva:x} has an empty body")
        text = section.get("c", "")
        require(text.strip(), f"0x{rva:x} has no decompiled text")
        require("{" in text and "}" in text, f"0x{rva:x} decompiled text is not a C body")
        require("WARNING: Decompiler process died" not in text and "Decompilation failed" not in text,
                f"0x{rva:x} decompiled text contains a decompiler failure")
        c_lines = len(text.splitlines())
        require(c_lines >= floor, f"0x{rva:x} decompiled only {c_lines} lines (floor {floor})")
        body_lines += c_lines
        # A call-heavy body must list its callees; the leaf 0x1bdb10 legitimately has none.
        if rva != 0x1BDB10:
            count = int(section["CALLEE_COUNT"][0])
            require(count > 0, f"0x{rva:x} has no callees recorded")
            require(len(section.get("CALLEE", [])) == count,
                    f"0x{rva:x} records {len(section.get('CALLEE', []))} CALLEE rows for CALLEE_COUNT={count}")

    # C_LINES is the dumper's own count of the text it printed. Compare loosely: the marker lines
    # and a leading blank line make an exact equality test an off-by-one trap with no evidentiary
    # value, while a truncated body is still caught by the per-section line floors above.
    recorded_lines = int(sections[0x2569D0]["C_LINES"][0])
    parsed_lines = len(sections[0x2569D0]["c"].splitlines())
    require(abs(recorded_lines - parsed_lines) <= 2,
            f"0x2569d0 C_LINES={recorded_lines} disagrees with the {parsed_lines} lines in the dossier")

    header = DOSSIER.read_text(encoding="utf-8", errors="replace")
    require(PINNED_SHA256 in header, "dossier was produced from an image with a different SHA-256")
    require("image_base: 0x0" in header, "dossier image base is not 0x0")
    require("x86:LE:64:default" in header, "dossier language is not x86:LE:64:default")
    return f"dossier {len(sections)} sections, {body_lines} decompiled lines"


def main() -> int:
    check_public_surface()

    facts = [check_ghidra_install(), check_project(), check_elf()]
    absent = [fact for fact in facts if fact.endswith("absent")]
    if DOSSIER.is_file():
        facts.append(check_dossier())
    else:
        absent.append("dossier absent")

    # Nothing private to verify: the public pins above still ran, so this is a clean skip.
    if len(absent) == len(facts):
        print("SKIP check_ghidra_pipeline: private Ghidra project / guest ELF / dossier absent "
              "(pipeline pins checked)")
        return 0
    print("OK check_ghidra_pipeline: " + ", ".join(facts))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
