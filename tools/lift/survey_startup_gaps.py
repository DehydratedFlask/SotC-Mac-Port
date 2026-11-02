#!/usr/bin/env python3
"""Survey of every unregistered startup-constructor slot (CPU-01 startup cliff).

Offline, read-only and stdlib-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256
d0723afb...5861d3) are read through `slot_pin_util.Elf`; instructions are decoded by the Xcode
`llvm-objdump` (`/usr/bin/objdump`) over each function's exact FDE range. Nothing is loaded,
relocated, executed or written into the pinned image.

What it does
  1. Reads the 573-entry constructor table (entry i = u64 at 0x2dd70e0 - 8*i) from the ELF and
     pairs it with the native registration (`native/core/startup_ctors.cpp`, the
     `recovered_constructors()` rows). The gap set is the union of the rows missing in `git HEAD`
     and in the working tree; a gap that the working tree already registers (or that has an
     untracked `ctor_slot_<N>_*.hpp`) is flagged `IN_FLIGHT` and is excluded from the batches.
  2. Parses `.eh_frame_hdr` and gives every gap its FDE size (and flags a ctor whose RVA is not an
     FDE start).
  3. Decodes each gap (and every registered slot, to learn which callees are already modelled) and
     records: instruction count, direct-call sites + callee histogram, `__cxa_atexit` calls,
     indirect call/jump sites (split into the functor-slot-3 `callq *0x18(%rax)` pattern and
     everything else), conditional branches, stack-guard presence, rip-relative operand/target/
     store counts and the local helpers (callees that have their own FDE).
  4. Classifies each gap against the modelled templates (slots 309/316/326/329): `debug-page
     template` / `simple leaf` / `unique large body`, ranks by an ESTIMATED effort score, and plans
     four disjoint worker batches (callees shared by gaps stay inside one batch).
  5. Writes `local/validation/startup_gap_survey/survey.json` + `survey.tsv` and, with
     `--write-doc`, `docs/STARTUP_GAP_SURVEY.md`.

Honesty labels: every count here is `STATIC_ANALYSIS_VERIFIED` (decoded from pinned bytes). The effort
score, the template classification and the batch plan are ESTIMATES (heuristics, stated in the doc);
nothing here is a gate result, and no gap is registered by this tool.

Fail-closed: a calibration run on slot 329 (a registered, byte-audited body) must reproduce its
pinned numbers (FDE 4082, 774 instructions, 97 direct calls / 11 callees, 22 `__cxa_atexit`, 3
indirect sites); the table must hold 573 distinct-index rows, `git HEAD` and the working tree must
parse, and every gap RVA must fall inside an FDE. Any violation exits 1. Missing private image:
prints SKIP and exits 0. Missing disassembler: exits 2.

Usage: survey_startup_gaps.py [--write-doc] [--json PATH] [--no-calibrate]
"""

from __future__ import annotations

import argparse
import concurrent.futures
import json
import re
import shutil
import struct
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import ROOT, TABLE_START, Elf  # noqa: E402

ELF_PATH = ROOT / "local/re/eboot.elf"
STARTUP_CPP = "native/core/startup_ctors.cpp"
OUT_DIR = ROOT / "local/validation/startup_gap_survey"
DOC_PATH = ROOT / "docs/STARTUP_GAP_SURVEY.md"
TABLE_COUNT = 573
EH_FRAME_HDR = 0x150C08C
ATEXIT = 0xF8F390
STACK_CHK_FAIL = 0xF8F3A0
CANARY_CELL = 0x15FE518
RELEASE = 0xF590
TEMPLATE_SLOTS = (309, 316, 326, 329)

# Console-variable / debug-menu helper callees that every modelled debug-page slot shares
# (names from tools/lift/l4_slot_lib.py COMMON_CALLEES and the slot headers).
HELPER_NAMES = {
    0xF590: "release_f590",
    0x20AF0: "concmd_base_ctor",
    0x20D60: "concmd_register",
    0x26EA0: "menu_toggle_ctor",
    0x26DB0: "menu_toggle_from_var_ctor",
    0x26C10: "menu_group_ctor",
    0x26B60: "menu_separator_ctor",
    0xC300: "bounded_float_ctor",
    0x6AA00: "unbounded_float_ctor",
    0x4B0: "cmd_dtor",
    0x20B0: "bool_dtor",
    0x20F0: "float_dtor",
    0x272D0: "menu_item_dtor",
    ATEXIT: "__cxa_atexit",
    STACK_CHK_FAIL: "__stack_chk_fail",
}
MENU_HELPERS = {0x20AF0, 0x20D60, 0x26EA0, 0x26DB0, 0x26C10, 0x26B60, 0xC300, 0x6AA00}

_LINE = re.compile(r"^\s*([0-9a-f]+):\s+(.*\S)\s*$")
_PREFIXES = {"rep", "repe", "repne", "lock", "notrack", "data16", "cs", "ds", "es", "ss", "fs", "gs", "addr32",
             "bnd", "xacquire", "xrelease"}
_HEX = re.compile(r"0x[0-9a-f]+")
_RIP_COMMENT = re.compile(r"#\s*(0x[0-9a-f]+)")


def die(msg: str, code: int = 1) -> None:
    print(f"FAIL {msg}", file=sys.stderr)
    raise SystemExit(code)


# ------------------------------------------------------------------ ELF / FDE / table

def parse_fdes(e: Elf) -> list[tuple[int, int]]:
    """(start, size) for every FDE in .eh_frame_hdr's binary-search table, sorted."""
    hdr = e.raw(EH_FRAME_HDR, 12)
    ver, fenc, cenc, tenc = hdr[0], hdr[1], hdr[2], hdr[3]
    if (ver, fenc, cenc, tenc) != (1, 0x1B, 0x03, 0x3B):
        die(f"unexpected .eh_frame_hdr encodings {(ver, fenc, cenc, tenc)}")
    count = struct.unpack_from("<i", hdr, 8)[0]
    tbl = e.raw(EH_FRAME_HDR + 12, 8 * count)
    out = []
    for i in range(count):
        _loc, fde = struct.unpack_from("<ii", tbl, 8 * i)
        fa = EH_FRAME_HDR + fde
        rec = e.raw(fa, 16)
        pcb = struct.unpack_from("<i", rec, 8)[0] + fa + 8
        rng = struct.unpack_from("<I", rec, 12)[0]
        out.append((pcb, rng))
    out.sort()
    return out


def fde_lookup(fdes: list[tuple[int, int]]):
    import bisect
    starts = [s for s, _ in fdes]

    def find(addr: int):
        k = bisect.bisect_right(starts, addr) - 1
        if k >= 0 and fdes[k][0] <= addr < fdes[k][0] + fdes[k][1]:
            return fdes[k]
        return None
    return find


def read_table(e: Elf) -> list[int]:
    return [e.u64(TABLE_START - 8 * i) for i in range(TABLE_COUNT)]


# ------------------------------------------------------------------ native registration

def registered_rows(text: str) -> dict[int, int]:
    """{index: rva} rows of recovered_constructors() (the ahead view is a different function)."""
    i = text.index("recovered_constructors() noexcept")
    j = text.index("registered_ahead_constructors() noexcept {")
    rows = re.findall(r"\{\s*(0x[0-9a-f]+),\s*(\d+),\s*\[", text[i:j])
    out: dict[int, int] = {}
    for rva, idx in rows:
        if int(idx) in out:
            die(f"duplicate registration row for index {idx}")
        out[int(idx)] = int(rva, 16)
    return out


def git_show(path: str) -> str | None:
    try:
        r = subprocess.run(["git", "-C", str(ROOT), "show", f"HEAD:{path}"], capture_output=True, text=True,
                           timeout=60)
    except (OSError, subprocess.SubprocessError):
        return None
    return r.stdout if r.returncode == 0 else None


def untracked_slot_files() -> dict[int, list[str]]:
    out: dict[int, list[str]] = defaultdict(list)
    for p in sorted((ROOT / "native/core").glob("ctor_slot_*_*.hpp")):
        m = re.match(r"ctor_slot_(\d+)_", p.name)
        if m:
            out[int(m.group(1))].append(p.name)
    return out


# ------------------------------------------------------------------ disassembly

def find_objdump() -> str:
    for cand in ("/usr/bin/objdump", shutil.which("llvm-objdump"), shutil.which("objdump")):
        if cand and Path(cand).exists():
            r = subprocess.run([cand, "--version"], capture_output=True, text=True)
            if "LLVM" in (r.stdout + r.stderr):
                return cand
    die("no llvm-objdump found (need Xcode's /usr/bin/objdump)", 2)
    raise AssertionError


def disassemble(objdump: str, start: int, size: int) -> list[tuple[int, str, str]]:
    r = subprocess.run([objdump, "-d", "--no-show-raw-insn", f"--start-address={start:#x}",
                        f"--stop-address={start + size:#x}", str(ELF_PATH)],
                       capture_output=True, text=True, timeout=120)
    if r.returncode != 0:
        die(f"objdump failed for {start:#x}: {r.stderr[-200:]}")
    ins = []
    for line in r.stdout.splitlines():
        m = _LINE.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        body = m.group(2)
        if body.endswith(">:") or not (start <= addr < start + size):
            continue
        parts = body.split(None, 1)
        mnem = parts[0]
        rest = parts[1] if len(parts) > 1 else ""
        while mnem in _PREFIXES and rest:
            p2 = rest.split(None, 1)
            mnem = p2[0]
            rest = p2[1] if len(p2) > 1 else ""
        ins.append((addr, mnem, rest))
    return ins


def analyse(ins: list[tuple[int, str, str]], start: int, size: int) -> dict:
    end = start + size
    calls: Counter = Counter()
    jmps_out: Counter = Counter()
    indirect_slot3 = 0
    indirect_reg = 0
    indirect_other = 0
    indirect_jmp = 0
    jcc = 0
    rets = 0
    rip_ops = 0
    rip_targets: set[int] = set()
    rip_stores = 0
    store_targets: set[int] = set()
    canary_reads = 0
    vex = 0
    ud2 = 0
    for addr, mnem, ops in ins:
        if mnem.startswith("call"):
            if ops.lstrip().startswith("*"):
                if re.match(r"\*0x18\(%r[a-z0-9]+\)", ops.strip()):
                    indirect_slot3 += 1
                elif re.match(r"\*%r[a-z0-9]+$", ops.strip()):
                    indirect_reg += 1          # functor self-pointer fix-up style `callq *%r13`
                else:
                    indirect_other += 1
            else:
                m = _HEX.match(ops.strip())
                if m:
                    calls[int(m.group(0), 16)] += 1
        elif mnem.startswith("jmp"):
            if ops.lstrip().startswith("*"):
                indirect_jmp += 1
            else:
                m = _HEX.match(ops.strip())
                if m:
                    t = int(m.group(0), 16)
                    if not (start <= t < end):
                        jmps_out[t] += 1
        elif mnem.startswith("j"):
            jcc += 1
        elif mnem.startswith("ret"):
            rets += 1
        elif mnem == "ud2":
            ud2 += 1
        if mnem.startswith("v"):
            vex += 1
        m = _RIP_COMMENT.search(ops)
        if "(%rip)" in ops and m:
            t = int(m.group(1), 16)
            rip_ops += 1
            rip_targets.add(t)
            # AT&T: destination is the last operand.
            dest = ops.split("#")[0].rsplit(",", 1)[-1]
            if "(%rip)" in dest and not mnem.startswith(("lea", "cmp", "test", "ucomis", "comis")):
                rip_stores += 1
                store_targets.add(t)
            if t == CANARY_CELL:
                canary_reads += 1
    return {
        "instructions": len(ins),
        "direct_calls": sum(calls.values()),
        "callees": {f"{k:#x}": v for k, v in sorted(calls.items())},
        "atexit_calls": calls.get(ATEXIT, 0),
        "stack_chk_fail_calls": calls.get(STACK_CHK_FAIL, 0),
        "indirect_calls_functor_slot3": indirect_slot3,
        "indirect_calls_reg": indirect_reg,
        "indirect_calls_other": indirect_other,
        "indirect_jumps": indirect_jmp,
        "tail_jumps_out": {f"{k:#x}": v for k, v in sorted(jmps_out.items())},
        "cond_branches": jcc,
        "rets": rets,
        "rip_operands": rip_ops,
        "rip_targets": len(rip_targets),
        "rip_stores": rip_stores,
        "rip_store_targets": len(store_targets),
        "canary_reads": canary_reads,
        "stack_guard": bool(canary_reads and calls.get(STACK_CHK_FAIL, 0)),
        "vex_instructions": vex,
        "ud2": ud2,
    }


# ------------------------------------------------------------------ classification / effort

def native_hex_tokens() -> dict[int, set[str]]:
    """{hex constant: native files naming it} over native/**/*.{hpp,cpp} (read-only)."""
    toks: dict[int, set[str]] = defaultdict(set)
    for p in sorted(list((ROOT / "native").rglob("*.hpp")) + list((ROOT / "native").rglob("*.cpp"))):
        try:
            txt = p.read_text(errors="ignore")
        except OSError:
            continue
        rel = str(p.relative_to(ROOT / "native"))
        for m in re.finditer(r"0x([0-9a-fA-F]{3,8})\b", txt):
            toks[int(m.group(1), 16)].add(rel)
    return toks


def git_tracked_slot_files() -> set[str]:
    try:
        r = subprocess.run(["git", "-C", str(ROOT), "ls-files", "native/core"], capture_output=True, text=True,
                           timeout=60)
    except (OSError, subprocess.SubprocessError):
        return set()
    return {Path(x).name for x in r.stdout.split() if Path(x).name.startswith("ctor_slot_")}


def local_callees(rec: dict, find_fde) -> dict[int, int]:
    out = {}
    for k, v in rec["callees"].items():
        t = int(k, 16)
        if t not in (ATEXIT, STACK_CHK_FAIL) and find_fde(t) is not None:
            out[t] = v
    return out


def classify(rec: dict, template_callees: dict[int, set[int]], find_fde, unmodelled: list[int]) -> dict:
    loc = set(local_callees(rec, find_fde))
    menu = sorted(MENU_HELPERS & set(int(k, 16) for k in rec["callees"]))
    best, best_shared = None, set()
    for tn, tc in template_callees.items():
        sh = (loc & tc) - {RELEASE}
        if len(sh) > len(best_shared) or (len(sh) == len(best_shared) and best is None):
            best, best_shared = tn, sh
    n = rec["instructions"]
    guard = rec["stack_guard"]
    density = rec["direct_calls"] / max(1, n)
    # The four modelled templates have 0.11-0.125 direct calls per instruction.
    if menu and guard and density >= 0.04:
        kind = "debug-page template"
    elif n <= 48 and rec["direct_calls"] <= 4 and rec["indirect_calls_other"] == 0 \
            and rec["indirect_jumps"] == 0 and rec["cond_branches"] == 0:
        kind = "simple leaf"
    elif n <= 120 and rec["indirect_calls_other"] == 0 and rec["indirect_calls_reg"] == 0 \
            and rec["indirect_jumps"] == 0 and rec["cond_branches"] <= 2 and not guard:
        kind = "simple leaf"
    else:
        kind = "unique large body"
    extras = []
    if kind == "debug-page template":
        if rec["indirect_calls_reg"]:
            extras.append(f"{rec['indirect_calls_reg']} functor fix-up call(s)")
        if rec["indirect_calls_other"]:
            extras.append(f"{rec['indirect_calls_other']} other indirect call(s)")
        if rec["indirect_jumps"]:
            extras.append(f"{rec['indirect_jumps']} jump-table jump(s)")
        if rec["cond_branches"] - 1 > 0:
            extras.append(f"{rec['cond_branches'] - 1} branch(es) beyond the canary")
        if unmodelled:
            extras.append(f"{len(unmodelled)} unmodelled helper(s)")
    return {
        "class": kind,
        "template_shape": ("pure" if not extras else "+ " + ", ".join(extras)) if kind == "debug-page template" else "",
        "call_density": round(density, 3),
        "menu_helpers": [f"{m:#x}" for m in menu],
        "best_template": best if best_shared else None,
        "template_shared_callees": sorted(f"{x:#x}" for x in best_shared),
    }


def effort(rec: dict, cls: dict, unmodelled: list[int]) -> float:
    pts = rec["instructions"] / 60.0
    pts += rec["rip_targets"] / 40.0
    pts += 0.6 * len(unmodelled)
    pts += 0.1 * rec["indirect_calls_functor_slot3"] + 0.5 * rec["indirect_calls_reg"]
    pts += 3.0 * rec["indirect_calls_other"] + 4.0 * rec["indirect_jumps"]
    pts += 0.5 * max(0, rec["cond_branches"] - (1 if rec["stack_guard"] else 0))
    pts += 0.8 * len(rec["tail_jumps_out"])
    factor = {"debug-page template": 0.6, "simple leaf": 0.6, "unique large body": 1.0}[cls["class"]]
    return round(max(0.3, pts * factor), 2)


def tier(points: float) -> str:
    return "S" if points < 1.0 else "M" if points < 4.0 else "L" if points < 10.0 else "XL"


# ------------------------------------------------------------------ batches

def plan_batches(gaps: list[dict], nbatch: int = 4) -> list[list[dict]]:
    """Group gaps by shared *unmodelled* local helper (they would otherwise both need to introduce
    the same helper file), then LPT-bin-pack the groups by effort points."""
    parent = {g["index"]: g["index"] for g in gaps}

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x
    owner: dict[str, int] = {}
    for g in gaps:
        for h in g["unmodelled_local_callees"]:
            if h in owner:
                a, b = find(owner[h]), find(g["index"])
                if a != b:
                    parent[b] = a
            else:
                owner[h] = g["index"]
    comps: dict[int, list[dict]] = defaultdict(list)
    for g in gaps:
        comps[find(g["index"])].append(g)
    groups = sorted(comps.values(), key=lambda c: -sum(g["effort_points"] for g in c))
    bins: list[list[dict]] = [[] for _ in range(nbatch)]
    loads = [0.0] * nbatch
    for grp in groups:
        k = min(range(nbatch), key=lambda b: loads[b])
        bins[k].extend(grp)
        loads[k] += sum(g["effort_points"] for g in grp)
    for b in bins:
        b.sort(key=lambda g: g["index"])
    return bins


# ------------------------------------------------------------------ pins that serialise the work

PIN_SITES = [
    "native/tests/test_startup_ctor_coverage.cpp",
    "tools/lift/check_console_var_ctors.py",
    "tools/lift/check_ctor_slots.py",
    "tools/lift/check_fctor_slots_296_297_300.py",
]
PIN_RE = re.compile(r"(slots\.size\(\)\s*==\s*\d+|first_unrecovered_constructor_index\(\)\s*==\s*\d+|"
                    r"first_unrecovered_constructor_rva\(\)\s*==\s*0x[0-9a-f]+|slots\.size\(\)\s*\+\s*\d+\s*==|"
                    r"len\(unregistered\)\s*==\s*\d+|len\(rows\)\s*==\s*\d+)")


def find_pin_sites() -> list[dict]:
    out = []
    for rel in PIN_SITES:
        p = ROOT / rel
        if not p.exists():
            continue
        for n, line in enumerate(p.read_text(errors="ignore").splitlines(), 1):
            if PIN_RE.search(line):
                out.append({"file": rel, "line": n, "text": line.strip()[:160]})
    return out


# ------------------------------------------------------------------ main

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--write-doc", action="store_true", help=f"also write {DOC_PATH.relative_to(ROOT)}")
    ap.add_argument("--json", type=Path, default=OUT_DIR / "survey.json")
    ap.add_argument("--no-calibrate", action="store_true")
    args = ap.parse_args()

    if not ELF_PATH.exists():
        print(f"SKIP survey_startup_gaps: private image {ELF_PATH} absent")
        return 0
    objdump = find_objdump()
    e = Elf()                                     # verifies the pinned SHA-256 (fails closed)
    fdes = parse_fdes(e)
    find_fde = fde_lookup(fdes)
    table = read_table(e)
    if len(table) != TABLE_COUNT:
        die("table length")

    wt_text = (ROOT / STARTUP_CPP).read_text()
    wt_rows = registered_rows(wt_text)
    head_text = git_show(STARTUP_CPP)
    head_rows = registered_rows(head_text) if head_text else dict(wt_rows)
    for idx, rva in list(wt_rows.items()) + list(head_rows.items()):
        if not 0 <= idx < TABLE_COUNT or table[idx] != rva:
            die(f"native row {idx}/{rva:#x} disagrees with ELF table word {table[idx] if idx < TABLE_COUNT else None}")
    slot_files = untracked_slot_files()
    gap_idx = sorted((set(range(TABLE_COUNT)) - set(head_rows)) | (set(range(TABLE_COUNT)) - set(wt_rows)))
    tracked = git_tracked_slot_files()
    # IN_FLIGHT: registered by the working tree but not by HEAD, or its ctor_slot_<N>_* header is untracked.
    in_flight = {i for i in gap_idx if i in wt_rows or any(n not in tracked for n in slot_files.get(i, []))}
    # MODELLED_UNWIRED: a tracked native model exists but no recovered_constructors() row (needs wiring only).
    unwired = {i for i in gap_idx if i not in in_flight and any(n in tracked for n in slot_files.get(i, []))}

    # Decode: every registered slot (callee knowledge), every gap, plus calibration.
    wanted: dict[int, int] = {}                    # rva -> fde size
    for i in sorted(set(head_rows) | set(wt_rows)) + gap_idx:
        rva = table[i]
        f = find_fde(rva)
        if f is None or f[0] != rva:
            if i in gap_idx:
                die(f"gap {i}: RVA {rva:#x} is not an FDE start (containing: {f})")
            continue
        wanted[rva] = f[1]
    wanted.setdefault(0x829E90, find_fde(0x829E90)[1])

    def work(item):
        rva, sz = item
        return rva, analyse(disassemble(objdump, rva, sz), rva, sz)
    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as ex:
        decoded = dict(ex.map(work, sorted(wanted.items())))

    if not args.no_calibrate:
        c = decoded[0x829E90]
        want = dict(instructions=774, direct_calls=97, atexit_calls=22, indirect_calls_functor_slot3=3,
                    indirect_calls_other=0, rip_operands=124, rip_targets=69, rip_stores=11, rip_store_targets=9)
        for k, v in want.items():
            if c[k] != v:
                die(f"calibration slot 329: {k} = {c[k]} (pinned {v})")
        if len(c["callees"]) != 11 or wanted[0x829E90] != 4082 or not c["stack_guard"]:
            die("calibration slot 329: callee/FDE/guard shape")
        # (rva, FDE bytes, insns, direct calls, distinct callees, indirect call sites in total), all taken from
        # the slot headers/checkers of the modelled templates.
        for rva, fsz, ni, nc, nd, nind in ((0xA505E0, 1892, 354, 40, 10, 3), (0xA33280, 2561, 474, 54, 9, 5),
                                           (0x837F30, 7055, 1306, 159, 10, 13)):
            c = decoded.get(rva)
            if c is None:
                die(f"calibration: template body {rva:#x} was not decoded")
            got = (wanted[rva], c["instructions"], c["direct_calls"], len(c["callees"]),
                   c["indirect_calls_functor_slot3"] + c["indirect_calls_reg"] + c["indirect_calls_other"])
            if got != (fsz, ni, nc, nd, nind):
                die(f"calibration {rva:#x}: {got} != {(fsz, ni, nc, nd, nind)}")

    # Modelled knowledge.
    template_callees = {}
    for tn in TEMPLATE_SLOTS:
        if tn not in head_rows:
            die(f"template slot {tn} is not registered in HEAD")
        template_callees[tn] = set(local_callees(decoded[table[tn]], find_fde))
    modelled_callers: dict[int, list[int]] = defaultdict(list)
    for i in sorted(head_rows):
        rec = decoded.get(table[i])
        if rec:
            for k in rec["callees"]:
                modelled_callers[int(k, 16)].append(i)
    native_tokens = native_hex_tokens()

    gaps = []
    for i in gap_idx:
        rva = table[i]
        rec = dict(decoded[rva])
        lc = local_callees(rec, find_fde)
        unmodelled = sorted(t for t in lc if t not in modelled_callers and t not in native_tokens)
        known = sorted(t for t in lc if t in modelled_callers or t in native_tokens)
        cls = classify(rec, template_callees, find_fde, unmodelled)
        pts = effort(rec, cls, unmodelled)
        g = {
            "index": i, "rva": f"{rva:#x}", "fde_size": wanted[rva], "status": "IN_FLIGHT" if i in in_flight else "MODELLED_UNWIRED" if i in unwired else "OPEN",
            "native_files": sorted(f for f in native_tokens.get(rva, set()) if not f.startswith(
                ("core/startup_ctors", "core/ctor_original_table"))),
            **rec, **cls,
            "local_callees": {f"{t:#x}": {"calls": n, "fde_size": find_fde(t)[1],
                                         "modelled_by_slots": modelled_callers.get(t, [])[:6],
                                         "in_native_tree": t in native_tokens,
                                         "name": HELPER_NAMES.get(t, "")} for t, n in sorted(lc.items())},
            "unmodelled_local_callees": [f"{t:#x}" for t in unmodelled],
            "modelled_local_callees": [f"{t:#x}" for t in known],
            "effort_points": pts, "effort_tier": tier(pts),
        }
        gaps.append(g)

    open_gaps = [g for g in gaps if g["status"] == "OPEN"]
    # New (unmodelled) helper work items shared across gaps.
    helper_users: dict[str, list[int]] = defaultdict(list)
    for g in gaps:
        for h in g["unmodelled_local_callees"]:
            helper_users[h].append(g["index"])
    helpers = [{"rva": h, "fde_size": find_fde(int(h, 16))[1], "used_by": sorted(u)}
               for h, u in sorted(helper_users.items(), key=lambda kv: (-len(kv[1]), kv[0]))]
    batches = plan_batches(open_gaps)
    ranked = sorted(open_gaps, key=lambda g: (g["effort_points"], g["index"]))

    result = {
        "schema": "startup_gap_survey/1",
        "elf_sha256_pinned": True,
        "coverage_head": f"{len(head_rows)}/{TABLE_COUNT}",
        "coverage_working_tree": f"{len(wt_rows)}/{TABLE_COUNT}",
        "gap_count": len(gaps), "open_count": len(open_gaps),
        "in_flight": sorted(in_flight),
        "modelled_unwired": sorted(unwired),
        "gaps": gaps,
        "unmodelled_helpers": helpers,
        "ranked_open": [g["index"] for g in ranked],
        "batches": [[g["index"] for g in b] for b in batches],
        "batch_points": [round(sum(g["effort_points"] for g in b), 1) for b in batches],
        "pin_sites": find_pin_sites(),
        "label": "STATIC_ANALYSIS_VERIFIED counts; effort/class/batches are ESTIMATES",
    }
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(result, indent=1, sort_keys=False) + "\n")
    cols = ["index", "rva", "fde_size", "instructions", "direct_calls", "atexit_calls", "indirect_calls_functor_slot3",
            "indirect_calls_reg", "indirect_calls_other", "indirect_jumps", "cond_branches", "stack_guard", "rip_targets", "class",
            "template_shape", "best_template", "effort_points", "effort_tier", "status"]
    (args.json.parent / "survey.tsv").write_text(
        "\t".join(cols) + "\n" + "\n".join("\t".join(str(g[c]) for c in cols) for g in gaps) + "\n")
    if args.write_doc:
        DOC_PATH.write_text(render_doc(result, gaps, ranked, batches, head_rows, wt_rows))

    print(f"OK survey_startup_gaps: HEAD {len(head_rows)}/{TABLE_COUNT}, working tree {len(wt_rows)}/{TABLE_COUNT}, "
          f"{len(gaps)} gaps ({len(open_gaps)} open, in-flight {sorted(in_flight)}, modelled-unwired {sorted(unwired)}); "
          f"classes {dict(Counter(g['class'] for g in open_gaps))}; batch points {result['batch_points']}")
    print(f"  wrote {args.json.relative_to(ROOT)}" + (f" and {DOC_PATH.relative_to(ROOT)}" if args.write_doc else ""))
    return 0


# ------------------------------------------------------------------ doc rendering

def ranges(nums: list[int]) -> str:
    out, run = [], []
    for n in sorted(nums):
        if run and n == run[-1] + 1:
            run.append(n)
        else:
            if run:
                out.append(run)
            run = [n]
    if run:
        out.append(run)
    return ", ".join(str(r[0]) if len(r) == 1 else f"{r[0]}-{r[-1]}" for r in out)


def render_doc(result, gaps, ranked, batches, head_rows, wt_rows) -> str:
    L = []
    w = L.append
    w("# Startup constructor gap survey\n")
    w("Generated by `tools/lift/survey_startup_gaps.py --write-doc` from the pinned ELF `d0723afb...5861d3`")
    w("(image base 0, RVA == address), `llvm-objdump` over each FDE range, `native/core/startup_ctors.cpp`")
    w("and `git HEAD`. Re-run to refresh; the script skips (exit 0) without the private image and fails")
    w("closed on any calibration, table or FDE inconsistency. Raw data: `local/validation/startup_gap_survey/`")
    w("(`survey.json`, `survey.tsv`; private, not published).\n")
    w("Labels: every count below is `STATIC_ANALYSIS_VERIFIED`. The **effort score, template class and batch plan are")
    w("ESTIMATES** (heuristics defined in the script header and below). Nothing here registers a slot or is a gate result.\n")
    w(f"Coverage: HEAD {result['coverage_head']}, working tree {result['coverage_working_tree']}. "
      f"Gaps considered: {result['gap_count']} ({result['open_count']} open, in flight: "
      f"{', '.join(map(str, result['in_flight'])) or 'none'}; in-flight slots belong to the integrator and are not batched).\n")
    cls = Counter(g["class"] for g in gaps if g["status"] == "OPEN")
    tiers = Counter(g["effort_tier"] for g in gaps if g["status"] == "OPEN")
    w("## Summary\n")
    w("| class (open gaps) | count | instructions (sum) |")
    w("|---|---:|---:|")
    for k in ("debug-page template", "simple leaf", "unique large body"):
        w(f"| {k} | {cls.get(k, 0)} | {sum(g['instructions'] for g in gaps if g['status'] == 'OPEN' and g['class'] == k)} |")
    w(f"\nEffort tiers: " + ", ".join(f"{t}={tiers.get(t, 0)}" for t in ("S", "M", "L", "XL")) +
      " (S < 1.0 point, M < 4, L < 10, XL >= 10; one point is roughly one small checked slot).\n")
    w("Effort score = `insns/60 + rip_targets/40 + 0.6*unmodelled_local_helpers + 0.1*functor_slot3_sites "
      "+ 0.5*register_indirect_calls + 3*other_indirect_calls + 4*indirect_jumps + 0.5*(cond_branches - canary) + 0.8*tail_jumps`, multiplied by "
      "0.6 for `debug-page template` and `simple leaf`, 1.0 for `unique large body`. A helper counts as unmodelled "
      "when no registered slot calls it and its RVA is not named anywhere under `native/`.\n")
    w("Template classes: **debug-page template** = stack-guarded body that calls one of the console-var/menu helpers "
      "(`0x20af0 0x20d60 0x26ea0 0x26db0 0x26c10 0x26b60 0xc300 0x6aa00`) like slots 309/316/326/329; **simple leaf** = "
      "short, guard-free, branch-free (or at most 2 branches, no indirect control flow) body; **unique large body** = "
      "everything else. `best template` is the modelled slot (309/316/326/329) sharing the most non-release local helpers.\n")
    # ranked table
    w("## Ranked table (easiest first, open gaps only)\n")
    w("Columns: idx, RVA, FDE bytes, insns, direct calls (callees), `__cxa_atexit`, indirect (functor slot-3 `*0x18(%reg)` / register `*%reg` / other / jmp), "
      "cond branches, guard, rip targets, class, best template, unmodelled helpers, points, tier.\n")
    w("| # | idx | RVA | FDE B | insn | calls (callees) | atexit | indirect s3/reg/oth/jmp | jcc | guard | rip tgts | class | tmpl | unmodelled helpers | pts | tier |")
    w("|--:|--:|---|--:|--:|---|--:|---|--:|:-:|--:|---|---|---|--:|:-:|")
    for n, g in enumerate(ranked, 1):
        w(f"| {n} | {g['index']} | `{g['rva']}` | {g['fde_size']} | {g['instructions']} | {g['direct_calls']} ({len(g['callees'])}) "
          f"| {g['atexit_calls']} | {g['indirect_calls_functor_slot3']}/{g['indirect_calls_reg']}/{g['indirect_calls_other']}/{g['indirect_jumps']} "
          f"| {g['cond_branches']} | {'Y' if g['stack_guard'] else 'n'} | {g['rip_targets']} | {g['class']} "
          f"| {g['best_template'] or '-'} | {len(g['unmodelled_local_callees'])} | {g['effort_points']} | {g['effort_tier']} |")
    inflight = [g for g in gaps if g["status"] == "IN_FLIGHT"]
    unwired = [g for g in gaps if g["status"] == "MODELLED_UNWIRED"]
    if inflight:
        w("\nIn flight (not ranked, owned by the integrator): " + ", ".join(
            f"{g['index']} (`{g['rva']}`, {g['fde_size']} B, {g['instructions']} insn)" for g in inflight) + ".\n")
    if unwired:
        w("Modelled but not wired (tracked `ctor_slot_<N>_*` model + test + checker exist, no `recovered_constructors()` "
          "row; wiring/verification only, not ranked, assigned to the integrator or the first free worker): " + ", ".join(
              f"{g['index']} (`{g['rva']}`, {g['fde_size']} B, files {', '.join(g['native_files'][:2]) or 'none'})"
              for g in unwired) + ".\n")
    shapes = [g for g in ranked if g["class"] == "debug-page template" and g["template_shape"] != "pure"]
    w("\n## Template shape of the debug-page gaps\n")
    w("`pure` = same shape as 309/326/329 (straight line, only the canary branch, only `callq *0x18(%rax)` functor sites, "
      "all local helpers already modelled). Others carry extras that need a new contract:\n")
    w(f"- pure: {sum(1 for g in ranked if g['template_shape'] == 'pure')} slots: " + ranges(
        [g["index"] for g in ranked if g["template_shape"] == "pure"]))
    for g in shapes:
        w(f"- {g['index']} (`{g['rva']}`, {g['instructions']} insn): {g['template_shape'][2:]}")
    w("")
    w("## New helpers (callees no registered slot calls and `native/` does not name)\n")
    w("| helper | FDE B | used by gap idx |")
    w("|---|--:|---|")
    for h in result["unmodelled_helpers"]:
        w(f"| `{h['rva']}` | {h['fde_size']} | {', '.join(map(str, h['used_by']))} |")
    w("")
    # histograms
    w("## Callee histograms (per gap, `callee x count`)\n")
    w("Known names: " + ", ".join(f"`{k:#x}`={v}" for k, v in sorted(HELPER_NAMES.items())) + ". "
      "`*` marks a local helper that no registered slot calls and no `native/` file names.\n")
    w("| idx | callee histogram | local helper bodies (FDE B) |")
    w("|--:|---|---|")
    for g in sorted(gaps, key=lambda x: x["index"]):
        hist = ", ".join(f"`{k}`{('=' + HELPER_NAMES[int(k, 16)]) if int(k, 16) in HELPER_NAMES else ''} x{v}"
                         for k, v in sorted(g["callees"].items(), key=lambda kv: -kv[1]))
        loc = ", ".join(f"`{k}`{'*' if k in g['unmodelled_local_callees'] else ''} ({d['fde_size']})"
                        for k, d in g["local_callees"].items() if int(k, 16) not in HELPER_NAMES)
        w(f"| {g['index']} | {hist or '-'} | {loc or '-'} |")
    # batches
    w("\n## Proposed four disjoint batches\n")
    w("Batching rule (estimate): two gaps that call the same *unmodelled* local helper are kept in one batch (otherwise "
      "both workers would introduce the same helper file), then the groups are bin-packed by effort points. "
      "Slot indices below are disjoint; per-slot files are `native/core/ctor_slot_<N>_<rva>.{hpp,cpp}`, "
      "`native/tests/test_ctor_slot_<N>.cpp`, `tools/lift/check_ctor_slot_<N>.py` (plus a local-only "
      "`local/validation/<TASK>/` log), which never collide across slots.\n")
    for k, b in enumerate(batches, 1):
        tot = round(sum(g["effort_points"] for g in b), 1)
        w(f"### Batch {k} - {len(b)} slots, {tot} points, {sum(g['instructions'] for g in b)} instructions\n")
        w("Slots: " + ranges([g["index"] for g in b]) + "\n")
        bc = Counter(g["class"] for g in b)
        w("Mix: " + ", ".join(f"{v} {c}" for c, v in bc.items()) + ". Heaviest: " + ", ".join(
            f"{g['index']} ({g['effort_points']})" for g in sorted(b, key=lambda x: -x["effort_points"])[:5]) + ".\n")
    uniq = [g for g in ranked if g["class"] != "debug-page template"]
    w("## Non-template bodies (simple leaves and unique large bodies)\n")
    w("Top callees of each (`callee x count`; names from the known-helper table). These are the slots that do not "
      "reuse the 309/316/326/329 console-var/menu helper set and so need a fresh byte contract.\n")
    w("| idx | RVA | class | insn | FDE B | guard | jcc | top callees | note |")
    w("|--:|---|---|--:|--:|:-:|--:|---|---|")
    for g in uniq:
        top = ", ".join(f"`{k}`{('=' + HELPER_NAMES[int(k, 16)]) if int(k, 16) in HELPER_NAMES else ''} x{v}"
                        for k, v in sorted(g["callees"].items(), key=lambda kv: -kv[1])[:3])
        note = ""
        if g["direct_calls"] and len(g["callees"]) == 1:
            note = f"{g['direct_calls']} calls to one callee (table-registration run?)"
        elif g["call_density"] < 0.02 and g["instructions"] > 500:
            note = "mostly inline stores/arithmetic (constant-table style), few calls"
        w(f"| {g['index']} | `{g['rva']}` | {g['class']} | {g['instructions']} | {g['fde_size']} | "
          f"{'Y' if g['stack_guard'] else 'n'} | {g['cond_branches']} | {top or '-'} | {note} |")
    poles = [g for g in ranked if g["effort_points"] >= 40.0]
    w("\n## Long poles\n")
    w("Slots at or above 40 points dominate their batch and should be scheduled first or split into a dedicated task "
      "with its own decomposition (repeat-block recognition: these are runs of the same ConCmd-var construction "
      "sequence, so a block-structure checker plus a table-driven model is the sensible shape): " + ", ".join(
          f"{g['index']} (`{g['rva']}`, {g['fde_size']} B, {g['instructions']} insn, {g['atexit_calls']} atexit, "
          f"{g['effort_points']} pts)" for g in poles) + ".\n")
    w("Batch notes: each batch's first item should be its heaviest slot; the other workers are not blocked on it. "
      "Shared lib/shape headers that workers may be tempted to extend (for example `lane_m6_shapes.hpp`, "
      "`ctor_slot_family_test_util.hpp`, `tools/lift/l4_slot_lib.py`, `slot_pin_util.py`, `slotfam_specs.py`) are "
      "integrator-owned: a worker that needs a new shape adds it to its own `ctor_slot_<N>_*` header and files a "
      "promote-to-shared request in its bundle.\n")
    w("## Serialising the shared files\n")
    w("Every slot needs edits in four files no two workers may touch concurrently: `native/CMakeLists.txt` "
      "(`target_sources`, `add_executable`, `add_test`), `native/core/startup_ctors.hpp` (`#include` + `StartupState` "
      "member) , `native/core/startup_ctors.cpp` (one `CtorSlot` row, index-ordered) and the count pins below. "
      "Pin sites found by the script:\n")
    for p in result["pin_sites"]:
        w(f"- `{p['file']}:{p['line']}`: `{p['text']}`")
    w("""
Recommended protocol (proposal, not yet implemented):

1. **Workers never edit the shared files.** Each worker delivers a bundle per slot under
   `local/validation/<TASK-ID>/bundle/slot_<N>/`: the four own files (header, source, test, checker) plus
   `integration.txt` with exactly the lines to paste: the `#include`, the `StartupState` member (+ result
   member if any), the `CtorSlot` row, the three CMake stanzas, and the pin deltas. The worker runs its own
   test/checker against a private scratch build that appends those lines to a *copy* of the shared files.
2. **One integrator applies bundles in ascending index order**, one commit per batch round, bumping the pins
   once: `slots.size() == N`, `first_unrecovered_constructor_index() == K`, `slots.size() + (573 - N)`, the
   `COVERAGE_TOKENS` in `check_console_var_ctors.py`, `len(unregistered)`/`len(rows)` in the two checkers.
   Rows are inserted in index order, so the `first_unrecovered` pin is the lowest index still unregistered.
3. **Reduce the bottleneck structurally** (follow-up task for the integrator): (a) replace the literal count
   pins by values derived from `original_constructors()` and `first_unrecovered_constructor_index()` so adding
   a slot changes no pin; (b) let CMake `file(GLOB_RECURSE CONFIGURE_DEPENDS native/core/ctor_slot_*.cpp)` and
   a foreach over `native/tests/test_ctor_slot_*.cpp` replace the per-slot stanzas; (c) generate the include
   and row blocks from a manifest. With (a)+(b) only `startup_ctors.{hpp,cpp}` stays shared, and it is a
   one-line-per-slot, conflict-free append for a single integrator.
4. Rounds: workers run in parallel; at each round end they hand bundles to the integrator, who merges all
   bundles of that round in one pass, runs the verification block from AGENTS.md once, and publishes. Workers
   start the next round from the merged tree. A worker whose slot needs a helper that another batch also
   needs must stop and ask the integrator (the batching rule above is meant to make this rare).
""")
    return "\n".join(L) + "\n"


if __name__ == "__main__":
    raise SystemExit(main())
