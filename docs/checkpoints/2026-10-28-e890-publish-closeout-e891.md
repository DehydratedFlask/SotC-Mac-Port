# E891 — E890 publication and attribution closeout

- Local date/time: 2026-10-28 02:01 CDT (America/Chicago)
- Agent: `/root`
- Task: `PUB-01-E890-PUBLISH-CLOSEOUT-01`
- Status: `ARTIFACT_VERIFIED`; E890 publication and post-push attribution evidence verified.
- Attempts: 0 failed hypotheses.

## E890 commit and live refs

Commit `e1a8c4c4effe5682a69757f06ad7824d427ca139` (`docs: reconcile native core constructor
coverage`) is authored and committed by Johnny Wang
`<dehydratedflask@users.noreply.github.com>`. The exact-range attribution check passed for
`HEAD^..HEAD`. A normal fast-forward push updated `origin/main` from
`f4b5d35cc4855e9b52174164300d486248496187` to E890. After `git fetch origin main`, local `HEAD`,
`origin/main`, `FETCH_HEAD`, and live `refs/heads/main` all resolve to E890. The captured output is
`local/validation/orch-20261028/app01-coverage-reconcile-e890/postpush-refs.log`.

The exact-range result is saved at
`local/validation/orch-20261028/app01-coverage-reconcile-e890/attribution-postpush-exact.log`.
The fetch/live-ref command output was observed in this session; the post-push full census and
contributors response are preserved below.

## Claude attribution and contributors

A fresh scan of all 36 named local non-stash refs reached 2,504 unique commits at E890. It found
zero Claude/Anthropic author or committer identity hits and zero Claude/Anthropic credit-trailer
hits. The public GitHub contributors endpoint returned only `DehydratedFlask`. The separate
exact-range checker also passed on E890. The user's selected private-stash boundary is unchanged:
`refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` (21 reflog tips / 826 commits / 70
Claude credit lines). No stash ref or content was changed.

Private evidence:

- `local/validation/orch-20261028/app01-coverage-reconcile-e890/postpush-attribution-census.log`, SHA-256 `cdb29c49e11f18d31e49b81d8433444f760a3a5ed03cd8ce3225040836b159a5`
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/github-contributors.log`, SHA-256 `c26494de7a2b2ed933b58e8e5f3d580c5b2d8658b9f42eceeaae171b0004eeb9`
- `local/validation/orch-20261028/app01-coverage-reconcile-e890/attribution-postpush-exact.log`

## Validation and scope

The E890 artifact checkpoint records the core CLI observation: 397/573 distinct registered
constructor indices, first gap 309, and 397 registered slots ran OK. E865's 308/573 source
snapshot and E868's 311/573 old launcher bundle remain artifact-specific; E890 verified the core
target was up to date but did not rebuild the launcher bundle. No gate is promoted.

The E891 documentation-only publication validation passed:

- `cmake -S native -B local/build/orch-r22`: exit 0; `cmake --build local/build/orch-r22 -j 12`: exit 0 (`ninja: no work to do`).
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **431/431**.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **516 passed / 0 skipped / 0 failed**.
- `python3 scripts/project_index.py`: **32,057 folders indexed; current**.
- `trail build --check`: **9,877-node wiring graph in sync**; deep meaning layer not built.
- `python3 -I scripts/audit_publish_paths.py`: **7/7 staged paths**; `python3 -I scripts/audit_publish_paths.py --all`: **3,108/3,108 paths**.
- Precommit attribution check passed on the empty unpublished range. Cached and worktree `git diff --check` both exited 0; the exact E891 commit-range check runs after commit.

Logs are under `local/validation/orch-20261028/e891/validation/`.

APP-01, G1, and G4 remain `PARTIAL`. The latest CUA preflight in this turn still reported the Mac
locked, so the authorized VoiceOver observation did not start and no setting changed. This
checkpoint makes no cold-boot, game-audio, real-save/load, or Metal-rendering claim. Next action:
finish docs-only publication validation and publish this closeout; then recheck the Mac prerequisite
before any APP-01 VoiceOver observation.
