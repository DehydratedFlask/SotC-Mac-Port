# GPU-02 raw default-material selector — claim

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
