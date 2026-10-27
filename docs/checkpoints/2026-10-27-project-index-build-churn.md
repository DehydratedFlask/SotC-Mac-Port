# Project index and Swift build churn

- **Date:** 2026-10-27
- **Task:** `DOC-INDEX-LAUNCHER-BUILD-CHURN-01`
- **Starting commit:** `d72870a319d58710152e93144f512a56cd0b20c7` on `main`, matching `origin/main`.
- **Files:** `scripts/project_index.py`, `scripts/test_project_tools.py`, `docs/FOLDERS.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `HANDOFF.md`, and this checkpoint. Private validation output is under `local/validation/orch-20261027/project-index-fix/`.

## Change

The recursive project index included the ignored Swift build tree `launcher/.build`, so ordinary Xcode/Swift cache changes invalidated the public folder summary. The inventory now prunes the exact relative subtree `launcher/.build`. Other directories named `.build` remain visible and new project folders still invalidate both generated indexes.

The new `test_index_ignores_only_launcher_build_output` fixture creates nested transient files under `launcher/.build`, verifies both indexes remain current, then verifies `other/.build` and `new-module` each make `--check` fail. The existing fixture also confirms the regenerated index retains a legitimate new folder.

## Verification

- `python3 -I scripts/test_project_tools.py`: **13/13** passed.
- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0 (`ninja: no work to do`).
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **431/431** passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **516 passed / 0 skipped / 0 failed**.
- `trail build --check` first reported the two edited script files stale. `trail build` exited 0; the subsequent `trail build --check` passed with **9,877 nodes / 13,470 edges**. The ignored Graft cache was regenerated locally and is not part of the commit; its deep meaning layer remains unbuilt.
- `python3 scripts/project_index.py` and `python3 -I scripts/project_index.py --check`: both exit 0 after final local inventory stabilization; **31,921 folders**.
- Staged `python3 -I scripts/audit_publish_paths.py`: **8/8**; full `python3 -I scripts/audit_publish_paths.py --all`: **3,034/3,034**.
- `python3 -I scripts/check_commit_attribution.py`, `--identity`, `git diff --cached --check`, and the strict project-index check: exit 0 before publication. A fresh live GitHub census re-parsed **2,427 commits** across 9 branch refs and 12 PR refs (15 distinct tips, 0 tags): zero Claude/Anthropic author/committer identities and zero coauthor/generated-by credit lines. `main` equals remote `main`. Private stashes were not inspected or modified.
- Exact gate logs are in `local/validation/orch-20261027/project-index-fix/`. The remote audit script SHA-256 is `9fcdc3263ccc0d5f6a1ac166f23a2e4476c1378bbd5f1cd2235e1e713b3426c0`; its output SHA-256 is `8734dd29feca52fef46e3796525899cd9273ed54b6a7b3ab25f8866c8e69c3b2`. CTest log SHA-256 `9d0ce45998ca3fc02b7809c6174e133230f8b0c3a4187f50e0b7f3da421b06b5`; lift log SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db`.

## Limits and handoff

This is an index-stability fix with synthetic filesystem coverage. No game code ran, and no boot, audio, save/load, or gameplay Metal gate advances. CPU-01 remains G1 PARTIAL; slot 309 remains unregistered. Next: finish staged/full path audits, commit under the owner identity, push `origin main`, fetch, and verify the published tree and commit attribution.
