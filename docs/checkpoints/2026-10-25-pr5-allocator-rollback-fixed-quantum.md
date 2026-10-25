# PR #5 allocator bootstrap rollback and fixed quantum

- Date/time: 2026-10-25 03:10 CDT.
- Agent: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01`, with allocator child tasks
  `SRV-01-ALLOCATOR-BOOTSTRAP-01`, `SRV-01-ALLOCATOR-LAZY-LATCH-01`, and
  `SRV-01-HEAP-ALLOCATOR-01`.
- Branch: `cursor/heap-depth-5d26`; latest main base `bbee4c1fae5acd4940d164f445299f988b33ec1b`.
- User choices: bootstrap failure must roll back; remove the custom heap quantum; use harness-only
  normals and preserve private test evidence boundaries elsewhere in the active project.

## Changes and review

- `AllocatorFront` gained compare/exchange helpers for transactional slot assignment and rollback.
- `bootstrap_allocator_hierarchy` serializes concurrent helper callers, configures the three
  children, installs available child slots with rollback on failure, then publishes the complete
  composite in slot 1. Slot 1 remains unavailable during child-slot scans, preserving the observed
  empty-table results 1–4. The injected lazy hook documentation states that callbacks can be
  concurrent and must publish a complete hierarchy before returning success.
- `test_allocator_lazy_latch` forces two alloc callers to observe empty slot 1 and overlap in the
  hook. Both allocations succeed through a three-child hierarchy, and both tracked allocations
  return to zero after free. Existing two-free-slot failure test verifies rollback and retry.
- `HeapAllocator` no longer accepts a configurable page quantum. The fixed observed 1 MiB ceiling is
  covered at the limit and one byte above it. The app-main checker now treats E405's PR #5 absence
  statement as historical.
- Initial review of the rollback/fixed-quantum revision found a race: slot 1 was visible before all
  children were installed. The follow-up serializes helper callers and publishes slot 1 last. A
  second independent review found no blocker; it noted other custom lazy-hook callbacks remain
  responsible for their own synchronization.

## Commands and results

- `git merge --no-commit --no-ff origin/main` — clean merge; no unresolved paths. Merge commit
  `039417f831fda47b54455130b86f3b0c0b35c05c` created as Johnny Wang and pushed to
  `cursor/heap-depth-5d26`; GitHub PR base OID is `bbee4c1`. Follow-up `gh pr view 5` reports
  `MERGEABLE/CLEAN`, no checks, and no review decision. The PR remains DRAFT until final source/docs
  push and body refresh.
- `cmake -S native -B local/build/orch-r22` — PASS; log
  `local/validation/pr5-rollback-fixed-quantum-20261025/configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — PASS; log
  `local/validation/pr5-rollback-fixed-quantum-20261025/build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — PASS, 397 total; 396 passed,
  one private-input `metal_asset_frame` test skipped. Log
  `local/validation/pr5-rollback-fixed-quantum-20261025/ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — PASS, 481 passed,
  0 skipped, 0 failed. Log `local/validation/pr5-rollback-fixed-quantum-20261025/checkers.log`.
- `ctest --test-dir local/build/orch-r22 -R '^(allocator_bootstrap|heap_allocator|allocator_lazy_latch)$' --output-on-failure`
  — PASS, 3/3; log `local/validation/pr5-rollback-fixed-quantum-20261025/focused-allocator.log`.
  The additional `ctest --test-dir local/build/orch-r22 -R '^allocator_lazy_latch$' --repeat
  until-fail:25 --output-on-failure` — PASS, 25/25 repeats; log
  `local/validation/pr5-rollback-fixed-quantum-20261025/lazy-repeat.log`.
- `trail build --check` — PASS; wiring graph in sync, deep meaning layer not built.
- `trail build` — PASS; 2,001 indexed files, 8,482 nodes, 11,440 edges. `graft grep
  'set_heap_if_empty'` found six references across the implementation and API; a subsequent
  `trail build --check` passed.
- `python3 scripts/project_index.py` — 1,509 folders indexed in the isolated candidate. The generated
  `docs/FOLDERS.md` omitted private/ignored workspace trees; it was restored from the candidate's
  tracked index and the partial generated inventory was not published.
- `python3 -I scripts/audit_publish_paths.py` — PASS, 22 staged paths. `python3 -I
  scripts/audit_publish_paths.py --all` — PASS, 2,776 paths. Explicit unpublished-range attribution
  and effective identity checks — PASS. All commits created for this update use Johnny Wang identity;
  no Claude/Anthropic trailer is present.
- Candidate logs, including path audit, are under
  `local/validation/pr5-rollback-fixed-quantum-20261025/`.

## Status and limits

- PR #5 remains DRAFT and OPEN pending final push and description refresh; GitHub currently reports
  `MERGEABLE/CLEAN` against latest main.
- Allocator hierarchy is `FIXTURE_VERIFIED`; it is not wired into game startup. This does not claim
  game boot, audio, saving, asset-derived rendering, or G1 completion.
- One failed review iteration was corrected; the current implementation review and all listed checks
  pass. No dependency changed.
- Next action: verify PR #5 reports `MERGEABLE/CLEAN`, commit and push the staged fixes/docs, update
  the PR description with these latest results, and merge through GitHub.
