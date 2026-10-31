# ConCmd comparator audit — E1047 claim

- **Local time / agent:** 2026-10-31 01:12 CDT, `/root`; read-only evidence audit assigned to `/root/task_board_survey`
- **Task:** `CPU-01-CONCMD-COMPARATOR-20D80-213B0-AUDIT-01`
- **Status:** ACTIVE; failed hypotheses: 0.
- **Scope:** identify the comparator call targets at `0x20d80` and `0x213b0` using the pinned private ELF and existing saved REA/disassembly evidence. Do not edit the ConCmd model or run tests. Stop if evidence does not pin both paths.
- **Dependency:** E1042 identifies `0xf8f500` as `strcasecmp` and says these targets must be checked before implementation of the blocked `0x21620` lookup.
- **Likely live source files for comparison only:** `native/core/fconcmd_object_slot306.cpp`, `native/core/fconcmd_object_slot306.hpp`, `native/core/concmd_base.cpp`, and `native/core/concmd_base.hpp`.
- **Next exact action:** compare each call target and any adapter-specific semantics against hash-bound evidence; report observations vs inference and recommend whether to create a separate model task.
