# APP-01-IMPORTER-ERROR-01 — cancellation behavior reclaimed

- Time: 2026-10-20 01:22 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task: `APP-01-IMPORTER-ERROR-01`; parent APP-01.
- Status: ACTIVE; failed tries: 0.
- Files changed: documentation only (`HANDOFF.md`, `docs/TASKS.md`, this claim checkpoint, and regenerated `docs/FOLDERS.md`). No launcher source changed yet.

## Requirement and live source evidence

The user selected a system alert for folder-import errors and requested silent cancellation.
Graft query:

`graft ask "Where does the macOS launcher choose a game folder, validate it, surface importer errors, and update readiness state?" --source --in launcher/`

returned `ContentView` at `launcher/Sources/SoCLauncher/ContentView.swift:L4-L81`. A subsequent
`graft grep "folderImportError" --in launcher/` found all six uses in that symbol. The live
`.fileImporter` callback currently assigns `error.localizedDescription` for every `.failure`.
Thus the explicit silent-cancellation behavior has not been shown; whether the platform invokes
the failure branch for cancellation is also untested.

## Bounded plan and acceptance

Add a small pure `LauncherCore` helper that maps cancellation to no alert and operational failures
to their localized alert text. Use the helper from `ContentView`, preserving the existing system
alert for non-cancellation failures and leaving the selected folder/readiness unchanged on import
failure. Add focused helper tests for cancellation and an ordinary error. Acceptance is `swift test`
passing from `launcher/`; that proves classification and build behavior, not native alert
interaction or VoiceOver presentation.

Exact next action: implement the helper and tests after this claim checkpoint is published, then run
the focused Swift package suite and record results. No game files, original executable, references,
or runtime were opened or modified.

## Completion checkpoint — 2026-10-20 01:32 CDT, /root

- Implemented public LauncherCore helper FolderImportErrorClassifier in
  launcher/Sources/LauncherCore/LauncherLifecycle.swift. It filters Swift CancellationError and
  Cocoa NSError values in NSCocoaErrorDomain with NSUserCancelledError; other errors use
  localizedDescription. ContentView assigns the helper's optional result on the existing importer
  failure branch, preserving the system alert for operational failures and leaving selection and
  readiness untouched.
- Focused tests cover Cocoa cancellation, Swift cancellation and localized ordinary failure in
  launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift.
- Worker run 1: swift test from launcher/ exited 1 because the test source lacked import Foundation.
  Worker added the import; run 2 exited 0 with all 14 tests in 3 suites passed. Full worker log:
  local/validation/app01-importer-error-01-swift-test.log; SHA-256
  54f47af305ed93e1af2e6c12e5126f4878ce757488dd7fe60242b66ffb2acd5a.
- Root independently ran
  swift test > ../local/validation/app01-importer-error-01-root-swift-test.log 2>&1; result_code=$?; printf 'exit status: %s\n' "$result_code" >> ../local/validation/app01-importer-error-01-root-swift-test.log; exit "$result_code"
  from launcher/. Exit 0; 14 tests in 3 suites passed. Root log SHA-256
  d1492ba66cccfc3e5f5c2536be8c4f40e4f55260295bff1ecbebf07e38b55705.
- git diff --check and graft check passed (wiring graph synced; meaning layer remains unbuilt,
  1,225 nodes pending). lsof -nP +D launcher/.build returned no open files. Root removed only the
  generated build cache with a guarded Python shutil.rmtree and reran python3 scripts/project_index.py:
  exit 0, 4,139 folders indexed, no docs/FOLDERS.md diff, and launcher/.build absent.
- Failed tries: 1 (missing test import corrected). Native alert presentation and platform picker
  cancellation delivery were not exercised; no UI-runtime or game claim follows.
- Exact next action: publish E58 and the task/handoff checkpoint, then commit the three allowlisted
  launcher files and required port documentation to origin/main.
- Status: ARTIFACT_VERIFIED for helper behavior/build/tests; native UI interaction NOT RUN; G1 NOT
  PASSED.
