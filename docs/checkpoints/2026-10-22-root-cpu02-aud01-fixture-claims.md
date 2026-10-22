# Checkpoint: CPU-02 and AUD-01 fixture claims

- Local date/time: 2026-10-22 00:06 CDT (America/Chicago)
- Integrator / agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
- Task IDs: `CPU-02-TAGGED31-RETAG-MAX-OFFSET-FIXTURE-01` and
  `AUD-01-XFF-ARCHIVE-SUMMARY-SHA256-MISMATCH-FIXTURE-01`
- Status: both `CLAIMED`; 0 implementation attempts and 0 failed tries for each.
- Files changed for this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and the
  generated project index if refreshed below. No source or test file changed.
- Ownership: `/root/next_bounded_scout` owns only `native/tests/test_addr.cpp` for the CPU fixture.
  Root owns only `scripts/test_audio_bank_header.py` and the count sentence in `docs/ASSETS.md` for
  the audio fixture. Root is sole integrator for shared handoff, task, evidence, and checkpoint docs.
- Exact baseline commands and results:
  - `cmake --build local/build/native --target test_addr`: exit 0.
  - `ctest --test-dir local/build/native -R '^addr$' --output-on-failure`: exit 0, 1/1 passed.
  - `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`: exit 0, 20 tests.
- Index command: `python3 scripts/project_index.py && python3 scripts/project_index.py --check`;
  exit 0, 24,078 folders indexed and current.
- Publication audit: `python3 -I scripts/audit_publish_paths.py` accepted the 4 staged paths;
  `python3 -I scripts/audit_publish_paths.py --all` accepted 1,689 paths. Logs are
  `local/validation/20261022-root-fixture-baselines/claim-audit-staged.log` (SHA-256
  `031592184d0ed78761ff640e528cf2a625bb735b9ae906c4843a84c46076f099`) and
  `local/validation/20261022-root-fixture-baselines/claim-audit-all.log` (SHA-256
  `c3e90d98ae955deb2f7840ca4f533da8a2b8844d43f0aeb849d72d9df2e40efc`).
- Baseline evidence:
  - `local/validation/20261022-root-fixture-baselines/cpu02-test-addr-build.log` SHA-256
    `6072e7fbcb139a83329e6e872fbf970094315d0ebf087314b6576e8cc474bdeb`.
  - `local/validation/20261022-root-fixture-baselines/cpu02-addr-ctest.log` SHA-256
    `f8373524996bcec5a72d9d84b0314514c2809afa184af468c53711c62e471eac`.
  - `local/validation/20261022-root-fixture-baselines/aud01-audio-bank-header.log` SHA-256
    `8c415ff8f67af64cec284b21a333d46083fc9459253f9cdfdc1a96f0074786a0`.
- Baseline source hashes: `native/tests/test_addr.cpp` is
  `793bcfa273a218f055a0d6c22637f2590e7a52252175dc46060ab5b7522dffb9`; unchanged
  `scripts/test_audio_bank_header.py` is
  `90f18dd3a98578e822a3674ef3d3d71fc237b89b46eb3f350dbb3460e7cd4833`.
- Observations: `docs/POINTERS.md` specifies offsets 0 through `0x7fffffff`. The current native
  fixture decodes raw `0xffffffff`, but does not retag an untagged maximum offset inside a full
  2 GiB `HeapWindow`. The audio reader separately checks summary byte count and digest before
  constructing `Psarc`; the existing test only corrupts the byte count and asserts `Psarc` is not
  constructed. The current audio test file has 20 methods while `docs/ASSETS.md` says 18.
- Inference: one synthetic test per file closes these two distinct fixture gaps without changing
  production code. This does not validate the console binary, real asset behavior, playback, or any
  game/runtime gate.
- Remaining risks: the CPU fixture proves only arithmetic in the native helper. The audio fixture
  proves only the pre-open summary-validation branch over synthetic files. Project completion gates
  remain unchanged.
- Exact next action: after this claim is committed and pushed, assign the CPU fixture to
  `/root/next_bounded_scout`; root adds the audio digest mismatch case and updates the ASSETS count
  to 21. Run the focused native and Python checks, record completion evidence, update the shared
  handoff, refresh indexes, audit allowlisted paths, then commit and push each completion checkpoint.
- Pending choices remain untouched: thin Mach-O CPU acceptance and PSARC negative-limit semantics.
