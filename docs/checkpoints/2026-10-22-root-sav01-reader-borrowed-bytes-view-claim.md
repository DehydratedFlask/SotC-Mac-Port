# Claim checkpoint: SAV-01-CHUNK-CODEC-READER-BORROWED-BYTES-VIEW-FIXTURE-01

- Local date/time: 2026-10-22 06:48 CDT (America/Chicago). Claim owner/integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); proposed implementation worker: `/root/next_bounded_fixture_scout_22`.
- Task / parent / status: `SAV-01-CHUNK-CODEC-READER-BORROWED-BYTES-VIEW-FIXTURE-01`, child of `SAV-01-CHUNK-CODEC-01`; `ACTIVE`, zero implementation attempts and zero failed tries.
- Bounded ownership: worker may change only `native/tests/test_save_chunk_codec.cpp`, adding a nonempty borrowed-view case inside `test_primitives_golden`. Root owns shared docs, independent replay, and publication. Production code, headers, CMake, API/Graft paths, reference clones, and game files are out of scope.
- Baseline HEAD `34a10b530e4b5e03ad1798d7bb098690efed1b69`. Test/helper/header/CMake SHA256: `ce9c3d7091d56209e3ba446a54f4aec66f830ce97a3cb3d2613b3c293f34b29e` / `e9c3f01b321377fccebc2feab24ada7238ad2ce1aa0d992277d503b97978598d` / `aabdd64ab2ce3cbf8d359572e1b2de958e984ddc4aac6925ec8337253a76f88a` / `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Navigation and live-source evidence: `PayloadReader::read_bytes` checks remaining length, returns `data_.subspan(pos_, count)`, and advances position; the header documents that returned view aliases the input. Existing test coverage pins short read and zero-count success but no nonempty borrowed view. Graft output is in `local/validation/srv01-f19e00-zero-request-20261022/root-graft-next-sav01-borrowed-view.log` (SHA256 `137d3dccf3e32fa4c63b5a7e204c15fb392138cc9a2a4d9f83fda6880fc33592`). CTest `save_chunk_codec` is registered for `test_save_chunk_codec`.
- Hypothesis and expected result: with bytes `{0xAA, 0x11, 0x22, 0xBB}`, consume the prefix byte, then read two bytes. The result has `CodecError::none`, aliases `bytes.data()+1`, contains `0x11, 0x22`, reports size 2 and reader position 3, with one byte remaining.
- Exact acceptance commands from repository root:
  1. `cmake -S native -B local/build/native-worker-sav01-reader-borrowed-bytes-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  2. `cmake --build local/build/native-worker-sav01-reader-borrowed-bytes-20261022 --target test_save_chunk_codec`
  3. `ctest --test-dir local/build/native-worker-sav01-reader-borrowed-bytes-20261022 --output-on-failure -R '^save_chunk_codec$'`
- Expected acceptance: configure/build exit 0 and focused `save_chunk_codec` CTest 1/1. At claim time, these implementation acceptance commands have not run. Root will replay them in a separate build directory after worker completion.
- Observation versus inference / limitation: synthetic reader API alias/cursor behavior only. This does not establish real-save compatibility, loader integration, or game save/load behavior.
- Risks: keep the change test-only and preserve each test's distinct build/log paths. Claim publication must precede worker dispatch.
- Exact next action: after publication, dispatch the one-file test change to `/root/next_bounded_fixture_scout_22`; root will independently replay the focused target.
