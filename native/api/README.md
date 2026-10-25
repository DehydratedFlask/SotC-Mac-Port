# native/api - versioned C ABI of the native game core (APP-01-CORE-ABI-01)

The launcher's only contract with the native core. The core has **no engine**, so `sotc_core_start()`
returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED` and leaves the handle in `SOTC_STATE_FAILED`. No code path
enters `SOTC_STATE_RUNNING`; every "engine" field in a result is a measured fact (`0` today).

| File | Role |
|---|---|
| `sotc_core.h` | Plain C99 header, ABI 1.0 (`SOTC_CORE_ABI_VERSION = (1 << 16) \| 0`). Checked with `clang -x c -std=c99 -pedantic -Werror -fsyntax-only` and a real C99 test. |
| `sotc_core.cpp` | Implementation. Thread-safe handle (one mutex; state callbacks fire after the lock is released). |
| `tests/test_sotc_core_api.cpp`, `tests/test_sotc_core_c.c` | ctest `sotc_core_api`, `sotc_core_c` |
| `CMakeLists.txt` | library `sotc_api` (static) + tests; links `sotc_core` automatically when that target exists |
| `standalone/CMakeLists.txt` | top level for building/testing without editing `native/CMakeLists.txt` |

## API summary

- Version / messages: `sotc_core_abi_version`, `sotc_core_error_name`, `sotc_core_state_name`,
  `sotc_core_error_message(code, buf, cap)` (snprintf-style copy, always NUL-terminated).
- Lifecycle: `sotc_core_create(abi_version_expected, &core)` (rejects a different MAJOR or a newer
  MINOR with `SOTC_ERR_ABI_MISMATCH`), `sotc_core_destroy`, `sotc_core_set_state_callback`.
- `sotc_core_validate(core_or_NULL, assets_dir, &result)`: the six PSARC archives, same order, checks and
  wording as the launcher's `AssetValidator`. `result` separates `assets_ready` from `engine_ready`
  (always 0) and `start_would_succeed` (always 0). With a handle it drives IDLE/READY/FAILED ->
  VALIDATING -> READY (all OK) or IDLE.
- `sotc_core_start` (READY, or FAILED with ready assets): STARTING -> FAILED, returns
  `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`. IDLE after failed validation: `SOTC_ERR_ASSETS_NOT_READY`; any
  other state: `SOTC_ERR_BAD_STATE`. Never `SOTC_OK`.
- `sotc_core_stop`: idempotent; RUNNING/STARTING -> STOPPING -> READY (unreachable today), otherwise an `SOTC_OK` no-op.
- `sotc_core_status` (state, last error, assets facts, `engine_implemented`, ctor prefix) and
  `sotc_core_last_error_message`.
- `sotc_core_coverage`: ctor-runner prefix plus five subsystem rows (`startup_constructors`,
  `engine_boot`, `metal_renderer`, `audio`, `save_load`) with `NOT_IMPLEMENTED` / `PARTIAL` /
  `IMPLEMENTED` / `NOT_LINKED`. Nothing reports `IMPLEMENTED`. `startup_constructors` is `PARTIAL`
  with a detail computed from `recovered_constructors()` (`N of 573 original constructor slots
  recovered; first unrecovered index I; M registered slots ran OK/blocked at RVA ...`, never
  hard-coded numbers). `save_load` is `PARTIAL` for the fixture-verified USR-DATA transaction and
  metadata only (no platform save, no real PS4 acceptance).
- Out-structs start with `uint32_t struct_size`; the library requires >= the v1 size, writes only the v1 prefix.

## Constructor-runner prefix

When built against `sotc_core` (`SOTC_API_WITH_CORE_LIB=1`) the library runs
`run_constructors(recovered_constructors(), StartupState)` once on a private heap `StartupState` and
reports `slots_registered`, `slots_with_callback`, `completed`, `run_ok`, `callback_failed`,
`blocked_on_rva`. Without it (the launcher's SwiftPM build) `linked = 0` and the subsystem row is
`NOT_LINKED`. A successful constructor run would still not imply an engine.

## Build

One line to add to `native/CMakeLists.txt` (after the `sotc_core` target, any later point):

    add_subdirectory(api)

Verified without editing it:

    cmake -S native/api/standalone -B local/build/native-agent-c
    cmake --build local/build/native-agent-c --target test_sotc_core_api test_sotc_core_c -j 8
    ctest --test-dir local/build/native-agent-c -R sotc_core --output-on-failure

The standalone build pulls `native/` in as an `EXCLUDE_FROM_ALL` subproject, so always filter ctest with
`-R sotc_core` (the other native tests are registered but not built there). `-DSOTC_API_LINK_CORE=OFF`
builds the NOT_LINKED variant.
