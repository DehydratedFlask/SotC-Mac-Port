# E848 — E847 publication closeout and live attribution census

**Date:** 2026-10-27 18:04 CDT

**Integrator:** `/root`

**Task IDs:** `PUB-01-CLAUDE-CREDIT-CLEANUP-01`; `RE-02-APPMAIN-INIT-B11290-STRUCTURE-01`

**Status:** `ARTIFACT_VERIFIED` for currently advertised GitHub refs through E847. The bounded b11290 structural task remains `STATIC_ANALYSIS_VERIFIED`; broader RE-02, G1 and game acceptance remain incomplete. The user's choice to leave private stashes unchanged remains in force.

## E847 commit publication

- Commit `72188696fdea81edf7bb00bb2a3d3016bfbcf08f`, parent `9f79b9b84e508c10a42b97709bb92f668707238b`, subject `docs: verify b11290 structure and close E846 publication`.
- Author and committer: Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- The six allowlisted docs paths passed staged path audit 6/6 and the full tracked tree passed 3,064/3,064. Worktree/staged diff checks and owner identity passed. Postcommit exact-range attribution, full-tree path audit (3,064/3,064), and committed diff check passed.
- `git push origin main` and subsequent `git fetch origin main` exited 0; `HEAD`, `origin/main`, and `FETCH_HEAD` all equal `72188696fdea81edf7bb00bb2a3d3016bfbcf08f`; worktree is clean.
- Exact commands and logs are under `local/validation/orch-20261027/e847/`. The E847 staged-path audit was run before commit; the postcommit full-tree audit covers the committed result.

## Fresh live GitHub attribution census after E847

The read-only scanner `local/validation/orch-20261027/e848/attribution/scan_advertised_refs_and_stash_e848.py` (SHA-256 `88fd6e3d1803aaba5f8c5f86b0f15c3a4abe14f6bb872247d46ee6dcf3f3642e`) queried 21 currently advertised refs (9 branches, 12 pull-request heads, 0 tags), 15 unique tips, and **2,460 reachable commits**. Claude/Anthropic author/committer identity and credit-line matches: **0**. Report SHA-256 `d914e2fd3da1c65271be3f7136f5677f4ea4264cf1b7d32a204941d3c5189cde`; scanner log SHA-256 `cd2966f024604e4c915f15c3a8ae5ea27a2e11d5325b16d348e323d867c17f48`.

The same read-only scan found 21 stash-reflog tips, 826 reachable commits, and 70 matches. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; no stash, remote ref, or reflog was rewritten. This census excludes unreachable objects outside the advertised refs and examined stash reflog.

## E847 bounded structural task

E847's resource-adjusted REA query for RVA `0xb11290` completed against pinned ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; REA Evidence ID `ev_7302c80935df3ad73bd81c2714026ba3264a2037d6445677300056fed8bc5eae`. The Ghidra 12.1.4 range is `0xb11290..0xb1133c` inclusive (173 bytes), and a root byte-profile replay matches all 15 E843 profile fields: body SHA-256 `2a0aa64699f2b560f4f02fb35bb2f34f1e8d3555514833599aa51bef207e2e38`, 37 instructions, 16 machine calls, one terminal direct JMP, zero decoded indirect transfers and RIP-relative references.

Preserve the provider limitation: Ghidra's reference manager labels 17 references `UNCONDITIONAL_CALL`, including the terminal JMP at `0xb11338` to thunk `0xf90d30`, and excludes fallthrough despite `hasNoReturn=false`. The cause is unknown; do not treat it as a 17th machine call or infer no-return/thunk semantics. “Complete” call-reference coverage does not include targetless computed or indirect flows; the independent byte-profile replay is the basis for the zero decoded indirect-transfer observation. Function semantics, native-liftability, initialization side effects and gameplay reachability remain unknown.

## E848 repository verification

- `cmake -S native -B local/build/orch-r22` — exit 0, 0.207 s; `e848-cmake-configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0, 0.030 s; Ninja reported no work; `e848-cmake-build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431 passed, 8.555 s; `e848-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed / 0 skipped / 0 failed, 95.410 s; `e848-lift-checkers.log`.
- `trail build --check` — exit 0; wiring graph in sync at 9,877 nodes; deep layer remains unbuilt; `e848-trail.log`.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — both exit 0; 31,974 folders indexed/current; `e848-project-index-generate.log` and `e848-project-index-check.log`.
- Six staged docs paths pass the publication-path audit (6/6); the full tracked tree passes (3,065/3,065). Worktree/staged diff checks and owner identity pass; precommit `HEAD^..HEAD` attribution passes for E847. Precommit fetch confirms `HEAD == origin/main == FETCH_HEAD == 72188696fdea81edf7bb00bb2a3d3016bfbcf08f`. Logs are under `local/validation/orch-20261027/e848/` (`e848-audit-staged.log`, `e848-audit-all.log`, `e848-diff-check.log`, `e848-staged-diff-check.log`, `e848-identity.log`, `e848-precommit-attribution.log`, `e848-fetch-before.log`). E848 exact-range attribution, push and post-push fetch verification remain pending.

## Remaining product work

Read-only triage found no other product task in `READY`. VoiceOver requires an unlocked Mac; save acceptance requires a user-owned PS4 save; AUD-02 lacks a released query scope; GPU tasks await dependencies; slot 309 still requires a concrete nonzero producer or semantic consumer. No native source changed and no game acceptance gate advanced. The real-game target still requires a native ARM64 executable, direct Metal, cold boot, game audio, and real save/reload without prohibited runtime modules.

**Next exact action:** complete the E848 project index and staged/full audits, diff/identity and exact-range checks; commit the six allowlisted docs paths as Johnny Wang, push `origin main`, and fetch-verify. Resume product implementation only when a documented input or dependency makes an independent task READY.
