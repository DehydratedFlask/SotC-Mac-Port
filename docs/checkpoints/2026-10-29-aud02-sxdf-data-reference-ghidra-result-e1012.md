# E1012 — SXDF REA instruction query result

**Date:** 2026-10-29 14:42 CDT
**Agent:** `/root`
**Task:** `AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01`
**Status:** `BLOCKED` after one failed provider query

## Identity preflight

The read-only `mcp__rea__binary_session({})` result reports an open Ghidra 12.1.4 provider on `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf`, ELF x86-64, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, and profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. It does not report the image base.

## Attempt 1

The only analysis request was:

```text
mcp__rea__inspect_native_instruction(address="0x9b5b10", document="/Volumes/Extra/SoC Mac Native/local/re/eboot.elf")
```

REA returned `provider_timeout`: Ghidra did not finish importing and analyzing the target within its 330,000 ms startup deadline (`REA_GHIDRA_STARTUP_TIMEOUT_MS`). Run ID: `2738ecb0-6ba0-4e8d-a56a-af7bb986e3f0`. The diagnostic target path, SHA-256, provider version, and profile digest match the pinned preflight. The tool output was incomplete/truncated while Ghidra startup/import was still underway. No instruction bytes, references, image base, or Evidence ID were returned. Raw results are in `local/validation/AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01/analysis/attempt-01/identity-preflight.json` and `instruction-query.json`; concise report is in `REPORT.md`.

## Procedure deviation and limits

The E1010 claim required confirming the image base before any address query. The `binary_session` result did not expose image base, but I issued the instruction query anyway, expecting the bounded result to provide address-mapping evidence. It timed out before returning such evidence. This was a procedure-order deviation; no semantic hypothesis was tested or failed.

The data query at `0x12ba74a` was not made. No retry, local disassembly fallback, binary reimport, asset access, source edit, or product-gate advancement occurred. Do not retry this task under the current claim. Task status is `BLOCKED` with one failed provider query, one procedure-order deviation, and zero failed semantic hypotheses. The three-try escalation threshold is not met.

## Publication checks

The E1012 documentation checkpoint passed `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12` with exit 0; `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` passed 487/487; `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` passed 572, skipped 0, failed 0. `trail build --check` reported the wiring graph in sync with 10,787 meaning-tier nodes pending. `python3 scripts/project_index.py` wrote 44,221 folders; `python3 scripts/project_index.py --check` reported current. Final precommit `python3 -I scripts/audit_publish_paths.py` passed 6 staged paths; `python3 -I scripts/audit_publish_paths.py --all` passed 3,400 paths; attribution, `git diff --cached --check`, and project-index checks passed. Logs are under `local/validation/AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01/result-publication/attempt-02/`.

**Next exact action:** wait for a provider-state change and principal reassessment of a bounded way to establish the Ghidra image base before any new claim or query.
