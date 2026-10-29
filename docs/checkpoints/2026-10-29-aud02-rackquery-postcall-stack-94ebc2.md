# AUD-02 RackQuery post-call stack-cell trace — claim (E972)

**Date:** 2026-10-29 09:24 CDT  
**Agent:** `/root`  
**Task:** `AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01`  
**Status:** `ACTIVE`; claim only, 0 supported and 0 failed semantic hypotheses.

## Bounded output

On computed-jump table slot 11 (`0x12ac0f4 -> 0x94eb4c`) only, byte-check the caller instructions from the `CALL 0x94ebc2` fallthrough at `0x94ebc7` through conditional branch `0x94ebf1`. The bounded slice includes the `RSP+0xe28` read at `0x94ebcf`, argument setup and call `0x94ebe1 -> 0x959620`, and store to `[RSP+0xe20]` at `0x94ebe6`; verify every selected instruction, direct edge, and memory operand against the pinned ELF. Stop at the branch; do not inspect either successor.

Do not inspect sibling computed-jump destinations, the negative pre-dispatch branch, the other 14 RackQuery raw call candidates, callee-internal memory effects, inferred API/ABI semantics, runtime reachability, audio playback, or product behavior. The address meaning remains UNKNOWN unless directly established by code.

## Claim and preconditions

- Base is clean `main` at `2b08f6b42bee7180df9bd39fbdf8e6688e3a91d2`; fetch verified `HEAD == origin/main == FETCH_HEAD`.
- Pinned plaintext ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra saved project `local/ghidra/persistent-20261020`, version 12.1.4, image base 0.
- E970 establishes that slot 11 resolves to candidate entry `0x94eb4c`, the selected case reaches `0x94ebc2`, and RBX from `0x94eb37 LEA RBX,[RSP+0xe20]` is copied to RDX at `0x94eb78`. E967/E970 establish the direct target `0xf91420`; earlier import evidence maps that thunk to symbol 239 `sceNgs2RackQueryBufferSize`.
- The E970 Ghidra transcript lists post-call instructions through `0x94ebe6`, including a later store to `[RSP+0xe20]`, but E970's raw verifier stops at the selected call. Treat the listing as a lead only; this claim must independently verify the selected post-call bytes and path before accepting any memory-reference result.
- Parent task `AUD-02` remains ACTIVE. The one-callsite row `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01` is reconciled as `STATIC_PATH_BYTE_VERIFIED`; the remaining 14 raw candidates and all NGS2 behavior remain open.

## Acceptance check

Use the Ghidra 12.1.4 persistent project read-only and an independent ELF/PT_LOAD decoder. Confirm the selected slot-11 entry from E970, the `0x94ebc2` fallthrough and negative-result edge, every instruction through `0x94ebf1`, and the direct targets of calls/branches from raw bytes. State only caller-side stack/register facts; the `+8` read's relation to `+0xe20` is arithmetic, not a value or API contract. Preserve unknowns about callee memory effects and runtime selection.

## Claim-publication checkpoint

**2026-10-29 09:30 CDT — `/root`; task attempt 1 (no analysis hypotheses tested).** Changed `HANDOFF.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, `docs/INDEX.md`, `docs/TASKS.md`, and this checkpoint. No dependency change. Base fetch verified `HEAD == origin/main == FETCH_HEAD == 2b08f6b42bee7180df9bd39fbdf8e6688e3a91d2`.

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exit 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` then `python3 scripts/project_index.py --check`: 43,115 folders indexed/current.
- `python3 -I scripts/audit_publish_paths.py`: 6/6 staged paths passed; `python3 -I scripts/audit_publish_paths.py --all`: 3,371/3,371 paths passed.
- `python3 -I scripts/check_commit_attribution.py` and `git diff --cached --check`: passed.
- The first `project_index.py --check` ran concurrently with index generation and private log creation and reported stale `local/indexes/folders.json`; regeneration after all log paths existed, followed by sequential check, passed. This was a logging/index race, not a source or task-analysis failure.
- Exact command logs: `local/validation/AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01/claim-publication/attempt-01/` and `attempt-02/`.

**Gate result:** claim documentation is ready for allowlisted commit/push. Analysis remains not run; next action is read-only Ghidra capture plus independent ELF byte/edge verification for `0x94ebc7..0x94ebf1`. The E972 task remains `ACTIVE`; the full native-port acceptance remains incomplete.
