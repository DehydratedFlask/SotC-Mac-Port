# E861 — Post-E860 public-ref attribution census

**Date:** 2026-10-27 19:25 CDT
**Integrator:** `/root`
**Task:** `PUB-01-LOCAL-REF-ATTRIBUTION-CENSUS-01`
**Status:** **ARTIFACT_VERIFIED** for the current named and GitHub-advertised non-stash refs. The documentation checkpoint is prepared; owner commit/push and exact-range verification are the next actions.
**Attempts:** 0 failed hypotheses; one root census and independent replay.

## Fresh current-ref census

The read-only scanner copied from E860 ran after fetching `origin` and compared all
currently named local, remote-tracking, and Codex turn-diff refs with all refs currently
advertised by GitHub. At scan time `HEAD`, `origin/main`, and advertised `main` all matched
`d989b3e0d7dd781d636dfedaa4e78fa09c63685c`.

- **Named non-stash refs:** 37 refs; 2,473 unique reachable commit objects.
- **Live advertised refs:** 21 refs; 2,473 unique reachable commit objects.
- **Union:** 21 unique tips; 2,473 unique reachable commit objects.
- **Claude/Anthropic:** zero identity hits, zero credit-line hits, and zero attributed commits.
- **Other assistant identity metadata:** four identity headers on two commits, both author and committer `orchestrator <orchestrator@local>`: `f6604a86e60e7fd18cb15548362b8f134e433f5d` and `d614808bbdf732b5a3ebcf26b6d4c2a38f8d93b2`. These are not Claude/Anthropic attributions and remain unchanged.
- **Assistant credit lines:** zero in the public/non-stash scope.

Independent reviewer `/root/live_attr_audit` replayed the ref enumeration and commit-object scan.
It matched all counts and records and confirmed the four `orchestrator@local` headers above.

The user directed us to leave private stashes unchanged. A separate read-only scan saw 21 reflog
tips / 826 commits and 70 Claude/Anthropic credit lines. `refs/stash` remained
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`; no stash ref or reflog was changed. This private
recovery history is outside the GitHub cleanup scope.

## Reproduction and hashes

From the repository root:

```sh
git fetch origin
python3 -I local/validation/orch-20261027/e861/attribution/scan_all_attribution_roots_e861.py
```

Exit status was 0. The scanner and complete output are kept outside the public repository:

- Scanner: `local/validation/orch-20261027/e861/attribution/scan_all_attribution_roots_e861.py`, SHA-256 `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`.
- JSON report: `local/validation/orch-20261027/e861/attribution/report.json`, SHA-256 `a4a1d9ec0c7100caa605a869f2da37dd8805c921665dddb7d04dfea4101e7903`.
- Root log: `local/validation/orch-20261027/e861/attribution/scan.log`, SHA-256 `6e9df19c82fe0a94b0169b5701dd481dac0b71894a01038b6eafba01cc2feeed`.

The scanner mirrors the alias, credit-key, signature, and robot patterns in
`scripts/check_commit_attribution.py`. The scan covers current named/advertised refs and stash
reflog tips only; unreachable Git objects outside those roots are not included.

## Repository and publication gates

All commands ran from the workspace root. Full logs are under
`local/validation/orch-20261027/e861/`.

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
```

Configure and build exited 0; Ninja reported no work. CTest passed **431/431**. Lift checkers
passed **516**, with **0 skipped / 0 failed**. Trail reported the wiring graph current and
`graph check: OK` at 9,877 nodes; the deep meaning layer is not built. The project index exited
0 and indexed **32,000 folders**. These repository checks do not demonstrate game boot, actual
audio, real save/load, Metal gameplay, or a native startup path.

The first two `python3 scripts/project_index.py --check` attempts exited 1 with
`Stale indexes: local/indexes/folders.json`; both failed logs are preserved as
`project-index-stale-first.log` and `project-index-check.log`. After all repo-local validation
artifacts existed, regenerating the indexes and running `--check` with output returned directly
completed successfully (`32000 folders indexed; current`, exit 0). Writing a new check log under
`local/` after generation reproduced the stale-index failure; no source or game-input files
changed.

Pre-publication audits passed on the final six explicitly staged documentation paths and all
**3,078** repository paths. Exact commands were `python3 -I scripts/audit_publish_paths.py`
and `python3 -I scripts/audit_publish_paths.py --all`; outputs are
`path-audit-staged.log` and `path-audit-all.log`. `git diff --cached --check` also passed.
The first staged diff check reported four trailing hard-break spaces in this checkpoint's
metadata. Those spaces were removed, and the final staged `git diff --cached --check` passed.

## Files, limits, and next action

This checkpoint updates `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
generated `docs/FOLDERS.md`, and this file. Fresh scan and validation artifacts remain ignored
under `local/validation/orch-20261027/e861/`.

Observation: public GitHub-advertised history is free of Claude/Anthropic identity and credit
metadata for every commit currently reachable from those refs. The E861 commit itself is not part
of this pre-commit scan and must pass an exact-range attribution check before push. Inference:
users browsing the current advertised ref graph will not see Claude as a commit contributor;
GitHub contributor aggregation can lag or differ from reachable commit metadata.

Native port acceptance remains **PARTIAL**: current triage found no independent `READY` product
task. Slot 309 lacks a concrete semantic producer/consumer path; CXML structure is `UNKNOWN` at
the permitted parser boundary; AUD-02 needs a released query scope; game save/load needs the
serializer, deserializer, and a user-owned save; launcher UI validation needs an unlocked Mac;
GPU tasks remain dependency-gated.

**Next exact action:** commit the audited six allowlisted documentation paths under Johnny Wang,
check the commit's exact attribution range and owner identity, push `origin main`, fetch-verify
the remote head, and confirm the worktree is clean. Keep the private stash and native-game
acceptance gates unchanged.
