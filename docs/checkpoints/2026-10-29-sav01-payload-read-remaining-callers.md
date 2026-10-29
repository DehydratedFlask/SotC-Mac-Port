# SAV-01 remaining payload-reader callers — E960 claim

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
