# Checkpoint — launcher child-process core client

- Local: 2026-10-23/24 (America/Chicago)
- Agent: Swift worker; orchestrator verification
- Task: `APP-01-LAUNCHER-PROCESS-CLIENT-01`
- Gate: **FIXTURE_VERIFIED** (Swift tests + a real-core protocol run); **NOT RUN** for a GUI launch

## What exists

- `CoreLocator`: `SOTC_CORE_PATH` -> `<bundle>/Contents/MacOS/SoCGameCore` ->
  `<bundle>/Contents/Resources/SoCGameCore` -> the running executable's directory. A candidate is
  accepted only as a regular executable whose first four bytes are a Mach-O magic; directories,
  scripts, non-regular files and non-executable files are rejected with the existing
  `SystemChecks` wording, so readiness copy is unchanged.
- `ProcessCoreClient`: launches `<core> --serve`, serializes one request at a time, reads one JSON
  line per response with a 5 s timeout, verifies the id and schema, maps status/coverage/validate/
  start/stop, and tears the child down on any transport failure. `shutdown()` is idempotent
  (quit -> 1 s -> SIGTERM -> 1 s -> SIGKILL). It never invents `.running`.
- `CoreClientFactory`: prefers the process client when the locator finds a valid core, otherwise
  the in-process `NativeCoreClient`/`UnavailableCoreClient` fallback. Wired in `SoCLauncherApp`.
- `CoreCtorPrefix` gained `originalSlots` and `firstUnrecoveredIndex` (defaults keep the in-process
  client compiling); `NativeCoreClientTests` now expects `save_load == .partial` (E601/E604/E606).

## Verification

- `swift build --package-path launcher --scratch-path local/build/launcher`: build complete.
- Hermetic suite: **137** `LauncherCoreTests` + **10** `LauncherNativeCoreTests` = **147** tests,
  all passing; the real-core test is skipped without `SOTC_CORE_PATH`.
- Real core: with `SOTC_CORE_PATH=local/build/orch-r22/SoCGameCore`, the integration test passes —
  status reports ABI 1, `engineImplemented=false`, `originalSlots=573`, `slotsRegistered=308`,
  `firstUnrecoveredIndex=296`; coverage has the five rows (startup/save partial); validate accepts
  a synthetic PSAR set; `start` throws `engineNotImplemented`; the state becomes `failed` with the
  same error; `stop` succeeds. **This is protocol agreement, not game boot.**

## Limits

No Finder launch, window, VoiceOver, Retina/display-change or controller check was run. The
hermetic tests use a temporary shell-script fake core (the locator is bypassed for it). Coverage v1
has no ctor prefix, so a diagnostics view may show `NOT LINKED` for it. The real-core test needs a
built core and is enabled only via `SOTC_CORE_PATH`.
