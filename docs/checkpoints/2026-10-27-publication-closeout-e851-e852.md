# E852 — E851 publication closeout and refreshed public attribution

**Date:** 2026-10-27 18:36 CDT
**Integrator:** `/root`
**Task IDs:** `PUB-01-CLAUDE-CREDIT-CLEANUP-01`; `AUD-01-PAKN-MUSIC-SXD1-BYTE-STRUCTURE-01`

## E851 publication

E851 commit `05e1d7ebe71f91967c46866c88009ecb8d245c52` (parent `58f93e7ab084d5bfcbad98a2a2921c9f800bbc19`) is authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. Its exact-range attribution and identity checks, six-path staged audit, 3,068-path committed-tree audit, and whitespace checks passed. It was pushed to `origin main`; fetch verified `HEAD == origin/main == FETCH_HEAD == 05e1d7ebe71f91967c46866c88009ecb8d245c52`. Logs are under `local/validation/orch-20261027/e851/`.

The bounded `AUD-01-PAKN-MUSIC-SXD1-BYTE-STRUCTURE-01` task is `ARTIFACT_VERIFIED` for its private byte profile. Root and independent verifier hashes, exact archive/member identities, and all 23 replay comparisons are recorded in the [E851 profile checkpoint](2026-10-27-aud01-music-sxd1-profile-e851.md). This does not identify format/codec or prove game usage or playback.

## Current GitHub attribution census

Exact command: `python3 -I local/validation/orch-20261027/e852/attribution/scan_advertised_refs_and_stash_e852.py`. The read-only scan covers 21 live advertised refs (9 branches, 12 pull-request heads, 0 tags), 15 unique tips, and 2,464 reachable commits with **0 Claude/Anthropic author, committer, or credit-line matches**. `HEAD`, `origin/main`, and live advertised `main` agree at `05e1d7ebe71f91967c46866c88009ecb8d245c52`.

The user chose to leave private stashes unchanged: 21 reflog tips, 826 commits, 70 matches; `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`. This census covers current advertised refs and stash reflog roots, not unreachable objects. Scanner SHA-256 `cedca20f3333742f8f86bf54da0c58d84cd4637ad9e4932a5a5b494954d749d8`; report SHA-256 `c6771aebf6ee5f575d34c664917c96d39155c7937ad0ca1b000daa0ae0b0add3`; log SHA-256 `a67c6fb43068ecefce1bed8760cb1e6fc5df832fcaf2e1968f29a5df076025a3`.

## E851 project-index correction

After the E851 checkpoint publication, the first final `python3 scripts/project_index.py --check` again found `local/indexes/folders.json` stale because a new private status log path had been created after generation. Regenerate and check after all E852 docs and private validation paths exist; the final check must be the last filesystem-inventory check and must not create a new path under the indexed tree afterward. The initial stale output is captured in `local/validation/orch-20261027/e851/e851-project-index-check.log`.

## E852 repository gates and publication

- `cmake -S native -B local/build/orch-r22` — exit 0; `local/validation/orch-20261027/e852/e852-cmake-configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0; Ninja had no work; `local/validation/orch-20261027/e852/e852-cmake-build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — **PASSED 431/431**; `local/validation/orch-20261027/e852/e852-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — **PASSED 516 / 0 skipped / 0 failed**; `local/validation/orch-20261027/e852/e852-lift-checkers.log`.
- `trail build --check` — **PASSED**, wiring graph in sync at 9,877 nodes; deep layer unbuilt; `local/validation/orch-20261027/e852/e852-trail.log`.
- `python3 scripts/project_index.py` — exit 0; 31,984 folders indexed; `local/validation/orch-20261027/e852/e852-project-index-generate.log`.
- `python3 scripts/project_index.py --check` — exit 0; 31,984 folders current; evidence index is `local/indexes/folders.json`. The first E851 check after publication was stale due a newly created local status-log path; the E852 full inventory regeneration resolves it.
- `python3 -I scripts/audit_publish_paths.py` — **PASSED 6/6 staged paths**; `local/validation/orch-20261027/e852/e852-audit-staged.log`.
- `python3 -I scripts/audit_publish_paths.py --all` — **PASSED 3,069/3,069 tracked paths**; `local/validation/orch-20261027/e852/e852-audit-all.log`.
- `git diff --cached --check` and `git diff --check` — exit 0; logs `e852-staged-diff-check.log` and `e852-diff-check.log`.
- `python3 -I scripts/check_commit_attribution.py --identity` and `--range HEAD^..HEAD` — exit 0 for owner identity and E851; `e852-identity.log` and `e852-precommit-attribution.log`.
- Precommit `git fetch origin main` — exit 0; `HEAD == origin/main == FETCH_HEAD == 05e1d7ebe71f91967c46866c88009ecb8d245c52`; `e852-fetch-before.log`.

No source or dependency change is part of this docs-only closeout. E852's own commit, exact-range check, push, and fetch verification are pending.

## Observation and next action

- **Observation:** all currently advertised GitHub commits scanned after E851 have zero Claude/Anthropic identity or credit-line matches; the 70 private-stash matches remain untouched by owner choice.
- **Inference boundary:** public attribution cleanup is verified only for the current advertised refs. Unreachable Git objects are not included; private stashes were explicitly excluded by the user.
- **Remaining risks:** full game boot, audible content, real save/load, Metal validation, and forbidden-runtime absence have not been newly established. Audio format, codec, NGS2 contract, game use, routing, and playback remain unknown.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this closeout checkpoint; private evidence remains ignored under `local/validation/`.
- **Next exact action:** finish E852 gates and final project-index check, audit only the six allowlisted docs paths, commit under Johnny Wang, exact-range audit, push `origin main`, fetch-verify, and refresh live advertised-ref attribution.
