# DATA-02-BRIDGE-SXD-01 — cachedFiles bridge member comparison claim

- Date/time: 2026-10-19 23:29 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/data02_bridge_sxd`.
- Task ID: DATA-02-BRIDGE-SXD-01; parent DATA-02.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No asset or source
  files have been changed.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

The private audio-header inventory classifies exactly two `.sxd` members with observed prefix
`fbad9897`, both named `sound/shadow/bridge.sxd` in the base and patch cachedFiles archives. E25
records the magic and broad entropy observation only. DATA-02 requires byte-validated schemas and
base/update origins; path names do not establish member semantics.

Re-read both exact archive members through `Psarc.read()`. Resolve archive paths, entry IDs, spans,
decoded lengths and full hashes from the private manifest. Compare variants by exact byte offset and
report only structural boundaries that follow from repeatable bytes/lengths. Do not download or
publish any game data, merge the overlay, or infer gameplay use. If the layout is not supported by
the two examples, stop at the evidence boundary and report what additional sample is needed.

## Acceptance and limits

Create a reproducible private script in existing `local/validation/` that re-reads both members,
asserts their manifest identities and emits a bounded comparison report. Record the exact command,
output, archive/member hashes, and observation/inference split. Do not add shared parser code unless
the compared samples establish a deterministic schema and root claims that follow-up separately.

This characterization cannot establish game-reader behavior or selected-scene asset resolution.
Root integrates all shared docs and refreshes the project index after private reports arrive.
Current status: ACTIVE; failed tries: 0; next action: inspect the two exact manifest rows and raw
decoded member bytes.
