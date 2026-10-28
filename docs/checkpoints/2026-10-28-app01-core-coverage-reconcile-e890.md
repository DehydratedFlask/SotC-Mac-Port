# E890 — reconcile current native core constructor coverage

- Local date/time: 2026-10-28 01:50 CDT (America/Chicago)
- Agent: `/root`
- Task: `APP-01-CORE-COVERAGE-COUNT-RECONCILE-01`
- Status: `ARTIFACT_VERIFIED` for the native core coverage report only; APP-01/G4 remain `PARTIAL`.
- Attempts: 0 failed hypotheses; 1 successful artifact measurement.

## Scope and provenance

This task reconciles the current native `SoCGameCore` coverage count with status summaries that
still said 308/573. The E868 workspace app bundle reported 311/573, and E865 documented an earlier
308/573 source snapshot. E618 recorded the intermediate count of 311 after three constructor
additions. Those are historical, artifact-specific measurements; this checkpoint does not rebuild
or claim the E868 launcher bundle.

## Exact build and observation

Commands:

```sh
cmake --build local/build/orch-r22 --target soc_game_core -j 12
file local/build/orch-r22/SoCGameCore
shasum -a 256 local/build/orch-r22/SoCGameCore
local/build/orch-r22/SoCGameCore --coverage
```

The targeted build exited 0 (`ninja: no work to do`). The executable is a Mach-O 64-bit arm64
binary with SHA-256
`b7f963b9fb8078095a730abe022f43300da1876c130ae851d5577d873f5ff0ec`. The CLI exited 0 and its
`startup_constructors` row says: **397 of 573 original constructor slots recovered; first
unrecovered index 309; 397 registered slots ran OK**. It also reports
`engine_implemented: false`, no engine boot or Metal renderer, no audio output/mixing, and only
fixture-verified save metadata/transaction support.

The current implementation in `native/api/sotc_core.cpp` counts distinct registered
`original_index` values below the fixed 573-slot table and formats the `completed` count separately.
That matches the existing E713 record that registration grew to 397/573 while leaving first gap
309. This runtime CLI result is consistent with E713; it does not prove a game boot, game audio,
real save/load, or a current app-bundle measurement.

Private evidence:

- `local/validation/orch-20261028/app01-coverage-reconcile-e890/build.log`
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/file.log`
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/sha256.log`
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/coverage.json` (SHA-256 `4f49160edcd498d2a0c238cbd21fa2f8e1dfa2504ee3b80bf88ccc0bb474158a`)
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/trail-coverage-query.log`

## Reconciliation

Current APP-01 summaries now report the native core's measured 397/573 and first gap 309. They
retain the E868 bundle's 311/573 and E865's 308/573 as historical snapshots and explicitly state
that this task verified the configured core target was up to date; it did not rebuild the launcher bundle. The task register records this artifact check without
promoting G1 or G4.

## Validation

- `cmake -S native -B local/build/orch-r22`: **PASSED**, exit 0.
- `cmake --build local/build/orch-r22 -j 12`: **PASSED**, exit 0 (`ninja: no work to do`).
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **PASSED**, 431/431.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **PASSED**, 516 passed / 0 skipped / 0 failed.
- `python3 scripts/project_index.py`: **PASSED**, 32,055 folders indexed; current. Log: `local/validation/orch-20261028/app01-coverage-reconcile-e890/project-index-final.log`.
- `trail build --check`: **PASSED**, graph 9,877 nodes in sync; deep meaning layer remains not built. Log: `local/validation/orch-20261028/app01-coverage-reconcile-e890/trail-check.log`.
- `python3 -I scripts/audit_publish_paths.py --all`: **PASSED**, 3,107 paths. Log: `local/validation/orch-20261028/app01-coverage-reconcile-e890/path-audit-all.log`.
- `python3 -I scripts/audit_publish_paths.py`: **PASSED**, 6/6 staged paths. Log: `local/validation/orch-20261028/app01-coverage-reconcile-e890/path-audit-staged.log`.
- `python3 -I scripts/check_commit_attribution.py`: **PASSED** for the empty precommit range; the exact commit-range check remains pending. Log: `local/validation/orch-20261028/app01-coverage-reconcile-e890/attribution-precommit.log`.
- `git diff --cached --check` and `git diff --check`: **PASSED**, exit 0. Logs: `diff-cached.log` and `diff-worktree.log` in the private E890 validation directory.

## Limits and next action

The current launcher bundle was not rebuilt or observed. The VoiceOver observation remains waiting
for manual Mac unlock and a usable speech/caption surface. No source code, original game input,
launcher preference, dependency, or stash changed. G1 and G4 remain `PARTIAL`; no boot/audio/save
claim is made. Next: finish publication-path, index, Trail, attribution, and diff audits, then commit
the allowlisted documentation checkpoint under Johnny Wang and push `origin main`.
