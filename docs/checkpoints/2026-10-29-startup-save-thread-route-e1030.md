# E1030 — save-thread callback route and provider boundary

- Date: 2026-10-29 23:10 CDT
- Agent: `/root` (integrator)
- Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`; provider update for `RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01`
- Status: slot-309 task remains ACTIVE with a new `PARTIAL / STATIC_ANALYSIS_VERIFIED` callback-thread edge; 0 failed semantic hypotheses. `0x499f80` task remains BLOCKED; 0 failed semantic hypotheses.

## Observations

The pinned ELF and saved `all.dis` show the `0xa5288c → 0x3b5c0 → 0x3b6a0 → 0xf8fab0` path. The `0xf8fab0` GOT/JMPREL record resolves by its NID to `scePthreadCreate`; the thread name bytes at `0x12c41ed` are `Save Task Thread`. The supplied callback descriptor points to `0xa4d330`, whose existing REA/Ghidra Evidence ID is `ev_8bbfdf359b1c43a9e13d4b805c072bf339f1c1a209a4ea86c71c9adf29ebe63f`. Its saved dossier describes a queued task worker. Detailed byte, import, and call-path evidence is in the private addendum `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-CALLBACK-ROUTE-01/CALLBACK-PTHREAD-ADDENDUM.md`.

The callback object is initialized by `0xa52530`, called from `0x2569d0`; the latter is called by `0x533100`. The owner and startup order remain unknown. No reviewed edge ties this route to slot 309 (`0xa505e0`), so the route does not justify registration or a product change.

The REA `procedure_callers(0x533100)` query timed out at 330,000 ms during target import, returning no callers or Evidence ID (provider run `2738ecb0-6ba0-4e8d-a56a-af7bb986e3f0`). A separate REA CLI decompile of `0x499f80` also timed out at the default 330,000 ms Ghidra startup deadline (run `a92119d9-664f-4710-b9dc-828f7d388408`); its JSON output is `local/validation/orch-20261029/RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01/cli-decompile-499f80.json`, SHA-256 `73ceedd72fcb48c4573bee62046881ef7bdbb2a1ee16736e0e2d9c31e1f39846`. No body facts for `0x499f80` were obtained.

## Exact commands and outcomes

- `python3 -I` parsed `local/re/eboot.elf` with `scripts/ps4_dynlink.py`; pinned ELF SHA matched. It decoded the `0xf8fab0` PLT/GOT target, found exactly one JMPREL row of type 7 with zero addend, matched NID `6UgtwV+0zb4`, recomputed `dyn.nid("scePthreadCreate")`, and matched the helper thunk decoder. Exit 0.
- Saved instruction slices were read from `local/validation/registry-analysis-agent/all.dis`; source SHA matched `9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83`.
- Existing REA dossier for `0xa4d330`: `local/re/evidence/sav01-region-dossier-01/queries/q2-fun-00a4d330/stdout.json`; Evidence ID above; pinned ELF hash matches.
- `mcp__rea__procedure_callers({"procedure":"0x533100"})`: `provider_timeout`, 330,000 ms, no caller list or Evidence ID.
- REA CLI `decompile` for `0x499f80`: `provider_timeout`, 330,000 ms, no body or Evidence ID. The failure output and stderr were preserved in the task directory.

## Product status, risks, and next action

No source, launcher, reference clone, tests, runtime, or game input changed. No cold boot, game audio, save/reload, Metal scene, or forbidden-runtime gate was demonstrated. `sotc_core_start()` still returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; G1 PARTIAL, G2 NOT RUN, G3 PARTIAL, G4 PARTIAL, G5 NOT RUN.

Next exact action: run one REA CLI function dossier for `0x533100` with `GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`, writing a unique private snapshot/output under the existing callback-route directory. Use the returned caller Evidence to test constructor ownership/order, then stop if no startup edge is found. Do not infer runtime invocation or game boot from this static path.

## Verification

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: graph in sync with 10,787 nodes pending the meaning layer.
- `python3 -I scripts/audit_publish_paths.py --all`: 3,414 paths passed; `python3 -I scripts/check_commit_attribution.py`: exit 0 (`origin/main..HEAD`).
- `python3 scripts/project_index.py --check`: 44,259/current. The first check ran while verification logs were still being created and reported `local/indexes/folders.json` stale; generation and check passed after the logs were present.
- Logs: `local/validation/orch-20261029/STARTUP-SAVE-THREAD-E1030-PUBLICATION/`.

Files changed at this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, this checkpoint, and private callback-route reports. No source/runtime change.
