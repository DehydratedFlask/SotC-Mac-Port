# Slot 303 checker publication repair

- Date: 2026-10-24, 14:15 CDT
- Task: `CPU-01-STARTUP-302-303-305-01`
- Status: **STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED** for checker scope; no runtime/game claim.

The public `tools/lift/check_fctor_slot_303_a82170.py` no longer embeds the full original
instruction table or reconstructs the 455-byte function. The 96-entry table is retained only in
ignored private evidence at `local/orchestrator/r30/slot303-private-instructions.json`. Public
checks retain the complete body digest and extent, selected operand/call assertions, relocations,
and native source/order checks.

Root replay on the current files:

- `python3 -I local/orchestrator/r30/check_startup_publication_ast.py` — exit 0; 0 public table
  entries, 195 selected literal-pin bytes, longest contiguous literal run 52 B, and a renamed
  full-table positive control rejected.
- `python3 -I tools/lift/check_fctor_slot_303_a82170.py` — exit 0; 226 ELF and 48 native mutants
  rejected. The earlier 330 ELF-mutation count is superseded because those mutations depended on
  the removed public table.
- `tools/dev/with_build_lock.sh ctest --test-dir local/build/orch-r22 -R '^ctor_slot_303$'
  --output-on-failure` — exit 0, 1/1 passed.

Logs: `local/orchestrator/r30/startup-publication-fix-checker.log`,
`startup-publication-fix-ast.log`, `startup-publication-fix-ctest.log`, and
`startup-publication-fix-SHA256SUMS`. Whole-workspace verification and publication audits remain
pending at the current integration checkpoint.
