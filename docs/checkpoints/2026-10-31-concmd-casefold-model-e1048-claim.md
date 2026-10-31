# ConCmd case-insensitive model correction — E1048 claim

- **Local time / agent:** 2026-10-31 01:20 CDT, `/root`
- **Task:** `CPU-01-CONCMD-CASEFOLD-MODEL-01`
- **Status:** ACTIVE; failed implementation hypotheses: 0.
- **Scope:** update the slot-306 ConCmd manager model comparator and documentation from `strcmp` to the `strcasecmp` import contract inferred in E1047; add case-only duplicate insertion and erase fixtures. Do not claim real manager installation, runtime binding, or game behavior.
- **Planned files:** `native/core/fconcmd_object_slot306.cpp`, `native/core/fconcmd_object_slot306.hpp`, `native/core/concmd_base.cpp`, `native/core/concmd_base.hpp`, `native/tests/test_fconcmd_object_slot306.cpp`; root-owned docs/checkpoint files.
- **Graft coverage:** `std::strcmp` occurrences in native/core were exhaustively listed. `StrcmpLess` appears only in the model declaration, definition, and map comparator. Graft caller tracing found no indexed callers for the nested functor; all text occurrences were identified before editing.
- **Next exact action:** update comparator implementation/comments, add case-only insertion/erase assertions, then run the existing focused test target and checker.
