# Checkpoint: DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01 — decoded block length guard

- Time / timezone: 2026-10-21 09:37 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status / attempt count: `DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01` / synthetic
  parser fixture / FIXTURE_VERIFIED / 0 failed tries.
- Prerequisites: DATA-02 and `DATA-02-PSARC-PARTIAL-READ-01`; existing `Psarc.read` decoded-length
  guard in `scripts/psarc.py`.
- Files changed: `scripts/test_psarc.py`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  generated `docs/FOLDERS.md`, and this checkpoint. Private command and log are under
  `local/validation/data02-psarc-decoded-length/`. `INSTALL_LOG.md` unchanged.
- Hypothesis / expected result: a synthetic compressed member with a declared uncompressed length
  of 63 bytes yields a final decoded block of 16 bytes when the reader expects 15; `Psarc.read`
  must raise its existing exact decoded-length error.
- Exact command and CWD: from `/Volumes/Extra/SoC Mac Native`, ran
  `python3 -I scripts/test_psarc.py`; command is recorded in
  `local/validation/data02-psarc-decoded-length/command.txt`.
- Exit / observed result: exit 0; 27 tests passed. The new fixture asserts
  `block 4 decoded to 16 bytes, expected 15`. Parser source and runtime behavior were not changed.
- Log / evidence: `local/validation/data02-psarc-decoded-length/test.log`, SHA256
  `1606c4b9a57565f041ec5e6779902a90851fc0d993474e6cf0016bbc827ff1cd`; modified test source
  SHA256 `81eeb4c60180802deeb4b167d9d065f694551829201c9971dcbd96c675503d5a`; public evidence E124.
- Observation versus inference / limits: the fixture reaches and pins the existing guard using
  generated PSARC bytes. It does not establish an additional archive-format rule, read a real
  archive, or demonstrate runtime archive loading.
- Remaining risks / blockers: parent DATA-02 schema, selected-asset resolution, and runtime use
  remain open. No conclusion is made about negative read limits; that behavior remains unspecified.
- Next exact action: claim `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01` before creating its private
  metadata-only analyzer; report aggregate counts only and leave digest meaning UNKNOWN.
- Index / graph: `python3 scripts/project_index.py` exited 0 and indexed 14,200 folders. The first
  `trail build --check` exited 1 because the source graph correctly reported the new test as stale;
  `trail build` exited 0 and `trail build --check` then exited 0. Wiring is synchronized; the
  meaning layer remains unbuilt (0% complete). `graft/` is a local ignored cache.
- Publication: only `scripts/test_psarc.py`, HANDOFF, task/evidence/folder indexes, and this
  checkpoint are allowlisted for the task. Unrelated native host work remains unstaged.
