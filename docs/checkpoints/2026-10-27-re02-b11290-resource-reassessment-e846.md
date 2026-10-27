# E846 — b11290 resource-adjusted REA reassessment and live attribution census

**Date:** 2026-10-27 17:44 CDT

**Integrator / principal:** `/root`

**Task IDs:** `RE-02-APPMAIN-INIT-B11290-STRUCTURE-01`; `PUB-01-CLAUDE-CREDIT-CLEANUP-01`

**Status:** RE-02 remains `PARTIAL / STATIC_ANALYSIS_VERIFIED` for E844's hash-pinned offline byte profile. Attempt 2 is authorized by the principal but not yet run. Publication attribution is `ARTIFACT_VERIFIED` for the live advertised GitHub refs through E845, subject to the stated ref boundary.

**Attempts:** RE-02 attempt 1 was the E844 MCP REA startup timeout (one provider timeout; zero failed semantic hypotheses). This checkpoint authorizes a distinct resource hypothesis for attempt 2: the existing REA 6.7.0 CLI, Ghidra 12.1.4, an 8G heap and a 1,200,000 ms startup allowance, with identical target/query and acceptance. No second analysis has started yet.

## Principal reassessment and exact next action

The first MCP attempt opened the hash-matched target/snapshot but timed out during the provider startup/overview path after 330,000 ms with a 2G heap. The same repository's `docs/REVERSE_ENGINEERING.md` documents a successful recovery route through the already-installed REA CLI. This reassessment changes only provider resource limits and invocation path; it does not weaken the structural-dossier acceptance, change the pinned input, or infer semantics from the timeout.

- Target ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Snapshot source: `local/re/rea-snapshot.json`, SHA-256 `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`; 98,783 bytes (about 96 KiB); it matches the target and profile recorded in E843/E844. A byte-identical working copy with the same digest is staged privately at `local/validation/orch-20261027/e847/rea-snapshot.json` so the CLI cannot mutate the canonical snapshot.
- Existing CLI: REA `6.7.0`, launcher `/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea`, SHA-256 `ed06cee49505b95a3cc5fceb1b4b279167e26df86227c644bb366ffffdbaf738`. No package was installed or changed.
- Existing Ghidra: `/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC`; Java: OpenJDK `25.0.4.1` at `/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home`.
- Exact next command, from the workspace root, after E846 is published:

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xb11290 --provider ghidra --snapshot local/validation/orch-20261027/e847/rea-snapshot.json --format json > local/validation/orch-20261027/e847/rea-function.json 2> local/validation/orch-20261027/e847/rea-function.stderr
```

Record exact start/end times, exit status, CLI/provider identity, command/environment, and output hashes in `local/validation/orch-20261027/e847/run-metadata.json` and logs. The existing acceptance remains the bounded structural dossier specified in E843 and `docs/TASKS.md`: exact body range/hash, reachable instruction count, direct calls/branches, RIP-relative references, unresolved indirect transfers, and supported REA evidence. Any provider timeout or incomplete result remains `PARTIAL`; do not automatically retry. If this second distinct hypothesis fails, stop for principal reassessment before a third attempt. Do not edit the canonical REA snapshot or claim semantics, native liftability, registration, G1, or runtime behavior from a profile/query result alone.

## E846 fresh live GitHub attribution census

The read-only scanner `local/validation/orch-20261027/e846/scan_advertised_refs_e846.py` (SHA-256 `e645c40e5ad2f1b8c210856df4c8983b92868e55957ed16bf75f3fc961142ec8`) queried the current GitHub-advertised set: 21 refs (9 branches, 12 pull-request heads, 0 tags), 15 unique tips, and 2,458 reachable commits parsed. Claude/Anthropic author or committer identities and credit-line hits: **0**. Report: `local/validation/orch-20261027/e846/remote-attribution-scan-e846.json`, SHA-256 `e433cbe4ad019d9d9e4980ca1eaa0e41c95ba901b0e9fd825484725b1c987ecc`.

The same read-only scan found 21 stash-reflog tips, 826 reachable commits, and 70 Claude/Anthropic matches. The owner explicitly chose to leave private stashes unchanged; `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`. No stash ref, reflog, or remote ref was modified. Unreachable objects outside the advertised refs and examined stash reflog are outside this census. REA CLI help smoke output and all scan artifacts remain under the ignored `local/validation/` directory.

## E846 repository verification

- `cmake -S native -B local/build/orch-r22` — exit 0, 0.207 s; log `local/validation/orch-20261027/e846/configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0, 0.029 s; Ninja reported no work; log `build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431 passed, 8.433 s; log `ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed / 0 skipped / 0 failed, 97.186 s; log `lift-checkers.log`.
- `trail build --check` — exit 0; wiring graph in sync at 9,877 nodes; deep layer remains unbuilt; log `trail.log`.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — both exit 0; 31,971 folders indexed/current; logs `project-index-generate.log` and `project-index-check.log`.
- Staged/full publication-path audits passed 6/6 and 3,063/3,063. Worktree and staged diff checks passed; owner identity and precommit `HEAD^..HEAD` attribution passed (the latter covers the published E845 commit). A precommit `git fetch origin main` exited 0 and confirmed `HEAD == origin/main == FETCH_HEAD == e3005c456785a59d97717f06149e9207c70dff7c`. Logs are `audit-staged.log`, `audit-all.log`, `diff-check.log`, `staged-diff-check.log`, `identity.log`, `precommit-attribution.log`, and `fetch-before.log` in `local/validation/orch-20261027/e846/`. The exact E846 commit-range check, push, and post-push fetch verification follow commit creation and will be recorded in the E847 publication closeout.

## Observation, limits, and next state

Observed: live public refs remain clear of Claude/Anthropic attribution; the matching REA snapshot copy is byte-identical to its source; installed REA and Ghidra paths exist and were version-checked. Inference: larger provider resources may allow a complete function query, based on the repository's documented CLI recovery path; this has not yet been tested for `0xb11290`.

Parallel read-only task triage found no independent product task marked `READY`. APP-01 VoiceOver readiness still needs an unlocked Mac, SAV-04 needs a user-owned PS4 save, AUD-02 still lacks a released provider-query scope, GPU lanes await dependencies, and startup slot 309 remains gated on a concrete nonzero producer or semantic consumer. These are unchanged prerequisites, not completed work.

No source implementation or product gate changes here. The E844 profile is static structure only; function meaning, native-lift boundary, startup registration, G1, cold boot, game audio, real save/reload, and Metal validation remain unproven. Next: complete the E846 publication checks and push/fetch the owner-authored documentation checkpoint; then run the single authorized attempt-2 command above and record its exact result in E847.
