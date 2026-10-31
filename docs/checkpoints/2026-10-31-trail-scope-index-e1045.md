# Trail source-scope refresh — E1045

- **Local time / agent:** 2026-10-31 01:06 CDT, `/root`
- **Task:** `DOC-TRAIL-SCOPE-INDEX-01`
- **Status:** COMPLETE / `ARTIFACT_VERIFIED`; failed tries: 0.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint, and the ignored local Trail graph/cache only. No game source files changed.

## Trigger and repair

The initial `trail build --check` exited 1 with `graph check: STALE` and 509 added launcher nodes. The persisted scope list had not included the current launcher tree. The repair retained existing scopes and added `native`, `launcher`, and root `tests`.

```sh
trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests
trail build --check
trail grep 'run_init_continue_c_after_merge' --in native/core/
graft check
python3 scripts/project_index.py
```

The first rebuild parsed 2,404 files and produced 10,930 nodes, 14,507 edges, and 1,804 cards. After the CritSec worker restored a previously absent `.cpp`, a later `trail build --check` again reported STALE with 509 launcher nodes. To keep the shared local graph scopes aligned, both Graft and Trail were rebuilt with the same six `--only-dir` values. Final graph: 2,405 files, 10,951 nodes, 14,519 edges, and 1,804 cards. `graft check` and `trail build --check` both exited 0. `trail grep 'critsec_lock_at' --in native/core/` found its declaration and implementation in the `.hpp` and `.cpp`; the earlier `run_init_continue_c_after_merge` query found its implementation and declaration. Graft's optional meaning tier remains 0% complete / 10,951 nodes pending. After this checkpoint file was created, `python3 scripts/project_index.py` exited 0 and reported 44,287 folders indexed. Logs: `local/validation/trail-scope-e1045/`.

This is local graph/cache state and is ignored by Git; a fresh checkout must rebuild it. No product gate changed. Next exact action: review the CritSec worker patch and bounded fixture.
