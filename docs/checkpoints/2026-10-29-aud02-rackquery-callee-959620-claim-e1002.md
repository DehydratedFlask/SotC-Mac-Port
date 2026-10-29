# E1002 — RackQuery target byte-slice claim

Date: 2026-10-29 12:50 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-CALLEE-959620-01`
Parent: `AUD-02`

## Claim and bounds

The starting tree is `main` at `5c50cfc28dfcf6bcbddcb293db0b6e1e7de84a31`, matching `origin/main` and `FETCH_HEAD`. The pinned input `local/re/eboot.elf` has SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. E993 verifies a caller-side direct call at `0x94ece8` to `0x959620`; this task starts at that exact target entry.

Inspect only the entry block at `0x959620`, decoding one instruction at a time until its first control transfer, decode gap, or PT_LOAD mapping gap. Stop at whichever boundary occurs first, with a hard cap of 32 instructions / 128 bytes. Map every selected instruction through file-backed ELF64 `PT_LOAD` bytes and independently recompute direct edges. Do not inspect any transfer target or later block. Use the existing bounded Rizin 0.9.1, Apple LLVM 21.0.0, and raw PT_LOAD verifier workflow; this is a byte-slice check, not function recovery.

## Limits and checkpoint state

The E993 caller edge is static and does not prove runtime reachability. This target-block task cannot establish a function identity, ABI/API meaning, NGS2 behavior, audio output, runtime execution, or game behavior. Parent `AUD-02` remains active; no product gate advances.

No analysis has run. Task status is `ACTIVE`, with 0 analysis attempts and 0 failed semantic hypotheses. Claim changes: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Private claim-publication logs: `local/validation/AUD-02-NGS2-RACKQUERY-CALLEE-959620-01/claim-publication/attempt-01/`; analysis outputs are preseeded under `local/validation/AUD-02-NGS2-RACKQUERY-CALLEE-959620-01/analysis/attempt-01/`.

Claim-publication commands and results: `python3 scripts/project_index.py` exited 0 (`43,188 folders indexed; written`); `python3 scripts/project_index.py --check` exited 0 (`43,188 folders indexed; current`); `trail build --check` exited 0 (wiring graph in sync, 10,787 deep-layer nodes pending); `python3 -I scripts/audit_publish_paths.py` exited 0 (6 staged paths); `python3 -I scripts/audit_publish_paths.py --all` exited 0 (3,390 paths). The initial `git diff --cached --check` exited 2 because the checkpoint metadata lines had trailing spaces; those spaces were removed and the rerun exited 0. `git fetch origin main` exited 0 and `git rev-parse HEAD origin/main FETCH_HEAD` showed all three refs at `5c50cfc28dfcf6bcbddcb293db0b6e1e7de84a31` before publication. No configure/build, CTest, or lift-checker rerun was needed because this claim changed documentation only; E1001 records the current source verification. Exact command output is in the private claim-publication log directory.

Next exact action after claim publication: run the bounded Rizin and Apple LLVM disassembly plus the raw ELF64 PT_LOAD verifier at `0x959620`; stop at the first transfer or decode/mapping gap, without inspecting a target. Record exact commands, exit results, source hash, tool versions, instruction/byte coverage, and unresolved boundary in the private analysis directory.
