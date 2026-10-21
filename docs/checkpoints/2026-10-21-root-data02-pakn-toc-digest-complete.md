# Checkpoint: DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01 — manifest values unique

- Time / timezone: 2026-10-21 09:29 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01` / artifact inspection / ARTIFACT_VERIFIED.
- Prerequisite evidence / input and source hashes: parent `DATA-02`; manifest
  `local/manifests/assets/CUSA08809-pakN.members.jsonl`, SHA256
  `ee7c927e8ae3b28b20d61bb460c0c3c6c783cf0a7242df366f968949d5d21075`, 12,644,864 bytes.
- Owned / changed files: public `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  `docs/ASSETS.md`, generated `docs/FOLDERS.md`, this checkpoint; private validation files under
  `local/validation/data02-pakn-toc-digest-uniqueness/`.
- Hypothesis and expected result: the `md5` field contains one 16-byte TOC value per manifest row;
  the bounded aggregate counts rows, unique values, repeated groups, size-spanning repeated groups,
  and the largest repeated group without reporting values or member paths.
- Exact command, CWD: from `/Volumes/Extra/SoC Mac Native`, ran the two
  `python3 -I local/validation/data02-pakn-toc-digest-uniqueness/verify_manifest.py
  local/manifests/assets/CUSA08809-pakN.members.jsonl` invocations and `cmp -s` recorded in
  `local/validation/data02-pakn-toc-digest-uniqueness/command.txt`.
- Exit / observed result: both runs exited 0 and `cmp -s` exited 0. Aggregate: 64,108 rows,
  64,108 unique values, 0 repeated-value groups, 0 repeated groups spanning sizes, largest group
  1. Script verifies the manifest SHA256 and size before and after each run.
- Log, capture and evidence paths/IDs: private identity replay at
  `local/validation/data02-pakn-toc-digest-uniqueness/claim-input-identity.log`; exact commands,
  stderr captures, outputs and replay record in the same private directory; public evidence E122.
  Verifier SHA256 `6136fe9937a737b279bf52cd43890d258cfc9f4c2b7d55e9298165e0fdce0f0d`;
  aggregate SHA256 `12326b767e686e75fa146689da24dbaab47aa997dac93a571ead68984f24f800`.
- Observation versus inference / coverage limitations: every metadata row had a valid 32-hex-digit
  `md5` field and nonnegative integer `size`; values were unique. This does not establish the digest
  algorithm, input bytes, integrity role, runtime use, or meaning. The verifier accesses only the
  selected `md5` and `size` fields; paths are not used or emitted, and no member payload was opened.
- Cumulative failed tries and distinct hypotheses: 0 failed hypotheses. Two successful deterministic
  executions; no failed aggregate attempt.
- Remaining risks / blockers: parent DATA-02 remains ACTIVE; manifest uniqueness does not establish
  member identity or archive integrity, encoding, mount precedence, or selected-scene resolution.
- Next exact action: claim a separate bounded metadata task before comparing TOC values across exact
  path overlaps; keep digest semantics UNKNOWN unless a validated algorithm/input is established.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, ASSETS
  and generated FOLDERS updated; INSTALL_LOG unchanged because no install/download/build occurred.
  Source graph check: `trail build --check` exit 0; wiring graph synced, meaning layer remains
  unbuilt (0% complete).
