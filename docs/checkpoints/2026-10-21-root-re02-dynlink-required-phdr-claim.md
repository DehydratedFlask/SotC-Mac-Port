# RE-02 dynlink required program headers — claim

- Time / timezone: 2026-10-21 22:53 CDT (America/Chicago); owner/integrator `/root`.
- Task: `RE-02-DYNLINK-REQUIRED-PHDR-01`, child of `RE-02-DYNLINK-01`.
- Scope: add synthetic cases for an ELF missing required `PT_DYNAMIC` and an ELF missing required
  `PT_SCE_DYNLIBDATA`. In each case, `parse()` must raise an explicit `ValueError` with a clear
  missing-header message, rather than leaking `StopIteration`. Keep duplicate-header behavior
  unchanged. Only `scripts/ps4_dynlink.py` and `scripts/test_ps4_dynlink.py` are in scope.
- Parent spec and source: `docs/REVERSE_ENGINEERING.md` records the parser as offline inspection
  for RE-02. `parse()` currently selects each required program header with `next(...)`; the
  synthetic `dynamic_import_elf_fixture()` builds one of each. The existing suite has 27 tests and
  covers an unterminated `PT_DYNAMIC` but no missing required program header.
- Baseline hashes: `scripts/ps4_dynlink.py` SHA-256
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`; `scripts/test_ps4_dynlink.py`
  SHA-256 `027533affd10b2f9acf798be3476fdaad551cdbc6631bd2416c337237165ce52`.
- Baseline command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`; exit 0,
  27 tests passed. Private stdout log `local/validation/re02-dynlink-required-phdr-01/baseline.stdout`
  SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; stderr log
  `baseline.stderr` SHA-256 `a970274c2867bbefbf99df0fe70810bcab5a2ae5fa4a54e75fbbbbc82f0927c8`.
- Acceptance: two focused synthetic cases must each assert the exact explicit `ValueError`; all
  existing dynlink tests must continue to pass. Synthetic parser behavior only. No live ELF loader,
  runtime, or game behavior claim; no REA/Ghidra use.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No code or test file
  was edited before the claim.
- Exact next action: after claim publication and fetch verification, assign
  `/root/next_bounded_scout` the parser change and two tests in the two owned source files; root will
  independently review and rerun the focused suite.
- Task status: `CLAIMED`; attempt count 0.
