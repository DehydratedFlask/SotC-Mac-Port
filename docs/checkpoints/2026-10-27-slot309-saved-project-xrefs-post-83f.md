# Slot 309 saved-project Ghidra census and post-`83f78f4f` verification (E770-E772)

- Local date/time: 2026-10-27 07:41 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- Status: **STATIC_ANALYSIS_VERIFIED** for bounded saved-project references/decompilation; slot 309 remains ACTIVE/G1 PARTIAL. **ARTIFACT_VERIFIED** for fetched GitHub refs under the owner's explicit stash scope.
- Attempts: 0 failed semantic hypotheses. The rejected `.gpr` REA open and unsupported `-list` option were tooling errors; no new semantic try failed.

## E770: saved-project static analysis

The existing Ghidra project `local/ghidra/persistent-20261020/SotC.gpr` / `.rep` was reopened using
`-process eboot.elf -noanalysis -readOnly`; this reused the saved database without a new import or
game execution. The loaded program is `eboot.elf` at `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf`,
SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base `0x0`,
language `x86:LE:64:default`. Ghidra version is 12.1.4 PUBLIC on OpenJDK 25.0.4.1 / macOS ARM64.

The incoming-reference census found one `DATA` reference to `0x531d60`, originating from pointer
table data at `0x15c8cf8`; it did not find a code caller. The slot itself has zero analyzed incoming
references, as does custom metadata address `0x2e716a0`. The table base `0x15c8cc8` has two data
references, from object setup functions `0x536aa0` and `0x537f90`. Their callers lead to
`0x536730` and `0xf120`, but no startup order or connection to constructor slot 309 was established.
The only incoming reference to constructor `0xa505e0` is its data slot at `0x2dd6738`.

Decompiler output describes `0xa50220` as copying an `undefined8` from offset `+0x10` and writing
the table pointer at `+0x20`; `0xa50270` returns without work. Ghidra has no declared semantic type
for the table or copied qword, and does not define `0xa50240` as a function in this project. This
does not resolve the `[RBP-0x38]` ownership or cleanup contract. Do not infer triviality or select
either `*(0x2dfd4b8)` branch. Keep slot 309 unregistered; no native-lift boundary is supported.

Private report and exact commands, limits, and log hashes:
`local/validation/orch-20261027/slot309/ghidra-readonly-xrefs/REPORT.md`, SHA-256
`f1f8eb90cd3961d071109d8b48e8c846e19c6f2769b4b71a0616e584dacbb0aa`.
Ghidra refreshed internal `~index.dat` / `~index.bak` sidecars; the `.gpr` marker and 350,109,696-byte
program database retain their 2026-10-20 modification times. No program analysis or annotations
were saved.

## E771: current public-ref attribution

`git fetch origin --prune` and the remote scanner exited 0. The scan covers 9 fetched branch heads,
0 remote tags, and 2,387 unique reachable commits through `origin/main ==
83f78f4fc43b2ec5cbd6b7dab0eab81d46af53d3`, with zero Claude/Anthropic identity or credit-line
matches. Private report:
`local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-resume-after-83f78f4f.json`,
SHA-256 `9c17316ac1d8260c998ef7b73a3aef42a091897a0b6f24dd1665304ce382869c`.

The owner chose to leave private `refs/stash` unchanged. Its 70 stash-only matches remain untouched
and excluded from public-history cleanup.

## Files changed and repository verification

Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/ATTRIBUTION.md`,
`docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint. No source, reference clone, game input, or
installation changed.

| Command | Result | Evidence |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | **PASSED**, exit 0 | `configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | **PASSED**, exit 0; no work to do | `build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **PASSED**, 431/431, exit 0 | `ctest.log`, SHA-256 `1e885df7681fb723ff5316bc80001950757c6cbcb9d627113816a1ff8999b075` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **PASSED**, 516/0/0, exit 0 | `lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | **PASSED**, wiring graph in sync; deep layer not built and 9,875 nodes remain pending | `trail-check.log`, SHA-256 `faf3792f3bd222b9d413eefb67ccad8b2d488029cc2ffbe5c38cb8c65e5c723f` |
| `python3 scripts/project_index.py` | **PASSED**, 33,111 folders indexed, exit 0 | `project-index.log`, SHA-256 `b3bfd2309be519ba57fb0144a47a0190d9d9a63e217084093ddc759876ffd1bb` |
| Staged publication-path and attribution audits | See results below | `local/validation/orch-20261027/slot309-checkpoint/e770/` |

The private result manifest `local/validation/orch-20261027/slot309-checkpoint/e770/results.json`
has SHA-256 `480ceae68e6d4a245d392d41737b53124ecc0f44081a76bcf252d2e2b873091d`.

| Command | Result | Log SHA-256 |
|---|---|---|
| `python3 -I scripts/audit_publish_paths.py` | **PASSED**, 7 staged paths, exit 0 | `5cdec42e77d35ed6592b011409cf38d7a0da5396f3b277cb7ed99ec083bb7119` |
| `python3 -I scripts/audit_publish_paths.py --all` | **PASSED**, 3,012 paths, exit 0 | `8c37c64a3c57b36868f82c670f676abdc9cbf33b3f9d41e1685f473c343b91ed` |
| `python3 -I scripts/check_commit_attribution.py` | **PASSED** for the pre-commit `origin/main..HEAD` range (empty), exit 0; rerun after commit | `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --identity` | **PASSED**, owner identity, exit 0 | `888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559` |
| `python3 -I scripts/check_commit_attribution.py --all` | **FAILED** only on previously documented non-Claude historical commit `f6604a86e60e` (`orchestrator@local`) | `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |

Audit-result manifest `local/validation/orch-20261027/slot309-checkpoint/e770/audit-results.json`
has SHA-256 `8037cafa725cc8bf02c0db190b7df3724c698daec7cb737ed80b0b1c85814008`.

No game boot, game audio, save/reload, Metal validation, or forbidden-runtime claim is made.

## Next exact action

Commit and publish the seven allowlisted documentation paths under the owner identity, rerun the
unpublished attribution-range check against the new commit, then refresh the remote-ref attribution
scan. Continue only from the saved Ghidra project for any further slot-309 xrefs; if no exact payload
type/ownership or invocation-order evidence emerges, preserve both branches and the hook boundary.
The current task register contains no other ACTIVE task.
