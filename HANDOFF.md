# SoC Mac Native — current handoff

Updated: 2026-10-21 10:28 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Previous transfer record:
[docs/checkpoints/2026-10-20-root-next-agent-handoff.md](docs/checkpoints/2026-10-20-root-next-agent-handoff.md);
older history: `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md`.

**Project remains incomplete.** G0 PARTIAL; G1–G6 NOT RUN. No native game executable, boot,
authentic game audio, real progress save/relaunch/load, or Metal gameplay is demonstrated.

## Session addendum 2026-10-21 round 8 (`/root`)

- `DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01` completion was committed and pushed as `fb28481`.
  This updates the completion checkpoint's previous next-action entry; E130 and the synthetic-only
  limits remain as recorded.
- Claimed `DOC-FOLDER-INDEX-ADMIN-DIR-01`, ACTIVE with zero failed tries. Add one temp-root test in
  `scripts/test_project_tools.py` proving nested `.git` and `__pycache__` trees do not make
  `docs/FOLDERS.md` stale. No source/index behavior change or live index regeneration is in scope.
  Claim: [docs/checkpoints/2026-10-21-root-doc-folder-index-admin-dir-claim.md](docs/checkpoints/2026-10-21-root-doc-folder-index-admin-dir-claim.md).
- Graft located the ignored-directory pruning in `scripts/project_index.py:27-40` and check mode
  in `:59-77`; the existing stability test covers `.DS_Store` and new folders but not these
  administrative directories. Prerequisite: none.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. No test
  has run for this task yet. After claim publication, `slot23_raw_xrefs` owns only that test file;
  root remains the sole `HANDOFF.md` integrator.
- Next exact action: publish the claim, then assign the worker to add the temp-root fixture and
  record its focused result. Keep escalated `native/api/CMakeLists.txt` and `native/api/host/`
  outside this task and all commits.

## Session addendum 2026-10-21 round 7 (`/root`)

- Completed and pushed `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01` in commit `35370fc`. The test-first
  `e_phentsize=55` fixture failed before the guard; worker and root focused runs pass 18/18 after
  it. E128 and the completion checkpoint record synthetic-only limits.
- Completed `DATA-02-OVERLAP-MAP-UNSHARED-SIZE-VALIDATION-01`, FIXTURE_VERIFIED: all member-map
  values must be exact nonnegative integers, including unshared paths. The `-1` and `True` cases
  failed before the change; worker and root each pass 18/18 after it. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-data02-overlap-map-unshared-size-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-data02-overlap-map-unshared-size-complete.md).
- Graft located the size check inside only the shared-path loop at
  `scripts/audit_archive_overlap_map.py:96-138`; callers are `build_report`, synthetic tests, then
  `main`. Existing contract says exact nonnegative integers, but unshared values bypass it.
- Claim commit `f1a98c5` is pushed to `origin/main`; worker `launcher_review` now owns the two claimed
  files for a test-first unshared negative/bool fixture and whole-map validation. No implementation
  result is integrated yet.
- Root reviewed the two-file diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`: exit 0, 18/18.
  E129 and private log hashes are recorded in the completion checkpoint. No archive data or game
  behavior is in scope.
- Commit `c0c7177` is pushed to `origin/main`; it includes the whole-map validation, two synthetic
  unshared-size cases, and E129. No real manifest/archive behavior is claimed.
- Completed `DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01`, FIXTURE_VERIFIED: a 16-byte synthetic raw
  block truncated by one byte raises the exact decoded-size `PsarcError`. Worker and root each pass
  the full PSARC suite 30/30; parser source is unchanged. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-data02-psarc-stored-block-truncation-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-data02-psarc-stored-block-truncation-complete.md).
- Graft confirms `Psarc.read` has an existing raw block-size check after `f.read(block_size)` and
  existing tests cover compressed truncation only. The fixture should assert the exact decoded-size
  mismatch on a truncated raw block; no parser source change is in scope.
- Claim commit `f41b202` is pushed to `origin/main`; `slot23_raw_xrefs` now owns only
  `scripts/test_psarc.py` for the synthetic raw-block truncation fixture. Worker result is pending.
- Root reviewed the test-only diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`: exit 0, 30/30. E130 and exact
  private log hashes are recorded in the completion checkpoint. No real archive/payload claim.
- Publication follow-up: the completion files were committed as `fb28481` and pushed to
  `origin/main`; current HEAD and `origin/main` both resolve to that commit. The commit included
  the test, E130, task/handoff updates, generated folder index, and completion checkpoint.

## Session addendum 2026-10-21 round 6 (`/root`)

- Completed and published `AUD-01-XFF-READER-IDENTITY-FIXTURE-01`: commits `14fcc86` and
  `1260c96` are on `origin/main`; worker and root both pass 16/16 synthetic tests; E127 records
  the test-only path expectation correction and unchanged parser hash.
- Completed `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01`, FIXTURE_VERIFIED: reject ELF64 program headers
  whose `e_phentsize` is not 56. The synthetic `e_phentsize=55` assertion failed before the guard;
  worker and root each pass 18/18 after it. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-re02-phdr-entsize-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-re02-phdr-entsize-complete.md).
- Graft located `program_headers` at `scripts/ps4_dynlink.py:46-52`; it decodes records with a
  56-byte stride but does not check ELF header `e_phentsize`. Its callers are parser → build → main.
  Existing synthetic tests cover class/data/machine and truncated tables, but not this field.
- Claim commit `bad72a6` was pushed to `origin/main`; `slot23_raw_xrefs` owns the two claimed
  source/test files. The worker is adding a synthetic test, recording the pre-guard failure, then
  adding the narrow check and rerunning the focused suite. No code result is integrated yet.
- Root reviewed the two-file diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`: exit 0, 18/18. E128 and
  exact private log hashes are recorded in the completion checkpoint; only synthetic parser
  behavior is demonstrated.
- Next exact action: refresh indexes, path-audit the staged source/test/docs, commit, and push the
  task completion. No live ELF, loader, or game milestone is implied.

## Session addendum 2026-10-21 round 5 (`/root`)

- Completed `AUD-01-XFF-READER-IDENTITY-FIXTURE-01`, FIXTURE_VERIFIED with one failed fixture
  attempt: synthetic tests cover full summary/archive/member identity, manifest mismatch, and reader
  closure on success/error. Worker changed only `scripts/test_audio_bank_header.py`; parser source
  was unchanged. Worker retry 2 and root replay each passed 16/16. Completion record:
  [docs/checkpoints/2026-10-21-root-aud01-xff-reader-identity-claim.md](docs/checkpoints/2026-10-21-root-aud01-xff-reader-identity-claim.md).
- The worker's first focused run, `python3 -I scripts/test_audio_bank_header.py`, failed 1 of 16
  tests because the success fixture expected a resolved summary path, while `inspect_members`
  preserves the supplied summary path verbatim. The mismatch/error cleanup test passed. Evidence:
  `local/validation/aud01-xff-reader-identity-fixture-01/attempt-1.log`; the incorrect expected
  value was test-only. The one-line fixture correction passed 16/16 on worker retry 2; root replay
  is pending. No parser source changed.
- Graft located the reader identity checks at `scripts/audio_bank_header.py:101-198`; its only
  indexed references are the definition and CLI call. The accepted fixture will mock `Psarc` and
  use temporary archive/summary/manifest files, so it does not read a game archive or invoke zlib.
- Root reviewed the diff and replayed `python3 -I scripts/test_audio_bank_header.py`: exit 0,
  16/16, log `local/validation/aud01-xff-reader-identity-fixture-01/root-replay.log` (SHA256
  `40e7e701b0fb7520dec152943e03896c5835fbb8b2ebe4c5ca0e04b62aeaeb2b`). Test SHA256
  `c5a09bccf054d0cd186144a22b602cb2777993857cd87d5ad3846cc04e230912`; parser SHA256 remains
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Commit `14fcc86` (`test(audio): verify inspect_members identity and cleanup`) was pushed to
  `origin/main`; this checkpoint's final documentation update will be committed and pushed
  separately. The task is FIXTURE_VERIFIED with no real archive, codec, playback, or G1 claim.
- Next exact action: claim `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01` as a bounded synthetic parser guard
  and regression fixture, then delegate its two owned files after the claim is published.

## Session addendum 2026-10-21 round 4 (`/root`)

- Resumed `APP-01-LAUNCHER-CORECLIENT-01` for a bounded live Mac UI and accessibility inspection
  of the already-built launcher. No source change or game-core launch is in scope. Claim record:
  [docs/checkpoints/2026-10-21-root-app01-launcher-runtime-claim.md](docs/checkpoints/2026-10-21-root-app01-launcher-runtime-claim.md).
- The existing launcher checkpoint records 89 passing Swift tests and an app-process smoke start,
  but no observed window, VoiceOver, Finder launch, or signed bundle. The executable currently
  exists at `local/build/launcher-agent-e/out/Products/Debug/SoCLauncher`.
- The first native UI inventory was blocked: `cua.getState()` reports the Mac is locked and needs
  manual unlock. No app was launched and no UI state was captured. The user has been asked to
  unlock it; the task remains PARTIAL with no failed implementation tries. Private result:
  `local/checkpoints/root-app01-launcher-ui-cua-blocked.log`.
- Next exact action after the user unlocks the Mac: bind the existing executable with CUA, capture
  its AX state and screenshot, and inspect visible readiness/state. Do not interpret the UI as game
  boot, audio, saving, or G4 evidence. VoiceOver, Finder launch, and signing remain NOT RUN.

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
- `DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01` is FIXTURE_VERIFIED: added a synthetic compressed
  member whose declared length is one byte shorter than its decoded final block. The exact existing
  error (`block 4 decoded to 16 bytes, expected 15`) is asserted; `python3 -I scripts/test_psarc.py`
  passed 27/27. Parser behavior is unchanged; see E124 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-decoded-length-complete.md).
- `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01` is ARTIFACT_VERIFIED: root independently replayed the
  aggregate across seven hash-bound manifests (170,451 rows). Of 130,235 distinct values, 35,737
  occur on multiple exact paths (71,474 row associations); 4,479 exact-path/value associations
  repeat across archives (8,958 rows), matching E123. Worker runs and identities match byte-for-byte;
  aggregate reports contain no identifiers. Semantics remain UNKNOWN; see E126 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-toc-distinct-path-reuse-complete.md).
- `DATA-02-MANIFEST-JSON-SYNTAX-01` is FIXTURE_VERIFIED: malformed nonblank JSONL now raises
  `PsarcError` with the file and 1-based line. The old parser produced the expected two
  `JSONDecodeError` test errors; the fixed parser passed 29/29, including a blank-line test preserving
  the previous behavior. Root replay passed; see E125 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-manifest-json-syntax-complete.md).
- Next exact action: continue `CPU-01-SLOT23-CONSUMERS-01` with a bounded name-free trace of the
  `0xe70af0` indirect targets and object/vtable source. Preserve unresolved targets and do not
  import original strings or register slot 23 without the principal's decision. The PSARC
  negative-limit behavior question remains open; do not change that API behavior yet.
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
