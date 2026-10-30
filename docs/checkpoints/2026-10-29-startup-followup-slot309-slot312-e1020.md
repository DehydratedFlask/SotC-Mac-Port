# Startup follow-up: slot-309 callers, ownership, and slot 312

Date: 2026-10-29 21:47 CDT. Agent: /root. Scope: bounded read-only follow-ups under the active game-startup objective. VoiceOver/accessibility is out of scope.

Starting state: clean main at d2d3456467b0cea557dfb5ec08012255e046e4a4. Exact command git fetch origin main exited 0; HEAD, origin/main, and FETCH_HEAD matched. No original game code was executed. No source, launcher, test, or runtime files changed.

## E1020 — slot-309 consumer caller/order boundary

Private report: local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-SAVECONSUMER-01/CONSUMER-CALLER-ORDER-NOTE.md, SHA-256 e4eb168697db78889dd9b104b4fcc531f256764420cad00f6eeff243ed54d3e1. Pinned ELF SHA-256: d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3. Saved all-disassembly SHA-256: 9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83.

Observed direct references: consumer A at 0xa47ec3 is called through 0xa47d60 from dispatcher 0xa4d330. Consumer B at 0xa4a9a6 is reached through 0xa4a940, with parent routes from 0xa4a600 and 0xa4ab90; dispatcher 0xa4d330 selects 0xa48280 or 0xa4ab90 based on type code. The routes stop at unresolved computed jumps 0xa4869b and 0xa4afba. A RELRO qword at 0x15fff00 contains 0xa4d330 and is loaded at 0xa5288c into a descriptor passed to 0x3b5c0. Its eventual invoker is unknown. No saved call edge or ordered registration record relates these consumers or the callback registration to slot-309 initializer 0xa505e0.

The exact queries, disassembly windows, bounded RELRO scan, and unavailable readelf attempt are listed in the private report. Result: PARTIAL / STATIC_ANALYSIS_VERIFIED for the listed caller references only. Do not infer runtime ordering or dispatch target behavior.

## E1021 — slot-309 alias and teardown ownership boundary

Private report: local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-TEARDOWN-OWNERSHIP-01/REPORT.md, SHA-256 e7052852f368033a5231562a0befcf92c89234d6198a3929c1ee5d7d3d4e36d2. The bounded constructor path builds two borrowed string views with null control pointers and initializes descriptor bytes +0x80..+0x9f to zero. Its atexit callback unregisters the static object, releases the borrowed views as no-ops on these inputs, does not free the global, and does not touch the descriptor fields. The root passes the global to 0x26ea0, which stores an alias at companion +0x90. In the inspected registration route, 0x268b0 passes companion +0x28 and +0x48 to 0x27470; no concrete write to the aliased global descriptor appears in that body. Later manager/list use of companion +0x90 and execution order against either consumer remain unresolved.

Result: PARTIAL / STATIC_ANALYSIS_VERIFIED for the observed constructor, registration, and teardown effects. Do not claim descriptor immutability, order, or native-lift safety.

## E1022 — slot-312 liftability review

Private report: local/validation/orch-20261030/CPU-01-STARTUP-SLOT-312-LIFTABILITY-01/liftability-review.md, SHA-256 057aa802caa05a1af2c100cea33a692171ef2916b19aece1ab4b9bdcce0214fc. The row-local indirect calls resolve to fixed ret helpers, but the constructor builds title metadata and debug-menu entries, registers cleanup, and reaches unresolved object-dependent and global-state-dependent calls. At least 18 nested indirect sites remain unresolved in the selected descendants, a lower bound. No evidence shows slot 312 is required before the game scene, audio, or save path, and no evidence proves omission safe.

Result: PARTIAL / STATIC_ANALYSIS_VERIFIED as a liftability screen. No bounded native lift is proposed; continue prioritizing the earlier slot-309 startup gap. The targeted Ghidra exports and their hashes/commands are in the private report.

## Current status and next action

The active parent CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01 remains ACTIVE; slot 309 stays unregistered and G1 stays PARTIAL. The native sotc_core_start path still returns SOTC_ERR_ENGINE_NOT_IMPLEMENTED. Cold boot, authentic game audio, real save/reload, game-driven Metal rendering, and forbidden-runtime absence remain unproved; G5 is NOT RUN. These analyses changed no product gate.

No tests or builds were run during the static-analysis subtasks. Before publication, the integrator ran the repository verification commands below; exact logs and final outcomes are recorded in HANDOFF.md:
- cmake -S native -B local/build/orch-r22
- cmake --build local/build/orch-r22 -j 12
- ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
- SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
- trail build --check
- python3 scripts/project_index.py
- staged/all-path publish audits, attribution scan, and diff checks

Next exact action: keep slot 309 unregistered. Resume this path only if new saved evidence establishes a concrete consumer invocation/order edge or a native-lift boundary; otherwise do not repeat the same callback/alias scans. No accessibility work is in scope.
