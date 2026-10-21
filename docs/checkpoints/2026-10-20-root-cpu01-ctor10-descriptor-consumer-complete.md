# Slot-10 descriptor consumer recovered; native `ClothingPreviewParam` and slot-10 wiring

- Time/agent: 2026-10-20 23:50 CDT, orchestrator session (`deepseek-v4.1-flash-1017`), sole
  integrator. Base `1f7c8e8` plus uncommitted handoff docs.
- Gates unchanged: G0 PARTIAL; G1–G6 NOT RUN. No native game boot, audio, save/load or Metal.
  This is static analysis plus a native fixture, not startup execution.

## Tasks closed in this checkpoint

| Task | Status | Evidence |
|---|---|---|
| RE-01-PERSISTENT-01 | DONE (reopen verified) | Saved project `local/ghidra/persistent-20261020` reopened read-only with `-noanalysis`; `function_count=38845`; probe log below |
| CPU-01-CTOR10-DESCRIPTOR-CONSUMER-01 | STATIC_ANALYSIS_VERIFIED | Consumer `FUN_00ed5a90`, factory `FUN_00ed59a0`, class `ClothingPreviewParam`; 45/45 verifier assertions |
| CPU-01-CTOR10-NATIVE-01 (slot 10) | FIXTURE_VERIFIED | Native descriptor + `StartupState` mutex; 252/252 CTest pass |
| CPU-01-LEAF-CTOR-BATCH-25..29 | FIXTURE_VERIFIED | 27/27 leaf ctors; worker checkpoint `2026-10-20-root-cpu01-leaf-ctor-batches-25-29-complete.md` |

## What was recovered

- Global `0x3356678` has exactly four references: slot 10 writes `0x15fb7c0`, and one virtual
  method `FUN_00ed5a90` reads it and installs it as an object vptr.
- `0x15fb7b0` is a vtable; `0x15fb7c0` is its Itanium address point. Slots 0/1 are the
  complete-object destructor (`0xf09350`) and deleting destructor (`0xf093f0`); slot 3
  (`0xf09d80`) is `reset()`.
- The class is `ClothingPreviewParam`, allocation size `0xb0`, alignment 8 (factory constants at
  `0xed59be`/`0xed59c3`; name literal at `0x1322b68`).
- Slot 10 builds a stack-local instance, publishes the address point, creates a 16-byte handle
  and initializes it with mutex attribute values 2 (recursive) and 1 (priority-inherit), then
  registers the handle cleanup. This confirms the earlier mutex contract from the caller side.

## Native changes

- New `native/core/clothing_preview_param.{hpp,cpp}`: base subobject `0x48` bytes, derived
  `0xb0`/8, recovered ctor/dtor/reset and owner release hooks, and a native descriptor.
- `native/core/startup_ctors.{hpp,cpp}`: slot 10 callback registered; `StartupState` owns the
  recursive mutex and descriptor; runner propagates `callback_failed`; explicit null slot 11
  (`0xf09160`) preserved.
- `native/tests/test_clothing_preview_param.cpp` added; `test_startup_ctors.cpp` updated for the
  11-completed / slot-11-blocked boundary and failure propagation.
- `tools/lift/check_ctor_slots.py` now verifies 104 pairs and asserts slot 11 is the blocker.
- `native/CMakeLists.txt`, `native/README.md` updated.

## Exact commands and results

```sh
cmake -S native -B local/build/native
cmake --build local/build/native -j 8
ctest --test-dir local/build/native --output-on-failure
# 100% tests passed, 252/252 (SOTC_SANITIZE=ON, ASan+UBSan, Mach-O arm64)

ASAN_OPTIONS=halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir local/build/native \
  -R '^(clothing_preview_param|startup_ctors|recursive_mutex|save_checksum)$'
# 4/4 pass

python3 -I tools/lift/check_ctor_slots.py
# OK: 104 registered RVA/index pairs; slots 0-10 recovered; blocker index 11 RVA 0xf09160

PYTHONDONTWRITEBYTECODE=1 python3 -I \
  local/re/evidence/cpu01-ctor10-descriptor-consumer-01/verify.py
# exit 0, 45/45 assertions

env JAVA_HOME=... GHIDRA_HEADLESS_MAXMEM=8G analyzeHeadless \
  local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly \
  -scriptPath local/re/work/ghidra-scripts -postScript <script>
# ReopenBaseline, ConsumerAnalysis, DescriptorDeep, SemanticsHelpers, MutexSemantics all exit 0
```

## Observations versus inference

Observed: instruction bytes, function body hashes, table words, LEA/store targets, factory size
and alignment immediates, mutex attribute immediates, class-name literal, and the decompiled
control flow of the eight exported functions.

Inference: Itanium ABI layout, slot 0/1 destructor roles, `0xf09d80` as `reset`, field semantics
of `+0x48..+0xa4`, and the exact import identity behind the lazy-binding slots
`0x00f90210`/`0x00f8f390` (zero on disk).

## Remaining risks / limits

- The original address-point table has 222 entries; only the destructor pair, `reset`, the
  constructor state block and the factory/consumer path were recovered. The remaining virtual
  methods are unknown.
- The native vtable is a semantic model, not binary-compatible with the original table. No
  original address is stored or dispatched.
- No runtime reachability, ABI or priority-inheritance behavior is proven.
- Slot 11 / RVA `0xf09160` is the next startup blocker.

## Next exact action

Claim bounded recovery of slot 11 (`0x00f09160`): use the saved read-only Ghidra baseline and
`tools/lift/check_ctor_slots.py`, or continue the audio/save static tracks. Do not treat slot 10
registration as engine startup or G1.

## Attempt counts

- CPU-01-CTOR10-DESCRIPTOR-CONSUMER-01: 0 failed semantic hypotheses. Three helper-script compile
  slips fixed before any claim (two type mismatches, one missing `throws`).
- RE-01-PERSISTENT-01: 0 failed import tries; reopen passed.
- SLOT10 native: 0 failed implementation tries after the two compile fixes above.
- CPU-01-LEAF-CTOR-BATCH-25..29: 0 failed lifts; 27/27 byte-exact.
