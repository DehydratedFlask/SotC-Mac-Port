# E996 — SAV-01 load-dispatch target-origin claim

Date: 2026-10-29 12:03 CDT
Agent: `/root`
Task: `SAV-01-LOAD-DISPATCH-5435FC-01`
Parent: `SAV-01`

## Claim and bounds

Starting tree: `main` at `257601e257d595f4046e2727582dde4088725659`, matching `origin/main`. Pinned input `local/re/eboot.elf` SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0.

Existing E866 REA/Ghidra evidence for `FUN_00542880` covers 4,886 bytes across three non-contiguous ranges and 949 assembly rows, with partial relationship coverage, unresolved indirect calls at `0x5435fc` and `0x543611`, and no resolved callers. This child traces only the reaching definition of the target for the call at `0x5435fc`, checking required local register/stack flow and any uniquely referenced vtable or relocation entry. It stops once one target entry is supported, at the first unresolved provenance/decode/mapping gap, or at 32 instructions / 128 bytes. It does not inspect the target body or the adjacent call at `0x543611`.

## Limits and task-register correction

This is a static target-origin task only. It does not establish a caller or startup path, SaveData API meaning, runtime behavior, malformed-save behavior, caller rollback, object-level atomicity, or real save/load. No user save file was read. SAV-04 remains `NOT RUN`.

The `SAV-01` summary in `docs/TASKS.md` was corrected to reflect that profile/progress chunk writers have partial byte-verified coverage in `SAV-01-PROFILE-CHUNKS-01`; the tag-4/tag-8 distinction remains unresolved, as does the load-side deserializer. No implementation gate advanced.

No analysis has been run. Task status is `ACTIVE`, with 0 analysis attempts and 0 failed semantic hypotheses. Claim changes: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Logs are under `local/validation/SAV-01-LOAD-DISPATCH-5435FC-01/claim-publication/attempt-01/`.

Next exact action after claim publication: use REA/Ghidra on the pinned ELF to trace only the target origin for `0x5435fc`, compare selected bytes/references with bounded LLVM and raw ELF/PT_LOAD mapping, then stop at the target entry.
