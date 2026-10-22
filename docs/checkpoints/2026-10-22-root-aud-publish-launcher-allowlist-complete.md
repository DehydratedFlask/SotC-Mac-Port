# Checkpoint: AUD-PUBLISH-LAUNCHER-ALLOWLIST-FIXTURE-01 — verified

- Time / timezone: 2026-10-22 00:52 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / parent / status: `AUD-PUBLISH-LAUNCHER-ALLOWLIST-FIXTURE-01`, child of
  `AUD-PUBLISH-EDGE-01`; `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed tries.
- Claim record: [claim checkpoint](2026-10-22-root-aud-publish-launcher-allowlist-claim.md).
- Baseline and final source hashes: baseline `scripts/test_audit_publish_paths.py` SHA-256
  `2759f5b9f554388ca7fd95ab2ee6457d16af995d3f0e86bbe85458422e65d2ac`; final test SHA-256
  `ff133364bb2b577249c8714e6836c6d6968bc3a32cbbc31b3a12b53dc852312c`. Production
  `scripts/audit_publish_paths.py` remained unchanged at SHA-256
  `2ea984cfb38329f4a9ec0b46300b0513998a1a642d224572906cee531653bba3`.
- Files changed for this task: one test method in `scripts/test_audit_publish_paths.py`.
  Integrated task records are `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated
  `docs/FOLDERS.md`, and this checkpoint. Existing user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` were not part of the task and remain unstaged.
- Hypothesis / expected result: `ALLOWED_TOP` and the `launcher/` path rule should accept a
  representative `launcher/Package.swift` path together with each listed top-level metadata path;
  `audit.violations(paths)` should return an empty list.
- Exact command, CWD and environment: from `/Volumes/Extra/SoC Mac Native`,
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.
- Exit / observed result: baseline passed 8/8; final root replay passed 9/9. Baseline log
  `local/validation/20261022-root-fixture-baselines/aud-publish-launcher-allowlist-baseline.log`
  SHA-256 `698d16d7b93c4bd9707d3cfacda6fb1e0fea99ed0cda6e69bedc42772940540a`; root log
  `local/validation/20261022-root-fixture-baselines/aud-publish-launcher-allowlist-root-final.log`
  SHA-256 `42f237d7abc9eb371abada54db94861f647b2fd2e4008b692ec18dfdcc2e00d5`.
- Trail evidence: `trail build --check` passed at 3,612 nodes / 1,240 files; deep meaning layer is
  not built. `trail grep "test_launcher_and_top_level_metadata_allowed"` returned the method in
  `scripts/test_audit_publish_paths.py:L42-L55`. Check and query logs are
  `local/validation/20261022-root-fixture-baselines/aud-fixtures-trail-check.log` SHA-256
  `53451a65e275859a881899073bf9409ef57118bd99c37fa7eaf91d7246b7d19b` and
  `.../aud-publish-launcher-allowlist-trail-query.log` SHA-256
  `67cb813455879727d21933fddc4564716ec199e02b9a7adb20ddc05ff60dd9d5`.
- Observation / inference and limits: the synthetic test observes zero audit violations for the
  listed input strings. It does not exercise a live staged tree, symlink targets, publish tooling,
  or any game runtime. Production audit logic is unchanged.
- Failed tries / risks: 0 failed tries. Remaining risk is limited to untested behavior outside
  this synthetic positive classification case.
- Project index: `python3 scripts/project_index.py` wrote 24,078 folders and
  `python3 scripts/project_index.py --check` passed. Logs are
  `local/validation/20261022-root-fixture-baselines/aud-fixtures-project-index-generation.log`
  SHA-256 `a79b0d25fced0d9c18dda3386413644761318dd915da578a4bbe36f239469b0c` and
  `.../aud-fixtures-project-index-check.log` SHA-256
  `2e24f86275b84b7efc28ec198e984a6698bc3584a72c960f1b280bdaa45c1b67`. An initial check failed
  because its newly created log changed the indexed `local/validation` file count after generation;
  the retained first-check log is SHA-256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`. After precreating all
  expected audit logs and regenerating, the check passed.
- Next exact action: stage only allowlisted task files and docs, run both publish-path audit
  commands required by `AGENTS.md`, and push `origin main`.
- `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md` have the completion status recorded;
  `INSTALL_LOG.md` is unchanged. Publication details are to be recorded in the following docs-sync
  checkpoint after the completion commit is pushed.
