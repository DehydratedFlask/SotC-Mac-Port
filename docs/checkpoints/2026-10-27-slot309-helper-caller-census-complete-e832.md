# E832 — Slot-309 helper caller census

- **Date / local time:** 2026-10-27 16:09 CDT (America/Chicago).
- **Integrator:** `/root`.
- **Task:** `CPU-01-STARTUP-SLOT-309-HELPER-CALLER-CENSUS-01`.
- **Status:** **PARTIAL / STATIC_ANALYSIS_VERIFIED** for two saved-reference rows and the exact pinned-ELF call chain. The parent `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` remains ACTIVE. Keep slot 309 unregistered, `+0x10` ownership UNKNOWN and G1 PARTIAL.
- **Attempts:** 0 failed semantic hypotheses; one bounded caller census with a read-only Ghidra replay and exact-ELF byte profile.
- **Source pin:** `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Findings

The existing Ghidra ReferenceManager and an identical `-noanalysis -readOnly` replay report one
recorded incoming call for each helper:

| Helper | Callsite | Caller |
|---|---:|---|
| `0xa50280` | `0xa50c57` | slot-309 body `0xa505e0` |
| `0xa50440` | `0xa503d7` | helper `0xa50280` |

Pinned-ELF disassembly and the hash-checking byte profiler confirm that `0xa505e0` passes a pointer
to its stack record at `[rsp+0x60]` to `0xa50280`. The function zeros `ymm0` at `0xa50bba`, then
stores that 32-byte vector at `[rsp+0x60..0x7f]` at `0xa50c14`; no call intervenes. The qword at
record `+0x10` is therefore zero in this observed invocation. This does not establish values for
other runtime contexts.

`0xa50280` copies the 32-byte record to a local buffer and passes it to `0xa50440`. The latter copies
source bytes `+0x08..+0x17` to destination `+0x88..+0x97`, propagating source `+0x10` to destination
`+0x90`. These are copy observations only; no semantic interpretation, nonzero seed, owning release,
or general lifetime contract is supported. Saved resolved references do not cover unresolved
indirect/runtime paths.

## Private evidence and exact commands

Worker report: `local/validation/orch-20261027/e831/slot309-helper-caller-census/REPORT.md`,
SHA-256 `b3cdfc8acd527ad3c9717c944472e802349534b098faecb7328c6d0242350559`. Exact saved-project
commands, arguments, output paths, tool versions and command outcomes are in the adjacent
`REPRODUCE.md`. Both `analyzeHeadless` read-only queries and replay exited 0; both disassembly calls,
`profile_callers.py`, and both `verify_refs.py` commands exited 0. The query verifier reports exact
matching caller rows. Root ran `shasum -a 256 -c
local/validation/orch-20261027/e831/slot309-helper-caller-census/SHA256SUMS.txt` from the workspace
root; all **24/24** entries passed. Independent reviewer `/root/slot309_helper_caller_review`
approved and independently ran the byte profile and both ReferenceManager verifiers.

One initial root hash-check attempt ran from inside the evidence directory, although the manifest
paths were workspace-root-relative; it could not open the listed files. The command was corrected
to run from the workspace root and passed 24/24. This was a working-directory mistake, not an
analysis or evidence failure. The root pass log is private at
`local/validation/orch-20261027/e831/root-hash-audit.log`.

## Repository verification

Commands run from the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed
**516 / 0 skipped / 0 failed**; Trail graph check passed at **9,877 nodes / 13,470 edges** (deep
layer not built); project index regenerated and checked current at **31,947 folders**. Logs are
private under `local/validation/orch-20261027/e832/`. These checks do not advance a game gate.

## Publication baseline

E831 claim commit `ca811c2373300b79100880fe801256b55d7f8097` was pushed and fetched at `origin/main`.
Post-E831 remote scan: 9 branches, 12 PR heads, 0 tags, 15 unique tips, **2,444 reachable commits**,
zero Claude-family author/committer identity or credit hits. Private stash
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` remains unchanged per owner instruction.
