# ConCmd comparator target audit — E1047

- **Local time / agent:** 2026-10-31 01:18 CDT, `/root` integrating `/root/task_board_survey`
- **Task:** `CPU-01-CONCMD-COMPARATOR-20D80-213B0-AUDIT-01`
- **Status:** `STATIC_ANALYSIS_VERIFIED` for call-target and imported comparator identity only; failed semantic hypotheses: 0.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint. No product source/test changes.

## Hash-bound observations

- Pinned `local/re/eboot.elf`: SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Saved `local/validation/registry-analysis-agent/all.dis`: SHA-256 `9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83`.
- Same-ELF `local/re/imports.json`: SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`; embedded ELF digest matches the pinned input.
- The E8 rel32 calls at `0x20e1a`, `0x20e4c`, `0x213eb`, and `0x2141e` all resolve to `0xf8f500`. The six-byte thunk `ff256a0b6700` jumps through GOT `0x1600070`; the import row labels libc NID `AV6ipCNa4Rw` as `strcasecmp`, with evidence status `name-by-nid-hash-only`.
- Existing E1042 static report corroborates the `0x20d80` map path comparator. No matching saved REA record for `0x213b0` was found; that path is claimed only at call-target-byte level.

## Inference and limits

Assuming the NID-derived imported API name has its conventional contract, the manager paths compare keys case-insensitively. That makes the native slot-306 `std::strcmp` comparator inconsistent for names differing only by case. This audit does not prove runtime dynamic binding, manager lifecycle, or game behavior. No REA calls, tests, or source changes were made.

Private report: `local/validation/concmd-comparator-audit-e1047/REPORT.md`, SHA-256 `111df4857198e89d77a6009e294ff7fc25b447d1ec727e457a1e41e660badf1a`. Verify JSON: `verify_calls.json`, SHA-256 `c1cb5c4cf5b019f7860a7e569f9e81e62f48847a8dfd3c17a92bdf6b962e66b2`. Three `llvm-objdump` invocations and `python3 -I .../verify_calls.py` exited 0; exact commands and log hashes are in the private report and `commands.txt`.

## Next exact action

Implement `CPU-01-CONCMD-CASEFOLD-MODEL-01` as a separate model/test correction. Preserve the NID-name inference in code comments. Keep the independent `lookup_1fc80` default comparator outside this task until its callsite contract is audited.
