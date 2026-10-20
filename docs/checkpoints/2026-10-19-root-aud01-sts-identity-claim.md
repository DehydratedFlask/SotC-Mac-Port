# AUD-01-STS-IDENTITY-01 — paired cachedFiles test_adpcm identity survey claim

- Time / timezone: 2026-10-19 23:50 CDT (America/Chicago).
- Agent and integrator: assigned `/root/aud01_sts_cp3_identity`; integrator `/root`.
- Task / gate / status: AUD-01-STS-IDENTITY-01 under AUD-01; CLAIMED, fixture/inventory only.
- Prerequisite evidence / input and source hashes: private member manifests report 290 exact paths
  in each cachedFiles layer. Inputs are `local/extracted/CUSA08809/cachedFiles.psarc` and
  `local/extracted/CUSA08809-patch/cachedfiles.psarc`; manifests are
  `local/manifests/assets/CUSA08809-cachedFiles.members.jsonl` and
  `local/manifests/assets/CUSA08809-patch-cachedfiles.members.jsonl`. `Psarc.read()` and member
  enumeration were located with Graft in `scripts/psarc.py`; live inputs must be revalidated.
- Owned / changed files: worker owns only new private output under `local/validation/`; root owns
  `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No archive, extracted game asset, or source
  file may be modified.
- Hypothesis and expected result: exact-path membership is paired across both manifests. The
  worker will independently read each of the 580 small members, verify entry ID/offset/size against
  its layer manifest, compute decoded-member SHA-256, and report exact-path size/hash equality and
  size distributions. Expected output is a reproducible inventory; extension and directory names
  do not establish codec, schema, gameplay use, or playback.
- Exact command, CWD and nonsecret environment: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sts-identity-01.py`
  from the repository root, with stdout captured to private run logs. No installs or downloads.
- Exit / observed result: both worker runs and root rerun exited 0. Worker run1/run2 logs compare
  byte-identically; root log compares byte-identically with worker run1. All archive and member
  manifest checks passed.
- Log, capture and REA evidence paths/IDs: private script/report/commands note plus
  `local/validation/aud01-sts-identity-01-run1.log`, `-run2.log`, and `-root.log`; all remain under
  ignored `local/validation/`. Report SHA-256 is
  `566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`; each run log SHA-256 is
  `bac56189a581fa1267881834679e98a302bf5a23bc18ac6a53e9f745c4171974`.
- Observation versus inference / coverage limitations: root reviewed the private script and
  independently ran `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sts-identity-01.py`
  from the repository root with stdout/stderr redirected to the private root log; exit 0 and output
  exactly matched run1. Both archive identities matched
  the private summaries before and after member reads. All 290 target members per layer had live
  ID/path/offset/first-block/decoded-size values matching their manifests. Path intersection 290;
  base-only 0; patch-only 0; same decoded SHA-256 0; different 290; same size 5; changed sizes
  285. Byte totals were 242,310 base and 241,080 patch; both layers range 0–3,060 bytes, with 190
  and 189 distinct decoded sizes. These observations do not identify format or gameplay use.
- Cumulative failed tries and distinct hypotheses: 0; no implementation hypothesis failed.
- Remaining risks / blockers: codec, member schema, reader, runtime selection, NGS2 behavior,
  gameplay use, and playback remain unknown.
- Next exact action: continue AUD-01 by recovering the game's own reader through the open RE-02/Fios/ULT
  work; do not infer ADPCM from filenames or byte differences.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE E44 and
  ASSETS updated; INSTALL_LOG unchanged because no dependency or download changed; folder count
  unchanged, project index refreshed before publication.

## Completion checkpoint — 2026-10-19 23:54 CDT

- Agent: `/root/aud01_sts_cp3_identity`; integrator verification: `/root`.
- Exact worker commands, hashes, and output records are preserved in the private commands note
  `local/validation/aud01-sts-identity-01-commands.md`; worker run1/run2 and root log SHA-256 all
  equal `bac56189a581fa1267881834679e98a302bf5a23bc18ac6a53e9f745c4171974`.
- Root-owned documentation completion is published separately from private game-derived files.
- Status: FIXTURE_VERIFIED for identity comparison only; parent AUD-01 remains ACTIVE; G1 remains
  NOT PASSED and G2–G6 remain NOT RUN.
