# CritSec slot-414 atexit teardown coverage audit — E1050

- **Local time / agents:** 2026-10-31 01:43 CDT, `/root`; independent read-only review `/root/driver_gap_review`.
- **Task:** `RE-02-CRITSEC-SLOT414-ATEXIT-LIFECYCLE-AUDIT-01`.
- **Status:** COMPLETE / `STATIC_ANALYSIS_VERIFIED`; failed semantic hypotheses: 0.
- **Question:** does a current host shutdown owner consume `StartupAtexitLog` entries or invoke slot 414's recorded destructor identity `0x3b380`?
- **Files changed:** documentation only (`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint). No source/test edits.
- **Graft-first exploration:** exact commands, search cautions, and source-span notes: `local/validation/critsec-slot414-teardown-e1050/REPORT.md`. Graft caller tracing warned duplicate `run_constructors` definitions could suppress edges; exhaustive lexical results resolved the production call sites.

## Observations

- `StartupAtexitLog` exposes `record`, `size`, and `at`; `record` appends `{dtor_rva, object}`. Graft found no production `at()` consumer; the reads are in tests/testkit.
- `init_slot414_57b9b0` calls `mutex_init_3b310`, records the `0x3b380` identity, zeroes fields, and returns. It never calls `mutex_destroy_3b380`. The header comment labels the dtor “recorded, never run.”
- `default_critsec_slot414_hooks` maps the destroy field to `critsec_destroy_hook`, but its direct invocation was found only in the CritSec test on a separate local object. Graft found no production caller of `init_slot414_57b9b0`.
- `StartupState` owns `ConCmdRegistry`/the log but has no `Slot414State` member. The recovered constructor table has no row for slot 414.
- The C API's `measure_ctor_prefix` creates a local heap `StartupState`, runs the diagnostic prefix, and stores only scalar counters. `sotc_core` itself has no `StartupState`; `sotc_core_start` returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; stop only handles state callbacks; destroy calls stop and deletes the core.
- Independent review by `/root/driver_gap_review` confirmed these spans and the absence of any current API owner or slot-414 teardown dispatch.

## Conclusion and boundary

**Observed:** no current host shutdown path drains or executes the log, and no production path invokes slot 414's destroy callback. The current diagnostic state is temporary and no engine lifecycle retains it.

**Inference:** normal C++ destruction releases the temporary log's vector storage; it does not call the stored `dtor_rva` values. No original address is executed.

Any future persistent engine state needs an explicit owner plus native teardown-hook mapping and ordering. This audit does not establish future teardown behavior, retail semantics, or game behavior. Tests were NOT RUN because the task is read-only. The broader CritSec lift remains PARTIAL, and no G0–G6 gate advances.

- **Project index:** `python3 scripts/project_index.py` — exit 0, 44,294 folders indexed.
- **Prepublication audits:** staged publication audit accepted 6 paths; all-path audit accepted 3,458 paths; attribution and staged/working diff checks passed. Logs: `local/validation/critsec-slot414-teardown-e1050/`.
- **Next exact action:** do not implement a destructor drain without an engine lifecycle owner and a native cleanup-order contract. Resume the independent `0x499f80` host-failure-policy task only after the user's preference arrives.
