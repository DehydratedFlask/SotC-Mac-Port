# E995 — Slot-23 direct-call target entry-block claim

Date: 2026-10-29 11:57 CDT
Agent: `/root`
Task: `CPU-01-SLOT23-DIRECT-CALLEE-0E70A60-01`
Parent: `CPU-01-SLOT23-CONSUMERS-01`

## Claim and bounds

The starting tree is `main` at `d9aa2a6d2a53c2e670ef87196d9f4b753a57d1fb`, matching `origin/main`. Pinned input `local/re/eboot.elf` has SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0.

E981 byte-verifies one conditional local path to call `0xe5c3f5` when `R13d == 0x2b`; that direct call targets `0xe70a60`. E981 did not inspect the target body. This task will inspect only the entry basic block at `0xe70a60`, one instruction at a time, to its first control transfer, with a hard cap of 32 instructions / 128 bytes. It will map selected bytes through file-backed PT_LOAD and recompute direct edges, then stop without inspecting any transfer target.

## Limits

The E981 path is conditional static flow only. Incoming caller/startup reachability and the value provenance for `R13d` remain unknown. This target-block task does not establish receiver type, method/API meaning, original-string necessity, runtime execution, or game behavior. Slot 23 remains unregistered; G1 remains PARTIAL. The E911 slot-309 stop remains unchanged.

No analysis has been run. Task status is `ACTIVE`, with 0 analysis attempts and 0 failed semantic hypotheses. Claim changes: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Claim build/check/audit logs are under `local/validation/CPU-01-SLOT23-DIRECT-CALLEE-0E70A60-01/claim-publication/attempt-01/`.

Next exact action after claim publication: open the pinned ELF with REA's Ghidra provider, capture only the target entry block, compare selected bytes with bounded LLVM output and a raw PT_LOAD verifier, and stop at the first control transfer.
