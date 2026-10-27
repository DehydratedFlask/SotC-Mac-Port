# E851 — Verify private music SXD1 byte profile and refresh live attribution

**Date:** 2026-10-27 18:32 CDT
**Integrator:** `/root`
**Task IDs:** `AUD-01-PAKN-MUSIC-SXD1-BYTE-STRUCTURE-01`; `PUB-01-CLAUDE-CREDIT-CLEANUP-01`

## E850 checkpoint publication

E850 claim commit `58f93e7ab084d5bfcbad98a2a2921c9f800bbc19` (parent `ce02512e76c3b6d80d3604d40b96ef63d6c7bfb5`) is authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. Its exact-range attribution check, identity check, committed-tree path audit, and diff check passed. It was pushed to `origin main`; fetch verified `HEAD == origin/main == FETCH_HEAD == 58f93e7ab084d5bfcbad98a2a2921c9f800bbc19`. Logs are under `local/validation/orch-20261027/e850/`.

## Live GitHub attribution census

Exact command: `python3 -I local/validation/orch-20261027/e851/attribution/scan_advertised_refs_and_stash_e851.py`. It scanned 21 live advertised refs (9 branches, 12 pull-request heads, 0 tags), 15 unique tips, and 2,463 reachable commits. Claude/Anthropic author, committer, or credit-line matches: **0**. `HEAD`, `origin/main`, and live advertised `main` agree at `58f93e7ab084d5bfcbad98a2a2921c9f800bbc19`.

The private stash census remains 21 reflog tips / 826 reachable commits / 70 Claude/Anthropic matches. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`, unchanged per owner direction. The scan covers current advertised refs and stash reflog roots, not unreachable objects. Scanner SHA-256 `2f9b69d3609bdeb5353c69386206859c681a03ebead5e9ae35da4b4c8544a70b`; report SHA-256 `c020f56f431681a28dbe80ff3fa0cb073daab8f803273a0d85075e5060120d2d`; log SHA-256 `953475ead837bc1079c35026096f5780817183a60d3b9d03db89357d349cdc87`.

## Private `music.sxd1` byte profile

**Status:** `ARTIFACT_VERIFIED` for this bounded profile only; parent AUD-01 and real-game audio remain open. The exact member `sound/shadow/music.sxd1` resolved to TOC entry 27715, 9,872 decoded bytes, and member SHA-256 `b925ca03cb6dddf8e9d3448c24a2e2eeb0ac595067cd3bbf25087ec3e6692a94`. The live 12,838,532,222-byte archive rehashed to `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`; the corrected manifest, summary, and audio-header manifest all matched their pinned hashes.

The root verifier `local/validation/orch-20261027/e850/aud01-music-sxd1-structure/profile_music_sxd1.py` (SHA-256 `304c2b737c7dd4ee1197082408599eadf950e3c7fa679a9ca0334bfca3538cad`) produced private report `profile.json` (SHA-256 `5d197c01af7d87ea091bbd4227ec401d99a349460f61f7bbed696b27aaed86ef`). A separately written standard-library PSARC parser/zlib decoder independently rehashed the archive and member, recomputed the exact block and byte profile, and matched all **23** compared fields with no mismatches. Exact replay command: `python3 -I local/validation/orch-20261027/e850/aud01-music-sxd1-structure/replay-review/run_independent_replay.py` (exit 0). Replay script SHA-256 `96341c00706436c004b8f18f794b95028b72a9d90609a3a066d63075a1faa61f`; private report SHA-256 `89fe8563597b5157c832c0e1e6de569e438698f20f8763b2b47bd7e3e6c8a851`; log SHA-256 `d90bf9af08fed39484847068a6fbbedc065cbcc8dad6b50bc96088e84a506200`.

No decoded payload was written. The first 32 bytes, byte histogram, entropy, and printable-run locations remain in private ignored reports only. This verifies the pinned container/member identity and declared byte statistics; it does not identify the SXD format or codec or establish NGS2 behavior, game use, routing, playback, or any real-game audio gate.

## E851 repository gates

- `cmake -S native -B local/build/orch-r22` — exit 0; `local/validation/orch-20261027/e851-cmake-configure.log`.
- `cmake --build local/build/orch-r22 -j 12` — exit 0; no work; `local/validation/orch-20261027/e851-cmake-build.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — **PASSED 431/431**; `local/validation/orch-20261027/e851-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — **PASSED 516 / 0 skipped / 0 failed**; `local/validation/orch-20261027/e851-lift-checkers.log`.
- `trail build --check` — **PASSED**, wiring graph in sync at 9,877 nodes; deep layer unbuilt; `local/validation/orch-20261027/e851-trail.log`.
- The first staged diff check found trailing hard-break spaces on the date and integrator lines in this checkpoint; those spaces were removed. The failed output is preserved at `local/validation/orch-20261027/e851/e851-staged-diff-check-first-fail.log`; the final staged check must pass.
- Project-index check first reported stale `local/indexes/folders.json` after the E851 private validation logs were created; this was an ordering issue, not a source failure. Regenerate the index after all checkpoint files and local logs exist, then run the final `--check` without writing its output under the indexed repository.

No source or dependency changed. Private scripts, reports, and logs remain under ignored `local/validation/`.

## Observation, limits, and next action

- **Observation:** the independent decoder matched all 23 reported fields and both independently computed content hashes. The live advertised-ref census contains zero Claude/Anthropic matches across all 2,463 reachable commits.
- **Inference boundary:** byte-level profile data does not show that the game opens or plays this asset. The format, codec, semantic records, game call sites, routing, and actual audio remain unknown.
- **Remaining risks:** the 70 private-stash matches remain untouched by the owner’s choice; unreachable Git objects are outside the census; full game boot/audio/save/Metal acceptance remains unmet.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. Evidence artifacts remain private under `local/validation/`.
- **Next exact action:** regenerate and check the project index after final docs/audit logs exist; run staged/full publication-path audits, owner-identity and diff checks; commit only the six allowlisted documentation paths under Johnny Wang; exact-range audit, push `origin main`, fetch-verify, and refresh live attribution for the pushed checkpoint.
