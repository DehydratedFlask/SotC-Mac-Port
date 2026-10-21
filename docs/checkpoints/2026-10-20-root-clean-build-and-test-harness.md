# Clean-build verification and stdlib test-harness fix

- Time/agent: 2026-10-21 00:20 CDT, orchestrator session, sole integrator. Base `77a594f`.
- No task status changes beyond verification; gates unchanged (G0 PARTIAL; G1–G6 NOT RUN).

## Clean native build

From-scratch Ninja build in a fresh directory (deleted afterwards), sanitizers on:

```sh
cmake -S native -B local/build/native-clean-252 -G Ninja
cmake --build local/build/native-clean-252 -j 8
ctest --test-dir local/build/native-clean-252 --output-on-failure
```

Results: configure exit 0, build exit 0, **252/252 CTest pass** (23.15 s).
Logs: `local/build/native-clean-252-configure.log`, `-build.log`, `-ctest.log`.
This removes the incremental-build caveat from the slot-10 checkpoint.

## Test-harness fix

`scripts/test_project_tools.py` imported its siblings (`project_index`, `unwrap_self`) without
adding the script directory to `sys.path`, so `python3 -I scripts/test_project_tools.py` failed
with `ModuleNotFoundError` even though the project documents `python3 -I` runs. Added an explicit
`sys.path.insert(0, str(Path(__file__).resolve().parent))`. After the fix every
`python3 -I scripts/test_*.py` invocation exits 0 (4 tests in this file, plus the other suites).

## Limits

Build success and fixtures are not engine startup, audio, save/load, Metal, or G1 evidence.
