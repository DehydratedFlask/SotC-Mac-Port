# CPU-01-CTOR10-CALLER-STATE-01 — root-verified caller-state slice

- Date/time: 2026-10-20 00:58 CDT (America/Chicago).
- Task ID: `CPU-01-CTOR10-CALLER-STATE-01`; parent CPU-01; analyst `/root/ctor10_caller_state`; integrator `/root`.
- Status: `STATIC_ANALYSIS_VERIFIED` for the bounded static input path.
- Failed analysis hypotheses: 0. The worker's Graft scope lookup returned exit 1 because the requested reference prefix is not indexed; this tool-scope result is not an analysis failure.

## Evidence and independent verification

Private worker report: `local/re/evidence/cpu01-ctor10-caller-state-01/report.md`, SHA-256
`7cc31c8beacd6b0cc6553bbb1b11509afd2a46d6290ca9832f29388eb02318f9`. Its byte verifier SHA-256 is
`ddead2fb4a512a0477da46af350f0280a3f020be93160dccd7f3baeecf5f08dc`. Source ELF SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

Root independently ran:

```sh
python3 -I local/re/evidence/cpu01-ctor10-caller-state-01/verify.py > local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.stdout.log 2> local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.stderr.log; result_code=$?; printf '%s\n' "$result_code" > local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.exit.txt; exit "$result_code"
```

Exit `0`; root stderr is empty (SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Root stdout SHA-256
`2695197686a62e0fbbc298aa661db7753f678eb7580c77dadbcd4f3d274bead2` exactly matches the worker's
`verification.json` SHA-256. The verifier rechecks the ELF identity, unique file-backed PT_LOAD
ranges, instruction bytes and branch/call destinations, plus hashes of prior E43/E46/E52 artifacts.

## Observations

- On the byte-constructed caller path at `0xf09e60`, the four fields read by helper `0xe1ede0` map
  to `RSP+0x38`, `RSP+0x40`, `RSP+0x49`, and `RSP+0x4a`; the caller's verified YMM zero stores cover
  all four before the direct call at `0xf09f04`.
- The existing verified initializer helper `0xe1eda0` writes only spans ending at `RSP+0x38` for
  this call's arguments, so it does not overlap those fields.
- The separate caller byte guard at `RSP+0xa8` is also zero on this path; the branch skips unresolved
  call `0xf09efb`. The two qword gates in `0xe1ede0` therefore skip its computed calls on this path.

## Limits and next action

These are static byte observations for one exact ELF and constructed input path. They do not prove
that constructor `0xf09e60` executes or is reachable, or that the unresolved calls are unreachable
for other states. Unknown call effects remain unknown. No field type, ABI, service/lifecycle meaning,
runtime behavior, native implementation, or G1 result is inferred.

Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`,
and this checkpoint. Private evidence remains under ignored `local/re/`. Exact publication and
post-push verification will be recorded in the next HANDOFF checkpoint. Next action: select another
small CPU-01 startup-recovery slice only after checking its prerequisites and claiming it.
