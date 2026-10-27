# Startup slot 312 characterization claim

Local date/time: 2026-10-26 23:44 CDT (America/Chicago). Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01`.

## Bounded objective

Read the exact index-312 RVA from the pinned original constructor table, then run the existing
read-only slot analyzer against the pinned local ELF. Record the body extent/hash, reachable
instruction and branch inventory, direct-call sites/targets, RIP-relative data references, and
indirect-call sites. Recheck any prior census size against the live ELF; it is only an estimate.

This parallel phase records analyzer output only. Do not infer call semantics, CFG completeness,
or native-lift feasibility from the analyzer summary. REA/Ghidra correlation is deferred after
three slot-309 startup timeouts; no shared source/docs edits are authorized by the worker.
Keep artifacts under `local/validation/orch-20261026/slot312/`. Do not run the original executable,
or use any emulator, guest interpreter/JIT, or Sony runtime.

## Status and next action

Status is **ACTIVE / preliminary static analysis**; failed hypotheses: **0**. Next exact action:
`/root/history_audit` uses Graft/Trail before source inspection, verifies the table row and ELF,
runs the existing analyzer, and returns exact commands, private artifact path, hashes, and
limitations. The root will review the analyzer report and keep G1 partial until independent
function evidence is available.
