# Checkpoint: SRV-01 accounting + CPU-01d vector-copy fixture batch

- Time / timezone: 2026-10-19 22:37 CDT (America/Chicago)
- Agent / integrator: `/root`; workers `/root/arena_accounting`, `/root/cpu01d_ce57b0`, `/root/cpu01d_ce0890`; root owns integration and HANDOFF.
- Task / gate / status: SRV-01 block `0x1a123–0x1a15a` and CPU-01d leaves `0xce57b0`, `0xce0890`; FIXTURE_VERIFIED at the bounded-function level; parent tasks remain ACTIVE; G1 NOT PASSED.
- Failed tries: SRV-01 has one earlier failed function-target hypothesis, then Ghidra confirmed `0x19e00`; each CPU-01d leaf has zero failed tries. No code hypothesis failed in this checkpoint.
- ELF identity: `local/re/eboot.elf`, SHA256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Files changed: `native/core/arena_accounting.{hpp,cpp}`, `native/tests/test_arena_accounting.cpp`, `tools/lift/check_f19e00_accounting.py`; `native/core/vec_ce57b0.{hpp,cpp}`, `native/tests/test_vec_ce57b0.cpp`, `tools/lift/check_fce57b0.py`; `native/core/vec_ce0890.{hpp,cpp}`, `native/tests/test_vec_ce0890.cpp`, `tools/lift/check_fce0890.py`; `native/CMakeLists.txt`, `native/README.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `HANDOFF.md`, generated `docs/FOLDERS.md`, and this checkpoint. Existing `0xce7300` edits were not staged in this batch.

## Observations and limits

- E28 is the Ghidra 12.1.4 headless export `local/re/evidence/ghidra-arena-accounting.txt`, bound to the ELF hash above and image base 0. It records `FUN_00019e00`, the raw qword update at `+0x80`, unsigned compare/update at `+0x88`, and mode-2 subtraction at `+0x90`. `+0x90` has no assigned semantic name. The native helper preserves raw offsets and does not connect to the allocator.
- For `0xce57b0`, the worker and root checker read the 17-byte body from the hash-pinned ELF and calculate the signed RIP-relative source/destination as `0x12f8330` and `0x3351cb0`; source words are `{0xffffffff, 0xffffffff, 0xffffffff, 0}`. The survey disassembly comment disagrees on target addresses, so the checker derives targets from ELF bytes and the stale comment is not relied on.
- For `0xce0890`, the 17-byte body decodes to source `0x12f7c00` and destination `0x3351c90`, with the same four source words. The private constructor survey position is 155 and reports no direct or indirect calls. Both vector fixtures use distinct host-side cells and verify an unguarded repeat overwrites prior contents.
- These byte/address/lane facts are binary observations. The C++ helpers and their tests demonstrate those bounded effects only. They establish no constructor reachability, startup order, engine behavior, or game boot.
- Independent reviewers found no blockers for the three modules. The accounting reviewer suggested an exact-size success boundary test; root added a `0x98`-byte-span case alongside the existing `0x97` rejection case.

## Exact commands and results

Working directory: `/Volumes/Extra/SoC Mac Native`.

```sh
cmake -S native -B local/build/native -G Ninja
```

Exit 1 because the existing cache uses Unix Makefiles. CMake reported that Ninja does not match the existing generator. This was an environment/configuration mismatch, not a source hypothesis; no cache was removed.

```sh
cmake -S native -B local/build/native
cmake --build local/build/native --target test_arena_accounting test_vec_ce57b0 test_vec_ce0890
ctest --test-dir local/build/native -R '^(arena_accounting|vec_ce57b0|vec_ce0890)$' --output-on-failure
python3 -I tools/lift/check_f19e00_accounting.py
python3 -I tools/lift/check_fce57b0.py
python3 -I tools/lift/check_fce0890.py
```

All exited 0. `native-fixture-batch-ctest.log` reports 3/3 passing tests. Checkers report: E28 bytes and derived accounting output match (ELF not executed); `0xce57b0` source/destination `0x12f8330`/`0x3351cb0` and expected lanes; `0xce0890` binary tie passed. Configure/build/checker logs are under `local/build/native/` with prefix `native-fixture-batch-`, `arena_accounting-`, or `check_fce*`.

```sh
python3 scripts/project_index.py
```

Exit 0; `local/build/native/project-index-fixture-batch.log` records 4,114 folders indexed.

```sh
graft build --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests
graft grep "init_vec_ce57b0|init_vec_ce0890|apply_f19e00_accounting"
graft check
```

Build exited 0 and included 519 source files as 1,149 nodes (all replayed from cache); the graph is ignored local cache. The query found all three symbols in their native files/tests. Freshness check passed. The optional deep/meaning layer remains unbuilt (0%).

## Remaining risks and next action

- No startup-ctor registration or native runtime allocator integration was added. No full-suite run was performed; only these three target tests and checkers were run.
- Actual cold game boot, game audio, real save/reload, Metal validation, and forbidden-runtime/module absence remain unproven. G1 remains NOT PASSED; G2–G6 remain NOT RUN.
- Next exact action: claim and correct the carried `0xce7300` checker/provenance metadata issue in its three existing files, run its focused CTest and hash-bound checker, then publish as a separate checkpoint. Keep it out of this batch's commit.
