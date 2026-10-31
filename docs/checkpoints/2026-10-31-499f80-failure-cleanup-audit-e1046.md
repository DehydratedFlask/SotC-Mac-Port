# `0x499f80` failure-cleanup audit — E1046

- **Local time / agent:** 2026-10-31 01:10 CDT, `/root` integrating `/root/driver_gap_review`
- **Task:** `CPU-01-499F80-FAILURE-CLEANUP-AUDIT-01`
- **Status:** `COMPLETE / STATIC_ANALYSIS_VERIFIED`; failed semantic hypotheses: 0.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and this checkpoint. The worker made no edits and ran no tests.

## Pinned body and host-model results

**Observed in the pinned evidence:** `0x499f80` is 193 B and has no null/error/status branches after allocator and CritSec calls. It performs the argument fallback and `store_cell` write before allocations; allocates the object, initializes CritSec, allocates two 0xa00 B buffers, then conditionally releases the previous object in the order: old+0x28 buffer (tag 2), old+0x08 buffer (tag 2), CritSec destroy, object free (tag 1), and finally writes the replacement cell. The imported helper success/error semantics are not established by these bytes.

**Observed in the native model:**

- `native/core/init_531d60_post_base_499f80.cpp:16–20` returns a failed body result without cleanup.
- `native/core/init_499f80_body.cpp:29–39` can return after object allocation with the object still live; `store_cell` was already written.
- `native/core/init_499f80_body.cpp:42–53` can return after object allocation and CritSec initialization, with both still live.
- `native/core/init_499f80_body.cpp:61–64` can return with object, CritSec, and first buffer live.
- The existing success/replacement fixture at `native/tests/test_init_499f80_body.cpp:74–81` does not inject failures. `tools/lift/check_init_499f80_body.py:18–37` checks bytes and wiring, not cleanup behavior.

**Inference:** `AllocDispatchContract` supports attempted tag-1/tag-2 frees and the CritSec hook supports attempted destroy. Successful rollback could be added as a host policy, but release failure ownership is not represented, and destroying/freeing the new object also raises the stored-cell ownership question. The current old-object replacement path discards destroy/free status; if CritSec destroy rejects a busy handle, freeing its containing object can lose the retained sidecar token.

## Decision and next action

No native code change is justified by the retail body: it contains no error branches whose cleanup behavior can be recovered. A separate host-only rollback task is tracked as `CPU-01-499F80-HOST-FAILURE-ROLLBACK-01`; implementation is waiting on the owner's preference between rollback with cleanup-result reporting and explicit partial ownership. The failure-path status remains `PARTIAL`; no boot/runtime or product-gate claim changes.
