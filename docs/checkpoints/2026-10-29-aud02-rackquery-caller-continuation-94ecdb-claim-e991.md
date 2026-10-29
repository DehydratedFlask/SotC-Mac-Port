# E991 — RackQuery caller continuation claim

Date: 2026-10-29 11:33 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-CALLER-CONTINUATION-94ECDB-01`
Parent: `AUD-02`

## Claim and scope

Claimed a bounded caller-side continuation after E985's direct call at `0x94ecd6`. E985 byte-verifies that the call is 5 bytes long, so inspect the caller beginning at `0x94ecdb`, one instruction at a time, through the first control transfer. Cap the work at 32 instructions or 128 bytes, whichever comes first. Map selected instructions through the pinned ELF's file-backed `PT_LOAD`; recompute a target only for direct transfers. Any decode or mapping gap remains unresolved.

Do not inspect a transfer target, later block, or alternate route. Do not establish caller/startup reachability, API/NGS2 semantics, or actual audio. E985's call and E989's target block are static-byte findings only.

Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. At claim time, clean `main` was `9445b7edb1759c252e5d8d0d7fff56aa0040db36`, matching fetched `origin/main` and `FETCH_HEAD`. The exact task ID was absent from the live register.

## Verification plan

Use bounded one-instruction Rizin and Apple LLVM output plus a raw ELF64 program-header parser. Initial commands:

```sh
rizin -q -e scr.color=false -c 'aoj @ 0x94ecdb' local/re/eboot.elf
xcrun llvm-objdump -D --start-address=0x94ecdb --stop-address=0x94ecdc local/re/eboot.elf
```

If the first instruction is not a control transfer, advance one instruction at a time, save each exact capture, and stop at the first transfer or the 32-instruction / 128-byte cap. Do not request a larger function listing or inspect transfer targets.

## Claim checkpoint

No analysis has run. Status `ACTIVE`; 0 attempts and 0 failed semantic hypotheses. Parent `AUD-02` remains active; no product gate advanced. Claim paths: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Claim gates passed: configure/build exit 0, CTest 487/487, checkers 572/0/0, Trail check in sync with 10,779 deep-layer nodes pending, project index 43,163/current. Exact logs are under `local/validation/AUD-02-NGS2-RACKQUERY-CALLER-CONTINUATION-94ECDB-01/claim-publication/attempt-01/`. Next exact action after claim publication: capture the first caller instruction, then byte-map the selected instruction(s) and static edge(s).
