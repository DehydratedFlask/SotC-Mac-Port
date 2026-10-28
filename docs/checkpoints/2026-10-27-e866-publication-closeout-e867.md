# E867 — E866 publication and post-push attribution closeout

- Local date/time: 2026-10-27 20:18 CDT
- Agent: `/root`
- Task: `PUB-01-E866-PUBLISH-AND-POSTPUSH-CENSUS-01`
- Status: `ARTIFACT_VERIFIED`; checkpoint published as `45af5f281fa5e93891795766fb0fb3646547df7f`, exact-range checked, pushed, and fetch-verified. E868 records the publication closeout.

## E866 publication

Checkpoint E866 was committed to `main` as `fc97d6fd55e5ff02a08ccc47ecb08f5592cf6db5` (`docs: record SAV loader rejection and attribution audit`). Author and committer are Johnny Wang `<dehydratedflask@users.noreply.github.com>`. The command `python3 -I scripts/check_commit_attribution.py` passed over `origin/main..HEAD` before push. `git push origin main` exited 0. After `git fetch origin`, local `HEAD`, `origin/main`, and live `refs/heads/main` all resolved to the same commit.

Private evidence/logs are under `local/validation/orch-20261027/e866/`: `attribution-postcommit.log`, `push.log`, and `fetch-verify.log`. No code, game input, or private evidence payload was staged.

## Fresh public attribution census

After E866 was published, a new read-only census covered 37 named non-stash refs and all 21 currently advertised GitHub refs (21 combined unique tips / 2,479 reachable commits). It found zero Claude/Anthropic author or committer identities, zero Claude/Anthropic credit lines, and zero assistant credit lines. GitHub's live `/contributors` endpoint listed only `DehydratedFlask`. Four assistant-alias identity headers remain on the two preserved non-Claude `orchestrator@local` commits recorded in E866; they are not Claude identities or credit lines.

The user's direction to leave private stashes unchanged still applies. The read-only stash census saw 21 reflog tips / 826 reachable commits / 70 Claude credit lines; `refs/stash` stayed at `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`. These stashes and unreachable objects are outside the requested public contributor cleanup scope.

Exact commands:

```sh
python3 -I local/validation/orch-20261027/e867/attribution/scan_all_attribution_roots_e867.py
gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'
```

Both exited 0. Private post-E866 artifact hashes: scanner `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`; report `da9fc8b5192aacc910febaa7de037a65dbe83173028623b7aeabb9836d0b2bcc`; census log `74ec49d9d9ba41bc7e0f495d6507fed41dd504b4310e0dca87c4c34cea9c6bc2`; contributors API response `c26494de7a2b2ed933b58e8e5f3d580c5b2d8658b9f42eceeaae171b0004eeb9`.

## Files and remaining verification

E867 is a documentation-only closeout in `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No dependencies were installed; `INSTALL_LOG.md` is unchanged.

Validation commands run from the workspace root and logs are private under `local/validation/orch-20261027/e867/`:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure and build each exited 0 (Ninja no work); CTest exited 0 with 431/431; lift checker exited 0 with 516 passed, 0 skipped, and 0 failed; Trail check exited 0 with 9,877 wiring nodes current (deep meaning layer unbuilt); project index reports 32,011 folders and `--check` is current. The staged path audit exited 0 for 7/7 paths, the full audit exited 0 for 3,084/3,084 paths, cached whitespace check exited 0, and the empty precommit attribution range passed. Exact logs: `cmake-configure.log`, `cmake-build.log`, `ctest.log`, `lift-checkers.log`, `trail-check.log`, `path-audit-all.log`, `path-audit-staged.log`, `path-audit-all-poststage.log`, `whitespace-staged.log`, `attribution-precommit.log`, `project-index.log`, and `project-index-check.log`, all under `local/validation/orch-20261027/e867/`. No dependencies were installed and `INSTALL_LOG.md` is unchanged.

The final staged set passed `python3 -I scripts/audit_publish_paths.py` (7/7) and `python3 -I scripts/audit_publish_paths.py --all` (3,084/3,084); `git diff --cached --check` passed. `git fetch origin` verified `main` at `fc97d6fd55e5ff02a08ccc47ecb08f5592cf6db5`, and the configured author and committer identities were both Johnny Wang `<dehydratedflask@users.noreply.github.com>`. E867 was committed as `45af5f281fa5e93891795766fb0fb3646547df7f`, exact-range checked, pushed to `origin main`, and fetch-verified at local/remote/live `main`. The post-push GitHub contributors endpoint lists only `DehydratedFlask`. Overall game acceptance remains PARTIAL.
