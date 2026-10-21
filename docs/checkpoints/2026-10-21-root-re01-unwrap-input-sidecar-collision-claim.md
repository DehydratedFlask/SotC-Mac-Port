# Claim: RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01

- Local time: 2026-10-21 10:37 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; zero failed implementation tries. The planned pre-guard failing regression is test-first evidence, not an unsuccessful implementation try.
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
- Exact commands/results so far: no task test has run; status NOT RUN. No code or test changes have been made. No task log exists yet.
- Claim-index command `python3 scripts/project_index.py` exited 0 and reported 14,209 folders indexed; `docs/FOLDERS.md` now includes this claim checkpoint. Log: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/project-index.log`.
- Claim graph command `trail build --check` exited 0 with the wiring graph in sync; deep layer remains unbuilt. Log: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/trail-check.log`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this claim checkpoint. No source/test changes.
- Claim publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for 4 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,503 allowed tracked paths; `git diff --cached --check` exited 0. Logs: `local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/{audit-staged.log,audit-all.log,cached-check.log}`. Escalated host files remain unstaged.
- Observation: the sidecar writer can target the input path. Inference: a same-file check before creating/writing outputs prevents this exact destructive collision.
- Remaining limits: synthetic plaintext SELF fixture only; no original game input, executable, firmware, or game runtime is read or used. No G1/G2/game gate claim follows. Keep worker logs under `local/validation/re01-unwrap-input-sidecar-collision/`.
- Next exact action after claim publication: assign the two owned files to `slot23_raw_xrefs`; require pre-guard failure evidence, then a passing focused suite. Root will review and independently replay the result.
