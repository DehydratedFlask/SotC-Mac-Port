# Checkpoint — round 24 verification

- Local: 2026-10-23 08:2x (America/Chicago)
- Agent: integrator
- Task: `CPU-01-ROUND-24-VERIFY-01`
- Gate: **G1 PARTIAL — no boot/audio/save/Metal claim**
- Evidence: E568

## Commands and results

| Command | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | exit 0, 345/345 targets, `-Wall -Wextra -Werror`, ASan+UBSan, arm64 |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **345/345 passed**, 8.69 s |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **442 passed / 0 skipped / 0 failed** |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0 |
| `python3 -I scripts/check_commit_attribution.py` | exit 0 (`origin/cursor/init-hash-551140-9dfc..HEAD`) |
| `python3 scripts/project_index.py` | exit 0, 4,009 folders indexed |
| `git diff --check` | clean |

## New native fixtures (all pass under ASan+UBSan)

- `app_info_registry` — 0x60 B owner layout, string/callable insert, flag=1 skip, flag=0 erase,
  missing allocator.
- `app_info_install` — four product entries in recovered order, NEO/STANDARD, debug True/False,
  camera conditional, failure site, borrowed holders.
- `registry_owner` — FIFO delegate dispatch, real `0x1290` handler coexistence and identity skip,
  null-fn failure, missing allocator.
- `registry_slot` — construct-before-destroy replace, previous teardown counts, missing-teardown
  failure.
- `container_dtor_1bede0` — C/B/A teardown, 6 releases, payload override, missing-hook failure.
- `shared_str_release_f590` — 10 release cases, wrap-safe counter, callback reload.

## New ELF ties

- `tools/lift/check_fapp_info_install.py` — 0x25a4d0 (1014 B, `12a2d0d7…`) and 0xb10 (139 B,
  `4220f09b…`), five inserts, four producers, twelve strings.
- `tools/lift/check_fcontainer_dtor_1bede0.py` — 208 B `632451ff…`, three loops, four `0xf590`,
  three `0x170c0`, list-C slot 3 = `0x19f0`.
- `tools/lift/check_ff590.py` — 73 B helper / 10 B wrapper, two register poisons.

## Files changed

`HANDOFF.md`, `docs/EVIDENCE.md`, `docs/TASKS.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, six new
checkpoints; `native/CMakeLists.txt`; 17 `init_531d60_post_base_*` modules; `save_stream`;
`test_save_stream_file`; `test_appmain_init_ctor_98a90_slice`; new `app_info_registry`,
`app_info_install`, `registry_owner`, `registry_slot`, `container_dtor_1bede0`,
`shared_str_release_f590` modules and tests; `scripts/aud02_rackquery_cfg.py` + test; three checkers.

No forbidden path, game binary, firmware, secret or private evidence file is staged. No assistant
attribution.

## Honest status

- **G1 PARTIAL.** The registry container, product/game entry writers, list-3 dispatch fragment and
  container destructor are native and fixture-verified. `0x2569d0` itself is not a native function;
  its remaining 19 constructor bodies, mode/gate dispatch, `[this+0x48]`/`[this+0x58]` bodies and
  property semantics are open. The list-A reader and dispatch timing are unknown.
- **G2–G6 NOT RUN.** No game boot, no game audio, no save/load, no Metal frame. Nothing in this round
  executed game code.
- Failed tries this round: zero analysis/implementation failures; one worker report inference (LIFO)
  was corrected from bytes to FIFO before any code used it. Two compile slips (List1bdeb0 size,
  dtor hook signature) were fixed within the same task, not counted as failed hypotheses.

## Next exact action

Continue the `0x2569d0` lift beyond the registry block, starting with the 19 constructor rows and the
`[this+0x48]`/`[this+0x58]` contracts from
`local/re/evidence/owner-installer-2569d0/report.md`.
