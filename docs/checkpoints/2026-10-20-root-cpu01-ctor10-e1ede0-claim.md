# CPU-01-CTOR10-E1EDE0-01 — direct slot-10 helper at `0xe1ede0`

- Date/time: 2026-10-20 00:30 CDT (America/Chicago).
- Integrator/owner: `/root`; assigned analyst: `/root/srv01_page_map_19660`.
- Task ID: `CPU-01-CTOR10-E1EDE0-01`; parent CPU-01, prerequisite
  `CPU-01-CTOR10-ANALYSIS-01` / E43.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
  No native implementation, pinned reference, original user input, or private analysis was changed.
- Failed hypotheses/tries: 0.

## Bounded scope

E43 byte-maps the direct call at `0xf09f04` to `0xe1ede0` from constructor slot 10 (`0xf09e60`),
but the callee's body semantics and local data references remain open. This task covers only that
callee: exact range and bytes, direct callers/callees, caller argument values when statically
available, and the effects justified by raw instructions and references. Do not infer a service or
allocator contract from neighboring function names, comments, or call order.

Pinned input is `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, prior REA image base 0.
Existing E43 is REA 6.7.0 / Ghidra 12.1.4 and provides the hash-bound callsite. Graft's exact
`graft grep "e1ede0" --in native/` exited 0 with no hits in 369 indexed files, so no existing
native implementation or fixture for this address was found in the graph.

First inspect existing local REA artifacts and pinned bytes. If they do not include a dedicated
callee analysis, use the installed REA/Ghidra pair and a distinct snapshot and private output:

```sh
mkdir -p local/re/evidence/rea-e1ede0
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xe1ede0 --provider ghidra --snapshot local/re/rea-e1ede0-snapshot.json --format json > local/re/evidence/rea-e1ede0/analysis.json 2> local/re/evidence/rea-e1ede0/analysis.stderr.log
```

Keep any analysis under `local/`; never overwrite `local/re/rea-snapshot.json`, E43/E46 snapshots,
the completed E49 artifacts, or the reference clone. Export a private report with exact commands,
REA Evidence ID if run, function range/hash, raw disassembly, call/data references, caller argument
provenance, output hashes, tool issues and limitations. If a dedicated REA run is unnecessary,
document why and still provide an independent hash/address-bound raw-byte verifier.

## Acceptance and limits

- Independently bind the callee bytes and callsite to the unchanged pinned ELF; root reviews before
  changing shared task/evidence status.
- Keep helper behavior at the level supported by the bytes. No runtime call, ABI/service declaration,
  constructor-reachability claim, native implementation, G1 promotion, installation, or download.
- Failed analysis hypotheses accumulate across agents; at three, stop and escalate by project policy.

## Claim checkpoint

- Exact next action: inspect existing E43/REA records for `0xe1ede0`, then export or reuse the
  bounded callee evidence and independently check its range/call/dataflow against the pinned ELF.
  Send only the private report to root; root owns shared docs, index refresh, audits, commit and push.
- Status: DISPATCHED; 0 failed hypotheses/tries.

## Dispatch checkpoint

- Date/time: 2026-10-20 00:31 CDT (America/Chicago); integrator `/root`.
- Task ID: `CPU-01-CTOR10-E1EDE0-01`; files changed: `HANDOFF.md`, this claim, and shared task/index
  documentation in commit `0e62162`.
- Exact dispatch: after pushing the claim, root sent `collaboration.followup_task` to
  `/root/srv01_page_map_19660` with the bounded scope above, directing it to inspect existing E43/REA
  artifacts first, use only the distinct REA snapshot/private output if needed, independently verify
  the callee against the unchanged ELF, avoid ABI/runtime guesses, and report without shared-doc edits.
- Result: dispatch accepted; analysis in progress. No callee result is inferred from the dispatch.
- Remaining risks: callee behavior and argument/dataflow are unknown; constructor reachability and G1
  remain unproven.
- Next exact action: review the private analyst report, rerun its hash/address-bound verifier, then
  integrate shared evidence and publication record.
- Failed hypotheses/tries: 0.
