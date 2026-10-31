# `0x499f80` owner policy and rollback handoff — E1055

- **Local time / agent:** 2026-10-31 11:15 CDT, `/root` integrating read-only review by `/root/rollback_handoff_review`
- **Task:** `CPU-01-499F80-HOST-FAILURE-ROLLBACK-01`
- **Status:** `READY`; owner-policy dependency resolved, implementation unclaimed; failed tries: 0.
- **Files:** documentation only (`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint, generated `docs/FOLDERS.md`). No source or test files changed.

## Owner decisions

The owner selected **pinned operation leases** for CritSec lifetime. E1044 already implements stable sidecar mutex storage with per-operation leases and does not hold the registry lock across blocking pthread operations. This choice does not prove the PS4 mutex ABI or type-2 mapping.

The owner selected **host rollback with cleanup-result reporting** for `0x499f80` allocation/init failures. This is a host safety policy only. E1046's pinned retail-body audit found no failure/status branches from which to recover rollback semantics.

## Live baseline and review

- `git fetch origin && git status --short --branch && git rev-parse HEAD origin/main FETCH_HEAD` exited 0. The checkout was clean on `main`; `HEAD`, `origin/main`, and `FETCH_HEAD` were all `6aa93e8243932fe76bb0d9e737891d86b9d07a34`.
- `graft callers run_init_499f80_body --depth all` exited 0, but Graft reports two same-name definitions and drops ambiguous cross-file edges. Its “no indexed callers” result is not proof of no callsites.
- Read-only review by `/root/rollback_handoff_review` found the current default `critsec_init_at` hook is failure-atomic: it writes the token after sidecar registration and destroys an initialized pthread mutex on its failure paths. Keep that guarantee explicit in the hook contract; arbitrary test/host hooks must not be assumed failure-atomic without asserting it.
- E1046 records existing gaps: candidate allocation/init failures leak already acquired state; replacement cleanup discards release results; an `EBUSY` CritSec destroy followed by freeing the containing object can lose the sidecar token.

## Implementation contract for the next owner

1. Build a new object as a candidate. Restore the prior `store_cell` value if candidate creation fails as an explicit host-only transactional choice. Keep `old_object_cell` unchanged until replacement completes.
2. Roll back candidate resources in reverse acquisition order. Buffer frees use tag 2; object free uses tag 1. Capture each cleanup result and retain the candidate plus all still-owned pointers when any cleanup fails.
3. Never free the object's handle storage after CritSec destroy fails (including `EBUSY`). Clear successfully released pointer fields and track already-destroyed resources so a caller retry cannot double-free or destroy twice.
4. Handle old-object replacement cleanup failures explicitly. Keep the old object reachable and report cleanup state; if replacement cannot complete, clean up the candidate under the same rollback contract. State any host-specific cleanup ordering.
5. Add failure injection for object allocation, CritSec initialization, each buffer allocation, each buffer free, CritSec destroy/`EBUSY`, and old-object cleanup. Assert result status, resource ownership, `store_cell` and `old_object_cell`, allocator tags, retained pointers, and retry safety.

The exact implementation can change source/result structs and `native/tests/test_init_499f80_body.cpp`; it must not wire new retail semantics into the app-main registry, advance `sotc_core_start()`, or claim boot/audio/save/Metal behavior. Keep the parent body task PARTIAL and game gates unchanged.

## Commands and result

- `git fetch origin && git status --short --branch && git rev-parse HEAD origin/main FETCH_HEAD` — exit 0; clean and fetch-verified baseline as above.
- `graft callers run_init_499f80_body --depth all` — exit 0; ambiguous same-name graph entry, so no caller conclusion.
- No implementation tests or game inputs were used for this checkpoint. Documentation/index checks and publication checks are recorded in the matching E1055 `HANDOFF.md` result.

## Next exact action

Take `CPU-01-499F80-HOST-FAILURE-ROLLBACK-01`, inspect `native/core/init_499f80_body.{hpp,cpp}`, its post-base wrapper, allocator/CritSec hooks, and existing fixture; implement the host-only ownership/result contract above; add failure-injection fixture cases and update its byte checker only if source-structure assertions require it. Run the focused test/checker, then the full repository publication validation. Failed tries remain 0.
