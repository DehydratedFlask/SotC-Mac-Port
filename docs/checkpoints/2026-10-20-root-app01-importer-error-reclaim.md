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
