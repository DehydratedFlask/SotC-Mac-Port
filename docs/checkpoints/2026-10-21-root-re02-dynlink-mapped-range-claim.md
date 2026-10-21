# Checkpoint: RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01 — claim

- Local date/time: 2026-10-21 11:31 CDT.
- Agent ID: `/root` (integrator); `launcher_review` is assigned after publication.
- Task ID: `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0. No test has run for this task.
- Files changed by this claim: `docs/TASKS.md`, `HANDOFF.md`, and this checkpoint. Implementation scope is only `scripts/test_ps4_dynlink.py`.
- Graft command from independent scout: `trail grep "mapped" --in scripts/`. It located `Image.mapped()` in `scripts/ps4_dynlink.py` and showed one existing direct assertion for a BSS-tail range. Live verification confirms `mapped()` requires the whole range to fit within one load segment's `memsz`; the two-segment fixture already tests `Image.read()` crossing a gap, but not `Image.mapped()`.
- Dependencies: `RE-02-DYNLINK-01` and `RE-02-DYNLINK-SEGMENT-SPAN-01` are `FIXTURE_VERIFIED`. No matching task ID existed before this claim.
- Scope: add synthetic assertions that a contained range is mapped, while a range crossing the first segment boundary and a nonzero range beginning in the gap are not mapped. Leave zero-length and adjacent-segment semantics unspecified. No production code changes.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Observations versus inference: the test will pin `Image.mapped()`'s single-segment range containment for this fixture. It does not establish live ELF mappings, relocation behavior, loader correctness, or game execution.
- Log/evidence paths: `local/validation/re02-dynlink-mapped-range/{attempt-1.log,root-replay.log}`.
- Remaining risks: no ELF, dynamic-linker, runtime, or game behavior claim.
- Next exact action: after this claim is on `origin/main`, assign only `scripts/test_ps4_dynlink.py` to `launcher_review`; root will independently replay and publish results.
