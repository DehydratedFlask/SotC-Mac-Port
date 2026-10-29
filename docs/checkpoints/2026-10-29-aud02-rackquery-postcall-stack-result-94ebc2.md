# AUD-02 RackQuery post-call caller slice — static result (E973)

**Date:** 2026-10-29 09:35 CDT  
**Agent:** `/root`  
**Task:** `AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01`  
**Status:** `STATIC_PATH_BYTE_VERIFIED` for this selected static caller slice; 1 supported static result, 0 failed hypotheses.

## Scope and evidence

This completes only the claimed caller-side slice following the selected table-slot-11 candidate route from E970. It does not establish runtime slot selection, runtime reachability, stack-value provenance, NGS2/API meaning, helper or callee effects, audio, or game behavior. No successor block was inspected.

- Pinned plaintext ELF `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra 12.1.4 saved project `local/ghidra/persistent-20261020`, image base 0, opened read-only with `-noanalysis -readOnly`.
- The Ghidra query identifies caller `FUN_0094ea10` and captures the contiguous 10-instruction, 53-byte slice `[0x94ebc2, 0x94ebf7)`. An independent verifier parses the ELF64 program headers, confirms the 11-entry table and file-backed `PT_LOAD` mapping, checks the SHA-256, matches each captured instruction to file bytes in phdr 0, and recomputes direct targets.
- Verified edges: call `0x94ebc2 -> 0xf91420`, fallthrough `0x94ebc7`; the negative-result `JS` at `0x94ebc9 -> 0x95077a`; helper call `0x94ebe1 -> 0x959620`, fallthrough `0x94ebe6`; and conditional `JZ` at `0x94ebf1 -> 0x94ecd6`, fallthrough `0x94ebf7`. Targets were arithmetically recomputed; no target code was examined.
- Caller-side accesses: `0x94ebcf` reads qword `[RSP+0xe28]` into RSI; `0x94ebd7` loads RDI from `[RSP+0x10]`; `0x94ebdc` sets EDX to 1; after the helper call, `0x94ebe6` stores RAX to `[RSP+0xe20]`, then `0x94ebee` tests RAX. The `0xe28` and `0xe20` offsets differ by 8 arithmetically; no value or API contract is inferred.
- Independent reviewer `/root/audio_save_path` parsed the pinned ELF and recomputed all 10 instruction byte sequences and the selected direct edges. Review passed with no discrepancies; the reviewer inspected no successors, other routes, or callee internals.
- Root verifier: `python3 -I local/validation/AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01/analysis/attempt-01/verify_postcall.py` exited 0 and wrote `verification.json`. The Ghidra command was `env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly -scriptPath local/validation/AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01/analysis/attempt-01 -postScript QueryNgsPostCall.java`; it exited 0.
- Private scripts, capture, reports, logs and hashes are under `local/validation/AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01/analysis/attempt-01/`. E970 remains the evidence for the selected candidate slot and RBX reaching definition.

## Remaining limits

The result proves only a static path and caller-side instruction bytes for one candidate route. It does not show the path is selected or reached at runtime, explain the stack values, show what `0x959620` or `0xf91420` does, or establish an NGS2 contract, codec, audio playback, or product behavior. Parent `AUD-02` and product acceptance remain open.

## Publication verification

**2026-10-29 09:39 CDT — `/root`; task attempt 1 (static result supported, no failed hypotheses).** Changed `HANDOFF.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, `docs/INDEX.md`, `docs/TASKS.md`, the corrected E972 claim checkpoint, and this E973 result checkpoint. No dependency change. Precommit fetch verified `HEAD == origin/main == FETCH_HEAD == 13a67b349b338f5195022dc482e5c63af0ea1c8a`.

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exit 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 43,119 folders indexed/current.
- `python3 -I scripts/audit_publish_paths.py`: 7/7 staged paths passed; `python3 -I scripts/audit_publish_paths.py --all`: 3,372/3,372 paths passed.
- `python3 -I scripts/check_commit_attribution.py` and `git diff --cached --check`: passed.
- Exact command logs are under `local/validation/AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01/result-publication/attempt-01/`.

**Precommit gate result:** all publication gates passed; the E973 result checkpoint is ready for owner-authored commit/push and fetch verification. Parent `AUD-02` and native-port acceptance remain open.
