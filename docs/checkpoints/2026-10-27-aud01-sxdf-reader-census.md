# AUD-01 SXDF/SXDS saved-evidence census — E791

## Task and boundaries

- **Task:** `AUD-01-SXDF-READER-CANDIDATE-01`.
- **Result:** `STATIC_ANALYSIS_VERIFIED` for the saved-text evidence scope below. No reader or decoder candidate was found.
- **Scope:** read existing text exports under `local/re/evidence` and `local/re`, looking for case-insensitive `.sxd`, `SXDF`, and `SXDS` literals. The query did not open binary artifacts, import/reopen the ELF, execute original code, or read PSARC/member payloads.
- **Specification:** `docs/REVERSE_ENGINEERING.md` tool ownership and Stage 3; `docs/ASSETS.md` keeps codec/framing claims UNKNOWN.

## Exact query and evidence

Command: `python3 -I local/validation/orch-20261027/aud01-sxdf-reader-census/search_saved_evidence.py` (exit 0).

The scanner hash-binds the sorted set of 3,453 textual exports under `local/re`, including report, log, JSON, disassembly, and source-text suffixes up to 256 MiB. It scanned 3,453 files and skipped 0. Query script SHA-256: `48b0f699775bc4fd4d047205374f772086e4ac9a51d23be728d18a9cbb0305da`. Full result and per-file SHA-256 manifest: `local/validation/orch-20261027/aud01-sxdf-reader-census/search.json`, SHA-256 `925d11c79d63a7e9f24d4fc720d37abda2537247904e8a30ed385fa839f8e712`.

The raw case-insensitive substring scan found 91 occurrences:

- 89 `sXds` matches are inside the repeated NID `m0iS6jNsXds`. The canonical import record `local/re/imports.json` (SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`) resolves it to `libScePosix` / `libkernel` `sched_get_priority_min`, a single import site at `0x1600c58`. This is not an `SXDS` asset-format reference.
- Two `.sxd` matches are the FourCC row in `local/re/evidence/platform-tag-147e20/report.md` (SHA-256 `25188ca6ff641d99d84be314bd51710a7e7b525a51483c3726cd715cc7f3624c`) and `summary.json` (SHA-256 `f957920af299c536289e12bcb15275c019455d318c48c00505d594f56cc81c93`). The report identifies platform-tag lookup `0x147e20`, registry entry registration `0xa60d60`, insertion `0xa60db1`, and the tag `_cmn`. The report's function role is tag lookup, not file parsing.
- Existing caller report `local/re/evidence/owner-installer-2569d0/report.md` (SHA-256 `6b739c0bd7b7c008b1fec435c0dda5706cf302b9fd93012f8fa10880087a6856`) records that registry installer `0x2569d0` calls `0xa60d60` at `0x256b1e`. This confirms registry setup only.

An independent read-only agent scan covered 4,135 saved text/report files under `local/re/evidence`; its sorted path/hash manifest SHA-256 is `218b8ae0951ec4eb0ee03acb96deca9a7104ad63e44ee6732c34a5b573cca050`. It found the same two `.sxd` registry rows, no `SXDF`/`SXDS` reader evidence, and the repeated import-NID substring as a false positive.

## Observations, limits, and next action

Observed: the saved evidence establishes `.sxd` as a platform-tag registry FourCC mapped to `_cmn`, and shows its registration caller. The scanned exports contain no `SXDF`/`SXDS` framing reference or `.sxd` asset-reader/decoder candidate.

Inference limit: this is a no-candidate result for the scanned saved text exports only. It does not prove a reader is absent from unexported analysis or other evidence types. Asset framing, codec, engine routing, and playback remain UNKNOWN. No game-audio acceptance gate advances.

## Pre-publication verification

- `cmake -S native -B local/build/orch-r22` — exit 0.
- `cmake --build local/build/orch-r22 -j 12` — exit 0 (`ninja: no work to do`).
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, **431/431** passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, **516 passed / 0 skipped / 0 failed**.
- `trail build --check` — exit 0, wiring graph **9,876 nodes / 13,469 edges**; deep meaning layer not built.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` — exit 0, **33,134 folders current**; regenerable `launcher/.build` inventory churn was excluded from the published diff, while the durable checkpoint count advanced 822→823.
- `python3 -I scripts/audit_publish_paths.py` — exit 0, **7/7** staged paths accepted.
- `python3 -I scripts/audit_publish_paths.py --all` — exit 0 after staging, **3,019/3,019** paths accepted.
- `python3 -I scripts/check_commit_attribution.py` — exit 0 (`origin/main..HEAD`).
- `git diff --check` and owner identity check — exit 0; configured identity is Johnny Wang `<dehydratedflask@users.noreply.github.com>`.

Gate logs are in `local/validation/orch-20261027/aud01-sxdf-reader-census/` (private, excluded from publication). The prepublication log/artifact manifest SHA-256 is `604267d17c33ca46383b120b91e216c970cb2ff5f745cf30352fdf184d96cc29`.

**Files changed for this task:** `HANDOFF.md`, `docs/ASSETS.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint. Private query and gate artifacts are under `local/validation/orch-20261027/aud01-sxdf-reader-census/` and excluded from publication.

## Publication closeout — E792

- **Commit:** `e7921208b541d03a517a8fbb7ade38c98ae77eda` (`docs: record SXDF saved-evidence census`), pushed to `origin/main`. Author and committer are Johnny Wang `<dehydratedflask@users.noreply.github.com>`; fetch verified `HEAD == origin/main`, clean tree.
- **Commit checks:** `python3 -I scripts/check_commit_attribution.py --range 01f166fd784b1d1fea80ba541d4e865c86957b9e..HEAD` passed; the exact seven committed docs passed staged/full path audits **7/7** and **3,019/3,019**; postcommit full path audit passed **3,019/3,019**.
- **Fetched-ref attribution scan:** `python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-e7921208.json` exited 0. It parsed 9 fetched GitHub branch heads, 0 tags, and **2,403 unique reachable commits**, with zero Claude/Anthropic identities or credit lines. Report SHA-256 `b87445237e6364b2a6baaab72483c1d6c5de94170a6641f50c95af02ed4173ae`.
- The 70 Claude-attributed commits reachable only from private `refs/stash` remain unchanged per the user's explicit choice. `check_commit_attribution.py --all` remains **FAILED** only on historical non-Claude `orchestrator@local` commit `f6604a86e60e`.
- The scan covers fetched GitHub refs, not private stash refs. E791's scoped static-analysis no-candidate result does not prove global reader absence; audio format, codec, routing, and playback remain UNKNOWN.
