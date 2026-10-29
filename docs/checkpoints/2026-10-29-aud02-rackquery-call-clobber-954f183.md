# AUD-02 RackQuery call-clobber review at `0x94f183` — claim (E966)

**Date:** 2026-10-29 08:18 CDT
**Agent:** `/root`
**Task:** `AUD-02-NGS2-RACKQUERY-CALL-CLOBBER-954F183-01`
**Status:** ACTIVE; one successful read-only Ghidra query, zero completed semantic hypotheses, zero failed semantic hypotheses.

## Bounded output

Independently byte-verify the direct call at `0x94f183` and the complete callee body at `0x954d70`; enumerate the callee's returned RDX values and their caller successors; then decide only whether this path narrows the RDX origin at the single RackQuery site `0x94ebc2`. Do not repeat the all-sites census, assign an unsupported NGS2 ABI/semantic name, or claim codec, playback, or runtime behavior.

## Preconditions and initial capture

- Repository was clean on `main` at `e5cce61ff21776c164f5e8d094aade2fe30c6566`. `git fetch origin main` verified that `HEAD`, `origin/main`, and `FETCH_HEAD` matched.
- Pinned plaintext ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra image base 0.
- Private Ghidra 12.1.4 saved-project query command (read-only):

  ```sh
  env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G \
    /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless \
    local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly \
    -scriptPath local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/attempt-01 \
    -postScript QueryNgsRdxCallClobber.java \
    > local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/attempt-01/ghidra.stdout.log \
    2> local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/attempt-01/ghidra.stderr.log
  ```

- The query exited 0. It identifies `0x94f183` in `FUN_0094ea10` as a direct call to `FUN_00954d70`; Ghidra reports the callee body as 430 bytes / 104 instructions. The initial capture shows EDX writes and an inner call at `0x954d83`, but that alone does not establish every returned RDX value.
- Query source SHA-256: `e76604d4f37acdd518f208bf22530603d01420e72dde0519fd28c2a19e68d00d`. Ghidra stdout SHA-256: `7933a0690f13dfa48129627822373e18dc7cdd6b355d2cb985f57a89fe67d273`. Private evidence and logs are under `local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/attempt-01/`.

## Observation, uncertainty, and next action

**Observation:** the direct target is present in the saved Ghidra project, has an ordinary function body, and contains multiple writes to EDX.

**Unknown:** whether all returns constrain RDX, whether the caller's branches preserve or replace those values, and whether the route can reach `0x94ebc2` with one of them.

**Next exact action:** independently compare the callsite and every callee instruction/control-flow target with the pinned ELF; enumerate return paths and the relevant caller successors; publish either a narrowed result or a retained UNKNOWN with precise limits.

## Claim publication gates

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exited 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` / `--check`: 43,094 folders indexed; final check current. An earlier check reported stale because its redirected log path was created after regeneration; generation was repeated with the log path present.
- Staged audit passed 6/6 paths; all-path audit passed 3,369/3,369; attribution and staged diff passed. The first staged diff check found Markdown trailing spaces used for hard line breaks in this claim file; they were removed before the passing check.
- Exact command logs are under `local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/claim-publication/attempt-01/`.

No source changes, original-game execution, ABI recovery, native audio implementation, or game-acceptance gate is included in this claim.
