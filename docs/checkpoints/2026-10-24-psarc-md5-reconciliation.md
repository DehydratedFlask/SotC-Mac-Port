# PSARC MD5 member reconciliation — 2026-10-24

- Agent: Codex `/root/assets`; task `DATA-02-PSARC-MD5-RECONCILE-01`.
- Local checkpoint: 2026-10-24 01:44 CDT. Root owns shared HANDOFF/TASKS/EVIDENCE integration.
- Status: **FIXTURE_VERIFIED** implementation; **ARTIFACT_VERIFIED** live replay, exit 0.
  No game gate is promoted.
- Attempts: one successful implementation/reconciliation attempt, zero failed hypotheses.
  Two intentional test-first red runs demonstrate the non-string lookup defect and old XFF
  report attribution before their fixes; these are not unsuccessful recovery hypotheses.

## Changes and semantics

Inherited uncommitted MD5 member-pairing work in `scripts/psarc.py`,
`scripts/classify_audio_headers.py`, `scripts/audio_bank_header.py` and their three test files
was independently reviewed and retained. `Psarc.members()` maps exact manifest paths to real TOC
IDs, and `entry_index()` performs exact lookup. The member key is MD5 of exact UTF-8 spelling;
there is no case folding, Unicode normalization, slash insertion/collapse, dot-segment removal,
or whitespace stripping. Pairing does not rely on input order. Missing/duplicate keys and
multiple paths claiming an entry are errors. Added regressions verify these properties and
cache isolation; non-string lookup now raises a descriptive `PsarcError`.

The legacy-named XFF module retains its parser/error symbols as generic asset tooling. It
requires an explicit member identity manifest; reports use `DATA-02-XFF-PREFIX-01` and
`generic_xff_prefix`. The old XFF audio-bank attribution is withdrawn. The admitted structure
check still accepts only complete `xff\0` data with u32le@0x14 equal to the buffer length;
opaque bytes and codec/runtime meaning remain unknown.

`docs/ASSETS.md` now records corrected metadata and explicitly withdraws the old path-specific
audio/STS/bridge conclusions. Historical private manifests and validation artifacts are
preserved. New corrected inputs remain in `local/manifests/assets-md5pairing-r30/`.

## Exact checks and private evidence

From workspace root:

```sh
python3 -I -W error local/orchestrator/r30/assets-run-tests.py
python3 -I -W error local/orchestrator/r30/assets-validate.py
python3 -I -W error tools/dev/gpu_asset_probe.py csdr local/extracted/CUSA08809/pak0.psarc --path 'materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr'
```

- Python suite: **PASSED 10/10 script files, 228/228 tests**, each run in its own `-I -W error`
  process. Counts: PSARC52, audio classifier28, generic XFF25, overlap24, rackquery args12,
  rackquery CFG20, publish12, attribution13, project tools12, dynlink30.
  Aggregate `local/orchestrator/r30/assets-python-tests.{json,log}`;
  individual `assets-test_*.log`. Final targeted PSARC recheck also exits 0.
- Live replay: `local/orchestrator/r30/assets-validate.py`, output `assets-validation.log` and,
  on success, `assets-validation.json`. It independently recomputes path MD5 values, verifies
  all corrected manifest row fields and archive hashes, compares every base/patch shared member,
  reclassifies the 86 audio-path members, resolves historical XFF IDs to actual paths, and checks
  that historical manifest hashes remain unchanged. All 7 archive hashes and all 170451 path/TOC
  pairs pass; the old positional pairing matches only 3 rows total. All 40213 shared cachedFiles
  pairs are byte-identical (66897142 decoded bytes compared across both layers). The corrected
  audio manifest reproduces exactly: 86 members, 2073162522 decoded bytes, zero XFF prefixes.
  All 44 former XFF observations retain their hashes/length relation, but map to 18 `.anb`,
  15 `.nmo`, 5 `.clb`, 4 `.nto`, and 2 `.skb` members. Final report SHA256
  `2cb20dd0dfc371231004a4218731f77b0be53def80260480fa110694fa0aabc3`;
  source/tool/output identities are in `assets-identities.json` (Python 3.15.0).
- Supplemental corrected sample identities: `assets-corrected-samples.json`. Actual selected
  STS member is 120/120 B, IDs13300/13302, identical decoded hash
  `0c3a40c38366dacda236c4f22c82434cc591ce3c1109b6ea6c60bdb13b607687`.
  Actual bridge.sxd is 144/144 B at ID2581, identical hash
  `9c288d5c8f9836ca8bc44676db159212b4b48e6fdf229e9d388cc7d3cc90a75e`.
- Default raw shader probe: **PASSED**, 1988 B, SHA256
  `c4214e4ee0852a962ce56e0cab0befbe3169698cb354bc62afa7eab4d1f3f356`,
  VS code@564 length272 info@972; PS code@1368 length240 info@1744.
  Log `assets-default-shader-probe.log`. No broad shader scan or GPU execution.
- Scoped `git diff --check` exits 0. No native/CMake, runtime, or publication check was claimed by
  this worker. No install/dependency change; no reference clone edits; no payload extraction.

## Corrected metadata margins (ARTIFACT_VERIFIED)

The seven corrected cached manifests have 170451 rows and 40216 shared-path pair occurrences
across 21 pairs. All same-path TOC keys agree. The corrected size/key cross-tab is
**40215 equal/equal, 1 equal-key/different-size, 0 different-key/equal-size, 0 different/both**.
There are 130235 distinct exact paths and path keys, zero path keys spanning different exact
paths, and 40216 repeated exact-path/key associations covering 80432 rows. E75's path-only zero
collision conclusions survive. Old E77/E123/E126/E239 margins were metadata-pairing artifacts.
Full proposed E55/E75/E77/E123/E126/E239 and task-status replacement text is in
`local/orchestrator/r30/assets-report.md`; root integrates that text without rewriting old
hash-bound private reports.

## Remaining risk and next exact action

The old default private manifests still exist for evidence preservation and must not be used
as corrected path/TOC identities. Root must reconcile shared evidence/task rows and
regenerate the project index. Publication and
native full-suite checks remain root-owned. Format/codec/loader semantics, selected-scene asset
resolution, game audio, Metal game frames, and save/load remain open. G1 PARTIAL; G2-G6 NOT RUN.
