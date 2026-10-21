# Claim: DATA-02-MANIFEST-JSON-SYNTAX-01

- Time / timezone: 2026-10-21 09:43 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `DATA-02-MANIFEST-JSON-SYNTAX-01`, ACTIVE, 0 failed implementation tries.
- Parent / prerequisites: DATA-02 and `DATA-02-MANIFEST-RECORD-SHAPE-01`. That task added file-and-line
  errors for record shape but explicitly left `json.JSONDecodeError` on malformed JSON lines unchanged.
- Owned files: `slot23_raw_xrefs` owns only `scripts/psarc.py` and `scripts/test_psarc.py`; root owns
  HANDOFF, TASKS, EVIDENCE and checkpoint documentation. No archive or reference clone is to be
  modified or read for this synthetic parser task.
- Question / acceptance: for malformed nonblank JSONL records, raise `PsarcError` naming the manifest
  path and 1-based line. Add synthetic cases for malformed JSON on line 1 and after a valid row;
  preserve the existing blank-line behavior. Prove the new assertions fail on the pre-change code,
  then run the focused PSARC suite successfully.
- Source exploration: the read-only scout used Graft and located `json.loads(line)` in
  `scripts/psarc.py:L99-L121`; the separate overlap-manifest parser already wraps malformed JSON
  with line context. The existing record-shape task and checkpoint document the deliberately
  unchanged JSON syntax behavior.
- Claim-time checkout: `main` at `35ac28f`, matching `origin/main`. The independent
  `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01` is active in its private validation directory and does
  not own either implementation file.
- Exact test / CWD: implementer will use `python3 -I scripts/test_psarc.py` from
  `/Volumes/Extra/SoC Mac Native`; exact commands and old/new captures will be recorded in the task
  completion checkpoint.
- Exit / result: claim only; no code or fixture files changed and no tests run yet.
- Observation versus inference / limits: malformed JSON currently escapes as a JSON decoder error;
  wrapping it as a module-level `PsarcError` is a parser diagnostic change only. It does not establish
  archive validity or runtime loading behavior.
- Remaining risks: preserve blank-line handling and exact 1-based line attribution while keeping the
  change limited to `load_members_jsonl`; check all existing consumers continue to receive
  `PsarcError` for malformed records.
- Next exact action: `slot23_raw_xrefs` records a failing-before test run, adds the two malformed-line
  fixtures and narrow error wrapping, then reports source hashes, logs and command results for root
  replay.
- HANDOFF / TASKS / EVIDENCE / INSTALL_LOG / indexes: HANDOFF and TASKS updated to claim; EVIDENCE
  unchanged until root verifies the result; INSTALL_LOG unchanged. `python3 scripts/project_index.py`
  exited 0 and indexed 14,201 folders. `trail build --check` exited 0; wiring is synchronized and
  the meaning layer remains unbuilt (0% complete).
