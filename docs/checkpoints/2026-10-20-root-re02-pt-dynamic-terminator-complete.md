# RE-02-DYNLINK-PT-DYNAMIC-TERMINATOR-01 complete — 2026-10-20, `/root`

Status: `FIXTURE_VERIFIED`. The prior worker was no longer reachable after the pause, so root made the two-file change directly.

- `scripts/ps4_dynlink.py`: `parse()` loop over the `PT_DYNAMIC` span now has a `for ... else` raising `ValueError('unterminated PT_DYNAMIC segment')` when no `DT_NULL` tag occurs (ELF gABI ch.5: `DT_NULL` terminates the dynamic array).
- `scripts/test_ps4_dynlink.py`: `test_parse_rejects_pt_dynamic_without_dt_null` shortens `p_filesz` and `p_memsz` (bytes 96/104) by 16 and asserts the exact message.
- Command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` → exit 0, 10 tests (baseline 9). With the parser change stashed the new test fails (1 failure), so it detects the defect.
- Log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-test.log`, SHA-256 `643c7cfc3921e5830e047bed28578bc933677c101c9c5a9f6610b6207052d03c`. Parser SHA-256 `3c3785bbb8fde61bb85aa98202b570b57c42a16b932f90aaedf28decca99ff30`; test SHA-256 `9893ece388140f8a3a0897a315c0c13b38c9f3b94df45af6bb1c6e8127f91f20`.
- Not established: behavior on the live ELF (not re-run), loader/runtime, game reachability. G1 NOT PASSED; G2 NOT RUN. Failed tries: 0. No install changes.
