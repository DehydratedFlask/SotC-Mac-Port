# Claim: RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01

- Local time: 2026-10-21 10:37 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: FIXTURE_VERIFIED; zero failed implementation tries. The planned pre-guard failing regression is test-first evidence, not an unsuccessful implementation try.
- Bounded scope: make `scripts/unwrap_self.py` reject before any output write when the derived `destination.with_suffix(".identity.json")` path refers to the same file as the SELF input. Add one synthetic regression using the existing fixture helpers in `scripts/test_project_tools.py`; assert rejection, unchanged input bytes, and no ELF output. Worker owns only those two files. Root owns task/handoff/evidence records.
- Prerequisites: `RE-01` is STATIC_ANALYSIS_VERIFIED and `RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail ask "Does unwrap_self reject an identity sidecar path that aliases its input before writing?" --source --in scripts/` located `unwrap` at `scripts/unwrap_self.py:10-28`. The live wrapper reads/parses the source, writes the ELF at lines 16-17, then writes the sidecar at line 27 without a source/sidecar alias check. The existing check at lines 14-15 protects only a different existing ELF destination.
- Graft command `trail callers unwrap --depth all --in scripts/` found three existing synthetic tests and the CLI call. Existing tests cover roundtrip, truncated program-header tables, and refusing to replace a different ELF artifact; no test covers the identity sidecar aliasing the input.
- The test can use the existing synthetic SELF fixture in `scripts/test_project_tools.py`, choose source path `input.identity.json` and destination `input.elf`, and assert the original SELF bytes remain unchanged. `docs/EXTRACTION.md:54-57,80-100` confirms the wrapper is plaintext reconstruction tooling and that the original inputs are preserved; this task does not modify or read the original game input.
- Checkout at claim: `main`, HEAD `6811f5272068ed1f54eda5d29936a29808472feb`, matching `origin/main`. `git status --short` shows only the pre-existing escalated changes under `native/api/CMakeLists.txt` and `native/api/host/`; these remain excluded.
- Baseline hashes: `scripts/unwrap_self.py` `d1da0676720ffdcff8dbb10765e70b2eae1309735fb4bcb822d3977209d234a4`; `scripts/test_project_tools.py` `9b385a4578100833be17a70060839aec84487028a35dc04fe0e62f46dc6126e6`.

## Acceptance and boundaries

- Expected pre-guard check: add the regression and run `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`; the assertion must fail because the current wrapper overwrites the source with sidecar JSON. Save it as `local/validation/re01-unwrap-input-sidecar-collision/attempt-1.log`.
- Acceptance command after the narrow guard: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Exact commands/results: the test-first regression failed before the guard as expected, showing `source_overwritten=True` and `elf_output_created=True`; log `local/validation/re01-unwrap-input-sidecar-collision/attempt-1.log`, SHA-256 `893f32e3c6289aad3ef5a0ff342322bbf6126db0d66b6b40e3b483c4eddcfcad`. Worker acceptance command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` exited 0 with 6/6; log `attempt-2.log`, SHA-256 `c7898f18dd249b374a1833944d38eeddf70146cd940bf3f3e855c78d64db67d1`. Root independently replayed the same command, exit 0 with 6/6; `root-replay.log` has the same SHA-256. The final guard rejects the alias before reads or writes, and the fixture verifies unchanged source bytes and no ELF output.
- Claim-index command `python3 scripts/project_index.py` exited 0 and reported 14,209 folders indexed; `docs/FOLDERS.md` now includes this claim checkpoint. Log: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/project-index.log`.
- Claim graph command `trail build --check` exited 0 with the wiring graph in sync; deep layer remains unbuilt. Log: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/trail-check.log`.
- Final source hashes: `scripts/unwrap_self.py` `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`; `scripts/test_project_tools.py` `e0edd276090e436377d2bf74dc6b852ca5aa742b0172906f8fa069a72058b51a`.
- After the worker edit, `trail build` exited 0 and `trail build --check` exited 0 (wiring graph in sync; deep layer remains unbuilt). Logs are under `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/`.
- Files changed: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, this claim checkpoint, `scripts/unwrap_self.py`, and `scripts/test_project_tools.py`. E132 and a separate completion checkpoint now record the verified result.
- Claim publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for 4 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,503 allowed tracked paths; `git diff --cached --check` exited 0. Logs: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/{audit-staged.log,audit-all.log,cached-check.log}`. Escalated host files remain unstaged.
- Observation: the sidecar writer can target the input path. Inference: a same-file check before creating/writing outputs prevents this exact destructive collision.
- Remaining limits: synthetic plaintext SELF fixture only; no original game input, executable, firmware, or game runtime is read or used. No G1/G2/game gate claim follows.
- Claim publication checkpoint:
  - Local time: 2026-10-21 10:39 CDT (America/Chicago). Integrator: `/root`; task `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`; ACTIVE, zero failed implementation tries.
  - Exact command: `git push origin main`; exit 0, advanced `6811f52..90a5d99`. `git rev-parse HEAD` and `git rev-parse origin/main` both returned `90a5d99413dfe53236828f3f963121ac7045186c`. Push output was not saved separately.
  - Observation: the task claim and acceptance boundary are published; implementation is now independently replayed and FIXTURE_VERIFIED.
  - Next exact action: publish E132, the updated task row and completion checkpoint. Then continue the separately claimed audit filename fixture.
