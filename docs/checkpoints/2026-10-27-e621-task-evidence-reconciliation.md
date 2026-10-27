# E621 task and evidence reconciliation — 2026-10-27

- **Agent:** `/root`; task `DOC-01-E621-SUPERSEDED-METADATA-CLAIMS-01`.
- **Starting checkpoint:** claim `97fca8449e1067d6136012656013a2c4cbedfb19` was pushed to `origin/main`; private stashes remain unchanged by owner choice.
- **Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint.
- **Status:** `ARTIFACT_VERIFIED` documentation correction; 0 failed hypotheses. Publication path checks and push are the remaining checkpoint steps. No native-game gate is promoted.
- **Scope:** reconcile E621-superseded metadata summaries in `docs/TASKS.md` and `docs/EVIDENCE.md` with the live corrected evidence in `docs/ASSETS.md` and `docs/checkpoints/2026-10-24-psarc-md5-reconciliation.md`.

## Corrections recorded

- DATA-02 overlap and value/size summaries now use E621's 40,215 equal/equal, 1 equal-path-key/different-size, 0 different-key/equal-size, and 0 different/different cells across 40,216 shared-path pair occurrences. Earlier E77/E123/E239 size/value margins remain historical and are superseded.
- DATA-02's bridge summary now points to the corrected `sound/shadow/bridge.sxd` base/patch member at TOC ID 2,581: 144 B each and byte-identical.
- AUD-01's former XFF observations are labeled generic prefix/length structure on corrected non-audio members. They do not establish audio content or decoding.
- The sidecar status now distinguishes the repeatable historical report from its superseded pre-E621 path/TOC assignments. E621 confirms the three shared cachedFiles path pairs are identical; the three pakN sidecar hashes have not been revalidated at corrected locators.
- The STS summary and E44 now say 290 exact `.sts_cp3` paths per cachedFiles layer are 120 B and byte-identical across base/patch. The selected `b_info_01.sts_cp3` pair is at TOC IDs 13,300/13,302 with shared SHA-256 `0c3a40c38366dacda236c4f22c82434cc591ce3c1109b6ea6c60bdb13b607687`. The former differing-hash/size counts are withdrawn.
- The bridge summary and E42 now identify the corrected 144-byte identical pair and mark the old entry-40079 offsets (619 aligned bytes, 152 equal / 467 different, 19-byte patch suffix) as misattributed and withdrawn.

## Evidence boundaries

E621 reports all 7 archive identities and 170,451 corrected path/TOC pairings and row fields verified. Its live replay compared 40,213 shared cachedFiles members (66,897,142 decoded bytes), all byte-identical. The corrected audio inventory has 86 rows / 2,073,162,522 decoded bytes and codecs UNKNOWN. No game archive/member payload was inspected to prepare these corrections; the independent reviewer read only the cited documentation. Required CTest/lift publication gates are repository checks and do not establish game runtime behavior.

These observations establish no `.sxd`/`.sts_cp3` codec or schema, bridge reader, NGS2 contract, XFF audio meaning, loader/mount/override behavior, dialogue coverage, selected-scene resolution, or runtime audio routing/playback. No boot, audio, saving, or Metal gate changes.

## Verification and publication

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **PASSED 431/431**, 8.29 s.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **PASSED 516 / 0 skipped / 0 failed**.
- `python3 -I scripts/audit_publish_paths.py`: staged **PASSED 6/6** for the complete checkpoint; after adding the scan record, the amended staged delta passed **2/2**. `python3 -I scripts/audit_publish_paths.py --all`: full **PASSED 3,032/3,032**. `git diff --cached --check` exits 0.
- `python3 -I scripts/check_commit_attribution.py`: **PASSED** on the final unpushed owner-authored checkpoint commit in `origin/main..HEAD`; see `commit-attribution-final-range.log`. The commit author and committer are Johnny Wang `<dehydratedflask@users.noreply.github.com>`.
- The targeted advertised-ref scan covers **21 GitHub refs** (9 branches, 12 pull-request heads, 0 tags) plus the local final-commit candidate: **2,423 unique commits**, 0 Claude/Anthropic author/committer identities, and 0 Claude/Anthropic contributor/signature hits. Stashes were excluded per owner choice. Logs/scripts: `advertised-ref-claude-scan.json`, `scan_claude_attribution.py`, and `advertised-ref-fetch.log` in the private folder above. At scan time remote `main` was the pushed claim checkpoint; the local candidate was one owner-authored doc commit ahead.
- **Scope-mismatch diagnostic:** an exploratory scan using the repository-wide `commit_problems()` policy returned **1,070** commits because that policy also rejects Codex/OpenAI and other assistant/agent aliases. This is broader than the user's Claude-only request and is not a Claude/Anthropic hit count. Its output is preserved as `advertised-ref-attribution-scan.json`; the targeted scan above is the relevant result.
- `python3 scripts/project_index.py`: exit 0, **33,160 folders indexed**. `python3 scripts/project_index.py --check` after restoring volatile launcher `.build` rows reports `Stale indexes: docs/FOLDERS.md, local/indexes/folders.json`; `docs/FOLDERS.md` retains the intentional `docs/checkpoints` count change from 835 to 836. Generated transient-row diff is saved privately.
- **Private logs:** `local/validation/orch-20261027/doc-e621-final/`.
- **Index behavior:** the `docs/FOLDERS.md` difference is limited to the new checkpoint count; volatile launcher `.build` rows are intentionally discarded and the strict check remains stale as recorded above.
- **Remaining risks:** the pakN sidecar sample hashes are not revalidated at corrected locators; all file-format, runtime, audio, save/load, and Metal unknowns above remain open.
- The exact-range `python3 -I scripts/check_commit_attribution.py` audit is rerun against the final owner-authored doc commit before push; its result and commit identity are saved in the private log folder.
- **Next exact action:** push the verified owner-authored documentation checkpoint to `origin main` as a fast-forward and fetch to verify the tip; then resume from the native-port blockers. Overall native-port goal remains active and incomplete.
