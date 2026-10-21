# Claim: DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01

- Time / timezone: 2026-10-21 09:26 CDT (America/Chicago), claim repair and independent identity replay.
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01`, ACTIVE, attempt 0.
- Parent: `DATA-02`. This is a metadata-only inventory, not a schema or loader-policy claim.
- Input: `local/manifests/assets/CUSA08809-pakN.members.jsonl`, SHA256
  `ee7c927e8ae3b28b20d61bb460c0c3c6c783cf0a7242df366f968949d5d21075`, 12,644,864 bytes.
  The seven-archive manifest set and pakN identity are described by the DATA-02 evidence and
  private manifests; this task reads only the pakN manifest.
- Field provenance: `scripts/psarc.py` parses the TOC's 16 raw bytes into the manifest field named
  `md5`. `docs/ASSETS.md` already records that digest semantics are UNKNOWN. This task counts
  repeated values only and will not infer an algorithm or meaning from uniqueness.
- Question / acceptance: count JSONL rows, unique `md5` field values, repeated-value groups,
  repeated groups spanning sizes, and largest group. Preserve only the aggregate result and input
  digest under `local/validation/data02-pakn-toc-digest-uniqueness/`; do not emit member paths,
  archive payload bytes, or original game strings.
- Exact next action: root independently parse and aggregate the pinned manifest, verify its SHA256
  before and after, save the counts and exact command to the private validation directory, then
  update the task/evidence checkpoint. No game payload is read; no code or test changes are planned.
- Files changed when the claim was first written: `HANDOFF.md` and this claim record. The task row
  and generated folder index were not yet updated; this resume adds the task row and regenerates
  `docs/FOLDERS.md`.
- Exact identity commands on resume:
  `shasum -a 256 local/manifests/assets/CUSA08809-pakN.members.jsonl` and
  `stat -f '%z' local/manifests/assets/CUSA08809-pakN.members.jsonl`.
  Both exited 0 at 2026-10-21 09:26 CDT; SHA256 and byte count matched the claim. Output:
  `local/validation/data02-pakn-toc-digest-uniqueness/claim-input-identity.log`.
- Observation: manifest identity is unchanged at the claim's digest and size. This does not verify
  row counts or identify what the TOC field hashes. Inference: none about digest semantics.
- Remaining risk: uniqueness can suggest the field is entry-specific but cannot identify its
  algorithm, input bytes, integrity role, or runtime meaning. No DATA-02 parent question is
  resolved by this task.
- Next exact action: run `python3 scripts/project_index.py`, then `trail build --check`; audit only
  the allowlisted claim docs and push the claim checkpoint before the metadata aggregation.
- Status / attempt count: ACTIVE / 0 hypothesis failures; identity replay only, no aggregate try.
- Remaining risk: uniqueness can suggest the field is entry-specific but cannot identify its
  digest algorithm, input bytes, integrity role, or runtime meaning. No DATA-02 parent question is
  resolved by this task.
