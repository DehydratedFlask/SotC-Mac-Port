# RE-02 dynamic-link image file-offset bounds fixture — complete

- Time / timezone: 2026-10-22 01:10 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `RE-02-DYNLINK-IMAGE-READ-FILE-OFFSET-BOUNDS-FIXTURE-01`, child of
  `RE-02-DYNLINK-SEGMENT-SPAN-01`; `FIXTURE_VERIFIED`.
- Claim checkpoint: [claim record](2026-10-22-root-re02-dynlink-image-read-file-offset-claim.md),
  commit `17ff82e5b58eeb7f24aaf3597369e549e894c5cc`, pushed to `origin/main`.
- Owned implementation file: `scripts/test_ps4_dynlink.py` only. Added
  `test_image_read_rejects_file_offset_past_elf_buffer`; no production parser code changed.
- Baseline source hashes: test file SHA-256
  `6681021f7cb4d66d2b23411d43c84e884309dca4898b3ccc7459ecb3bda97ece`; unchanged
  `scripts/ps4_dynlink.py` SHA-256
  `c0fcf34b5b9a4ad626a5b88cd2a100be37312e8038c26fb230337bc98bf5ba95`.
- Hypothesis and assertion: with ELF bytes `b'abc'` and a `PT_LOAD` segment whose virtual
  address is `0x1000`, `filesz=memsz=1`, and file offset is `0x10`, `Image.read(0x1000, 1)` passes
  virtual containment, computes file offset `0x10`, then raises exact `ValueError` text
  `out of bounds: 0x10+0x1 / 0x3` through `span()`.
- Exact baseline and final command / CWD: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_ps4_dynlink.py`, CWD `/Volumes/Extra/SoC Mac Native`.
- Results: baseline exit 0, 29 tests; final exit 0, 30 tests. Baseline log
  `local/validation/20261022-root-fixture-baselines/re02-dynlink-image-read-file-offset-baseline.log`
  SHA-256 `7f6d674dc38c83e5df2985921ab33658727cdf69955265a530601defdf4dcbeb`. Final log
  `local/validation/20261022-root-fixture-baselines/re02-dynlink-image-read-file-offset-final.log`
  SHA-256 `359772aa50dc90b6b5176299e9d459af2dee7127197baa419a5ac5106dc03202`.
- Final test-file SHA-256 `0f769f98c6de395c040193f44e8fff4fed389d40091b6cdeda87955ddddf98de`;
  production parser remains at SHA-256
  `c0fcf34b5b9a4ad626a5b88cd2a100be37312e8038c26fb230337bc98bf5ba95`.
- Trail result: the initial `trail build --check` reported the new method as an added node and
  requested a rebuild. `trail build` then completed; the final check passed at 3,614 nodes / 1,240
  files. `trail grep 'test_image_read_rejects_file_offset_past_elf_buffer' --in scripts/` resolved
  the method to `scripts/test_ps4_dynlink.py:L357-L363`. The deep meaning layer remains unbuilt.
- Project index: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`
  both exited 0; current count 24,080 folders. `git diff --check` exited 0.
- Observation versus inference: the test proves this synthetic helper path rejects an out-of-buffer
  file offset. It does not prove malformed ELF parser policy, real ELF validity, loader behavior,
  relocation correctness, or game execution.
- Attempt count: one successful test-only implementation attempt; 0 failed implementation tries.
- Completion commit: `dc9f153e23551ef7333c34523eba2cbd2ab223bf`; pushed to `origin/main`. Pre-push
  staged audit accepted 6 paths and full audit accepted 1,704. Post-push fresh fetch and divergence
  check returned 0/0; post-push full audit accepted 1,704. Publication command output is retained
  in `local/validation/20261022-root-fixture-baselines/re02-dynlink-image-read-file-offset-publication.log`,
  SHA-256 `65ca9947b397c9c0311155f2f1517377385e7a75dee860995915ab26a3892946`.
- Final `git status --short --branch`: only pre-existing user-owned `M native/api/CMakeLists.txt`
  and `?? native/api/host/` remain; neither was staged or changed by this task.
- Remaining risks: real ELF and loader behavior are not exercised. No subsystem gate changes.
- Next exact action: review the read-only scout candidate
  `AUD-01-CLASSIFIER-PROBE-OFFSET-CUTOFF-FIXTURE-01` against live files and task records; claim it
  before editing if distinct.
- Documentation sync follows the fixture commit to record its final publication evidence.
