# Round 72 fixture completion checkpoint

Completion checkpoint time: 2026-10-21 16:44 CDT (America/Chicago). Integrator: `/root` (Codex
desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim base: `6e517b91a849c48a2775edce346c9539abb8c7e6`;
claim commit: `df1af7d60f48c4b51222f6fb7705f93fa7eaae1c`.

All three Round72 test-only tasks have one successful implementation attempt and zero failed tries.
The implementation agents edited only their assigned test files. Root reviewed the diffs and reran
each focused check independently. Production source hashes are unchanged.

## Verified fixtures

### DATA-02-PSARC-EMPTY-MEMBER-READ-FIXTURE-01 — E216

- Worker: `/root/review_host_proto`. File changed: `scripts/test_psarc.py`; added
  `self.assertEqual(a.read(3), b'')` for the existing zero-size synthetic member.
- Worker and root command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; each exited 0
  with 36 tests passing.
- Worker log `local/validation/root-round72-scout/psarc-worker.log`, SHA-256
  `1209b09247e7955d5dde23ffba55c0b27be4f05d191acd5f46c82a59e0e20446`; root log
  `psarc-root.log`, SHA-256 `55f0679f8ab195999d310c536f1ab8e01ba88b6408d3359e7a36c827845c1e3e`.
- Test file SHA-256 `1eda5a3ff072729536afad21da53fa8267f5774589c63a550378c6aab40e0816`; unchanged parser
  `scripts/psarc.py` SHA-256 `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- Observation: the synthetic empty member's unbounded read returns `b''`. Scope: no real archive or
  game asset behavior is established.

### APP-01-CORE-DIAGNOSTICS-STOPPING-STATE-FIXTURE-01 — E217

- Worker: `/root/launcher_review`. File changed:
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`; creates a synthetic
  `.stopping` `CoreStatus` with `.ok` last error and asserts the exact row
  `  Core state: stopping; last error: OK`.
- Worker and root command:
  `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests`;
  each exited 0 with 3 tests in 1 suite passing.
- Worker log `local/validation/root-round72-scout/launcher-worker.log`, SHA-256
  `2d0963e47a89fc98bd132ce9e95b9fd54c59690ecc2fc8d62a1ca1613188a3c1`; root log `launcher-root.log`,
  SHA-256 `2a29a65efab7b27f42494e59d51f612ce310e7d0f25757a86e33cf0ab609bf8d`.
- Test file SHA-256 `66e8f7c7f98465d3845cd038966934da5c32025371304c5ec6e24b4ad2a80c6a`; unchanged
  `launcher/Sources/LauncherCore/CoreReadiness.swift` SHA-256
  `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb`.
- Observation: the pure formatter includes the expected stopping/error row. Scope: no live status
  polling or UI behavior is established.

### SRV-01-FREE-LIST-CARVE-LEAD-EXCEEDS-FREE-SIZE-FIXTURE-01 — E218

- Worker: `/root/round68_native_scout`. File changed: `native/tests/test_free_list_carve.cpp`; the new
  input has a `0xf000` alignment lead, `0x1000` free size, and a fitting `1`-byte request, and
  asserts no fit.
- Worker and root build: `cmake --build local/build/native --target test_free_list_carve`; both exited
  0. Worker and root focused CTest: `ctest --test-dir local/build/native -R '^free_list_carve$'`; both
  exited 0 with 1/1 passed.
- Worker build/CTest log SHA-256
  `2a8cc4e37fa84a48b9e6ffe94aa21b4c89362d2eaad928ebc97d7e99046f6ee7` /
  `cd9d560f619a65f7982878b1d49c44d2beb7cb0a0c8a32be4e083ed97dac7ea2`; root build/CTest logs
  `free-list-carve-root-build.log` / `free-list-carve-root-ctest.log` SHA-256
  `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` /
  `3e68d0037317133fde230ae08fb257e72e357a0ff6f445f77dfbc03a148bf9f9`.
- Test file SHA-256 `359ca51ecb8362097a687a640e2307c39d1caf4bd7898b57f1706fe3af913c64`; unchanged
  `native/core/free_list_carve.cpp` SHA-256
  `a5d0d660cb49b73499c27b42724afc6d57a8735a01ce07017d11599b3e8b1630`.
- Observation: the helper rejects because lead exceeds free size before subtraction. Scope: pure
  helper arithmetic only; no allocator integration or runtime behavior is established.

Root `git diff --check` passed. The worker and root evidence supports only synthetic PSARC member
reading, synthetic diagnostics formatting, and pure carve-fit math. None advances game acceptance:
G0 remains PARTIAL; G1–G6 remain NOT RUN. No install or dependency facts changed. User-owned
`native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.

Next exact action: finish project-index and Trail refresh/checks, audit only allowlisted changes,
commit and push the three tests plus E216–E218, task status, handoff, and this checkpoint. Then scout
the next independent bounded tasks. Status: all three fixtures `FIXTURE_VERIFIED`; one successful
attempt and zero failed tries each.
