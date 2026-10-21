# SoC Mac Native — current handoff

Updated: 2026-10-21 08:36 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
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
- Starting checkout: `main` at `57a5282`, matching `origin/main`. An unfinished native diff was
  already present before this task: changes under `native/core/`, CMake registrations, a new ahead
  constructor fixture, and a separate untracked `native/api/host/` protocol implementation. Root
  has not changed those implementation files yet. The host protocol is outside this task until
  separately claimed.
- Graft located the ahead-table symbol in `native/core/startup_ctors.{hpp,cpp}`, its state in
  `registration_thunks.hpp`, and the new focused test. The code comments keep it outside the
  production constructor list; this claim has not yet run the test or checker.
- Next exact action: inspect the focused fixture and checker contract, build/run the ahead-table
  test, replay `tools/lift/check_ctor_slots.py`, and prove the production runner still stops at
  index 23. Record results before any code commit. Slot 23 and G1 remain unresolved.
- Also claimed `APP-01-CORE-HOST-01`, status ACTIVE, attempt 0, for the existing native host
  protocol. A parallel read-only audit found the hello limit can disagree with its configured
  parser limit, input errors are returned as clean EOF, the closed-stdout test accepts the wrong
  exit code, and the parent test writer can receive SIGPIPE. Worker owns only `native/api/host/`
  sources/tests; root remains the sole integrator for handoff/task/evidence files. Record:
  [docs/checkpoints/2026-10-21-root-app01-core-host-claim.md](docs/checkpoints/2026-10-21-root-app01-core-host-claim.md).
- Host task's next exact action: fix those protocol and pipe-test defects, then return a concise
  checkpoint with exact changed files and the suggested focused CMake/CTest command. `start` still
  returns `ENGINE_NOT_IMPLEMENTED`; this work is not launcher IPC integration or G4 evidence.

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

## Blocker

Slot 11 / RVA `0x00f09160` is still the first unresolved original constructor
(`ClothingPhysicalMeshParameters`; ctor `0xf06410`, vptr `0x15fa770`, globals
`0x3356068`/`0x3356078`, consumer `0xed5880`, accessor `0xf08bc0`). Its singleton creator allocates
`0x2bf0` bytes of 74×`0x98` PhysX reflection records, so registration still needs that subsystem.
See [its checkpoint](docs/checkpoints/2026-10-20-root-cpu01-ctor11-structural-analysis.md).
Do not claim engine startup or G1 from slot-10 registration.

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- One shared native helper type is the intended shape for the 34-slot family, but it must stay
  unregistered until slot 11 is resolved; the family verifier is the guard against shape drift.

## Next exact work

1. Slot 11 (`0xf09160`): bound the reflection/attributes subsystem it needs, or escalate back to
   the principal with the smallest missing contract. Do not register slots 12+ first.
2. Identify the class/interface behind vtable `0x15f9ef0` (slot 12) and the forwarding callees
   `0xf011f0`, `0xefe410`, `0xefd140`, `0xefad20`, `0xef8600`, `0xf030e0` (slots 13–19).
3. Resolve the `0xe71990` name-hash function, which blocks lifting the 14 record pairs.
4. Keep `0xd71f60` NEEDS_PRINCIPAL (3 failed tries) and do not retry without principal
   reassessment.
5. Any new native module needs a README row, a test, and CMake registration; any new parser fixture
   needs a failing-before proof where it claims a defect fix.
6. Before publishing: `python3 scripts/project_index.py`, `trail build --check`,
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
