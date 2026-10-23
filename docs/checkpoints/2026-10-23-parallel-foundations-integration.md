# Checkpoint — parallel-foundations work integrated (save temp isolation, shared-string release, rackquery CFG)

- Local: 2026-10-23 08:0x (America/Chicago)
- Agent: integration worker; integrator replay
- Task: `PAR-INTEGRATE-WIP-01`
- Gate: **FIXTURE_VERIFIED — no runtime/game claim**
- Evidence: E566

## Source

Commit `a49fd2a` on `origin/cursor/parallel-foundations-wip-20261023`, preserved by the previous
round after its worktree was removed. Ten files; nothing had been ported to the current branch.

## Ported

1. `native/core/save_stream.{hpp,cpp}` + `native/tests/test_save_stream_file.cpp` — atomic save
   writer now exclusively creates a unique `<path>.tmp.<rand>.<rand>` with `O_EXCL`, mode 0644
   subject to umask, 128 retries on `EEXIST`, stale temps/aliases preserved. The old shared
   `path + ".tmp"` + `O_TRUNC` version was still in the tree; the new alias/umask regressions detect
   it (mutation restored the old code and `save_stream_file` failed at line 130, exit 8).
2. `native/core/shared_str_release_f590.{hpp,cpp}` + test — full shared-string release with an
   external typed deallocator, `atomic_ref<int64_t>` decrement, control reload after the data
   callback, wrap-safe previous-bit test, no field clearing. Distinct from the existing
   `shared_release_f590` holder model.
3. `scripts/aud02_rackquery_cfg.py` + `scripts/test_aud02_rackquery_cfg.py` — stdlib-only
   conservative reaching-origin solver over a hash-pinned cached CFG; calls clobber all GPRs unless
   explicitly preserved; private per-site output is refused outside a Git-ignored path. 20 synthetic
   tests pass.
4. `tools/lift/check_ff590.py` — byte oracle for the 73 B helper and 10 B wrapper, two register
   poisons, external `0x12af0` seam.
5. `native/CMakeLists.txt` — source/test registration.

## Verification

| Command | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | exit 0, `-Werror`, ASan+UBSan |
| `ctest ... -R 'save_stream_file\|shared_str_release_f590\|shared_release_f590\|hashed_string_14c3a0'` | 4/4 pass |
| full `ctest -j 12` | 340/340 pass (before the later registry modules) |
| `python3 -m unittest scripts.test_aud02_rackquery_cfg` | 20/20 OK |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 python3 -I tools/lift/check_ff590.py` | OK |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | 440 passed, 0 skipped, 0 failed |
| private aud02 rerun ×4 (seed variations) | byte-identical `sha256 6d81ac74…bf315` |

Active Try counts: one successful implementation attempt for each ported item; zero failed
hypotheses. The only non-zero exits were the deliberate mutation/restore pair.

## Limits and risks

- Temp names are ~25 characters longer; a destination at the path limit could now fail
  `ENAMETOOLONG`.
- Concurrent writers to the same destination still race on the final rename (ordering unspecified).
- `release_shared_str_f590` leaves `0x12af0` external and is not wired into Init.
- `aud02_rackquery_cfg.py` fails closed on input-pin drift; the 11 unresolved `arg3` origins stay
  `unknown` by design.
- No game/runtime claim. **FIXTURE_VERIFIED.**
