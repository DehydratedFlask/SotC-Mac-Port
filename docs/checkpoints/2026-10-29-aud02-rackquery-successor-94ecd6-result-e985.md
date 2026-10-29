# E985 — RackQuery successor first-transfer result

Date: 2026-10-29 11:09 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01`
Parent: `AUD-02-NGS2-RACKQUERY-STACK-POSTCALL-94EBC2-01` / `AUD-02`

## Scope and result

This result inspects only the first instruction at the direct successor address `0x94ecd6` from E973's branch at `0x94ebf1`, then stops at that instruction's control transfer.

Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. Rizin, bounded LLVM, and the primary raw ELF verifier agree on the five bytes `e8 05 ec 04 00`, decoded as a direct `CALL rel32` at `0x94ecd6`, size 5, target `0x99d8e0`. The bytes map through file-backed `PT_LOAD` phdr 0 to file offset `0x952cd6`. The signed displacement is `+0x4ec05`; `0x94ecdb + 0x4ec05 = 0x99d8e0`.

Independent reviewer `/root/audio_save_path` separately parsed the ELF, verified the file mapping and exact five bytes, and recomputed the same instruction and target. The bounded Apple LLVM `objdump` command returned 0 with a section-range warning while emitting exactly one instruction. No discrepancy was reported.

Status: `STATIC_PATH_BYTE_VERIFIED` for one static instruction/edge. This is not a runtime reachability or game-behavior claim.

## Evidence and limits

Primary analysis artifacts and the SHA-256 manifest are private under `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/analysis/attempt-01/`. The manifest was verified with `shasum -a 256 -c SHA256SUMS`.

The call at `0x94ecd6` is the stop boundary. The target `0x99d8e0`, later instructions, alternate routes, runtime route selection, caller/startup reachability, API/NGS2 semantics, audio, and game behavior were not inspected or established. Parent `AUD-02` remains active; no product gate advanced. The saved Ghidra `.gpr` was not opened. No source, executable, reference, or original game-input files changed.

Commands and private captures are listed in the attempt `result.md`; verification output is `verify_first_transfer.out`, and the independent replay is recorded in `independent-review.md`.

## Checkpoint state

Files changed for this result: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Required configure/build exited 0, CTest passed 487/487, lift checkers passed 572/0/0, Trail reported the graph in sync with 10,779 deep-layer nodes pending, and project index is 43,145/current. Final staged audit passed 6/6 and all-path audit passed 3,377/3,377; the first staged diff check found trailing spaces on the metadata lines above, which were removed, and the final diff check passed. Gate logs are under `local/validation/AUD-02-NGS2-RACKQUERY-SUCCESSOR-94ECD6-01/result-publication/attempt-01/`. Analysis attempt count: 1 supported static instruction/edge result; 0 failed semantic hypotheses. The next action is commit/push and publication closeout.
