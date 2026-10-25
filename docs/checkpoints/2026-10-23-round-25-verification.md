# Checkpoint — round 25 verification and publication

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator (OpenCode orchestrator)
- Tasks: `CPU-01-APPINFO-LIST-READER-01`, `CPU-01-APPINFO-COLLECT-NATIVE-01`,
  `CPU-01-INSERT-1BDC80-NATIVE-01`, `CPU-01-OWNER-ROWS-2569D0-01`,
  `CPU-01-OWNER-PROPERTIES-2569D0-01`, `CPU-01-CAMERA-CHAIN-25A4D0-01`,
  `CPU-01-CAMERA-MATH-NATIVE-01`
- Gate labels: `FIXTURE_VERIFIED` for the native modules, `STATIC_ANALYSIS_VERIFIED` for the
  dossiers, **G1 PARTIAL** overall (no boot/audio/save/Metal).

## Commands and exact results

| command | result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | exit 0, 348 targets linked |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **349/349 passed**, 8.8 s |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **446 passed / 0 skipped / 0 failed** |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0 |
| `python3 -I scripts/check_commit_attribution.py` | `ATTRIBUTION AUDIT OK` |
| `python3 scripts/project_index.py` | exit 0 |
| `git diff --check` | exit 0 |

## What landed this round

- `app_info_collect` — list-A FIFO serialization and the `flag == 1` drain, exact field order and
  second-call/hook behaviour (E569).
- `insert_1bdc80` — list-B 0x50-byte node insert, sentinel `container+0x20`, count `+0x30`,
  four `0xf590` hand-offs (E571).
- `owner_properties_2569d0` — RetailBuild gate, mode classification (mode 2 is unreachable and
  fails explicitly), 260-row constant property table with map A/map B record shapes (E570).
- `camera_location_25a4d0` — pure `0x50ba0` basis→Euler degrees and the six-double
  `setCameraPos %.2f …` formatter; identity/yaw/general/gimbal fixtures (E573).
- `container_dtor_1bede0` list-B node type corrected to the 0x50-byte `Node1bdc80`.
- Four new ELF checkers pin the recovered bytes; `scripts/extract_owner_properties_2569d0.py`
  regenerates the table from the private symbol dump.

## Observations vs inference

- Observed: all commands above, on this tree, at this tip.
- Inference: nothing in this checkpoint claims runtime behaviour. The live camera pose, the 20
  remaining `0x2569d0` constructor rows, list-B read/clear and the report/FieldEntry construction
  remain unimplemented contracts.

## Remaining risks

- The property table is data-only; its consumer (`0xac5260`/`0xac5320` sink) is not lifted, so
  `apply_owner_properties` currently reports into a host-provided sink.
- `camera_location_25a4d0` assumes the C locale and returns `std::string`, not the original
  refcounted String.
- The four provably-zero `0x533100` `f590` wirings are in flight and are not part of this
  checkpoint's claims.

## Next exact action

1. Verify and publish the `0x533100` `f590` wiring when it lands (targeted tests, full
   `ctest`, checker sweep, mutation check).
2. Then lift the list-B reader/clear (`0x1be2f7..0x1be49d`) and the `FieldEntry` report
   construction (`0xab1b20`, `0xab0d40`).
