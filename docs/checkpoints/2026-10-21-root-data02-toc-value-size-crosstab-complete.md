# DATA-02 TOC-value / declared-size crosstab — artifact verified

- Time / timezone: worker 2026-10-21 22:50 CDT; root replay/review 2026-10-21 22:53 CDT
  (America/Chicago). Worker `/root/round78_next_task_scout`; shared-document integrator `/root`.
- Task / status: `DATA-02-TOC-VALUE-SIZE-CROSSTAB-01`, `ARTIFACT_VERIFIED`; parent DATA-02 remains
  ACTIVE / partial. No acceptance gate advanced.
- Inputs: exactly the seven private `local/manifests/assets/*.members.jsonl` metadata manifests;
  170,451 rows and 21 unordered archive pairs. Current byte sizes and SHA-256 identities match all
  seven entries in E123's `local/validation/data02-overlap-toc-digest/claim-inputs.json`. No PSARC
  archive or member payload was opened.
- Worker files: only `local/validation/data02-toc-value-size-crosstab-01/`. Analyzer
  `analyze.py` SHA-256 `0ecf9bad49f2c42c6aff3f9fd10c5504644e7590114b4a73da4d42de96af9cec`, runner
  `capture_runs.py` SHA-256 `46be1c680382e4f8963168873b9af59d66cc15ae6e0dcde80a333e898c19b628`, privacy/replay
  verifier SHA-256 `6f22e77e06b01ed6e919b4d9d6f3f8e110a3ef7bbdf1f7d89df51aa40af00763`, and
  `result-summary.md` SHA-256 `0bdc2c8b8c9448e6ba196ace60939f9a603908046834df4706580e5be58fe1bd`.
- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  local/validation/data02-toc-value-size-crosstab-01/capture_runs.py` (stdout/stderr redirected to
  `local/validation/data02-toc-value-size-crosstab-01/capture.stdout` /
  `local/validation/data02-toc-value-size-crosstab-01/capture.stderr`); exit 0. Wrapper launched
  `python3 -I .../analyze.py` twice
  with distinct aggregate and identity output files. Both runs exit 0, stderr empty, and produce
  byte-identical report SHA-256 `1b9935fbd06053506f82f8d5d8c860d4417fe54ad89e0cd461da92ea65295183`
  and identity sidecar SHA-256 `a94b6cf1a0cfbbd27159a9bc3c56cf1252ece6f5fc6657a3c211a49acd0d9776`.
- Worker consistency/privacy command: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  local/validation/data02-toc-value-size-crosstab-01/verify_replay.py`; exit 0, stderr empty at
  `local/validation/data02-toc-value-size-crosstab-01/verify.stderr`, stdout at
  `local/validation/data02-toc-value-size-crosstab-01/verify.stdout` SHA-256
  `10a65ec8d141807e5c2aa54885e3ba4986b930f04368b746bb5711fd1e63964c`.
- Root analyzer replay: `python3 -I
  local/validation/data02-toc-value-size-crosstab-01/analyze.py --output
  local/validation/data02-toc-value-size-crosstab-01/root-replay-report.json --identities-output
  local/validation/data02-toc-value-size-crosstab-01/root-replay-input-identities.json`; exit 0.
  Root report `root-replay-report.json` SHA-256
  `1b9935fbd06053506f82f8d5d8c860d4417fe54ad89e0cd461da92ea65295183`; identity sidecar
  `root-replay-input-identities.json` SHA-256
  `a94b6cf1a0cfbbd27159a9bc3c56cf1252ece6f5fc6657a3c211a49acd0d9776`; stdout
  `root-replay.stdout` SHA-256 `ae75f4df85f7fbab9f0eaf4c58a036af6d7aae1144cdee18c7b01daf06763a92`,
  stderr `root-replay.stderr` empty (SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`), exit file
  `root-replay.exit` contains `0`. Root verifier command `python3 -I
  local/validation/data02-toc-value-size-crosstab-01/verify_replay.py` exits 0 and checks both
  worker runs, seven current input identities against E123, the four cells/margins, and absence of
  every manifest member path and raw `md5` value from the aggregate report. Root verification stdout
  `root-verify.stdout` SHA-256 is `10a65ec8d141807e5c2aa54885e3ba4986b930f04368b746bb5711fd1e63964c`;
  stderr `root-verify.stderr` is empty and exit file `root-verify.exit` contains `0`.
- Observed cross-tab: value equal / size equal = **4,479**; value equal / size different = **0**;
  value different / size equal = **225**; value different / size different = **35,512**. The four
  cells sum to 40,216. TOC-value margins are 4,479 equal / 35,737 different (E123); declared-size
  margins are 4,704 equal / 35,512 different (E77).
- Observation versus inference: these are equality relations between manifest metadata fields.
  Equal values and sizes do not establish payload identity, digest semantics, integrity role,
  overlay priority, mount order, loader behavior, or runtime use. No paths or raw field values were
  emitted in the aggregate report.
- Attempts: 0 failed tries. Root reviewed the exact implementation and independently reproduced
  the byte-identical aggregate; no source code or input data changed.
- Index/graph: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`
  both exit 0 at 19,275 folders. Captured logs `/tmp/round78-crosstab-dynlink-final-project-index.log`
  SHA-256 `be383a9ca396b168dc2b07fd43c859eebbc366320ef05de31997c0775952b2a2` and
  `/tmp/round78-crosstab-dynlink-final-project-index-check.log` SHA-256
  `aad69692b772bf23bd5a7ec150587b537c633cb4c270c3b34101e9cbde87a82d`. `trail build --check`
  exits 0 with 3,597 nodes; deep semantic layer is unbuilt. Log
  `/tmp/round78-crosstab-dynlink-final-trail-check.log` SHA-256
  `a658c88a8f13351e499914f814602c21b11d347ef1c2cc5bab21fdea9100f6cb`.
- Remaining risk: field semantics and all member contents remain unknown; this is not evidence of
  archive decoding or game behavior.
- Exact next action: after publishing this result, proceed with the separately claimed
  `RE-02-DYNLINK-REQUIRED-PHDR-01` synthetic parser task.
- Task status: `ARTIFACT_VERIFIED`; G0 remains PARTIAL and G1–G6 remain NOT RUN.
