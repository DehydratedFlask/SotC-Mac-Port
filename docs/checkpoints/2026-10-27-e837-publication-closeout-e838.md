# E838 — E837 publication and attribution closeout

**Date:** 2026-10-27 16:50 CDT
**Agent:** `/root` (integrator)
**Task ID:** `PUB-01-CLAUDE-CREDIT-CLEANUP-01` and E837 checkpoint publication
**Status:** `ARTIFACT_VERIFIED` for advertised GitHub refs
**Attempts:** one publication closeout; no product-analysis tries

## E837 publication

- Commit: `dce99981d73824d557d6c030179ab9f5ebc56876`, parent `c84312ddb635bfa05a9922349f6779117b5dbbf8`.
- Subject: `docs: close out E836 publication and attribution audit`.
- Author and committer: Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- `git push origin main` advanced `main` from `c84312dd` to `dce99981`; fetch verified `HEAD == origin/main == dce99981d73824d557d6c030179ab9f5ebc56876`, clean.
- Exact commit-range attribution audit passed; full tracked-path audit passed **3,054/3,054**. Logs: `local/validation/orch-20261027/e837/post-commit-attribution.log`, `audit-all.log`, `push.log`, `fetch.log`.

## Live advertised-ref census after E837

The adapted read-only scanner enumerated **21 advertised refs** (9 branches, 12 PR heads, 0 tags), **15 unique tips**, and **2,450 reachable commit objects**. It found **zero** Claude/Anthropic author/committer identities and zero matching credit lines. This covers the current advertised GitHub refs; unreachable objects outside those refs are not included.

The separate stash-reflog scan enumerated 21 tips and 826 reachable commits, including 70 Claude/Anthropic attribution matches. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; no stash ref or reflog was modified, per the user's explicit choice.

- Scanner: `local/validation/orch-20261027/e837/scan_advertised_refs_e837.py`, SHA-256 `e94109586bb10d72583b4df756124ba3fc7683b16898cbe2f345f8abef134135`.
- Report: `local/validation/orch-20261027/e837/remote-attribution-scan-e837.json`, SHA-256 `1856ace3ca33f94182b0b976c8acfc8fcd7043f34afb11c86f761e3dc74c2b39`.
- Summary log: `local/validation/orch-20261027/e837/post-push-scan.log`.

## E837 repository gates and corrections

E837 prepublication gates passed: CMake configure/build exit 0, CTest **431/431**, lift **516/0/0**, Trail **9,877 nodes / 13,470 edges** in sync, project index **31,950** current, staged/full path audits **6/6** and **3,054/3,054**. Exact E837 commit-range attribution passed. Logs are under `local/validation/orch-20261027/e837/`.

The checkpoint recorded the final project-index count and post-commit attribution after those checks; this closeout corrects its temporary “pending” wording. The earlier E836 shell-wrapper and copied-scanner output-path corrections are documented in the E837 checkpoint; neither changed product code or private stash state.

## E838 closeout repository verification

The standard repository gates for this closeout passed:

| Command | Result | Log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 | `local/validation/orch-20261027/e838/cmake-configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0, no work to do | `local/validation/orch-20261027/e838/cmake-build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **431/431 passed** | `local/validation/orch-20261027/e838/ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **516 passed, 0 skipped, 0 failed** | `local/validation/orch-20261027/e838/lift.log` |
| `trail build --check` | **9,877 nodes / 13,470 edges**, in sync; deep layer not built | `local/validation/orch-20261027/e838/trail.log` |
| `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` | expected **31,951 folders**; final current result to be recorded after final docs edit | `local/validation/orch-20261027/e838/project-index-{generate,check}.log` |
| `python3 -I scripts/audit_publish_paths.py` and `--all` | **7/7 staged** and **3,055/3,055 tracked** paths passed | `local/validation/orch-20261027/e838/audit-{staged,all}.log` |
| `python3 -I scripts/check_commit_attribution.py` | precommit empty range passed; repeat after E838 commit and before push | `local/validation/orch-20261027/e838/post-commit-attribution.log` |

These checks cover repository build and fixtures only. No full-port or game boot/audio/save claim is implied.

## Files and next action

E838 changes `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No source, game binaries, game inputs, or private stash data are staged. Public advertised refs are attribution-clean through E837; the stash remains user-excluded. Continue only when an external task dependency or input changes.
