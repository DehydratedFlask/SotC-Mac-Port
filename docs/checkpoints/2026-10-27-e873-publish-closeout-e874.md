# E874 — E873 publication closeout and current task triage

Date: 2026-10-27 (local, America/Chicago)
Agent: `/root`
Task: `PUB-01-E873-PUBLISH-CLOSEOUT-01`
Status: `ARTIFACT_VERIFIED`

## E873 publication verification

E873 commit `72eb3f529f08dc1f7af87d7aa99103b67987408f` (`docs: record E873 attribution census and APP-01 follow-up`) was created by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. Its exact range from the previously measured E872 tip passed `python3 -I scripts/check_commit_attribution.py --range origin/main..HEAD` before the push. `git push origin main` exited 0. After `git fetch origin`, local `HEAD`, `origin/main`, `FETCH_HEAD`, and live `refs/heads/main` all equal `72eb3f529f08dc1f7af87d7aa99103b67987408f`.

The live GitHub contributors endpoint, `gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'`, exited 0 and listed only `DehydratedFlask`. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`, unchanged. The E873 all-ref census remains measured at E872 HEAD and covers 2,485 reachable commits with zero Claude/Anthropic identities or credits; E873 was exact-range checked separately and is owner-authored.

## Current task readiness

`rg -n '\| READY' docs/TASKS.md` exited 1 with no matches. The current task scout found no newly actionable product task. Remaining prerequisites include a nonzero producer or semantic consumer for slot 309, a released query scope for AUD-02, a native serializer and real save/load path for SAV-01, and verified VoiceOver traversal/speech/caption output for APP-01. G4 remains `PARTIAL`; no game boot, audio, save/load, or full-port completion is claimed.

## Verification and evidence

The publication gates and private command outputs for E873 are recorded in `local/validation/orch-20261027/e873/attribution/`. This closeout rechecks the commit range, GitHub ref state, contributors endpoint, unchanged stash SHA, and task readiness. Exact E874 command outputs are under `local/validation/orch-20261027/e874/`.

| Command | Result |
|---|---|
| `python3 -I scripts/check_commit_attribution.py --range origin/main..HEAD` (before E873 push) | Exit 0; E873 commit attribution clean |
| `git push origin main` | Exit 0; E873 pushed to `origin/main` |
| `git fetch origin` and compare `HEAD`, `origin/main`, `FETCH_HEAD`, `git ls-remote --refs origin refs/heads/main` | Exit 0; all four identify E873 `72eb3f529f08dc1f7af87d7aa99103b67987408f` |
| `gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'` | Exit 0; only `DehydratedFlask` |
| `git rev-parse refs/stash` | Exit 0; unchanged `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` |
| `rg -n '\| READY' docs/TASKS.md` | Exit 1; no READY task rows |

E874's required publication gates also passed:

| Command | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | Exit 0; `cmake-configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | Exit 0; Ninja reported no work; `cmake-build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | Exit 0; 431/431 passed; `ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | Exit 0; 516 passed / 0 skipped / 0 failed; `lift.log` |
| `trail build --check` | Exit 0; wiring graph OK at 9,877 nodes; deep meaning layer remains unbuilt; `trail.log` |
| `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` | Exit 0; 32,018 folders indexed and current; `project-index.log`, `project-index-check.log` |
| `python3 -I scripts/audit_publish_paths.py` | Exit 0; 6 staged paths passed; `audit-staged.log` |
| `python3 -I scripts/audit_publish_paths.py --all` | Exit 0; 3,091 paths passed; `audit-full.log` |
| `git diff --cached --check` | Exit 0; `diff-check.log` |
| `python3 -I scripts/check_commit_attribution.py` | Exit 0 for the precommit range; `attribution-precommit.log` |

An initial task-readiness scan returned a match because the closeout task row itself contained the literal query pattern. That wording was corrected; the final scan exited 1 with no matches. The checker output is in `task-ready.log`.

Files changed for this closeout: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No source, game input, dependency, or stash changed; `INSTALL_LOG.md` remains unchanged.

Remaining limits: the scan does not cover unreachable objects outside current named refs, advertised refs, and stash reflog roots. Private stashes remain unchanged by user direction. VoiceOver output remains unverified. G4 is still `PARTIAL`.
