# SoC Mac Native — current handoff

Updated: 2026-10-21 09:35 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Previous transfer record:
[docs/checkpoints/2026-10-20-root-next-agent-handoff.md](docs/checkpoints/2026-10-20-root-next-agent-handoff.md);
older history: `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md`.

**Project remains incomplete.** G0 PARTIAL; G1–G6 NOT RUN. No native game executable, boot,
authentic game audio, real progress save/relaunch/load, or Metal gameplay is demonstrated.

## Session addendum 2026-10-21 round 3 (`/root`)

- Claimed `CPU-01-NATIVE-SLOT24-47-01`, status ACTIVE, attempt 0. Review and verify the existing
  uncommitted native counterpart for constructor slots 24–47, with a test-only ahead table that
  cannot bypass unresolved production slot 23. Claim record:
  [docs/checkpoints/2026-10-21-root-cpu01-native-slot24-47-claim.md](docs/checkpoints/2026-10-21-root-cpu01-native-slot24-47-claim.md).
- Claim-time checkout: `main` at `57a5282`, matching `origin/main`. An unfinished native diff was
  already present before this task under `native/core/`, CMake registrations, a new ahead
  constructor fixture, and a separate `native/api/host/` implementation. Root has since reviewed
  and corrected comments/test scope and the slot-table checker; host protocol work was separately
  claimed below.
- Graft located the ahead-table symbol in `native/core/startup_ctors.{hpp,cpp}`, its state in
  `registration_thunks.hpp`, and the focused test. The test-only range is kept outside production.
- Current CPU-01 result: `check_ctor_slots.py`, `verify_ctor_family.py`, and `check_ctor_thunks.py`
  exited 0; `test_registration_thunks_ahead` built and focused CTest passed 1/1 under the ARM64
  ASan/UBSan build. The test asserts production stops at index 23; no G1 or game startup claim.
  The first missing-target build failure and CMake recovery are recorded in the linked checkpoint.
- First-build checkpoint: `check_ctor_slots.py`, `verify_ctor_family.py`, and `check_ctor_thunks.py`
  exited 0. The focused build command exited 2 before compilation because Make has no rule for
  `test_registration_thunks_ahead`; this was failed try 1, with no native test run. The cache was
  then confirmed to point to this checkout and explicitly reconfigured as recorded below. See
  [the checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-build-target-failure.md).
- Also claimed `APP-01-CORE-HOST-01`, status ACTIVE, attempt 0, for the existing native host
  protocol. A parallel read-only audit found the hello limit can disagree with its configured
  parser limit, input errors are returned as clean EOF, the closed-stdout test accepts the wrong
  exit code, and the parent test writer can receive SIGPIPE. Worker owns only `native/api/host/`
  sources/tests; root remains the sole integrator for handoff/task/evidence files. Record:
  [docs/checkpoints/2026-10-21-root-app01-core-host-claim.md](docs/checkpoints/2026-10-21-root-app01-core-host-claim.md).
- Host task is now `NEEDS_PRINCIPAL` after three failed targeted tries. Do not change or rerun the
  host task until the principal records a reassessment; see
  [the escalation](docs/escalations/APP-01-CORE-HOST-01.md). `start` still returns
  `ENGINE_NOT_IMPLEMENTED`; G4 remains NOT RUN.
- Host worker implementation is now ready in the four assigned `native/api/host/` files. It adds
  configured hello limits, distinguishes stream read errors from EOF, tightens the exit-code check,
  and handles SIGPIPE/EPIPE in the parent pipe writer. The initial implementation checkpoint was
  written before root's build verification. See
  [the implementation checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-implementation.md).
- CMake was reconfigured for this `native/` tree with Unix Makefiles, arm64, and `SOTC_SANITIZE=ON`.
  The source graph was rebuilt with the original scopes preserved plus `native`, `launcher`, and
  `tests`; `trail build --check` passes and Graft resolves the new ahead-table test file. See
  [the failed-build checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-build-target-failure.md).
- `CPU-01-NATIVE-SLOT24-47-01` is now FIXTURE_VERIFIED. Full native build exited 0 and 295/295
  CTests passed with only the escalated `sotc_host_protocol` test excluded. See
  [the completion checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-complete.md) and
  evidence row E120. Slot 23 remains the production blocker; G1 is unchanged.
- Claimed `CPU-01-SLOT23-CONSUMERS-01` for a name-free trace of the slot-23 table and consumers.
  Current status is PARTIAL: a decoded pointer path passes a table pointer to `0xe70af0`, while a
  separate reachable path hashes an object-row pointer and scans a per-object hash list. The
  pointer consumer has 31 unresolved indirect transfers, and Ghidra 12.1.4 timed out during
  whole-program auto-analysis at 330 seconds, so string necessity and function ownership remain
  unknown. No names, native data, or registration were added. See
  [the claim](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-claim.md) and
  [the partial checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-partial.md).
- `DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01` is ARTIFACT_VERIFIED: a root-owned metadata aggregate
  over the pakN manifest found 64,108 rows and 64,108 unique TOC values, with no repeated value
  groups. Two executions were byte-identical; input SHA256 remained
  `ee7c927e8ae3b28b20d61bb460c0c3c6c783cf0a7242df366f968949d5d21075`. This establishes
  uniqueness in this manifest only; digest semantics remain UNKNOWN. See E122 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-pakn-toc-digest-complete.md).
- Next exact action: claim `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01` for a separate aggregate of
  TOC-value equality across exact-path overlaps in the seven existing manifests, then independently
  replay it from root. Preserve only aggregate counts; no payload or digest-semantics claim.
- `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01` is ARTIFACT_VERIFIED: root compared the TOC field only
  across exact-path overlaps in all 21 pairs from the seven private manifests. Two executions
  matched byte-for-byte and the shared-path total matched E77: 40,216 occurrences, 4,479 equal
  values and 35,737 different values. These are metadata comparisons; digest meaning, content
  equality, overlay precedence and runtime use remain UNKNOWN. See E123 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-overlap-toc-digest-complete.md).
- Next exact action: inspect the remaining open DATA-02 work packets for another bounded task and
  record its claim before changing files. Keep parent archive/asset schema and selected-scene gates
  open until supported by evidence.
- Claimed `DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01`, status ACTIVE, attempt 0: add a synthetic
  compressed-block regression for the existing decoded-length guard in `Psarc.read`, without
  changing parser behavior or using a real archive. The exact error and focused suite must pass;
  see the [claim checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-decoded-length-claim.md).
- After reconfigure, `test_registration_thunks_ahead` built and its focused CTest passed 1/1.
  The host library and child executable build. The first host target build failed at line 508
  because `WIFEXITED` received temporary `reap(unused)`; storing status in a local made the target
  compile. Focused CTest then failed at line 333 because the output-failure fixture did not make the
  stream fail. After making flush fail deterministically, try 3 reached the bad-stdin process test,
  which did not exit with expected code 4. The actual wait status was not printed; the task is
  escalated and must not be retried without principal reassessment. See
  [the host build checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-build-failure.md)
  and [the output-fixture checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-output-test-failure.md),
  plus [the escalation](docs/escalations/APP-01-CORE-HOST-01.md).

## Session addendum 2026-10-21 round 2 (orchestrator)

- Native tests **294/294**; all **287** `tools/lift` checkers pass; launcher **80+9** tests pass.
- **Slot 11 is no longer the blocker.** Reflection scope showed the 74-record tree is lazy (vtable getters only);
  slot 11 registered with `LazyParamDefTree` returning `schema_not_available`. Slots 12–19 and 21 registered
  (registration-only). Checker verifies **118** RVA/index pairs. **First unresolved: index 23, RVA `0xe71990`**
  (needs original name strings; hash is djb2-XOR, see native `name_hash`) — open decision: import original names as data?
- Save: `save_chunk_codec` typed codec; profile-chunk facts in `docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md`.
  Unresolved: tag 4 vs 8; case-folding in name hash is inference.
- `LauncherModel` now routes through `CoreClient`; no runtime UI proof yet.
- Next: decide slot-23 name-string import; slots 24–47 family (descriptor_mutex_atexit); 6 tail-call leaf bodies; child-process core client.

## Session addendum 2026-10-21 (orchestrator, 4 parallel workers)

- Batch 34: 12 more call-free leaf ctors lifted (unregistered). Native tests now **281/281**.
- `native/core/save_stream`: opaque-payload tagged-chunk container + atomic file I/O (fixture only).
- `native/api/` C ABI 1.0 (`add_subdirectory(api)` now in `native/CMakeLists.txt`) + Swift `CoreClient`/`NativeCoreClient`;
  start() reports ENGINE_NOT_IMPLEMENTED. `LauncherModel` is not yet routed through `CoreClient`.
- Name hash for `0xe71990` is djb2-XOR at `0xe70aa0` (the ctor itself has no call); native `name_hash` added.
  Lifting the 14-record table still needs a decision on importing original names as data.
- Vtable `0x15f9ef0` mapped (222 slots; slot 12 = `0xf06330`).
- Known pre-existing checker failures (3 of 279): `check_fb747f0.py`, `check_fb76950.py` (tests absent from CMake),
  `check_math_ctor_clones.py` (asserts distinct clone cells, contradicts shared-range finding). Not fixed.
- Next: route `LauncherModel` via `CoreClient`; remaining 7 true leaves; fix the 3 checkers; slot 11 is still the blocker.

## Start here

Read [INDEX](docs/INDEX.md), [ROADMAP](ROADMAP.md), [TASKS](docs/TASKS.md), [EVIDENCE](docs/EVIDENCE.md),
[CTOR_CLASSES](docs/CTOR_CLASSES.md), then the latest checkpoints:
- [Constructor family map (573 slots)](docs/checkpoints/2026-10-21-root-cpu01-ctor-family-map.md)
- [Slot 10–47 family verification](docs/checkpoints/2026-10-21-root-cpu01-slot10-47-family.md)
- [Slot 12 structure](docs/checkpoints/2026-10-21-root-cpu01-slot12-struct.md),
  [slot 13](docs/checkpoints/2026-10-21-root-cpu01-slot13-struct.md),
  [slots 14–19](docs/checkpoints/2026-10-21-root-cpu01-slot14-19-family.md)
- Native batches [30](docs/checkpoints/2026-10-21-root-native-batch-30.md),
  [31](docs/checkpoints/2026-10-21-root-native-batch-31.md),
  [32](docs/checkpoints/2026-10-21-root-native-batch-32.md),
  [33 (analysis-only)](docs/checkpoints/2026-10-21-root-native-batch-33.md),
  [SAV chunk header](docs/checkpoints/2026-10-21-root-native-batch-sav01-chunk-header-01.md),
  [SAV checksum edges](docs/checkpoints/2026-10-21-root-native-batch-sav01-checksum-edge-01.md),
  [slot 21 constant table](docs/checkpoints/2026-10-21-root-cpu01-ctor21-const-table.md),
  [static-cleanup thunks](docs/checkpoints/2026-10-21-root-cpu01-static-cleanup-thunks.md),
  [family native helper](docs/checkpoints/2026-10-21-root-cpu01-family-native-helper.md)

## Current verified state

- Native tests: **265/265 pass** (`cmake --build local/build/native -j 8 && ctest --test-dir
  local/build/native --output-on-failure`), ASan+UBSan on, Mach-O arm64.
- Constructor-table checker: `python3 -I tools/lift/check_ctor_slots.py` verifies **108**
  RVA/index pairs; slots 0–10 are recovered; slot 11 is the explicit blocker. Lifted-but-unregistered
  gaps 50/70/82/147 are now registered and asserted.
- Launcher: **42 Swift tests / 7 suites** (`swift test --package-path launcher --scratch-path
  local/build/launcher`); adds tested accessibility copy, diagnostics text, ordered asset problems
  and lifecycle copy. Still no core ABI, no runtime UI/VoiceOver/Finder proof, G4 NOT PASSED.
- Project scripts: **103 Python tests** (`cd scripts && python3 -I -m unittest discover -s . -p
  'test_*.py'`). `python3 -I -m unittest <module>` cannot import on this interpreter — use
  `discover -s .` or the direct script path.
- RE-01-PERSISTENT-01: the saved Ghidra baseline still reopens read-only `-noanalysis`
  (38,845 functions, 9 blocks). See `local/checkpoints/root-20261020-reopen-baseline.txt`.

## What changed in this session (31 bounded tasks)

Constructor structure (all offline, all replayed by a verifier):
- **`docs/CTOR_CLASSES.md`** classifies all 573 table entries by reachable body: 227
  `leaf_vec_copy`, 12 `guarded_const`, **34 `descriptor_mutex_atexit` in contiguous runs 10–19 and
  24–47**, 300 `other`, 0 `undecoded`.
- Every one of the 34 family slots publishes **two `.bss` globals 0x10 apart**, one `malloc(0x10)`
  sized from `[0x12d2d1c]`, one mutex init through `0xb30950`
  (`pthread_mutexattr_init/settype/setprotocol/init/attr_destroy`), and exactly one
  `__cxa_atexit(0xe44a10, &global, 0x1604218)`; `0xe44a10` is `pthread_mutex_destroy` + `free`.
- Slots 12–19 are *registration thunks*, not class constructors. Slot 12's 205-byte body ends at
  `0xf0640d`; the slot-11 class constructor `0xf06410` starts immediately after and is never called
  by it.
- Six guarded-constant constructors (`0xdd91b0`, `0xdd7e30`, `0xdd4210`, `0xd39b00`, `0xd366d0`,
  `0xd33b50`) are now lifted with all 9 guarded cells and their unguarded tails; they share one
  guarded cell range, so only one of them can do meaningful work at runtime.
- Three AVX ctors (`0xd8fa40`, `0xd86ed0`, `0xd5a280`) are lifted; `0xd5a280` fills a **2048-byte**
  int32 identity table (the loop counter counts dwords).
- Batch 33 (`0xe71990`, `0xdf94c0`, `0xde0160`) is analysis-only on purpose: `0xe71990` needs the
  unknown name-hash function and original strings; `0xdf94c0` and `0xde0160` call `_FSin`/`memset`
  and a `ret`-stub `__cxa_atexit` cleanup.
- Slot 21 (`0xee1130`) is lifted as a byte-exact 128-byte constant table (15 stores into
  `0x33540f0..0x3354170`; the checker models four register/vector reloads). Cell meanings are
  unknown, so the table stays untyped.
- **15 further entries (515, 522, 523, 525, 527, 528, 531, 533, 542, 546, 547, 549, 551, 564, 565)
  are 33-byte static-destructor registration thunks**: zero one `.bss` cell, then tail-call
  `__cxa_atexit(cell, destructor, 0x1604218)`. Verified as a family; not lifted because the
  destructors are unresolved (`0x1bdae0` alone calls `0x1bede0` and tail-jumps `0x170b0`).
- `DescriptorRegistration` is the tested native lifetime model for the 34-slot family (one-shot
  publish, owned recursive+PI mutex, explicit errors) — deliberately not the original heap block or
  atexit ordering, and not registered.

Coverage statement: **every constructor-table index 0–149 now has a native lift or a documented
dossier**, and slots 21/515/522/523/525/527/528/531/533/542/546/547/549/551/564/565 are covered by
the two families above. Outside that prefix, 54 call-free `other` bodies remain unlifted (several
are the same 16-byte copy or YMM-store idioms already lifted in batches 32/33) plus the ~240 `other`
bodies that contain calls.

Services/parsers:
- New native `core/save_chunk_header.{hpp,cpp}` models the 12-byte tagged-chunk header with exact
  wire bytes and truncation/magic errors; new checksum edge vectors (one case NOT RUN by design).
- 11 new synthetic parser fixtures across PSARC, overlay/manifest JSONL, PS4 dynlink, audio header
  classification and XFF; two real defects fixed (`psarc.load_members_jsonl` error type/line number;
  `classify_audio_headers.family_of` input handling).

Tooling correctness (do not repeat these mistakes):
- A fixed-window byte scan over-reported 49 family slots; `0xf53de0`'s reachable body has no calls.
- A linear sweep ran from `0xf06340` into the next function because a `__stack_chk_fail` block falls
  through into padding.
- An `objdump` line parser that assumes a trailing space drops any instruction whose bytes fill the
  byte column, silently truncating the rest of the block (`0xdd7e30` 319 vs 396 bytes,
  `0xdd4210` 334 vs 486). Split on the tab.
- `scripts/disasm_range.sh` uses `--adjust-vma`, so its `#` target comments are wrong; re-derive
  targets from the raw displacement bytes.

## Current blocker

Slot 23 / RVA `0x00e71990` remains the first unresolved production constructor. It writes a 14-row
name/hash registry at `0x3353f40`; the accessors accept 0–12 and row 13 is a hash-zero sentinel. A
decoded call sequence passes a pointer from `0xe70a60` to `0xe70af0`, while the reachable
`0xe5cd78` path hashes an object-row pointer and scans per-object hashes in `0xe715d0`. Ghidra
timed out before function evidence was available; `0xe70af0` still has 31 unresolved indirect
transfers, so the string consumer's semantics and reachability remain open. Keep original strings
private and do not register slot 23 or claim startup/G1. Slot 11's fixture is unchanged.
See [the partial slot-23 checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-partial.md).

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- The shared descriptor helper models the 34-slot family; slots 24–47 remain in a test-only ahead
  table outside `recovered_constructors()` until production reaches and resolves slot 23.

## Next exact work

1. Continue `CPU-01-SLOT23-CONSUMERS-01` with a bounded, name-free trace of `0xe70af0`'s indirect
   calls and object/vtable source. Ghidra 12.1.4's 330-second startup timeout is recorded; do not
   treat the local decoder as equivalent Ghidra evidence. Once semantics are understood, take the
   data-only import choice to the principal before changing native code.
2. Identify the class/interface behind vtable `0x15f9ef0` (slot 12) and the forwarding callees
   `0xf011f0`, `0xefe410`, `0xefd140`, `0xefad20`, `0xef8600`, `0xf030e0` (slots 13–19).
3. Preserve `0xd71f60` NEEDS_PRINCIPAL (3 failed tries) and do not retry without principal
   reassessment.
4. Any new native module needs a README row, a test, and CMake registration; any new parser fixture
   needs a failing-before proof where it claims a defect fix.
5. Before publishing: `python3 scripts/project_index.py`, `trail build --check`,
   `python3 -I scripts/audit_publish_paths.py --all`, then commit allowlisted paths and
   `git push origin main`.

## Parallel-agent state

- Native worker completed batches 30–33 plus the two SAV-01 C++ fixtures (263/263, nine checkers,
  `local/build/native-agent-a` scratch tree remains private/regenerable).
- Parser worker completed 8 DATA-02/RE-02 fixtures; audio/archive worker completed 3 AUD-01/DATA-02
  fixtures (103 tests total).
- Launcher worker completed 4 APP-01 fixtures (42 tests / 7 suites).
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Family manifest: `local/re/work/ctor-classes.json` (per-slot span, digest, calls, class).
- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.
