# E989 — RackQuery direct-call target first-block result

Date: 2026-10-29 11:27 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-CALLEE-99D8E0-01`
Parent: `AUD-02`

## Scope and result

This result inspects the first basic block at E985's static direct-call target `0x99d8e0`, one instruction at a time, and stops at the first control transfer.

Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. Rizin 0.9.1 and Apple LLVM 21.0.0 agree on exactly two instructions: `mov eax, 0x8190` at `0x99d8e0`, size 5, followed by `ret` at `0x99d8e5`, size 1. The `ret` is the first control transfer and the stop boundary.

A raw ELF64 verifier confirmed exact bytes and file-backed `PT_LOAD` phdr 0 mappings: `b8 90 81 00 00` at virtual `0x99d8e0` -> file offset `0x9a18e0`, and `c3` at virtual `0x99d8e5` -> file offset `0x9a18e5`. The verifier also checked the pinned hash and that both disassemblers emitted only the selected instruction at each address.

Status: `STATIC_PATH_BYTE_VERIFIED` for this one two-instruction block. The 32-instruction / 128-byte hard cap was not reached; the block ended after the second instruction.

## Evidence and limits

Primary captures, exact command exits, tool versions, verifier and SHA-256 manifest are private under `local/validation/AUD-02-NGS2-RACKQUERY-CALLEE-99D8E0-01/analysis/attempt-01/`. The manifest passed `shasum -a 256 -c SHA256SUMS`.

The `ret` destination is stack-sourced and was not resolved. No later instruction, target contents, runtime route selection, caller/startup reachability, function identity, ABI/API meaning, NGS2 behavior, actual audio, or game behavior was inspected or established. The E985 call is a static direct edge only. Parent `AUD-02` remains active; no product gate advanced. No source, binary, reference, or original game-input files changed.

## Checkpoint state

Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Required configure/build exited 0; CTest passed 487/487; lift checkers passed 572/0/0; Trail reported its graph in sync with 10,779 deep-layer nodes pending; project index is 43,154/current. Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-CALLEE-99D8E0-01/result-publication/attempt-01/`. Analysis attempt count: 1 supported static byte-verified block; 0 failed semantic hypotheses. Publication audits remain to be run before commit/push.
