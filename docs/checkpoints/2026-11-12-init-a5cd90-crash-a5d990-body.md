# 2026-11-12 — Init-window crash fixed; `0xa5d990` body executed natively

**Task:** `CPU-01-INIT-A5D990-BODY-01` (plus the `0xa5cd90` tag-dereference repair)
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED for both bodies; callee closure **false**
**Gate:** G1 **PARTIAL** (unchanged); G0/G3/G4 PARTIAL; G2/G5/G6 NOT RUN

## What was actually broken

The working tree handed over from 2026-11-10 was **red**, and no record said so:

- `sotc_init_window` aborted with `AddressSanitizer: SEGV on unknown address 0x0000012c4f51`.
- `ctest` was **701/702**, the single failure `init_window_runtime` (Subprocess aborted).

Cause: the native `0xa5cd90` body passed the selected config-string **original-image RVA**
(`0x12c4f51` / `0x12c4efa`) to its `0x994e70` hook as a `const char*`, and the host hook did
`snprintf("%s", config)`. `strlen` then read address `0x12c4f51`, which is not host memory.
The RVAs are identity tags, exactly like `0x3e7e0`'s `message` argument, and must never be
dereferenced.

Fix: `call_994e70` now takes `std::uintptr_t config_rva`; the body passes the tag unchanged; the
host prints it as `0x%llx` and states that the bytes are not read. The two config constants were
also renamed to match the observed selection (the old `Config0Rva`/`Config1Rva` names were
inverted): `kInitA5cd90ConfigFlagZeroRva = 0x12c4f51` ("cpuEnableFlags=0000011") and
`kInitA5cd90ConfigFlagSetRva = 0x12c4efa` ("cpuEnableFlags=0000001"). The fixture now asserts the
selected tag.

Result: `sotc_init_window` runs clean (exit 0, empty stderr) and reports
**`post_calls_executed=15`, `post_calls_blocked=2`** — `0xa5cd90` was already native; the crash
had been hidden inside its own block record.

## `0xa5d990` (2,709 B) now executes natively

Body `0xa5d990..0xa5e425`, sha256 `2e6a87f41290146d2c9aae776af1e30c49f4c8f63af9096da33f58ca1ca4488f`,
500 instructions, **64 direct call sites and no indirect call**, cold stack-check tail at
`0xa5e41f -> 0xf8f3a0` (pinned, never called).

The body is three regions:

1. `0xa5da20..0xa5ddde` — the 32-iteration loop appending one `sndxCategory_Mute_%d` and one
   `sndxCategory_Filter_%d` bool variable to the engine's 0xc8-stride vectors at
   `engine+0x8bd0` / `engine+0x72a8`, registered through `0x20d60`, plus the conditional
   per-index toggle pair. This region is the existing native `build_sndx_category_vars`
   (`engine_category_vars.*`); the model reuses it instead of a second loop. The new module
   **writes the resulting span back** into the engine fields, matching the body's own
   `addq $0xc8,0x8bd8(%rbx)`.
2. `0xa5ddec..0xa5e058` — the "Unnamed Categories" block. **Dead:** it is gated by
   `testb $0x1,-0x731(%rbp)`, and that byte's only writer is the loop head (`0xa5da20`) fed by
   `xorl %eax,%eax` at `0xa5da17`. The model records `dead_unnamed_block = true` and never runs it.
3. `0xa5e05d..0xa5e3fc` — the static menu cluster: separator (`0x26b60`) + "Mute All" /
   "Unmute All" under the parent path "Category Mute", separator + "Filter All" / "Unfilter All"
   under "Category Filters", all flags `0x30`, all targeting the opaque `0x2dfcbd0` tag, all
   registered through `0x268b0`, then six direct `0x272d0` destroys. The constructors and the
   registration helper are the existing `fmenu_ctors_slot306` models; `0x272d0` is a
   caller-owned hook (pinned, never executed).

Observed runtime record:

```
{"callee": "0xa5d990", "caller": "0x56dcb0", "state": "executed_native",
 "detail": "native 0xa5d990 (2709 B): mute=32 filter=32 toggles=0 stage=0
            separators=2 commands=4 registered=6 destroys=6 dead_unnamed=1"}
```

`toggles=0` is honest: the original's category-name table is runtime data at `0x1604bf0` that the
host does not have, so the runtime passes a null table. The toggle path (two `0x26ea0` items per
enabled category, registered and destroyed) is fixture-covered.

The new ELF checker caught one wrong constant during the pass: the "Unfilter All" command length
is `0x1c` (28), not the `0x10` first written. The header and the checker now agree with the
body's own `movq $0x1c,-0x728(%rbp)`.

## Files changed

- `native/core/init_a5d990_body.{hpp,cpp}` — the composition model, region map, pinned strings,
  dead-gate constant, span write-back.
- `native/core/init_a5cd90_body.{hpp,cpp}` — config hook takes an RVA tag; constants renamed;
  measured call count corrected (44 direct + 2 indirect).
- `native/core/init_window_runtime.cpp` — `host_a5_994e70` no longer dereferences; engine vector
  storage, name table, registry, atexit log, menu registry and grow/insert/destroy hooks for
  `0xa5d990`; `hook_56_c` now runs the body and reports its counts.
- `native/tests/test_init_post_bodies.cpp` — asserts the selected config tag.
- `native/tests/test_init_a5d990_body.cpp` — full run, toggle path, and four fail-closed modes.
- `native/CMakeLists.txt` — `core/init_a5d990_body.cpp`, `test_init_a5d990_body`.
- `tools/lift/check_a5cd90_body.py` — body hash, 44 calls, 2 indirect sites, six strings, two
  table words.
- `tools/lift/check_a5d990_body.py` — body hash, 64 calls, the dead gate, fifteen strings.
- `HANDOFF.md`, `ROADMAP.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md` — records.

## Verification

- Full build exit **0**; full CTest **703/703** (`init_a5d990_body` added).
- `sotc_init_window` exit **0**, stderr **0 B**: `ok=true`, `post_chain_complete=true`,
  `post_calls_complete=false`, `post_call_count=17`, **`post_calls_executed=16`**,
  **`post_calls_blocked=1`**, 275 registry entries, status 0, boss flag `0xA7`,
  `first_failed_stage=""`.
- `python3 -I tools/lift/check_a5cd90_body.py` → OK (44 calls, 2 indirect, 6 strings).
- `python3 -I tools/lift/check_a5d990_body.py` → OK (64 calls, dead gate, 15 strings).
- Full checker sweep and entry map: recorded in `HANDOFF.md`.

## Not claimed

`0x5b9f90` (46,881 B / 7,685 insns) is the last blocked post-call and is **not** lifted, so
`post_calls_complete` stays false. No callee closure. No cold boot, audio, save/reload, Metal
validation or forbidden-runtime absence. G1 stays **PARTIAL**; G2/G5/G6 NOT RUN.
