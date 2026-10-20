# G0-ENV-BASELINE-01 — read-only machine and native toolchain baseline

- Date/time: 2026-10-20 00:35 CDT (America/Chicago).
- Agent/integrator: `/root`.
- Task ID: `G0-ENV-BASELINE-01`; gate: G0; status: ARTIFACT_VERIFIED.
- Failed hypotheses/tries: 0.
- Claim and dispatch were published in commit `8d5af9d` before any baseline command ran.

## Files and exact command

Shared documentation updated: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/VALIDATION.md`, and this checkpoint. `docs/FOLDERS.md` will be regenerated after this file
is created. Private captures are under the ignored directory
`local/validation/g0-env-baseline-20261020/`.

Exact command:

```sh
python3 -I local/validation/g0-env-baseline-20261020/capture.py
```

Exit code 0. It ran each of the six candidate baseline commands from `docs/VALIDATION.md` as a
separate subprocess. The private `capture.json` retains argv, local start/end, duration, exit status,
stdout, stderr, and paths to individual output files. Every inner command exited 0 and every stderr
was empty. `capture.json` SHA-256:
`cea524d435eeef15e02d506744969dab872990e4812d6621d6b256f042e475b0`; capture script SHA-256:
`14e5d80357dabb626d7f6749e3fc47f31f8fa519d2cbe8b6eeba17367a3eea32`.

| Exact command | Observed stdout summary | Exit |
| --- | --- | ---: |
| `uname -m` | `arm64` | 0 |
| `sw_vers` | macOS 27.0.1, build 26A434 | 0 |
| `sysctl machdep.cpu.brand_string hw.ncpu hw.memsize` | Apple M4 Pro; 12 CPUs; 51,539,607,552 bytes (48 GiB) | 0 |
| `xcodebuild -version` | Xcode 27.1, build 27A9275 | 0 |
| `xcrun --find metal` | `/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metal` | 0 |
| `xcrun --sdk macosx --show-sdk-version` | `27.0` | 0 |

## Observation, inference, and limits

Observed values match the supplied 2026-10-19 machine baseline (M4 Pro, 12 CPU cores, 48 GB unified
memory, macOS 27.0.1, Xcode 27.1); this run additionally records architecture, build IDs, Metal
compiler path, and SDK version. This establishes local toolchain inventory for this date only. It
does not establish that Metal compiles or runs correctly, compatibility of the game, game boot/audio/
save, or a G0 pass. No install, download, build, or game/runtime command ran.

## Index, publication, and next action

- `python3 scripts/project_index.py` exited 0 and reported `4132 folders indexed; written` after
  adding this completion checkpoint. `docs/FOLDERS.md` now reflects 30 immediate files under
  `docs/checkpoints`.
- `graft check` exited 0: wiring graph is in sync; optional deep meaning layer remains 0% with 1,219
  nodes pending. No source files changed in this task.
- `git diff --check` passed.
- Publication: staged audit `python3 -I scripts/audit_publish_paths.py` accepted 6 paths; full audit
  `python3 -I scripts/audit_publish_paths.py --all` accepted 585 paths; `git diff --cached --check`
  passed; `git fetch origin main` succeeded and pre-commit divergence was `0 0`. Commit `a4c40f5`
  (`docs: verify G0 environment baseline`) pushed successfully (`8d5af9d..a4c40f5`). Post-push
  `git status --short` was empty; `HEAD` equals `origin/main` at
  `a4c40f566b8f31b168a409f6d3bec42d53d0dd1e`; divergence `0 0`.
- Exact next action: continue root integration of active `CPU-01-CTOR10-E1EDE0-01` when its analyst
  returns. G0 stays PARTIAL.
