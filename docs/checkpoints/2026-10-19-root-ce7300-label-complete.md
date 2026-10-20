# Checkpoint: CPU-01d `0xce7300` provenance and index-label correction

- Time / timezone: 2026-10-19 22:41 CDT (America/Chicago)
- Agent / integrator: `/root`.
- Task / gate / status: CPU-01d fixture metadata correction; FIXTURE_VERIFIED; failed tries 0.
- Files changed: `native/core/vec_ce7300.hpp`, `native/tests/test_vec_ce7300.cpp`, `tools/lift/check_fce7300.py`, `native/README.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `HANDOFF.md`, and this checkpoint. The feature computation, CMake registration, and startup registration were unchanged.
- Input: `local/re/eboot.elf`, SHA256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Observations and changes

- The original lift owner is `bc-effcf5dc`; the previous unstaged header edit incorrectly attributed it to `bc-02397584`.
- Survey row 149 identifies RVA `0xce7300`; vector-copy manifest row 91 identifies the same constructor. Survey row 91 is `0xd7c380`, and vector-copy row 149 is `0xc42b00`.
- The fixture's `--dump` JSON now names both fields as `survey_index: 149` and `copy_manifest_index: 91`, and emits hexadecimal source/destination constants rather than unexplained decimal literals.
- The checker pins the ELF hash, decodes signed RIP-relative offsets from the 17 bytes at `0xce7300`, checks source lanes, validates both survey/manifest rows and their fields, compares the native fixture output, and checks the optional private body dump. It does not execute the ELF.
- These are metadata and checker-clarity corrections. They do not add runtime behavior, startup coverage, or G1 evidence.
- Independent review confirmed the two list positions, checker assertions, provenance update, and absence of runtime behavior changes; no blockers were reported.

## Exact commands and results

Working directory: `/Volumes/Extra/SoC Mac Native`.

```sh
cmake --build local/build/native --target test_vec_ce7300
ctest --test-dir local/build/native -R '^vec_ce7300$' --output-on-failure
python3 -I tools/lift/check_fce7300.py
git diff --check
```

All exited 0. Logs: `local/build/native/ce7300-build.log`, `ce7300-ctest.log`, and `ce7300-checker.log`. CTest passed 1/1. Checker output: `OK: 0xce7300 source=0x12f8bb0 destination=0x3351cf0 bits=~0,~0,~0,0; survey position 149; vector-copy manifest position 91; native dump matches`.

```sh
graft check
graft build --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests
graft check
graft grep "copy_manifest_index"
```

The initial freshness check reported the three edited files stale. Rebuilding with the preserved scopes exited 0; the next check passed, and the query found `copy_manifest_index` in the checker and test. Graft's meaning/deep layer remains unbuilt.

## Remaining risks and next exact action

This follow-up adds no new function or runtime proof. It does not establish cold boot, actual game audio, real save/load, Metal validation, or forbidden-runtime/module absence. G1 remains NOT PASSED; G2–G6 remain NOT RUN. Next exact action: choose and claim one bounded, unblocked CPU-01/SRV-01 slice after checking its evidence and dependencies.
