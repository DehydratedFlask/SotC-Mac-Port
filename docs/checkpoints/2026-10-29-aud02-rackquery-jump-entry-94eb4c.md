# AUD-02 RackQuery dispatch-entry review at `0x94eb4c` — claim (E968)

**Date:** 2026-10-29 08:38 CDT
**Agent:** `/root`
**Task:** `AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01`
**Status:** `STATIC_ANALYSIS_VERIFIED` for this selected candidate route; parent `AUD-02` remains ACTIVE. One supported static result, zero failed semantic hypotheses.

## Bounded output

Verify the Ghidra-reported candidate computed-jump entry `0x94eb4c` from dispatch at `0x94f1ac`, trace only that selected route to RackQuery call `0x94ebc2`, and determine RBX's reaching definition at `0x94eb78`. Independently compare relevant instruction bytes and direct flow targets against the pinned ELF. Keep the result UNKNOWN if the entry, route, or reaching definition cannot be established.

Do not enumerate sibling indirect destinations, trace the negative branch from `0x94f18a` to `0x94f370`, infer an NGS2 ABI/semantic name, or claim codec, playback, runtime, or product behavior.

## Claim and preconditions

- Base is clean `main` at `409ed112cf1d285dd4973756c7370a915d367dba`; fetch verified `HEAD == origin/main == FETCH_HEAD`.
- Pinned plaintext ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra saved project `local/ghidra/persistent-20261020`, version 12.1.4, image base 0.
- E967's independently verified observations are limited to callee `0x954d70` returning zero-extended RDX in `0..14`, a nonnegative caller dispatch that sets RDX to a jump-table base at `0x94f1a2`, Ghidra's report of `0x94eb4c` as one possible dispatch entry, and `MOV RDX,RBX` at `0x94eb78` before call `0x94ebc2`. E967 did not establish this entry's actual route or RBX's value.
- Claim changes only the allowlisted docs listed in E968. No source edits, local game execution, or game/firmware acquisition.

## Observation, uncertainty, and next action

**Observation:** the current saved-analysis report names `0x94eb4c` as a possible target of the computed jump at `0x94f1ac`; the selected RackQuery path consumes RBX through `MOV RDX,RBX` at `0x94eb78`.

**Unknown:** whether the pinned instruction bytes and control flow confirm this entry, whether the selected route reaches `0x94ebc2`, and which value reaches RBX at the move.

**Next exact action:** use the existing Ghidra project read-only to capture the selected block path and reaching-definition chain for RBX, then independently verify those bytes and edges against the pinned ELF. Do not inspect other jump-table entries.

## Claim publication gates

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exited 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 43,102 folders indexed; current.
- `python3 -I scripts/audit_publish_paths.py`: passed 6/6 staged paths; `python3 -I scripts/audit_publish_paths.py --all`: passed 3,370/3,370 paths.
- `python3 -I scripts/check_commit_attribution.py` passed (`origin/main..HEAD`); `git diff --cached --check` passed; `python3 scripts/project_index.py --check` confirmed 43,102 folders current.
- Exact logs/status files are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/claim-publication/attempt-01/`.

## Claim publication closeout (E969)

- Owner-authored commit `12ad9e2ad6fe6f969bdea753c5c5a1df4a659621` (`docs: claim RackQuery dispatch entry review`) changed six allowlisted docs paths and was pushed to `origin main`; fetch verified `HEAD == origin/main == FETCH_HEAD`, with a clean worktree.
- Postcommit `python3 -I scripts/audit_publish_paths.py --all` passed 3,370/3,370; attribution, `git show --format= --check HEAD`, and `python3 scripts/project_index.py --check` (43,102 folders current) passed.
- Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/claim-publication/attempt-02/`. This closes publication only; the static task remains ACTIVE with 0 completed and 0 failed semantic hypotheses.

## Static result and independent verification (E970)

### Commands and evidence

- Pinned input remains `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra 12.1.4 saved project `local/ghidra/persistent-20261020`, image base 0. No original executable was run.
- Root read-only query command (exit 0):

  ```sh
  env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly -scriptPath local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/attempt-01 -postScript QueryNgsJumpEntry.java
  ```

- Root byte verifier (exit 0): `python3 -I local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/attempt-01/verify_jump_entry.py`. It independently parses ELF64 program headers and verifies the selected table slot, target arithmetic, dispatch/setup bytes, direct edges, selected route and RBX reaching-definition path.
- Root query SHA-256 `99ec3f380eca035e3a6ae6e43e3d41ce6f386d5ef84174d2662b1dda97a858a`; verifier SHA-256 `133521d551cf9516adb1c0ab1b62619992b9d8afcc974dfd0bcb01cceb5f094b`; `verification.json` SHA-256 `460bb38b6b60ce6fcc43f6db74b0be1185804735a123912ac70e7fff3e9e00d3`; Ghidra stdout SHA-256 `2fa45c8586d73f880e47b654307a79fa5476d9285d06da7deed45abca1df9d1b`.
- Independent reviewer `/root/audio_contract_path` ran a separate saved-project query and raw ELF checks. The first report verified the selected dispatch reference and all 17 route instructions (plus dispatch bytes); the upstream follow-up verified 42 unique instructions, including the caller path and the callee's RBX save/overwrite/restore sequence. Reports, scripts and exact logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/independent-review/`.
- REA `analyze_function` and `inspect_native_instruction` requests did not return and were cancelled. No REA Evidence ID was returned. This is recorded as provider timeout only; the saved-project Ghidra query and byte verifier provided the accepted evidence.

### Observations and limits

- The dispatch instruction at `0x94f1ac` is `JMP RAX`. Its relative table base is `0x12ac0c8`. Slot 11 is at `0x12ac0f4`; raw bytes `84 2a 6a ff` decode as signed `-9,819,516`, yielding target `0x94eb4c`. This agrees with the selected computed-jump reference in the saved Ghidra project. The bytes prove that index 11 addresses this candidate entry; they do not prove a runtime execution or runtime index.
- The selected case contains 17 contiguous instructions through call `0x94ebc2`; all 17 route instruction byte sequences match file-backed PT_LOAD bytes. The call displacement resolves to `0xf91420`.
- `0x94eb37` defines RBX with `LEA RBX,[RSP+0xe20]`; no caller-side RBX write intervenes on the selected route. The caller invokes `0x954d70` at `0x94f183`; that callee saves RBX at `0x954d74`, overwrites it at `0x954d76`, then restores it at `0x954f1b` before the sole recorded RET at `0x954f1d`. At `0x94eb78`, `MOV RDX,RBX` therefore supplies the stack-relative address formed from RSP at `0x94eb37` plus `0xe20` on this route. Its purpose and concrete runtime address remain unknown.
- E967's verified return set for the callee is RDX `0..14`, but this does not establish that the caller selects table index 11. No NGS2 ABI/API interpretation, codec, playback, runtime behavior, or product behavior follows.
- Attempt accounting: one supported static result; zero failed semantic hypotheses. Root parser/setup corrections preceded the passing replay. Two REA provider requests timed out and were cancelled; they are not semantic attempts.

### Result publication checkpoint

Precommit result gates: `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12` exited 0; CTest exited 0 with 487/487 passed; checker sweep reported 572 passed, 0 skipped, 0 failed; `trail build --check` exited 0 with the wiring graph in sync and 10,779 deep-layer nodes pending; `python3 scripts/project_index.py` and `--check` exited 0 at 43,110 folders/current; staged path audit passed 5/5; all-path publish audit passed 3,370/3,370; attribution and staged diff checks passed. Precommit fetch verified `HEAD == origin/main == FETCH_HEAD == ef0f4137923ff0675ed77022b7d5f5d2c4018466`. Exact command logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/result-publication/attempt-01/`.

## Publication closeout (E971)

- E970 result commit `9a4997c5f074e04e471578118931b0a41160def2` (`docs: verify RackQuery dispatch entry`) changed five allowlisted documentation files. Push exited 0; fetch verified `HEAD == origin/main == FETCH_HEAD` at that SHA; worktree clean.
- Precommit gates passed: configure/build, CTest 487/487, checker sweep 572/0/0, Trail wiring graph in sync with 10,779 deep-layer nodes pending, project index 43,110/current, staged path audit 5/5, all-path audit 3,370/3,370, attribution and staged diff.
- Postcommit checks passed: all-path audit 3,370/3,370, attribution, `git show --format= --check HEAD`, and project-index check at 43,110/current. Exact logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/result-publication/attempt-01/`.
- Before publishing this closeout record, configure/build, CTest 487/487, checker sweep 572/0/0, Trail graph in sync with 10,779 deep-layer nodes pending, project index 43,111/current, staged audit 3/3, all-path audit 3,370/3,370, attribution and staged diff passed. Fetch verified `HEAD == origin/main == FETCH_HEAD == 9a4997c5f074e04e471578118931b0a41160def2`. Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/result-publication/attempt-02/`.
- Overall `AUD-02` and G1-G6 remain open.
