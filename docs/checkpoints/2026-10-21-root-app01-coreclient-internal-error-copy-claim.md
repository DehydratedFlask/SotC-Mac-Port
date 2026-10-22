# APP-01 core-client internal error copy — claim

- Time / timezone: 2026-10-21 23:04 CDT (America/Chicago); owner/integrator `/root`.
- Task: `APP-01-CORECLIENT-INTERNAL-ERROR-COPY-FIXTURE-01`, child of `APP-01`.
- Scope: only `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`. Add exact assertions that
  `.internalError` and `.unknown(5)` both return `The game core reported an error.` Do not change
  production wording or code; this is a copy mapping fixture, not UI/runtime evidence.
- Source context: `CoreClientMapping.failureText(for:)` returns that fallback in the `default`
  branch (`launcher/Sources/LauncherCore/CoreClient.swift:52-61`). The existing
  `failureTextCoversTheErrorsTheLauncherCanSee` test checks exact copy for `.io`, `.badState`, and
  `.abiMismatch`, and checks only that `.unknown(5)` equals `.internalError`.
- Baseline hashes: `CoreClient.swift` SHA256
  `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`;
  `CoreClientTests.swift` SHA256
  `a80ae2ec916a48911645580f9317412c1d98b02ee9ac228168f039c2e2210701`.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-internal-error-copy --filter CoreClientTests`; exit 0, 22 tests passed across
  `NativeCoreClientTests` (9) and `CoreClientTests` (13). Combined log
  `local/validation/app01-coreclient-internal-error-copy-baseline.log` SHA256
  `22286765f4a240ea0890103ca94d9503186383e9635c1e245aa5e5739d38c647`; exit record SHA256
  `9a271f2a916b0b6ee6cecb2426f0b3206ef074578be55d9bc94f6f3fe3ab86aa`.
- Acceptance: assert the exact fallback string for `.internalError` and `.unknown(5)`; rerun the
  focused `CoreClientTests` suite. Preserve test-only scope; no view, process, ABI, core, or game
  behavior claim.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No source/test file
  was edited before claim publication.
- Task status: `CLAIMED`; attempt count 0.
- Exact next action: publish the claim, then add the two assertions in the single scoped test file
  and independently rerun the focused suite.
