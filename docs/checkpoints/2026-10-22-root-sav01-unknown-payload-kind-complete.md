# Checkpoint: SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 03:12 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  implementation worker `/root/next_fixture_scout_2`.
- Task / gate / status: `SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01`, child of
  `SAV-01-NATIVE-STREAM-01`; `FIXTURE_VERIFIED`; one successful implementation attempt and zero
  failed tries. The worker's first patch attempt had a context mismatch and changed no source; the
  second patch applied the one proposed assertion. Focused validation passed on the worker and on
  root's independent fresh build.
- Files changed: `native/tests/test_save_chunk_codec.cpp`; `HANDOFF.md`; `docs/TASKS.md`;
  `docs/EVIDENCE.md`; this checkpoint; the claimed checkpoint's CMake hash transcription was
  corrected from a typo to the live hash; and generated `docs/FOLDERS.md`. `INSTALL_LOG.md`
  is unchanged. Pre-existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/`
  are untouched and must remain unstaged.
- Scope: one synthetic case in `test_decode_by_spec_every_kind()`, constructing a matching-tag
  chunk for `static_cast<PayloadKind>(0xff)` and asserting `CodecError::wrong_value_type`. No
  production code, headers, or build files changed. Final test SHA256 is
  `ce9c3d7091d56209e3ba446a54f4aec66f830ce97a3cb3d2613b3c293f34b29e`; unchanged decoder,
  header, and `native/CMakeLists.txt` SHA256 values are `e9c3f01b321377fccebc2feab24ada7238ad2ce1aa0d992277d503b97978598d`,
  `aabdd64ab2ce3cbf8d359572e1b2de958e984ddc4aac6925ec8337253a76f88a`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Exact worker commands (CWD `/Volumes/Extra/SoC Mac Native`; logs under
  `local/validation/20261022-root-fixture-baselines/`):
  `cmake --build local/build/native --target test_save_chunk_codec` — exit 0, log SHA256
  `1a2e14bbc7858d7e8f4303ca7e5178d993b2a0894b954b0db933d00ab4fc33b1`;
  `ctest --test-dir local/build/native --output-on-failure -R '^save_chunk_codec$'` — exit 0,
  1/1 passed, log SHA256
  `5ed2ca29b8991b8d201a146f5703132503827b4e6c39465df016a10f412a293e`.
- Exact independent root commands, fresh build directory `local/build/native-root-sav01-unknown-payload-final`:
  `cmake -S native -B local/build/native-root-sav01-unknown-payload-final -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, log SHA256
  `63715121bc9b2646837d4a186f4319cdb853fb11161ede8ea49cc3f471f019b1`;
  `cmake --build local/build/native-root-sav01-unknown-payload-final --target test_save_chunk_codec`
  — exit 0, log SHA256
  `7df794f73fee91955631dbace9fe3eaf172cadcce3db0faec3aa3145cee216c4`;
  `ctest --test-dir local/build/native-root-sav01-unknown-payload-final --output-on-failure -R '^save_chunk_codec$'`
  — exit 0, 1/1 passed, log SHA256
  `6f382e9a60646003c85bbc135c01386c78c422e2d5278a5e1b6a528d0847bcd4`.
- Trail: `trail build --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests`
  exits 0 and reports 3,618 nodes / 1,240 files; log SHA256
  `3afc77bcb0785d1580714e62fc5a4d302610c1c42c53f5fb5a7a7af082c04626`.
  `trail build --check` exits 0; the wiring graph is in sync and the deep meaning layer remains
  unbuilt; log SHA256 `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc`.
  `trail grep "test_decode_by_spec_every_kind"` resolves the test at
  `native/tests/test_save_chunk_codec.cpp:L828-L886` and `main` at L909; log SHA256
  `57081b3fdbd95991bcac650764b28eb196c158a7e4e77eef405f9f002c1bb398`.
- Project index: `python3 scripts/project_index.py` exits 0, reports 31,336 folders, and writes
  `local/indexes/folders.json`; log SHA256
  `8d96cd1a29b1fbaa84e751da55b630e215bfcab4d20f830b82f9af4322b54fe4`.
  `python3 scripts/project_index.py --check` exits 0 at 31,336 folders; log SHA256
  `f71f44e8af43bbbfe99e4c907ad85f26fcaa602e134723980ca4fc148707ccea`.
  An earlier immediate check reported the index stale because a new log path had been created
  after generation; its redirected log was overwritten by the successful rerun. All validation
  log paths were then precreated before this final successful generation/check.
  `git diff --check` exits 0; log SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Observation versus inference / limits: the new spec tag matches the chunk, so the decoder
  reaches the unknown-kind switch fallback and returns `wrong_value_type`. The enum comment labels
  that error as a writer error, while the decoder also uses it here. This pins only current
  synthetic decoder behavior; save-format compatibility and game behavior are not established.
- Claim publication: claim commit `ff63a179c0c9000a6a9a71472741c5657ffee9ef` is on
  `origin/main`; E298 records its 0/0 divergence and path-audit evidence. Completion publication is
  pending and will be recorded in E300.
- Next exact action: run staged and full publish-path audits, explicitly stage allowlisted
  completion files, commit, and push. After publication, record commit/audit evidence and claim
  `SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01` from the independent scout.
