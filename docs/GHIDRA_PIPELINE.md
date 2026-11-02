# Ghidra decompilation pipeline (PS4 guest executable, offline)

**Status:** `ARTIFACT_VERIFIED` for the five dossiers below — they were produced by the commands on
this page and saved under `local/validation/ghidra-decompile-20261101/`.
`STATIC_ANALYSIS_VERIFIED` for the facts read out of them.
No guest code was executed: this pipeline only reads an image Ghidra already loaded.

## Verified line (2026-11-01, one attempt, no retries)

| Step | Result |
|---|---|
| `shasum -a 256 local/re/eboot.elf` | **RAN** — `d0723afb…61d3`, matches the pin |
| `run_dump.py --check-project` on the 2026-10-20 project | **RAN**, exit 0 — lists `/eboot.elf` |
| `run_dump.py --script probe_project.py …` | **RAN**, exit 0 — language, base, 38,845 functions, all five RVAs are functions |
| `run_dump.py --script dump_functions.py …` | **RAN**, exit 0 — five bodies, 2,372 decompiled lines |
| Same dump a second time | **RAN**, exit 0 — byte-identical except the `DECOMPILE_SECONDS` lines |
| Same dump against the persistent project (not the copy) | **RAN**, exit 0 — identical, and `find -newermt` shows **no file in the project changed** |
| `analyzeHeadless … -postScript probe_project.py` | **FAILED**, exit 1 — Python unavailable in a plain headless JVM (the reason `run_dump.py` exists) |
| `python3 -I tools/lift/check_ghidra_pipeline.py` | **RAN**, exit 0 |
| Checker with the private inputs removed / with a truncated dossier / with a wrong ELF | **RAN** — SKIP exit 0 / FAIL exit 1 / FAIL exit 1 |
| Creating a new project and re-running auto-analysis | **NOT RUN** — not needed: the existing project hashes to the pinned image and already holds a finished analysis |
| Any guest instruction executed | **NOT RUN** — out of scope and forbidden |

## What is pinned

| Item | Value |
|---|---|
| Ghidra | **Ghidra 12.1.4** (`application.version=12.1.4`, build 2026-Sep-21) at `/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC` |
| analyzeHeadless | `/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless` |
| JDK | 25.0.4.1 (`/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home`) |
| Guest image | `local/re/eboot.elf`, 65,214,308 B, `sha256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` |
| Ghidra project | `local/ghidra/persistent-20261020` (project `SotC`, program `/eboot.elf`) |
| Dossier output | `local/validation/ghidra-decompile-20261101/` |

The project is the 2026-10-20 import. It is not stale: opening it and hashing the file Ghidra
recorded as the program source (`getExecutablePath()`) reproduces the **pinned SHA-256 above**, and
the database already holds a finished analysis (`function_count = 38845`, `language =
x86:LE:64:default`, `image_base = 0x0`, 206,621 symbols, 9 memory blocks).

## The one thing that surprises people: analyzeHeadless cannot run Python scripts

`analyzeHeadless` launches a plain JVM. This install has no Jython feature directory (Ghidra 12
dropped it), and a `.py` post-script fails with:

```
ERROR REPORT SCRIPT ERROR: probe_project.py :
  Ghidra was not started with PyGhidra. Python is not available
  ghidra.app.script.GhidraScriptLoadException
    at ghidra.pyghidra.PyGhidraScriptProvider.getScriptInstance(PyGhidraScriptProvider.java:75)
```

That was measured on 2026-11-01 against this install (exit 1, log
`local/validation/ghidra-decompile-20261101/probe-existing.log`). Python only works when the **JVM is
started by PyGhidra**. `tools/re/ghidra/run_dump.py` is that entry point: it starts PyGhidra, opens
the project, opens `/eboot.elf`, and runs the given GhidraScript with the program as
`currentProgram`. The scripts stay ordinary GhidraScript files, so they also work from the Ghidra
GUI or any future headless build that does start PyGhidra.

## One-time setup: a private PyGhidra python

The wheels Ghidra ships are used offline; nothing is downloaded. Python must be one of the versions
Ghidra's wheels cover (3.9–3.13); the system `python3` here is 3.15, which has no matching wheel.

```sh
/opt/homebrew/bin/python3.13 -m venv "local/venv-pyghidra"
./local/venv-pyghidra/bin/pip install --no-index \
    --find-links /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/Ghidra/Features/PyGhidra/pypkg/dist \
    pyghidra
```

Produces `pyghidra 3.1.0` + `jpype1 1.5.2` + `packaging 25.0`, all from
`Ghidra/Features/PyGhidra/pypkg/dist`. `local/` is private and git-ignored, so this environment is
never published. See `INSTALL_LOG.md`.

## Reproduce from scratch

Run from the workspace root. `JAVA_HOME` and `GHIDRA_INSTALL_DIR` must be exported; everything else
is a flag.

```sh
export JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home
export PATH="$JAVA_HOME/bin:$PATH"
export GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC

# 0. Confirm the image is the pinned one.
shasum -a 256 local/re/eboot.elf

# 1. Confirm the project opens and is the same image (name, language, base, function count,
#    and which RVAs are functions). Read-only; no script runs against the ELF.
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/probe_project.py --out /dev/null \
    0x2569d0 0x1bdf30 0x1bdb10 0xa2afb0 0x531d60

# 2. Produce the dossier.
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/dump_functions.py \
    --out local/validation/ghidra-decompile-20261101/dossier-20261101.txt \
    0x2569d0 0x1bdf30 0x1bdb10 0xa2afb0 0x531d60

# 3. Check it.
python3 -I tools/lift/check_ghidra_pipeline.py
```

Step 2 took **3.7 s of decompiler time** for all five bodies on this machine (`0x2569d0` alone is
3.25 s; the whole run including JVM and project start is ~30 s). No auto-analysis is re-run, no
program is saved, and the project is opened for reading only. The 2026-11-01 run did it twice: once
against a copy (`local/ghidra/work-20261101`) and once against `local/ghidra/persistent-20261020`
itself. Both produced the same dossier apart from timings, and the persistent project's files were
unchanged afterwards, so the copy was belt-and-braces rather than a requirement.

To check the project without against-the-clock surprises, add `--check-project` and skip `--script`;
it lists the programs in the project.

## Running it against a project of your own

The driver takes the project on the command line, so nothing is hard-coded except the default
Ghidra installation:

```sh
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir <dir> --project-name <name> --program /<program> \
    --script tools/re/ghidra/dump_functions.py --out <file.txt> <rva> [<rva> ...]
```

`dump_functions.py` writes, per RVA: `ENTRY`, `ADDRESS`, `NAME`, `SIZE` (Ghidra body bytes),
`BLOCKS`, `SIGNATURE`, `CALLEE_COUNT` + one `CALLEE=` row per called function, `CALL_SITE_COUNT` +
`CALL_SITE=` rows (including `indirect` call sites Ghidra could not resolve), `DECOMPILE_SECONDS`,
`DECOMPILE_STATUS`, `C_LINES`, and the decompiled C between `--- decompiled C ---` markers.

`probe_project.py` writes `PROBE key=value` lines: program name, language, compiler spec, image
base, executable path/MD5/SHA-256, function and symbol counts, and `present/kind/size/blocks` for
each RVA.

## The five dossiers

| RVA | Ghidra body | Decompiled C | Callees | What it is |
|---|---|---|---|---|
| `0x2569d0` | 12,626 B | 1,589 lines | 74 | `owner_properties_2569d0` — owner construction, then the constant property table |
| `0x1bdf30` | 1,479 B | 181 lines | 17 | app-info registry dispatch/drain |
| `0x1bdb10` | 56 B | 16 lines | 0 | app-info registry constructor |
| `0xa2afb0` | 441 B | 64 lines | 8 | per-user input-device owner (four 0xc88 objects) |
| `0x531d60` | 3,889 B | 527 lines | 49 | the closure root of `docs/INDIRECT_FRONTIER.md` (499 indirect sites) |

Sizes are Ghidra's function body (`getBody().getNumAddresses()`), which is the union of instruction
bytes it owns. They differ slightly from the FDE lengths quoted in `AGENTS.md` (e.g. `0x2569d0` is
12,621 B there, 12,626 B here) because an FDE range is the unwind span, not the byte set. Both
numbers are kept rather than reconciled.

### What the output corroborates, without re-deriving it

These are readings of the decompiled text, recorded because they match the contract in `AGENTS.md`:

- `0x1bdb10` writes three self-referencing list heads at `+0x00/+0x08`, `+0x20/+0x28`,
  `+0x40/+0x48` and zeroes `+0x10`, `+0x30`, `+0x50` — the 0x60-byte three-list container.
- `0x1bdf30` walks list C first (`param_1[8]` = `+0x40`), calling `node[3]` (`+0x18`) with
  `(node, registry)`; then serializes list A (`*param_1`, entries using `entry[2]` `+0x10` and
  `entry[6]` `+0x30`); then drains list A again where `*(int *)(entry + 0x50) == 1`; then clears
  list B (`param_1 + 4` = `+0x20`) with two `0xf590` releases per element (`+0x10`, `+0x30`).
  The dword flag really is at `+0x50`, and the drain really is `flag == 1`.
- The same body names its own work: `"BugData"`, `"FieldEntry"`, `"Attachment"`, `"Command"`,
  `"screenshot"`, `"bug_screenshot.png"` — the `snapshotBugData` path, consistent with `+0x56`
  being the only normal dispatch gate.
- `0xa2afb0` allocates 0xc88-byte objects in a loop over user ids, consistent with the
  per-user input-device reading.
- `0x2569d0` is two things in one body. Its first half allocates a 0x350-byte object, installs
  vtable `0x159e208` on an 8-byte shim, stores both in `DAT_02ee2d00` / `DAT_02ee2a50` with a
  release of the previous object, and then calls ~40 subsystem initializers with the new object.
  Its second half is the property table: **259 calls** to `FUN_00ac5260` / `FUN_00ac5320`
  (`insert_equal` consumers) with keys `0x00..0x9f`, each passed a small stack record. The records
  are visibly typed — e.g. `local_918 = 0x3f80000000000000; local_910 = 0x800000043f800000;` is
  `{f32 1.0f, u32 4}`, and `local_928 = 0x7f7fffffff7fffff; local_920 = 0x800000033f800000;` is
  `{f32 FLT_MAX, f32 FLT_MAX, f32 1.0f, u32 3}`. That is the `{i32,f32=1,u32}` map A and the
  `{f32 lo,f32 hi,f32 v,u32}` map B from `AGENTS.md`, read straight off the decompiled text.

## Limits

- **Signatures are not recovered.** Ghidra has no calling convention here, so every function is
  `undefined FUN_xxx()` and the C uses `undefined8`/`long`/`long *` parameters with no names. The
  bodies are faithful; the types are not.
- **`0x2569d0` decompiles to one block** (`BLOCKS=1`) — Ghidra's body for it is a single range even
  though `AGENTS.md` counts 87 reachable blocks. Block counts are structural counts of the *body
  address set*, not of the control-flow graph; do not read `BLOCKS` as a CFG measure.
- **Indirect calls are listed, not resolved.** `CALL_SITE=... -> indirect` rows are calls through a
  register or vtable slot; the callee list only holds statically known targets.
- The dossier is **evidence of what the binary says**, not a model. Lifting still has to be done by
  hand in `native/`, and no claim about game behaviour follows from any of it.

## Checker

`tools/lift/check_ghidra_pipeline.py` is discovered by `tools/lift/run_all_checkers.sh` (it globs
`tools/lift/check_*.py` and skips only `check_all_*.py` and `*util*`). It pins this install path,
`analyzeHeadless`, the project path, the image SHA-256 and the five RVAs; when the private inputs are
present it re-parses the dossier and requires each RVA to be a function with the pinned body size, a
non-empty C body, and a callee list. It prints `SKIP` and exits 0 when the project, the ELF and the
dossier are all absent.

Measured 2026-11-01:

| Run | Result |
|---|---|
| `python3 -I tools/lift/check_ghidra_pipeline.py` | exit 0 — `5 sections, 2372 decompiled lines` |
| Same, with `local/` removed from a copy of the tree | exit 0 — `SKIP … (pipeline pins checked)` |
| Same, with a truncated dossier | exit 1 — `0x2569d0 decompile status is not ok` |
| Same, with a non-pinned `eboot.elf` | exit 1 — `local/re/eboot.elf is not the pinned image` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | two runs, both `717 passed, 0 skipped, 1 failed`; each time the single failure was a peer-owned artifact (`check_startup_slot309_table.py` mid-edit in the first run, `check_entry_map.py` reporting a stale `entry_map.json` in the second). `check_ghidra_pipeline.py` passed in both |
