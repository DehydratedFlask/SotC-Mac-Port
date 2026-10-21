# Checkpoint: RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01 — claim

- Local date/time: 2026-10-21 11:48 CDT.
- Agent ID: `/root` (integrator); `launcher_review` is assigned after claim publication.
- Task ID: `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01`.
- Status: `ACTIVE`; failed tries: 0. No test has run for this task.
- Files changed by this claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. Implementation scope is only `scripts/test_ps4_dynlink.py`.
- Graft search: `trail grep "mapped" --in scripts/` found `Image.mapped()` at `scripts/ps4_dynlink.py:64-65` and existing mapped-range tests; no test checks a zero-length range at a segment endpoint.
- Live implementation: `Image.mapped()` returns true when `p['vaddr'] <= vaddr` and `vaddr + size <= p['vaddr'] + p['memsz']`. For the first synthetic segment in `two_segment_image()`, the end address is `0x1010`; a zero-length range at that address is accepted by these live inequalities. Existing tests cover a nonzero range starting there but not the zero-length case.
- Dependencies: `RE-02-DYNLINK-01`, `RE-02-DYNLINK-SEGMENT-SPAN-01`, and `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01` are `FIXTURE_VERIFIED`.
- Scope: add one synthetic assertion that `two_segment_image().mapped(0x1010, 0)` is true. Do not change production semantics or infer live ELF/loader behavior.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Observations versus inference: this pins the helper's current empty-range boundary behavior only; it does not say whether a real loader accepts an analogous request.
- Log/evidence paths: `local/validation/re02-dynlink-mapped-zero-length/{attempt-1.log,root-replay.log}`.
- Remaining risks: no live ELF, loader, relocation, runtime, or game behavior claim.
- Next exact action: after claim publication, assign only `scripts/test_ps4_dynlink.py` to `launcher_review`; root will replay the focused suite.
