# E845 — E844 publication and refreshed Claude attribution census

**Date:** 2026-10-27 17:34 CDT

**Integrator:** `/root`

**Task ID:** `PUB-01-CLAUDE-CREDIT-CLEANUP-01`

**Status:** `ARTIFACT_VERIFIED` for the current advertised GitHub refs through E844; the user-directed private-stash exclusion remains in force.

**Attempts:** one fresh read-only advertised-ref scan; no product-analysis attempt.

## E844 commit publication

- Commit: `ecb63ce92ee44c5987c7a955ac7916d079a971d8` (`docs: record partial RE-02 b11290 profile analysis (E844)`).
- Parent: `7a0fbef5423ac9e57693957cc20df146f3ebdd42`.
- Author and committer: Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- `python3 -I scripts/check_commit_attribution.py --identity` and `python3 -I scripts/check_commit_attribution.py --range HEAD^..HEAD` passed. The range log is `local/validation/orch-20261027/e844/post-commit-attribution.log`.
- The six staged paths passed `python3 -I scripts/audit_publish_paths.py` (6/6); the committed repository passed `python3 -I scripts/audit_publish_paths.py --all` (3,061/3,061). Logs are under `local/validation/orch-20261027/e844/`.
- `git push origin main` exited 0; after `git fetch origin main`, `HEAD`, `origin/main`, and `FETCH_HEAD` all resolved to `ecb63ce92ee44c5987c7a955ac7916d079a971d8`, with a clean worktree.

## Fresh advertised-ref scan

The read-only command `python3 -I local/validation/orch-20261027/e845/scan_advertised_refs_e845.py` exited 0. It queried 21 advertised refs (9 branches, 12 PR heads, 0 tags), 15 unique tips, and parsed 2,457 reachable commits. Claude/Anthropic author/committer identity and credit-line matches: **0**.

- Scanner SHA-256: `0d3f57a1609dfdff91083550557c5669fa601527c5651c691f3ae796f668ddd2`.
- Report: `local/validation/orch-20261027/e845/remote-attribution-scan-e845.json`; SHA-256 `5c120b9bb950beadb8d9a0fe12544e8902b3e292d4ef3098b4ab7caa796c388d`.
- The same read-only scan found 21 stash-reflog tips, 826 reachable commits, and 70 Claude/Anthropic matches. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; no stash ref or reflog was modified, as the user chose.
- The live-ref census covers only refs advertised by GitHub at scan time. Unreachable objects outside those refs and the examined stash reflog are not covered.

## E844 work and limits

E844 is documentation-only. Its independent static-profile replays match, but the REA/Ghidra function query for `0xb11290` timed out and no REA function dossier was produced. E844 remains partial; no native-port source, startup registration, runtime behavior, boot/audio/save, Metal, or game acceptance gate changed. The underlying profile and limitations are recorded in [the E844 partial checkpoint](2026-10-27-re02-b11290-partial-e844.md).

## E845 repository checks

- `cmake -S native -B local/build/orch-r22` — exit 0; log `local/validation/orch-20261027/e845/cmake-configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0; log `local/validation/orch-20261027/e845/cmake-build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431 passed; log `local/validation/orch-20261027/e845/ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed / 0 skipped / 0 failed; log `local/validation/orch-20261027/e845/lift-checkers.log`.
- `trail build --check` — exit 0; 9,877-node wiring graph in sync; deep layer not built; log `local/validation/orch-20261027/e845/trail.log`.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — exit 0; 31,968 folders indexed/current. Logs `local/validation/orch-20261027/e845/project-index-generate.log` and `project-index-check.log`. An earlier check reported `Stale indexes: local/indexes/folders.json`; its result is retained in `project-index-check-initial-stale.log`, then the index was regenerated and the final check passed.
- E845's six staged documentation paths passed the staged audit (6/6) and the full tracked-path audit passed 3,062/3,062; logs `local/validation/orch-20261027/e845/audit-staged.log` and `audit-all.log`.
- `git diff --check` and `git diff --cached --check` passed; logs `diff-check.log` and `staged-diff-check.log`.
- Before creating the E845 commit, `python3 -I scripts/check_commit_attribution.py --identity` and `--range HEAD^..HEAD` both passed; logs `precommit-identity.log` and `precommit-attribution.log`. The latter checks the already-published E844 commit. The exact E845 commit range and push/fetch result are verified after the commit and retained in private logs.
