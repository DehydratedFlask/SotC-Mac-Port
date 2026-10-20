# APP-01-UI-A11Y-REVIEW-01 — static macOS launcher UI/accessibility review claim

- Time / timezone: 2026-10-19 23:53 CDT (America/Chicago).
- Agent and integrator: assigned `/root/app01_ui_review`; integrator `/root`.
- Task / gate / status: APP-01-UI-A11Y-REVIEW-01 under APP-01; static review complete, runtime UI validation NOT RUN.
- Prerequisite evidence / input and source hashes: E36 records 12 passing Swift package tests and
  five pure lifecycle cases; they do not exercise the GUI. Source spans were located with Graft in
  `launcher/Sources/SoCLauncher/ContentView.swift`, `ReadinessRow.swift`, `SoCLauncherApp.swift`,
  and `launcher/Sources/SoCLauncher/LauncherModel.swift`. Verify source live before citing lines.
- Owned / changed files: worker is read-only and owns no files; root owns this claim and all shared
  docs. No source, test, package, or project files may be modified.
- Hypothesis and expected result: identify only concrete, source-supported macOS interaction or
  accessibility gaps and distinguish them from runtime behavior that cannot be confirmed statically.
  In particular, trace the folder-import URL through validation and the child-process path; do not
  infer sandbox or distribution policy that the project has not selected.
- Exact commands, CWD and nonsecret environment: from repository root, Graft source lookups were
  `graft ask "Review the launcher main window for macOS navigation, accessibility labels, keyboard behavior, status/readiness clarity, and Finder launch experience" --source --in launcher/`,
  `graft grep "assetsDirectory" --in launcher/`, and
  `graft skeleton launcher/Sources/LauncherCore/AssetValidator.swift`; the live Swift spans were
  then reviewed. No test/build was requested for this read-only review.
- Exit / observed result: source review completed; worker made no changes and ran no tests/builds.
  The review found four bounded follow-ups:
  1. P2, conditional on sandbox/distribution policy: `launcher/Sources/SoCLauncher/ContentView.swift:48-52`
     stores the imported URL; `launcher/Sources/SoCLauncher/LauncherModel.swift:27-32` passes it
     through validation; `launcher/Sources/LauncherCore/AssetValidator.swift:12-30` reads it; and
     `launcher/Sources/SoCLauncher/LauncherModel.swift:48-50` passes only the path string to the
     child. No explicit security-scope start/stop or bookmark handling was found. Apple documents
     security-scoped `fileImporter` URLs and bookmark access for sandboxed file access.
  2. P2: `launcher/Sources/SoCLauncher/SoCLauncherApp.swift:7-15` declares the primary window;
     `launcher/Sources/SoCLauncher/LauncherModel.swift:65-68` terminates the process from explicit
     Stop only. No close/Quit cleanup hook is visible. Whether the child outlives the launcher is
     not established statically.
  3. P3: `launcher/Sources/SoCLauncher/ContentView.swift:48-53` handles importer success without
     surfacing failure. Cancellation can remain a no-op; actual importer errors need user feedback.
  4. P3 follow-up: `launcher/Sources/SoCLauncher/ContentView.swift:32-42,57-68` renders changing
     lifecycle status as ordinary text; verify VoiceOver announcements through Checking/Running/
     failure before adding explicit announcements.
  No menu or keyboard source blocker was found: visible controls have text labels, readiness rows
  have textual state (`ReadinessRow.swift:7-16`), and Play uses `.defaultAction`
  (`ContentView.swift:40-42`).
- Log, capture and REA evidence paths/IDs: no runtime capture or REA evidence expected. Source
  references and primary Apple documentation links are recorded in E45 above; no separate command
  log was created.
- Observation versus inference / coverage limitations: code inspection can establish the APIs and
  paths used, not actual VoiceOver output, keyboard focus order, file access under a signed/sandboxed
  app, or Finder behavior. Those require a built app and native UI validation.
- Cumulative failed tries and distinct hypotheses: 0; no implementation hypothesis tested.
- Remaining risks / blockers: the app has no core ABI or distributable app bundle yet; final
  sandbox/distribution model and child-process ownership remain unspecified. Static inspection
  cannot establish runtime file grants, process survival, VoiceOver speech, keyboard focus, or
  Finder behavior.
- Next exact action: once sandbox/distribution and process ownership are selected, implement the
  applicable folder grant and quit-cleanup behavior; surface importer errors; then verify file
  access and lifecycle with a packaged app, VoiceOver, and Full Keyboard Access. Do not treat this
  static review as G4 acceptance.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE E45, and
  this completion checkpoint updated; INSTALL_LOG unchanged because no install/download occurred;
  project index refreshed before publication.

## Completion checkpoint — 2026-10-19 23:56 CDT

- Agent: `/root/app01_ui_review`; integrator: `/root`.
- Files changed by worker: none. Root changed `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and
  this checkpoint only.
- Exact check from the repository root: the three Graft source lookups listed above succeeded
  (exit 0); no test, build, app launch,
  accessibility session, or Finder smoke run was executed. No standalone log exists; this file and
  E45 are the retained review record.
- Observations versus inference: the missing explicit scope/bookmark path, missing quit cleanup,
  and ignored failure branch are source observations. Whether they cause failures depends on
  sandbox/distribution policy and child ownership/runtime behavior. VoiceOver change announcements
  require execution to determine.
- Remaining risks: no core ABI or app bundle; all runtime UI/accessibility checks remain NOT RUN;
  G4 remains NOT PASSED; G1 remains NOT PASSED.
- Task status: STATIC_ANALYSIS_VERIFIED, 0 failed implementation tries. Parent APP-01 remains ACTIVE.
