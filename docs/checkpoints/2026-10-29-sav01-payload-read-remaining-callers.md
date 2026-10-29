# SAV-01 remaining payload-reader callers — E960 claim and E961 result

## Claim

- Date/time: 2026-10-29 07:09 CDT. Agent/integrator: `/root`.
- Task: `SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01`; status `ACTIVE`; attempts 0; failed semantic hypotheses 0.
- Scope: inspect the three remaining known direct calls to `0x8eba0` at `0x543697`, `0x543ccd`, and `0x5460ad`. Trace the reader and destination setup plus the predecessor guards; determine whether each caller establishes eight readable payload bytes before the qword read. Byte-check the relevant bodies, path instructions, and call edges against pinned `local/re/eboot.elf`.
- Exclusions: no native lift, wiring, source edits, original-code execution, runtime binding resolution, malformed-input runtime claim, or save/load claim.
- Prerequisites: E955 (`0x8eba0` and four direct call contexts), E958 (`0x543462` caller path). Specification: `docs/REVERSE_ENGINEERING.md` Stage 2 and `docs/TASKS.md` SAV-01/SAV-02.

## Preflight

- Repository was clean on `main` at `4a081a62b9872e27261310675a83517dd78d706c`; `origin/main` and `FETCH_HEAD` matched.
- The task ID was absent from the task, evidence, index, and handoff registers.
- Exact preflight commands: `date '+%Y-%m-%d %H:%M %Z'`, `git status --short --branch`, and `git rev-parse HEAD origin/main FETCH_HEAD`. Result: local time 2026-10-29 07:09 CDT; clean worktree; all refs matched. Log: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01/claim-publication/attempt-01/preflight.log`.
- Live task-register status-cell scan found no `READY` row. Slot 309 remains the first G1 startup gap; E911 prohibits repeating generic scans without a concrete edge or principal reassessment. This claim is a separate SAV-01 serializer continuation.

## Remaining risks and next action

The original `0x8eba0` short-read path wraps without returning an error, and E955 leaves the import binding and physical backing allocation unresolved. The native parser's truncation checks do not establish original runtime behavior.

## Claim publication gates

- `cmake -S native -B local/build/orch-r22` exited 0; `cmake --build local/build/orch-r22 -j 12` exited 0; `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` passed 487/487.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` passed 572, skipped 0, failed 0.
- `trail build --check` reported the wiring graph in sync; 10,779 deep-layer nodes remain pending.
- `python3 scripts/project_index.py` wrote 43,079 folders. Its first `--check` failed with `Stale indexes: local/indexes/folders.json`; after validation logs existed, rerunning the generator and checker produced `43,079 folders indexed; current`. This was index synchronization only, not a semantic task failure.
- Exact logs for these commands are under `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01/claim-publication/attempt-01/`.
- Six allowlisted claim paths are changed; staged/all-path audits and owner attribution checks remain pending. Next exact action: run those audits, commit the claim under the owner identity, push/fetch-verify `main`, then begin the three bounded call-path queries.

## E961 result — 2026-10-29 07:26 CDT (/root)

- Result: `STATIC_ANALYSIS_VERIFIED` for the three scoped caller paths. Pinned ELF SHA-256: `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Ghidra 12.1.4 opened `local/ghidra/persistent-20261020/SotC` with `-noanalysis -readOnly`; query exit 0. The independent verifier exited 0 and checked full bodies for `0x543ba0` (355 bytes / 91 instructions) and `0x545d30` (11,797 bytes / 2,191 instructions), all direct edges in both bodies, the three call windows, and the tag-13 dispatch-table entry at `0x1016a00`.
- `0x543697` uses reader `RDI=&[RSP+0x308]` and destination `RSI=&[RSP+0x2c0]`; tag `0xd` gates the read, and the qword compare is after the call. `0x5460ad` uses `RDI=&[RSP+0x648]`, `RSI=&[RSP+0x600]`, with the same guard ordering. `0x543ccd` uses `RDI=&[RBP-0x188]`, `RSI=&[RBP-0x190]`; tag 13 dispatches to the reader case. None of these paths proves eight remaining bytes before the call.
- The REA provider timed out during fresh Ghidra startup at 330 seconds; run ID `2738ecb0-6ba0-4e8d-a56a-af7bb986e3f0`. No REA function evidence was emitted. Recovery used the persistent Ghidra project read-only. One completed Ghidra query, one provider timeout, two verifier setup corrections replayed, zero failed semantic hypotheses.
- Report, query script/log, verifier, verification output, timeout log, and checksums: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01/attempt-01/`.
- Limits: static caller evidence only. No physical over-read, runtime binding or malformed-input behavior, native lift/wiring, real save/load, or G1–G6 gate is established. The dispatch's other cases and indirect call `0x547d46` remain unresolved.
- Result publication gates: CMake configure/build exited 0; CTest exited 0 with 487/487 passed; lift checkers exited 0 with 572 passed / 0 skipped / 0 failed; `trail build --check` exited 0 with the wiring graph in sync and 10,779 deep-layer nodes pending; project-index generation and check exited 0 with 43,083 folders current. The initial index check was stale because the check's log file did not exist at generation time; after precreating it, regeneration and replay passed. Logs: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01/result-publication/attempt-01/`.
- Precommit audit: staged path audit passed 5/5; all-path audit passed 3,367/3,367; attribution and staged diff checks passed. Owner identity is Johnny Wang. Precommit fetch verified `HEAD == origin/main == FETCH_HEAD == 7af38324ebe3282d89a24f208ded827f86d834db`. Logs are in the same result-publication attempt directory.
- Files changed for the result checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and this checkpoint; no source or binary files. Next exact action: commit this result, push/fetch-verify `main`, then record the publication closeout.

## E962 publication closeout — 2026-10-29 07:40 CDT (/root)

- Result commit `bf692def93260089a2ba9b323cea34116bd29b98` (`docs: verify remaining save reader caller bounds`) changed five allowlisted docs. Owner identity: Johnny Wang. The commit was pushed to `origin main`; fetch verified `HEAD == origin/main == FETCH_HEAD`; worktree clean.
- Required gates passed for the E961 result commit and were replayed for this closeout: configure/build; CTest 487/487; lift checkers 572 passed / 0 skipped / 0 failed; Trail graph in sync with 10,779 deep-layer nodes pending; project index 43,083/current. Exact logs: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-REMAINING-CALLERS-01/result-publication/attempt-01/` and `.../attempt-02/`.
- Audits: staged path audit passed 5/5 before commit; all-path audit passed 3,367/3,367 before and after commit; explicit-path audit passed 5/5 after commit; attribution and diff checks passed. Postcommit staged audit found no staged paths and exited 2; the actual five commit paths had passed the staged audit before commit and explicit-path audit after commit.
- The first postcommit index check found newly created validation log files. After those files existed, the index regenerated and checked at 43,083/current. This is inventory synchronization only.
- E961 is `STATIC_ANALYSIS_VERIFIED` for the three named caller paths only. This closeout does not establish physical over-read, runtime binding, malformed-input behavior, native implementation, real save/load, or G1–G6. The whole native port is not complete.
- Next exact action: inspect the live task register for another bounded open item; the E911 restriction on generic slot-309 scans remains in force.
