# E789 — AUD-01 music/BGM path census and attribution checkpoint

- **Date/time:** 2026-10-27, 09:24 CDT.
- **Agent/task:** `/root`; `AUD-01-MUSIC-BGM-PATH-CENSUS-01`, with PUB-01 attribution closeout context.
- **Task claim:** recorded at the top of `HANDOFF.md` before editing the other shared documents.
- **Input:** corrected private manifest `local/manifests/assets-md5pairing-r30/audio-headers.json`, SHA-256 `4e681a1242e30f9f47a3df42ad933f28bdbd4dbad9bade8c578be7a231374470`.
- **Exact root command:** `python3 -I local/validation/orch-20261027/aud01-music-bgm-census/query_music_bgm_paths.py`. The private query script SHA-256 is `041cb72d54810f2ca7b2231b3c747401510bef5ce780ed5690874949830cc240`; output log SHA-256 is `753acd073a9f7a9b566107b2923f4a47e936839beed334602c5c8d0816775156`. It asserts `len(members) == member_count == 86`, then applies case-insensitive literal-token searches to `row['path']` for the five tokens. The command reads JSON metadata only and emits each hit's archive, TOC entry ID, decoded size, and exact path.
- **Root result:** `music` 2; `bgm` 2; `dialog` 0; `voice` 0; `stream` 0. Four hits, all in `pakN.psarc`:

| TOC entry | Decoded size | Path |
|---:|---:|---|
| 27715 | 9,872 B | `sound/shadow/music.sxd1` |
| 37843 | 172,367,136 B | `sound/shadow/music.sxd2` |
| 61642 | 2,461,232 B | `sound/shadow/str_bgm.sxd1` |
| 17150 | 514,125,344 B | `sound/shadow/str_bgm.sxd2` |

- **Independent review:** `/root/aud_music_census_review` read only the same corrected JSON manifest and reported matching counts and candidates. No archive/member payload was read by root or reviewer; the inventory script was not rerun.
- **Observation vs inference:** paths and TOC locators identify only names in the corrected manifest. No content format, codec, dialogue coverage, engine routing, or playback behavior is established. AUD-01 and game-audio gates remain open.
- **Historical correction:** E34's 3,726,760-byte replay is now labeled as a historical result from mispaired positional locators, withdrawn for path-specific claims; E621's corrected 2,073,162,522-byte result remains authoritative. The duplicate E787 evidence row was removed.
- **Public attribution evidence:** the latest fetched-ref scan at `e9be553af6f0136c69701501dc38d535fbe46bbe` covers 9 GitHub branch heads, 0 tags, and 2,400 reachable commits with zero Claude/Anthropic matches. Report SHA-256 `2844551c2cd9b7f5061a20a66672b31810bcc6c116f0d5d7494297ac74fda48c`. The current checkpoint commit will be separately checked after creation. The 70 stash-only matches remain unchanged per user choice; `--all` continues to find only unrelated non-Claude `orchestrator@local` commit `f6604a86e60e`.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/ASSETS.md`, `docs/EVIDENCE.md`, `docs/ATTRIBUTION.md`, `docs/INDEX.md`, this checkpoint, and generated folder index if its inventory changes.
- **Verification:** root census passed and matches the independent review. CMake configure/build passed; CTest **431/431**; lift checkers **516 passed / 0 skipped / 0 failed**; `trail build --check` passed at 9,876 nodes / 13,469 edges (deep meaning layer not built); project index generation and `--check` passed at 33,133 folders. Transient `launcher/.build` index churn was excluded and only the durable `docs/checkpoints` count advanced 820→821 in `docs/FOLDERS.md`. Staged/full path audits passed **8/8** and **3,017/3,017**; cached diff check, owner identity, and default attribution range passed. `check_commit_attribution.py --all` remains **FAILED** only on historical non-Claude `orchestrator@local` commit `f6604a86e60e`. Prepublication log-manifest SHA-256 `e6d806e6c43c90a44531ef07af275415dcd5186b4ee8bed9075f91664352f9a8`; logs are under `local/validation/orch-20261027/e789-publication/`. Commit/push and post-commit checks are pending.
- **Remaining risks:** no game-content, audio-engine, dialogue-location, playback, or G2/G5 behavior claim follows from this census. Full native-port acceptance remains incomplete.
- **Next exact action:** commit the eight staged allowlisted docs as the owner, push `origin main`, fetch, audit the new commit's exact range/identity/path set, and scan fetched refs.
- **Task status / attempts:** `AUD-01-MUSIC-BGM-PATH-CENSUS-01` **ARTIFACT_VERIFIED**, 0 failed hypotheses; publication pending.
