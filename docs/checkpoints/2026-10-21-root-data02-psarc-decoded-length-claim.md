# Claim: DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01

- Time / timezone: 2026-10-21 09:35 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01`, ACTIVE, 0 failed tries.
- Parent / prerequisites: DATA-02 and `DATA-02-PSARC-PARTIAL-READ-01` (existing prefix/clamp
  fixture). Graft identifies `Psarc.read` at `scripts/psarc.py:L244-L275`; its existing length
  guard is at L272. The synthetic builder and current read-limit tests are in
  `scripts/test_psarc.py:L20-L50` and `L154-L169`.
- Owned files: `scripts/test_psarc.py`; root integrator owns HANDOFF, TASKS, EVIDENCE and checkpoint
  documentation.
- Question / acceptance: add one synthetic compressed member whose TOC-declared length makes the
  final block's decoded byte count differ from the expected block byte count. Assert the exact
  `PsarcError` message from the existing guard. Run `python3 -I scripts/test_psarc.py` and require
  the full focused PSARC suite to pass. Do not change parser behavior or read a real archive.
- Observation versus inference: the current reader checks `len(chunk) != full` and raises a block
  length error. A fixture can pin this branch. This does not add an archive-format rule beyond the
  existing implementation's bound.
- Exact exploration commands, CWD `/Volumes/Extra/SoC Mac Native`:
  `trail ask "How does Psarc.read implement its optional limit, and what does the manifest/reader contract require for negative limits?" --source --in scripts/`,
  `trail grep "decoded to" --in scripts/`,
  `trail skeleton scripts/test_psarc.py`, and `sed -n '20,52p;139,169p;194,210p' scripts/test_psarc.py`.
  No test or implementation command has run.
- Exit / result: Graft located the existing block-length guard; the test builder can construct
  synthetic PSARC data; source excerpt confirms stored/compressed read-limit coverage and tests for
  truncated and corrupt compressed blocks. No files were edited before this claim.
- Log / evidence paths: interactive Graft output in this session; no private game input or log file.
- Remaining risks: choose compressed bytes whose zlib output is 16 bytes while the final expected
  block length is 15, so the intended guard is reached instead of the corrupt-zlib branch. Keep the
  assertion tied to an actual synthetic fixture and exact message.
- Next exact action: add the test by tampering only the synthetic member's declared size, then run
  `python3 -I scripts/test_psarc.py`.
- HANDOFF / TASKS / EVIDENCE / INSTALL_LOG / indexes: HANDOFF and TASKS claimed; EVIDENCE unchanged
  until verification; INSTALL_LOG unchanged; regenerate FOLDERS and run `trail build --check` before
  committing this claim.
