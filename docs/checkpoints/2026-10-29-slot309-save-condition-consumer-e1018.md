# E1018: Slot-309 save-condition consumer edge

Date: 2026-10-29 21:10 CDT. Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`. Status: `PARTIAL / STATIC_ANALYSIS_VERIFIED`; one bounded static consumer-reference scan, zero failed semantic hypotheses.

## Scope and starting state

This checkpoint continues game-startup work only; VoiceOver and accessibility are out of scope. The worktree started clean on `main` at `b64823fba9dd62c2305608b58ea4d1bd7b4476fc`. `git fetch origin main` exited 0, and `HEAD`, `origin/main`, and `FETCH_HEAD` matched.

The pinned game ELF is `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The saved disassembly input is `local/validation/registry-analysis-agent/all.dis`, SHA-256 `9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83`.

## Observations

The exact-address scan was:

```sh
rg -n -F -e '0x32f79e8' -e '0x32f7af0' -e '0x32f7d30' -e '0x32f7e08' -e '0x32f7eb0' -e '0x32f7f88' local/validation/registry-analysis-agent/all.dis
```

In the saved disassembly, `0xa47ea9` and `0xa4a992` each load the slot-309 global object address `0x32f79e8` into `RDI`; the surrounding paths call `0xa4d9c0` at `0xa47ec3` and `0xa4a9a6`. Existing slot-309 `query.log` evidence names that object `force_save_debug_condition`. These are static address and call edges; the scan does not prove either path runs at runtime.

The bounded structural replay was:

```sh
python3 -I tools/lift/analyze_slot_struct.py 0xa4d9c0 --label CPU-01-STARTUP-SLOT-309-CONSUMER-0A4D9C0-01 > local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-CONSUMER-0A4D9C0-01.json
```

It produced parseable JSON (SHA-256 `b0d859cfb5c347d6bbdc39f53687023152d6d4fee1a387d3fcd5bc47e6995a18`) for the pinned ELF. The body spans 1,154 bytes, contains 266 reachable instructions, and has body SHA-256 `743a740d63ff9d68642c3d6eb547ec416dbf0ac1c478e306d8e8675f4873b8be`. It has one indirect call, `call rax` at `0xa4de23`; the target and callback contract are unresolved. Private report: `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-SAVECONSUMER-01/REPORT.md`.

REA requests for `current_document`, `list_documents`, and `open_binary` each remained pending for more than 90 seconds and were terminated. No Evidence ID, Ghidra result, or source-level interpretation was returned. The attempted `readelf -l` command also failed because `readelf` is unavailable; it contributes no evidence.

## Result and limits

The two static callsites establish a concrete consumer edge for the slot-309 object, satisfying E911's resume condition. They do not establish runtime reachability or behavior. The `0xa4de23` indirect target and the object's constructor and teardown ownership remain open, so the object cannot be classified as unused or safely omitted. Slot 309 remains unregistered and G1 remains PARTIAL.

No source or launcher files changed; no game code ran. `sotc_core_start()` still returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`. Cold boot, actual game audio, real save/reload after relaunch, gameplay Metal, and forbidden-runtime/module absence remain unverified.

## Repository verification before publication

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exit 0; wiring graph in sync, 10,787 meaning-tier nodes pending.
- `python3 scripts/project_index.py`: 44,240 folders indexed.
- `python3 -I scripts/audit_publish_paths.py`: 6/6 staged paths passed. `python3 -I scripts/audit_publish_paths.py --all`: 3,405 paths passed after staging.
- `python3 -I scripts/check_commit_attribution.py` and `git diff --check`: exit 0.

Private logs are under `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-SAVECONSUMER-01/`. These checks verify the existing harness and publishable paths only; no game gate advances.

## Next action

Resolve the target and contract of the `0xa4de23` indirect callback for the exact slot-309 object, then close the related constructor and destructor ownership edges before considering native wiring.
