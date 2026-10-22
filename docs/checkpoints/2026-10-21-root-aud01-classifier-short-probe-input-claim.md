# AUD-01 empty decompression-probe input — claim checkpoint

- Local date/time: 2026-10-21 23:37 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `AUD-01-CLASSIFIER-SHORT-PROBE-INPUT-FIXTURE-01`, child of `AUD-01`.
- Status: `ACTIVE`, baseline verified; 0 successful implementation attempts, 0 failed tries.
- Specification: [AUD-01 in docs/ASSETS.md](../ASSETS.md#aud-01-header-classification-and-member-identity-codec-unknown).
- Bounded scope: add one direct test in `scripts/test_audio_headers.py` calling
  `cah.decompress_probe(b'')` and asserting the exact returned mapping contains all seven zlib,
  raw-DEFLATE, and LZMA offset names, each mapped to `{'ok': False, 'err': 'too_short'}`. The
  production classifier already has explicit input-length guards. No classifier or archive code
  changes, and no codec, game-asset, or audio-playback inference.
- Baseline command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` exited 0;
  25 tests passed. Log `local/validation/aud01-classifier-short-probe/baseline.log` SHA256
  `e88eaf6e50984f6551445a6c9e77e4ca9e63f0e93ed442e493d2682e576a4f19`.
- Baseline SHA256: `scripts/test_audio_headers.py`
  `0728afcf3e614e54a60be4fa0dcefe6b6aa9cf69a659974ec47bcfced583d5c8`;
  unchanged `scripts/classify_audio_headers.py`
  `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Trail `grep "decompress_probe"` located the function at
  `scripts/classify_audio_headers.py:81-127` and existing direct probe tests in
  `scripts/test_audio_headers.py:135-196`. The existing tests cover accepted offsets and trivial
  outputs but no empty-input case.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` exited 0;
  24,074 folders indexed and current. Logs are under
  `local/validation/aud01-classifier-short-probe/`.
- Claim publication preflight: `git diff --cached --check` exited 0; the staged publish audit
  accepted 4 allowlisted files; the full audit accepted 1,683 paths. `git fetch origin` exited 0 and
  `git rev-list --left-right --count HEAD...origin/main` returned `0 0`. Logs are
  `claim-diff-check.log`, `claim-staged-audit.log`, `claim-all-audit.log`, `claim-fetch.log`, and
  `claim-divergence.log` in `local/validation/aud01-classifier-short-probe/`.
- Exact next action: add the single test, run its focused unittest and the full 26-test file, then
  update completion evidence and publish allowlisted changes. Keep `AUD-01` codec identification
  and all game-derived files outside this task.
