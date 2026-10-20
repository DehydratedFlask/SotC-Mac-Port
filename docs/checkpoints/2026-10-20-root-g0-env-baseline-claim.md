# G0-ENV-BASELINE-01 — read-only machine and native toolchain baseline

- Date/time: 2026-10-20 00:32 CDT (America/Chicago).
- Agent/integrator: `/root`.
- Task ID: `G0-ENV-BASELINE-01`; gate: G0; status: CLAIMED.
- Initial claim file was created before its matching registry/handoff updates; corrected before any
  baseline command ran. Claim bundle files are `HANDOFF.md`, `docs/TASKS.md`, generated
  `docs/FOLDERS.md`, and this checkpoint.
- Failed hypotheses/tries: 0.

## Bounded scope

Capture the current machine architecture/model, OS version, CPU and memory counts, Xcode version,
Metal compiler path, and macOS SDK version using the read-only baseline commands listed in
`docs/VALIDATION.md`. Store the exact command, stdout, stderr, exit code, and duration under
`local/validation/g0-env-baseline-20261020/`. Verify the output and add one EVIDENCE row; update the
validation document to point to that record. No install, download, build, or runtime execution.

Exact commands to execute individually:

```sh
uname -m
sw_vers
sysctl machdep.cpu.brand_string hw.ncpu hw.memsize
xcodebuild -version
xcrun --find metal
xcrun --sdk macosx --show-sdk-version
```

This closes only the current environment/toolchain inventory slice. G0 stays PARTIAL until all G0
provenance and extraction requirements pass. Do not update the project baseline to a new machine if
the observed values do not match; record the difference and keep the dated evidence separate.

## Acceptance and limits

- All six commands must return captured outputs and exit codes; errors remain visible.
- Root reviews the recorded values and hashes before shared evidence changes.
- This is not a G0 pass, game-build proof, or compatibility promise.

## Claim checkpoint

- `python3 scripts/project_index.py` exited 0, reporting `4131 folders indexed; written`. The
  generated `docs/FOLDERS.md` checkpoint-folder count changed from 28 to 29. `git diff --check`
  passed. These record indexing/whitespace only; none of the six baseline commands has run.
- Exact next action: stage and audit only this claim bundle, fetch/check divergence, commit and push;
  then create the ignored validation folder and run the six commands separately, retaining all
  outputs/status/durations in a structured private log and comparing values with the dated
  ROADMAP/VALIDATION baseline.
- Status: CLAIMED; 0 failed hypotheses/tries.
