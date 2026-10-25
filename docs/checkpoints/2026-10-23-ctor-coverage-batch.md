# Checkpoint — constructor coverage batch (308/573)

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; orchestrator verification
- Task: `CPU-01-CTOR-COVERAGE-BATCH-01`
- Gate: **FIXTURE_VERIFIED** for the registered table; **STATIC_ANALYSIS_VERIFIED** for the
  573-row original table

Later slice: E618 adds indices 296/297/300, moving the registered count to 311/573 and the first
gap to 298 (`0xb0e250`). The facts below describe this batch.

## What changed

- `recovered_constructors()` registers **308 of the original 573** entries (was 155): 152
  already-lifted `vec_*` / `leaf_*` modules were put on the production path and the one missing
  17-byte leaf was lifted.
- `native/core/ctor_original_table.{hpp,cpp}` carries the full 573-row `{index, rva}` table (the
  original table is stored backwards: index `i` is the qword at `0x2dd70e0 - 8*i`, sha256
  `5917cb85…3ab28`) plus `first_unrecovered_constructor_index()` /
  `first_unrecovered_constructor_rva()`.
- The remaining gap is explicit and checked: **265 unregistered entries, all `other` class,
  first index 296 / RVA `0xb2b6b0`**. Index 296 is a 12-byte tail jump into `0x323d0`; its analysis
  is the next startup-cliff task (`CPU-01-STARTUP-CLIFF-296-01`).

## The new leaf (index 95)

`0xd71f60..0xd71f71`, 17 B, sha256 `80d38118…7c960`: `vmovups xmm0,[0x1305650]`,
`vmovups [0x33529c0],xmm0`, `ret`. The 16-byte constant is `{~0, ~0, ~0, 0}`. The module follows
the sibling `vec_*` shape (`kVecD71f60Bits`, `VecD71f60`, `init_vec_d71f60`).

## Two runtime-input leaves

`leaf_575d40` (index 417) reads a 20-byte BSS cell `0x2eea480` whose producer is not recovered;
`leaf_99350` (index 548) reads a 24-byte object at `0x2dd9fa8` and stores the image address
`0x2dd9f84`. Both native rows take documented zero-initialised inputs
(`StartupState::leaf_575d40_source`, `leaf_99350_inputs`); neither value is fabricated as recovered.

## Verification

- `test_startup_ctor_coverage`: the full recovery run completes with 308/308 callbacks, every row
  pairs with the pinned table, the gap is 296/`0xb2b6b0`, and 10 destination cells hold their
  module constants. `test_vec_d71f60`: 3 sections.
- `tools/lift/check_ctor_slots.py` now derives the rule instead of a hard-coded list: every
  RVA-named module must be registered with the pinned index, in order, once; the registered
  index-set sha256 is pinned (`34340400…75b4`); 573 rows are re-derived from the ELF.
- Mutation: swapping indices 200/201 fails the checker; dropping the index-200 row fails with
  “module RVA 0xc5c320 is not registered”. Restored and green.
- At the round tip: ctest 381/381, checker sweep with 0 failures, both publish audits exit 0.

## Limits

`run_ok=true` means every **registered** slot ran, not all 573. The 265 remaining entries are real
`other`-class bodies; the first is the tail-jump cliff above. No boot, audio, save or render claim.
