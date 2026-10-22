# Orchestrator session handoff — 2026-10-22 (agent: Claude Sonnet 5.5, orchestrator)

Read this first, then HANDOFF.md, docs/TASKS.md, AGENTS.md. State at end of session: G0 PARTIAL; G1–G6 NOT RUN.
No boot, game audio, save/reload or Metal gameplay exists. Everything below is fixture or offline-static evidence.

## How this session worked (reuse it)

- Orchestrator dispatched file-disjoint subagents in parallel; each reported commands, hashes and limits.
  Orchestrator never accepted a report unreplayed: fresh `cmake -S native -B local/build/<name> -G Ninja`,
  full build, full `ctest`, plus the task's `tools/lift/check_f*.py` with `SOTC_NATIVE_BUILD=<build dir>`.
- Only one agent per round may edit `native/CMakeLists.txt` / `native/README.md`. Analysis agents write only
  under `local/validation/<name>/` (private; contains game strings/code; never publish).
- Subagents do not edit HANDOFF/TASKS/EVIDENCE; the integrator records E-numbers (last used: E391; next E392).
- Publish: `git add` explicit paths only, `python3 scripts/project_index.py` then `--check`,
  `python3 -I scripts/audit_publish_paths.py` and `--all`, commit, `git push origin main`.
  `native/api/CMakeLists.txt` and `native/api/host/` are user-owned and uncommitted; keep them unstaged.
  (The APP-01 host fix, an fd-0 streambuf in `native/api/host/main.cpp`, lives only in that working tree.)

## Completed this session (all FIXTURE_VERIFIED or ARTIFACT/STATIC_ANALYSIS_VERIFIED; see TASKS rows, E383–E391)

- SRV-01 region-list uncounted head unlink; PSARC negative read limit raises PsarcError; AUD-01 bank-header length guard;
  audit_publish_paths main() nothing-staged test.
- Launcher: `scripts/package_launcher.sh` + `launcher/Packaging/Info.plist` (arm64-only shell bundle, not launched);
  core readiness now requires thin 64-bit ARM64 MH_EXECUTE (`swift test` 102/102).
- APP-01-CORE-HOST-01 escalation resolved (libc++ cin hid EBADF as EOF; fd-0 streambuf). Engine still absent.
- CPU-01 lifts (each with unit test + byte checker in tools/lift/): ctors 0x83d800, 0x7ce8d0, 0xe71990, 0xdf94c0
  (via existing `_FSin` L1 wrapper), FNV helper 0x52a990/0x161fb0 + ctors 0x669450/0x842450/0x7dace0/0x6dde20,
  guarded statics 0xa77960/0x268d40 + ctors 0x78c760/0x268ce0, tiny leaves 0x14ccb0/0xb30cf0/0xb30d00/0x158340,
  chain 0x161f80/0x14c3a0/0x14c4a0/0xf730/0x14c9a0 (null parent only)/0x14d880 + ctor 0xb25470.
  None of these new lifts is registered in `startup_ctors` (another actor wired vec_ce6fb0/ce67b0 separately).
- SRV-01 allocator front-end (`allocator_front`, `iallocator.hpp`, `page_allocator`, `page_owner_map`) as contract-derived
  native code over fakes; bucket/heap/composite layers, bootstrap 0x13380, TLS hint, realloc not modelled.
- Static analyses (no REA/Ghidra; offline objdump + pinned bytes): AUD-02 rackquery 15/15 candidates on instruction
  boundaries (`scripts/aud02_rackquery_args.py`), hashed-name registry global, allocator contract, input-table ctor
  0x551140, destructor chain, app-main chain (below).

## App-main / first-G1-slice findings (observation unless marked INF; private detail in local/validation/appmain-agent/)

- Entry 0xf120 calls main 0x536730 → builds app object via ctor 0x536aa0 (base 0x2534c0) with vptr 0x15c8cd8 →
  vcall slot 3 = Run 0x25a8d0 → at 0x25aa06 slot 4 = Init 0x531d60 (returns bool), at 0x25ac9f slot 5 = Shutdown 0x5365b0.
  This resolves the earlier unresolved receiver for RE-02-APPMAIN-VCALL-25AA06/25AC9F.
- Init 0x531d60: 3,889 B, 87 direct calls (46 unique), 24 indirect, no jump tables. It builds a 0xf08-byte singleton,
  calls base Initialize 0x255130, hides the splash screen (a Sony import — must be a native no-op/contract), registers
  text tables, polls pad input, and builds the 0x98-byte hash-keyed registry at 0x2efd160 from the static 275-entry
  table 0x2eff8e0 via 0x52a220 (1,890 B, 17 direct callees, no vcalls — best self-contained slice).
- Ctor 0x551140 (28,370 B) fills that table: 3 pre-objects with calls, then a pure store region 0x55141d..0x557fe4
  (2,088 stores, 0 calls/branches) that can be byte-pinned; emit the table as generated data with null callbacks.
- `init_callee_triage.json`'s `lifted` flag is unreliable (wrongly marks 0xf590 and 0x17070). Use the triage table in the
  subagent report summarized in TASKS (trivial leaves, small, guard/atexit, medium, heavy-vcall).

## Recommended next actions (in priority order)

1. Byte-pin 0x55141d..0x557fe4 and generate the 275-entry table; lift trivial Init leaves (0x54dc10, 0x54dc20, 0x4c4950,
   one-byte `ret`s, 0xa431e0/0xa52b70/0xa599e0 setters); then registry builder 0x52a220 (+0x1fc80, 0x1e4c0).
2. Lift destructor-chain leaf 0x14d2d0 (note 0x14ccc0 stores to freed memory on the observer-null path; reorder, do not copy).
3. Resolve the next blockers for a G1-PARTIAL startup slice: base Initialize 0x255130 and its vslots 6/8 (0x534130, 0x533100
   3,961 B), producers of the globals Init reads (0x32f79a8, 0x32f6d18, 0x32f4fe0, 0x32f84f8, 0x32f8b80, 0x3305568, 0x2ee2cf8).
4. Re-open REA/Ghidra (`list_documents` returned target_unavailable this session) to confirm offline-objdump claims and
   attribute indirect calls; objdump sweeps end at 0x1579006 (`<unknown>`).
5. Pending policy: private asset/fixture string policy for name-hash inputs (callers currently supply name pointers; no
   game strings are embedded in published code).

## Risks and open items

- Another actor was editing/committing `startup_ctors`, `ROADMAP.md` concurrently (commit be05c4a). Before touching them, `git fetch` and
  check `git status`. Under `-DSOTC_SANITIZE=ON` with the user's `native/api/host` plus those edits, `sotc_core_api` aborted on a
  UBSan bool-load-255 in recursive_mutex.cpp; clean HEAD passes. Owner of the startup wiring should re-run it sanitized.
- Open observation: `audit_publish_paths.violations()` lets a directory/gitlink path without trailing slash through.
- Project-level estimate given to the user: months-to-years scale; shaders (GCN→MSL), NGS2 audio, gameplay recovery,
  and the startup chain are unstarted or unproven. No completion promise is justified (see ROADMAP).
- `docs/TASKS.md` still lists parent rows (CPU-01, SRV-01, AUD-02, DATA-02, RE-02, APP-01) as ACTIVE; none is complete.
