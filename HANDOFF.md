# SoC Mac Native — current handoff

Updated: 2026-10-22 05:52 CDT (America/Chicago). Integrator: /root (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Previous transfer record:
[docs/checkpoints/2026-10-20-root-next-agent-handoff.md](docs/checkpoints/2026-10-20-root-next-agent-handoff.md);
older history: `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md`.

**Project remains incomplete.** G0 PARTIAL; G1–G6 NOT RUN. No native game executable, boot,
authentic game audio, real progress save/relaunch/load, or Metal gameplay is demonstrated.

## DATA-02 pakN TOC payload-MD5 sample — verified

- Task ID `DATA-02-PAKN-TOC-PAYLOAD-MD5-SAMPLE-01`, owned by `/root`; status `ARTIFACT_VERIFIED`,
  one successful analysis attempt, zero failed hypotheses. It compares the TOC field with MD5 of
  the full decoded payload for exactly 17 pakN members selected from the existing
  `audio-headers.json` family inventory. The classifier calls `8c0a` / `8c06` an offset-zero
  prefix observation only; it is not a codec ID.
- This does not repeat the existing 39-member TOC-vs-path/content sample: `docs/ASSETS.md` says
  that earlier sample was pak2. E25 records the 17-row pakN family. The selection preflight joins
  17/17 rows to the pakN member manifest by entry ID, exact path, declared size, offset and block;
  total decoded payload is 955,789 bytes. No per-member paths or digests are in public notes.
- Baseline selection output and SHA-256 are in
  `local/validation/data02-pakn-toc-payload-md5-sample-01/selection-baseline.log` and E340.
  Root's live full-file SHA-256 reread passed at 12,838,532,222 bytes and matched the ASSETS
  summary: `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73` (E340).
- The private checker pins the archive, all manifests, `scripts/psarc.py`, and the family
  classifier. Both full reads decoded 17/17 members and matched the audio-manifest SHA-256 values;
  0/17 decoded-payload MD5 values matched the live TOC field. The two per-member reports are
  byte-identical (SHA256 `026295c5122b5919a849c095fa00193f46da7606a06d46905b8ca706c1e08058`); checker
  SHA256 `a403a8cd7d4fbda8ffd89343ffe3e8fa8719aca9c7c0f9c05c6305bd4cf206ca`. Per-member data stays
  under `local/validation/data02-pakn-toc-payload-md5-sample-01/`; E343 records aggregate evidence.
- Claim commit `85ad661baa087297743fdb0a3b0482c80c636961` is pushed to `origin/main` and fetch-
  verified at 0/0 divergence. Pre-push audits accepted 5 staged / 1,740 full paths; post-push full
  audit accepted 1,740 paths. E341 records the publication logs. Only user API edits and Graft
  telemetry cache files remain outside the published scope.
- The claim-audit record commit `eea1ac4c49b6ac4f4e5a019a030f2efa515cebde` is also pushed and
  fetch-verified at 0/0 divergence; its publication audit is in E342. The completion/next-task
  claim commit `fdfd466c1269e786db29769a974c3e9ac2a7a84d` is pushed and fetch-verified at 0/0;
  E344 records its path audits. `docs/ASSETS.md` and `docs/TASKS.md` record the sample-only result.
- Exact next action: review the read-only scout's next-candidate report; if it identifies an
  unclaimed bounded test-only task, verify the live source/ledger, claim it, and continue.
- Remaining risks: any result applies only to this sample and does not establish global TOC
  semantics, integrity role, codec, runtime behavior, or game use. The native/API user changes
  and the Graft telemetry cache paths remain unmodified and outside publication.

## SRV-01 host-map requested versus page-rounded size — verified

- Task ID `SRV-01-HOST-MAP-REQUESTED-VS-PAGE-ROUNDED-SIZE-FIXTURE-01`, owned by `/root`; status
  `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. The one synthetic
  case in `native/tests/test_host_map.cpp` uses `0x200001` bytes and keeps requested region metadata
  distinct from page-rounded backing-map metadata.
- The claim checkpoint is
  [docs/checkpoints/2026-10-22-root-srv01-host-map-requested-vs-rounded-claim.md](docs/checkpoints/2026-10-22-root-srv01-host-map-requested-vs-rounded-claim.md).
- Current `native/tests/test_host_map.cpp`, `native/core/host_map.cpp`, and `native/core/host_map.hpp`
  hashes are `bc966cfaf4c3a7367008ee44209558aba2a549f356887418c1124c7d7f9210de`,
  `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`, and
  `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`. These equal E325's final
  test/helper/header hashes; E325 includes an independent fresh Debug/Ninja build and focused CTest
  pass 1/1. Reusing that exact-source baseline avoids a redundant pre-change build.
- Fresh Debug/Ninja configure/build and focused `host_map` CTest pass 1/1; test source SHA256
  `ec2fe0c8a16553c6918bfb62da66c2ab7c16b93309e8bf33ccb2256d664256e7`; production helper/header
  hashes remain unchanged. E346 records commands, logs, and publication audit. See the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-host-map-requested-vs-rounded-complete.md).
- This claim and the DATA-02 completion summary were published in `fdfd466c1269e786db29769a974c3e9ac2a7a84d`; E344 records the staged and full-path audits.
- Fixture commit `8cbe3366abeec92cfb083ab2ad0e0b3d51a07319` is pushed and fetch-verified at 0/0 divergence; post-push full audit accepted 1,743 paths. E347 records the publication audit.
- Risk boundary: synthetic `map_region`/`mmap` behavior only; it does not establish allocator
  reachability or game behavior. Keep the existing 512 MiB alignment and production code unchanged.

## SAV-01 native stream directory errno — verified

- Task ID `SAV-01-NATIVE-STREAM-FILE-DIRECTORY-ERRNO-FIXTURE-01`, owned by `/root`; child of
  `SAV-01-NATIVE-STREAM-01`; status `FIXTURE_VERIFIED`, one successful implementation attempt and
  zero failed tries. The directory-read case in `native/tests/test_save_stream_file.cpp` now asserts
  both `io_read` and `os_errno == EISDIR`.
- The claim checkpoint is
  [docs/checkpoints/2026-10-22-root-sav01-native-stream-file-directory-errno-claim.md](docs/checkpoints/2026-10-22-root-sav01-native-stream-file-directory-errno-claim.md).
- Fresh Debug/Ninja configure/build passed; focused `save_stream_file` CTest passed 1/1. Test source
  SHA256 `eef6bc0f263815d54143bd0da210fae756ee26c96c06129eac772f493ea5f04c`; production source/header
  unchanged. E348 records the command logs and publication audit. See the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-sav01-native-stream-file-directory-errno-complete.md).
- Risk boundary: synthetic filesystem error reporting only; no real save compatibility, durability,
  or game behavior is claimed. Production code remains out of scope.

## SRV-01 host-map alignment wrap/minimum-clamp fixture — verified

- SRV-01-HOST-MAP-ALIGNMENT-UINT32-WRAP-CLAMP-FIXTURE-01 adds one synthetic assertion at
  `native/tests/test_host_map.cpp:32`: `reserve_alignment(0xffff0001u) == 0x100000u` after the
  32-bit 64 KiB rounding wrap reaches the 1 MiB minimum clamp. Worker and independent root fresh
  Debug/Ninja configure/builds pass; focused `host_map` CTest passes 1/1 in both. E325 records the
  final test hash and worker/root command logs.
- Project index regenerated and checked current at 42,316 folders. Trail wiring graph passes at
  3,618 nodes / 1,240 files and resolves the new assertion; `git diff --check` passes. See E326
  and the [claim/completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-host-map-align-u32-wrap-clamp-claim.md).
- `host_map.hpp:L35-L39` contains the helper; existing ordinary checks remain in
  `test_host_map.cpp:L29-L35`; CMake registers `test_host_map` / CTest `host_map` at
  `native/CMakeLists.txt:L418-L420`. The lower `align_up_64k` overflow case already exists in
  allocator-region tests. This fixture pins wrapper arithmetic only; it does not establish that
  the extreme input is a valid allocator request or reachable by game code.
- Claim commit `9718f056d8851f27bf33586fad615ba3ddd8a5f5` and completion commit
  `11c8e06db51cbfba8c4a13978793ef0f2e9792eb` are pushed and fetch-verified at 0/0 divergence;
  claim/completion publication audits are in E324/E327. Only user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remain unstaged and untouched.
- Status is `FIXTURE_VERIFIED`: one successful implementation attempt, zero failed tries. Next
  exact action: scout the next distinct bounded task.

## SRV-01 region-split no-carve-fit/no-mutation fixture — verified

- `SRV-01-REGION-SPLIT-NO-CARVE-FIT-NO-MUTATION-FIXTURE-01` is a one-case test-only addition to
  `native/tests/test_region_split.cpp:147`. `split_using_carve_fit` returns immediately when
  `try_carve_fit` reports no fit (`region_split.cpp:L30-L37`); an oversized request reaches the
  `free_size < req_size` rejection (`free_list_carve.cpp:L10-L13`).
- Existing wrapper calls cover a successful carve at `test_region_split.cpp:L120` and an exact-node
  rejection at `L132`; neither checks the no-fit branch and full node preservation. The lower-level
  `test_free_list_carve.cpp:L60-L61` checks oversized-request rejection without wrapper nodes.
- The fixture seeds distinct bytes in both 0x70-byte nodes, sets a valid free region of `0x30000`
  bytes, requests `0x30001` at 64 KiB alignment, then asserts no fit and byte-for-byte preservation
  of both full records. CTest `region_split` is registered at
  `native/CMakeLists.txt:L468-L470`.
- Baseline, worker, and independent root fresh Debug/Ninja configure/builds pass; each focused
  `region_split` CTest passes 1/1. Exact logs and final hashes are in E335, E337, and E338. Both raw
  node records are preserved on no-fit. Production/helper/header/CMake hashes stay unchanged. The
  project index is current at 45,976 folders; Trail build/check is current at 3,618 nodes / 1,240
  files and resolves the new assertion. This is synthetic helper behavior only.
- Claim commit `165cfeb9b835746fadd98cca72da2a7f6360375d` is pushed and fetch-verified at 0/0
  divergence. Pre-push audits accepted 6 staged / 1,738 full paths; post-push full audit accepted
  1,738. E336 records claim publication. The worker's test-only edit and independent root replay are
  complete; E337/E338 document the results.
- Status is `FIXTURE_VERIFIED`: one successful implementation attempt, zero failed tries. Completion
  commit `59c96624c47f583109ca66fec21fc1b9448a0643` is pushed and fetch-verified at 0/0 divergence.
  Pre-push audits accepted 8 staged / 1,739 full paths; post-push full audit accepted 1,739. E339
  records publication. Only user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain
  unstaged and untouched. The next task is the claimed DATA-02 pakN payload-MD5 sample above.

## CPU-01 math-constant mixed guard-state fixture — verified

- `CPU-01-MATH-CONSTANTS-MIXED-GUARD-STATE-FIXTURE-01` is a test-only fixture under
  `CPU-01`. Root inspected the live `MathConstants` fields and initializer: eight value/guard
  pairs; `once` branches on the low guard byte, preserving full value+guard on nonzero and setting
  value plus guard `1` when zero; `xyz_mask_copy` is always rewritten.
- Fresh root Debug/Ninja configure/build of `test_math_constants` passes; focused `math_constants`
  CTest passes 1/1. Baseline hashes and logs are in E328; folder index is current at 42,926 and
  Trail wiring graph is synchronized at 3,618 nodes / 1,240 files (E329).
- Only `native/tests/test_math_constants.cpp` changed: four fresh instances use distinct sentinels
  and unique four-bit guard signatures `1..8`, so every pair exercises both paths. Low-byte-zero
  guards retain nonzero high bytes (`0x7f00`); preserve checks assert the full guard word, and all
  four instances check the always-written `xyz_mask_copy`.
- The worker and independent root each passed a fresh Debug/Ninja configure, target build, and
  focused `math_constants` CTest (1/1). Final source/test hashes, logs, and synthetic-only scope
  are in E332 and the [completion checkpoint](docs/checkpoints/2026-10-22-root-cpu01-math-constants-mixed-guard-state-complete.md).
- Regenerated and checked the project index at 44,146 folders; `trail build --check` passes at
  3,618 nodes / 1,240 files and resolves the added initializer call. `git diff --check` passes.
  E333 records these checks. Trail's optional deep meaning layer remains unbuilt.
- Current registration is live at `native/CMakeLists.txt:L306-L308`; existing test coverage only
  mixes `half` preservation and `pi` high-byte-only reset, not all eight pairs. This remains a
  synthetic helper fixture and makes no constructor reachability or game-runtime claim.
- Claim commit `a8a0172c6e5bfff9040d88020b81df2b4d3c3b2c` is pushed and fetch-verified at 0/0
  divergence; claim publication audits accepted 6/1,736 paths (E330). The implementation passed
  on one successful try with zero failed tries. An index check after adding private validation logs first reported only
  `local/indexes/folders.json` stale; regenerating then rechecking passed at 42,926 folders (E331).
- Completion commit `f08e3bf6e318db7ab889b55375578d62a2506b20` is pushed and fetch-verified at 0/0
  divergence. Pre-push audits accepted 7 staged / 1,737 full paths; the post-push full audit
  accepted 1,737. E334 records publication. Only user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain outside the published scope.

## CPU-01 store_zero32 adjacent-canary fixture — verified

- CPU-01-STORE-ZERO32-ADJACENT-CANARY-FIXTURE-01 adds one three-word synthetic case to
  native/tests/test_store_zero32.cpp: zero the center word and assert both adjacent words retain
  their canary values. No implementation, header, or CMake file changed.
- The independent root Debug/Ninja configure and target build pass; focused store_zero32 CTest
  passes 1/1. Folder index is current at 40,486 folders. Final test and unchanged
  implementation/header/CMake hashes, logs, Trail checks, and
  limits are in E318 and the [completion checkpoint](docs/checkpoints/2026-10-22-root-cpu01-store-zero32-adjacent-canary-complete.md).
- Trail resolves the new call at native/tests/test_store_zero32.cpp:L16 and the helper at
  native/core/store_zero32.cpp:L5-L7. This confirms the helper's synthetic write boundary only;
  it does not identify a game callsite or runtime behavior.
- Status is FIXTURE_VERIFIED, one successful implementation attempt and zero failed tries. Claim
  commit cd4efb68ec9def3c1a97ead595ea39b4a97e7b7e, audit-record commit
  7cf4f7916bf919736d4d26e04ede1c0abbe76c3b, and completion commit
  e3c36eb23c95281da685a8788e6f8c7a166983c2 are pushed and fetch-verified at 0/0 divergence.
  Pre-push audits accepted 6 staged / 1,734 tracked paths; post-push full audit accepted 1,734.
  E320 records the exact logs. User API edits remain outside the published scope.
- The worker's final turn returned a model-capacity error after the edit. The root replay is the
  verification record. The user's native/api/CMakeLists.txt change and untracked native/api/host/
  remain outside this task and untouched.
- Next exact action: continue from the active SRV-01 candidate section above and publish its claim
  before any source edit.

## SAV-01 native stream error-name mapping fixture — verified

- `SAV-01-NATIVE-STREAM-ERROR-NAME-MAP-FIXTURE-01` is scoped under `SAV-01-NATIVE-STREAM-01`.
  The claim adds checks only in `native/tests/test_save_stream.cpp::test_error_names()` for the
  remaining 19 `SaveStreamError` names and the `unknown` fallback. The existing `bad_hash` and
  `none` checks stay; production and build files are out of scope.
- The fixture now checks all 21 enum names plus an out-of-range `0xff` value returning `unknown`.
  Worker and independent root fresh configure/build/focused CTest pass; only the test file changed.
  Hashes and focused logs are recorded in E295 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-sav01-stream-error-name-map-complete.md).
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries.
  This pins diagnostic strings only, with no save compatibility claim.
- Claim commit `1f194e4ef9389d33520f57a01d2439db5535d8f6` is pushed to `origin/main`; pre/post
  staged or explicit and full audits accepted 8/1,721 paths, with 0/0 fetch divergence. Publication
  logs are in E294.
- Completion commit `00b2d2c63f16919b9821a8fa786162c4eb239dbc` is pushed and fetch-verified at 0/0
  divergence. Staged and post-push explicit audits accepted 6 paths; full audits accepted 1,722
  tracked paths. User-owned `native/api/CMakeLists.txt` and `native/api/host/` remain unstaged.
  Publication hashes and the final project-index refresh are in E296.
- Exact next action: claim the separately scouted
  `SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01`; its baseline and ownership are now recorded
  below.

## SAV-01 chunk-codec unknown payload kind fixture — verified

- `SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01` under `SAV-01-NATIVE-STREAM-01` adds one
  synthetic case in `native/tests/test_save_chunk_codec.cpp::test_decode_by_spec_every_kind()`:
  a matching-tag chunk with `static_cast<PayloadKind>(0xff)` returns
  `CodecError::wrong_value_type`. Production and build files are unchanged.
- Worker and root fresh configure/build plus focused `save_chunk_codec` CTest pass; worker and root
  CTest each pass 1/1. Final hashes, logs, Trail proof, and the synthetic-only limit are in E299 and
  the [completion checkpoint](docs/checkpoints/2026-10-22-root-sav01-unknown-payload-kind-complete.md).
  Trail resolves the test at `native/tests/test_save_chunk_codec.cpp:L828-L886` and its call at L909.
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt, zero failed tries. The
  decoder returns `wrong_value_type` after the kind switch; the enum comment calls this a writer
  error. This pins current synthetic decoder behavior only and makes no save-compatibility claim.
- Claim commit `ff63a179c0c9000a6a9a71472741c5657ffee9ef` is pushed and fetch-verified at 0/0
  divergence; publication hashes are in E298. Completion commit
  `141146e0b12fdd0f1150ab1f5e1d199c11ff033e` is pushed and fetch-verified at 0/0 divergence.
  Pre/post explicit audits accepted 7 paths; full audits accepted 1,724 tracked paths. See E300.
- Documentation audit record commit `724995e513d0237d2eada3fdcb98df677f8889d5` is also pushed and
  fetch-verified at 0/0 divergence; its explicit and full audits accepted 4/1,724 paths.
- Only the pre-existing user-owned `native/api/CMakeLists.txt` modification and untracked
  `native/api/host/` remain in the worktree; neither was staged or included in the commit.
- Next exact action: send `/root/native_fixture_scout` the single-file region-list fixture below;
  `/root` retains `HANDOFF.md` integration and publication.

## SRV-01 allocator 64 KiB alignment overflow fixture — fixture verified

- `SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01` is a child of `SRV-01`. It adds one synthetic
  assertion in `native/tests/test_allocator_region.cpp::main()`:
  `CHECK(align_up_64k(0xffff0001u) == 0u);`. No implementation, header, or CMake change.
- Worker and independent root fresh builds pass; focused `allocator_region` CTest passes 1/1 in
  both. Final test/source hashes, logs, and bounded interpretation are in E303 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-align64k-uint32-wrap-complete.md).
- The live helper performs 32-bit addition of `0xffffu` before masking with `0xffff0000u`; the
  input `0xffff0001u` wraps that addition to zero. Existing tests cover ordinary rounding values,
  not this overflow boundary. Trail found the helper at `native/core/allocator_region.hpp:L50-L52`
  and existing alignment checks in `test_allocator_region.cpp:L18-L22`.
- This records synthetic integer arithmetic only, not allocator runtime behavior or game behavior.
  Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. Claim
  commit `0341cacc5273102d566a6c35b74e3fef615bd258` is pushed and fetch-verified at 0/0 divergence;
  completion commit `42331857a2212ec2f3e7c283f41b1f197da0d585` is pushed and fetch-verified at 0/0.
  Pre/post explicit audits accepted 8 paths and full audits accepted 1,727 paths. See E302 and E306.
- Next exact action: continue the separate null-head-slot fixture claim below. User-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain outside scope.

## SRV-01 region-list null head-slot insert fixture — verified

- `SRV-01-REGION-LIST-NULL-HEAD-SLOT-INSERT-FIXTURE-01` is a child of verified
  `SRV-01-REGION-LIST-EMPTY-HEAD-INSERT-FIXTURE-01`. Proposed scope is one synthetic case in
  `native/tests/test_region_list.cpp::main()`: call
  `list_insert_at_head(neu, nullptr, arena, &alloc_count)` with count 7, then check node identity,
  null next, arena-valued prev, arena owner, and count 8. No implementation, header, or CMake change.
- Worker and independent root each configured a fresh Debug/Ninja build, built `test_region_list`,
  and ran focused `region_list` CTest: exit 0; 1/1 passed in both. Final source hashes, logs, the
  post-change Trail result, and the synthetic-only limit are in E308 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-region-list-null-head-complete.md).
- Trail's `list_insert_at_head` query resolved the null-slot branch at
  `native/core/region_list.cpp:L28-L39` and existing test calls at
  `native/tests/test_region_list.cpp:L136`, `L154`, and `L168`. The helper skips slot assignment
  for a null slot pointer while still initializing node fields, optional count, and owner. This
  pins helper behavior only; it does not establish an actual list-head callsite or game behavior.
- Status is `FIXTURE_VERIFIED`: one successful implementation attempt and zero failed tries.
  Claim/completion commit `42331857a2212ec2f3e7c283f41b1f197da0d585` and docs audit record commit
  `c9ecd22156497404edcef5728d64e910730ef96e` are pushed and fetch-verified at 0/0 divergence; see
  E306/E307. Completion commit `1400c835ff74b1744c65ded8e0139cb2051b92fa` is pushed and
  fetch-verified at 0/0 divergence. Pre/post explicit audits accepted 8 paths and full audits
  accepted 1,729 paths; see E310. User-owned API changes remain unstaged and untouched.

## SRV-01 recursive-mutex owner reentrant try-lock fixture — verified

- `SRV-01-RECURSIVE-MUTEX-REENTRANT-TRYLOCK-FIXTURE-01` is a child of verified
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`. It proposes two assertions in
  `native/tests/test_recursive_mutex.cpp::main()`: while the current thread already owns two
  recursive levels, `try_lock()` succeeds and one matching `unlock()` succeeds.
- Worker and independent root fresh Debug/Ninja builds pass; focused `recursive_mutex` CTest passes
  1/1 in both. The only code change is a pair of balanced assertions while the current thread owns
  two recursive levels. Final hashes, logs, Trail evidence, and limits are in E311 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-recursive-mutex-reentrant-trylock-complete.md).
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. The
  claim is in `1400c835ff74b1744c65ded8e0139cb2051b92fa`; completion and the CPU name-hash claim are
  published in `eaa9e0b405175b78ac35c756155ae525c8f71f20`, pushed, and fetch-verified at 0/0
  divergence. E313 records the publication audits. This pins local Darwin mutex behavior only, not
  priority inheritance under contention or game behavior.
- Folder index and `trail build --check` are current: 39,266 folders and a synchronized 3,618-node /
  1,240-file wiring graph. `git diff --check` passes. Root owns documentation and publication;
  user-owned API changes remain unstaged.
- Next exact action: independently replay the worker's name-hash test edit, then publish its
  completion evidence.

## CPU-01 inline name-hash embedded-NUL fixture — verified

- `CPU-01-NAMEHASH-INLINE-EMBEDDED-NUL-FIXTURE-01` is a child of verified
  `CPU-01-NAMEHASH-E71990-01`. It proposes one synthetic assertion in
  `native/tests/test_name_hash.cpp`: `name_hash_inline("abc\0def") == 0x0b873285u`. Trail located
  the inline helper and found no embedded-NUL assertion; the test currently covers ASCII, zero hash,
  and non-ASCII inputs.
- Root fresh Debug/Ninja `test_name_hash` build passes and focused `name_hash` CTest passes 1/1;
  source and log hashes are in E312 and the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-cpu01-namehash-inline-embedded-nul-claim.md).
- Worker and independent root fresh Debug/Ninja builds pass; focused `name_hash` CTest passes 1/1
  in both. The only code change is the proposed assertion in `native/tests/test_name_hash.cpp`; final
  hashes and logs are in E314 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-cpu01-namehash-inline-embedded-nul-complete.md).
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. The
  claim is in `eaa9e0b405175b78ac35c756155ae525c8f71f20`; completion and the next CPU-01 claim are in
  pushed commit `cd4efb6`, fetch-verified at 0/0 divergence. E316 records publication audits.
- Next exact action: publish the completion audit record, then dispatch the `store_zero32` test edit.
  The synthetic case does not resolve whether the port needs original game-name strings and makes no
  game behavior claim.

## CPU-01 store-zero32 adjacent-canary fixture — active claim

- `CPU-01-STORE-ZERO32-ADJACENT-CANARY-FIXTURE-01` is a child of active `CPU-01`. It proposes one
  synthetic three-word array case in `native/tests/test_store_zero32.cpp`: zero the middle word and
  assert both neighboring canaries retain their values. Trail locates the helper at
  `native/core/store_zero32.cpp:L5-L7` and current tests at `native/tests/test_store_zero32.cpp:L8-L17`;
  the live CMake registration is `native/CMakeLists.txt:L385-L387`.
- Root fresh Debug/Ninja baseline configure/build passes and focused `store_zero32` CTest passes 1/1.
  Source hashes, logs, and limits are in E315 and the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-cpu01-store-zero32-adjacent-canary-claim.md).
- Status is `ACTIVE`, zero implementation attempts and zero failed tries. The claim is in pushed
  commit `cd4efb6`, fetch-verified at 0/0 divergence. Root owns docs and independent replay; the
  single-file edit will be assigned after the publication audit record is pushed.
- Next exact action: finish the completion audit record and dispatch the test edit. This pins only the
  local helper write boundary, not any game callsite or runtime behavior.

## SAV-01 stream-file symlink destination fixture — verified

- `SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01` is claimed under verified parent
  `SAV-01-NATIVE-STREAM-01`. Scope is one synthetic test in
  `native/tests/test_save_stream_file.cpp`: replace a destination symlink to a sentinel with a
  synthetic stream and assert the destination is a regular file containing the new bytes, the
  referent retains its original bytes, and `.tmp` is absent. The production helper is unchanged.
- Baseline and worker target builds passed; baseline, worker, and root focused `save_stream_file`
  CTests pass 1/1. The test uses `lstat`. Status is `FIXTURE_VERIFIED`, one successful implementation
  attempt and 0 failed tries. It pins current same-volume rename behavior only; no real-save
  compatibility or power-loss durability claim is made. See E276/E278 and the
  [claim](docs/checkpoints/2026-10-22-root-sav01-native-stream-file-symlink-claim.md) and
  [completion checkpoint](docs/checkpoints/2026-10-22-root-sav01-native-stream-file-symlink-complete.md).
- Claim commit `457601bde1c1f5931590a0c8fb1deb52d55e3e73`, claim-publication docs commit
  `2b456ecaf93ab3570e9b9de63f50662f19b81307`, and completion commit
  `f544acc702340fbc79e19e68e2c5050e89e4f705` are pushed. Completion staged/full audits accepted
  7/1,710 paths; fresh fetch showed 0/0 divergence; post-push explicit/full audits accepted 7/1,710.
  See E279 and publication-log SHA256 there. Project index generation/check pass at 24,080 folders;
  Trail wiring check passes at 3,617 nodes / 1,240 files, and the new method is queryable at
  `native/tests/test_save_stream_file.cpp:L104-L119`, with its call at L238. The work log SHA256 is
  in E278.

## SRV-01 HostMapping null-pointer fixture — verified

- `SRV-01-HOST-MAP-EMPTY-NULL-NONZERO-SIZE-FIXTURE-01` under verified parent
  `SRV-01-HOST-MAP-EMPTY-NONNULL-ZERO-SIZE-FIXTURE-01` adds one synthetic assertion in
  `native/tests/test_host_map.cpp`: set a default `HostMapping` size to 1 and assert
  `empty()`. Production code is unchanged. This covers the null-pointer arm of the predicate only;
  it makes no OS mapping or game-runtime claim.
- Baseline, worker, and root focused builds exit 0; their `host_map` CTests pass 1/1. Root used a
  fresh build directory and compiled the test source independently. Test/source hashes, logs,
  Trail query, and project-index evidence are in E280/E283 and the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-srv01-host-map-empty-null-nonzero-claim.md)
  and [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-host-map-empty-null-nonzero-complete.md).
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries.
  Claim commit `b33b4bfbb60c746853e6a2ca1bd5a983e92825c3` and completion commit
  `ddcb75292a9be2fa543f756c6dfa69eda574b8cf` are pushed. Staged/full audits accepted 8/1,713
  paths, fresh fetch showed 0/0 divergence, and post-push explicit/full audits accepted 8/1,713.
  Publication log SHA256 is in E282/E283. The fixture pins only synthetic accessor behavior.

## APP-01 readiness fallback-copy fixture — verified

- `APP-01-CORE-READINESS-ZERO-PROBLEM-FALLBACK-FIXTURE-01` under `APP-01` adds one
  assertion in `CoreReadinessTests.assetsStatusMapsEveryCase()` in
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`: for a non-nil asset directory and
  synthetic validation with `assetsReady == false`, `.assetsNotReady`, `problemCount == 0`, and
  empty detail, assert `.missing("The game files did not pass validation.")`. Production code is
  unchanged; this is synthetic copy-mapping only.
- Baseline, worker, and root fresh-scratch `CoreReadinessTests` runs each pass 6 tests. The worker
  and root changed test hashes, source hashes, logs, and Trail query are in E282/E285 and the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-app01-core-readiness-zero-fallback-claim.md)
  and [completion checkpoint](docs/checkpoints/2026-10-22-root-app01-core-readiness-zero-fallback-complete.md).
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries.
  Claim commit `ddcb75292a9be2fa543f756c6dfa69eda574b8cf` is pushed with staged/full audits
  accepting 8/1,713 paths; fresh fetch showed 0/0 divergence and post-push explicit/full audits
  accepted 8/1,713. Completion commit `57ec40a42f5be178375b7b40e73dd8ca9c74ae4e` is pushed;
  staged/full audits accepted 10/1,715 paths, fresh fetch showed 0/0 divergence, and post-push
  explicit/full audits accepted 10/1,715. Publication log SHA256 is in E282/E285. No actual core
  validation, asset readiness, launcher runtime, game boot, or gameplay is demonstrated.

## APP-01 dangling-symlink asset report — fixture verified

- `APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01` is scoped under verified parent
  `APP-01-ASSET-VALIDATOR-REPORT-01`. Add one synthetic case in
  `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`: point required `pak0.psarc`
  to a nonexistent target and assert the ordered problem is `pak0.psarc not found` and
  `missingArchives` contains only `pak0.psarc`. Production `AssetValidator.swift` remains unchanged.
- `APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01` adds one synthetic case in
  `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`: create a dangling symlink at
  required `pak0.psarc` and assert the exact ordered problem and `missingArchives` result. Production
  `AssetValidator.swift` is unchanged.
- Worker and root fresh-scratch runs each pass 8 `AssetValidationReportTests`; Trail resolves the
  method at L110-L123. Final test/source/package hashes, command logs, and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-app01-asset-report-dangling-symlink-complete.md)
  are in E290. Claim and baseline are in E287; the claim publication is in E288. Completion
  publication audits remain to run.
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. This
  covers synthetic dangling-target classification only, separate from a missing path and symlink
  to a directory; it makes no archive-validity or game-assets claim.
- Claim was published in `c434246b3ec9d3656289ac08525a452c88d065ec`; publication verification is
  in E288. Completion was published in `6441c02473a6c64b14246075258bf7400f3ac2bc`; staged and
  full audits accepted 7/1,719 paths, a fresh fetch showed 0/0 divergence, and post-push explicit/full
  audits accepted 7/1,719. Publication logs are in E291.

## SRV-01 region-list uncounted head-insert fixture — verified

- `SRV-01-REGION-LIST-UNCOUNTED-HEAD-INSERT-FIXTURE-01` adds one synthetic case in
  `native/tests/test_region_list.cpp`: insert before an existing head while omitting the optional
  allocation-counter argument, then check the slot, forward/backward links, arena predecessor, and
  owner. The production helper and header are unchanged.
- Worker and root builds pass; focused `region_list` CTest passes 1/1 in both. Root configured a
  fresh Ninja build with AppleClang 21 and independently compiled the target. Final test/source
  hashes, exact command logs, Trail query, and [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-region-list-uncounted-head-insert-complete.md)
  are in E286. Project index generation/check pass at 26,499 folders (446 checkpoint files); Trail
  wiring check passes at 3,617 nodes / 1,240 files. Current index/Trail log SHA256 is in E286.
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt and zero failed tries. Claim
  commit `57ec40a42f5be178375b7b40e73dd8ca9c74ae4e` remains pushed. This fixture establishes only
  synthetic intrusive-list behavior; parent `SRV-01` and all real-game gates remain open.
- Completion and claim are published in `c434246b3ec9d3656289ac08525a452c88d065ec`; staged and
  full audits accepted 7/1,717 paths, fresh fetch showed 0/0 divergence, and post-push explicit/full
  audits accepted 7/1,717. Audit log hashes and the post-publication project-index/Trail recheck are
  in E288.
- This published fixture is complete; the next bounded task is the region-split metadata claim
  below.
- Keep pending Mach-O CPU and PSARC policy choices untouched.

## SRV-01 region-split metadata preservation — verified

- `SRV-01-REGION-SPLIT-SUCCESS-PRESERVE-FREE-NODE-METADATA-FIXTURE-01` is scoped under `SRV-01`.
  The new synthetic success case seeds a free node, calls `split_free_at_lead` with `lead = 0xf000`,
  asserts capacity and size become `lead`, and checks all other free-node raw bytes against the
  snapshot. Only `native/tests/test_region_split.cpp` changed; `region_split.cpp` and its header are
  unchanged.
- Worker build and focused `region_split` CTest pass; root independently configured a fresh
  AppleClang 21 / Ninja Debug tree, built the target, and passed the focused CTest 1/1. Test and log
  hashes are in E292 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-region-split-metadata-complete.md).
- `python3 scripts/project_index.py` and `--check` pass at 29,507 folders. Trail was rebuilt with
  explicit `scripts`, `tools`, `reference`, `native`, `launcher`, and `tests` scopes and
  `trail build --check` passes at 3,618 nodes / 1,240 files.
- Status is `FIXTURE_VERIFIED`, one successful implementation attempt, zero failed tries. The first
  root CTest wrapper invocation failed before launching CTest due to a generated Python string
  quoting error; the corrected invocation passed. Its record is in
  `local/validation/20261022-root-fixture-baselines/srv01-region-split-metadata-root-ctest-wrapper-error.log`.
- Claim published in `6441c02473a6c64b14246075258bf7400f3ac2bc`; staged/full and post-push
  explicit/full audits accepted 7/1,719 paths, with 0/0 fetch divergence. Publication logs are in
  E291. Completion commit `1f194e4ef9389d33520f57a01d2439db5535d8f6` is also pushed; its staged and
  post-push explicit/full audits accepted 8/1,721 paths, with 0/0 fetch divergence. Scope remains
  synthetic helper behavior only, with no allocator-runtime or game claim.
- Exact next action: receive the assigned `test_save_stream.cpp` fixture, then independently replay
  its focused CTest.

## RE-01 identity sidecar symlink alias fixture — verified

- `RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01` under verified parent
  `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01` adds one synthetic test in
  `scripts/test_project_tools.py`: point the derived `out.identity.json` sidecar at the synthetic
  SELF input through a symlink, then assert the exact pre-write `Path.samefile` rejection, unchanged
  source bytes, and no ELF output. No wrapper change is planned.
- Baseline passes 11 tests; worker and root focused runs pass 12. Production wrapper remains
  unchanged. See E272/E274 and the [claim](docs/checkpoints/2026-10-22-root-re01-sidecar-symlink-claim.md)
  and [completion checkpoint](docs/checkpoints/2026-10-22-root-re01-sidecar-symlink-complete.md).
- Status `FIXTURE_VERIFIED`, one successful implementation attempt and 0 failed tries. Completion
  commit `0aee49f185e8162fda021f74fc4b48dc742ffd35` is pushed; staged/full audits accepted 6/1,708
  paths, fresh fetch showed 0/0 divergence, and post-push full audit accepted 1,708 paths. See E275.
  Trail graph check passes at 3,616 nodes / 1,240 files, and the new method resolves at
  `scripts/test_project_tools.py:L119-L144`. This is synthetic filesystem behavior only; it says
  nothing about live game files or execution.

## AUD-01 probe offset cutoff fixture — verified

- `AUD-01-CLASSIFIER-PROBE-OFFSET-CUTOFF-FIXTURE-01` adds one test method in
  `scripts/test_audio_headers.py`. It pins `too_short` for zlib, raw-DEFLATE, and LZMA offset-2
  probes on 2-byte input and LZMA offset-4 on 4-byte input. Worker and root audio-header suites
  pass 27 tests; the classifier is unchanged. See E269/E270 and the
  [claim](docs/checkpoints/2026-10-22-root-aud01-probe-offset-cutoff-claim.md) and
  [completion checkpoint](docs/checkpoints/2026-10-22-root-aud01-probe-offset-cutoff-complete.md).
- Status `FIXTURE_VERIFIED`, one successful implementation attempt and 0 failed tries. Trail wiring
  graph check passes at 3,615 nodes / 1,240 files and resolves the new method at
  `scripts/test_audio_headers.py:L147-L154`. Project index check passes at 24,080 folders. Root
  replay log SHA256 is recorded in E270. Completion commit `f82942d7688f8fdc88274540ab72ebf38eeedcf6`
  is pushed; staged/full audits accepted 6/1,706 paths, fresh fetch showed 0/0 divergence, and
  post-push full audit accepted 1,706 paths. Publication log SHA256 is recorded in E271. This is
  synthetic classifier behavior only; it establishes no asset parsing, codec identity, archive
  interpretation, or game playback.
- Keep pending Mach-O CPU and PSARC policy choices untouched.

## RE-02 dynamic-link image file-offset bounds fixture — verified

- `RE-02-DYNLINK-IMAGE-READ-FILE-OFFSET-BOUNDS-FIXTURE-01` adds one synthetic case to
  `scripts/test_ps4_dynlink.py`: virtual containment succeeds, but the segment file offset extends
  past the 3-byte ELF buffer, so `Image.read()` raises the exact `span()` bounds error. Baseline
  passed 29 tests; final run passed 30. Parser SHA256 is unchanged. See E266 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-re02-dynlink-image-read-file-offset-complete.md).
- Status `FIXTURE_VERIFIED`, one successful implementation attempt and 0 failed tries. Trail rebuilt
  to 3,614 nodes / 1,240 files and resolves the new method at
  `scripts/test_ps4_dynlink.py:L357-L363`; wiring check passes. Project index is current at 24,080
  folders. Claim commit `17ff82e5b58eeb7f24aaf3597369e549e894c5cc` and completion commit
  `dc9f153e23551ef7333c34523eba2cbd2ab223bf` are pushed. Completion staged/full audits accepted
  6/1,704 paths; a fresh fetch confirmed 0/0 divergence and post-push full audit accepted 1,704.
  This is a synthetic helper test only; no real ELF, loader, or runtime claim.
- The follow-on AUD-01 task is claimed above. Leave pending Mach-O CPU and PSARC policy choices
  alone.

## Completed checkpoints — two bounded regression fixtures

- `AUD-PUBLISH-LAUNCHER-ALLOWLIST-FIXTURE-01` under `AUD-PUBLISH-EDGE-01` adds one test-only
  assertion in `scripts/test_audit_publish_paths.py` for `launcher/Package.swift` and each current
  allowlisted top-level metadata path. Root replay passes 9 tests; baseline passed 8. Production
  audit code is unchanged. See the [completion checkpoint](docs/checkpoints/2026-10-22-root-aud-publish-launcher-allowlist-complete.md).
- `AUD-01-XFF-PREFIX-OFFSET-ONLY-FIXTURE-01` under `AUD-01-XFF-01` adds one test-only case in
  `scripts/test_audio_bank_header.py`: a synthetic member with `XFF_MAGIC` only in bytes 4..7 of
  its opaque span and `qath` at byte zero is rejected for the byte-zero prefix. Worker and root
  replays pass 22 tests; baseline passed 21. Production parser is unchanged. See the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-aud01-xff-prefix-offset-only-complete.md).
- Both tasks are `FIXTURE_VERIFIED`, each with one successful implementation attempt and zero
  failed tries. The new methods are queryable in Trail; wiring graph is current at 3,612 nodes /
  1,240 files and the deep meaning layer is not built. The completion commit
  `a0bf2b85be868ecc5de98c17d22dccef6247aefb` is pushed; staged and exact-path audits accepted 8
  paths, full audit accepted 1,700, and fresh fetch confirmed 0/0 divergence. No production code,
  asset-decoding, playback, or game evidence changed. Claim commit
  `93c693d211fadee9d16746f6be56ea5fca300bd1` is pushed and fetch-verified at 0/0; its staged and
  exact-path audits accepted 6 paths and full audit accepted 1,698.

## APP-01 audio readiness start gate — verified

- `APP-01-AUDIO-READINESS-GATE-FIXTURE-01` adds one test-only case in
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`: assets, core, and Metal are ready;
  injected audio output is missing; the audio row names the blocker; Play stays disabled; and the
  scripted core's `start()` is never called. Baseline `LauncherModelValidationTests` passed 8 tests;
  the final run passed 9. Production model/readiness sources are unchanged. See the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-app01-audio-readiness-gate-claim.md) and
  [completion checkpoint](docs/checkpoints/2026-10-22-root-app01-audio-readiness-gate-complete.md).
- Status `FIXTURE_VERIFIED`, one successful implementation attempt, 0 failed tries. Trail wiring
  graph is current at 3,613 nodes / 1,240 files; the new method is queryable, and the project index
  is current at 24,080 folders. Completion commit `eab74f91afbac3d8d56183631bb834df43c9e334` is
  pushed, staged and exact-path audits accepted 6 paths, full audit accepted 1,702, and fresh fetch
  confirmed 0/0 divergence. This is only an injected host-check test, not a physical audio probe or
  audible-content claim. The independent RE-02 follow-up is recorded above. Leave pending Mach-O
  CPU and PSARC negative-limit choices alone.

## SRV-01 arena high-water uint64 wrap fixture — verified

- Claimed bounded task `SRV-01-ARENA-HIGH-WATER-UINT64-WRAP-FIXTURE-01` under `SRV-01`.
  Scope: add one synthetic assertion in `native/tests/test_arena_high_water.cpp` that seeds both
  used counters at `UINT64_MAX - 3`, both peaks at 10, adds 8, and checks both used values wrap to
  4 while both peaks stay 10. Production code and checker are unchanged. See the
  [claim checkpoint](docs/checkpoints/2026-10-22-root-srv01-arena-high-water-u64-wrap-claim.md).
- Added one scenario to `native/tests/test_arena_high_water.cpp`. Baseline and worker/root target
  builds exit 0; focused `arena_high_water` CTest passes 1/1 for worker and root. Test/helper/checker
  hashes, logs, Trail results, and project-index results are in E258 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-arena-high-water-u64-wrap-complete.md).
  Production code and byte checker are unchanged.
- Status `FIXTURE_VERIFIED`, one successful implementation attempt, 0 failed tries. The assertion
  pins only synthetic `uint64_t` wrap and peak retention. Claim commit
  `b8a734c0c20b731e9f8a5de11669b2acdd9f1469` and completion commit
  `e9a6c631d0a68f4bca66a3a0ddedc5689c0f1bfd` are pushed and fetch-verified at 0/0 divergence.
  Staged/full audits accepted 7/1,696 paths; the post-commit exact-path audit also passed.
- Current work is listed in the active-claims section above. This completed task remains synthetic
  arithmetic only; the pending Mach-O CPU and PSARC negative-limit choices remain untouched.

## RE-01 identical-output SELF unwrap repeat fixture — verified

- `RE-01-UNWRAP-IDENTICAL-ELF-REPEAT-FIXTURE-01` adds one synthetic test in
  `scripts/test_project_tools.py`: call `unwrap()` twice with the same plaintext SELF, source and
  destination, then assert the second result, ELF bytes and identity-sidecar bytes match the first.
  The wrapper implementation is unchanged. See E256 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-re01-unwrap-repeat-complete.md).
- Baseline suite passed 10 tests; worker and root replay each pass 11. The reference checkout stays
  clean at pinned commit `70bb43859cd7ff181a6d409540b757866184d60d`. Project index is current at
  24,078 folders. Trail wiring graph is in sync at 3,610 nodes / 1,240 files; its source query
  resolves the new method. The deep meaning layer is not built.
- Status `FIXTURE_VERIFIED`; one successful attempt, 0 failed tries. Claim commits
  `8020974b0d77aca07143026ab717a2c44c9f9740` and `af40659`, plus completion commit `b58c301`, are
  pushed and fetch-verified at 0/0 divergence. Staged/full audits accepted 7/1,694 paths. This is
  synthetic wrapper repeatability only; it does not process a real game SELF or execute code.
- This completed fixture is published; current work and its exact next action are recorded at the
  top of this handoff.

## SRV-01 arena-page-debit uint32 wrap fixture — verified

- `SRV-01-ARENA-PAGE-DEBIT-UINT32-UNDERFLOW-WRAP-FIXTURE-01` adds one test-only helper assertion
  in `native/tests/test_arena_page_debit.cpp`: seed the synthetic page count to 1, debit two 64 KiB
  pages, and assert `UINT32_MAX`. This records observed uint32 helper arithmetic; it does not say a
  valid allocator runtime may underflow. See E255 and the
  [completion checkpoint](docs/checkpoints/2026-10-22-root-srv01-arena-page-debit-wrap-complete.md).
- Baseline, worker, and root target builds exit 0; focused `arena_page_debit` CTest passes 1/1 for
  worker and root. The helper implementation is unchanged. Project index is current at 24,078
  folders. Trail wiring graph check passes at 3,609 nodes / 1,240 files; the source query resolves
  `native/tests/test_arena_page_debit.cpp`. Trail's deep meaning layer is not built.
- Status `FIXTURE_VERIFIED`; one successful implementation attempt, 0 failed tries. Claim commit
  `872f9b66ae02711bb2bf3c5dac6b47b01bda397d` and completion commit
  `a9ec99ebe7d8c1c45fb76b01b252b53794a828e9` are pushed and fetch-verified at 0/0 divergence.
  Staged audit accepted 7 paths; full audit accepted 1,692. Existing user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remain outside the task.
- Exact next action: evaluate the read-only `RE-01-UNWRAP-IDENTICAL-ELF-REPEAT-FIXTURE-01` candidate
  from `/root/next_independent_task_scout` against the live spec, task list, and tests; claim it only
  if distinct and supported. Leave the pending Mach-O CPU and PSARC negative-limit choices alone.

## CPU-02 max-offset and AUD-01 archive-summary fixtures — verified

- `CPU-02-TAGGED31-RETAG-MAX-OFFSET-FIXTURE-01` now pins retag/decode of `0x7fffffff` in a
  synthetic full 2 GiB `HeapWindow`; worker and root focused builds pass and `addr` CTest passes
  1/1. Production code is unchanged.
- `AUD-01-XFF-ARCHIVE-SUMMARY-SHA256-MISMATCH-FIXTURE-01` keeps the summary byte count correct,
  corrupts only its digest, and asserts the exact error before `Psarc` construction. The
  audio-bank-header suite passes 21/21. Production parser is unchanged, and `docs/ASSETS.md` now
  reports 21 synthetic test methods.
- See [completion checkpoint](docs/checkpoints/2026-10-22-root-cpu02-aud01-fixtures-complete.md)
  and E253/E254. Claim commit `8e7c307020022f03691da4bb0255226822b2ec52` and completion commit
  `c97c1dbb957882a915a89927ef9a94a9e0eb5740` are pushed and fetch-verified at 0/0 divergence.
  Staged audit accepted 8 allowlisted paths; full audit accepted 1,690.
- Trail initially reported stale while both test edits were in progress. After rebuilding, the
  final wiring check passes at 3,609 nodes / 1,240 files. Project index regeneration and check pass
  at 24,078 folders. The first project-index check was stale because its own output log was created
  after generation; pre-creating both logs resolved the ordering issue.
- Both tasks are `FIXTURE_VERIFIED`, 1 successful implementation attempt each and 0 failed tries.
  Evidence is synthetic only; no game/runtime acceptance gate changes. Existing user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remain untouched.
- Exact next action: independently verify the read-only arena-page-debit wrap candidate, then claim
  it only if the live spec and existing tests confirm a distinct arithmetic fixture. Leave the
  pending Mach-O CPU and PSARC negative-limit choices untouched.

## SRV-01 HostMapping non-null / zero-size empty state — fixture verified

- Root claimed bounded task `SRV-01-HOST-MAP-EMPTY-NONNULL-ZERO-SIZE-FIXTURE-01` under `SRV-01`;
  see [claim checkpoint](docs/checkpoints/2026-10-21-root-srv01-host-map-empty-nonnull-zero-claim.md).
- The fixture now constructs a `HostMapping` with a non-null pointer to a local byte and zero size,
  then asserts `empty()`. The baseline, worker, and root replay builds pass; focused `host_map` CTest
  passes 1/1. Production code is unchanged.
- This pins only the accessor predicate. It does not call mmap, exercise OS mapping, prove allocator
  integration, or establish game behavior. Normal map requests reject zero size.
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. See the
  [claim checkpoint](docs/checkpoints/2026-10-21-root-srv01-host-map-empty-nonnull-zero-claim.md),
  [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-host-map-empty-nonnull-zero-complete.md),
  and E251/E252.
- Trail wiring graph check passes with 3,608 nodes / 1,240 files. Project index is regenerated for
  the completion checkpoint. Existing user-owned `native/api/` changes remain untouched and
  unstaged.
- Completion commit `47f332e3a323312fc3f3e5bab0a282d725811c04` is pushed and fetch-verified at 0/0
  divergence; staged audit accepted 6 paths and the full audit accepted 1,688. Publication logs are
  recorded in E252. The thin Mach-O CPU policy remains pending the user's choice.
- Newer progress is recorded at the top of this handoff.

## SRV-01 allocator rounding zero-input fixture — fixture verified

- Root completed bounded task `SRV-01-ALLOCATOR-ROUNDING-ZERO-INPUT-FIXTURE-01` under `SRV-01`;
  see the [claim checkpoint](docs/checkpoints/2026-10-21-root-srv01-allocator-rounding-zero-claim.md)
  and [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-allocator-rounding-zero-complete.md).
- Scope is three assertions in `native/tests/test_allocator_region.cpp`:
  `align_up_64k(0) == 0`, `align_up_1m(0) == 0`, and `align_size_up_64k(0) == 0`. Baseline target
  build and focused CTest passed; the changed target builds and focused CTest passes 1/1. Production
  helpers are unchanged. Logs and hashes are recorded in E249.
- Trail initially reported the edited file stale; `trail build` rebuilt the wiring graph, and the
  final check returned `OK` for 3,608 nodes / 1,240 files. Lexical `trail ask` resolves
  `native/tests/test_allocator_region.cpp`; literal search for `align_up_64k(0)` had no hit, so Trail
  confirms the indexed file but does not surface that assertion text. The project folder index is
  current at 24,075 folders.
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. This pins only
  synthetic integer rounding and makes no allocator integration or runtime claim. Claim commit
  `dbad3b0e9f905293b18f76691edf7b7839536f68` and completion commit
  `6481d1f124edcbdec0637fe028876aee73db430c` are pushed and fetch-verified at 0/0 divergence; see E250.
- Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain outside this task.
- Exact next action: claim another independent native fixture while the thin Mach-O CPU policy
  remains pending; leave that launcher task untouched until the user chooses its CPU acceptance rule.

## AUD-01 empty decompression-probe input — fixture verified

- Root claimed bounded task `AUD-01-CLASSIFIER-SHORT-PROBE-INPUT-FIXTURE-01`, child of `AUD-01`;
  the governing audio inventory and evidence boundary are in [docs/ASSETS.md](docs/ASSETS.md#aud-01-header-classification-and-member-identity-codec-unknown).
- Scope was one test in `scripts/test_audio_headers.py`: call `cah.decompress_probe(b'')` and assert
  exactly seven zlib, raw-DEFLATE, and LZMA offset results, each
  `{'ok': False, 'err': 'too_short'}`. Focused test passes 1/1 and the full audio-header suite passes
  26/26. Production classifier and archives were untouched; no codec/audio claim. See E247 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-classifier-short-probe-input-complete.md).
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. The probe guard
  only covers empty input; it does not characterize real archive members, codec identity, or playback.
- Claim commit `0052594834883aa8186be8bc599c1b4f7af6d02e` and completion commit
  `d1da5f14fb2bf862724d925555918b5d09024a9e` are pushed and fetch-verified at 0/0 divergence.
  Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` changes remain unstaged.
- Exact next action: wait for the user's thin Mach-O CPU policy choice before claiming the follow-on
  launcher architecture task; continue scouting an independent task outside that decision.

## DATA-02 TOC-value / declared-size crosstab — artifact verified

- Root completed bounded task `DATA-02-TOC-VALUE-SIZE-CROSSTAB-01`; claim details are in
  [the claim checkpoint](docs/checkpoints/2026-10-21-root-data02-toc-value-size-crosstab-claim.md).
- Worker and root analysis produce 4,479 equal-value/equal-size, 0 equal-value/different-size,
  225 different-value/equal-size, and 35,512 different-value/different-size exact-path pair
  occurrences. The 40,216 total and E77/E123 margins reconcile. Aggregate report contains no member
  paths or raw TOC values; no archive or payload was opened. See E239 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-toc-value-size-crosstab-complete.md).
- Status `ARTIFACT_VERIFIED`; this remains metadata-only, with no byte-identity, digest-semantics,
  loader, mount-order, or runtime claim. Parent DATA-02 remains ACTIVE / partial.
- Claim commit `f9fc4302f1702af6677e90121a6040becc533d5e`, documentation syncs
  `539a6bd1cfae0f3d75f13ac460d6c3ddf851734e` / `5da4e8f37f26f3b156055a202252123b9e01a41c`,
  and the crosstab-plus-parser claim commit `4fb696fe09af255c4219705c69af8b95f5f2d0fe` were pushed
  and fetch-verified.
- Dynlink parser completion is recorded in commit `109b348b7e9d09a518d0cb78a4c7bd14a1c75284`,
  pushed to `origin/main` and fetch-verified with 0/0 divergence.
- Exact next action: complete the recursive-mutex non-owner unlock fixture claimed below; leave PSARC
  negative-limit behavior unclaimed pending the user's answer.

## RE-02 dynlink required program headers — fixture verified

- Root completed `RE-02-DYNLINK-REQUIRED-PHDR-01`, child of `RE-02-DYNLINK-01`; scope and baseline
  are in [the claim checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-required-phdr-claim.md).
- `parse()` now raises `ValueError('missing PT_DYNAMIC program header')` and
  `ValueError('missing PT_SCE_DYNLIBDATA program header')`; both exact messages are asserted by
  synthetic fixtures. Worker and root each pass all 29 dynlink tests. Mutations that remove either
  guard fail with the original `StopIteration`; a root synthetic duplicate-header replay confirms
  first-match behavior for both required types.
- Root's first duplicate-check comparison failed because it compared the returned `ph` list after
  intentionally adding a header. The corrected check compares parsed tables while retaining the
  invalid later duplicate; see both attempt logs in the private validation directory and E240.
  This was a verification-harness correction, not a failed implementation try.
- Status `FIXTURE_VERIFIED`; attempt count 0 failed implementation tries. No live ELF, loader,
  runtime, or game behavior claim. See E240 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-required-phdr-complete.md).
- Publish preflight passed: `git diff --cached --check`; staged
  `python3 -I scripts/audit_publish_paths.py` reported 7 allowed paths (log SHA256
  `425420af1120f073fb803e1f280ab1afa6dd17141303f1165fef19e57e21f29c`); full
  `python3 -I scripts/audit_publish_paths.py --all` reported 1,670 allowed paths (log SHA256
  `04a990ad7f752384cd56e45177d99c5723f916b29ad6eef6dca4a67fe560a382`). Before the commit,
  `git fetch origin main` exited 0 and `git rev-list --left-right --count HEAD...origin/main`
  returned `0 0`. Commit `109b348b7e9d09a518d0cb78a4c7bd14a1c75284` was pushed; post-push fetch
  confirmed `HEAD == origin/main` and 0/0 divergence. Existing native API edits stayed unstaged.
- Exact next action: complete the archive status-code fixture claimed below; leave PSARC
  negative-limit behavior unclaimed pending the user's choice of invalid-error versus empty-read.

## APP-01 core-client internal error copy — fixture verified

- Root completed `APP-01-CORECLIENT-INTERNAL-ERROR-COPY-FIXTURE-01`, child of `APP-01`. Scope was only
  `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`; assert that `.internalError` and
  `.unknown(5)` both map to the existing exact fallback text `The game core reported an error.`
- The current suite already checks exact text for `.io`, `.badState`, and `.abiMismatch`, and checks
  `.unknown(5)` equals `.internalError`; this task pins the matching fallback without changing
  production wording or claiming UI/runtime behavior.
- Baseline command `swift test --package-path launcher --scratch-path
  local/build/app01-internal-error-copy --filter CoreClientTests` passed 22 tests (9 native-client,
  13 core-client), exit 0. Log SHA256 `22286765f4a240ea0890103ca94d9503186383e9635c1e245aa5e5739d38c647`;
  source/test baseline hashes are in the claim checkpoint.
- Root added exact assertions for both cases without changing production code. The focused suite
  passed 22 tests; completion log SHA256 `bf2010b1150e5b5578cd4fe8c9ec65d7153d7ce96e88dbdc3cb1e5bf19eeb8bb`.
  Status `FIXTURE_VERIFIED`; 0 failed implementation tries. No UI, runtime, ABI, core, or game
  behavior claim. See E241 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-coreclient-internal-error-copy-complete.md).
- Claim docs commit `8c33c317cc5cb6863b3ab618ebfb28d36557570a` is pushed and fetch-verified.
- Completion commit `2152dfbbe90699d550b07d26b90d3bfd7d18394b` is pushed and fetch-verified at
  `origin/main` with 0/0 divergence.
- Exact next action: complete the recursive-mutex non-owner unlock fixture claimed below;
  leave negative-limit behavior unclaimed pending the user's answer.

## APP-01 archive status codes — fixture verified

- Root claimed `APP-01-CORE-ARCHIVE-STATUS-CODES-FIXTURE-01`, child of
  `APP-01-CORE-ABI-01`; see the [claim checkpoint](docs/checkpoints/2026-10-21-root-app01-core-archive-status-codes-claim.md).
- Scope was only `launcher/Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift`. The new
  `archiveStatusNumericCodesMapToAbiCases` fixture asserts codes 0–3 map to `.ok`, `.missing`,
  `.unreadable`, `.badMagic`, and codes 9 / `UInt32.max` remain `.unknown` with the original value.
- Baseline command `swift test --package-path launcher --scratch-path
  local/build/app01-core-archive-status-codes --filter NativeCoreClientTests` passed 9 tests; final
  replay passed 10 tests. Logs and hashes are recorded in E242 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-core-archive-status-codes-complete.md).
- The C enum constants and Swift mapping align. Only the fixture test file changed; the production
  mapper/header are unchanged. This pins a pure mapping contract, not live ABI transport, archive
  inspection, filesystem behavior, or game readiness.
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. Existing
  `native/api/CMakeLists.txt` and `native/api/host/` changes remain user-owned and excluded.
- Claim commit `ee7e7a906450ecf75e328d6d00c2ea4feeb1a44c` and completion commit
  `45d0656b59a07a8595c9886f0cbe432230e82914` were pushed and fetch-verified; `HEAD == origin/main`
  with 0/0 divergence. User-owned native API changes remained unstaged.
- Exact next action: complete the claimed `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01` fixture;
  leave PSARC negative-limit behavior unclaimed pending the user's answer.

## SRV-01 recursive-mutex non-owner unlock — fixture verified

- Root claimed `SRV-01-RECURSIVE-MUTEX-NONOWNER-UNLOCK-FIXTURE-01`, child of
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`; see the
  [claim checkpoint](docs/checkpoints/2026-10-21-root-srv01-recursive-mutex-nonowner-unlock-claim.md).
- Scope is test-only in `native/tests/test_recursive_mutex.cpp`: while the main thread owns two
  recursive levels, another thread directly calls `unlock()` and must receive Darwin `EPERM`; the
  owner must still release both levels. Apple documents `EPERM` when the current thread holds no
  lock on the mutex. The wrapper implementation and CMake target remain unchanged.
- Baseline build command `cmake --build local/build/native --target test_recursive_mutex` and focused
  CTest command `ctest --test-dir local/build/native -R '^recursive_mutex$' --output-on-failure`
  both pass; CTest is 1/1. Logs and hashes are in the claim checkpoint.
- The worker added a direct non-owner unlock attempt while the owner holds two levels; it returns
  `EPERM`, after which the owner successfully releases both levels. Worker and root build the target
  successfully and each pass focused CTest 1/1. Logs and hashes are in E243 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-recursive-mutex-nonowner-unlock-complete.md).
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. This is a local
  Darwin mutex error-contract fixture only; it does not validate scheduler priority inheritance or
  game behavior. Existing user-owned native API edits remain untouched.
- Claim commit `5815f97ce87576c7cf9b9348ddc0aed4a1fcd8d6` and completion commit
  `0ae3bcabf96cd115fdaf40cf4aa79cef6e22794e` are pushed and fetch-verified. The completion commit's
  staged-path audit accepted 8 allowlisted paths and the full audit accepted 1,679 paths. Post-push
  `HEAD == origin/main` with 0/0 divergence; only user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain outside the commit. Audit/fetch/status logs are in
  `local/validation/app01-core-error-codes/` and the completion checkpoint.

## APP-01 CoreErrorCode numeric values — fixture verified

- Root claimed `APP-01-CORE-ERROR-NUMERIC-CODES-ALL-VALUES-FIXTURE-01`, child of
  `APP-01-CORE-ABI-01`; see the [claim checkpoint](docs/checkpoints/2026-10-21-root-app01-core-error-numeric-codes-claim.md).
- Scope is a new test-only file `launcher/Tests/LauncherCoreTests/CoreErrorCodeMappingTests.swift`.
  Assert C ABI error values 0–8 map to `.ok`, `.invalidArgument`, `.badState`, `.assetsNotReady`,
  `.engineNotImplemented`, `.io`, `.bufferTooSmall`, `.abiMismatch`, `.internalError`; 9 and
  `UInt32.max` remain `.unknown(value)`. Keep production code unchanged.
- Baseline command `swift test --package-path launcher --scratch-path
  local/build/app01-core-error-codes --filter CoreClientTests` passes 13 existing tests. The current
  `CoreClientTests.numericCodesMapToTypedValues` asserts only error codes 3, 4, and 99.
- The standalone fixture asserts every code 0–8 and unknown values 9 / `UInt32.max`. The new suite
  passes 1 test; the existing `CoreClientTests` suite passes 13 tests on root replay. Logs and hashes
  are in E244 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-core-error-numeric-codes-complete.md).
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. This test pins the
  Swift decoder against the C enum constants; it does not establish live ABI transport or engine
  behavior.
- Claim commit `a82fe6cdd8ac98709858f7c787d7c6085e77667e` and completion commit
  `0ae3bcabf96cd115fdaf40cf4aa79cef6e22794e` are pushed and fetch-verified. The completion commit's
  staged-path audit accepted 8 allowlisted paths and the full audit accepted 1,679 paths. Post-push
  `HEAD == origin/main` with 0/0 divergence; only user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain outside the commit. Audit/fetch/status logs are in
  `local/validation/app01-core-error-codes/` and the completion checkpoint.

## APP-01 SystemChecks 32-bit Mach-O refusal — fixture verified

- Root claimed `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, child of APP-01; the parent launcher
  scope is specified in [docs/TASKS.md](docs/TASKS.md#app-01--native-mac-launcher), and the platform
  target requires an ARM64 Mach-O in [docs/PLATFORM.md](docs/PLATFORM.md#1-required-product-boundary).
- `SystemChecks.isMachOExecutable` currently accepts both 64-bit and 32-bit magic values without
  examining CPU type; no matching architecture fixture existed. The bounded change removes acceptance
  of the 32-bit magic values and adds a synthetic i386-header classification fixture. It does not
  validate CPU type in 64-bit Mach-O or inspect FAT slices; do not treat it as full ARM64 validation.
- Baseline command `swift test --package-path launcher --scratch-path
  local/build/app01-systemchecks-arch --filter AssetValidatorTests` passes 11 tests. Log
  `local/validation/app01-systemchecks-arch/baseline.log`, SHA256
  `f1cef398f1013d2114ddec8afec668a9751bcb8962c097319ca54f5f41ce79bc`.
- The focused new suite passes 1 test and the existing `AssetValidatorTests` regression suite passes
  11 tests. The 32-bit magic values are now rejected; 64-bit and universal magic acceptance remains
  unchanged. Logs, hashes, graph checks, and limitations are in E245 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-systemchecks-reject-32bit-macho-complete.md).
- Status `FIXTURE_VERIFIED`, 1 successful implementation attempt, 0 failed tries. This closes only
  the 32-bit magic path; 64-bit CPU type and FAT slice validation remain unimplemented. No actual
  native core or game readiness is established.
- Claim commit `daa92c26f8972cadbe9f97e39669577b4c8daf62` and completion commit
  `ab4aeba25543e9a1e62ef85a5beae759419efa9d` are pushed and fetch-verified. The completion staged
  audit accepted 7 allowlisted paths and the full audit accepted 1,682 paths; post-push `HEAD ==
  origin/main` with 0/0 divergence. User-owned native API changes remain unstaged. Completion
  publication logs and hashes are in the completion checkpoint.
- This follow-on task is complete; see the audio-probe section at the top of this handoff.

## RE-02 app-main +0x20 virtual call — static path byte verified

- `/root/re02_vcall_25aa06` owns bounded task `RE-02-APPMAIN-VCALL-25AA06-01`; claim and
  baseline are in its [claim checkpoint](docs/checkpoints/2026-10-21-root-re02-appmain-vcall-25aa06-claim.md).
- Scope is only `0x25aa06` (`call qword ptr [RAX+0x20]`): verify the reaching RAX/object/vptr and
  slot bytes against the pinned ELF, then report a unique static target or verifier-backed
  unresolved edge. No runtime, class, ABI, or G1 claim; no native lift.
- Claim `866e7bf648976bf7f0f83943884c9df65d66922b` and its publication sync
  `414a0434e96885b5f028e9b2c7e9f18730955b3e` are pushed and fetch-verified; later first-task
  handoff sync `54854c9` was also pushed. The exact claim baseline output is recorded in the claim
  checkpoint. Parent-body verifier mapping failures remain assigned to the parent task.
- REA attempt 1 called `analyze_function(0x25a8d0)` and returned `provider_timeout` after
  330,000 ms at Ghidra 12.1.4 startup, run ID `1d30f13b-1bf7-4897-a088-b59b8e01e6d4`. The target
  SHA matched the pinned ELF. No REA records were produced; the available bundle was exported and
  the session closed. See the [timeout checkpoint](docs/checkpoints/2026-10-21-root-re02-appmain-vcall-25aa06-rea-timeout.md).
- The shared REA session is closed; root controls any further REA/Ghidra use. The worker continues
  offline and must not repeat the same profile. Task attempt count is 1.
- Worker `/root/re02_vcall_25aa06` completed an offline byte/mapping verifier. Root independently ran
  `python3 -I tools/lift/verify_appmain_vcall_25aa06.py`; exit 0 and stdout byte-matches the worker
  output (SHA-256 `00ed6182b87b9860cc1eee41f58cba7966b70dd660e32b113cfc789c752adf86`); stderr is
  empty. The verifier reports the constructor-linked table's `+0x20` word as `0x531d60`, while two
  other file-backed `+0x18` references to app-main have `0x255130` in the following `+0x20` word.
  The receiver's current table at the call is not established; edge remains unresolved.
- Bounded status is `STATIC_PATH_BYTE_VERIFIED`; no unique target, class, ABI, runtime, or G1 claim.
  The REA Ghidra startup timeout remains failed try 1; no same-profile retry was made. Full hashes,
  byte observations, limits, and replay paths are in the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-appmain-vcall-25aa06-complete.md).
- Completion record and verifier are pushed in `f9fc4302f1702af6677e90121a6040becc533d5e`,
  fetch-verified at `origin/main` with 0/0 divergence.
- The publication record sync is also pushed as `539a6bd1cfae0f3d75f13ac460d6c3ddf851734e` and
  fetch-verified.
- Exact next action: do not start a runtime-edge investigation without a separate claim and
  reassessment of REA use.

## RE-02 app-main +0x28 virtual call — static path byte verified

- Task `RE-02-APPMAIN-VCALL-25AC9F-01` is bounded complete at `STATIC_PATH_BYTE_VERIFIED`; claim
  details are in [the claim checkpoint](docs/checkpoints/2026-10-21-root-re02-appmain-vcall-25ac9f-claim.md)
  and verification/results in [the completion checkpoint](docs/checkpoints/2026-10-21-root-re02-appmain-vcall-25ac9f-complete.md).
- Worker `/root/re02_vcall_25ac9f` verified the call operand and reaching register sequence, plus a
  conditional constructor-table value. At `0x25ac9f`, bytes `ff 50 28` encode `call qword ptr
  [RAX+0x28]`; if the object still has constructor address point `0x15c8cd8`, file-backed slot
  `0x15c8d00` contains `0x5365b0`. The object first word at the call is not proven unchanged across
  prior calls, so the runtime call edge remains unresolved. No class, ABI, runtime, or G1 claim.
- Root independently ran `python3 -I tools/lift/verify_appmain_vcall_25ac9f.py`; exit 0 and stdout
  byte-matches the worker output (SHA-256 `f27559dd088cfdffe126db29e21d780ab3201dd6a31bbf7d8041e1f6f9a10e62`);
  stderr is empty. Verifier SHA-256 `c0454ec60e63604952f66fda100b99eee31934e5bf92b891b35bdae6e7c3e262`.
- Claim `7975c5c6f1d55e0efe7dc61031f8ff6d69cfe557` and documentation sync
  `9462efc0187ca14f417fd7a33b6cc4733b5474b8` are pushed and fetch-verified. Completion commit
  `ed15ff09811a51f5942dbd549404ddaf2c7af43e` is also pushed and fetch-verified at `origin/main`
  with 0/0 divergence. The shared REA session was not used by this worker; root controls any future
  REA operation.
- Attempt count: one bounded analysis, zero failed analysis hypotheses. Exact private report and
  probe paths, hashes, observations, and limits are in the completion checkpoint. G0 stays PARTIAL;
  G1–G6 stay NOT RUN.

## Round 77 fixtures verified (/root)

- All four claimed test-only tasks are `FIXTURE_VERIFIED`: all numeric `CoreState` values and unknown codes; exact diagnostic state text; PSARC unsupported entry-size rejection; ordered native ABI subsystem names. Evidence E233–E236, exact source/log hashes, commands, observations and limits are in the [Round77 completion checkpoint](docs/checkpoints/2026-10-21-root-round77-fixtures-complete.md); claim baselines are in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round77-fixture-claims.md).
- Sole test files and owners: `/root/launcher_review` — `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`; `/root/scout_launcher_queue` — `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`; `/root` — `scripts/test_psarc.py`; `/root/round68_native_scout` — `native/api/tests/test_sotc_core_api.cpp`. Worker and root checks pass: 22 CoreClient tests across 2 suites, 6 CoreReadiness tests, 39 PSARC tests, and native target build plus focused CTest 1/1. Each task has 1 successful implementation attempt and 0 failed tries; production sources are unchanged.
- These fixtures assert pure Swift mappings/formatter output, synthetic PSARC headers, and the current stub ABI report. They prove no launcher UI, live archive, native game core, or game behavior. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- Project folder index is current at 19,268 folders; Trail's wiring graph is current at 3,580 nodes. The deep semantic layer remains unbuilt; exact check logs are in the Round77 completion checkpoint.
- The native API check used the existing user-owned `native/api/CMakeLists.txt` (SHA256 `aa04d19b5fde7c32ae4f30d28bcf781f0a65a337c75c96ad63c29e320bb402de`). That file and untracked `native/api/host/` remain untouched and excluded from publication.
- Claim commit `84706a333356aab9d45b3c8355b15835da96a70c` and publication-record sync `cb45ea030424882f1fd924323dd7fecc4e139f97` are pushed and fetch-verified. Round77 completion is published in `b6ada6cb27bb858a5b935ed667516078214eaf2f`; push succeeded and a subsequent fetch confirmed `HEAD == origin/main` with 0/0 divergence. The preflight and push logs are in the [Round77 completion checkpoint](docs/checkpoints/2026-10-21-root-round77-fixtures-complete.md).
- Exact next action: select the next independent bounded task from `docs/TASKS.md` using the live source graph, publish its claim before implementation, and dispatch separate files to available workers.

## Round 76 fixtures verified (/root)

- All three claimed test-only tasks are `FIXTURE_VERIFIED`: empty-subsystem diagnostics text, malformed partial-width PSARC table rejection, and exact maximum save-stream file-size boundary. Evidence E230–E232 records the changes, worker/root results, hashes, and limitations in the [Round76 completion checkpoint](docs/checkpoints/2026-10-21-root-round76-fixtures-complete.md); pre-change baselines and claims are in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round76-fixture-claims.md).
- Owners and sole test files: `/root/launcher_review` — `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`; `/root/review_host_proto` — `scripts/test_psarc.py`; `/root/round68_native_scout` — `native/tests/test_save_stream_file.cpp`. Worker and root checks each pass: 5 `CoreReadinessTests`, 38 PSARC tests, and native target build plus focused CTest 1/1. Each task has one successful implementation attempt and zero failed tries; production hashes are unchanged.
- The batch is synthetic-only: formatter output, malformed synthetic TOC width, and an invalid sparse zero-version file that passes the exact size boundary. The last fixture reads about 100 MiB. No game save, real archive, launcher UI, or game behavior is claimed. G0 remains PARTIAL; G1–G6 remain NOT RUN. Project index is current at 14,475 folders; Trail wiring is synced at 3,578 nodes. The graph cache is local and ignored.
- Round76 completion is published as `7bc6dff58aae425db3b5ffcdafcc1fae143e40a0`; `git push origin main` succeeded and a subsequent `git fetch origin main` confirmed `HEAD == origin/main` with 0/0 divergence. Preflight accepted 9 staged paths and 1,657 full-tree paths; details are in the [Round76 completion checkpoint](docs/checkpoints/2026-10-21-root-round76-fixtures-complete.md).
- Claim commit `ec88e14f829f2079758a7873a38bc468d081660f` is pushed and fetch-verified at `origin/main`; publication and dispatch evidence is in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round76-fixture-claims.md). Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.
- Exact next action: finish the four claimed Round77 fixtures, independently replay them, then update the project index and publish allowlisted changes.

## Round 75 fixtures verified (/root)

- All three claimed test-only tasks are `FIXTURE_VERIFIED`: reject a one-byte synthetic zlib result at offset zero, assert the exact launcher script-stub refusal copy, and reject preserved 3 MiB host-map reserve alignment. Evidence E227–E229, exact hashes/commands/logs, observations, and scope boundaries are in the [Round75 completion checkpoint](docs/checkpoints/2026-10-21-root-round75-fixtures-complete.md); claim and pre-change baselines are in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round75-fixture-claims.md).
- Owners and sole files: `/root/review_host_proto` — `scripts/test_audio_headers.py`; `/root/launcher_review` — `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`; `/root/round68_native_scout` — `native/tests/test_host_map.cpp`. Worker and root checks pass: 25 audio tests, 11 `AssetValidatorTests`, native target build exit 0 and focused CTest 1/1. Each task has one successful implementation attempt and zero failed tries; production hashes are unchanged.
- The completion batch remains synthetic-only: no codec/playback, launcher runtime, successful mapping, allocator integration, or game behavior is claimed. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- Claim commit `f931b7925fc23d7fb9e2230073f9a40610ffe6c7` is pushed and fetch-verified. Claim publication recorded in `682477a4198c47fd754e942a3e18a675c81bdf93`; completion commit `0eb59a9aed89f95d61042ae27a441437147798f0` is also pushed and verified with `HEAD == origin/main`, divergence 0/0. Staged/full publication audits pass at 8/1,655 paths; indexes report 14,470 folders and a synced 3,576-node Trail wiring graph. Details are in the [completion checkpoint](docs/checkpoints/2026-10-21-root-round75-fixtures-complete.md).
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded. Next exact action: scout the next independent bounded tasks from `docs/TASKS.md` and the live Trail graph, check for duplicates, publish claims before editing, then continue with isolated worker ownership.

## Round 74 fixtures verified (/root)

- All five claimed test-only tasks are now `FIXTURE_VERIFIED`: PSARC TOC upper bound, relocated
  constructor sentinel, ordered launcher diagnostics failures, `.io`/`.badState` failure copy, and
  singleton region-list unlink. Evidence E222–E226 records focused results and scope limits; logs,
  hashes, exact commands, and observations are in the [Round74 completion checkpoint](docs/checkpoints/2026-10-21-root-round74-fixtures-complete.md).
- Owners: `/root/review_host_proto` owns the PSARC and dynlink test files; `/root/launcher_review`
  owns the two distinct launcher test files; `/root/round68_native_scout` owns only
  `native/tests/test_region_list.cpp`. `/root` remains sole integrator for shared docs.
- Worker and root results: PSARC 37 tests; dynlink 27; CoreReadiness 4; CoreClient 22 (9
  native-client + 13 core-client); native `test_region_list` build exit 0 and focused CTest 1/1.
  Each task has one successful attempt and zero failed tries. Production files are unchanged.
- G0 remains PARTIAL; G1–G6 remain NOT RUN. Index checks passed at 14,468 folders and 386 checkpoint
  documents; the Trail wiring graph is synced at 3,575 nodes. Staged/full publication audits pass
  for 10 staged and 1,653 total paths; HEAD and `origin/main` match at `8bea844`. Full logs are in
  the completion checkpoint. Existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain untouched and excluded. Next exact action: commit/push only the five test
  files and allowlisted documentation, verify the remote tip, then scout the next independent task.
- Claim commit `820bb128adfb3c9a03271c33a704a3ae2682aed1` (`docs: claim Round 74 fixture tasks`) and
  publication-record sync `8bea84495265e2f1d504221f2831c9a2527659fd` (`docs: record Round 74 claim
  publication`) are pushed to `origin/main`; fetch verification is captured in the
  [Round74 claim checkpoint](docs/checkpoints/2026-10-21-root-round74-fixture-claims.md).
- Completion commit `9c0d96c9a23fd3b452f678ed0e47f85666586606` (`test: cover Round 74 fixture gaps`)
  is pushed and fetch-verified at `origin/main`; `HEAD == origin/main`, divergence `0 0`. Push and
  post-push evidence is recorded in the [Round74 completion checkpoint](docs/checkpoints/2026-10-21-root-round74-fixtures-complete.md).
- Next exact action: use workers to scout the next independent bounded tasks from `docs/TASKS.md`
  and the live Trail graph, verify no duplicate task, claim in HANDOFF/TASKS/checkpoint, then assign
  file-isolated work.

## Round 73 fixtures verified (/root)

- Three claimed test-only tasks are now `FIXTURE_VERIFIED`: synthetic `DT_SCE_EXPORT_LIB` metadata,
  exact launcher copy for ABI mismatch, and zero-lead splitter rejection with byte-preservation.
  Evidence E219–E221 and worker/root logs, hashes, scope limits, and observations are in the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-round73-fixtures-complete.md); claim and
  dispatch history is in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round73-fixture-claims.md).
- Claim checkpoint and task rows are published in `01e683bcc2e172e409f9d65fd06f6e54ff6f8acf`
  (`docs: claim Round 73 fixture tasks`); after fetch, `HEAD == origin/main`, divergence `0 0`.
  The publication checks and log hashes are recorded in the claim checkpoint.
- The claim publication record is synced in `20683aae7cb0e99a14e0358a5580d295f68e1676`
  (`docs: record Round 73 claim publication`), also pushed and verified at `origin/main`.
- Pre-dispatch checkpoint `a1fa566838fbeed6caa71dc53eed499302cbc3a0` is also pushed. Worker and root runs pass 26 dynlink tests,
  22 launcher tests, and native target build plus focused CTest 1/1. All implementation changes are
  limited to those three test files; production hashes are unchanged.
- Completion commit `e5d3e40c308679a356998e506441eba2c013e17a` (`test: verify Round 73 export, ABI,
  and split fixtures`) is pushed and verified at `origin/main`; `HEAD == origin/main`, divergence
  `0 0`. Post-push evidence is recorded in the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-round73-fixtures-complete.md).
- Publication-record sync commit `4e7d0fc84a491b746b0efe54a2ec87def8903790` (`docs: record Round 73
  publication`) is also pushed and verified at `origin/main`; its fetch, state, and index evidence
  is in the same completion checkpoint.
- Owners are `/root + review_host_proto` on `scripts/test_ps4_dynlink.py`, `/root + launcher_review`
  on `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`, and `/root + round68_native_scout` on
  `native/tests/test_region_split.cpp`. `/root` remains sole integrator for shared docs.
- Each task has 1 successful implementation attempt and 0 failed tries. These synthetic fixtures do
  not establish loader behavior, live ABI handling, allocator integration, or any game gate. G0
  remains PARTIAL; G1–G6 remain NOT RUN. Next exact action: inspect `docs/TASKS.md` for independent
  bounded fixtures, query relevant source through Graft, then publish Round74 claims before
  delegating implementation. User-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.

## Round 72 fixtures verified (/root)

- Claimed three independent, test-only tasks: reading an empty synthetic PSARC member, including
  `.stopping` in launcher core diagnostics, and rejecting a carve lead larger than the free region.
  Claim-time code/test identities and passing root baselines are in the
  [claim checkpoint](docs/checkpoints/2026-10-21-root-round72-fixture-claims.md).
- Owners: `/root + review_host_proto` owns only `scripts/test_psarc.py`; `/root + launcher_review`
  owns only `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`; `/root + round68_native_scout`
  owns only `native/tests/test_free_list_carve.cpp`. `/root` remains sole integrator for shared docs.
- Three tasks are `FIXTURE_VERIFIED` with one successful implementation attempt and zero failed
  tries. E216–E218 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-round72-fixtures-complete.md)
  record the exact worker/root commands, log hashes, production/test hashes, observations, and limits.
  Each edit is confined to its claimed test file; production files are unchanged.
- Worker and root checks pass: 36 PSARC tests, 3 `CoreReadinessTests`, and the free-list-carve
  target build plus focused CTest 1/1. The fixtures cover an empty synthetic archive member, a
  synthetic diagnostic formatter value, and pure carve-fit arithmetic only. They do not establish
  real archive behavior, runtime diagnostics, allocator integration, or any game acceptance gate.
- Claim commit `df1af7d60f48c4b51222f6fb7705f93fa7eaae1c` and docs sync commit
  `963adb58cfbaab654d1d11b663cbfb6405768073` are pushed; the claim and completion checkpoints
  record the claim publication. User-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain untouched and excluded.
- Folder index is current at 14,467 folders. Trail was refreshed to 3,572 nodes / 4,837 edges /
  943 cards; `trail build --check` passes. An initial folder-index check was stale because a new
  private log path was created after generation; precreating the log paths and regenerating made
  the check current. The attempted fixtures themselves had no failed runs. Completion commit
  `be687574e57b6626851406fc86fea5508b90a026` (`test: verify Round 72 fixtures`) is pushed; after
  fetch, `HEAD == origin/main`, divergence 0/0. Only the pre-existing user-owned `native/api`
  changes remain outside the commit. Next exact action: scout the next independent bounded fixture
  tasks and publish claims before implementation.

## Round 71 fixtures verified (/root)

- Three claimed tasks are now `FIXTURE_VERIFIED`: exact launcher copy for a missing core path,
  parsing repeated synthetic `DT_SCE_NEEDED_MODULE` records, and region-split rejection when lead
  exceeds node size with both nodes preserved. Evidence E213–E215 records hashes, worker/root logs,
  and scope limits; claim and completion checkpoints carry the detailed records.
- Worker and root checks pass: 11 launcher tests, 26 dynlink tests, and region-split build plus
  focused CTest 1/1. Each edit is confined to its assigned test file; production hashes are
  unchanged. The tests cover temporary filesystem classification/copy, synthetic ELF parser list
  handling, and synthetic helper node arithmetic only.
- These fixtures establish no loader/runtime behavior, original allocator integration, core
  execution, game boot, authentic audio, save/reload, or Metal gameplay. G0 remains PARTIAL;
  G1–G6 remain NOT RUN.
- Claim commit `bb1f58628a85dbea0331e8195d792a49193d02d8` and claim-doc sync
  `3a866fa106f43d7cc4bff3ec46fc1dac3cd6cbcc` are pushed; completion publication is recorded in
  the [completion checkpoint](docs/checkpoints/2026-10-21-root-round71-fixtures-complete.md).
- Completion commit `6e517b91a849c48a2775edce346c9539abb8c7e6` (`test: verify Round 71 launcher,
  dynlink, and split fixtures`) is pushed to `origin/main`; `HEAD == origin/main`, divergence `0 0`.
  Only the pre-existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/`
  remain in the worktree and are excluded.
- Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and
  excluded. Next exact action after publication: identify additional independent bounded tasks;
  do not change any acceptance gate based on these fixtures.

## Round 70 fixtures verified (/root)

- The three published Round70 claims are now `FIXTURE_VERIFIED` in `docs/TASKS.md`; E210–E212 and
  the [completion checkpoint](docs/checkpoints/2026-10-21-root-round70-fixtures-complete.md)
  record exact commands, hashes, logs, and limits. Root reviewed each diff and independently
  replayed all three checks.
- `DOC-FOLDER-INDEX-CHECK-DIRECTORY-OUTPUT-01` changes check mode to report a directory at an
  output path as stale. The focused project-tools suite passes 10 tests. The first root run failed
  because the new test expected the public docs index to stale when the private `local/` JSON path
  became a directory; the docs renderer intentionally omits nested private paths. The corrected
  expectation passes and the indexer no longer raises `IsADirectoryError`.
- `RE-02-DYNLINK-SPAN-NEGATIVE-OFFSET-SIZE-FIXTURE-01` pins exact errors for a negative offset and
  a negative length; worker and root each pass 25 tests. The task
  `APP-01-PRESENTATION-IDLE-HINT-ABSENCE-FIXTURE-01` asserts the idle launcher presentation has no
  recovery hint; worker and root each pass all 3 focused presentation tests.
- These fixtures establish temporary-root index reporting, synthetic byte-span validation, and a
  pure launcher presentation-model value only. They do not establish ELF loading, game startup,
  runtime UI behavior, actual game audio, save/reload, or Metal rendering. G0 remains PARTIAL;
  G1–G6 remain NOT RUN. No install or dependency facts changed.
- The folder index check passes at 14,465 folders. Trail wiring graph is current at 3,570 nodes,
  4,832 edges, and 943 cards; `trail build --check` passes. Its optional meaning layer remains
  unbuilt.
- Claim commit `099136c56e4de5101b97112d634b5d86864c7425` and claim documentation sync
  `bf96db6ebbaa897b9916f7b4bd5ebd4d57d634ed` are pushed to `origin/main`. Completion commit
  `5a2e8341d9432639609c18f639f49cb1d1155106` (`fix: handle directory index outputs and add Round70
  fixtures`) is pushed. After `git fetch origin`, `HEAD == origin/main` and divergence is `0 0`;
  state log SHA-256 `ab55f80110225d059f77f72e00d59d066a799f1cc7516c25a2945d2c6fbe6f28` is in the
  completion checkpoint.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched
  and excluded.
- Those native/API edits are the only remaining worktree changes and are excluded from the Round70
  completion commit.
- Exact next action: use read-only scouts and the live task register to identify independent
  Round71 candidates; verify their contracts before publishing bounded claims.

## Round 69 fixtures verified (/root)

- Three test-only tasks are recorded as `FIXTURE_VERIFIED` in `docs/TASKS.md` with evidence E207–E209.
  Workers changed only their assigned test files; root reviewed all three diffs and independently
  replayed each focused command. Results are 11 Swift tests across 2 suites, 24 dynlink tests, and
  36 PSARC tests. Hashes, logs, scope limits, and observations are in the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-round69-fixtures-complete.md).
- Claim commit `4fabeb0ebbc759a3d9bec424e8f39d48893fc83e` and claim documentation sync
  `e669b7c35dd0fa155e962a57957a5da686be87e2` are pushed. The tasks were dispatched only after the
  claim commit was public.
- These fixtures cover pure lifecycle classification, a synthetic ELF identity guard, and synthetic
  PSARC prefix behavior. They establish no live child-process timing, ELF loading, real archive data,
  game behavior, or acceptance gate. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- The invalid-UTF-8 PSARC exception preference remains pending and is excluded. Native scouting
  found no nonredundant region-list case; a nonaligned free-list edge was deferred because E164
  covers related arithmetic and no production caller establishes that direct input contract.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched
  and excluded. `INSTALL_LOG.md` did not change.
- Project index is current at 14,464 folders. Trail was refreshed to 3,568 nodes / 4,829 edges /
  943 cards, and `trail build --check` passes; the optional meaning layer remains unbuilt.
- Completion commit `9e38191f70c2a3e5f44f2d1c3daf9676378a1593` (`test: verify Round 69 lifecycle and
  parser fixtures`) is pushed. After `git fetch origin`, `HEAD == origin/main` and divergence is
  `0 0`; push and state log hashes are in the completion checkpoint.
- Exact next action: scout Round 70 with Graft and the live task register, then publish bounded
  claims before implementation.

## Round 67 fixtures verified (/root)

- Five bounded test-only fixtures are recorded in `docs/TASKS.md` and `docs/EVIDENCE.md` (E199–E203): two launcher assertions, one zero-byte synthetic manifest record, one direct zero-size host-map helper assertion, and one project-index stale-output identification case. All worker and root checks passed; task rows are `FIXTURE_VERIFIED` with one successful implementation attempt and zero failed tries. Commands, source/test hashes, logs, limits, and observations are in the [completion checkpoint](docs/checkpoints/2026-10-21-root-round67-fixtures-complete.md). The earlier claim and dispatch commits remain recorded there and in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round67-fixture-claims.md).
- The launcher focused runs pass 8 and 11 tests; the archive-overlap parser passes 24; host-map target builds exit 0 with focused CTest 1/1; project-tools passes 9. Each is a bounded fixture only and establishes no native-game behavior.
- Source graph refreshed with scopes `scripts`, `tools`, `reference`, `native`, `launcher`, and `tests`: 3,566 nodes / 4,824 edges; `trail build --check` passes. The project index refresh and check results are recorded in the completion checkpoint. Optional meaning layer remains unbuilt.
- This fixture batch does not advance game gates: G0 remains PARTIAL; G1–G6 remain NOT RUN. No game executable, cold boot, actual game audio, real save/reload, or Metal gameplay is demonstrated.
- Completion commit `393d26e96e46ed1691928dc23855c7095d15487a` is pushed; after the push, `HEAD == origin/main` at that commit. Push and state logs with hashes are in the completion checkpoint.
- Documentation sync commit `d86d5db6fd67a114cc2054a078b72ac1c6d60fa8` is also pushed; its combined push/state log `local/validation/root-round66-scout/round67-sync-push.log` (SHA-256 `6d578babaf8f510af1baab27a848d2255c0d75b270c94d56f4d7acd2c1c6ffb4`) verifies `HEAD == origin/main` at that commit.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded from this publication.
- Exact next action: scout and claim the next bounded batch from the live repository before implementation.

## Round 68 fixtures verified (/root)

- Three test-only tasks are `FIXTURE_VERIFIED` with one passing implementation attempt and zero
  failed tries. Workers changed only their assigned test files; root reviewed each diff and
  independently replayed the focused command. Evidence E204–E206 and full source/test/log hashes
  are in the [completion checkpoint](docs/checkpoints/2026-10-21-root-round68-fixtures-complete.md).
- The claim was published before worker edits in `7afe5fcb699137eaff4c557ba30151b5231234d0`.
  Dispatch and post-push refs are recorded in the [claim checkpoint](docs/checkpoints/2026-10-21-root-round68-fixture-claims.md).
- Verification commit `6e6db1d4b8d9bcae9d778d099ee22dcef3aa683c` is pushed; after fetch,
  `HEAD == origin/main` and divergence is zero. Only the pre-existing user-owned `native/api`
  changes remain outside the commit. Post-push logs are in the completion checkpoint.
- The fixtures cover a pure Swift lifecycle value, a synthetic ELF table guard, and synthetic
  host-map cleanup state. They do not establish live process behavior, ELF loading, allocator
  integration, game execution, or an acceptance gate. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- `python3 scripts/project_index.py` and `--check` pass at 14,463 folders. The configured wiring
  graph was refreshed to 3,567 nodes / 4,826 edges / 943 cards and `trail build --check` passes;
  the optional deep meaning layer remains unbuilt. The first graph check correctly reported the
  three edited test files as stale; both outcomes are retained in the completion logs.
- Pre-push checks pass: `git diff --cached --check` and `git diff --check` exit 0; the staged audit
  accepts 9 allowlisted paths and the full audit accepts 1,641 paths. `HEAD` and `origin/main` both
  remain at the published claim commit before this completion push. Log hashes are in the
  completion checkpoint.
- The separate PSARC invalid-UTF-8 candidate remains unclaimed pending the user's choice between
  `PsarcError` normalization and preserving `UnicodeDecodeError`.
- Exact next action: scout independent Round69 candidates, preserve the pending PSARC preference,
  and publish any new bounded claims before implementation.

## Round 66 bounded fixtures verified (/root)

- Round65 fixture work and Round66 claims are published. Round66 claim commit
  `9bb85cc4299134fb230165a70891dfc135034aa1` is pushed; the post-claim docs record at
  `273b4419007c1e0fc16fc9caca1a4c4414afe7ff` was the starting `HEAD == origin/main` for this batch.
- All three workers changed only their claimed existing test files. Root reviewed each exact diff,
  replayed every focused check, and confirmed unchanged production hashes. APP-01 passes 10 Swift
  tests, RE-02 passes 23 dynlink tests, and SRV-01's target builds and focused CTest pass 1/1.
  Details, hashes, commands, logs, and limits are in [the Round66 claim checkpoint](docs/checkpoints/2026-10-21-root-round66-fixture-claims.md),
  [completion checkpoint](docs/checkpoints/2026-10-21-root-round66-fixtures-complete.md), and E196–E198.
- Each task is `FIXTURE_VERIFIED`, with one successful implementation attempt and zero failed
  tries. These are synthetic/unit fixtures; they do not advance the game acceptance gates. G0
  remains PARTIAL; G1–G6 remain NOT RUN.
- Source graph refreshed with scopes `scripts`, `tools`, `reference`, `native`, `launcher`, and
  `tests`: 3,564 nodes / 4,820 edges; `trail build --check` passes. Optional meaning layer remains
  unbuilt. `python3 scripts/project_index.py` and its `--check` now pass at 14,458 folders; the
  initial stale JSON report and subsequent clean refresh are recorded in the completion checkpoint.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched,
  unstaged, and excluded.
- Fetch and remote comparison passed (`HEAD...origin/main` = `0 0`). Staged and full publish audits
  passed for 9 and 1,637 paths, respectively. `git diff --cached --check` and `git diff --check`
  passed; logs and hashes are in the completion checkpoint.
- Completion commit `ed97b75ab47b656abf3e42861e341c2486a3409a`, docs-only publication-record commit
  `6184e32941ef791366f9162f79c76cc96bee8a00`, and final docs sync commit
  `11a904ec2dfcc8aaeb1061b23db4f721a7272046` are pushed. After the last push,
  `HEAD == origin/main == 11a904ec2dfcc8aaeb1061b23db4f721a7272046`; only the pre-existing
  user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain. Logs are in the
  completion checkpoint.
- Exact next action: start fresh Round67 scouting with Graft and the live repository; claim and
  publish disjoint work before dispatching implementation.

## Round 65 bounded test-only fixtures verified (/root)

- The claim checkpoint was committed as `756e9f4cd2ddfa2a2ff2cc39915f3f705651a3ef` and pushed
  before implementation prompts. The three workers changed only their assigned existing test
  files; root reviewed the exact diffs, verified unchanged production hashes, and independently
  replayed all focused commands. Full logs, hashes, exact outcomes, and limitations are in
  [the Round 65 completion checkpoint](docs/checkpoints/2026-10-21-root-round65-fixtures-complete.md).
- APP-01 pins ABI minor ordering with equal major values (worker and root: 22 Swift tests across
  two suites); DATA-02 pins a terminal NUL in a synthetic PSARC manifest (35 Python tests each);
  SRV-01 pins a non-wrapping region end at `UINT64_MAX` (target builds exit 0; focused CTest 1/1).
  Evidence E193–E195 records changed-test and unchanged-production hashes plus worker/root log hashes.
- Each task is `FIXTURE_VERIFIED`, with 0 failed tries and 1 passing implementation attempt.
  These checks cover pure Swift ordering, synthetic archive path parsing, and endpoint arithmetic;
  they establish no archive convention, memory mapping, or game behavior.
- `trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference
  --only-dir native --only-dir launcher --only-dir tests` exited 0 with 3,562 nodes / 4,816 edges;
  `trail build --check` passed. The optional meaning layer remains unbuilt. The new assertions are
  queryable in all three files. `python3 scripts/project_index.py` and `--check` results are in the
  completion checkpoint.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched
  and excluded. No installation or dependency facts changed. G0 remains PARTIAL; G1–G6 remain NOT
  RUN.
- Completion commit `061d8d530d4a84213372d03289e72535f9be6102` (`test: cover Round65 launcher,
  archive, allocator fixtures`) is pushed to `origin/main`; push log and the state showing
  `HEAD == origin/main` are recorded in the [completion checkpoint](docs/checkpoints/2026-10-21-root-round65-fixtures-complete.md).
- Exact next action: review the read-only Round66 scouting results, verify candidate source behavior
  and prerequisites with Graft/live files, then claim and publish bounded independent tasks before
  assigning implementation.

## Round 64 test-only fixtures verified (/root)

- Claim commit `218ce2134fa8e1af05c6e5e6e419cb30ceaa7310` was published before worker edits. The
  handoff/index finalization commit `7593e52cc87b413b968f2ee75aa81e58bad30cf3` is also on
  `origin/main`. Exact claims and dispatch are in
  [the Round 64 claim checkpoint](docs/checkpoints/2026-10-21-root-round64-fixture-claims.md).
- Completion commit `941011b08a3bb445747c8a49385bbba0b2ad982a` is pushed to `origin/main`.
  Post-push state is recorded in the completion checkpoint; this final docs-only update records
  that publication and moves the next action to Round65 scouting.
- APP-01 pins the exact failed-state copy; DATA-02 parses a synthetic final JSONL record without a
  newline; SRV-01 pins host-map alignment just above 1 MiB. Root reviewed each one-file diff and
  independently replayed every focused command. Worker and root results pass; E190–E192 and
  evidence limits are in
  [the Round 64 completion checkpoint](docs/checkpoints/2026-10-21-root-round64-fixtures-complete.md).
- `trail build` with scopes `scripts`, `tools`, `reference`, `native`, `launcher`, and `tests`
  reports 3,561 nodes / 4,813 edges; `trail build --check` passes. Graft resolves the Python and
  Swift fixtures and `trail grep "0x100001u"` resolves the native test at line 22. The optional
  meaning layer remains unbuilt. Project indexing is refreshed before publication.
- All three tasks are `FIXTURE_VERIFIED`, 0 failed tries. They establish synthetic test behavior
  only. G0 remains PARTIAL; G1–G6 remain NOT RUN. Existing `native/api/CMakeLists.txt` changes and
  untracked `native/api/host/` remain user-owned and excluded.
- Round 65 claims above supersede this scouting action.

## Round 63 test-only tasks verified (/root)

- Three read-only scouts identified distinct synthetic fixtures; root verified each candidate against
  live production and test files and ran the focused baselines. Claims, exact assertions, digests,
  commands, log hashes, and evidence limits are in
  [the Round 63 claim checkpoint](docs/checkpoints/2026-10-21-root-round63-fixture-claims.md).
- APP-01 asserts that an explicit failed-status message overrides the mapped error fallback
  (worker and root: 22 Swift tests across two suites). DATA-02 combines compressed and stored blocks
  in one 32-byte synthetic PSARC member (worker and root: 34 Python tests). SRV-01 pins an exact
  post-alignment remainder fit in the free-list carve helper (worker and root target build; CTest
  1/1).
- Owners are `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs`; each
  may edit only its claimed existing test file and may not commit. Root owns shared docs and replay.
- Claims were published before implementation prompts in commit
  `3a79b5318beb437011384ca6c250c5f56b12ead9`; `HEAD` and `origin/main` matched. Push log
  `local/validation/root-round63-claims/push-claims.log` SHA-256
  `9ac94a0929fcf694f749680b1a93f761736a99497aaabeb959b59e04fdd4417d`; publication-state log
  SHA-256 `009237c394683aedd262a2d22db3a1f964cd5af54f9ace71df27380705292b91`.
- At 14:24 CDT, prompts were sent to all three owners. Worker attempts passed: APP 22 tests across
  2 suites, DATA-02 34 parser tests, and SRV-01 target build plus CTest 1/1. Worker logs, test and
  production hashes are in
  [the Round 63 dispatch checkpoint](docs/checkpoints/2026-10-21-root-round63-workers-dispatched.md).
- Root reviewed each single-file diff and independently replayed every focused command. The three
  tasks are `FIXTURE_VERIFIED`, with 0 failed attempts; source/test hashes, worker/root log hashes,
  exact results and limitations are in
  [the Round 63 completion checkpoint](docs/checkpoints/2026-10-21-root-round63-fixtures-complete.md)
  and evidence E187–E189. `trail build` and `trail build --check` pass; optional meaning layer stays
  unbuilt. Synthetic fixtures do not advance G0–G6 runtime gates. Existing user-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.
- Completion commit `a6e5afa08d33133804768fe4f10804bd2a7b603a` is pushed to `origin/main`; post-push
  `HEAD` and `origin/main` match. Push log and state log hashes are in the completion checkpoint.
- Round 64 claims above supersede this earlier scouting action.

## Session addendum 2026-10-21 Round 62 fixtures verified (/root)

- The three test-only Round 62 claims were published in `2c30005` before implementation prompts.
  `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` each changed only
  its assigned test file and reported 0 failed attempts. Root reviewed the diffs and replayed all
  focused commands; full evidence, digests, and limitations are in
  [the Round 62 completion checkpoint](docs/checkpoints/2026-10-21-root-round62-fixtures-complete.md).
- APP-01 pins the exact `.notBuilt` message for a directory at the core executable path; worker and
  root each pass 9 tests. AUD-01 pins the wrapped synthetic PSARC read error and reader cleanup;
  worker and root each pass 20 tests. SRV-01 pins rounding of `0x10001` to `0x20000`; worker and
  root builds pass and focused CTest is 1/1. Production sources match their claim-time hashes.
- Worker logs remain under `local/validation/{app01-system-checks-core-directory-path,
  aud01-xff-psarc-read-failure-cleanup,srv01-allocator-size-align-second-64k-boundary}/`; root replay
  logs are under `local/validation/root-round62-replay/`. E184–E186 record both log hashes and
  synthetic-only limits. No dependency or install facts changed.
- The fixtures do not advance runtime gates: G0 remains PARTIAL; G1–G6 remain NOT RUN. No native
  boot, actual game audio, save/reload, or Metal gameplay is established.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched
  and excluded from publication.
- Completion commit `40a08252cee4d16bec4a1ff57df6da72b3a67334` is pushed to `origin/main`; `HEAD` and
  `origin/main` match. Push log `local/validation/root-round62-replay/push-main.log` SHA-256
  `af03d97be76c36f17610d49f8bcb547944e43fc3965d55bc9805284aaa9c622c`; publication-state log
  SHA-256 `5fb39cee8eed007120abb6138bcac9c9fb1ddc45e98f9869b0316573ed58c1fc`.
- The Round 62 completion checkpoint and published commit are linked above in the session history.

## Session addendum 2026-10-21 Round 61 fixtures verified (/root)

- Three independent test-only tasks were claimed in
  [the Round 61 claim checkpoint](docs/checkpoints/2026-10-21-root-round61-fixture-claims.md),
  assigned to `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs`, and
  completed with 0 failed fixture attempts each. The root reviewed each single-file diff and
  independently replayed all three acceptance commands. Detailed hashes and limits are in
  [the Round 61 completion checkpoint](docs/checkpoints/2026-10-21-root-round61-fixtures-complete.md).
- APP-01 asserts max major/minor fields pack to `UInt32.max` and round-trip; worker and root each
  pass 22 tests across `CoreClientTests` (13) and `NativeCoreClientTests` (9). AUD-01 verifies a
  synthetic one-byte LZMA result is rejected; worker and root each pass 24 tests. SRV-01 verifies
  `lead == free.size()` rejects without changing either raw node; worker/root build exits 0 and
  focused CTest passes 1/1. All three production files match their claim-time hashes.
- Root replay logs are in `local/validation/root-round61-replay/`; worker logs remain in the
  task-specific `local/validation/` folders. E181–E183 record test and production hashes, both log
  hashes, exact observations, and synthetic-only limits. `docs/TASKS.md` marks all three
  `FIXTURE_VERIFIED`; `INSTALL_LOG.md` has no new facts.
- `trail build` exited 0 at 3,557 nodes / 4,804 edges; `trail build --check` exited 0. Logs are
  `local/validation/root-round61-replay/trail-build.log` and `trail-check.log`. Only the wiring
  graph is current; its optional meaning layer remains unbuilt. `docs/FOLDERS.md` and the local
  project index are regenerated at this checkpoint.
- `python3 scripts/project_index.py` indexed 14,421 folders and `--check` now reports current;
  `docs/FOLDERS.md` lists 358 checkpoint files. The first two checks were stale because their new
  log files were created inside the indexed folder after generation; final precreated-log results
  and hashes are in the completion checkpoint.
- Claim commit `4cbd478180cb983c52b711e8a69cca2e732ea4f6` and assignment checkpoint commit
  `0d4aa90` are already pushed. Round61 completion audit accepts 10 staged paths and 1,625 full
  paths; cached and working diff checks pass. Exact audit hashes are in the completion checkpoint.
  User-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and
  excluded.
- Synthetic fixture results do not advance game gates: G0 remains PARTIAL; G1–G6 remain NOT RUN.
  No runtime behavior, real asset classification, C ABI interoperability, allocator integration,
  cold boot, actual game audio, save/reload, or Metal gameplay is established.
- Completion commit `741174232d44236e7c674f92c106992d6613e0e9` (`test: verify round 61 synthetic
  fixtures`) is pushed to `origin/main`; `HEAD` and `origin/main` match. Push and ref log hashes are
  in the completion checkpoint. The working tree now contains only the pre-existing, excluded
  `native/api` changes.
- Exact next action: use Graft to scout independent Round 62 tasks, verify their candidates and
  baselines against live files, then publish claims before implementation edits.

## Session addendum 2026-10-21 Round 60 fixtures verified (/root)

- Root reviewed and replayed all three single-file test changes after worker completion. APP-01
  symlink-to-directory fixture passes 7 tests in one suite; AUD-01 LZMA offset-4 fixture passes 23
  tests; SRV-01 uncounted region-list unlink builds and passes focused CTest 1/1. Production sources
  match claim-time hashes; failed fixture attempts remain 0. Evidence E178–E180 and exact logs,
  hashes, commands and limits are in
  [the Round 60 completion checkpoint](docs/checkpoints/2026-10-21-root-round60-fixtures-complete.md).
- `trail build` exited 0 and refreshed the wiring graph to 3,555 nodes / 4,801 edges;
  `trail build --check` exited 0. Only the wiring tier is in scope; the optional deep concept layer
  remains unbuilt. The dispatch checkpoint records the earlier stale check while workers were
  editing.
- User-owned `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and excluded.
  Synthetic tests do not advance game gates: G0 stays PARTIAL and G1–G6 stay NOT RUN.
- `python3 scripts/project_index.py` exited 0 and indexed 14,416 folders; `docs/FOLDERS.md` lists
  355 checkpoint files. Log `local/validation/root-round60-replay/project-index.log`, SHA-256
  `07954259c59a360103bc473829a9fef45d72b51c242fc87544c76506b1d3e44f`.
- Pre-push audit accepted 8 staged paths and 1,622 full paths. Logs
  `local/validation/root-round60-replay/audit-staged.log` (SHA-256
  `0447dc488d329d4c4b55286a29d73c02e87971a5d6618503f9c214b578758f4e`) and
  `local/validation/root-round60-replay/audit-all.log` (SHA-256
  `eb127a95a8c8da17da30c013461297031bea680cbeb45ada7610bf30102bd993`). Cached and working
  `git diff --check` passed; only claimed tests and docs are staged, while existing `native/api`
  user work remains unstaged.
- Completion commit `4ede1c9` is pushed; `HEAD` and `origin/main` match. A confirming no-op push
  returned `Everything up-to-date`, with state and push evidence in the Round 60 completion
  checkpoint. Existing user-owned `native/api` work remains excluded.
- Exact next action: review Round 61 scout reports, claim three distinct one-file synthetic fixtures
  in the task and handoff registries, publish those claims, then dispatch.

## Session addendum 2026-10-21 Round 60 workers dispatched (/root)

- Round 60 claims were published before worker edits in commit `4e5779a`. `/root/launcher_review`
  owns only `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift` for the symlinked
  required-archive path fixture; `/root/review_host_proto` owns only `scripts/test_audio_headers.py`
  for LZMA offset 4; `/root/slot23_raw_xrefs` owns only `native/tests/test_region_list.cpp` for
  unlink with the optional count pointer omitted. Exact assertions, claim-time file hashes and
  focused commands are in the claim checkpoint.
- Workers were asked to make test-only changes, run their focused command, report log/file hashes,
  and not edit shared docs or commit. All three tasks remain ACTIVE with 0 failed tries pending
  root review and replay.
- `python3 scripts/project_index.py` exited 0 and refreshed `docs/FOLDERS.md` for 14,415 folders;
  log `local/validation/root-round60-dispatch/project-index.log`, SHA-256
  `c2e27c54c58f4eb9ecf508e31834abb7be3e189e49b23e1376681c3fc7d0f666`. The first
  `trail build --check` exited 1 (`STALE`) while worker edits were still arriving: six existing
  graph nodes changed and two test-method nodes were added. Log
  `local/validation/root-round60-dispatch/trail-check.log`, SHA-256
  `fb5887c0c35ce94f58fb8ad09770a7285750583980b8bdf63cbedcbc7b2c87b1`. This is a graph freshness
  result, not a fixture failure. Exact next action: publish this dispatch checkpoint, root-replay
  all three focused commands from stable diffs, then rebuild/check trail before marking tasks.
- Dispatch-doc publication audits passed: staged 3 paths (log
  `local/validation/root-round60-dispatch/audit-staged.log`, SHA-256
  `84dac959477de3004b6eba701143ea3d8da546ca29c1d069a64a6ba370b707b9`) and full 1,621 paths
  (log `local/validation/root-round60-dispatch/audit-all.log`, SHA-256
  `92783414bf65602e32ae84af85cec98d5e87ae43ba99cfb25f623b3d194effe4`); cached and working
  `git diff --check` both exited 0. Only `HANDOFF.md`, `docs/FOLDERS.md`, and the dispatch
  checkpoint are staged; worker test files and existing `native/api` user work remain unstaged.

## Session addendum 2026-10-21 Round 60 fixture claims (/root)

- Round 59 completion was pushed as `7f9ce03` after the failed DATA-02 fixture checkpoint commit
  `0653bce`; both are on `origin/main`. Round 60 claims three independent synthetic tests:
  APP-01 validates a required archive path that is a symlink to a directory; AUD-01 validates the
  existing LZMA probe at offset 4 with a synthetic payload; SRV-01 validates unlinking a middle node
  while the optional free-count pointer is null. Exact owner, file, branch, command, claim-time
  hashes and limitations are in
  [the Round 60 claim checkpoint](docs/checkpoints/2026-10-21-root-round60-fixture-claims.md).
- Target tests were clean at claim time; production sources remain unchanged. Root owns shared docs
  and will independently replay all commands. Existing user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` work remains excluded.
- `python3 scripts/project_index.py` exited 0 for 14,411 folders (353 checkpoint files), log
  `local/validation/root-round60-claims/project-index.log`, SHA-256
  `aa66db4a28246681bb51bd2b3d670bd0b926ece1103d0c04c13fc9c721637a0f`. `trail build --check`
  exited 0 with 3,553 nodes / 4,795 edges in sync; optional deep layer remains unbuilt. Log
  `local/validation/root-round60-claims/trail-check.log`, SHA-256
  `edd294377abf28693b2f71cccac3065592d98729c855c751af91b8e78c547973`.
- Pre-push audit accepted 4 staged paths and 1,620 full paths; cached and working diff checks passed.
  Exact audit log hashes are in the Round 60 claim checkpoint. Only claim/docs paths are staged.
- Exact next action: publish the Round 60 claim checkpoint, then dispatch the three single-file
  worker tasks.

## Session addendum 2026-10-21 Round 59 fixtures verified (/root)

- Completed three single-file synthetic fixtures. APP-01 exact unavailable-engine accessibility
  copy passed in the worker and root runs (3 tests, 1 suite each; production mapping unchanged;
  E175). DATA-02's initial one-path fixture failure is preserved in E174; corrected two-path NUL
  manifest passed 33 PSARC tests on root with the parser unchanged (E176; one failed fixture
  construction hypothesis). SRV-01 valid `0xffff` / `0xfffff` alignment boundaries built and passed
  focused CTest 1/1 on both worker and root (implementation unchanged; E177).
- Exact commands, log and file hashes, attempt accounting, observations and test limits are in
  [the Round 59 completion checkpoint](docs/checkpoints/2026-10-21-root-round59-fixtures-complete.md).
  Only the three claimed test files changed; no task claims live launcher/VoiceOver, archive/runtime,
  allocator integration, game boot/audio/save/load, or Metal gameplay.
- `python3 scripts/project_index.py` exited 0 and indexed 14,410 folders (352 checkpoint files);
  log `local/validation/root-round59-replay/project-index.log`, SHA-256
  `bff35dfce43fadd9cf3ebcbdec9bc876b225f99a61243b5a4db2bd79cda2305c`.
- Pre-push audit accepted 8 staged paths and 1,619 full paths; both cached and working diff checks
  passed. Exact audit log hashes are in the Round 59 completion checkpoint. User-owned `native/api`
  changes remain unstaged.
- After the PSARC test edit, `trail build --check` reported STALE for that test file; `trail build`
  exited 0 and the follow-up check passed at 3,553 nodes / 4,795 edges. The optional deep layer is
  not built. G0 remains PARTIAL; G1-G6 remain NOT RUN. Existing user-owned `native/api` changes stay
  untouched and excluded.
- Exact next action: claim the next bounded independent fixture tasks in `docs/TASKS.md` and record
  Round 60 ownership/commands in `HANDOFF.md` before dispatch.

## Session addendum 2026-10-21 Round 59 DATA-02 first attempt (/root)

- DATA-02-PSARC-MANIFEST-NUL-PRECEDENCE-FIXTURE-01 attempt 1 failed in the test fixture, not
  production code. With one member path, the requested NUL separator is not emitted; the parser
  therefore chooses newline splitting and raises `PsarcError: manifest has 2 paths for 1 members`
  for the embedded newline path. Worker log
  `local/validation/data02-psarc-manifest-nul-precedence/attempt-1.log` SHA-256
  `3b252197ce74415027f60684da4838940c86e576fb39bde6d5c3fd3851a0282a`; failed test file SHA-256
  `fc0f0cf8c6b73484ddcee2c0e5fca4b7aa740ad6b08578e95154adb3c19718a4`; parser SHA-256 remains
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. The failure and exact
  next action are recorded in
  [the attempt 1 checkpoint](docs/checkpoints/2026-10-21-root-round59-data02-attempt1-failed.md)
  and E174. Failed fixture hypotheses: 1. No implementation defect has been demonstrated.
- Attempt 2 will use at least two paths, with a newline in the first path, ensuring the raw
  manifest contains an actual NUL delimiter. Root owns the corrected fixture and will run
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` before acceptance.
- `/root/launcher_review` reports APP-01's focused suite passed (3 tests); `/root/slot23_raw_xrefs`
  reports SRV-01 build and focused CTest passed (1/1). Root replay and review remain pending. Existing
  `native/api/CMakeLists.txt` and `native/api/host/` user work remains excluded.
- `python3 scripts/project_index.py` exited 0 and indexed 14,409 folders (351 checkpoint files);
  log `local/validation/root-round59-data02-failure/project-index.log`, SHA-256
  `a8d712e8af762dda25bc1f8d83ba259485ceac1a7e70b7512b6b7a5ffb446ea9`. `trail build --check`
  exited 0 with the wiring graph in sync at 3,553 nodes; optional deep layer remains unbuilt. Log
  `local/validation/root-round59-data02-failure/trail-check.log`, SHA-256
  `edd294377abf28693b2f71cccac3065592d98729c855c751af91b8e78c547973`.
- The pre-push staged path audit accepted 5 paths and the full audit accepted 1,618 paths; exact
  log hashes are in the attempt 1 checkpoint. Cached and working diff checks passed. Only docs are
  staged; the three test-file edits and existing user-owned native/api paths are unstaged.
- Exact next action: publish this failure checkpoint with docs-only staging, then correct the
  two-path fixture and root-replay all three Round 59 commands.

## Session addendum 2026-10-21 round 59 fixture claims (/root)

- Claimed three independent, test-only fixtures after Round 58 completion was published. APP-01
  pins the exact accessibility value for a synthetic engine-not-implemented failure; DATA-02 pins
  newline preservation within a NUL-separated synthetic PSARC member path; SRV-01 pins valid
  representable values immediately below 64 KiB and 1 MiB rounding boundaries.
- Claims, worker assignments, exact commands, and claim-time hashes are in
  [Round 59 fixture claims](docs/checkpoints/2026-10-21-root-round59-fixture-claims.md). The
  existing presentation, PSARC, and allocator-region implementation paths remain unchanged.
- Target test files were clean at claim time and task IDs are new. Each owner has one test file;
  root owns shared docs and will independently replay each command after files stabilize. The
  rejected alignment-overflow candidate is not part of this claim.
- Claim-time main and origin/main match at 451530b. The existing native/api/CMakeLists.txt
  modification and native/api/host/ remain user-owned, untouched, and excluded.
- python3 scripts/project_index.py exited 0 and refreshed docs/FOLDERS.md for 14,405 folders
  (350 checkpoint files). Log local/validation/root-round59-dispatch-project-index.log, SHA-256
  f6c5c13cb05cedfc697bbb8ac1be428467d25356e4de4454d70dad499e887566. trail build --check exited
  0 with the wiring graph in sync at 3,552 nodes / 4,792 edges; the optional deep concept layer
  remains unbuilt. Log local/validation/root-round59-dispatch-trail-check.log, SHA-256
  fe0e8f6183c356799cde0f4919e1268085909678b5839242d7617facbea22cdb.
- Staged and full publish-path audits passed for 4 and 1,616 paths, and cached/working diff checks
  passed. Exact log paths and hashes are in the Round 59 claim checkpoint.
- Claim commit 78d232f (docs: claim round 59 synthetic fixtures) was pushed to origin/main; both
  branch refs matched afterward. The staged audit accepted 4 paths and the full audit accepted
  1,616 paths; cached/working diff checks passed. Logs are recorded in the claim checkpoint.
- Pre-push staged path audit accepted 3 paths; log
  local/validation/root-round59-dispatch-audit-staged.log, SHA-256
  7ac8aeb3e1f29f0a997c41e851e9c207ac130b7b82485d0565961974aedb3845. The full audit accepted
  1,617 paths; log local/validation/root-round59-dispatch-audit-all.log, SHA-256
  e40d9016da33b7f642dcfea8833b82b53682d0fb80f544cd8020ec5073a6eefe. Cached and working diff
  checks exited 0.
- Exact next action: commit and push the worker-dispatch checkpoint, then send the three single-file
  tasks.

## Session addendum 2026-10-21 round 58 completion (/root)

- Completed all three Round 58 synthetic fixture tasks after the claim commit a7c6e67 and dispatch
  commit b8bd38d had been published. APP-01 exact unexpected-exit status and recovery-hint copy
  passed 7 tests in one suite; DATA-02 duplicate archive identifiers passed 22 tests; SRV-01
  empty-head region-list insertion built and passed focused CTest 1/1. Each worker result was
  independently replayed by root, and git diff --check passed.
- Changed only the three existing test files; the launcher mapping, archive-map helper, and region
  list implementation stayed byte-identical to claim-time hashes. Evidence E171–E173 and the three
  completion checkpoints record exact commands, file/log hashes, observations, limits, and zero
  failed tries.
- All results are synthetic unit/fixture coverage. They establish no live launcher process behavior,
  real archive or overlay behavior, allocator integration, game boot/audio/save/load, or Metal
  gameplay. Acceptance gates remain G0 PARTIAL and G1–G6 NOT RUN.
- python3 scripts/project_index.py exited 0 and indexed 14,405 folders; docs/checkpoints lists
  348 entries. Log local/validation/root-round58-replay/project-index-publication.log, SHA-256
  f6c5c13cb05cedfc697bbb8ac1be428467d25356e4de4454d70dad499e887566.
  trail build exited 0 and refreshed the wiring graph to 3,552 nodes / 4,792 edges after parsing
  2 of 1,236 files; log local/validation/root-round58-replay/trail-build.log, SHA-256
  25439c52ad4aa53faa88a3d6caa12d1922617b42e37069ff71f625bd267ed8f4.
  trail build --check exited 0; wiring graph is in sync. The optional deep concept layer is not
  built. Check log SHA-256 fe0e8f6183c356799cde0f4919e1268085909678b5839242d7617facbea22cdb.
- The existing user-owned native/api/CMakeLists.txt modification and untracked native/api/host/
  remain untouched and excluded from publication.
- Pre-push staged path audit accepted all 12 paths; log
  local/validation/root-round58-replay/audit-staged.log, SHA-256
  cd108b30a26db323c470796fe77e8b13330b9cfd73ccb0e73795ac4e4fc69611. Full audit accepted 1,614
  paths; log local/validation/root-round58-replay/audit-all.log, SHA-256
  ec667deb72eb61d9b5a0a1b7b7f32a555ded2f26aca69a5a638b50c91e5069c7. Both cached and working
  diff checks exited 0.
- Completion commit 6f649e8 (test: pin launcher exit copy, archive IDs, and empty-list insertion)
  was pushed to origin/main; the remote branch matched after push. Publication record:
  [Round 58 completion publication](docs/checkpoints/2026-10-21-root-round58-completion-publication.md).
- The publication-record staged audit accepted 3 paths; log
  local/validation/root-round58-replay/audit-publication-staged.log, SHA-256
  852a05cae74c891f18e0649f386ca6818ac81595f2da73aa23b5dee9b3d5d15e. The full audit accepted
  1,615 paths; log local/validation/root-round58-replay/audit-publication-all.log, SHA-256
  02a032840c5295cb19dd5c533578697c3aa4bcf55d9eb4b0262e79d1a686acff. Cached and working diff
  checks exited 0.
- The existing user-owned native/api/CMakeLists.txt modification and untracked native/api/host/
  remain untouched, unstaged, and unpublished.
- Exact next action: scout independent bounded tasks in launcher, DATA-02 tooling, and native
  fixtures; claim suitable tasks in docs/TASKS.md and HANDOFF.md before implementation.

## Session addendum 2026-10-21 round 58 fixture claims (`/root`)

- Claimed three independent synthetic fixture tasks with zero failed tries before implementation:
  `APP-01-LIFECYCLE-UNEXPECTED-EXIT-EXACT-COPY-FIXTURE-01` adds exact status/recovery-hint assertions
  to the existing launcher status-copy test; `DATA-02-OVERLAP-MAP-DUPLICATE-ARCHIVE-IDENTIFIERS-FIXTURE-01`
  pins the existing duplicate-ID guard; `SRV-01-REGION-LIST-EMPTY-HEAD-INSERT-FIXTURE-01` covers
  insertion into an empty synthetic list.
- Claims and source/test hashes are in
  [round 58 claim checkpoint](docs/checkpoints/2026-10-21-root-round58-fixture-claims.md). Each task
  has an existing test file, a focused command, unchanged production code, and a separate owner.
- Live source observations: the launcher currently checks only partial unexpected-exit copy and a
  nonempty recovery hint; `compare_member_maps` checks duplicate archive IDs before key-set parity,
  while its existing fixture covers a different key-set guard; `list_insert_at_head` handles a null
  old head, while the existing head-insertion fixture covers only a nonempty list. All three cases
  remain synthetic and do not prove runtime/game behavior.
- Claim-time state: `main` and `origin/main` match at `21b0ac4`; the only pre-existing local work is
  the user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/`. They
  remain untouched and excluded.
- Claim commit `a7c6e67` (`docs: claim round 58 synthetic edge fixtures`) is pushed to
  `origin/main`. The exact owner assignments and focused commands are in
  [round 58 dispatch checkpoint](docs/checkpoints/2026-10-21-root-round58-workers-dispatched.md).
- `/root/launcher_review` owns only `launcher/Tests/LauncherCoreTests/LauncherStatusCopyTests.swift`;
  `/root/review_host_proto` owns only `scripts/test_archive_overlap_map.py`;
  `/root/slot23_raw_xrefs` owns only `native/tests/test_region_list.cpp`. Root owns shared docs and
  independent replay. Implementation begins only after dispatch documentation is published.
- `python3 scripts/project_index.py` exited 0 and regenerated the folder index for 14,401 folders;
  `docs/checkpoints` lists 343 and `logs` lists 93. Log `logs/root-round58-claim-project-index.log`,
  SHA256 `1fce09b08a087681b67949e46317690178e639d219ac20fbf6999c6e81c8e6f9`. `trail build --check`
  exited 0 with the wiring graph in sync at 3,551 nodes / 4,790 edges; log
  `logs/root-round58-claim-trail-check.log`, SHA256
  `1d6a8b7368552c48cb7630dfe051bb32e14cd65ebc74330bcb42255d15f9027f`. The optional deep concept
  layer is not built. `git diff --check` exited 0.
- Pre-push audit accepted all 4 staged paths; log `logs/root-round58-claim-audit-staged.log`, SHA256
  `41386acd7ce72037de0a3d7dcfc4167adc0185ffe83c4cf256500f9d1e39dbe5`. Full path audit accepted
  1,610 paths; log `logs/root-round58-claim-audit-all.log`, SHA256
  `585781946f40998853916242d1376e8a33dcdb4fbc014b1882dcaa7201dbec8e`. `git diff --cached --check`
  and `git diff --check` both exited 0.
- `python3 scripts/project_index.py` exited 0 and refreshed `docs/FOLDERS.md` for 14,401 folders;
  `docs/checkpoints` lists 344 and `logs` lists 97. Log `logs/root-round58-dispatch-project-index.log`,
  SHA256 `1fce09b08a087681b67949e46317690178e639d219ac20fbf6999c6e81c8e6f9`. `trail build --check`
  exited 0 with the wiring graph in sync at 3,551 nodes / 4,790 edges; log
  `logs/root-round58-dispatch-trail-check.log`, SHA256
  `1d6a8b7368552c48cb7630dfe051bb32e14cd65ebc74330bcb42255d15f9027f`. The optional deep concept
  layer is not built; `git diff --check` exited 0.
- Pre-push staged path audit accepted 4 paths; log `logs/root-round58-dispatch-audit-staged.log`,
  SHA256 `5dff47839ee41befc61dbd1bf1f57e1b7206249867d3fab6977283f9cd06f144`. Full path audit
  accepted 1,611 paths; log `logs/root-round58-dispatch-audit-all.log`, SHA256
  `d19c1d0532caa61fce3db62db77a82ae6f79ca0c83afa8f3c458e1da3d066aee`. Cached and working diff
  checks both exited 0.
- Exact next action: restage the four allowlisted files after this handoff update; rerun staged/full
  path audits, commit with `docs: record round 58 worker dispatch`, and push `origin main`. Then
  send each owner its single-file task.

## Session addendum 2026-10-21 round 57 completion (`/root`)

- Completed three test-only fixtures claimed in the round 57 claim checkpoint. APP-01 pins the
  pure Swift `.notBuilt` mapping after a failed core query with nil validation/coverage (worker and
  root each pass 21 tests across two suites; E168). RE-02 pins the numeric-string fallback for
  synthetic ELF symbol type 3 while preserving global binding (Python output and root direct replay
  each report 22 tests OK; E169). SRV-01 pins rejection of `UINT64_MAX` before host mapping
  (worker/root each build `test_host_map` and pass focused CTest 1/1; E170).
- Completion checkpoints record commands, source/test hashes, private logs, observations and limits:
  [APP-01](docs/checkpoints/2026-10-21-root-app01-core-readiness-query-failure-complete.md),
  [RE-02](docs/checkpoints/2026-10-21-root-re02-dynlink-unmapped-symbol-type-complete.md), and
  [SRV-01](docs/checkpoints/2026-10-21-root-srv01-host-map-roundup-overflow-complete.md).
- RE-02 worker's zsh wrapper returned 1 after the Python test process finished with `OK`, because
  its wrapper attempted to assign the reserved read-only variable `status`. Root independently ran
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` and got exit 0 / 22 tests. This
  is recorded as a wrapper issue; there are zero failed test hypotheses.
- Production files are unchanged. These results establish only pure mapping, synthetic ELF parsing,
  and the pre-mapping arithmetic guard. They add no actual-game execution evidence. `native/api/`
  remains user-owned local work and is excluded.
- Task rows and evidence E168–E170 are updated. `python3 scripts/project_index.py` exited 0 and
  regenerated `docs/FOLDERS.md` for 14,401 folders; log `logs/root-round57-project-index.log`,
  SHA256 `1fce09b08a087681b67949e46317690178e639d219ac20fbf6999c6e81c8e6f9`. `trail build` exited
  0 and rebuilt 3,551 nodes / 4,790 edges; log `logs/root-round57-trail-build.log`, SHA256
  `00c81bf0855864d2341b87a9c4d6941dd9a390522979b9622e3fc872d42a44e8`. `trail build --check`
  exited 0 with the wiring graph in sync; log `logs/root-round57-trail-check.log`, SHA256
  `1d6a8b7368552c48cb7630dfe051bb32e14cd65ebc74330bcb42255d15f9027f`. The optional deep concept
  layer remains unbuilt; the wiring graph is the source of truth. `git diff --check` exited 0.
- The required build/test environment and commands were recorded in `INSTALL_LOG.md`; existing
  Swift 6.4.0, Apple clang 21.0.0, CMake 4.4.4, Python 3.15.0, and macOS 27.0.1 were reused with no
  install or dependency change.
- Pre-push `git diff --check` and `git diff --cached --check` exited 0. The staged path audit passed
  all 12 paths; `logs/root-round57-audit-staged.log`, SHA256
  `84c6f44dd225ea334970a4c48e6d59dd385e4a3db6d0332c1323684788684ced`. The full audit passed all
  1,608 paths; `logs/root-round57-audit-all.log`, SHA256
  `dc8e89770b8177aebdb6c00a57ae87ebb0ffcdde71fa4bfc7f27355fb54be4a3`.
- Completion commit `406eb4e` (`test: pin core readiness, dynlink and host-map edge cases`) is
  pushed; `main` and `origin/main` both resolve to `406eb4e`. The publication record is
  [round 57 completion publication](docs/checkpoints/2026-10-21-root-round57-completion-publication.md).
- After adding the publication record, `python3 scripts/project_index.py` exited 0 and refreshed
  `docs/FOLDERS.md` (14,401 folders; `docs/checkpoints` lists 342 and `logs` 89); log
  `logs/root-round57-publication-project-index.log`, SHA256
  `1fce09b08a087681b67949e46317690178e639d219ac20fbf6999c6e81c8e6f9`. `trail build --check`
  exited 0 with the wiring graph in sync at 3,551 nodes / 4,790 edges; log
  `logs/root-round57-publication-trail-check.log`, SHA256
  `1d6a8b7368552c48cb7630dfe051bb32e14cd65ebc74330bcb42255d15f9027f`. The optional deep concept
  layer remains unbuilt. `git diff --check` exited 0.
- The publication-record staged audit accepted all 3 staged paths; log
  `logs/root-round57-publication-audit-staged.log`, SHA256
  `4b70d819180336381c93f9ad4282c601ec8c520b07d7c3be6b43deb334a60b69`. The full audit accepted
  1,609 paths; log `logs/root-round57-publication-audit-all.log`, SHA256
  `8c0c88bd09b02d6ca6a2a43760bd2ff4b3b12dc0a5c4480725e4492173a165b6`. Cached and working diff
  checks both exited 0.
- Exact next action: restage `HANDOFF.md` after this update, rerun staged and full
  `python3 -I scripts/audit_publish_paths.py` checks, then commit with
  `docs: record round 57 fixture publication` and push `origin main`. Keep
  `native/api/CMakeLists.txt` and `native/api/host/` unstaged. After publication, review the three
  read-only scout reports and claim the next independent bounded tasks before any implementation.

## Session addendum 2026-10-21 round 57 claim (`/root`)

- Claimed three distinct synthetic fixture tasks with zero failed tries before implementation:
  `APP-01-CORE-READINESS-QUERY-FAILURE-NOT-BUILT-FIXTURE-01` pins the failed-query mapping when
  validation and coverage are absent; `RE-02-DYNLINK-UNMAPPED-SYMBOL-TYPE-FIXTURE-01` pins the
  numeric-string fallback for an unknown ELF symbol type; `SRV-01-HOST-MAP-ROUNDUP-OVERFLOW-FIXTURE-01`
  pins rejection of a size that overflows page rounding before mapping.
- Claim details and focused commands are in
  [round 57 fixture claims](docs/checkpoints/2026-10-21-root-round57-fixture-claims.md). Each task
  has an existing, registered test file; workers own separate files and root owns shared docs.
- Source observations: `CoreReadiness.nativeCoreStatus` has a distinct `.notBuilt` branch for a
  query failure with nil validation/coverage; `parse` falls back to the decimal string for an
  unknown symbol type; `map_anonymous_aligned` detects wrapped page rounding before calling
  `mmap`. All evidence is synthetic/pure code.
- Claim-time state: `main` at `4439751`, matching `origin/main`; only the pre-existing
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/` are dirty. They remain
  untouched and excluded.
- Claim publication: commit `8b53498` is pushed to `origin/main`; staged and full path audits passed
  for 4 and 1,603 paths. The publication record is
  [round 57 claims published](docs/checkpoints/2026-10-21-root-round57-claims-published.md).
- Dispatched after publication: `/root/launcher_review` owns only
  `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`; `/root/review_host_proto` owns only
  `scripts/test_ps4_dynlink.py`; `/root/slot23_raw_xrefs` owns only
  `native/tests/test_host_map.cpp`. Commands and private log paths are in the
  [dispatch checkpoint](docs/checkpoints/2026-10-21-root-round57-workers-dispatched.md). Shared
  documentation remains root-owned.
- Interim graph check: `trail build --check` exited 1 while workers edited their files; it reported
  changes to `CoreClientTests.swift` and `test_host_map.cpp`. Output is saved at
  `local/validation/round57-dispatch/trail-check-during-edit.log`. Rebuild only after all three files
  stabilize.
- Exact next action: inspect each isolated diff and independently replay the focused command(s),
  rebuild/check the source graph, then update EVIDENCE/TASKS/HANDOFF and publish verified completions.

## Session addendum 2026-10-21 round 56 completion (`/root`)

- Completed all three claimed synthetic fixtures: APP-01 error-code formatting (E165), AUD-01 stale
  archive-summary byte identity (E166), and SRV-01 exact-fit finalization (E167). Each worker and
  root replay passed; completion checkpoints hold source/test/log hashes and limitations.
- The launcher filter passed 29 tests across four suites; the Python XFF suite passed 19 tests; the
  native target built and focused CTest passed 1/1. Production files are unchanged. Root replay logs
  are `logs/root-round56-app01-diagnostics-unknown-code.log` (SHA256
  `c80d1fa28a438e5c58054952d2510d56ce0089ef46e8f2ce13d890aefeecb1a6`),
  `logs/root-round56-aud01-summary-byte-identity.log` (SHA256
  `d604d5b4a307d14f9b407f9dc4ec3c155113ee4ba72b98ed71fc618e7f6974db`), and native build/CTest
  logs (SHA256 `35b39ff164f5cdb62d0d42ba7dd6d6c084ddb6d1b30b0fd5d944818ff834b81b` and
  `a93bfa23ef4ae5f34326c7324f8768932560026f3149f70aefd79b5ce61cf44d`).
- The initial graph check during worker edits reported their two in-flight test symbols as stale.
  Once files stabilized, `python3 scripts/project_index.py` recorded 14,395 folders and `trail
  build` rebuilt 3,549 nodes / 4,785 edges; follow-up `trail build --check` passed. The optional
  deep concept layer remains unbuilt. `native/api/CMakeLists.txt` and `native/api/host/` remain
  untouched, unstaged, and unpublished.
- Completion commit `5c46167` is pushed to `origin/main`. Staged/full path audits passed for 11 and
  1,601 paths. E165–E167, the three task rows, and completion checkpoints were published with the
  synthetic test files. Publication record:
  [round 56 completion publication](docs/checkpoints/2026-10-21-root-round56-completion-publication.md).
- These fixtures add no boot, audio playback, real save/load, Metal gameplay, or packaging evidence;
  acceptance gates remain G0 PARTIAL and G1–G6 NOT RUN.
- Exact next action: record this publication, then scout the next independent bounded tasks without
  touching the existing `native/api/` work.

## Session addendum 2026-10-21 round 56 claim (`/root`)

- Claimed three independent synthetic fixture tasks with zero failed tries before implementation:
  `APP-01-DIAGNOSTICS-UNKNOWN-ERROR-CODE-FORMAT-FIXTURE-01` pins the public error-code formatter's
  unknown raw value; `AUD-01-XFF-ARCHIVE-SUMMARY-BYTE-IDENTITY-FIXTURE-01` pins rejection of a stale
  summary byte count before opening PSARC; `SRV-01-EXACT-FIT-CONSUME-FINALIZE-FIXTURE-01` passes an
  exact carve result through the existing finalizer and checks the synthetic node fields.
- The claim checkpoint is
  [round 56 fixture claims](docs/checkpoints/2026-10-21-root-round56-fixture-claims.md). The live
  task register has no duplicate claim. Each worker owns a separate existing test file; root owns
  shared documentation and independent replay.
- Source observations: `CoreDiagnostics.code` returns `UNKNOWN(raw)` for `.unknown(raw)`;
  `inspect_members` checks `summary['bytes']` against the current file size before constructing
  `Psarc`; the exact-fit helper and finalizer have separate coverage, while this task adds their
  combined synthetic path. These fixtures add no actual-game evidence.
- Current task status is `ACTIVE` for all three; implementation and focused verification are NOT
  RUN. The pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/`
  remain untouched and excluded.
- Claim publication: commit `68e2779` is pushed to `origin/main`; staged and full path audits passed
  for 4 paths and 1,596 paths. The publication record is
  [round 56 claims published](docs/checkpoints/2026-10-21-root-round56-claims-published.md).
- Dispatched after publication: `/root/launcher_review` owns only
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`; `/root/review_host_proto` owns only
  `scripts/test_audio_bank_header.py`; `/root/slot23_raw_xrefs` owns only
  `native/tests/test_exact_fit_finalize.cpp`. Exact commands and private log paths are in the
  [dispatch checkpoint](docs/checkpoints/2026-10-21-root-round56-workers-dispatched.md). Shared
  documentation remains root-owned.
- Exact next action: inspect each worker's isolated diff, rerun its focused command independently,
  then update EVIDENCE/TASKS/HANDOFF and publish verified completions.

## Session addendum 2026-10-21 round 55 (`/root`)

- Completed `APP-01-CORE-DIAGNOSTICS-UNKNOWN-SUBSYSTEM-FIXTURE-01`: worker and root each pass 83
  launcher tests across 12 suites; `.unknown(99)` preserves code and detail in diagnostics copy.
  E162 and [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-core-diagnostics-unknown-subsystem-complete.md)
  record hashes and logs.
- Completed `DATA-02-OVERLAP-MAP-IDENTIFIER-KEYSET-FIXTURE-01`: worker and root each pass all 21
  Python tests; a synthetic map-key mismatch raises the exact guard error. E163 and
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-overlap-map-identifier-keyset-complete.md)
  record hashes and logs.
- Completed `SRV-01-ARENA-PAGE-DEBIT-WHOLE-PAGE-ROUNDING-FIXTURE-01`: worker and root each build
  the test target and pass focused CTest 1/1; a `0x1ffff` debit reduces page count by one. E164 and
  [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-arena-page-debit-rounding-complete.md)
  record hashes and logs.
- All three changes are test-only; production files are unchanged. Root replay output is
  `logs/root-round54-completion-validation.log` (SHA256
  `c2c7afd349ccdc979c3638e3ae328ea46b990106b12bb440541138fecd8b7f60`). Folder index is current at
  14,391; the source graph was rebuilt to 3,547 nodes / 4,781 edges and its follow-up check passed.
  Completion records and all three fixtures were published in `36cdbb8`.
- The round-54 claim publication is documented in
  [round 55 publication](docs/checkpoints/2026-10-21-root-round55-publication.md). Existing
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and unpublished.
- Actual-game gates remain open. Exact next action: record the completion publication, then scout
  the next independent bounded tasks and publish claims before assigning implementation.

## Session addendum 2026-10-21 round 54 (`/root`)

- Claimed three independent synthetic fixture tasks, each `ACTIVE` with zero failed tries:
  `APP-01-CORE-DIAGNOSTICS-UNKNOWN-SUBSYSTEM-FIXTURE-01` pins preservation of an unknown subsystem
  code in the diagnostics text; `DATA-02-OVERLAP-MAP-IDENTIFIER-KEYSET-FIXTURE-01` pins the exact
  error for archive IDs that do not match member-map keys; and
  `SRV-01-ARENA-PAGE-DEBIT-WHOLE-PAGE-ROUNDING-FIXTURE-01` pins one-page debit for a `0x1ffff` byte
  request. Claim checkpoints are linked in `docs/TASKS.md`.
- At claim time `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` owned
  distinct test files. The claim checkpoint was published in `a70dcc8`, with its publication
  record pushed in `d61f11c`; the three tasks are completed in the round-55 addendum above.
- The candidates exercise existing, live, synthetic/pure-code branches and do not require production
  changes. Actual-game gates remain open; `native/api/CMakeLists.txt` and `native/api/host/` remain
  untouched and unpublished.
- The first folder-index check was stale because the validation commands had just added private log
  files after generation; the index was regenerated after the log set stabilized and the follow-up
  index check passed. Full validation output is in `logs/root-round54-claim-project-index.log`;
  the first stale result is retained in `logs/root-round54-claim-project-index-check.log`.
- Exact next action: publish the claim-publication checkpoint, then assign the fixtures and replay
  their focused commands.

## Session addendum 2026-10-21 round 53 (`/root`)

- Published round-52 completions and E159–E161 in commit
  `30f063e6e15999f05efb1ae9dbd1b3dd9ca3a900` (`test: cover launcher ABI sidecar and carve
  boundaries`); push succeeded. See the [round 50 publication checkpoint](docs/checkpoints/2026-10-21-root-round50-publication.md).
- Pre-push checks passed: folder index current at 14,388; the first graph check found three changed
  test files and a new Swift test symbol; `trail build` rebuilt 3,546 nodes / 4,779 edges and the
  follow-up check passed; staged audit accepted 11 paths and full audit accepted 1,584 paths.
- Actual-game gates remain open. `native/api/CMakeLists.txt` and untracked `native/api/host/`
  remain untouched and unpublished.
- Exact next action: scout the next independent bounded tasks, verify their live evidence and
  registry entries, and publish claims before assigning implementation.

## Session addendum 2026-10-21 round 52 (`/root`)

- Published the three claims in `e8207987476db2072c4b4a52c7fe2b7ac0d6ab02` before implementation.
- Completed `APP-01-CORE-COVERAGE-ONLY-STATUS-FIXTURE-01`: worker and root each pass 83 launcher
  tests across 12 suites; the synthetic mapping includes the core ABI when coverage exists without
  validation. E159 and its completion checkpoint record hashes and logs.
- Completed `RE-01-UNWRAP-IDENTITY-SIDECAR-ROUNDTRIP-FIXTURE-01`: worker and root each pass all 8
  project-tool tests; the saved synthetic identity JSON equals the helper's returned record. E160
  and its completion checkpoint record hashes and logs.
- Completed `SRV-01-FREE-LIST-CARVE-ALIGN-OVERFLOW-FIXTURE-01`: worker and root each build the
  native test target and pass focused CTest 1/1 for uint64 align-up overflow rejection. E161 and its
  completion checkpoint record hashes and logs.
- All three changes are test-only. Actual-game gates remain open; `native/api/CMakeLists.txt` and
  untracked `native/api/host/` remain untouched and unpublished.
- Regenerated the folder index to 14,388 entries. The first source-graph check was stale for the
  three changed test files and new launcher test symbol; `trail build` rebuilt the graph to 3,546
  nodes / 4,779 edges, and the follow-up `trail build --check` passed. The graph cache is ignored.
- Exact next action: stage only the three test files and the completion, evidence, task, handoff,
  index, and publication records; rerun staged/full path audits, then commit and push.

## Session addendum 2026-10-21 round 51 (`/root`)

- Claimed three independent fixture tasks, each `ACTIVE` with zero failed tries and its own
  completion gate: (1) `APP-01-CORE-COVERAGE-ONLY-STATUS-FIXTURE-01` asserts the pure launcher
  mapping includes ABI `1.2` when coverage exists but validation is absent; (2)
  `RE-01-UNWRAP-IDENTITY-SIDECAR-ROUNDTRIP-FIXTURE-01` asserts a synthetic successful unwrap writes
  the exact JSON record returned by the helper; (3)
  `SRV-01-FREE-LIST-CARVE-ALIGN-OVERFLOW-FIXTURE-01` asserts an overflowing synthetic align-up is
  rejected. Claim checkpoints are linked in `docs/TASKS.md`.
- `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` own distinct
  test files for those claims. No implementation changes may start until this claim checkpoint is
  pushed.
- Live-source review confirmed each case exercises existing behavior without a production change.
  Actual-game gates remain open; `native/api/CMakeLists.txt` and `native/api/host/` remain untouched.
- Exact next action: publish these three claims and the preceding publication record, then assign
  each test-only fixture and independently replay the focused suites.

## Session addendum 2026-10-21 round 50 (`/root`)

- Published round-49 fixture completions and E156–E158 in commit
  `97a180f376b626098fdc7bb49378207460dcc907` (`test: verify clean exit raw offset and list
  endpoints`); push succeeded. See the [round 47 publication checkpoint](docs/checkpoints/2026-10-21-root-round47-publication.md).
- Pre-push checks passed: folder index current at 14,385; source graph check passed at 3,545 nodes;
  staged audit accepted 11 paths; full audit accepted 1,575 paths; staged diff check passed.
- `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` are scouting
  one candidate each in separate launcher, Python, and native test areas. No new task is claimed;
  do not edit until a bounded claim is recorded and pushed.
- Actual-game gates remain open. `native/api/CMakeLists.txt` and untracked `native/api/host/`
  remain untouched and unpublished.
- Exact next action: review the three scout findings against current files and task registry, then
  claim and publish suitable independent fixture tasks before assigning implementation.

## Session addendum 2026-10-21 round 49 (`/root`)

- Completed `APP-01-LIFECYCLE-CLEAN-EXIT-EXACT-COPY-FIXTURE-01`: the pure launcher value test
  asserts the exact clean-exit text; worker and root each pass 82 tests across 12 suites. E156
  and the [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-clean-exit-copy-complete.md)
  record hashes and logs.
- Completed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET2-FIXTURE-01`: the synthetic raw-DEFLATE stream
  after a two-byte prefix succeeds with 64 output bytes; worker and root each pass 22 tests. E157
  and the [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-raw-inflate-offset2-complete.md)
  record hashes and logs.
- Completed `SRV-01-REGION-LIST-ENDPOINT-UNLINK-FIXTURE-01`: synthetic head/tail removal from a
  three-node list verifies endpoint links, removed-node self-loop, and free-count decrements.
  Worker and root each build the test target and pass focused CTest 1/1. E158 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-region-list-endpoints-complete.md)
  record hashes and logs.
- Regenerated project indexes and source graph after the source additions; actual-game gates remain
  open. `native/api/CMakeLists.txt` and untracked `native/api/host/` are pre-existing local work and
  remain untouched and unpublished.
- Exact next action: run index/graph checks, path audits, and commit/push only the allowlisted
  fixture, evidence, task, handoff, folder-index, and checkpoint files.

## Session addendum 2026-10-21 round 48 (`/root`)

- Published the clean-exit copy, raw-inflate offset-2, and region-list endpoint claims in commit
  `65af615e38202eb5026f9b11808430ea73127fa9` (`docs: claim clean-exit and raw-offset-two
  fixtures`); push succeeded and `HEAD` matches `origin/main`. See the
  [round 46 publication checkpoint](docs/checkpoints/2026-10-21-root-round46-publication.md).
- Pre-push checks passed: folder index current at 14,382; trail graph current at 3,544 nodes;
  staged audit accepted 7 paths; full audit accepted 1,571 paths; staged diff check passed.
- `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` own separate
  test files for the three claims. Their changes and tests are in progress; root has not replayed
  them yet.
- Actual-game gates remain open. `native/api/CMakeLists.txt` and `native/api/host/` remain
  untouched and unpublished.
- Exact next action: review worker diffs, replay the Swift and Python suites and rebuild/replay the
  native list test, then record the outcomes and publish.

## Session addendum 2026-10-21 round 47 (`/root`)

- Published the round-44 publication record for commit
  `454ef8213e352d54eb831e6fa563e81ba37fe8e0` (`docs: record fixture publication round 44`);
  push succeeded. Checkpoint:
  [round 45 publication](docs/checkpoints/2026-10-21-root-round45-publication.md).
- Claimed `APP-01-LIFECYCLE-CLEAN-EXIT-EXACT-COPY-FIXTURE-01`, `ACTIVE`, zero failed tries:
  assert the exact clean-exit status value in the existing pure-model fixture. Claim:
  [clean-exit copy](docs/checkpoints/2026-10-21-root-app01-clean-exit-copy-claim.md).
- Claimed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET2-FIXTURE-01`, `ACTIVE`, zero failed tries:
  prove a synthetic raw DEFLATE stream after a two-byte prefix succeeds in the offset-2 probe.
  Claim:
  [raw-inflate offset 2](docs/checkpoints/2026-10-21-root-aud01-raw-inflate-offset2-claim.md).
- Claimed `SRV-01-REGION-LIST-ENDPOINT-UNLINK-FIXTURE-01`, `ACTIVE`, zero failed tries:
  cover unlinking the head and tail of a synthetic three-node chain. Claim:
  [region-list endpoints](docs/checkpoints/2026-10-21-root-srv01-region-list-endpoints-claim.md).
- The three claims use clean, distinct test files. `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs` may start only after the claim commit is
  pushed. Actual-game acceptance gates remain open; `native/api/CMakeLists.txt` and
  `native/api/host/` stay untouched.
- Exact next action: publish the claim and prior-publication checkpoints, then assign
  `LauncherStatusCopyTests.swift` to `/root/launcher_review`, `test_audio_headers.py` to
  `/root/review_host_proto`, and `test_region_list.cpp` to `/root/slot23_raw_xrefs`; root will
  replay all acceptance commands.

## Session addendum 2026-10-21 round 46 (`/root`)

- Published the launcher-tone, decoder-fail-closed, and region-overflow fixture completions in
  `a7ad0f9b01ca50f3b45f024a87b13ff108a22492` (`test: cover idle tone, decoder, and split
  overflow`); push succeeded and `HEAD` matches `origin/main`. See the
  [round 44 publication checkpoint](docs/checkpoints/2026-10-21-root-round44-publication.md).
- Pre-push checks: folder index current at 14,382; trail graph rebuilt to 3,544 nodes / 4,774
  edges and `trail build --check` passed; staged audit accepted 11 paths; full audit accepted
  1,566 paths; staged diff check passed.
- Worker and root evidence is recorded in E153–E155. These three tasks are `FIXTURE_VERIFIED`
  with zero failed tries; no production code changed. The shared handoff and task registries keep
  actual-game gates open.
- `/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` are scouting
  the next clean test-only candidate in separate launcher, Python, and native modules. No new
  task is claimed yet.
- Exact next action: review all scout results against the live tree and task registry, publish a
  bounded claim, then assign separate files after its claim commit is pushed.

## Session addendum 2026-10-21 round 45 (`/root`)

- Completed `APP-01-LAUNCHER-PRESENTATION-IDLE-TONE-FIXTURE-01`: worker and root each pass
  82 launcher tests across 12 suites. The idle model now asserts `.neutral`; production is
  unchanged. E153 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-app01-idle-tone-complete.md)
  record log hashes.
- Completed `CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01`: worker and root each pass the
  synthetic NOP `DecodeError` test; the decoder is unchanged. E154 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-cpu01-leaf-ctor-decode-fail-closed-complete.md)
  record evidence.
- Completed `SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01`: worker and root each pass focused CTest
  1/1; root rebuilt the test target before replay. The overflow is rejected without mutation.
  E155 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-srv01-region-split-overflow-complete.md)
  record evidence.
- Regenerated the folder index to 14,382 folders. The first `trail build --check` after the test
  edits reported the region test change and new decoder test as stale; `trail build` rebuilt the
  graph to 3,544 nodes / 4,774 edges, and the follow-up check passed. The graph cache is ignored.
- See the [round 43 publication checkpoint](docs/checkpoints/2026-10-21-root-round43-publication.md)
  for publication of the claims. Existing `native/api/CMakeLists.txt` and `native/api/host/`
  remain untouched and unpublished. G0 is still PARTIAL; G1–G6 remain NOT RUN.
- Exact next action: stage only the three verified test files, evidence/task/handoff updates,
  folder index, and completion checkpoints; run staged/full audits, commit, and push. Then select
  another unclaimed bounded task if the goal remains active.

## Session addendum 2026-10-21 round 44 (`/root`)

- Published the save-header fixture completion and three independent test-only claims in commit
  `ba0075db83b663821567b65d4a0af8c947e7295f` (`test: verify trailing save chunk header fields`);
  push succeeded and `HEAD` matches `origin/main`. See the
  [round 43 publication checkpoint](docs/checkpoints/2026-10-21-root-round43-publication.md).
- The commit passed `python3 scripts/project_index.py` and `--check` at 14,379 folders,
  `trail build --check` at 3,539 nodes, staged path audit (10 paths), and full path audit
  (1,561 paths). Only allowlisted documentation and `native/tests/test_save_chunk_header.cpp`
  were committed.
- Workers now own separate files: `/root/launcher_review` has
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`, `/root/review_host_proto` has the
  new `tools/lift/test_leaf_ctor_decode.py`, and `/root/slot23_raw_xrefs` has
  `native/tests/test_region_split.cpp`. All three tasks are `ACTIVE`, zero failed tries; tests
  have not yet been independently replayed by root.
- `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and unpublished.
  Actual boot, authentic audio, real save/reload, Metal, and forbidden-runtime/module gates remain
  open.
- Exact next action: review each worker's isolated diff, replay the focused acceptance command,
  capture root logs and hashes, then publish verified outcomes and this commit's publication
  record.

## Session addendum 2026-10-21 round 43 (`/root`)

- Published claim `SAV-01-CHUNK-HEADER-TRAILING-FIELDS-FIXTURE-01` in commit
  `92189ffa4171b789be76a4da2890fb7c86d46d1f` (`docs: claim trailing payload header assertions`);
  push succeeded. See the [round 42 publication checkpoint](docs/checkpoints/2026-10-21-root-round42-publication.md).
- Completed the save-header fixture. Worker and root each pass
  `ctest --test-dir local/build/native -R '^save_chunk_header$'` (1/1); only
  `native/tests/test_save_chunk_header.cpp` changed, and the parser is unchanged. E152 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-sav01-chunk-header-trailing-fields-complete.md)
  record both logs and hashes.
- Claimed `APP-01-LAUNCHER-PRESENTATION-IDLE-TONE-FIXTURE-01`, `ACTIVE`, zero failed tries:
  assert `.neutral` on the existing idle presentation fixture. Claim:
  [idle presentation tone](docs/checkpoints/2026-10-21-root-app01-idle-tone-claim.md).
- Claimed `CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01`, `ACTIVE`, zero failed tries:
  add a synthetic unsupported-NOP fixture that requires `DecodeError` at RVA `0x4000`, without
  changing the decoder. Claim:
  [leaf decoder fail-closed fixture](docs/checkpoints/2026-10-21-root-cpu01-leaf-ctor-decode-fail-closed-claim.md).
- Claimed `SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01`, `ACTIVE`, zero failed tries:
  assert an overflowing synthetic lead address is rejected without modifying either node.
  Claim: [region split overflow](docs/checkpoints/2026-10-21-root-srv01-region-split-overflow-claim.md).
- The three claims have distinct test files and workers. Actual boot, authentic game audio,
  real save/load, Metal gameplay, and forbidden-runtime/module gates remain open. Existing
  `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and unpublished.
- Exact next action: after this claim publication, assign `LauncherModelTests.swift` only to
  `/root/launcher_review`, the new `tools/lift/test_leaf_ctor_decode.py` only to
  `/root/review_host_proto`, and `native/tests/test_region_split.cpp` only to
  `/root/slot23_raw_xrefs`; then replay each acceptance command and record results.

## Session addendum 2026-10-21 round 41 (`/root`)

- Published the duplicate-readiness and unknown-subsystem-code fixture completions in commit
  `ca6bcd3302e672dcb0b6126ae125f5362b609576` (`test: cover duplicate readiness and unknown
  subsystem codes`); push succeeded and `HEAD` matched `origin/main`. See the
  [round 40 publication checkpoint](docs/checkpoints/2026-10-21-root-round40-publication.md).
- Pre-push checks passed: folder index current at 14,378 entries; trail graph current at 3,539
  nodes after rebuilding for the two changed Swift test files; staged audit accepted 9 paths;
  full audit accepted 1,553 paths.
- The short-magic, conflicting-duplicate, and unknown-subsystem fixtures are `FIXTURE_VERIFIED`.
  These synthetic tests do not change the actual-game acceptance gates.
- Existing `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and unpublished.
- Exact next action: review the scout's next independent fixture candidate, check its prerequisite
  status, then publish a bounded claim if it is still unowned.

## Session addendum 2026-10-21 round 40 (`/root`)

- Completed `APP-01-DIAGNOSTICS-CONFLICTING-DUPLICATE-01`; worker and root each pass 7/7 focused
  tests. The fixture proves a later missing duplicate cannot make an all-ready report startable.
  E150 and its completion checkpoint record both logs and hashes.
- Completed `APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01`; worker and root each pass 82 tests
  across 12 suites. The unknown-code assertion is test-only; `CoreModels.swift` is unchanged.
  E151 and its completion checkpoint record both logs and hashes.
- The three launcher fixtures from rounds 38–40 are synthetic test coverage. No real game
  acceptance gate changed. Existing `native/api/CMakeLists.txt` and `native/api/host/` remain
  untouched and unpublished.
- Exact next action: regenerate/check indexes and the source graph, publish the two fixture
  completions and round-38 publication record, then select another bounded unowned task.

## Session addendum 2026-10-21 round 39 (`/root`)

- Published short-magic fixture completion, E149, and two separate fixture claims in commit
  `d7f471fca29bf7ddd380d44efa5cea6d47d8c628` (`test: cover short PSARC magic in launcher report`);
  push succeeded and `HEAD` matched `origin/main`. See the
  [round 38 publication checkpoint](docs/checkpoints/2026-10-21-root-round38-publication.md).
- Pre-push verification passed: project index current at 14,372 folders; trail graph current at
  3,538 nodes; staged audit accepted 8 paths; full audit accepted 1,550 paths; staged diff check
  passed. Only allowlisted documentation and the short-magic Swift test were committed.
- `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01` is `FIXTURE_VERIFIED`; the diagnostics
  conflicting-duplicate and unknown-subsystem-code tasks are `ACTIVE`, each with zero failed
  tries and a distinct test-file owner.
- Actual game acceptance gates remain open. Existing `native/api/CMakeLists.txt` and
  `native/api/host/` remain untouched and unpublished.
- Exact next action: receive both workers' results, review their distinct diffs, replay each
  focused test suite, and publish accurate outcomes.

## Session addendum 2026-10-21 round 38 (`/root`)

- Completed `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01`. Worker and root each pass the
  focused launcher suite (6/6); only `AssetValidationReportTests.swift` changed, while
  `AssetValidator.swift` stayed unchanged. E149 and the completion checkpoint record both logs
  and hashes.
- Claimed `APP-01-DIAGNOSTICS-CONFLICTING-DUPLICATE-01`, `ACTIVE`, zero failed tries. Add a
  synthetic `ReadinessReport` with the four required subsystem rows READY plus a later duplicate
  assets row marked missing; assert the report is not startable and the summary does not claim
  Play is ready. Claim:
  [conflicting duplicate diagnostics](docs/checkpoints/2026-10-21-root-app01-diagnostics-conflicting-duplicate-claim.md).
- Claimed `APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01`, `ACTIVE`, zero failed tries. Add the
  missing pure-Swift fallback assertion `CoreSubsystem.Status(code: 99) == .unknown(99)` in
  `CoreClientTests.swift`. Claim:
  [unknown subsystem code](docs/checkpoints/2026-10-21-root-app01-core-subsystem-unknown-code-claim.md).
- `/root/review_host_proto` owns only `DiagnosticsSummaryTests.swift`; `/root/launcher_review`
  owns only `CoreClientTests.swift`. Both changes begin after these claims are published. Root
  will replay the focused suites.
- No actual game acceptance gate changed. Existing `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain untouched and unpublished.
- Exact next action: regenerate/check indexes, path-audit the exact documentation publication,
  push the short-magic completion and both claims, then release the two test files to their owners.

## Session addendum 2026-10-21 round 37 (`/root`)

- Recorded publication of the NUL-filename completion and the raw-DEFLATE task-status correction
  in commit `ae2ee8d8daf8330a461cc5e1330309c2c80c7e24` (`docs: reconcile published fixture task
  states`); push succeeded and HEAD matched `origin/main`. See the
  [round 35 publication checkpoint](docs/checkpoints/2026-10-21-root-round35-publication.md).
- Claimed `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01`, `ACTIVE` with zero failed tries. Add
  one test in `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`: create the
  existing six-file valid fixture, replace `pak0.psarc` with `Data("PS".utf8)`, and assert the
  sole problem is `pak0.psarc is not a PSARC archive` while `missingArchives` remains empty. No
  production code changes or real archive data. Claim:
  [short PSARC magic](docs/checkpoints/2026-10-21-root-app01-asset-validator-short-magic-claim.md).
- `/root/slot23_raw_xrefs` may edit only the test file after this claim is published. Root owns
  shared docs and will run `swift test --package-path launcher --filter AssetValidationReportTests`.
- Actual game acceptance gates remain open. Existing host-runtime worktree paths remain untouched
  and unpublished.
- Exact next action: run `python3 scripts/project_index.py`, verify indexes, path-audit, then commit
  and push the round 35 publication record and short-magic claim.

## Session addendum 2026-10-21 round 36 (`/root`)

- Published `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01` completion in commit
  `e9b1a15526e247401bb1622cfd077853ba9b7a31` (`test: preserve newlines in staged path audit
  names`); push succeeded and HEAD matches `origin/main`. See the
  [round 34 publication checkpoint](docs/checkpoints/2026-10-21-root-round34-publication.md).
- Corrected the stale `docs/TASKS.md` status for `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`:
  E145 and its completion checkpoint already recorded `FIXTURE_VERIFIED`, while the task row still
  said `ACTIVE`. This is a documentation reconciliation; no code or test result changed.
- `/root/slot23_raw_xrefs` is scouting another bounded test-only task in a distinct module. No
  implementation begins until root checks the candidate and publishes a claim.
- Pre-push checks for the prior commit passed: folder index current at 14,235; source graph current
  at 3,537 nodes; staged audit accepted 7 paths and full audit accepted 1,544 paths. Only the
  pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain.
- Actual game acceptance gates remain open.
- Exact next action: review the scout's candidate and claim one unowned, bounded task if available.

## Session addendum 2026-10-21 round 35 (`/root`)

- Completed `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `/root/review_host_proto` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_audit_publish_paths.py` (8/8). A mocked staged Git result containing one embedded
  newline is returned as one path; the fixture also asserts `git diff --cached --name-only -z`
  and `cwd=audit.ROOT`. Test SHA256 is
  `2759f5b9f554388ca7fd95ab2ee6457d16af995d3f0e86bbe85458422e65d2ac`; production audit
  `scripts/audit_publish_paths.py` is unchanged at
  `2ea984cfb38329f4a9ec0b46300b0513998a1a642d224572906cee531653bba3`. E148 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-aud-publish-git-nul-filename-complete.md)
  record worker/root logs and hashes.
- The claim publication for this task is recorded at [round 33](docs/checkpoints/2026-10-21-root-round33-publication.md).
  `trail build` parsed the test and rebuilt 3,537 nodes / 4,758 edges; `trail build --check` passed.
- Actual game acceptance gates remain open. Only the pre-existing `native/api/CMakeLists.txt`
  modification and untracked `native/api/host/` remain outside the task.
- Exact next action: update and check the folder index, then path-audit, commit, and push the fixture
  completion and publication record.

## Session addendum 2026-10-21 round 34 (`/root`)

- Published the claim for `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01` in commit
  `c640c20b9bf48f720a5e4e8d87e82bd45d798e59` (`docs: claim NUL-safe audit filename fixture`);
  the push succeeded and HEAD matches `origin/main`. See the
  [round 33 publication checkpoint](docs/checkpoints/2026-10-21-root-round33-publication.md).
- The task remains `ACTIVE` with zero failed tries. `/root/review_host_proto` now owns only
  `scripts/test_audit_publish_paths.py`; root owns shared docs and will replay the focused test.
- Pre-push checks passed: project index current at 14,234 folders; source graph current at 3,536
  nodes; staged-path audit accepted 5 paths and full-path audit accepted 1,542 paths. The only
  unrelated dirty paths remain the pre-existing `native/api/CMakeLists.txt` and `native/api/host/`.
- Actual game acceptance gates remain open.
- Exact next action: review the worker diff and root-replay
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.

## Session addendum 2026-10-21 round 33 (`/root`)

- Recorded the prior completion publication in commit `33099216c0777f517d4087faaf68b8cb61181bc8`
  (`docs: record PSARC bounds publication`); `git push origin main` succeeded and local HEAD
  matches the remote. See the [round 32 publication checkpoint](docs/checkpoints/2026-10-21-root-round32-publication.md).
- Claimed `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`, `ACTIVE` with zero failed tries. Add one
  synthetic test in `scripts/test_audit_publish_paths.py` that mocks the staged-name Git command
  to return `b"scripts/name\nwith-newline.py\0"`, then confirms `git_names("staged")` returns
  exactly one path containing the embedded newline and uses `--name-only -z` from `audit.ROOT`.
  Do not change the production audit. Claim: [NUL-delimited filename](docs/checkpoints/2026-10-21-root-aud-publish-git-nul-filename-claim.md).
- `/root/review_host_proto` has been asked to implement only the test module after this claim is
  published. Root owns shared docs and will independently replay
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.
- Actual game acceptance gates remain open; the only unrelated dirty paths remain the pre-existing
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/`.
- Exact next action: run `python3 scripts/project_index.py`, verify folder and source indexes,
  path-audit the publication, then commit and push the round 32 publication record and task claim.

## Session addendum 2026-10-21 round 32 (`/root`)

- Published `DATA-02-PSARC-BLOCK-SIZE-RANGE-01` completion in commit
  `cf5096b0c775104739f871c40da8b698618219b6` (`test: cover PSARC block-size validation bounds`);
  `git push origin main` succeeded and HEAD matches `origin/main`. See the
  [round 31 publication checkpoint](docs/checkpoints/2026-10-21-root-round31-publication.md).
- Pre-push checks passed: `python3 scripts/project_index.py --check` reports 14,234 folders
  current; `trail build --check` reports 3,536 nodes in sync; `git diff --check` passes; the staged
  path audit accepted 7 paths and the full tracked-path audit accepted 1,539 paths.
- The worktree retains only the pre-existing `native/api/CMakeLists.txt` modification and untracked
  `native/api/host/`. Actual game acceptance gates remain open.
- `/root/review_host_proto` is scouting one additional test-only task in a disjoint module; it is
  read-only and must not edit before a bounded claim is recorded and published.
- Exact next action: review the scout's candidate against Graft, live source, and `docs/TASKS.md`,
  then claim one bounded task before assigning any edit.

## Session addendum 2026-10-21 round 31 (`/root`)

- Completed `DATA-02-PSARC-BLOCK-SIZE-RANGE-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `slot23_raw_xrefs` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_psarc.py` (32/32). Starting from a valid synthetic PSARC, the fixture mutates
  header bytes 24–27 to `0` and `16,777,217`; both are rejected with the exact
  `PsarcError('unsupported entry/block size')`. Test SHA256 is
  `0a922203a17cfe7454cbf5f1f8e5787413511320d6bd4a54415c205360263dfc`; parser
  `scripts/psarc.py` is unchanged at
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. E147 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-block-size-range-complete.md)
  record the worker/root log hashes and synthetic-only limit.
- The prior publication is recorded at [round 29](docs/checkpoints/2026-10-21-root-round29-publication.md).
  `trail build` parsed the changed test and rebuilt 3,536 nodes / 4,756 edges; subsequent
  `trail build --check` passed. The project folder index became stale after the root test log was
  created; regenerate and check it before the next push.
- The only excluded worktree paths remain the pre-existing `native/api/CMakeLists.txt` modification
  and untracked `native/api/host/`. Actual game acceptance gates remain open.
- Exact next action: run `python3 scripts/project_index.py`, verify it with `python3 scripts/project_index.py --check`, then stage only the PSARC test and its task/evidence/checkpoint records, audit, commit, and push.

## Session addendum 2026-10-21 round 30 (`/root`)

- Published the zero-length mapping fixture and the bounded PSARC block-size claim in commit
  `13b1b3ab64ed29ee4c3992d6edf27e85a8646ff9` (`test: pin dynlink empty ranges and claim PSARC
  size bounds`) to `origin/main`; the remote accepted the push and HEAD matches origin/main.
  See [publication checkpoint](docs/checkpoints/2026-10-21-root-round29-publication.md).
- Before the push, `python3 scripts/project_index.py` wrote 14,233 folder entries;
  `python3 scripts/project_index.py --check` reported them current; `trail build --check` reported
  3,535 nodes in sync; and `git diff --check` passed. `python3 -I scripts/audit_publish_paths.py`
  accepted all 7 staged paths, and `python3 -I scripts/audit_publish_paths.py --all` accepted
  1,537 paths. The new checkpoint and task claim are now published.
- `DATA-02-PSARC-BLOCK-SIZE-RANGE-01` remains `ACTIVE`, with `/root/slot23_raw_xrefs` assigned
  only `scripts/test_psarc.py`; the worker has been asked to add the two synthetic invalid-header
  cases, run the focused suite, and save a private log. Root owns shared docs and will replay the
  focused suite before marking completion.
- Only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/`
  remain in the worktree outside the commit. No actual game acceptance gate advanced.
- Exact next action: review the worker's `scripts/test_psarc.py` diff and independently run
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.

## Session addendum 2026-10-21 round 29 (`/root`)

- Completed `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `launcher_review` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_ps4_dynlink.py` (21/21). The new synthetic assertion confirms that an empty range
  at `0x1010`, the first segment's one-past-end address, is considered mapped. Test SHA256 is
  `75f971b5834fd95f2b16b92133aa999d3f77211e73d74834b6f88da6dfe12e07`; parser
  `scripts/ps4_dynlink.py` is unchanged at
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E146 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-zero-length-complete.md)
  record the worker/root log hashes and synthetic-only limit.
- Claimed `DATA-02-PSARC-BLOCK-SIZE-RANGE-01`, `ACTIVE` with zero failed tries. The scoped test
  mutates the block-size field in a valid synthetic archive to `0` and `16,777,217`, then asserts
  the exact `PsarcError('unsupported entry/block size')` for both invalid bounds. Only
  `scripts/test_psarc.py` may change; production parser and game archives are out of scope. Claim:
  [PSARC block-size range](docs/checkpoints/2026-10-21-root-data02-psarc-block-size-range-claim.md).
- Root owns shared documentation. After this claim checkpoint is published, assign only
  `scripts/test_psarc.py` to `slot23_raw_xrefs`; worker and root must save separate logs in the
  private validation folder. Actual game acceptance gates remain open.
- Exact next action: run `python3 scripts/project_index.py`, verify folder and source indexes, then
  path-audit, commit, and push the zero-length completion plus the new bounded claim.

## Session addendum 2026-10-21 round 11 (`/root`)

- Completed three independent, synthetic test-only tasks: `DATA-02-OVERLAP-MAP-JSON-CONSTANTS-01`
  (worker `launcher_review`, `scripts/test_archive_overlap_map.py` only),
  `AUD-01-XFF-EXACT-PATH-CARDINALITY-01` (worker `slot23_raw_xrefs`,
  `scripts/test_audio_bank_header.py` only), and `DOC-FOLDER-INDEX-SYMLINK-01`
  (worker `review_host_proto`, `scripts/test_project_tools.py` only). All are FIXTURE_VERIFIED with
  zero failed tries; root owns this handoff and the project/task records.
- Their prerequisites are complete: the archive-overlap map artifact is verified; XFF identity
  verification is fixture-verified; and the folder-index admin-directory fixture is fixture-verified.
  No production parser/index code or game data is in scope.
- Claims: [JSON constants](docs/checkpoints/2026-10-21-root-data02-overlap-map-json-constants-claim.md),
  [XFF exact paths](docs/checkpoints/2026-10-21-root-aud01-xff-exact-path-cardinality-claim.md),
  [folder-index symlinks](docs/checkpoints/2026-10-21-root-doc-folder-index-symlink-claim.md).
- Acceptance commands: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`,
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`, and
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`, respectively. Workers must
  save logs in private `local/validation/` paths; root independently replayed them. Worker/root
  results are 19/19, 18/18, and 7/7; E134-E136 and three completion checkpoints record hashes,
  logs, observations, and synthetic-only limits.
- Claim-time repository state: `main`, HEAD equals `origin/main` at `667a88c`; only the previously
  escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` are dirty. Those paths stay
  outside all three tasks and all commits. No game gate advances from these fixtures.
- Claim publication: commit `8723be0eba56ba3ed675dc73b7379edc6f2391eb` is pushed to `origin/main`;
  staged and full path audits passed for 6 staged paths and 1,509 tracked paths. Root verified
  HEAD equals `origin/main`; only the excluded host escalation paths remain dirty.
- Checkpoint-index correction: after private checkpoint logs were added, two
  `python3 scripts/project_index.py --check` runs reported stale `local/indexes/folders.json`;
  the index was regenerated after all log paths existed, and the final check passes with 14,214
  folders. This affects only the private index; `docs/FOLDERS.md` remains current.
- Source-graph checkpoint: `trail build` exited 0 and parsed the changed source; `trail build --check`
  exited 0 with wiring graph in sync (3,527 nodes / 4,739 edges). The deep meaning layer remains
  unbuilt (0% complete); see private logs in `local/checkpoints/root-round11-claims/`.
- Final folder-index checkpoint: `python3 scripts/project_index.py` and
  `python3 scripts/project_index.py --check` both exited 0 with 14,217 folders; generated
  `docs/FOLDERS.md` includes the three completion records, and the private recursive index is current.
  Logs: `local/checkpoints/root-round11-claims/{index-final.log,index-final-check.log}`.
- Immediate next action: continue with another bounded unclaimed task from the read-only scout, if
  available. Preserve the slot-23/Ghidra blocker, three-try host escalation, and all game/runtime
  acceptance gates.

## Session addendum 2026-10-21 round 12 (`/root`)

- Published round 11's three synthetic test-only completions: overlap-map JSON constants, XFF exact
  path cardinality, and folder-index symlink exclusion. Commit `219f261ce102aabe6050a3ffa63ff1e4f7114856`
  is on `origin/main`; root replay counts were 19/19, 18/18, and 7/7. E134-E136 and the three
  completion checkpoints record private logs and hashes. No production behavior or game gate changed.
- Graft located `name_by_hash()` at `scripts/ps4_dynlink.py:155-170` and its `build()` call at
  `:212`. The live function hashes identifier-like tokens from selected text files; the caller uses
  results only to label unresolved import spellings. `scripts/test_ps4_dynlink.py` tests `nid()` and
  synthetic ELF parsing but has no `name_by_hash()` coverage.
- Claimed `RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01`, ACTIVE with zero failed implementation tries.
  Bounded scope is a synthetic temporary text tree and direct assertions that a requested known NID
  resolves to its token while an absent NID is omitted. Test-only edit to
  `scripts/test_ps4_dynlink.py`; no parser/build changes or real source-tree/game behavior claim.
- Claim checkpoint: [name-by-hash fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-name-by-hash-claim.md).
  Prerequisite `RE-02-DYNLINK-01` is FIXTURE_VERIFIED. Acceptance command:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Claim publication: commit `fe7446f58d87bad444c3cec18196d9ac09442184` is pushed to `origin/main`;
  staged audit accepted four paths, full tracked-path audit accepted 1,513 paths, and root verified
  HEAD equals `origin/main`. `python3 scripts/project_index.py --check` reports 14,218 folders
  current; `trail build --check` reports the wiring graph in sync (3,527 nodes / 4,739 edges), with
  the deep meaning layer still unbuilt. Private command logs are in
  `local/checkpoints/root-round12-dynlink-claim/`.
- Next exact action: assign only `scripts/test_ps4_dynlink.py` to `launcher_review`, independently
  replay the focused suite, record the fixture result, and publish completion. Keep
  `native/api/CMakeLists.txt` and `native/api/host/` excluded; preserve all emulation/module and real
  game acceptance gates.

## Session addendum 2026-10-21 round 13 (`/root`)

- Completed `RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01`, now FIXTURE_VERIFIED with zero failed
  implementation tries. Worker `launcher_review` and root each pass
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (19/19). The test creates a
  temporary `fixture.cpp` containing `synthetic_export_name`, asserts the requested NID resolves to
  that token, and asserts an absent requested NID is omitted. E137 and
  [the completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-name-by-hash-complete.md)
  retain log/test/source hashes and the spelling-only boundary.
- Only `scripts/test_ps4_dynlink.py` changed in the worker implementation; production
  `scripts/ps4_dynlink.py` is unchanged. `trail build --check` passes with the new test indexed
  (3,528 nodes; deep meaning layer remains unbuilt). No import semantics, loader, or game gate is
  established.
- Completion publication: commit `cfebea9848583ed1df9124a869e99379e65f988a` is pushed to
  `origin/main`; staged audit accepted six paths and full tracked-path audit accepted 1,514 paths.
  Root verified HEAD equals `origin/main`; the folder index reports 14,219 folders current and
  `trail build --check` passes with 3,528 nodes. Next exact action: continue with a bounded unclaimed
  task from the independent scouts. Keep the existing host escalation and slot-23/Ghidra blocker
  unchanged.

## Session addendum 2026-10-21 round 14 (`/root`)

- Claimed two independent synthetic test-only tasks: `DATA-02-PSARC-BLOCK-TABLE-WIDTH-01` and
  `DOC-FOLDER-INDEX-CHECK-READONLY-01`, both ACTIVE with zero failed implementation tries. Claims
  and dependencies are recorded in `docs/TASKS.md`; detailed evidence is in the two claim
  checkpoints named below.
- Graft confirms `Psarc._load()` computes `self.width = max(1, ((self.block_size - 1).bit_length() + 7) // 8)` and decodes fixed-width big-endian block entries. The synthetic PSARC builder in `scripts/test_psarc.py` fixes `BS = 16` and serializes `blocks` with `bytes(blocks)`, so existing fixtures exercise only one-byte entries. Planned fixture uses block size 257 and a 256-byte raw payload whose table value is 256 (`0x0100`), asserting width 2 and successful roundtrip.
- `project_index.generate(root, check=True)` skips directory creation, checks both generated files, and raises `ValueError` when either is missing. Existing test coverage calls `generate(root)` first, so it does not prove the empty-root check avoids writes. Planned test asserts exact stale-index error and confirms neither `docs/` nor `local/` is created.
- Claim checkpoints: [PSARC block width](docs/checkpoints/2026-10-21-root-data02-psarc-block-table-width-claim.md) and [folder-index check mode](docs/checkpoints/2026-10-21-root-doc-folder-index-check-readonly-claim.md). Dependencies: `DATA-02-PSARC-PARTIAL-READ-01` is FIXTURE_VERIFIED; folder-index task has no prerequisite. Acceptance commands are `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` and `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Worker assignments after claim publication: `launcher_review` owns only `scripts/test_psarc.py`; `slot23_raw_xrefs` owns only `scripts/test_project_tools.py`. Root retains sole ownership of handoff/task/evidence records. Claim commit `f427b2bb412379a54feb7e5f3af27420420a31da` and publication-record commit `95642f017d58c2fccaf1cd2398a32077230a5d8d` are on `origin/main`; staged audits accepted five paths and then three follow-up paths, full tracked-path audits accepted 1,516 files, the folder index reports 14,222 folders current, and `trail build --check` passes with 3,528 nodes. Root verified HEAD equals `origin/main`. Preserve the escalated native host paths outside both tasks and commits.

## Session addendum 2026-10-21 round 15 (`/root`)

- Completed `DATA-02-PSARC-BLOCK-TABLE-WIDTH-01`, FIXTURE_VERIFIED: the builder now emits width-sized table entries for configurable block size; the synthetic block-size-257 fixture uses `bytes(range(256))`, asserts width 2 and parsed value 256 (`0x0100`), then verifies exact readback. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` (31/31). E138 and [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-block-table-width-complete.md) record hashes/logs. Production `scripts/psarc.py` is unchanged.
- Completed `DOC-FOLDER-INDEX-CHECK-READONLY-01`, FIXTURE_VERIFIED: an empty temporary root in check mode raises the exact two-output stale-index error and creates neither `docs/` nor `local/`. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` (8/8). E139 and [completion checkpoint](docs/checkpoints/2026-10-21-root-doc-folder-index-check-readonly-complete.md) record hashes/logs. Production `scripts/project_index.py` is unchanged.
- Graph checkpoint: `trail build --check` first reported stale after these two new test methods; `trail build` exited 0 and rebuilt 3,530 nodes / 4,746 edges; subsequent `trail build --check` exited 0. Deep meaning layer remains unbuilt. Stale output, build, and passing check are under `local/checkpoints/root-round13-claims/`.
- Completion publication: commit `42af4f281e2d310024bd2ce1f99a3784bae2371e` is on `origin/main`;
  staged audit accepted eight paths and the full tracked-path audit accepted 1,518 paths. Root
  verified HEAD equals `origin/main`. `python3 scripts/project_index.py --check` reports 14,222
  folders current; `trail build --check` passes with 3,530 nodes / 4,746 edges. Next exact action:
  continue with the independent scout's candidate if still unclaimed. All real game acceptance gates
  remain unchanged.

## Session addendum 2026-10-21 round 28 (`/root`)

- Published the round 27 raw-DEFLATE fixture and zero-length mapped-range claim in commit `fd601388d2205cf7403e3796260d6fa7d51b40fd` (`test: cover raw inflate offset zero and claim empty range`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round27-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,232 folders current); `trail build --check` (3,535 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (8 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,534 paths). Post-push `git status --short --branch` showed `main` synchronized; only the pre-existing native host worktree paths remain outside the commit.
- `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01` is `FIXTURE_VERIFIED`; `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01` is `ACTIVE`. Both have zero failed tries. Actual game acceptance gates remain open.
- `launcher_review` now owns only `scripts/test_ps4_dynlink.py` for the zero-length boundary fixture. `slot23_raw_xrefs` has been asked to scout one more distinct test module. Next exact action: review the dynlink test patch, root-replay `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`, and record the result.

## Session addendum 2026-10-21 round 27 (`/root`)

- Completed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` (21/21). A 64-byte synthetic payload compressed with raw DEFLATE selects `raw_inflate_offset0` and decodes to 64 bytes; test SHA256 `accbc01ef1b55b7f832d84c0ece73eb2bab97200bdb05f305c35604be8e8c7c1`; classifier unchanged at `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`. E145 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-classifier-raw-inflate-offset0-complete.md) record both log hashes.
- Claimed `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01`, `ACTIVE` with zero failed tries. Graft and live source review confirm that `Image.mapped()` includes a zero-length range at the first segment's one-past-end address `0x1010`; no existing test pins this. Add one synthetic assertion in `scripts/test_ps4_dynlink.py`, without changing parser behavior. Claim: [zero-length mapped range](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-zero-length-claim.md).
- `slot23_raw_xrefs` completed only `scripts/test_audio_headers.py`; `launcher_review` will own only `scripts/test_ps4_dynlink.py` after this claim is published. Root owns shared documentation.
- At this checkpoint the next action was to publish E145 and the zero-length claim; publication and worker assignment completed in round 28 above.

## Session addendum 2026-10-21 round 26 (`/root`)

- Published the round 25 RELRO fixture and raw-DEFLATE claim in commit `62e2b677eee5ae4136d9c9bc672eff6e2e825566` (`test: verify RELRO image reads and claim raw inflate branch`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round25-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,231 folders current); `trail build --check` (3,534 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (7 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,531 paths). The post-push `git status --short --branch` showed `main` synchronized; only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain outside the commit.
- `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01` is `FIXTURE_VERIFIED`; `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01` is `ACTIVE`. Both have zero failed tries. All actual game acceptance gates remain open.
- At this checkpoint the next action was to review and replay the raw-DEFLATE fixture; this completed in round 27 above.

## Session addendum 2026-10-21 round 25 (`/root`)

- Completed `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (21/21). The synthetic `PT_SCE_RELRO` segment maps and reads its exact bytes; test SHA256 `dbac8eeb97a1fe98e7c9e5c3a16bfe4b133127214f5da5eda1849aa3ccb3efec`; parser unchanged at `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E144 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-relro-image-complete.md) record both log hashes.
- Claimed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`, `ACTIVE` with zero failed tries. Graft located the raw DEFLATE offset-0 probe branch with no matching test. Add one synthetic `wbits=-15` success fixture in `scripts/test_audio_headers.py`; codec identity and playback remain out of scope. Claim: [raw DEFLATE offset-0 fixture](docs/checkpoints/2026-10-21-root-aud01-classifier-raw-inflate-offset0-claim.md).
- `launcher_review` completed only `scripts/test_ps4_dynlink.py`; `slot23_raw_xrefs` will own only `scripts/test_audio_headers.py` after claim publication. Root owns the shared documentation.
- At this checkpoint the next action was to publish E144 and the new claim; publication and worker assignment completed in round 26 above.

## Session addendum 2026-10-21 round 23 (`/root`)

- Published the round 22 CRLF fixture and RELRO claim in commit `91d58a255b0e94dc271de24d43a1fd2ec0bc76c0` (`test: verify CRLF manifest records and claim RELRO coverage`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round22-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,230 folders current); `trail build --check` (3,533 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (7 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,528 paths). The post-push `git status --short --branch` showed `main` synchronized; only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain outside the commit.
- `DATA-02-ARCHIVE-MANIFEST-CRLF-01` is `FIXTURE_VERIFIED` with zero failed tries. `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01` is `ACTIVE` with zero failed tries. All real-game acceptance gates remain open.
- At this checkpoint the next action was to review and replay the RELRO fixture; this completed in round 25 above.

## Session addendum 2026-10-21 round 22 (`/root`)

- Completed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` (20/20). The synthetic two-record CRLF fixture preserves exact paths and sizes; test SHA256 `07ab59c0500a99ad13d3d0c15d63e525a06061ccef34fe734ab5e76b796665b7`; parser unchanged at `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`. E143 and [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-complete.md) record both logs.
- Claimed `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01`, `ACTIVE` with zero failed tries. Scout's Graft search found the `PT_SCE_RELRO` constant/header filter but no matching test. Add one synthetic `Image` fixture that checks mapped/read on a `PT_SCE_RELRO` segment; do not change parser behavior. Claim: [RELRO image fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-relro-image-claim.md).
- `review_host_proto` completed the CRLF test file; `launcher_review` will own only `scripts/test_ps4_dynlink.py` after this claim is published. Root owns all task/evidence/handoff documentation.
- At this checkpoint the next action was to publish E143 and the RELRO claim; publication and worker assignment completed in round 23 above.

## Session addendum 2026-10-21 round 21 (`/root`)

- Published the round 20 batch in commit `d19067e952736880d643b9d521d9eaaa5316b455` (`test: record mapped-range fixture and manifest CRLF claim`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round20-publication.md).
- Pre-commit checks: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` exited 0 (14,230 folders indexed/current); `trail build --check` exited 0 (wiring graph in sync; the optional deep layer is not built); `git diff --cached --check` exited 0; `python3 -I scripts/audit_publish_paths.py` exited 0 for 9 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,525 paths. Logs are under `local/checkpoints/root-round20-claim/`.
- Post-commit `git status --short --branch` showed `main` synchronized with `origin/main`. The only remaining changes are the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/`; neither was staged or published. A post-commit staged audit reported no staged files (exit 2); the staged audit immediately before commit passed.
- `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01` and `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01` are `FIXTURE_VERIFIED`; `DATA-02-ARCHIVE-MANIFEST-CRLF-01` is claimed `ACTIVE`. All have zero failed tries. Actual game boot/audio/save-load/Metal acceptance gates remain open.
- `review_host_proto` now owns only `scripts/test_archive_overlap_map.py` for the claimed CRLF fixture. Next exact action: review its patch, root-replay `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`, and record the result.

## Session addendum 2026-10-21 round 20 (`/root`)

- Completed `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01`, FIXTURE_VERIFIED with zero failed tries. The synthetic two-segment fixture asserts a contained mapped range is true, while a range crossing the first segment end and a nonzero range starting in the gap are false. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (20/20); test SHA256 `f779cc8f7983c20efed84f2c17f35415f8daa718f8a812599986747b3b261695`; production parser unchanged at `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E141 and [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-range-complete.md) record both log hashes. No live ELF or loader claim.
- Completed `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01`, FIXTURE_VERIFIED with zero failed tries. `docs/ASSETS.md` now accurately says “18 synthetic test methods”; the count command returns 18 and the test suite passes 18/18. `docs/ASSETS.md` SHA256 `ca0e0e495ef02a88a56a52589dda8d790c018162ff009d9ee5463558e9b85f87`; E142 and [completion checkpoint](docs/checkpoints/2026-10-21-root-doc-assets-aud01-xff-count-complete.md) record worker/root logs. This is a documentation count correction only; no codec or playback claim.
- Claimed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, ACTIVE with zero failed tries. The parser has an explicit CRLF strip at `scripts/audit_archive_overlap_map.py:66`; current tests cover LF and blank records but not CRLF. Add one synthetic two-record CRLF fixture to `scripts/test_archive_overlap_map.py`. Claim: [CRLF manifest fixture](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-claim.md). After its claim publication, `review_host_proto` will own only that test file; root owns all documentation.
- At this checkpoint the next action was to publish E141/E142 and the CRLF claim; publication completed in round 21 above.

## Session addendum 2026-10-21 round 19 (`/root`)

- Claimed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, ACTIVE with zero failed tries. Graft's `trail grep "line.endswith" --in scripts/` found the parser's explicit CRLF branch at `scripts/audit_archive_overlap_map.py:66`. Live tests in `scripts/test_archive_overlap_map.py` use LF records and do not cover CRLF. Add a synthetic two-record CRLF fixture that asserts exact path/size output; parser behavior and real manifest interpretation are out of scope. Parent `DATA-02-ARCHIVE-OVERLAP-MAP-01` is ARTIFACT_VERIFIED. Claim: [CRLF manifest fixture](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-claim.md).
- The implementation owns only `scripts/test_archive_overlap_map.py`; it is separate from round 18's `scripts/test_ps4_dynlink.py` and `docs/ASSETS.md`. `review_host_proto` will own the file after publication; root retains all task/evidence/handoff documentation.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`. Expected result: CRLF-delimited JSONL records parse to their exact declared paths and sizes. No test has run at claim time; this does not establish archive contents or runtime behavior.
- Next exact action: publish the claim, then assign its test-only file to `review_host_proto` and root-replay the focused suite.

## Session addendum 2026-10-21 round 18 (`/root`)

- Claimed `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01`, ACTIVE with zero failed tries. The scout's Graft query `trail grep "mapped" --in scripts/` found `Image.mapped()` and only one existing direct test assertion, for a mapped BSS-tail range. `two_segment_image()` already supports checking a range contained in one segment, a range crossing its end into a gap, and a nonzero range beginning in the gap. Add test-only assertions in `scripts/test_ps4_dynlink.py`; do not change parser/runtime behavior. Dependencies `RE-02-DYNLINK-01` and `RE-02-DYNLINK-SEGMENT-SPAN-01` are FIXTURE_VERIFIED. Claim: [mapped-range fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-range-claim.md).
- Claimed `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01`, ACTIVE with zero failed tries. `docs/ASSETS.md` says “six synthetic fixtures,” while `scripts/test_audio_bank_header.py` currently has 18 test methods (`rg -c '^    def test_' ...` → 18). Correct the prose to “18 synthetic test methods” in `docs/ASSETS.md` only. XFF characterization, reader identity, and exact-path-cardinality dependencies are FIXTURE_VERIFIED. Claim: [XFF test-count documentation correction](docs/checkpoints/2026-10-21-root-doc-assets-aud01-xff-count-claim.md).
- Both claim tasks are file-disjoint from each other and the completed audio-header fixture. After this claim publication, `launcher_review` will own only `scripts/test_ps4_dynlink.py`; `slot23_raw_xrefs` will own only `docs/ASSETS.md`. Root retains sole ownership of `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and checkpoints. Their acceptance commands and no-runtime-claim boundaries are recorded in the claim files.
- Claim publication: `3709a5da33f066cbb6936935545f86ec73ccd409` is on `origin/main`; `git push origin main` exited 0 and advanced `517defa..3709a5d`. This commit publishes E140, both claim records, and both active task rows. Staged audit accepted eight paths; full tracked-path audit accepted 1,522 paths. Folder index check reports 14,227 current; `trail build --check` passes with 3,531 nodes; `git diff --check` passes. HEAD and `origin/main` both resolve to this commit. The prior audio-claim publication record is `517defab025c6e2c8ef84330d4fbe86fd4fda5eb`. Logs are under `local/checkpoints/root-round18-claims/`. Only the preexisting escalated `native/api/CMakeLists.txt` and `native/api/host/` remain outside the published scope.
- Next exact action: dispatch the two published tasks to their assigned workers, then independently replay their acceptance commands and publish completion evidence.

## Session addendum 2026-10-21 round 17 (`/root`)

- Completed `AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01`, FIXTURE_VERIFIED with zero failed tries. `scripts/test_audio_headers.py` adds a 64-byte synthetic payload compressed with zlib and prefixed with `b'\x00\x01'`; it asserts successful `zlib_offset2` selection and exact decoded length. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` (20/20); test SHA256 `888a6abcf077d6a3d9f861bf7d80acb163540fbf873d7b288b6fb45beaca8c45`; classifier source unchanged at SHA256 `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`. E140 and [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-classifier-zlib-offset2-complete.md) record both log hashes. Synthetic branch behavior only; no real codec, game asset, or playback claim.
- Next exact action: complete the next independent bounded task after confirming no file-ownership overlap; all game-level audio and playback gates remain open.

## Session addendum 2026-10-21 round 16 (`/root`)

- Claimed `AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01`, ACTIVE with zero failed tries. Graft found `decompress_probe()`'s `zlib_offset2` branch in `scripts/classify_audio_headers.py:81-127`; live test coverage accepts zlib at offset 0 but has no offset-2 regression. Add one synthetic prefixed-zlib fixture to `scripts/test_audio_headers.py`; production code and real archive interpretation are out of scope. The root independently checked that no matching task row existed before the claim.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`. Expected assertion: a two-byte prefix followed by `zlib.compress(payload)` is reported as `zlib_offset2` and returns the expected decoded length. No test has run at claim time. Claim checkpoint: [zlib offset-2 fixture](docs/checkpoints/2026-10-21-root-aud01-classifier-zlib-offset2-claim.md). Root retains sole ownership of `HANDOFF.md`, `docs/TASKS.md`, and evidence records; `review_host_proto` will own only `scripts/test_audio_headers.py` after claim publication. The fixture demonstrates synthetic classifier behavior only, not codec identity, game asset use, or playback.
- Claim publication: `6685c7f73b800e49b9905cc1b2e289b20a7f087c` is on `origin/main`; `git push origin main` exited 0 and advanced `59dad26..6685c7f`. Staged audit accepted four paths; full tracked-path audit accepted 1,519 paths. `python3 scripts/project_index.py --check` reports 14,224 folders current, `trail build --check` passes with 3,530 nodes / 4,746 edges, and `git diff --check` passes. HEAD and `origin/main` both resolve to the claim commit. Publication logs are under `local/checkpoints/root-round14-aud01-claim/`. Next exact action: dispatch `scripts/test_audio_headers.py` to `review_host_proto`, then root replays and publishes the result.

## Session addendum 2026-10-21 round 10 (`/root`)

- `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01` is FIXTURE_VERIFIED with zero failed implementation
  tries. Worker and root each pass 6/6; E132 records the expected pre-guard failure, both passing
  logs and source hashes. Completion and claim records were pushed in `c8dd0b3`; no game gate advanced.
- AUD-PUBLISH-SECRET-NAME-FIXTURE-01 is FIXTURE_VERIFIED with zero failed tries. Worker and
  root each pass 7/7 (E133); the test-only cases cover nested .ENV.local and CREDENTIALS.txt
  names. Completion and evidence were pushed in 8d463ab; the production audit stayed unchanged.
- Graft confirmed the deny regex in `scripts/audit_publish_paths.py:30-42`; current tests cover
  nested forbidden components and file extensions but no `.env` or `credentials` names. Both
  prerequisite audit tasks are FIXTURE_VERIFIED.
- Acceptance command: PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py.
  Worker and root each pass 7/7; source graph is rebuilt and in sync after the new test method.
  The completion checkpoint records private log and source hashes.
- Next exact action: select and claim another bounded, non-escalated task. Preserve escalated host paths and the slot-23 blocker.
  then record E133 and publish the Task B completion checkpoint. Escalated host paths remain out.
- The secret-name claim checkpoint was reviewed, contains no credentials, and is published in c8dd0b3 despite the .gitignore:76 *secret* rule. The worker's test-only scope is now complete.
  `c8dd0b3` despite the `.gitignore:76` `*secret*` rule. The test-only work is assigned to
  `launcher_review` and is in progress.

## Session addendum 2026-10-21 round 9 (`/root`)

- Completed and pushed `DOC-FOLDER-INDEX-ADMIN-DIR-01` in commits `94a8cbc` and `6811f52`.
  Worker and root each pass the synthetic nested `.git`/`__pycache__` check 5/5; E131 records the
  log hashes. No source behavior or live inventory changed.
- Claimed `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`, ACTIVE with zero failed implementation tries.
  `unwrap()` can write `destination.with_suffix(".identity.json")` over the source if that sidecar
  aliases the input; the task adds a pre-write same-file guard and a synthetic regression.
  Claim: [docs/checkpoints/2026-10-21-root-re01-unwrap-input-sidecar-collision-claim.md](docs/checkpoints/2026-10-21-root-re01-unwrap-input-sidecar-collision-claim.md).
- Graft located the unguarded sidecar write at `scripts/unwrap_self.py:27` and showed callers from
  the three existing wrapper tests plus the CLI. Prerequisites `RE-01` and
  `RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01` are satisfied.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. Worker
  will first record a pre-guard failing collision assertion, then add the guard and rerun; the
  expected red fixture is not counted as a failed implementation try. No test has run for this task.
- Claim commit `90a5d99` is pushed to `origin/main`; `git push origin main` exited 0 and advanced
  `6811f52..90a5d99`. Root verified `HEAD` and `origin/main` both resolve to
  `90a5d99413dfe53236828f3f963121ac7045186c`. Next exact action: assign
  `scripts/unwrap_self.py` and `scripts/test_project_tools.py` to `slot23_raw_xrefs`. Root remains
  the sole integrator for handoff/task/evidence documentation. Keep all user game files read-only
  and host escalation files unstaged.

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
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. Worker and
  root each pass 5/5. E131 and the completion checkpoint record the exact log hashes and test-only
  limits. `slot23_raw_xrefs` owned only `scripts/test_project_tools.py`; root remained the sole
  `HANDOFF.md` integrator.
- Claim commit `51f6549` is pushed to `origin/main`; `git push origin main` exited 0 and advanced
  `fb28481..51f6549`. Root verified `HEAD` and `origin/main` both resolve to
  `51f654923eb76ba2d10c6fcf88f222089ded84e9`. `slot23_raw_xrefs` is now assigned only
  `scripts/test_project_tools.py`; implementation and acceptance result are pending.
- During implementation, `trail build --check` reported the expected stale graph after the test file
  gained its new test symbol. `trail build` then exited 0 and the subsequent check passed; the graph
  remains in sync. No production code changed. Keep escalated `native/api/CMakeLists.txt` and
  `native/api/host/` outside this task and all commits.
- Final folder index after adding the completion checkpoint: `python3 scripts/project_index.py`
  exited 0 with 14,208 folders; `docs/checkpoints` now reports 236 files. `trail build --check`
  exited 0 with the wiring graph in sync (deep layer remains unbuilt).
- Publication gates pass: the staged path audit accepts 7 allowlisted files; the full tracked path
  audit accepts 1,502 paths; `git diff --cached --check` is clean. Escalated host files remain
  unstaged. Completion commit `94a8cbc` was pushed to `origin/main`; the push advanced
  `51f6549..94a8cbc`. `HEAD` and `origin/main` both resolve to
  `94a8cbc922a32ab37d8215cbac16d98c6f56077f`.
- Next exact action: select and claim another bounded, non-escalated project task. Preserve the
  three-try host escalation and the slot-23/Ghidra blocker; no game gate advanced from this fixture.

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
