# Startup slot 309 manager creation order

## 2026-10-27 13:09 CDT (`/root`)

- **Task:** `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` — characterize the first remaining startup gap at constructor index 309 / RVA `0xa505e0`.
- **Starting state:** clean `main`; `HEAD == origin/main == 1f5783687e3d07dacca9043f767816649b4b5427`.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint.
- **Result:** static order evidence places the known normal-entry manager-creation path after the full constructor-list call. It does **not** exclude alternate vtable dispatch before slot 309. Preserve both registration branches, leave slot 309 unregistered, and keep G1 PARTIAL.

## Observations

- The pinned ELF is SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; its ELF64 little-endian header has `e_entry = 0xf120`. The constructor-list table has 573 entries at `0x2dd70e0`; the parser places slot 309 / `0xa505e0` at table address `0x2dd6738` and slot 567 / `0x235d0` at `0x2dd5f28`.
- Ghidra decompilation of `FUN_00000020` shows the custom list is walked downward from `0x2dd70e0` until the `-1` sentinel. With entries stored at decreasing addresses, invocation proceeds in ascending constructor-index order.
- The ELF entry function `FUN_0000f120` calls `FUN_00000020` at `0xf14d` and then `FUN_00536730` at `0xf15a`.
- The saved-project reference census finds one code caller of `0x536730` (`0xf15a`) and one code caller of `0x536aa0` (`0x536775`). `0x536aa0` sets the constructed object's vptr to `0x15c8cd8`; `0x536730` dispatches through vptr `+0x10` and `+0x18` only after that setup.
- Vtable cell `0x15c8ce8` (`0x15c8cd8 + 0x10`) contains `0xb11340`. That wrapper tail-jumps to `0x255110`; `0x255110` calls `0x242c0`; `0x242c0` writes the manager pointer to `0x2dfcbc8` at `0x243f5`.
- Ghidra reports 64 references to `0x2dfcbc8`, with three `WRITE` references: reset at `0x23612`, cleanup at `0x241e1`, and manager store at `0x243f5`.
- Other vtable data references point to `0x242c0` at `0x1594ed8`, `0x255110` at `0x159c588`, and `0xb11340` at `0x15d5b90`. Their indirect consumers and ordering remain unresolved.

## Inference boundary

The observed normal-entry path cannot set the manager cell before constructor index 309 because its sole direct setup caller runs after `FUN_00000020`. The alternate vtable/data-driven paths mean this does not prove that the manager cell is null when slot 309 reaches `0x20d60`. `[RBP-0x38]` ownership/lifetime is also still unknown, so no native-lift boundary is supported.

This is **STATIC_ANALYSIS_VERIFIED** for the addresses, table order, references, and known normal-entry call sequence only. No game code was executed. No boot, game audio, save/reload, or gameplay Metal gate advances.

## Repository checks for publication

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0, no work to do.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: exit 0, **431/431** passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: exit 0, **516 passed / 0 skipped / 0 failed**.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0, **3,033/3,033** paths passed. Staged `python3 -I scripts/audit_publish_paths.py`: exit 0, **6/6** paths passed.
- `python3 -I scripts/check_commit_attribution.py`: exit 0 for the current empty `origin/main..HEAD` range; exact new-commit range check follows commit creation.
- `trail build --check`: exit 0; the 9,876-node wiring graph is in sync. Trail reports its deep meaning layer is not built.
- `python3 scripts/project_index.py`: exit 0, **33,162 folders** indexed. A strict `--check` reports `docs/FOLDERS.md` and private `local/indexes/folders.json` stale after generated/build inventory churn; only the durable `docs/checkpoints` count was retained in `docs/FOLDERS.md`.

Gate logs are under `local/validation/orch-20261027/slot309-docs/`. These repository checks do not advance a game acceptance gate.

## Private evidence and failed tooling attempts

The private report is `local/validation/orch-20261027/slot309/manager-order/REPORT.md`, SHA-256 `b96e91f04ecf1e08e90237c41862691e379a8421fd70d7332de21e7b7ee50d77`. It records the pinned binary identity, Ghidra 12.1.4/OpenJDK 25 environment, exact commands, report hashes, observations, and limits. The constructor manifest is SHA-256 `2da1b0a88434574076443564dcd4cd2c1166a19dfeb0a94bd708eec3049dbdf0`; key Ghidra output hashes are in the private report.

Two initial `ExportRefs.java` runs had Ghidra API type-signature errors (`getReferencesTo` returns an iterator; `getReferencesFrom` returns an array). The script was corrected and rerun with fresh logs; the successful saved-project query exited 0. These were tooling failures, not failed semantic hypotheses. No game execution or tracked binary modification occurred.

## Remaining risks and next action

- Keep `0x2dfd4b8` list/tree branches unresolved and slot 309 unregistered.
- Continue a focused read-only census of the alternate vtable references at `0x1594ed8`, `0x159c588`, and `0x15d5b90`, and prove whether any can dispatch before index 309. In parallel, continue `[RBP-0x38]` payload ownership/lifetime analysis.
- Task status remains **ACTIVE**, failed semantic hypotheses: **0**, G1: **PARTIAL**. Other acceptance gates remain unchanged.

## Publication and post-push verification

- E809 commit `47ea5711a50b4e68046c3ee642ebcda5db65a1cb` has parent `1f5783687e3d07dacca9043f767816649b4b5427`; author and committer are Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- Exact-range attribution for `1f5783687e3d07dacca9043f767816649b4b5427..HEAD` and effective owner identity passed. The committed-tree path audit passed **3,033/3,033**, and `git diff-tree --check HEAD^ HEAD` passed.
- The commit was pushed as a fast-forward to `origin main`. `git fetch origin main` confirmed `HEAD == origin/main == FETCH_HEAD == 47ea5711a50b4e68046c3ee642ebcda5db65a1cb`; the worktree was clean.
- The earlier precommit staged audit passed **6/6**. A post-commit staged audit reported `nothing staged`, as expected after the commit.
- This commit adds one owner-identified commit to the prior advertised-ref census of 2,423 commits with zero Claude/Anthropic matches. The new commit was checked separately by exact range; the 70 stash-only credit matches remain unchanged by owner choice.

## E810 documentation-closeout checks

- CMake configure exited 0; CMake build exited 0 with no work; CTest passed **431/431**; lift checkers passed **516/0/0**.
- Final staged/full path audits passed **5/5** and **3,033/3,033**. The precommit attribution audit passed for the current empty `origin/main..HEAD` range.
- `trail build --check` exited 0 with the 9,876-node wiring graph in sync; the deep layer remains unbuilt.
- `python3 scripts/project_index.py` exited 0 at **33,162 folders**. Strict check reports generated `docs/FOLDERS.md` and private `local/indexes/folders.json` stale after launcher build/private evidence inventory churn; only the durable checkpoint count is retained in `docs/FOLDERS.md`.
- Logs: `local/validation/orch-20261027/slot309-docs/e810-*`. These checks validate repository state only and do not advance a game gate.

## E811 publication closeout

- E810 commit `3c6846bed53b67becd5031bbd560c418b078eae0` has parent `47ea5711a50b4e68046c3ee642ebcda5db65a1cb`; author and committer are Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- `git push origin main` and `git fetch origin main` exited 0; fetch confirmed `HEAD == origin/main == FETCH_HEAD == 3c6846bed53b67becd5031bbd560c418b078eae0` and clean status.
- E810 exact-range attribution, committed-tree audit (**3,033/3,033**), diff check, and precommit staged audit (**5/5**) passed. E811 closeout check outcomes are recorded in the current handoff section.
- The stash-only Claude credit matches remain untouched per owner choice. E810 advances no native implementation or game acceptance gate.

### E811 closeout gate results

- CMake configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed **516/0/0**.
- `trail build --check` exited 0 with the 9,876-node wiring graph in sync; deep layer remains unbuilt.
- `python3 scripts/project_index.py` exited 0 at **33,162 folders**. Strict check exits 1 with transient `docs/FOLDERS.md` and private `local/indexes/folders.json` stale after `.build` churn; the generated `docs/FOLDERS.md` diff was restored.
- Final staged/full publish path audits passed **5/5** and **3,033/3,033**; staged diff check passed; precommit attribution passed for the current empty `origin/main..HEAD` range.
- The supplemental full-history `check_commit_attribution.py --all` audit reports legacy commit `f6604a86e60e` with author and committer `orchestrator <orchestrator@local>`. It is a generic assistant/agent identity, not Claude/Anthropic credit; current effective identity and the E811 exact commit range pass.
- Exact logs: `local/validation/orch-20261027/slot309-docs/e811-*`. These repository checks do not advance G1 or another game acceptance gate.
