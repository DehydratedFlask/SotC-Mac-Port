# Claim: RE-02-DYNLINK-IMAGE-READ-FILE-OFFSET-BOUNDS-FIXTURE-01

- Time / timezone: 2026-10-22 01:08 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `RE-02-DYNLINK-IMAGE-READ-FILE-OFFSET-BOUNDS-FIXTURE-01`, child of
  `RE-02-DYNLINK-SEGMENT-SPAN-01`; `ACTIVE`, 0 failed implementation tries.
- Dependencies: `RE-02-DYNLINK-01` and `RE-02-DYNLINK-SEGMENT-SPAN-01` are `FIXTURE_VERIFIED`.
  A live duplicate check found no task ID match in `docs/TASKS.md`, `HANDOFF.md`, or
  `docs/EVIDENCE.md` before claim.
- Graft source context: the earlier scoped `trail ask` located `Image.read()` and `span()` in
  `scripts/ps4_dynlink.py`; a direct `trail grep 'Image.read' --in scripts/` had no hit, and the
  instructed short-term retry `trail grep 'read' --in scripts/` searched all 16 indexed scripts.
  Live source behavior: a segment whose virtual range is contained within `filesz` computes the
  backing file offset as `p['offset'] + vaddr - p['vaddr']`; `span()` checks that offset against
  the ELF byte buffer and raises `ValueError` on overflow.
- Baseline hashes: `scripts/test_ps4_dynlink.py` SHA-256
  `6681021f7cb4d66d2b23411d43c84e884309dca4898b3ccc7459ecb3bda97ece`; unchanged
  `scripts/ps4_dynlink.py` SHA-256
  `c0fcf34b5b9a4ad626a5b88cd2a100be37312e8038c26fb230337bc98bf5ba95`.
- Scope: add one test only in `scripts/test_ps4_dynlink.py`. Construct an `Image` over `b'abc'`
  with one `PT_LOAD` segment at virtual address `0x1000`, `filesz=memsz=1`, and file offset
  `0x10`; call `read(0x1000, 1)` and assert the exact `ValueError` text
  `out of bounds: 0x10+0x1 / 0x3`. The virtual-range predicate should pass, so this covers
  `Image.read()`'s integration with `span()` bounds. Do not modify parser code.
- Exact baseline command / CWD: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_ps4_dynlink.py`, CWD `/Volumes/Extra/SoC Mac Native`.
- Baseline result: exit 0; 29 tests passed. Log
  `local/validation/20261022-root-fixture-baselines/re02-dynlink-image-read-file-offset-baseline.log`,
  SHA-256 `7f6d674dc38c83e5df2985921ab33658727cdf69955265a530601defdf4dcbeb`.
- Execution note: an initial shell wrapper used zsh's reserved variable name `status` and failed
  while reporting after invoking the baseline test. The test was rerun with task-specific variable
  `fixture_rc`; the captured rerun exited 0. This was a command-wrapper issue, not an implementation
  attempt.
- Observation versus inference: this pins only synthetic helper behavior. It does not establish
  ELF parser policy, real ELF validity, actual loader behavior, relocation behavior, or game
  execution. The unchanged parser remains responsible for rejecting the file-buffer overflow.
- Files changed by claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint, and
  generated folder indexes. Test implementation remains pending.
- Remaining risks: no live ELF or loader behavior is covered; the existing check rejects bounds only
  after the virtual range is deemed file-backed.
- Next exact action: add the single synthetic `Image.read()` file-offset integration test, run
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (expected 30 tests), record
  result/hash, refresh indexes, and publish a completion checkpoint.
