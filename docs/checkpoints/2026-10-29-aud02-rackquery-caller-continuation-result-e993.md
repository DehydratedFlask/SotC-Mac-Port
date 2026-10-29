# E993 — RackQuery caller continuation result

Date: 2026-10-29 11:39 CDT
Agent: `/root`
Task: `AUD-02-NGS2-RACKQUERY-CALLER-CONTINUATION-94ECDB-01`
Parent: `AUD-02`

## Scope and result

This result inspects caller-side instructions after E985's five-byte direct call at `0x94ecd6`. Starting at `0x94ecdb`, it decodes one instruction at a time and stops at the first control transfer.

Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. Rizin 0.9.1 and Apple LLVM 21.0.0 agree on four contiguous instructions / 18 bytes:

| RVA | Bytes | Decoding |
| --- | --- | --- |
| `0x94ecdb` | `48 8b 7c 24 10` | `mov rdi, qword [rsp+0x10]` |
| `0x94ece0` | `48 63 f0` | `movsxd rsi, eax` |
| `0x94ece3` | `ba 01 00 00 00` | `mov edx, 0x01` |
| `0x94ece8` | `e8 33 a9 00 00` | direct `call 0x959620` |

A raw ELF64 verifier mapped those spans through file-backed `PT_LOAD` phdr 0 to file offsets `0x952cdb`, `0x952ce0`, `0x952ce3`, and `0x952ce8`. For the last instruction, signed `rel32` is `+0xa933`; `0x94eced + 0xa933 = 0x959620`. The call at `0x94ece8` is the stop boundary. The verifier and each Rizin/LLVM command exited 0.

Status: `STATIC_PATH_BYTE_VERIFIED` for this one caller slice. The 32-instruction / 128-byte cap was not reached because the first transfer occurred after four instructions.

## Evidence and limits

The instructions load RDI from `[RSP+0x10]`, sign-extend EAX into RSI, set EDX to 1, and call `0x959620`. These are raw instruction effects; ABI argument roles are not established.

E989 separately verifies `mov eax, 0x8190; ret` at E985's direct-call target. If the E985 call executes and returns normally, EAX would carry the value into `movsxd rsi,eax`; this is a conditional static inference, not evidence of runtime execution. The route, stack-value provenance, return behavior, call target contents, API/NGS2 semantics, actual audio, and game behavior remain unknown. Parent `AUD-02` remains active; no product gate advanced. No later instruction or alternate route was inspected.

Exact captures, tool versions, verifier, and the SHA-256 manifest are private under `local/validation/AUD-02-NGS2-RACKQUERY-CALLER-CONTINUATION-94ECDB-01/analysis/attempt-01/`. The manifest passed `shasum -a 256 -c SHA256SUMS`. No source, binary, reference, or original game-input file changed.

## Checkpoint state

Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Configure/build exited 0; CTest passed 487/487; lift checkers passed 572/0/0; Trail reported its graph in sync with 10,779 deep-layer nodes pending; project index is 43,163/current. Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-CALLER-CONTINUATION-94ECDB-01/result-publication/attempt-01/`. Analysis attempt count: 1 supported static caller-slice result; 0 failed semantic hypotheses. Publication path audits and staged diff check remain to be run before commit/push.
