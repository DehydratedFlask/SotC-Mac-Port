# Slot 306 category variables (CPU-01-SLOT306-CATEGORY-VARS-01)

Date: 2026-10-26. Orchestrator: GLM-5.3 `/root` (the background-subagent lane for the handlers had
died to transport errors, so this lane was also implemented directly). Static model only: no
original code was executed, loaded or relocated.

## Contract source (private listing, re-verified against the pinned ELF this run)

The `sndxCategory_*` block is built by the 0xa5d990 loop inside engine init 0xa5cd90 (body
`[0xa5cd90,0xa5e41f)`, SHA-256 `460f5e5226e6e090e8af001f2df159bfd55dd60b2d4e01b0f59a279eb0686e92`,
re-read and matched). One loop, i = 0..31 (`cmpq $0x20`), two `ConCmdBoolVar` appends per
iteration: Mute ("sndxCategory_Mute_%d" via the variadic bpe::String::Format 0x10a90, format
{0x12c4fe6, 20}) and Filter ("sndxCategory_Filter_%d", {0x12c4ffb, 22}), into the engine vectors
+0x8bd0/{+0x08 end}/{+0x10 cap} and +0x72a8/+0x72b0/+0x72b8, stride 0xc8. Both: value false,
default false, flags 1, empty-literal description {0x12c4d1a, 0}, functor tag 0x15d2060 — the
same inline shape as slot 306's static bools — registered via 0x20d60, **no per-object atexit**
(the vectors own the vars; the engine teardown destroys them). Growth goes through 0xa62bc0 (which
also constructs the element in the original; its body is NOT lifted). For every non-null,
non-empty `category_names[i]` the loop also formats "%d - %s" ({0x12c5012, 7}) and builds TWO stack
menu toggles via 0x26ea0 (parents "Category Mute" {0x12c4d2f} / "Category Filters" {0x12c4ddd},
targets the two fresh vars, flags 0x30), then destroys the stack temporaries with DIRECT 0x272d0
calls. A "Unnamed Categories" ({0x12c501a}) block at 0xa5ddec is gated by a stack flag the visible
body never sets nonzero — recorded as an open conditional, not modelled.

## Deliverables (all green)

- `native/core/engine_category_vars.{hpp,cpp}`: `build_sndx_category_vars` (32 Mute + 32 Filter
  appends with formatted names built through the 0xf1a0 SharedStr model — owned names, acquired at
  construction, stack reference released through the real 0xf590 — and registered in the observed
  interleaved order), `sndx_category_vectors_from_engine` (reads the two spans at the pinned
  offsets), `sndx_format_category_name`, and explicit hooks: name allocator (0xf1a0), name
  deallocator (0xf590), vector growth (0xa62bc0 reallocation stand-in), per-index menu toggle.
  Growth relocates: the module detaches this build's contiguous pending-list block before the
  hook, then re-creates the observable end state after it (each moved slot's +0x60 value_ref
  rebased to its own +0xc0; the block relinked in the exact construction order; the failure path
  restores the links). The original's exact relocation mechanics inside 0xa62bc0 remain UNKNOWN —
  the model documents that only the end state is reproduced. Entry spans must be empty (named
  `span_not_empty` refusal otherwise).
- `native/core/concmd_vars.{hpp,cpp}`: additive sibling `init_bool_var_vector` — the same
  construction as `init_bool_var_static` minus the atexit record, with the owned-name release
  contract (borrowed names release as inert views; owned names require a `SharedStrDeallocatorF590`).
  `sndx_command_handlers.hpp`: `SndxCategoryVectorSpan` gained an additive `cap` member + `capacity()`
  (the two modules now share one span type).
- `native/tests/test_sndx_category_vars.cpp`: name formatting (boundary indices), engine-offset
  extraction, a full build (counts, per-slot shape incl. owned-name refcount 1, interleaved
  pending-list order, zero atexit records, menu hook for enabled names only, stable-pointer
  assertions), a moving-growth case (doubling realloc: 8 growths, every slot re-verified after the
  moves, list intact), and fail-closed cases (hooks missing, span not empty, first/later grow
  refusals with exact partial counts, name allocation failure). Passes 1/1 (ASan+UBSan build);
  an ASan heap-buffer-overflow in the first draft was a TEST-hook bug (stale `end` after realloc)
  and a real MODEL gap (grow-failure left the pending block detached) — both fixed and covered.
- `tools/lift/check_sndx_category_vars.py`: ELF-tie checker — engine-init body hash, 20 exact
  instruction-byte pins with decoded call targets/lea targets/displacements (incl. the SIB-shifted
  filter disp32) and the 0xc8 stride immediates, 7 string pins, the module + shared constants, the
  vector-owned sibling, and the build files. Exit 0; a tampered pin exits 1.

## Gates

`cmake --build local/build/orch-r22 -j 12` exit 0; full CTest **403/403**
(`local/validation/orch-20261026/ctest-final.log`); lift checkers **488/0/0**
(`local/validation/orch-20261026/lift-checkers-final.log`). Publish-path/attribution audits and
the project index run before any publication (see HANDOFF).

## Status

**CPU-01-SLOT306-CATEGORY-VARS-01: FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED.** 0 failed
hypotheses (the two fixed defects above were caught by the test suite pre-acceptance; compile
slips excluded per repo convention). Open, named: the 0xa62bc0 growth policy and its relocation
mechanics, the "Unnamed Categories" conditional block, the menu-toggle construction itself (hook
with byte facts; the menu path is the debug-menu modules' scope), and the engine teardown that
eventually frees these names. No runtime/G1 claim; G1 remains PARTIAL.
