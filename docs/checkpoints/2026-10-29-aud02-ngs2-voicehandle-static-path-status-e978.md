# AUD-02 NGS2 VoiceHandle one-callsite status reconciliation — 2026-10-29

**Date:** 2026-10-29 10:07 CDT

**Agent:** `/root`

**Task ID:** `DOC-AUD02-VOICEHANDLE-STATIC-PATH-STATUS-01`

**Result:** `STATIC_PATH_BYTE_VERIFIED` for the existing single-callsite task; parent `AUD-02` remains `ACTIVE`.

## Scope and result

Reconciled the status of `AUD-02-NGS2-RACK-VOICEHANDLE-01` against its saved callsite evidence. The task scope is one static callsite path. No code, binary, reference, or game-input files changed, and no new Ghidra/REA query was made.

The callsite at VMA `0x9496a8` is the five-byte instruction `e8 63 7d 64 00` in executable `PT_LOAD` phdr 0 (file offset `0x94d6a8`). Its rel32 target is thunk `0xf91410`; thunk bytes `ff 25 e2 fb 66 00` select slot `0x1600ff8`, which maps to import symbol 238, `sceNgs2RackGetVoiceHandle`, NID `MwmHz8pAdAo`. The saved REA/Ghidra 12.1.4 function query reports the call as an `UNCONDITIONAL_CALL` edge in `FUN_009485a0`, a 5,585-byte body. The independent root verifier cross-checks these facts against the pinned ELF and import manifest.

## Replay commands and evidence

From `local/re/evidence/aud02-ngs2-rack-voicehandle-01/function-009496a8/`:

```sh
shasum -a 256 -c SHA256SUMS
python3 -I verify_callsite_function.py
```

Both exited 0. All 16 manifest entries passed. The verifier returned `PASS_STATIC_CALLSITE_FUNCTION_ATTRIBUTION_PARTIAL_TASK`, Evidence ID `ev_7cd1fd53a368738d773628f4c4df821b0612640a832abe1c31afbeca14f29fd0`, and pinned ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Replay logs are private under `local/validation/DOC-AUD02-VOICEHANDLE-STATIC-PATH-STATUS-01/claim-publication/attempt-01/`.

## Limits

This result does not establish caller reachability, runtime import-slot binding, argument meanings, ABI, the NGS2 contract, voice-handle semantics, or audio behavior. The verifier reports 104 raw rel32 candidates, with six inside this Ghidra function body; other-candidate attribution is tracked separately. The earlier 330-second xrefs timeout remains recorded and was not retried. No audio, boot, save, or game-acceptance gate advanced: G1 remains `NOT PASSED`; G2–G6 remain `NOT RUN`.

## Publication record

E977 claim commit `b61a9a4bf29a9921685d1204e16355d3c9c5502d` was pushed to `origin/main`. Claim-gate outputs are private under `local/validation/DOC-AUD02-VOICEHANDLE-STATIC-PATH-STATUS-01/claim-publication/attempt-01/`. The initial project-index check reported `Stale indexes: local/indexes/folders.json`; rerunning `python3 scripts/project_index.py` and its `--check` then yielded a current 43,127-folder index. The initial checker log reports 572 passed, 0 skipped, 0 failed; the first terminal poll lost its session handle after the checker log completed, so its exit was not captured in that invocation.

Result-publication gates: configure/build exited 0; CTest 487/487; checker exit 0 with 572 passed / 0 skipped / 0 failed; Trail wiring graph check passed with 10,779 deep-layer nodes pending; project index generator exited 0 at 43,130 folders. The precommit staged audit passed 6/6; the precommit all-path audit passed 3,373/3,373; staged diff passed. After commit, all-path audit passed 3,374/3,374, attribution passed on `origin/main..HEAD`, `git show --format= --check HEAD` passed, and owner identity was verified. The postcommit staged audit returned 2 because nothing remained staged; this does not supersede the precommit 6/6 result. Four `project_index.py --check` calls reported the private index stale after new private log paths appeared; after the final log paths existed, the required plain generator rewrote the private index successfully. E978 result commit `16e3bc8f979c6d08ded1e095f7be0be77256eff8` was pushed to `origin/main`; E979 fetched and verified `HEAD == origin/main == FETCH_HEAD`, clean worktree. Exact logs are under `local/validation/DOC-AUD02-VOICEHANDLE-STATIC-PATH-STATUS-01/result-publication/attempt-01/`.

**Observation:** the raw bytes, import mapping, saved Ghidra edge, and independent offline replay support this one static callsite path.

**Inference excluded:** no runtime route or game audio follows from this result.

**Remaining risks:** no native audio contract or game-driven playback is established; parent `AUD-02` is partial and overall port acceptance is incomplete.
**Next exact action:** inspect remaining open tasks for an independent bounded item; keep slot 309 stopped absent a new concrete producer/consumer/order edge or principal reassessment. No product task is READY.
