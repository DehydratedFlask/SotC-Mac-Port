# Claim: APP-01-LAUNCHER-CORECLIENT-01 — inspect the built launcher UI

- Local time: 2026-10-21 09:56 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task: `APP-01-LAUNCHER-CORECLIENT-01`, ACTIVE continuation; 0 failed tries.
- Bounded scope: launch the already-built Mac launcher and inspect its visible readiness/state
  and accessibility text. Attempt a VoiceOver read-through if the existing host state allows it.
  No source changes, game assets, game-core launch, or G4 claim.

## Claim-time state

- `git status --short`: only the pre-existing modified `native/api/CMakeLists.txt` and
  untracked `native/api/host/` are present; both belong to escalated
  `APP-01-CORE-HOST-01` and are out of scope.
- `git log --oneline -4`: current HEAD is `95fca61`; this claim follows the published E126
  docs checkpoint.
- `ls -la local/build/launcher-agent-e/out/Products/Debug/SoCLauncher`: exit 0; executable
  exists, mode `-rwxr-xr-xr-x`, 895,984 bytes, timestamp Oct 21 07:35.
- The prior implementation checkpoint reports 89 launcher tests and a four-second process smoke
  start. It explicitly records that no window, VoiceOver, Finder launch, or signing was observed.

## Observation, inference, and limits

- Observation: a previously built launcher executable is available at the path above. The tracked
  launcher source routes the app through `NativeCoreClient` and uses
  `UnavailableCoreClient` if the ABI handshake fails.
- Inference: launching this binary can verify the current visible launcher presentation without
  implying that any game functionality exists.
- Remaining risks: the executable may fail to present a native window in this session; accessibility
  tree text is not by itself proof of spoken VoiceOver output. The launcher has no demonstrated
  game core and must remain clearly NOT BUILT / not running.

## Next exact action

Launch the existing executable in the native Mac UI, capture the window and accessibility state,
attempt the bounded VoiceOver read-through if feasible, then close the launcher and record the
actual command, result, evidence paths, observations, remaining limits, and task status.
