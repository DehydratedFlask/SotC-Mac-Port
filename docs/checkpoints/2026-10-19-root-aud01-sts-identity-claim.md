# AUD-01-STS-IDENTITY-01 — paired cachedFiles test_adpcm identity survey claim

- Time / timezone: 2026-10-19 23:50 CDT (America/Chicago).
- Agent and integrator: assigned `/root/aud01_sts_cp3_identity`; integrator `/root`.
- Task / gate / status: AUD-01-STS-IDENTITY-01 under AUD-01; CLAIMED, fixture/inventory only.
- Prerequisite evidence / input and source hashes: private member manifests report 290 exact paths
  in each cachedFiles layer. Inputs are `local/extracted/CUSA08809/cachedFiles.psarc` and
  `local/extracted/CUSA08809-patch/cachedfiles.psarc`; manifests are
  `local/manifests/assets/CUSA08809-cachedFiles.members.jsonl` and
  `local/manifests/assets/CUSA08809-patch-cachedfiles.members.jsonl`. `Psarc.read()` and member
  enumeration were located with Graft in `scripts/psarc.py`; live inputs must be revalidated.
- Owned / changed files: worker owns only new private output under `local/validation/`; root owns
  `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No archive, extracted game asset, or source
  file may be modified.
- Hypothesis and expected result: exact-path membership is paired across both manifests. The
  worker will independently read each of the 580 small members, verify entry ID/offset/size against
  its layer manifest, compute decoded-member SHA-256, and report exact-path size/hash equality and
  size distributions. Expected output is a reproducible inventory; extension and directory names
  do not establish codec, schema, gameplay use, or playback.
- Exact command, CWD and nonsecret environment: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sts-identity-01.py`
  from the repository root, with stdout captured to private run logs. No installs or downloads.
- Exit / observed result: pending.
- Log, capture and REA evidence paths/IDs: pending; all artifacts remain under ignored
  `local/validation/`.
- Observation versus inference / coverage limitations: this is a complete identity/byte-comparison
  slice for the named 290-path group per layer, not a decoder, format parser, NGS2 analysis, or
  evidence of runtime selection.
- Cumulative failed tries and distinct hypotheses: 0; no implementation hypothesis has been
  tested yet.
- Remaining risks / blockers: archive identity may drift from the already indexed manifest; if any
  live locator or digest disagrees, stop and report the mismatch without updating the baseline.
- Next exact action: run the bounded private script twice, compare outputs, and send exact hashes,
  commands, logs, results, and limitations to root for independent rerun.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS claim recorded;
  EVIDENCE and INSTALL_LOG unchanged at claim; project index to refresh after this checkpoint.
