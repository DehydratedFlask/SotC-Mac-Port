# Round 72 fixture claims

Claim checkpoint time: 2026-10-21 16:37 CDT (America/Chicago). Integrator: `/root` (Codex desktop
task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim base: commit
`6e517b91a849c48a2775edce346c9539abb8c7e6` (`origin/main`).

Three independent bounded test-only tasks are claimed before implementation. No code/test file has
been edited for these claims. `/root` owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, the
project index, and checkpoints; each worker owns only the one test file listed below.

## Claims

| Task | Owned file | Hypothesis and acceptance | Status / attempts |
|---|---|---|---|
| `DATA-02-PSARC-EMPTY-MEMBER-READ-FIXTURE-01` | `scripts/test_psarc.py` | The existing synthetic roundtrip archive contains `('z', b'')`; add `self.assertEqual(a.read(3), b'')` to pin an unbounded read of a zero-size member. This differs from a zero-prefix read of a nonempty member and from manifest-only zero-size coverage. | ACTIVE; 0 failed tries |
| `APP-01-CORE-DIAGNOSTICS-STOPPING-STATE-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` | A synthetic `CoreStatus(state: .stopping, lastError: .ok, assetsValidated: false, assetsReady: false, engineImplemented: false, ctor: .notLinked)` should produce the exact row `  Core state: stopping; last error: OK`. This is formatter output only. | ACTIVE; 0 failed tries |
| `SRV-01-FREE-LIST-CARVE-LEAD-EXCEEDS-FREE-SIZE-FIXTURE-01` | `native/tests/test_free_list_carve.cpp` | `try_carve_fit(0x20001000, 0x1000, 1, 0x10000)` should return no fit: alignment produces lead `0xf000`, greater than free size `0x1000`, while request size itself fits. This pins the pure helper's bound check only. | ACTIVE; 0 failed tries |

Graft/Trail scouts identified the target spans, current coverage, and adjacent task IDs; root verified
the live code/test state and ran fresh baselines. No matching task/evidence IDs were found. No user
choice is needed because each task pins one existing, explicit synthetic/helper contract.

## Baselines and claim-time hashes

| Scope | Exact command | Result / log |
|---|---|---|
| PSARC | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` | Exit 0; 36 tests. `local/validation/root-round72-scout/psarc-baseline.log`, SHA-256 `7de6e7ef645548fd64e1f5519031fde8b3621d15c98ca997d8f107920596cb80`. |
| Launcher diagnostics | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests` | Exit 0; 3 tests in 1 suite. `local/validation/root-round72-scout/launcher-baseline.log`, SHA-256 `b2737773c170f7e55b098a8fe706b023730cd43bb7a602108dd12f5ae1468ea6`. |
| Free-list carve | `cmake --build local/build/native --target test_free_list_carve`; then `ctest --test-dir local/build/native -R '^free_list_carve$'` | Build exit 0; focused CTest exit 0, 1/1 passed. Logs `free-list-carve-baseline-build.log` SHA-256 `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` and `free-list-carve-baseline-ctest.log` SHA-256 `3e68d0037317133fde230ae08fb257e72e357a0ff6f445f77dfbc03a148bf9f9`. |

Claim-time hashes:

- `scripts/psarc.py`: `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- `scripts/test_psarc.py`: `23dc33b3e06ed08fc13139a5691232f7a95492ce7d1034f924449c44fcb8bd43`.
- `launcher/Sources/LauncherCore/CoreReadiness.swift`: `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb`.
- `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`: `24ce0317362a36bf95106f59e4c58a48439d91489105e081ffe4ffb4378cfb98`.
- `native/core/free_list_carve.cpp`: `a5d0d660cb49b73499c27b42724afc6d57a8735a01ce07017d11599b3e8b1630`.
- `native/tests/test_free_list_carve.cpp`: `58c7f6a88eea2801050b00a345ea5b9478be9823be51317a7ce263782fed2f56`.

## Limits, risks, and next action

The assertions cover one synthetic PSARC member, one pure diagnostic formatter value, and scalar
carve-fit math. They establish no real game archive behavior, live status polling/UI, original
allocator integration, executable behavior, game boot/audio/save, or Metal rendering. G0 remains
PARTIAL; G1–G6 remain NOT RUN. No install/dependency facts changed. Existing user-owned
`native/api/CMakeLists.txt` and `native/api/host/` remain untouched and excluded.

Claim publication and project-index refresh are pending. Next exact action: regenerate/check the
project index, commit and push only allowlisted claim docs/index plus the completed Round71 checkpoint
sync, verify `HEAD == origin/main`, then dispatch the three single-file tasks.
