# DATA-02, AUD-01, and CPU-01d `0xcdc760` verified slices

- Date/time: 2026-10-19 23:00 CDT (America/Chicago)
- Integrator: `/root`; workers: `/root/data02_probe`, `/root/aud01_probe`, `/root/cpu01d_probe`
- Task IDs: DATA-02 archive identity; AUD-01 member identity; CPU-01d one unguarded vector-copy leaf
- Source files changed: `scripts/psarc.py`, `scripts/test_psarc.py`,
  `scripts/classify_audio_headers.py`, `scripts/test_audio_headers.py`,
  `native/CMakeLists.txt`, `native/README.md`, `native/core/vec_cdc760.{hpp,cpp}`,
  `native/tests/test_vec_cdc760.cpp`, `tools/lift/check_fcdc760.py`, `docs/ASSETS.md`,
  `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, `HANDOFF.md`, and this checkpoint.
  Private outputs and logs remain under ignored `local/` and were not staged.

## DATA-02 archive identity

- Observation: the private archive is exactly 12,838,532,222 bytes. `scripts/psarc.py` now hashes
  in bounded 1 MiB reads for all archive sizes and compares device/inode/size/mtime/ctime on the
  path and open descriptor before and after the pass. It detects observed truncation, growth, or
  replacement; no archive bytes were modified.
- Exact generator command (cwd `/Volumes/Extra/SoC Mac Native`, exit 0):
  ```sh
  python3 -I '/Volumes/Extra/SoC Mac Native/scripts/psarc.py' --out '/Volumes/Extra/SoC Mac Native/local/build/psarc-pakN-summary-checkpoint-_886avv4' '/Volumes/Extra/SoC Mac Native/local/extracted/CUSA08809/pakN.psarc'
  ```
  The one-use output directory was cleaned after copying only the summary to
  `local/manifests/assets/CUSA08809-pakN.summary.json`; no member list or game data is published.
- `python3 -I scripts/test_psarc.py` — exit 0, 12/12; root log
  `local/build/psarc-tests-20261019-root.log`. Tests cover bounded chunk reads and rejecting path
  replacement during hashing.
- `shasum -a 256 local/extracted/CUSA08809/pakN.psarc` — exit 0; root log
  `local/build/pakN-sha256-20261019-root.log`; output digest
  `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`. An independent Python
  assertion confirmed archive `stat().st_size`, summary `bytes`, and summary digest agree.
- Inference boundary: this proves archive identity only. Member formats, TOC digest meaning,
  `pakN` encoding, overlay semantics and game loader use remain open. DATA-02 stays ACTIVE/partial;
  implementation failed tries: 0.

## AUD-01 member identities

- `scripts/classify_audio_headers.py` now hashes the complete decoded member while keeping magic
  and entropy observations limited to the configured prefix. Each row records PSARC entry ID,
  offset and first block. The private manifest has 86 rows across 5 archives, covering 3,726,760
  decoded member bytes.
- `python3 -I scripts/test_audio_headers.py` — exit 0, 12/12; root log
  `local/build/audio-headers-tests-20261019-root.log`. The added synthetic >4 KiB member test
  checks full payload hashing, locator metadata, a single full read, closure, and `UNKNOWN` codec.
- `python3 -I -m unittest discover -s scripts -p 'test_audio_headers.py'` — exit 0, 12/12;
  root log `local/build/audio-headers-unittest-20261019-root.log`.
- Classifier regeneration command:
  `python3 -I scripts/classify_audio_headers.py --out local/manifests/assets/audio-headers.json`
  — worker report: exit 0, 86 rows / 33 classes. Root independently recomputed all member hashes
  through `Psarc.read()` and checked entry path, ID, offset, block and size; log
  `local/build/audio-identities-verify-20261019-root.log` says 86 rows / 3,726,760 decoded bytes /
  5 archives matched.
- A first attempted test invocation, `python3 -I -m unittest scripts/test_audio_headers.py`, exited
  1 with `ModuleNotFoundError: No module named 'scripts'` because isolated Python omits cwd import
  resolution. Running the file directly passed. This is an invocation correction, not a failed
  implementation hypothesis. Codec remains `UNKNOWN`; no NGS2 or playback claim. AUD-01 stays
  ACTIVE/partial; implementation failed tries: 0.

## CPU-01d vector-copy leaf `0xcdc760`

- Hash-pinned ELF SHA-256:
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The 17-byte body is
  `c5 f8 10 05 d8 ac 61 00 c5 f8 11 05 00 55 67 02 c3`; it resolves source `0x12f7440` to
  destination `0x3351c70` and copies `{~0,~0,~0,0}`. Constructor survey position 157 and
  vector-copy manifest position 99 are checked. Source/destination are a unique non-aliased pair.
- A first `cmake --build local/build/native --target test_vec_cdc760` exited 2 because the existing
  generated Make tree had no rule yet. `cmake -S native -B local/build/native` exited 0, then the
  same build command passed. The initial transient output was not saved separately; successful
  configure/build logs are `local/build/native/cdc760-configure.log` and
  `local/build/native/cdc760-build.log`. This was stale build generation, not a failed code
  hypothesis; implementation failed tries: 0.
- `ctest --test-dir local/build/native -R '^vec_cdc760$' --output-on-failure` — exit 0, 1/1;
  `local/build/native/cdc760-ctest.log`.
- `python3 -I tools/lift/check_fcdc760.py` — exit 0; verifies ELF hash/template, signed RIP-relative
  addresses, source words, survey and copy-manifest rows, and native dump;
  `local/build/native/cdc760-checker.log`.
- `graft grep "init_vec_cdc760"` reports 4 hits in 3 source symbols; `graft check` exits 0 and says
  the wiring graph is in sync (deep meaning layer remains unbuilt). `python3 scripts/project_index.py`
  exited 0: 4,116 folders indexed. No startup registration or engine reachability is asserted. The
  `0xd71f60` escalation was not touched. CPU-01d leaf slice is FIXTURE_VERIFIED; G1 remains NOT
  PASSED and G2-G6 remain NOT RUN.

## Publication state

- CPU claim docs were already pushed: `1494700` (`docs: claim cdc760 native fixture`) and `54de3cf`
  (`docs: record cdc760 claim publication`).
- The implementation and completion-doc batch is ready for staged-path audit and publication.
  Exact commit/push results will be appended after publishing; no game-derived file is allowlisted
  or staged.
- Next action: commit the reviewed allowlisted implementation/docs, then select and claim the next
  bounded task. Cold boot, audible in-game content, real save/reload, Metal scene validation, and
  forbidden-runtime absence have not been demonstrated.
