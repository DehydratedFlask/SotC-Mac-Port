# Claim: AUD-01-CLASSIFIER-PROBE-OFFSET-CUTOFF-FIXTURE-01

- Time / timezone: 2026-10-22 01:14 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `AUD-01-CLASSIFIER-PROBE-OFFSET-CUTOFF-FIXTURE-01`, child of
  `AUD-01-CLASSIFIER-SHORT-PROBE-INPUT-FIXTURE-01`; `ACTIVE`, 0 failed implementation tries.
- Dependencies: parent `AUD-01-CLASSIFIER-SHORT-PROBE-INPUT-FIXTURE-01` is `FIXTURE_VERIFIED`.
  Live duplicate check found no matching task ID in `docs/TASKS.md`, `HANDOFF.md`,
  `docs/EVIDENCE.md`, or `docs/checkpoints/` before claim.
- Graft source context: `trail grep 'decompress_probe' --in scripts/` found the function at
  `scripts/classify_audio_headers.py:L81-L127`, the prior empty-input test at
  `scripts/test_audio_headers.py:L135-L145`, and existing accepted-offset tests. Live code records
  `too_short` whenever `len(data) <= off`; covered probe offsets are zlib 0/2, raw-DEFLATE 0/2,
  and LZMA 0/2/4.
- Baseline hashes: `scripts/test_audio_headers.py` SHA-256
  `e3447abd6c427ff010ac5b4bc8e8d4fb92b55b3fd57c46a0f73344119b62795a`; unchanged
  `scripts/classify_audio_headers.py` SHA-256
  `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Scope: one test method in `scripts/test_audio_headers.py`. For a 2-byte synthetic input, assert
  `zlib_offset2`, `raw_inflate_offset2`, and `lzma_offset2` each equal
  `{'ok': False, 'err': 'too_short'}`. For a 4-byte input, assert `lzma_offset4` equals that same
  value. Do not modify the classifier.
- Exact baseline command / CWD: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_audio_headers.py`, CWD `/Volumes/Extra/SoC Mac Native`.
- Baseline result: exit 0; 26 tests passed. Log
  `local/validation/20261022-root-fixture-baselines/aud01-probe-offset-cutoff-baseline.log`,
  SHA-256 `1bdefdd928b8e7365d8aae40791acfe542c28ed6e9f684cede822c670ee39484`.
- Observation versus inference: this pins only synthetic classifier cutoff results at lengths equal
  to the probed offset. It does not establish asset parsing, codec identity, archive behavior, or
  game playback. No unresolved user choice affects this bounded test.
- Files changed by claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint, and
  generated folder index. Implementation remains pending.
- Remaining risks: none for the test-only scope; no real audio data is read.
- Next exact action: after this claim is pushed, dispatch `/root/next_fixture_scout_2` to add only the
  test method, run `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`, and report
  diff, output, hashes, and attempt count to the root integrator.
