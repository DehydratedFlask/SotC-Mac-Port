# E987 — RackQuery direct-call target first-block claim

Date: 2026-10-29 11:19 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-CALLEE-99D8E0-01`
Parent: `AUD-02`

## Claim and scope

Claimed a bounded child from E985's static direct call at `0x94ecd6 -> 0x99d8e0`. Starting from the direct target `0x99d8e0`, inspect one instruction at a time through the first control transfer, capped at 32 instructions or 128 bytes, whichever comes first. Map selected instruction bytes through the pinned ELF's file-backed `PT_LOAD`; recompute a target only if the transfer is direct. A decode or mapping gap remains unresolved.

Do not inspect any transfer target or later block. Do not infer a function identity, ABI/API meaning, NGS2 semantics, runtime reachability, or actual audio from this static edge. E985 does not prove that this call executes at runtime.

Pinned input: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. At claim time, clean `main` was `824f3f49749ad7ed469b62be99acb22f7fb242d6`, matching fetched `origin/main` and `FETCH_HEAD`. The active task board had no READY row; the exact task ID was absent. E911's slot-309 stop is unchanged.

## Verification plan

Use bounded Rizin / LLVM one-instruction output and a raw ELF64 parser. Initial commands:

```sh
rizin -q -e scr.color=false -c 'aoj @ 0x99d8e0' local/re/eboot.elf
xcrun llvm-objdump -D --start-address=0x99d8e0 --stop-address=0x99d8e1 local/re/eboot.elf
```

If the first instruction is not a control transfer, advance one instruction at a time, capturing and verifying each instruction, then stop at the first control transfer or the 32-instruction / 128-byte cap. Do not request a full function listing. No tool installation or Ghidra project is needed.

## Claim checkpoint

No analysis has run. Status `ACTIVE`; 0 attempts, 0 failed semantic hypotheses. Parent `AUD-02` remains active; no product gate advanced. Claim paths: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Private claim-publication logs are under `local/validation/AUD-02-NGS2-RACKQUERY-CALLEE-99D8E0-01/claim-publication/attempt-01/`. Next exact action after claim publication: execute the bounded first-instruction captures, then independently verify the selected PT_LOAD bytes and direct edge(s).
