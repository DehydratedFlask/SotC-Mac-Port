# AUD-02 RackQuery successor block claim (E983)

Date: 2026-10-29 11:00 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01`

## Claim

E973 verified a conditional branch from `0x94ebf1` to `0x94ecd6` and explicitly left successor code unexamined. This task will inspect only the basic block at `0x94ecd6`, map each instruction byte to the pinned ELF, and stop at its first control transfer. It will record that transfer's target but will not inspect its successor or infer behavior beyond the boundary.

Pinned input: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. E973 is the predecessor evidence and the Stage 2 per-function protocol in `docs/REVERSE_ENGINEERING.md` is the analysis policy.

The task board has no `READY` rows. The successor is a concrete open edge from E973 and is independent of the slot-309 stop. The task ID was absent from `docs/TASKS.md` and `docs/EVIDENCE.md` before this claim.

The current saved Ghidra project file `local/ghidra/persistent-20261020/SotC.gpr` is 0 bytes. This bounded block check will not use or reopen it; it will use raw LLVM/Rizin disassembly and a separate ELF64 `PT_LOAD` byte/edge verifier. No tool installation or source/code change is required.

## Acceptance and limits

- Pin the ELF hash, image base, tool versions, command outputs, and capture hashes.
- Use a bounded initial capture from `0x94ecd6`; map all selected instruction bytes through file-backed `PT_LOAD` and recompute the first transfer target independently.
- Stop at the first transfer, even when it is direct. Preserve `UNKNOWN` for route selection, runtime reachability, API/audio meaning, and all successor/callee behavior.
- Do not inspect another route or another function as part of this child.

Status: `ACTIVE`; no analysis run yet; 0 attempts; 0 failed semantic hypotheses. Parent `AUD-02` remains active. Product acceptance is unchanged: G1/G4 PARTIAL and G2–G6 NOT RUN.

Private preflight: `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/claim/attempt-01/`. Claim-publication logs will be under `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/claim-publication/attempt-01/`.

## Claim verification

Before publication, configure/build exited 0; CTest passed 487/487; lift checkers passed 572, skipped 0, failed 0; `trail build --check` reports the wiring graph in sync with 10,779 deep-layer nodes pending; project index wrote and checked 43,141 folders. Exact logs are in `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/claim-publication/attempt-01/`.

Before commit, the staged audit passed 6/6; the all-path audit passed 3,376/3,376; and `git diff --cached --check` passed. A precommit fetch verified `HEAD == origin/main == FETCH_HEAD == ead03a8b5d8dceafe9699afb8c35f07eca421a99`; configured author is Johnny Wang. The first diff check found trailing Markdown whitespace in the metadata lines; it was corrected and the final check passed. No analysis hypothesis failed.

## Publication closeout (E984)

The claim was published in owner-authored commit `e9827d71a0258ce43be07517ab4309b391f875cd` (`docs: claim RackQuery successor block check`), changing six allowlisted documentation paths. Push/fetch verified `HEAD == origin/main == FETCH_HEAD`; author and committer are Johnny Wang. Claim gates: configure/build exit 0; CTest 487/487; lift checkers 572/0/0; Trail wiring graph in sync with 10,779 deep-layer nodes pending; project index 43,141/current; staged audit 6/6; all-path audit 3,376/3,376; staged diff and attribution passed. Postpush path audit, attribution, commit check, project-index check, and clean worktree passed. Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/claim-publication/attempt-01/`.

The task remains ACTIVE at 0 analysis attempts. This claim does not establish behavior, route selection, audio, runtime reachability, or any product gate.

Exact next commands:

```sh
xcrun llvm-objdump -D --start-address=0x94ecd6 --stop-address=0x94ed16 local/re/eboot.elf
rizin -q -e scr.color=false -c 'pD 64 @ 0x94ecd6' local/re/eboot.elf
```

Then compare only the decoded basic block through its first control transfer, create an independent raw ELF verifier, and request a bounded independent replay.
