# Checkpoint: DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01 — compare opaque TOC values

- Time / timezone: 2026-10-21 09:33 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01` / artifact inspection / ARTIFACT_VERIFIED.
- Prerequisite evidence / input hashes: E75 confirms no exact duplicate paths within a manifest;
  E77 reports 40,216 exact-path overlap pair occurrences across the seven manifests. Claim-time
  basenames, SHA256 values and byte sizes are in private
  `local/validation/data02-overlap-toc-digest/claim-inputs.json`.
- Owned / changed files: public `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/ASSETS.md`,
  generated `docs/FOLDERS.md`, this checkpoint; private validation files under
  `local/validation/data02-overlap-toc-digest/`.
- Hypothesis and expected result: exact-path overlaps can be joined across the seven manifest
  tables and each joined row pair's 16-byte TOC field can be compared as an opaque value. Expected
  total is 40,216 pair occurrences per E77; this task does not infer what those field values mean.
- Exact command, CWD: `/Volumes/Extra/SoC Mac Native`; ran `python3 -I
  local/validation/data02-overlap-toc-digest/analyze.py` twice and compared outputs with `cmp -s`,
  exactly as recorded in `local/validation/data02-overlap-toc-digest/command.txt`.
- Exit / observed result: both runs exited 0 and `cmp -s` exited 0. The report has 7 manifests,
  21 unordered archive pairs, 170,451 total manifest rows, 40,216 exact-path pair occurrences,
  4,479 equal TOC values and 35,737 different values. All input identities matched claim-time
  hashes before and after both executions.
- Log, capture and evidence paths/IDs: private `claim-inputs.json`, `command.txt`, `validation.log`,
  `report.json`, `run1.json`, `run2.json`, and stderr files in the same directory; public evidence
  E123. Analyzer SHA256 `c1bba631e392df87186fe4b29cabc12df5b335f30bf3412c1a71959c92f9ac64`;
  report SHA256 `02bd5d3f372c96027253c706348b2ab699ec43b0cddc4f98b5b900df0cfb3d8e`.
- Observation versus inference / coverage limitations: only exact path keys and TOC `md5` field
  values were joined; row path values were not emitted, and no archive payload bytes were opened.
  Equal TOC values are metadata equality only. The algorithm, input bytes, integrity role, underlying
  member byte equality, mount order, overlay policy and game use remain UNKNOWN.
- Cumulative failed tries and distinct hypotheses: 0 failed hypotheses. Two successful deterministic
  executions; no failed analysis attempt.
- Remaining risks / blockers: parent DATA-02 remains ACTIVE. The counts do not resolve member
  schemas, `pakN` encoding or selected-scene asset resolution.
- Next exact action: inspect the remaining open DATA-02 work packets for a bounded follow-up, then
  add its task row and claim checkpoint before any work on that task.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, ASSETS
  and generated FOLDERS updated; INSTALL_LOG unchanged because no install/download/build occurred.
  `python3 scripts/project_index.py` exit 0; `trail build --check` exit 0 (wiring graph synced;
  meaning layer remains unbuilt, 0% complete).
