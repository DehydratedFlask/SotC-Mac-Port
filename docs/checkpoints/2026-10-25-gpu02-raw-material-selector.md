# GPU-02 raw default-material selector — claim

## 2026-10-25 11:38 CDT — post-publication task triage

- Read-only scouts found no existing READY/unblocked follow-up in the Metal/raw-asset queue or the
  startup queue. The TEX0 format-6 task remains UNKNOWN from E662 pending new authoritative evidence;
  the owner decision to defer `0x8C` remains in force (E665).
- Startup's first gap remains slot 306 / RVA `0xa633b0`. E676 records an 8,461-byte body, 11
  unresolved indirect calls, and no proven incoming direct code-call edge. Slot 307 is present at
  `0xa5b090`, but surveying it now would skip the first unresolved gap. The scouts read project docs
  only; they did not read executable bytes, edit source, or run tests.
- Next task needs an owner choice: (a) a bounded analysis following one selected unresolved edge for
  slot 306, or (b) a broader constructor-contract analysis. No new task was claimed. Ask before
  creating the task card or starting source/reverse-engineering work.

## 2026-10-25 11:34 CDT — source and evidence published to main (E688)

- Commit `e6cc690419e0d7acd278041f562115277f9186b0` pushed to `origin main` (exit 0).
  `git fetch origin main` confirms `HEAD == origin/main`; the worktree is clean on `main`; no open
  PRs remain; remote query confirms the old `cursor/init-hash-551140-9dfc` branch is absent.
- Author/committer: Johnny Wang (`dehydratedflask@users.noreply.github.com`). The commit message has
  no Claude, Anthropic, or co-author trailer. Staged/all-path audits passed 9/2,802, whitespace and
  attribution checks passed. The private publication log is
  `local/validation/gpu02-raw-material-selector-20261025/publication.log`.
- `GPU-02-RAW-MATERIAL-SELECTOR-01` is **PUBLISHED / FIXTURE_VERIFIED / ARTIFACT_VERIFIED** within
  its exact raw-member and E627 replay scope. `.cmat` engine consumption/constants, retail bindings,
  mesh/texture interpretation, `0x8C`, game-driven frame and G3 remain UNKNOWN/PARTIAL.
- There is no next READY task in the existing Metal/raw-asset queue. Exact next action is to use the
  read-only startup task triage, then claim only an unblocked bounded task from `docs/TASKS.md`.

## 2026-10-25 11:31 CDT — implementation, replay and review complete

- Integrator: Codex `/root`; implementation worker `/root/gpu02_raw_selector` owned only the two
  `tools/dev/` files; independent reviewer `/root/review_gpu02_selector` read the source/tests and
  found no remaining findings. Current branch is `main` at `71c578a9b56d4e05bf09c0735ef3ba861c61ca14`
  before publication.
- Source hashes: verifier `739338e40ece220924c4e79e5ce088727d167b0fb8828c388cb00d1bd6711930`;
  tests `828552b074da842c4bb0e265fcfb064566f6e028ce3668c4b2f52b4852337bbe`.
- Exact root command `python3 -I tools/dev/test_raw_material_selector.py` passed **16/16**;
  `local/validation/gpu02-raw-material-selector-20261025/root-final-unit-tests.log` SHA-256
  `129723ba0f4d933bd2e4eba413631bb09bfaf3ac28a2f366a8d71bd0ddd5b975`.
- Exact root replay command passed with
  `--cachedfiles local/extracted/CUSA08809/cachedFiles.psarc --pak0
  local/extracted/CUSA08809/pak0.psarc --output
  local/validation/gpu02-raw-material-selector-20261025/root-final-replay`. It pins both exact
  PSARC entries and full member hashes, proves the one selector literal at cmat byte 37, parses the
  exact CSDR and matches E627 spans, regenerates the exact MSL, passes all 2,141 oracle cases / 34,256
  components with max absolute error 0, compiles a fresh arm64 Metal verifier and executes it with
  Metal API/shader validation. Summary/log SHA-256
  `3111ff56fa74685b54590eaa9081e694af970a2e2f404eda209b841722203788`.
- Repository gates: `cmake -S native -B local/build/orch-r22` and
  `cmake --build local/build/orch-r22 -j 12` exited 0; CTest **397/397** (8.59 s); lift checkers
  **481/0/0**; `trail build --check` passed at **8,623** nodes; `project_index.py` generation and
  `--check` both passed at **20,429** folders. The first two index checks were stale because two
  validation log files were created after generation; after logs stabilized, generation/check
  passed. Logs are under `local/validation/gpu02-raw-material-selector-20261025/`.
- One root invocation was rejected because the output directory had already been created; removed
  that empty directory and reran successfully. This tests the verifier's no-overwrite guard and is
  not a semantic failure. Earlier worker test-fixture setup corrections and their final pass are in
  the prior E684 section. Total semantic failures for this task: **0**.
- Observed: exact bytes, selector literal, hashes, parser spans, MSL/oracle output, Metal execution,
  test counts, source review and repository gates. Inference not made: engine consumption of this
  material. `.cmat` constants/layout, retail ABI/bindings, all `0x8C` members, mesh/texture semantics,
  game frame and G3 remain UNKNOWN or PARTIAL.
- Files for this checkpoint: two `tools/dev/` files, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/METAL_FRAME_PATH.md`, `docs/FOLDERS.md` (generated),
  and this file. No install/dependency change. Private inputs and every generated binary/report/log
  remain under ignored `local/`.
- Next exact action: stage only allowlisted files; run staged and full path audits plus
  commit-attribution audit; commit/push to `origin main` under Johnny Wang
  without Claude/Anthropic contributor trailers; fetch and verify `HEAD == origin/main`, then
  record the publication hash in HANDOFF/EVIDENCE.
- Status: **FIXTURE_VERIFIED / ARTIFACT_VERIFIED**, publication pending; attempt 0 semantic failures.

## 2026-10-25 11:02 CDT — bounded task claimed

- Integrator: Codex `/root`; task `GPU-02-RAW-MATERIAL-SELECTOR-01`; local branch `main`, base
  `d6d2f0782c877737f1af41b944ed88b4f754767b`, matching `origin/main` at claim time. Attempt 0; no
  failed hypotheses.
- Owner-approved scope: consume raw material/shader asset members from the local extracted PSARCs
  with the documented import path; keep extracted members, logs, and generated outputs under ignored
  `local/`. Continue raw-asset work only. The `0x8C` decoder is deferred. This is an offline
  selector/importer fixture and makes no game-frame, runtime, G3, boot, audio, or save/load claim.
- Input pins from `docs/METAL_FRAME_PATH.md` §4.6:
  - `cachedFiles` path `coredata/enginesupport/materials/_ps4/default.cmat`, 171 bytes,
    SHA-256 `0c2233c1aab20cfc1da914de96cfd1cbd0a20e37964024d54e10e36b260c6d29`.
  - `pak0` path
    `materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr`, 1,988
    bytes, SHA-256 `c4214e4ee0852a962ce56e0cab0befbe3169698cb354bc62afa7eab4d1f3f356`. The documented
    `_shipping` twin in `pakN` is not a substitute for the pinned `pak0` member.
- Required result: prove that the exact path bytes in the exact pinned `default.cmat` select the
  exact pinned CSDR; resolve each archive member through `Psarc.entry_index(path)` and `read(index)`,
  never positional manifest pairing; run the existing raw `.csdr` structural parser; match default
  VS code offset 564 / length 272 / BinaryInfo offset 972 and PS offset 1368 / length 240 /
  BinaryInfo offset 1744 to E627; then reproduce the already-verified MSL and CPU-oracle baseline.
  Missing, ambiguous, changed, truncated, unsupported, or structurally inconsistent input must
  fail closed. If the material selector cannot be proved from bytes, report `UNKNOWN`; do not infer
  constants, descriptors, or bindings.
- Explicit exclusions: `.cmat` constants/layout, mesh/texture decoding, NRM0/TEX0, shader resource
  ABI, `0x8C`, Metal runtime behavior beyond the already-verified E627 replay, and game-driven
  rendering. Do not edit `native/tests/test_metal_texture_binding.cpp`; keep the work in a new
  verifier/test pair under `tools/dev/` unless Graft shows a necessary existing call site.
- Files changed in claim checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  `docs/INDEX.md`, `docs/METAL_FRAME_PATH.md`, regenerated `docs/FOLDERS.md`, and this checkpoint.
  No source, dependency, or private asset file changed; no implementation test has run.
- Exact discovery commands: `git status --short --branch` and `git rev-parse HEAD origin/main` both
  confirmed clean `main` at `d6d2f0782c877737f1af41b944ed88b4f754767b`; `trail ask` located the
  structural CSDR parser, existing default oracle, fixed shader lowerer, and PSARC API; `trail
  skeleton scripts/psarc.py` confirmed `Psarc.entry_index` and `Psarc.read`. These are discovery
  results, not implementation verification.
- Repository bookkeeping: `trail build --check` exited 0; wiring graph is in sync at 8,571 nodes
  (deep concept layer not built), log `local/validation/gpu02-raw-material-selector-claim-20261025/trail-build-check.log`.
  `python3 scripts/project_index.py` exited 0 with 20,421 folders and `python3 scripts/project_index.py
  --check` exited 0; logs are `local/validation/gpu02-raw-material-selector-claim-20261025/project-index.log`
  and `project-index-check.log`.
- One initial `project_index.py --check` exited 1 because its own newly-created check log changed the
  private folder file count after regeneration (`local/indexes/folders.json` was stale). After all
  log files existed, the index was regenerated and the final check passed; this was sequencing, not
  an implementation or index-content defect.
- Observation versus inference: the exact member paths, sizes, hashes, CSDR spans, and target string
  are documented artifact facts. Byte-level selector validation and baseline replay remain NOT RUN.
- Risks: local archive/member availability and the format's selector semantics still need byte-level
  confirmation. The documented target string does not prove the engine consumes it as a selector.
- Next exact action: write an isolated raw-member verifier and tests that resolve the two exact PSARC
  entries, hash-gate both complete members, prove the target string occurs exactly once in the cmat,
  parse and match the CSDR spans, and compare generated MSL/oracle outputs against E627; stop as
  UNKNOWN on unsupported or ambiguous material bytes.
- Publication: commit `6c6bdfd5badae7cb2cd74b432bf2d9d42b38ddd4` (`docs: claim raw default material
  selector verification`) was pushed to `origin main`; `git fetch origin main` and
  `git rev-parse HEAD origin/main` confirm both refs at that commit; checkout is clean. Precommit
  audits passed 7 staged paths, 2,800 all paths, whitespace, and attribution. The postcommit staged
  audit found no staged paths (exit 2); explicit audit of the 7 committed paths and the full tree
  then passed. Attribution passed for `origin/main..HEAD` before push. Audit log:
  `local/validation/gpu02-raw-material-selector-claim-20261025/publication.log`. Author and committer
  are Johnny Wang; no Claude/Anthropic or assistant co-author trailer.
- Status: ACTIVE / attempt 0. Implementation tests are NOT RUN. Claim publication is complete;
  source implementation remains the next action.

## 2026-10-25 11:11 CDT — raw material and shader bytes verified (E684)

- Private import/probe by `/root/gpu02_raw_selector`; root independently replayed the local bytes
  against the live §4.6 pins and `parse_raw_csdr`. No public source files were added yet.
- Exact MD5(path) PSARC result for the material: `cachedFiles.psarc`, entry 23809,
  `coredata/enginesupport/materials/_ps4/default.cmat`, entry/path MD5
  `96452e65ef0730502ba802bdd1029f91`, 171 B, SHA-256
  `0c2233c1aab20cfc1da914de96cfd1cbd0a20e37964024d54e10e36b260c6d29`.
- Exact MD5(path) PSARC result for the shader: `pak0.psarc`, entry 3300,
  `materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr`, entry/path
  MD5 `79f789146d1bf565cf8f3e289a66233f`, 1,988 B, SHA-256
  `c4214e4ee0852a962ce56e0cab0befbe3169698cb354bc62afa7eab4d1f3f356`.
- The exact ASCII selector `$/materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr`
  occurs once in the cmat at byte offset 37; a follow-up scan found one `.csdr` suffix. The existing
  structural parser returns VS `(type 1, code 564, length 272, info 972)` and PS
  `(type 0, code 1368, length 240, info 1744)`, matching E627. Probe JSON SHA-256
  `ab6e7bc3630f5e6ca254bdfd20bc5ef88a699245a6420ea78142eed80b64803f` is at
  `local/validation/gpu02-raw-material-selector-20261025/probe.json`; raw bytes and probe remain
  ignored under `local/validation/gpu02-raw-material-selector-20261025/`.
- Root replay exited 0 and confirmed both pins/sizes, unique selector offset and equality between the
  probe program rows and `parse_raw_csdr`. The root's preliminary inline checker needed corrections
  for a copied cmat digest, the nested probe JSON schema, and selecting the hash-bearing table row
  instead of an earlier prose mention; after correction the replay passed. The worker also retained
  two tooling corrections: an expected-digest typo stopped before writing bytes, then a report
  formatter called `.hex()` on the string-valued PSARC MD5 after cmat extraction. None of these
  failures tested selector semantics; failed semantic hypotheses remain 0.
- Observation versus inference: member identity, hashes, path bytes, literal occurrence and container
  spans are observed and root-replayed. The engine consuming the literal as a selector is NOT proven;
  cmat constant/layout semantics remain UNKNOWN. E627 MSL/oracle replay and the new verifier/tests
  are NOT RUN.
- Next exact action: implement the isolated verifier/test pair under `tools/dev/`, including exact
  path lookup, pin checks and fail-closed mutations, then reproduce the pinned E627 MSL SHA and
  34,256-component oracle result. No shared Metal test or game-frame claim.
- Status: raw-member/selector artifact subtask **ARTIFACT_VERIFIED**; parent task ACTIVE, attempt 0
  semantic failures.
