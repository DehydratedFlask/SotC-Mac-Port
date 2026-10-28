# E879 — Slot-23 indirect-site structure replay and task triage

**Date:** 2026-10-28, CDT
**Time:** 00:04 CDT
**Task:** `CPU-01-SLOT23-CONSUMERS-01` (existing root-owned partial task)
**Status:** `PARTIAL`; no constructor registration, native lift, or G1 promotion.

## Bounded result

The pinned ELF is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
Root's bounded Apple LLVM disassembly and an independent Rizin reviewer agree on the local
register flow in `0xe70af0`: `rdi` is retained as a receiver in `r12`, `rsi` is retained in
`r15` and null-checked, the vptr is loaded from `[r12]`, and the indirect call at
`0xe70b30` uses vtable offset `+0x38` with `r12`/`r15` as the first two argument registers.
The 32-bit result in `eax` is saved in `r14d`, moved to `esi`, and passed to the indirect call
at `0xe70b40` through vtable offset `+0xa8`.

This is instruction-level dataflow only. The containing caller at `0xe5c3f5` is still behind
an unresolved indirect jump, and the receiver type, target methods, function signatures,
runtime reachability, and whether the original strings are required remain unknown. No Ghidra
function Evidence was obtained, so this does not close the parent task or establish a product
contract. Keep slot 23 unregistered and G1 `PARTIAL`.

## Commands and evidence

- `shasum -a 256 local/re/eboot.elf` — matched the pinned SHA-256 above; private result:
  `local/validation/orch-20261028/e879/elf.sha256`.
- `xcrun llvm-objdump --disassemble --start-address=0xe70af0 --stop-address=0xe70b40 --show-raw-insn local/re/eboot.elf` — exit 1 because Apple LLVM 21.0.0 does not accept `--show-raw-insn`; private record:
  `local/validation/orch-20261028/e879/slot23-command-syntax-error.txt`.
- `xcrun llvm-objdump --disassemble --start-address=0xe70af0 --stop-address=0xe70b40 local/re/eboot.elf` — exit 0; emitted the requested bytes/instructions with a warning that the address interval does not overlap an ELF section. Private output SHA-256 `a82086286fd9511b4e6074c0f3196999730ce78eeb6db92eceba622c447a4038` at `local/validation/orch-20261028/e879/slot23-e70af0-objdump.txt`.
- `xcrun llvm-objdump --disassemble --start-address=0xe70b26 --stop-address=0xe70b60 local/re/eboot.elf` — exit 0; same section warning; includes both indirect calls and the `eax` to `esi` handoff. Private output SHA-256 `78e78b3b8eec9340628196c7764ba7849c8fe33ead66c0df8a37853dc0575884` at `local/validation/orch-20261028/e879/slot23-e70af0-tail-objdump.txt`.
- Independent reviewer command: `rizin -N -NN -q -a x86 -b 64 -c 'pD 58 @ 0xe70b26' local/re/eboot.elf` — reported exit 0 and matching register dataflow. Rizin warned about the reconstructed ELF's absent sections and malformed/odd program-header/dynamic entries. Reviewer report SHA-256 `2d58268230d6fa7b4c90239a3b718c74ce16bf79e9be8284255372735203fbbb` at `local/validation/orch-20261028/e879/slot23-rizin-independent-review.txt`; raw reviewer output was not retained.
- `REA open_binary` on the existing saved Ghidra project pointer with `provider_id=ghidra` — `target_unavailable`, `unsupported_target`, non-retryable: the `.gpr` file is zero bytes and was rejected as truncated. No Ghidra program was loaded or modified. Private result SHA-256 `f5df2468786e4f889965bd5cd31e6353eb145076f1d6f3654191f27ca186c0bc` at `local/validation/orch-20261028/e879/rea-gpr-open-error.json`.

The independent LLVM/Rizin replay is recorded as a bounded static-disassembly observation, not
as Ghidra or REA function analysis. The prior Ghidra startup timeout remains unresolved.

## Audio-scout scope deviation

After E828 explicitly stopped the CXML structure probe and prohibited retrying with a different
parser or DTD policy, the audio scout nevertheless invoked strict `xmllint --nonet --noout -`
on the in-memory, E824-identity-matched member. The child exited 1, the Python wrapper exited 0,
no element/attribute counts were produced, and no bytes or output files were saved. This was an
out-of-scope action; it adds no CXML evidence. Keep the CXML structure `UNKNOWN` and do not
repeat the parser probe. The scout's scope-deviation record is private at
`local/validation/orch-20261028/e879/audio-xmllint-scope-deviation.txt` (SHA-256
`8f2d8e61cfaf3bc9697656dbea23bd35628ce2c76df39f1790595de7e54149ee`).

## Triage and next action

CPU, audio, and save scouts found no existing `READY` product row. The save scout identified a
possible future one-field GameData-writer investigation, but no task was claimed or started in
this checkpoint. APP-01 VoiceOver remains `NEEDS_PRINCIPAL`; G4 remains `PARTIAL`. Boot, authentic
audio, and real save/reload remain unverified.

Next exact action: before tracing the two vtable targets, establish whether the candidate caller
block at `0xe5c3f5` is reachable from a concrete startup path. If that prerequisite stays unknown,
preserve the current partial result and do not register slot 23.

## Publication validation

Commands run from the workspace root:

- `cmake -S native -B local/build/orch-r22` — exit 0.
- `cmake --build local/build/orch-r22 -j 12` — exit 0, `ninja: no work to do`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed, 0 skipped, 0 failed.
- `python3 -I scripts/audit_publish_paths.py --all` — exit 0, 3,095/3,095.
- `python3 -I scripts/check_commit_attribution.py` — exit 0, `ATTRIBUTION AUDIT OK (origin/main..HEAD)` before the new commit existed.
- `python3 scripts/project_index.py` — exit 0, 32,025 folders indexed; `python3 scripts/project_index.py --check` — exit 0, current.
- `trail build --check` — exit 0, graph current at 9,877 nodes; deep meaning layer not built.
- `git diff --cached --check` — final exit 0. `python3 -I scripts/audit_publish_paths.py` — exit 0, 6/6 staged paths.

Logs are under `local/validation/orch-20261028/e879/`. A project-index check after a new private
validation note found `local/indexes/folders.json` stale again; both indexes were regenerated
and checked after all private outputs were written. The first staged diff check found two
Markdown hard-break spaces in this checkpoint; they were removed before the final clean check.
E880 records the new commit's exact attribution and live-ref verification.
