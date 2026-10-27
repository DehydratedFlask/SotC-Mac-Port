# Slot-309 wrapper-table relocations and unresolved ordering (E818)

- **Date:** 2026-10-27
- **Task:** `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`; bounded read-only identification of the producer, consumer, and ordering of wrapper table `0x15d5b80/0x15d5b88`.
- **Starting state:** clean `main == origin/main == 16df074d58c21d9f340f08054df486e2c8ea1284`.
- **Owner:** `/root`; private static-analysis report supplied by `/root/slot309_dispatch` and independently reviewed by `/root/slot309_order_review`.

## Findings

The pinned local ELF SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the offline parser SHA-256 is `c0fcf34b5b9a4ad626a5b88cd2a100be37312e8038c26fb230337bc98bf5ba95`. Dynamic RELA entries 692840 and 692841 populate `0x15d5b80` and `0x15d5b88` with `0xb11360` and `0xb11370`. Saved-project Ghidra shows each cell pointing to its wrapper. Byte disassembly at `0xb11360` is a tail `JMP` to `0x254bf0`; `0xb11370` calls `0x254bf0` and then frees its object. The previously recorded body at `0x254bf0` tail-jumps to cleanup `0x240f0`.

The read-only saved-reference sweep over all 112 bytes `[0x15d5b60, 0x15d5bd0)` reports zero reference destinations. That is a bounded query of Ghidra's saved `ReferenceManager`; it does not establish that the table lacks a consumer. The relocation rows establish table contents, but no code consumer or invocation order relative to constructor-list slot 309 was found. The wrapper path only implies a cleanup route **if** a consumer invokes the pointer.

No supported pre-slot or post-slot ordering claim follows for this table. Keep slot 309 unregistered, keep the native-lift boundary unresolved, and leave G1 `PARTIAL`. The broader CPU-01 characterization remains active; this bounded table-owner subtask is closed pending a concrete consumer candidate. No game code was run and no game acceptance gate advanced.

## Evidence and review

The full private report is `local/validation/orch-20261027/slot309/agent-240f0-order/table-producer-consumer-report.md`, SHA-256 `21055248a7ead59a8073be061a520d34d54aa0bf2d455d4de2b5e0c90e8ccc77`. It records exact commands and hashes for the parser scripts, JSON output, Ghidra invocations, script logs, and stdout. Root independently recomputed the report and all 11 cited script/output/log hashes; the pinned ELF and parser hashes match. The reviewer approved the evidence and limits, and noted that `0xb11360` is a byte-level tail jump despite Ghidra categorizing its reference as an unconditional-call edge; this checkpoint uses the byte-level description.

Primary private evidence paths:

- Relocation result: `local/validation/orch-20261027/slot309/agent-240f0-order/reloc-census.json` and `reloc-entries.stdout.log`.
- Table/wrapper references: `table-owner-query.stdout.log` and the read-only Ghidra logs in the same directory.
- Bounded zero-destination sweep: `table-range-query.stdout.log` (`REFERENCE_COUNT=0`) and its Ghidra/script logs.
- Wrapper bytes: `b11350-b11390.objdump.log`, SHA-256 `5e7824cac1207735b58253b35d5754be86051c21c7b89dc7bb38de0f5ed24ac0`.
- Pinned ELF, parser, and exact queries are documented in the private report. Ghidra invocations used `-noanalysis -readOnly`.

## Repository verification

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: **PASSED**, exit 0 (`ninja: no work to do`).
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **PASSED**, 431/431.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **PASSED**, 516 passed / 0 skipped / 0 failed.
- `trail build --check`: wiring graph **PASSED**, 9,877 nodes / 13,470 edges; deep layer not built.
- `python3 -I scripts/project_index.py` and `python3 -I scripts/project_index.py --check`: **PASSED**, 31,925 folders indexed/current.
- Precommit `python3 -I scripts/audit_publish_paths.py` / `--all`: **PASSED**, 6/6 staged paths and 3,037/3,037 tracked paths. `python3 -I scripts/check_commit_attribution.py` reports `ATTRIBUTION AUDIT OK (origin/main..HEAD)`; `--identity`, staged/worktree `git diff --check`, and `git fetch origin main` passed. Precommit HEAD, `origin/main`, and `FETCH_HEAD` all equal `16df074d58c21d9f340f08054df486e2c8ea1284`.
- First strict-index check ran after creating its own new log file and found `local/indexes/folders.json` stale; after the log path existed, generation and a rerun of check passed. No source/index logic change was needed.

Gate logs: `local/validation/orch-20261027/e818/`.

## E819 publication closeout

E818 owner commit `e50179c13de2d2f165acb3ab267fd3fd42fa85b2` (parent `16df074d58c21d9f340f08054df486e2c8ea1284`) was pushed to `origin main`; fetch confirmed `HEAD == origin/main == FETCH_HEAD`, clean. Its exact-range attribution/identity and diff checks passed. The exact six committed paths passed the publication allowlist (**6/6**) and the full tracked tree passed (**3,037/3,037**). Owner author/committer is Johnny Wang `<dehydratedflask@users.noreply.github.com>`.

Fresh live GitHub ref scan covers 9 branch refs, 12 pull-request refs, 0 tags, 15 unique tips, and **2,431 reachable commits**. It found zero Claude/Anthropic author/committer identity matches and zero co-author/generated-by credit lines; live remote `main` matches local `main`. The private stash refs were left untouched by the user's explicit choice. This scan is attribution evidence only; it does not change the native-port status.

## Next action and status

Do not broaden the reference sweep or register slot 309 without a concrete producer/consumer candidate. If a separate `+0x10` ownership path is later selected, give it a bounded task with exact callsite evidence. Overall CPU-01 slot-309 characterization remains **ACTIVE**, G1 **PARTIAL**, with 0 failed semantic hypotheses. One root hash-check wrapper typo (wrong report filename) was corrected; the rerun verified every cited hash. No dependency or tool installation changed.
