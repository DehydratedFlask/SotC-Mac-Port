# AUD-02 VoiceGetStateFlags function root replay

- Time: 2026-10-20 04:19 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-02-NGS2-VOICE-GETSTATEFLAGS-01`.
- Status: `STATIC_PATH_BYTE_VERIFIED`; one provider query succeeded; two root verifier corrections; zero failed analysis hypotheses and zero failed provider operations.
- Prior state: this checkpoint supersedes the root-replay-pending state in [the query capture checkpoint](2026-10-20-root-aud02-voice-getstateflags-query-review-pending.md). The failed verifier attempts and second mapping correction are preserved in [the correction checkpoint](2026-10-20-root-aud02-voice-getstateflags-replay-correction.md).

## Input identity and provider operation

- Exact source: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; ELF64 little-endian x86-64; entry VMA `0xf120`; first executable `PT_LOAD` starts at VMA 0. Import manifest SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- One serialized REA CLI `function` query at candidate `0x9534cb` exited 0 and returned Evidence ID `ev_3f98dc6164521a74e7e9c2ff6f3a389c3945923ba312b1287343c59ba2298b99`. REA package was 6.7.0 and provider-reported Ghidra version 12.1.4. Query duration was 366.4698545 s; exact nested argv/environment, resource samples, and private snapshot are in `local/re/evidence/aud02-ngs2-voice-getstateflags-01/`.
- Worker dossier integrity: `shasum -a 256 -c SHA256SUMS` from the worker dossier directory exited 0 with all 56 entries accepted. The shared REA snapshot and pinned source hashes remained unchanged; no second provider call was issued.

## Root replay

- Root parser command: `python3 -I scripts/ps4_dynlink.py local/re/eboot.elf --out local/re/evidence/aud02-ngs2-voice-getstateflags-01-root-review/imports.with-thunks.json --thunk 0xf91480` — exit 0. It maps the thunk to slot `0x1601030`, symbol 245, NID `rEh728kXk3w`, relocation `R_X86_64_JUMP_SLOT`, addend 0.
- Root byte command: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/aud02-ngs2-voice-getstateflags-01-root-review/verify.py > local/re/evidence/aud02-ngs2-voice-getstateflags-01-root-review/verification.json 2> logs/aud02-voice-getstateflags-root-verifier-final.stderr.log` — exit 0; report status `ROOT_REPLAY_PASS_STATIC_PATH_BYTE_VERIFIED`; stderr empty.
- Root verifier records exact hashes for its script, independent parser output and verification report in `local/re/evidence/aud02-ngs2-voice-getstateflags-01-root-review/SHA256SUMS`; `shasum -a 256 -c SHA256SUMS` from that directory exited 0 (3/3).
- The final root result is SHA-256 `ab730ea5f70fad4041bd9fbc3d797da4d8970e0c020c2ff95eb21c83e99cb6df`. It independently confirms:
  - REA body `FUN_00953470`, exact range `[0x953470, 0x9534ed)` (126 bytes), contains the call edge at `0x9534cb`.
  - Register setup at `0x9534c3`, bytes `488b7de8`, decoded by Ghidra as `MOV RDI, qword ptr [RBP + -0x18]`; and `0x9534c7`, bytes `488d75e4`, `LEA RSI, [RBP + -0x1c]`. These are recorded as register-state observations; no ABI or parameter semantics are assigned.
  - Direct call at VMA `0x9534cb`, file offset `0x9574cb`, bytes `e8b0df6300`, computes to thunk `0xf91480`.
  - Thunk at VMA `0xf91480`, file offset `0xf95480`, bytes `ff25aafb6600`, computes to import slot `0x1601030`; slot file offset is `0x1605030` in the file-backed `PT_SCE_RELRO` program header (type `0x61000010`). The failed `PT_LOAD`-only checker assumption and correction are recorded in the historical correction checkpoint.
  - Manifest and parser agree the slot is symbol 245 / `sceNgs2VoiceGetStateFlags` / NID `rEh728kXk3w` / `R_X86_64_JUMP_SLOT`.

## Coverage and limits

- The Ghidra function dossier reports 0 unresolved calls within the selected function; its stated limitation says unresolved computed/indirect flows without target addresses are not represented as reference edges. This is local function-dossier coverage only, not whole-image coverage.
- The verified path is static. It does not prove the function is reachable, that the runtime slot resolves to a particular implementation, any ABI/parameter meaning, NGS2 behavior, game use, or audio playback. G1 remains NOT PASSED and G2 NOT RUN.
- No source, reference checkout or original PKG was modified/read for this task. Root replay artifacts are private under `local/re/evidence/aud02-ngs2-voice-getstateflags-01-root-review/`.
- `python3 scripts/project_index.py` exited 0; 4,156 folders indexed and `docs/checkpoints` reports 64 files; log `logs/root-voice-getstateflags-complete-project-index.log`.
- `graft check` exited 0 with the wiring graph in sync; the deep meaning layer remains unbuilt with 1,225 nodes pending; log `logs/root-voice-getstateflags-complete-graft-check.log`.
- Staged/full path audits, diff checks, remote divergence, commit and push are pending for this checkpoint; `tools/graft/` remains unstaged.
