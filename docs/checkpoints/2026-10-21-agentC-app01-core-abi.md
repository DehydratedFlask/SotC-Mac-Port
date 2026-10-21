# Checkpoint - APP-01-CORE-ABI-01: versioned core C ABI + Swift CoreClient

- Date: 2026-10-21 (local), agent: agent-C (Claude Sonnet 5.5), task: APP-01-CORE-ABI-01.
- Status: **PARTIAL** (ABI + client abstraction + tests done; `LauncherModel` not rewired; no engine). Attempt 1, no failed tries.
- Not touched by design: `native/CMakeLists.txt`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`. Nothing committed or pushed.

## Files

New: `native/api/{sotc_core.h,sotc_core.cpp,CMakeLists.txt,README.md,standalone/CMakeLists.txt,tests/test_sotc_core_api.cpp,tests/test_sotc_core_c.c}`;
`launcher/Sources/LauncherCore/{CoreModels.swift,CoreClient.swift}`; `launcher/Sources/LauncherNativeCore/NativeCoreClient.swift`;
`launcher/Sources/CSotcCore/{sotc_core.cpp,include/sotc_core.h}` (symlinks into `native/api/`);
`launcher/Tests/LauncherCoreTests/CoreClientTests.swift`; `launcher/Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift`.
Edited: `launcher/Package.swift` (targets `CSotcCore`, `LauncherNativeCore`, test target `LauncherNativeCoreTests`, `cxxLanguageStandard: .cxx20`), `launcher/README.md`.

## Commands and results

| Command | Result |
|---|---|
| `cmake -S native/api/standalone -B local/build/native-agent-c` | exit 0 (links `sotc_core`, `SOTC_API_WITH_CORE_LIB=1`) |
| `cmake --build local/build/native-agent-c --target test_sotc_core_api test_sotc_core_c -j 8` | exit 0, `-Wall -Wextra -Werror`, ASan+UBSan; log `local/build/native-agent-c-build.log` |
| `ctest --test-dir local/build/native-agent-c -R sotc_core --output-on-failure` | 2/2 pass (`sotc_core_api`: 7 test functions; `sotc_core_c`: C99 flow) |
| `clang++ ... -DSOTC_API_WITH_CORE_LIB=0` (NOT_LINKED variant, ASan+UBSan, scratchpad) | all checks pass |
| `clang -x c -std=c99 -pedantic -Werror -fsyntax-only native/api/sotc_core.h` | exit 0 |
| mutation: patched copy of `start()` to enter RUNNING and return OK | `test_sotc_core_api` FAILS at the `ENGINE_NOT_IMPLEMENTED` assertion (exit 1), so the never-running tests do detect the regression |
| `swift test --package-path launcher --scratch-path local/build/launcher-agent-c` | exit 0, **60 tests**: 42 pre-existing (7 suites, unchanged) + 10 `CoreClientTests` + 8 `NativeCoreClientTests`; log `local/build/launcher-agent-c-test.log` |

## Observations vs inference

Observed: SwiftPM compiles a C++ target whose sources are symlinks outside the package and imports the C header from Swift without breaking `swift test`/`swift build` (the `SoCLauncher` product still builds). Swift `AssetValidator` and the C validation agree on missing/bad-magic/ordering/first-problem text (tested).
Inferred: a future `SoCGameCore` child process can link `sotc_api` + `sotc_core` and sit behind a child-process `CoreClient` without touching `LauncherCore`.

## Honest limits

- `start()` is `SOTC_ERR_ENGINE_NOT_IMPLEMENTED` by construction; RUNNING is unreachable. No boot, audio, or save claim.
- The SwiftPM facade is built without `sotc_core`: ctor-runner facts read `NOT_LINKED` there; they are real only in the CMake build.
- Ctor-prefix facts run the runner once per process on a private `StartupState`; counts track `startup_ctors.cpp`, not hard-coded (tests cross-check against a direct run).
- Standalone build registers the other 265+ native tests (EXCLUDE_FROM_ALL subproject) but does not build them: always use `ctest -R sotc_core`.
- `LauncherModel` still uses `SystemChecks.nativeCore` + `Process`; the validate/start path is not yet routed through `CoreClient`. G4 stays NOT PASSED.
- Symlinks in `launcher/Sources/CSotcCore` need a git checkout that preserves symlinks; `scripts/audit_publish_paths.py` has not been run on the new (unstaged) paths.

## Next exact actions

1. Integrator adds `add_subdirectory(api)` to `native/CMakeLists.txt`, rebuilds `local/build/native`, runs full ctest.
2. Route `LauncherModel.validate/start/stop` through a `CoreClient` (keep `SystemChecks.nativeCore` as the child-process presence probe until `SoCGameCore` exists).
3. Run `python3 scripts/project_index.py` and `python3 -I scripts/audit_publish_paths.py` (staged) before the commit.
