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
- Status: ACTIVE / attempt 0. Implementation tests are NOT RUN. Documentation claim is ready for
  allowlisted commit and push to `main`.
