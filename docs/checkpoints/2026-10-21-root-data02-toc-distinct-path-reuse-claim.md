# Claim: DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01

- Time / timezone: 2026-10-21 09:40 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01`, ACTIVE, 0 failed tries.
- Parent / prerequisites: DATA-02; E75's seven-manifest path audit; E77's exact-path overlap map;
  E123's TOC comparison on exact-path matches; E122's single-manifest uniqueness audit.
- Owned files: private analyzer, bound-input record, commands, logs and aggregate report under
  `local/validation/data02-toc-value-distinct-path-reuse/` belong to `launcher_review`. Root owns
  HANDOFF, TASKS, ASSETS, EVIDENCE and public checkpoints. No reference clone or original game
  package will be modified or staged.
- Question / acceptance: across the seven existing hash-bound member manifests, count opaque 16-byte
  TOC values associated with more than one distinct exact member path. Separately aggregate repeated
  `(TOC value, exact path)` associations across archives, so same-path cross-archive matches are not
  conflated with distinct-path reuse. Emit aggregate counts only, with no path, TOC value or
  per-value/per-path row. Verify all seven input identities before and after; account for all
  170,451 manifest rows; run twice and require byte-identical aggregate reports.
- Data boundary: read only the seven manifest JSONL files and the fields needed for the aggregate;
  do not open archives or member payloads. Equality is an opaque TOC-field observation and cannot
  establish content identity, digest semantics, integrity, overlay precedence or runtime use.
- Exact exploration command and CWD: from `/Volumes/Extra/SoC Mac Native`,
  `trail grep "parse_manifest_bytes" --in scripts/`. It located the existing manifest parser at
  `scripts/audit_archive_overlap_map.py:L54-L93` and its caller; no parser code was changed. E75/E77
  identify the seven manifests and 170,451-row total.
- Claim-time checkout: `main` at `fb5bd8c`, matching `origin/main`. Existing unrelated native host
  protocol work remains unstaged.
- Exit / result: claim only. No analyzer, validation output, or task-specific test has been created
  or run yet; no archive or member payload was opened.
- Observation versus inference: the existing audits establish seven metadata manifests and
  exact-path overlap counts. The proposed aggregate will report only how opaque field values group
  by exact path; it will not infer the meaning of those values.
- Remaining risks: the aggregate must distinguish a value recurring at one path in several archives
  from a value reused on multiple distinct paths, while suppressing identifiers and preserving
  hash-bound input identities.
- Next exact action: `launcher_review` records all seven input identities, implements the private
  aggregate, executes it twice, then reports paths, hashes, commands and aggregate results to root
  for an independent replay.
- HANDOFF / TASKS / EVIDENCE / ASSETS / INSTALL_LOG / indexes: HANDOFF and TASKS updated to claim;
  EVIDENCE and ASSETS unchanged until the result is independently verified; INSTALL_LOG unchanged.
  `python3 scripts/project_index.py` exited 0 and indexed 14,200 folders. `trail build --check`
  exited 0; the wiring graph is synchronized and the meaning layer remains unbuilt (0% complete).
