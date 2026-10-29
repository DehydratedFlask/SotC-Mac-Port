# Slot-23 caller reachability checkpoint (E981)

Date: 2026-10-29 10:48 CDT
Agent: `/root`
Task: `CPU-01-SLOT23-CALLER-REACHABILITY-01`
Parent: `CPU-01-SLOT23-CONSUMERS-01`

## Result

`STATIC_PATH_BYTE_VERIFIED` for one conditional static path from the computed jump at `0xe5c365` to the call instruction at `0xe5c3f5`. This is not a complete recovery of the enclosing function and does not complete the parent task.

The pinned input `local/re/eboot.elf` has SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. At `0xe5c0a5`, `lea r12,[rip+0x4bda5c]` computes `0x1319b08`. This address maps through PT_LOAD phdr 0 (`p_offset=0x4000`, `p_vaddr=0`, `p_filesz=0x1593978`) to file offset `0x131db08`. The 13 signed relative table entries decode to:

```text
index  target
0      0xe5c3a7
1      0xe5c37b
2      0xe5c3b8
3      0xe5c37b
4      0xe5c38c
5-8    0xe5c367
9      0xe5c3cd
10     0xe5c3d3
11     0xe5c39a
12     0xe5c3de
```

The first setup reaches the dispatch conditionally: the common path reaches `0xe5c09d`, sets R12 at `0xe5c0a5`, and reaches `0xe5c330` from the nonzero edge of the compare at `0xe5c30b`. There are no explicit R12 writes from `0xe5c0a5` through the first dispatch at `0xe5c365`. This dataflow relies on ordinary x86-64 callee-saved R12 behavior and normal returns across intervening calls; those callees were not inspected.

`0xe5c336` sets EBX to 0. Thus entry 0 targets `0xe5c3a7`. If the loaded `R13d` at `0xe5c350` equals `0x2b`, the nonzero check at `0xe5c3aa` falls through, the equality branch at `0xe5c3b4` reaches `0xe5c3f0`, and the call at `0xe5c3f5` executes in this static path. The call's direct target is `0xe70a60`; it was not inspected.

The alternate path at `0xe5c425` loads a vtable-derived target into R12 at `0xe5c43a`; after that call, `0xe5c474` reloads `0x1319b08` before the loop backedge at `0xe5c492`. This later reset belongs to that alternate path and later iterations; it does not supersede the earlier `0xe5c0a5` definition for the first dispatch.

## Independent replay

An independent reviewer verified the pinned ELF hash, mapped the table through PT_LOAD phdr 0, recomputed all 13 signed entries, and checked the pre-dispatch `R12` definition plus the conditional path through `0xe5c3f5`. The primary verifier parses the ELF header/program headers and checks the table and selected instruction bytes, branch displacements, call target, and expected file hash. Its replay and all 35 manifest entries passed.

Private evidence and outputs:

- `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/attempt-01/result.md`
- `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/attempt-01/independent-review.md`
- `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/attempt-01/verify_dispatch_path.py` and `verify_dispatch_path.out`
- Bounded LLVM/Rizin disassemblies and stderr captures in the same attempt directory
- `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/attempt-01/SHA256SUMS`

Primary tool versions: Apple LLVM 21.0.0, Rizin 0.9.1. LLVM and Rizin disassembly commands exited 0; both emitted expected warnings because the stripped ELF has no section headers/section overlap for the selected virtual-address ranges. The table bytes were independently mapped through PT_LOAD phdr 0. The byte/table verifier exited 0. The manifest check `shasum -a 256 -c .../SHA256SUMS` passed 35/35.

## Limits and status

This is a conditional static path only. No evidence shows the runtime field equals `0x2b`, that callers/startup reach this block, or that the path runs in a game session. Receiver identity, target semantics, original-string necessity, and complete function Evidence remain unresolved. Slot 23 remains unregistered; parent `CPU-01-SLOT23-CONSUMERS-01` stays PARTIAL; G1 remains PARTIAL and G2–G6 NOT RUN.

One shell wrapper failed before launching either disassembler because zsh reserves `status`; the wrapper variable was changed to `rc` and captures succeeded. This was not a failed semantic hypothesis. Analysis count: one supported path replay, zero failed semantic hypotheses.

## Repository gates

The following commands passed before publication; logs are under `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/result-publication/attempt-01/`:

```sh
cmake -S native -B local/build/orch-r22                  # exit 0
cmake --build local/build/orch-r22 -j 12                # exit 0
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure  # 487/487
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh  # 572 passed, 0 skipped, 0 failed
trail build --check                                      # graph check OK; 10,779 deep-layer nodes pending
python3 scripts/project_index.py                       # wrote 43,136 folders
python3 scripts/project_index.py --check               # 43,136 folders current
python3 -I scripts/audit_publish_paths.py --all        # 3,374 paths accepted
```

Before commit, the staged path audit passed 6/6, the all-path audit passed 3,375/3,375, and `git diff --cached --check` passed. A precommit fetch verified `HEAD == origin/main == FETCH_HEAD == 0fed710f28aa763f325353922dd07c20186e4dff`; configured commit author is Johnny Wang. Attribution, commit, push, and post-push fetch verification are pending.

## Publication closeout (E982)

Published in owner-authored commit `2193f712ee31af3a02a8ab2c15b816892d32b64a` (`docs: verify slot-23 dispatch reachability`), changing six allowlisted documentation paths. Push and fetch verified `HEAD == origin/main == FETCH_HEAD`; author/committer are Johnny Wang. Precommit configure/build, CTest 487/487, lift checkers 572/0/0, Trail graph check, project index 43,136/current, staged audit 6/6, all-path audit 3,375/3,375, staged diff check, and attribution passed. Postpush all-path audit, attribution, `git show --format= --check HEAD`, project-index check, and clean-worktree verification passed. Logs are under `local/validation/CPU-01-SLOT23-CALLER-REACHABILITY-01/result-publication/attempt-01/`.

The publication advances only the static path documentation. The parent slot-23 task remains PARTIAL; slot 23 is unregistered; G1 remains PARTIAL and G2–G6 NOT RUN. Full game boot/audio/save-load/Metal acceptance is incomplete.

Next exact action: inspect the task register for another bounded evidence task and preserve E911's slot-309 stop.
