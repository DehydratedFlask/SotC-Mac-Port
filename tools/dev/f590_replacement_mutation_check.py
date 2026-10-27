#!/usr/bin/env python3
"""REPLACEMENT mutation check for the native 0xf590 shared-string release.

This is NOT the original five-mutant set of task CPU-01-SHARED-STR-RELEASE-F590-01
(that private list was lost). It is a new, clearly labelled set of semantic mutants.

For each mutant the harness copies native/core/shared_str_release_f590.{hpp,cpp} and
the header it includes into a private scratch tree, applies exactly one textual
mutation to the COPY of the .cpp (the tracked tree is never written), builds the
unmodified native/tests/test_shared_str_release_f590.cpp against the copy with
arm64 clang and -fsanitize=address,undefined, runs it, and requires it to fail
(mutant killed). An unmutated control must pass. A mutant the test cannot detect is
reported as SURVIVED; nothing is weakened to hide it. As a second, independent layer
the pinned byte oracle tools/lift/check_ff590.py is run against each mutant binary.

Third layer ('disassembly-ordering'): memory ordering is not observable by a
single-threaded test, so each variant's object is also compiled with the SAME flags
(`-c`, arm64 clang -O1 ...), the release helper's mangled symbol is disassembled with
`xcrun llvm-objdump -d --disassemble-symbols=SYM`, and the atomic read-modify-write of
the reference count must lower to an acquire+release form: exactly one `ldaddal`
(LSE, what the pinned toolchain emits for seq_cst fetch_sub on arm64) or an
`ldaxr`..`stlxr` pair (LL/SC fallback). Relaxed lowers to `ldadd`/`ldxr`+`stxr`. The
control MUST satisfy this (else exit 5: the expectation or toolchain moved). A mutant
that the unit test does not kill is KILLED by this layer only if the verdict differs
from the control's; a missing symbol or missing atomic is reported as
ANALYSIS_ERROR, never as a pass. Mutants already killed by the unit test still have
their disassembly verdict recorded as evidence.

Usage: f590_replacement_mutation_check.py [--out DIR] [--no-oracle]
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_CPP = ROOT / "native/core/shared_str_release_f590.cpp"
SRC_HPP = ROOT / "native/core/shared_str_release_f590.hpp"
SRC_DEP = ROOT / "native/core/hashed_string_14c3a0.hpp"
TEST_CPP = ROOT / "native/tests/test_shared_str_release_f590.cpp"
ORACLE = ROOT / "tools/lift/check_ff590.py"
DEFAULT_OUT = ROOT / "local/validation/f590-replacement-mutants-20261025"

RELEASE_SYMBOL = "__ZN4sotc23release_shared_str_f590ERNS_9SharedStrERNS_24SharedStrDeallocatorF590E"
# Single-copy atomic RMW (LSE): ld<op>[a][l][b|h], swp, cas.
RMW_RE = re.compile(r"^(ld(?:add|clr|eor|set|smax|smin|umax|umin)|swp|cas)(a?)(l?)([bh]?)$")
# Exclusive pair (LL/SC fallback): ldxr/ldaxr and stxr/stlxr (+ b/h/p variants).
LDX_RE = re.compile(r"^ld(a?)xr([bh]?)$")
STX_RE = re.compile(r"^st(l?)xr([bh]?)$")
PINNED_MNEMONIC = "ldaddal"  # observed in the control build (Apple clang, arm64 LSE default)

FETCH_SUB = (
    "const auto previous = std::atomic_ref<std::int64_t>(*initial_ctl)\n"
    "                              .fetch_sub(1, std::memory_order_seq_cst);"
)
DATA_FREE = "    deallocator.deallocate(str.data, SharedStrDeallocationKindF590::data);\n"
CTL_FREE = (
    "    if (auto* const reloaded_ctl = str.ctl; reloaded_ctl != nullptr) {\n"
    "        deallocator.deallocate(reloaded_ctl, SharedStrDeallocationKindF590::control);\n"
    "    }\n"
)

# (id, description, old, new). Each `old` must occur exactly once in the .cpp copy.
MUTANTS = [
    ("M01_skip_decrement", "load instead of fetch_sub: counter never decremented",
     FETCH_SUB,
     "const auto previous = std::atomic_ref<std::int64_t>(*initial_ctl)\n"
     "                              .load(std::memory_order_seq_cst);"),
    ("M02_relaxed_ordering", "fetch_sub uses memory_order_relaxed instead of seq_cst",
     "std::memory_order_seq_cst);", "std::memory_order_relaxed);"),
    ("M03_prev_zero_releases", "release when previous <= 1 (previous 0 and negatives treated as last ref)",
     "if (previous != 1) return;", "if (previous > 1) return;"),
    ("M04_ctl_before_data", "control allocation released before data (order swapped)",
     DATA_FREE + "    // The original reloads this+0x18 after the first external call. Keeping\n"
     "    // initial_ctl here would miss callbacks that clear or replace the field.\n" + CTL_FREE,
     CTL_FREE + DATA_FREE),
    ("M05_no_reload_ctl", "control release uses the pre-callback ctl instead of reloading str.ctl",
     "if (auto* const reloaded_ctl = str.ctl; reloaded_ctl != nullptr) {",
     "if (auto* const reloaded_ctl = initial_ctl; reloaded_ctl != nullptr) {"),
    ("M06_no_dealloc_last_ref", "last reference never calls the deallocator",
     DATA_FREE + "    // The original reloads this+0x18 after the first external call. Keeping\n"
     "    // initial_ctl here would miss callbacks that clear or replace the field.\n" + CTL_FREE,
     "    (void)deallocator;\n"),
    ("M07_dealloc_when_prev_gt1", "also release when previous > 1 (only previous == 1 may release)",
     "if (previous != 1) return;", "if (previous < 1) return;"),
    ("M08_off_by_one_compare", "compare previous against 2 instead of 1",
     "if (previous != 1) return;", "if (previous != 2) return;"),
    ("M09_null_ctl_deref", "null-control early return removed (null ctl is dereferenced)",
     "    if (initial_ctl == nullptr) return;\n", ""),
    ("M10_skip_data_free", "data allocation never released",
     DATA_FREE, ""),
    ("M11_swap_kinds", "data allocation reported with the control kind",
     "deallocator.deallocate(str.data, SharedStrDeallocationKindF590::data);",
     "deallocator.deallocate(str.data, SharedStrDeallocationKindF590::control);"),
    ("M12_clear_ctl_after", "helper clears str.ctl after release (contract: no fields cleared)",
     "        deallocator.deallocate(reloaded_ctl, SharedStrDeallocationKindF590::control);\n    }\n",
     "        deallocator.deallocate(reloaded_ctl, SharedStrDeallocationKindF590::control);\n"
     "    }\n    str.ctl = nullptr;\n"),
]


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(cmd, env=None, cwd=None):
    return subprocess.run(cmd, capture_output=True, text=True, env=env, cwd=cwd)


def build(work: Path, name: str, cpp_text: str):
    d = work / name
    if d.exists():
        shutil.rmtree(d)
    (d / "core").mkdir(parents=True)
    shutil.copy2(SRC_HPP, d / "core" / SRC_HPP.name)
    shutil.copy2(SRC_DEP, d / "core" / SRC_DEP.name)
    (d / "core" / SRC_CPP.name).write_text(cpp_text)
    exe = d / "test_f590"
    cmd = ["clang++", "-arch", "arm64", "-std=c++20", "-g", "-O1", "-Wall", "-Wextra", "-Werror",
           "-ffp-contract=off", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
           "-I", str(d), str(TEST_CPP), str(d / "core" / SRC_CPP.name), "-o", str(exe)]
    p = run(cmd)
    (d / "build.log").write_text(" ".join(cmd) + "\n" + p.stdout + p.stderr)
    return exe, p.returncode, d


def compile_object(d: Path):
    """Compile only the (possibly mutated) release .cpp with the same flags as build()."""
    obj = d / "shared_str_release_f590.o"
    cmd = ["clang++", "-arch", "arm64", "-std=c++20", "-g", "-O1", "-Wall", "-Wextra", "-Werror",
           "-ffp-contract=off", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
           "-I", str(d), "-c", str(d / "core" / SRC_CPP.name), "-o", str(obj)]
    p = run(cmd)
    with open(d / "build.log", "a") as f:
        f.write("\n" + " ".join(cmd) + "\n" + p.stdout + p.stderr)
    return obj, p.returncode


def ordering_verdict(d: Path, obj: Path) -> dict:
    """Disassemble the release helper and classify the refcount atomic's ordering.

    verdict: ACQ_REL_OK | WEAK_ORDERING | NO_ATOMIC_RMW | MULTIPLE_ATOMICS | ANALYSIS_ERROR.
    Never returns OK unless a pinned acquire+release atomic was positively found.
    """
    r = run(["xcrun", "llvm-objdump", "-d", "--no-show-raw-insn",
             f"--disassemble-symbols={RELEASE_SYMBOL}", str(obj)])
    text = r.stdout
    (d / "release.disasm").write_text(text + r.stderr)
    if r.returncode != 0:
        return {"verdict": "ANALYSIS_ERROR", "detail": f"llvm-objdump exit {r.returncode}: {r.stderr.strip()[:200]}"}
    insns = []
    for line in text.splitlines():
        m = re.match(r"^\s*[0-9a-f]+:\s+([a-z0-9.]+)\b(.*)$", line)
        if m:
            insns.append((m.group(1), m.group(2).strip()))
    if not insns:
        return {"verdict": "ANALYSIS_ERROR",
                "detail": f"symbol {RELEASE_SYMBOL} not found / no instructions in disassembly"}
    rmw, ldx, stx = [], [], []
    for mn, ops in insns:
        m = RMW_RE.match(mn)
        if m:
            rmw.append((mn, bool(m.group(2)), bool(m.group(3))))
        m = LDX_RE.match(mn)
        if m:
            ldx.append((mn, bool(m.group(1))))
        m = STX_RE.match(mn)
        if m:
            stx.append((mn, bool(m.group(1))))
    evidence = [t[0] for t in rmw] + [mn for mn, _ in ldx] + [mn for mn, _ in stx]
    if rmw and (ldx or stx):
        return {"verdict": "MULTIPLE_ATOMICS", "atomics": evidence, "detail": "mixed LSE and exclusive atomics"}
    if rmw:
        if len(rmw) != 1:
            return {"verdict": "MULTIPLE_ATOMICS", "atomics": evidence, "detail": f"{len(rmw)} RMW atomics"}
        mn, acq, rel = rmw[0]
        ok = acq and rel
        return {"verdict": "ACQ_REL_OK" if ok else "WEAK_ORDERING", "atomics": evidence,
                "detail": f"{mn}: acquire={acq} release={rel}"}
    if ldx or stx:
        if len(ldx) != 1 or len(stx) != 1:
            return {"verdict": "MULTIPLE_ATOMICS", "atomics": evidence,
                    "detail": f"{len(ldx)} load-exclusive, {len(stx)} store-exclusive"}
        acq, rel = ldx[0][1], stx[0][1]
        return {"verdict": "ACQ_REL_OK" if (acq and rel) else "WEAK_ORDERING", "atomics": evidence,
                "detail": f"{ldx[0][0]}/{stx[0][0]}: acquire={acq} release={rel}"}
    return {"verdict": "NO_ATOMIC_RMW", "atomics": [],
            "detail": f"no atomic RMW/exclusive instruction among {len(insns)} instructions"}


def classify(p: subprocess.CompletedProcess) -> str:
    err = p.stderr
    m = re.search(r"FAIL (?:.*):(\d+) (.*)", err)
    if m:
        return f"CHECK line {m.group(1)}: {m.group(2).strip()}"
    m = re.search(r"(AddressSanitizer|runtime error|UndefinedBehaviorSanitizer)[^\n]*", err)
    if m:
        return "sanitizer: " + m.group(0).strip()[:160]
    return f"exit {p.returncode} (no recognised message)"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=DEFAULT_OUT)
    ap.add_argument("--no-oracle", action="store_true")
    args = ap.parse_args()
    out = args.out.resolve()
    if ROOT in out.parents and ROOT / "local" not in out.parents and out != ROOT / "local":
        print("refusing: --out must be under local/ or outside the repo", file=sys.stderr)
        return 2
    out.mkdir(parents=True, exist_ok=True)
    work = out / "work"
    base = SRC_CPP.read_text()
    tracked_before = {p: sha256(p) for p in (SRC_CPP, SRC_HPP, SRC_DEP, TEST_CPP)}
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0")
    use_oracle = not args.no_oracle and ORACLE.exists() and (ROOT / "local/re/eboot.elf").exists()

    def oracle(exe):
        if not use_oracle:
            return None
        p = run([sys.executable, "-I", str(ORACLE), "--elf", str(ROOT / "local/re/eboot.elf"),
                 "--native", str(exe)], env=env)
        return {"exit": p.returncode, "tail": (p.stdout + p.stderr).strip().splitlines()[-1:] }

    results = []
    # control
    exe, rc, d = build(work, "control", base)
    if rc != 0:
        print("control build FAILED; see", d / "build.log", file=sys.stderr)
        return 3
    p = run([str(exe)], env=env)
    (d / "run.log").write_text(p.stdout + p.stderr)
    obj, orc = compile_object(d)
    cdis = ordering_verdict(d, obj) if orc == 0 else {"verdict": "ANALYSIS_ERROR", "detail": "object compile failed"}
    control = {"id": "CONTROL", "description": "unmutated copy", "test_exit": p.returncode,
               "result": "PASS" if p.returncode == 0 else "FAIL", "oracle": oracle(exe),
               "ordering": cdis}
    results.append(control)
    ctrl_ok = (cdis["verdict"] == "ACQ_REL_OK" and cdis.get("atomics") == [PINNED_MNEMONIC])
    if not ctrl_ok:
        print("control disassembly-ordering check FAILED (expected a single %s): %s %s %s"
              % (PINNED_MNEMONIC, cdis["verdict"], cdis.get("atomics"), cdis.get("detail")), file=sys.stderr)

    for mid, desc, old, new in MUTANTS:
        n = base.count(old)
        if n != 1:
            results.append({"id": mid, "description": desc, "result": "BAD_MUTATION",
                            "detail": f"pattern occurs {n} times"})
            continue
        exe, rc, d = build(work, mid, base.replace(old, new))
        if rc != 0:
            results.append({"id": mid, "description": desc, "result": "BUILD_ERROR",
                            "detail": str(d / "build.log")})
            continue
        p = run([str(exe)], env=env)
        (d / "run.log").write_text(p.stdout + p.stderr)
        killed = p.returncode != 0
        obj, orc = compile_object(d)
        mdis = ordering_verdict(d, obj) if orc == 0 else {"verdict": "ANALYSIS_ERROR", "detail": "object compile failed"}
        # Layer verdict differs from the control's pinned form -> the layer detects it.
        same_as_control = (mdis["verdict"] == cdis["verdict"] and mdis.get("atomics") == cdis.get("atomics"))
        entry = {"id": mid, "description": desc, "test_exit": p.returncode, "ordering": mdis,
                 "oracle": oracle(exe)}
        if killed:
            entry.update(result="KILLED", killed_by="unit-test", assertion=classify(p))
        elif mdis["verdict"] == "ANALYSIS_ERROR":
            entry.update(result="ANALYSIS_ERROR", detail="unit test passed and ordering layer failed: " + mdis["detail"])
        elif not same_as_control:
            entry.update(result="KILLED", killed_by="disassembly-ordering",
                         assertion="ordering: %s %s (control: %s %s)" % (
                             mdis["verdict"], mdis.get("detail"), cdis["verdict"], cdis.get("detail")))
        else:
            entry.update(result="SURVIVED", assertion=None)
        results.append(entry)

    tracked_after = {p: sha256(p) for p in tracked_before}
    untouched = tracked_before == tracked_after
    report = {"label": "REPLACEMENT mutation set (not the original five mutants)",
              "test_source_sha256": sha256(TEST_CPP), "impl_source_sha256": sha256(SRC_CPP),
              "tracked_tree_untouched": untouched, "results": results}
    (out / "report.json").write_text(json.dumps(report, indent=2) + "\n")

    print("REPLACEMENT mutation set (NOT the original five mutants)")
    for r in results:
        o = r.get("oracle")
        ox = f" oracle_exit={o['exit']}" if o else ""
        od = r.get("ordering") or {}
        ax = ",".join(od.get("atomics") or []) or "-"
        by = r.get("killed_by") or "-"
        print(f"{r['id']:26s} {r['result']:14s} by={by:19s} atomic={ax:9s} {od.get('verdict', '')}{ox}  "
              f"{r.get('assertion') or r.get('detail') or ''}")
    print("tracked tree untouched:", untouched)
    ok = (control["result"] == "PASS" and untouched and
          all(r["result"] in ("KILLED", "SURVIVED") for r in results[1:]))
    for r in results[1:]:
        if r.get("killed_by") == "disassembly-ordering":
            print("NOTE: %s survived the unit test and is killed ONLY by the disassembly-ordering layer" % r["id"])
    survivors = [r["id"] for r in results[1:] if r["result"] == "SURVIVED"]
    print("survivors:", survivors or "none")
    if not ctrl_ok:
        return 5
    if not ok:
        return 1
    return 4 if survivors else 0


if __name__ == "__main__":
    sys.exit(main())
