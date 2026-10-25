# Integration PR #8 closeout

**Date:** 2026-10-25 03:30 CDT

**Integrator:** Codex `/root`

**Task:** `INT-OPEN-PRS-1-2-3-5-01`

**Status:** FIXTURE_VERIFIED

## Merge and branch cleanup

- Repository: `DehydratedFlask/SotC-Mac-Port` on `github.com`; authenticated GitHub identity was
  `DehydratedFlask`.
- PR #8, `integration: reconcile reviewed work and publish current checkpoints`, merged normally at
  `2026-10-25T08:29:00Z`. Reviewed head was
  `8f57b64e58622ba181e797701ed407cd38e64cef`; GitHub merge commit is
  `61c96f6de128b53a5970a33ed504346e47ce42f2`.
- `gh pr merge 8 --merge --match-head-commit 8f57b64e58622ba181e797701ed407cd38e64cef
  --delete-branch` exited 0. GitHub deleted the remote branch.
- Exact local cleanup commands `git fetch origin main`, `git switch main`,
  `git pull --ff-only origin main`, and `git branch -d codex/merge-all-open-changes` all exited 0.
  `HEAD` and `origin/main` match `61c96f6de128b53a5970a33ed504346e47ce42f2`, and the old branch is
  absent locally and remotely.

## Verification on the merged PR tree

- `cmake -S native -B local/build/orch-r22` — PASSED.
- `cmake --build local/build/orch-r22 -j 12` — PASSED; full initial build completed 448/448 steps.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — PASSED, 397/397.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — PASSED, 481 passed,
  0 skipped, 0 failed.
- `trail build --check` — PASSED; graph has 8,490 nodes and 11,465 edges. Project index: 19,586
  folders.
- Staged path audit: 41 paths before merge commit; full audit: 2,780 paths; committed PR-diff audit:
  11 paths. Commit attribution and diff checks passed. Logs are under
  `local/validation/root-merge-integration-20261025/`.

## Limits and next work

PR #8 publishes the root GPU-03 mesh/import/frame records and the E641 checker fix. The specialized
asset-derived frame acceptance check failed its index-mutation gate after three hypotheses and is
`NEEDS_PRINCIPAL`; do not retry without a principal reassessment. G3 remains PARTIAL. No cold boot,
real game audio, or save/reload is claimed.
