# E850 — Current attribution census and music SXD1 profile claim

**Date:** 2026-10-27 18:17 CDT
**Integrator:** `/root`
**Task IDs:** `PUB-01-CLAUDE-CREDIT-CLEANUP-01`; `AUD-01-PAKN-MUSIC-SXD1-BYTE-STRUCTURE-01`

## Current publication and attribution state

E849 commit `ce02512e76c3b6d80d3604d40b96ef63d6c7bfb5` (parent `d763a2eb71d24f2497a39a9918d9f147f9e14cb1`) is authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. A fresh exact-range attribution check and identity check passed; fetch confirmed `HEAD == origin/main == FETCH_HEAD == ce02512e76c3b6d80d3604d40b96ef63d6c7bfb5`.

Exact command: `python3 -I local/validation/orch-20261027/e850/attribution/scan_advertised_refs_and_stash_e850.py`. The read-only scanner (SHA-256 `ad27b7ce98819d33848a43cd277079f50d344d04f8e0a2d495b9939f0390ef40`) queried 21 advertised refs (9 branches, 12 pull-request heads, 0 tags), 15 unique tips and **2,462 reachable commits**. Claude/Anthropic author/committer identity and credit-line matches: **0**. Report SHA-256 `0ab934876467a9fcbec6fbc27e1249ef2fcf57e6abfa8d531ee247f33529f74e`; log SHA-256 `c1507d633bd6ef085e3318ca87f8cd813a5b884fbe48aac3614241f5c7f630a5`.

The same read-only scan found 21 stash-reflog tips, 826 reachable commits and 70 matches. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; it was not modified by owner choice. Unreachable objects outside the advertised refs and scanned stash reflog are not covered.

## Bounded AUD-01 claim

**Status:** `CLAIMED`; 0 failed attempts. This is a private, read-only in-memory byte-structure probe of exactly `sound/shadow/music.sxd1` from the local `pakN.psarc`.

Pinned metadata currently in the corrected local manifests:

| Field | Expected value |
|---|---|
| Archive bytes | 12,838,532,222 |
| Archive SHA-256 | `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73` |
| Corrected manifest SHA-256 | `4f7fba6bb85aa29712f502aa0c97ac0daa604dbe9766161d0f71f665b7c290ec` |
| Member path / TOC ID | `sound/shadow/music.sxd1` / `27715` |
| TOC offset / first block | `569314592` / `13967` |
| Decoded size / member SHA-256 | `9872` B / `b925ca03cb6dddf8e9d3448c24a2e2eeb0ac595067cd3bbf25087ec3e6692a94` |
| TOC MD5(path) | `6ec2a9049170e8701b16e2dd63bb9f0d` |

The existing 86-row audio manifest labels the target's leading bytes `other-53584446`; this is a byte-prefix label, not a format or codec identification. The fixed profile output will include: (1) exact archive/manifest/member identity; (2) each PSARC block's TOC block index, stored size and expected decoded length; (3) the first 32 member bytes as hex in the private report only; (4) full-member byte-frequency counts, NUL and printable-ASCII counts, Shannon entropy, and maximal printable-run offset/length pairs (never run contents). An independent replay must match every field. It will not write a decoded member or infer SXD record meanings, codec, NGS2 contract, game use, routing, or playback. AUD-01 and actual game audio remain incomplete.

## Repository verification

- `cmake -S native -B local/build/orch-r22` — exit 0; `e850-cmake-configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0; `e850-cmake-build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431 passed; `e850-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed / 0 skipped / 0 failed; `e850-lift-checkers.log`.
- `trail build --check` — exit 0; wiring graph current at 9,877 nodes; deep layer unbuilt; `e850-trail.log`.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — exit 0; 31,979 folders current; `e850-project-index-generate.log` and `e850-project-index-check.log`.
- `python3 -I scripts/audit_publish_paths.py` — exit 0; staged allowlist 6/6; `e850-audit-staged.log`.
- `python3 -I scripts/audit_publish_paths.py --all` — exit 0; tracked allowlist 3,067/3,067; `e850-audit-all.log`.
- `git diff --cached --check` — exit 0; `e850-staged-diff-check.log`. First run exited 2 on two Markdown hard-break spaces; failed output is preserved at `e850-staged-diff-check-first-fail.log`. Worktree `git diff --check` passed; `e850-diff-check.log`.
- `python3 -I scripts/check_commit_attribution.py --identity` — exit 0; `e850-identity.log`.
- `python3 -I scripts/check_commit_attribution.py --range HEAD^..HEAD` — exit 0 for E849; `e850-e849-exact-attribution.log`. E850's own range check remains postcommit.
- `git fetch origin main` — exit 0; `HEAD`, `origin/main`, and `FETCH_HEAD` equal `ce02512e76c3b6d80d3604d40b96ef63d6c7bfb5`; `e850-fetch-before.log`.
- Project-index check initially found stale `local/indexes/folders.json` after a new private log was added; `python3 scripts/project_index.py` regenerated it and a subsequent `--check` passed. The first failed check is preserved at `e850-project-index-check-first-fail.log`.
- E850's own exact range, postcommit path audit, push and fetch verification remain pending.

All command logs and the private census artifacts are under `local/validation/orch-20261027/e850/`. No source or dependency changed in this checkpoint.

## Observation, risks, and next action

- **Observation versus inference:** the scanner directly counted commits reachable from the 21 current GitHub refs and found zero Claude/Anthropic matches. The archive summary and corrected manifest contain the pinned member locator and hashes, but E850 has not yet rehashed the live 12.8 GB archive or read the member. The `SXDF`-like prefix label does not establish a schema.
- **Remaining risks:** all 70 Claude/Anthropic matches in the owner-excluded private stash remain unchanged. The SXD header/record structure, codec, game use, playback, and all real-game audio gates remain unknown.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint; private scan/gate files remain ignored under `local/validation/`.
- **Next exact action:** rerun the staged diff check after whitespace correction; finish identity/fetch/status checks; exact-range check after commit, then push and fetch-verify `main`. Only after the claim is published, verify the archive and manifest hashes, read TOC entry 27715 in memory for the fixed profile above, and obtain an independent replay/review.
