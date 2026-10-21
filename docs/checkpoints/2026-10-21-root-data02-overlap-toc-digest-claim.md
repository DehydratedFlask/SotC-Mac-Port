# Claim: DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01

- Time / timezone: 2026-10-21 09:31 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01`, ACTIVE, 0 failed hypotheses.
- Parent / prerequisites: DATA-02; `DATA-02-ARCHIVE-OVERLAP-MAP-01` / E77 confirms 40,216 exact-path
  overlap pair occurrences across the seven manifests. E75 found no exact duplicate path groups
  within any individual manifest.
- Inputs: seven private `*.members.jsonl` files under `local/manifests/assets/`. Their basenames,
  SHA256 values and byte sizes are recorded in `local/validation/data02-overlap-toc-digest/claim-inputs.json`.
- Owned files: this checkpoint, `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and
  private validation artifacts under `local/validation/data02-overlap-toc-digest/`.
- Question / acceptance: for each of the 21 unordered archive pairs, join rows by exact manifest
  path and count equal vs. unequal values in the opaque 16-byte TOC field exported as `md5`. Root
  must independently reproduce the aggregate twice, verify the seven input hashes before and after,
  and match the E77 total of 40,216 shared-path pair occurrences. Emit aggregate counts only; do
  not emit member paths or digest values and do not read archive payloads.
- Observation vs. inference: a read-only scout proposed this bounded task and reported preliminary
  counts of 4,479 equal and 35,737 different TOC fields. These are unverified worker observations,
  not evidence or the expected outcome. This task will establish only root-replayed metadata equality
  counts; it will not identify a digest algorithm/input or infer content equality, integrity, overlay
  precedence, mount order, or runtime use.
- Exact claim-time command, CWD: `/Volumes/Extra/SoC Mac Native`; see
  `local/validation/data02-overlap-toc-digest/claim-command.txt`. It hashes and sizes the seven
  manifest files without parsing record fields.
- Exit / result: identity command exited 0; found exactly seven manifests. Full identities are in
  private `claim-inputs.json`. No member paths, archive payloads or original strings were emitted.
- Log / evidence paths: private `claim-command.txt` and `claim-inputs.json`; prerequisite public E75
  and E77 in `docs/EVIDENCE.md`.
- Remaining risks: the `md5` label is only a field name; its algorithm and input are UNKNOWN. Exact
  path overlap is metadata and does not establish what the runtime mounts or reads.
- Next exact action: write `local/validation/data02-overlap-toc-digest/analyze.py` to compare only
  exact-path joins and aggregate counts, run it twice with input-hash checks, then checkpoint the
  root result before any parent-task claim.
- HANDOFF / TASKS / EVIDENCE / INSTALL_LOG / indexes: HANDOFF and TASKS claimed; EVIDENCE unchanged
  until root verification; INSTALL_LOG unchanged (no install/download); regenerate FOLDERS and run
  `trail build --check` before committing this claim.
