# E1006 — SXDF/SXDS raw ELF candidate scan claim

**Date:** 2026-10-29 13:34 CDT
**Agent:** `/root`
**Task:** `AUD-02-SXDF-RAW-ELF-CANDIDATE-SCAN-01`
**Status:** `ACTIVE`

## Bounded scope

Hash-pin `local/re/eboot.elf`; scan the complete file for raw `SXDF` and `SXDS` tags and classify every hit against the ELF program headers. For executable hits, disassemble only bounded neighborhoods using Rizin and an independent LLVM decoder. Record candidate references as preliminary static evidence only.

Do not query REA/Ghidra, read any PSARC/member payload, implement a parser or decoder, or claim that the candidate is an audio reader, codec, engine route, or game behavior. No product source changes are in scope.

## Starting evidence and limits

- The worktree was clean `main` at `eb30a143596680026898c29f41b487859e5d42eb`; `git fetch origin main` exited 0 and `HEAD == origin/main == FETCH_HEAD`.
- The pinned ELF identity is SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- The task is grounded in `docs/ASSETS.md`'s corrected `.sxd` family inventory and `docs/checkpoints/2026-10-27-aud01-sxdf-reader-census.md` (E791), which scanned existing saved REA/Ghidra text exports, not raw executable bytes.
- Exploratory preflight saw eight `SXDF` byte occurrences and zero `SXDS`; Rizin annotated code comparisons plus one data occurrence. This is only a task-selection lead. It has not established a reader or audio use.

## Claim publication

The claim modifies only allowlisted documentation: `HANDOFF.md`, `docs/TASKS.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No source, ELF, reference clone, or original asset was changed. `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12` exited 0; CTest passed **487/487**; lift checkers passed **572**, skipped **0**, failed **0**. `trail build --check` exited 0 with the wiring graph in sync (10,787 meaning-tier nodes pending); full path audit exited 0 over **3,394** paths; staged audit passed **5/5**; attribution audit exited 0; `git diff --cached --check` passed after one trailing-whitespace correction. The first project-index check found a stale private folder inventory after creating a log; regeneration and final check passed at **44,201** indexed folders (final logs: `/tmp/sotc-e1006-project-index-final.log`, `/tmp/sotc-e1006-project-index-final-check.log`). Task logs are under `local/validation/AUD-02-SXDF-RAW-ELF-CANDIDATE-SCAN-01/claim-publication/attempt-01/`. These checks verify the existing native code and documentation only; no game gate advances.

## Next action and acceptance

Run the formal full-file hash-pinned scan; classify all hits using independently parsed ELF64 program headers; cross-check bounded executable-hit decoding with Rizin and LLVM. Stop at the task's explicit bounds. If the scan finds only data or unresolved code candidates, report that result without promoting an audio or shipped-code claim. Attempt count: 0 formal scans; 0 failed semantic hypotheses.
