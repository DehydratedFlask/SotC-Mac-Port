# E865 — Reconcile APP-01 status and current attribution census

**Date:** 2026-10-27 19:54 CDT
**Integrator:** `/root`
**Task:** `DOC-APP01-PUB-RECONCILE-E865-01`
**Status:** **ARTIFACT_VERIFIED** — registry correction and required repository checks passed.
**Attempts:** 0 failed product hypotheses; one corrected local scanner-path invocation.

## Current APP-01 state

The current `LauncherModel` depends on and routes lifecycle queries/requests through `CoreClient`;
the child-process locator, protocol client, and factory are documented in E613. E614 verifies a
signed thin-arm64 launcher bundle containing `SoCGameCore`, but explicitly leaves Finder launch
NOT RUN. E775 reports the full launcher suite at 157/157 tests across 20 suites. These facts
supersede stale task-register statements that there is no core ABI/bundle or that the launcher is
not routed through `CoreClient`.

Live source at `native/api/sotc_core.cpp:363-389` remains fail-visible: after valid assets,
`sotc_core_start()` returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`, transitions to FAILED, and never
RUNNING. G4 remains PARTIAL. The UI/VoiceOver task is still waiting because the current Mac session
is locked; no UI action was attempted. No native or product code changed.

## Current public-ref attribution census

The scanner is a copy of the E863 checker-pattern census, placed at
`local/validation/orch-20261027/e865/attribution/scan_all_attribution_roots_e865.py` so its
repository-root resolution is preserved. Script SHA-256:
`3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`. Report SHA-256:
`dafbaacf9d060af67cee5f61767fa5d74b0c675aaeffa7e93906abb8cda282c0`.

At base `c68be85c17037f9e63033d58d743a806d7313d78`, 37 named non-stash refs and all 21 live
advertised GitHub refs (21 unique combined tips) each reach 2,477 commits. All 2,477 objects
parsed. Claude/Anthropic identities, credit lines, and attributed commits: **0**; assistant credit
lines: **0**. Four assistant-alias identity headers belong to the two previously identified
non-Claude `orchestrator@local` commits. The current stash was read only and stayed at
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; its 21 reflog tips still reach 826 commits, with 70
Claude/Anthropic credit-line matches, per the user's direction to leave private stashes unchanged.

The exact-range checker passed for the E864 base tip and, after commit, for E865 against
`origin/main..HEAD` before push. Full census output and checker logs are under
`local/validation/orch-20261027/e865/`.

## Repository verification

Commands and results from the repository root:

- `cmake -S native -B local/build/orch-r22` — exit 0.
- `cmake --build local/build/orch-r22 -j 12` — exit 0; Ninja had no work.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — **431/431 passed**.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — **516 passed / 0 skipped / 0 failed**.
- `trail build --check` — graph check OK at **9,877 nodes**; deep meaning layer remains unbuilt.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — **32,007 folders**, current; `docs/checkpoints` has 886 entries.
- `git diff --cached --check` — passed after removing four trailing Markdown hard-break spaces caught on the first check.
- `python3 -I scripts/audit_publish_paths.py` — **6 allowlisted paths** passed.
- `python3 -I scripts/audit_publish_paths.py --all` — **3,082 paths** passed.
- `python3 -I scripts/check_commit_attribution.py --range origin/main..HEAD` — passed post-commit for E865 before push; output is `attribution-prepush.log`.

Logs are `cmake-configure.log`, `cmake-build.log`, `ctest.log`, `lift-checkers.log`,
`trail-check.log`, `project-index.log`, `path-audit-staged.log`, `path-audit-all.log`,
`attribution-newest.log`, `attribution-prepush.log`, and `attribution-census.log` in
`local/validation/orch-20261027/e865/`.
Scanner source and JSON report hashes are recorded above. Push/fetch verification is recorded by
the final git state and user-facing result after publication.

## Failed invocation

The first local scanner invocation was placed one directory shallower than its E863 source, so its
`Path(__file__).resolve().parents[5]` root calculation selected the parent of the repository and
`git for-each-ref` exited 128. No refs or stash changed. The unchanged scanner was then copied into
the expected `attribution/` subdirectory and completed successfully. Command and traceback summary
are preserved in private `local/validation/orch-20261027/e865/attribution-placement-failure.md`.

## Next exact action

After fetch-verified publication, keep the native-port goal active and rerun bounded product
readiness triage when new source evidence or an external prerequisite changes. APP-01 VoiceOver
readiness remains waiting on an unlocked Mac; the native engine still has no implemented start path.
