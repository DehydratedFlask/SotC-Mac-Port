# Checkpoint: APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01 — complete

- Local date/time: 2026-10-21 12:14 CDT.
- Agent ID: `/root` (integrator); implementation by `/root/launcher_review`.
- Task ID: `APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01`.
- Status: `FIXTURE_VERIFIED`; failed tries: 0.
- Changed file: `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` only. Added `#expect(CoreSubsystem.Status(code: 99) == .unknown(99))` to the numeric mapping test.
- Worker command: `swift test --package-path launcher --scratch-path local/build/launcher`; exit 0, 82 tests in 12 suites. Log SHA256: `8ca122bd4fe7b0897db3fc713cf2d47a3f28dbe6d3c49417dc38dfd1b6e9a400`.
- Root replay command: `swift test --package-path launcher --scratch-path local/build/launcher`; exit 0, 82 tests in 12 suites. Log SHA256: `d64c712bd43d69c7b276f66f6f0992e7e44dcd53760e6ff45b142ee0457ec563`.
- Test SHA256: `87da47e17cfb47fde2594fe709719c4f4db995851ca39f48d61cfb939c50696a`. Production `CoreModels.swift` SHA256 is unchanged: `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`. `git diff --check` passed.
- Observation: an unknown numeric subsystem code preserves its original value in `.unknown(99)`. Inference limit: this is a pure Swift value mapping; it does not test the C ABI or a running native core.
- Operational note: the first root hash-gathering command looked for `source-hashes.sha256`, which does not exist in this validation directory, and exited 1 at that file lookup. Re-running against the actual `hashes.txt` and `SHA256SUMS` paths succeeded; this did not affect either test run or source.
- Remaining risks: CoreClient runtime behavior, native game execution, boot, audio, save/load, and Metal gameplay remain unproven.
- Next exact action: publish E151, task status, and this completion checkpoint.
