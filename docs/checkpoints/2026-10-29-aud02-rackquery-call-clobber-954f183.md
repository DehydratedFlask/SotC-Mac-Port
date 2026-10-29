# AUD-02 RackQuery call-clobber review at `0x94f183` — E966/E967

**Date:** 2026-10-29 08:18 CDT
**Agent:** `/root`
**Task:** `AUD-02-NGS2-RACKQUERY-CALL-CLOBBER-954F183-01`
**Status:** `STATIC_ANALYSIS_VERIFIED` for the callee's returned RDX set. Parent `AUD-02` remains ACTIVE; the RackQuery RDX origin at `0x94ebc2` remains UNKNOWN. One supported static result; zero failed semantic hypotheses.

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

## Independent byte verification and result (E967)

- Root verifier `python3 -I local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/attempt-01/verify_call_clobber.py` exited 0. It maps the pinned ELF's file-backed `PT_LOAD` ranges, matches all 104 instructions / 430 contiguous bytes in `FUN_00954d70`, and recomputes all 30 direct call/branch targets from rel8/rel32 bytes against Ghidra's flows. The 15 inner calls target `0x955860`; the only RET is `0x954f1d`.
- Direct call bytes at `0x94f183` are `e8 e8 5b 00 00`, resolving to `0x954d70`. Every path to the sole RET sets EDX after its last helper call: values 0 through 13 are written at `0x954d88` and `0x954da1..0x954ee1`; value 14 is written at `0x954f00`. Thus the machine code returns zero-extended RDX in the set `{0, 1, ..., 14}`; the exact value depends on helper results.
- The caller's negative-EAX branch at `0x94f18a` targets `0x94f370`; that route has not been traced. On the nonnegative path, the guard at `0x94f195` precedes `MOV RDX,RAX` at `0x94f1a2`, then computed jump `0x94f1ac`; Ghidra reports `0x94eb4c` as one possible dispatch entry. The RackQuery setup at `0x94eb78` is `MOV RDX,RBX`, and `0x94ebc2` calls `0xf91420`; those bytes also match the pinned ELF.
- Existing strict cached-CFG output still reports RDX at `0x94ebc2` as UNKNOWN, with call-clobber origin `0x94f183` and unverified successor entries `0x94eb4c`, `0x94f190`, `0x94f19b`, and `0x94f1ac`. The callee's bounded return set alone does not prove the value at the RackQuery argument.
- Root verifier SHA-256 `208ff0d84388303b619888364ec3571a4c0313948619870a1db4925f05e460b1`; its result `verification.json` SHA-256 `e38743d2186e1cf0e6f4f6059a97710c430b9aa557d49496a2e975b4d6dd3cd6`.
- Independent reviewer `/root/audio_contract_path` supplied verifier SHA-256 `8510dbfc4dde452ce4d7597408cfefc50ac5004c16bfb3d7b49aff0cb66fc76d` and output SHA-256 `77c1fd8652a08d30f5328b7972f6c5112087121c300013a83a4493a2e2f226c5`. Root replay exited 0 and `cmp` returned 0 against the reviewer's output.
- Three root verifier setup corrections (log suffix parsing, Ghidra function-field parsing, near-Jcc opcode recognition) and one independent verifier assertion-shape correction were fixed before acceptance; none changed the semantic hypothesis. The final independent verifiers agree. No semantic hypothesis failed.

## Observation, conclusion, and next action

**Observation:** callee `0x954d70` returns with RDX in `0..14`; caller dispatch overwrites RDX on the nonnegative path; the RackQuery pre-call instruction copies RBX into RDX.

**Conclusion:** this closes the direct callee's returned-RDX characterization, but does not resolve RDX at `0x94ebc2`. The negative branch to `0x94f370`, its reachability to the query, and the cached-CFG indirect-successor gaps remain unknown. No NGS2 ABI, codec, audio, or runtime claim.

**Next exact action:** claim a separate bounded check for only the computed-jump entry `0x94eb4c`, tracing that route to `0x94ebc2` and the reaching definition of RBX at `0x94eb78`; do not broaden to every predecessor or reinterpret the negative path without new evidence.

## Claim publication gates

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exited 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` / `--check`: 43,094 folders indexed; final check current. An earlier check reported stale because its redirected log path was created after regeneration; generation was repeated with the log path present.
- Staged audit passed 6/6 paths; all-path audit passed 3,369/3,369; attribution and staged diff passed. The first staged diff check found Markdown trailing spaces used for hard line breaks in this claim file; they were removed before the passing check.
- Exact command logs are under `local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/claim-publication/attempt-01/`.

## Result publication gates (E967)

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exited 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 43,097 folders indexed; current.
- `python3 -I scripts/audit_publish_paths.py --all`: passed 3,369/3,369 paths. `python3 -I scripts/check_commit_attribution.py`: passed (`origin/main..HEAD`). `git diff --check`: passed.
- `python3 -I scripts/audit_publish_paths.py`: passed 5/5 staged paths; `git diff --cached --check`: passed. `python3 -I scripts/audit_publish_paths.py --all`: passed 3,369/3,369; attribution and project-index checks passed.
- Exact command logs/status files are under `local/validation/AUD-02-NGS2-RACKQUERY-RDX-CALL-CLOBBER-01/result-publication/attempt-01/`.

No source changes, original-game execution, ABI recovery, native audio implementation, or game-acceptance gate is included in this result.
