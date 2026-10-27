# E821 — Slot-309 `+0x10` payload copy and cleanup boundary

**Date:** 2026-10-27 (America/Chicago)
**Integrator:** `/root`
**Task:** `CPU-01-STARTUP-SLOT-309-PAYLOAD-OWNERSHIP-01`
**Checkpoint:** `E821`

## Scope and result

This read-only pass followed the observed slot-309 path at RVA `0xa505e0`, its concrete copy helper, and the wrapper cleanup call. It does not establish the general lifetime contract of the copied field.

On the pinned ELF (`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`), helper `0xa50220` loads and stores one qword from source offset `+0x10` to destination offset `+0x10`, then writes a vptr at destination `+0x20`. The wrapper’s cleanup call reads its indirect slot from vptr `0x15d1cc8 + 0x18 = 0x15d1ce0`. PH1 maps that address to file offset `0x15d5ce0`, which contains qword pointer `0xa50270`. PH0 maps the target to a single `ret` byte (`c3`). The inspected cleanup therefore does not read or release the copied qword on this path.

**Observation:** the field undergoes a shallow qword copy, and this cleanup target is a no-op.
**Inference boundary:** do not label the field borrowed or owned. Other nonzero payload consumers and destruction paths remain untraced. No slot-309 registration or native-lift boundary is supported; G1 remains **PARTIAL**.

## Independent review and private evidence

Root verified all nine evidence hashes cited by the private report. `/root/slot309_order_review` independently approved the report and checked the PH1/PH0 mapping. The final report SHA-256 is `a451c83d3cd0b84b6cc20817526a8072f151728e7d59a44c39faa71d46486a4e`; the slot-data log SHA-256 is `e3f16fbe782233752c1a4891e63034368ac7d8378f802d2378ee154c69a7ee18`. Private evidence remains under `local/validation/orch-20261027/slot309/worker-payload-ownership-recheck/`.

No game code was executed. No source code changed. The private report identifies the exact saved Ghidra invocation, disassembly excerpts, ABI/decompiler inference limits, and every cited log digest.

## Verification

Commands run from the repository root; complete logs are private under `local/validation/orch-20261027/e821/`:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 -I scripts/project_index.py
python3 -I scripts/project_index.py --check
```

Configure and build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed **516**, skipped **0**, failed **0**; Trail graph check exited 0 with **9,877 nodes / 13,470 edges**, deep layer not built; project-index generation and check passed at **31,928 folders**. These are repository verification results, not game boot/audio/save or Metal acceptance evidence.

The project-index command outputs are recorded in `local/validation/orch-20261027/e821/project-index-generate.log` and `project-index-check.log`.

The initial combined `git diff --check` failed on Markdown hard-break trailing spaces in this checkpoint; those spaces were removed and a later cached/worktree diff check passed. A subsequent project-index check was stale after doc/log updates; `python3 -I scripts/project_index.py` regenerated the folder index and the immediate `--check` passed at 31,928 folders.

## Status and next action

Status is **PARTIAL / STATIC_ANALYSIS_VERIFIED** for the inspected copy and cleanup path only. There were zero failed semantic hypotheses and one bounded analysis pass. Keep slot 309 unregistered. Next exact action: resolve a concrete nonzero `+0x10` consumer/destruction path before proposing a native ownership contract; otherwise retain the hook boundary.
