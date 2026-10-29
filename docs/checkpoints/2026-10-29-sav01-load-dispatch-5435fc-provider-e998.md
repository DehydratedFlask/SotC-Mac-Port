# E998 — SAV-01 load-dispatch target-origin provider timeout

Date: 2026-10-29 12:22 CDT
Agent: `/root`
Task: `SAV-01-LOAD-DISPATCH-5435FC-01`
Parent: `SAV-01`

## Attempt and result

Starting tree: clean `main` at `f9df6e7bbd23501ec00282d2a05208e69248225e`, matching `origin/main`. Pinned input `local/re/eboot.elf` SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0.

REA opened the pinned ELF with the existing q2 Ghidra 12.1.4 snapshot. The sole `inspect_native_instruction(0x5435fc)` request returned `provider_timeout` at 330,000 ms, with no instruction bytes, references, target origin, or Evidence ID. REA 6.7.0, Ghidra 12.1.4, profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, run ID `126e828e-7595-4795-aa99-8cd6eba74fb0`.

One follow-up invocation used the installed `analyzeHeadless` with the existing project `local/ghidra/persistent-20261020/SotC`, `-process eboot.elf -noanalysis -readOnly`, and the private bounded script `QuerySAV01Dispatch5435FC.java`. Exact command and incident facts are in `local/validation/SAV-01-LOAD-DISPATCH-5435FC-01/analysis/attempt-01/fallback-incident.txt`. The command response was truncated and no stdout/stderr was redirected or retained; there is no exit status, session ID, capture, or result. No retry or further analysis followed.

## Ghidra project side effect

During the fallback invocation, the project files below acquired a new mtime at `2026-10-29 12:19:02 -0500`:

- `local/ghidra/persistent-20261020/SotC.rep/versioned/~index.dat`: 59 bytes, SHA-256 `0a5034997e47499d758115fa580ad21d96557d2a32c89f2263bcfd3fa8a9a6e5`.
- `local/ghidra/persistent-20261020/SotC.rep/idata/~index.dat`: 105 bytes, SHA-256 `81c5b7fd0df99dfd6b1f2b201122d64c83d83a4b9564fdf7040dfc3b2c5b7b29`.
- `local/ghidra/persistent-20261020/SotC.gpr`: 0 bytes, mtime remains `2026-10-20 22:06:40 -0500`, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.

No before-run hashes or snapshots are available, so the exact project-content change cannot be determined. No recovery/restoration was attempted. Do not claim the project is unchanged. The private `fallback-incident.txt`, `commands.txt`, script, and REA timeout JSON are retained under `local/validation/SAV-01-LOAD-DISPATCH-5435FC-01/analysis/attempt-01/`; their SHA-256 values are `29894f6f55260313acfd36b1d53729a6120b9c865e98932bb419327b3231f232`, `26ac87df93232119c4473ffa2a5457bae7e6878d3d3ebfc351517cc8a8a55c93`, `43756788679e2b8a21302ce6f9dede58ab96fa329b981c5256b1a012978146e3`, and `dab5c6277571baff364a321a462debd04f9a4ef829195b112cce0e83e9cafb8b`, respectively.

## Status and limits

The E996 child task is `BLOCKED`: one REA provider timeout and one incomplete headless invocation; zero returned bytes and zero failed semantic hypotheses. The pinned ELF still matches its input hash. No user save or original game asset was accessed. No target origin, runtime/API meaning, rollback, real-save behavior, or product gate was established. `SAV-01` remains partial and SAV-04 remains `NOT RUN`.

Next exact action: stop work on `0x5435fc`; do not retry REA/headless or modify/restore the Ghidra project without a known before-state. Continue with a separate task that does not require this project.
