# E1010 — Claim bounded SXDF ELF code-to-data reference check

**Date:** 2026-10-29 14:21 CDT
**Agent:** `/root`
**Task:** `AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01`
**Status:** `ACTIVE`

## Starting state

The starting tree is clean `main` at `328e02f72b3c26a84631616d02c1e2ac23f5e9b6`. `git fetch origin main` exited 0; `HEAD`, `origin/main`, `FETCH_HEAD`, and live `refs/heads/main` matched.

E1007 identifies a string-like candidate at RVA `0x12ba74a` and records a bounded LEA at `0x9b5b10` that computes this address, followed by a length value of 4. E1009 verifies that the exact `sound/shadow/music.sxd1` member begins with `SXDF`, matching the four-byte token at offset 10 of the static ELF context. The full 48-byte sequences differ. These observations do not establish a reader, codec, audio role, event route, or playback.

The current read-only REA binary-session preflight reports an open Ghidra 12.1.4 provider targeting `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf`, ELF x86-64, with SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` and profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. The provider's image base is not yet confirmed. The prior broad `list_documents` preflight stalled and was cancelled; this claim does not repeat it.

## Bounded scope

After confirming the open provider's target path, exact ELF hash, and image base, make only these two REA queries:

1. Inspect instruction `0x9b5b10`, bounded to the code/data reference and adjacent length value.
2. Inspect data at `0x12ba74a`, bounded to its block and relevant leading bytes.

Record the returned Evidence ID, provider/program identity, image base, tool version, coverage, exact instruction/reference, and exact returned bytes. If the target identity or image base cannot be confirmed, or a provider query fails or leaves the edge unresolved, stop and record the result without retry or local fallback.

No function-wide decompilation, broad xref sweep, binary reimport, asset read, source change, parser/codec/audio-semantic inference, route, runtime, playback, or game-gate claim is in scope. No product gate advances from this task.

## Claim publication checks

The task row and claim were added to `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `HANDOFF.md`, and this checkpoint. Generated `docs/FOLDERS.md` now indexes 44,216 folders and passes `python3 scripts/project_index.py --check`. CMake configure/build exited 0; CTest passed 487/487; lift checkers passed 572, skipped 0, failed 0; `trail build --check` reported the wiring graph in sync with 10,787 meaning-tier nodes pending; the staged-path audit passed 6/6 and the all-path audit passed 3,398 paths; attribution and final `git diff --cached --check` passed. An initial staged diff check found four trailing-space lines in this checkpoint; they were removed before the passing rerun. After the initial commit, the staged-only audit reported no staged paths (exit 2, expected with an empty index); the all-path audit was the applicable committed-tree check. A post-commit project-index check reported `local/indexes/folders.json` stale after private closeout logs were created; regeneration followed by `python3 scripts/project_index.py --check` passed at 44,216/current. These corrections are included before the claim is pushed. Private claim logs are under `local/validation/AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01/claim-publication/attempt-01/`.

**Attempt count:** 0 analysis attempts; 0 failed semantic hypotheses.
**Next exact action:** finish claim publication and fetch verification, then re-check the live provider identity/image base before the two bounded queries.
