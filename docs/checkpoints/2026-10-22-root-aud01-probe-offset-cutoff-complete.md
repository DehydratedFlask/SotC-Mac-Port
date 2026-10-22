# AUD-01 classifier probe offset cutoff fixture — complete

- Time / timezone: 2026-10-22 01:18 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `AUD-01-CLASSIFIER-PROBE-OFFSET-CUTOFF-FIXTURE-01`, child of
  `AUD-01-CLASSIFIER-SHORT-PROBE-INPUT-FIXTURE-01`; `FIXTURE_VERIFIED`.
- Claim checkpoint: [claim record](2026-10-22-root-aud01-probe-offset-cutoff-claim.md), claim commit
  `0cd636e411b7305546555ca2d137c3ec4ff6395c`, pushed to `origin/main`.
- Owned implementation file: `scripts/test_audio_headers.py` only. Added
  `test_probe_offsets_equal_to_input_length_report_too_short`; the classifier was not changed.
- Baseline test-file SHA-256 `e3447abd6c427ff010ac5b4bc8e8d4fb92b55b3fd57c46a0f73344119b62795a`;
  final test-file SHA-256 `dd8b580307a3fe3a5998575bc2251126b74d7d3737f1d37cb09fe1da21646349`. The unchanged
  `scripts/classify_audio_headers.py` SHA-256 is `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Hypothesis and assertions: a 2-byte input ends exactly at the zlib, raw-DEFLATE, and LZMA
  offset-2 probes, so each result is `{'ok': False, 'err': 'too_short'}`. A 4-byte input ends at
  the LZMA offset-4 probe and returns the same result.
- Exact baseline/final command and CWD: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_audio_headers.py`, CWD `/Volumes/Extra/SoC Mac Native`. Baseline exited 0 with
  26 tests; worker and root final runs exited 0 with 27 tests. Baseline, worker, and root logs are
  `local/validation/20261022-root-fixture-baselines/aud01-probe-offset-cutoff-baseline.log`,
  `.../aud01-probe-offset-cutoff-worker.log`, and `.../aud01-probe-offset-cutoff-final.log`, with
  SHA-256 values recorded in E270.
- `git diff --check` exited 0. `python3 scripts/project_index.py` exited 0 and reported 24,080
  folders indexed; `python3 scripts/project_index.py --check` exited 0 and reported current at
  24,080 folders. Check log: `local/validation/20261022-root-fixture-baselines/aud01-probe-offset-cutoff-work.log`.
- Trail wiring graph check passed at 3,615 nodes / 1,240 files; the deep meaning layer is not built.
  `trail grep 'decompress_probe' --in scripts/` resolves the new test method to
  `scripts/test_audio_headers.py:L147-L154`.
- Observation versus inference: the assertions pin synthetic probe cutoff results at lengths equal
  to offsets. They do not establish asset parsing, codec identity, archive behavior, or game
  playback.
- Attempt count: one successful test-only implementation attempt; 0 failed implementation tries.
- Completion commit: `f82942d7688f8fdc88274540ab72ebf38eeedcf6`, pushed to `origin/main`. The staged
  audit accepted 6 paths and full audit accepted 1,706. Fresh fetch/divergence returned 0/0 and
  post-push full audit accepted 1,706. Publication output is retained in
  `local/validation/20261022-root-fixture-baselines/aud01-probe-offset-cutoff-publication.log`,
  SHA-256 `57a47a09232b7e9e9c8032802654962fa31e3a7a1864cdac3c5a5b22675e1aff`.
- Working-tree boundary: existing user-owned `native/api/CMakeLists.txt` and `native/api/host/`
  remain untouched and outside this task.
- Remaining risks: none for this synthetic fixture. No subsystem gate changes.
- Next exact action: verify the scout's `RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01`
  candidate against the live task ledger, parent specification, and wrapper source, then claim it
  before editing if it remains distinct.
