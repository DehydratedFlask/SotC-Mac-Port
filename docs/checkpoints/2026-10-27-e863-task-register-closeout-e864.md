# E864 — Reconcile E863 publication status in the task register

**Date:** 2026-10-27 19:47 CDT
**Integrator:** `/root`
**Task:** `PUB-01-E863-TASK-REGISTRY-CLOSEOUT-04`
**Status:** **ARTIFACT_VERIFIED** — the E863 publication fact is confirmed, the task register reflects it, and the documentation checks passed.
**Attempts:** 0 failed hypotheses; one registry correction.

## Current publication evidence

E863 commit `1c2088bf1cd7076f86bee5d20d3458a2f56481fe`
(`docs: finalize public attribution handoff`) is authored and committed by Johnny Wang
`<dehydratedflask@users.noreply.github.com>`. `python3 -I scripts/check_commit_attribution.py`
passed against `origin/main..HEAD`; the commit was pushed to `origin main`, fetched, and confirmed
as `HEAD == origin/main == FETCH_HEAD ==` the commit above. The live advertised `main` ref matched.

The fresh E863 scan covers 37 named non-stash refs and all 21 advertised GitHub refs / 2,475
reachable commits at parent tip `c01d1b9c`, with zero Claude/Anthropic identities or credit
lines. The E863 commit itself was then separately exact-range checked. This resolves the stale
`PUB-01-E862-PUBLISHED-CLOSEOUT-03` sentence in `docs/TASKS.md` that still described publication
as pending.

The user chose to leave private stashes unchanged. The private scan remains 21 reflog tips / 826
commits / 70 Claude/Anthropic credit lines at `refs/stash` SHA
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; no stash ref or reflog was changed.

## Repository checks

From the repository root, with outputs saved under `local/validation/orch-20261027/e864/`:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
git diff --cached --check
python3 -I scripts/audit_publish_paths.py
python3 -I scripts/audit_publish_paths.py --all
```

Configure/build exited 0; Ninja reported no work. CTest passed **431/431**. Lift checkers passed
**516**, with **0 skipped / 0 failed**. Trail reported the 9,877-node wiring graph current and
`graph check: OK`; the deep meaning layer remains unbuilt. Project-index generation wrote **32,005**
folders; direct `--check` reported current. `git diff --cached --check` passed. The staged path
audit passed **6** allowlisted paths and the all-path audit passed **3,081** paths. Exact logs:
`cmake-configure.log`, `cmake-build.log`, `ctest.log`, `lift-checkers.log`, `trail-check.log`,
`project-index.log`, `path-audit-staged.log`, and `path-audit-all.log` in the validation directory.

## Scope and project status

This correction changes documentation only: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No native source, original game
input, private evidence, stash, or runtime behavior changed.

Current triage still found no independent `READY` product task. Slot 309 lacks a verified
semantic producer/consumer path; AUD-02 needs a released query scope; real save/load needs
serializer/deserializer evidence and a user-owned save; launcher UI validation needs an unlocked
Mac; GPU work remains dependency-gated. Keep the native-port goal active and acceptance
**PARTIAL**. Resume implementation when new evidence or a prerequisite opens a bounded product
task.
