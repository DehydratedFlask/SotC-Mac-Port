# DATA-02-BRIDGE-SXD-01 — cachedFiles bridge member comparison claim

- Date/time: claim 2026-10-19 23:29 CDT; completion 2026-10-19 23:35 CDT (America/Chicago).
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
## Completion checkpoint

- Agent ID: `/root/data02_bridge_sxd`; integrator verification: `/root`.
- Private artifacts only: `local/validation/data02-bridge-sxd-01.py`, `.commands.txt`, `.json`,
  `-run1.log`, `-run2.log`, and `-root.log`. No shared source or game bytes were published.
- Worker commands: `python3 -I local/validation/data02-bridge-sxd-01.py` redirected separately to
  `run1.log` and `run2.log`; `cmp ...-run1.log ...-run2.log` passed; `shasum -a 256` on JSON and
  both logs gave `0340b13e1c0bc1f4baf1ae2cb1194a23bce23b57edbfc85447adee024eaf90b3` for each.
- Root independently ran `python3 -I local/validation/data02-bridge-sxd-01.py` to `-root.log`;
  `cmp ...-root.log ...-run1.log` passed; `shasum -a 256` on JSON, root log and run1 log gave the
  same digest above.
- Observation: base `cachedFiles.psarc` is 37,159,164 bytes, SHA-256
  `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`; entry 40079 has stored span
  `[28218302,28218921)`, decoded length 619 and SHA-256
  `81a635460b7da1979052478b2e7324e1cdef0e3402c096d22565d03b9790d2b8`. Patch archive is
  37,274,125 bytes, SHA-256
  `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`; entry 40079 has stored span
  `[29830538,29831176)`, decoded length 638 and SHA-256
  `0dd7a8f06d54b471e06dd9cd4f25fe3db904c3497d47daeb5deaa7c1c122bd78`. Both begin with observed
  `fbad9897`. Of 619 aligned offsets, 152 are equal and 467 differ; there are 29 equal and 29
  differing runs, and the patch has a 19-byte suffix.
- Inference/limits: this comparison proves locator/hash identities and byte differences only. Two
  examples do not establish a schema, record boundaries, codec, reader behavior, overlay meaning,
  or game use. One initial report-construction defect duplicated boolean-run categories; it was
  corrected and final repeated output matches. Failed tries: 1 report-construction defect; 0 failed
  schema hypotheses.
- Exact next action: finish CPU-01-CTOR10-ANALYSIS-01; later locate and identity-check a third,
  independently named cachedFiles `.sxd` with the same prefix before any schema claim.
- Task status: ARTIFACT_VERIFIED; DATA-02 remains ACTIVE. Root integrates and publishes this record.
