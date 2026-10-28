# E858 — E857 publication closeout and post-push attribution census

**Date:** 2026-10-27 19:08 CDT
**Integrator:** `/root`
**Task ID:** `PUB-01-LOCAL-REF-ATTRIBUTION-CENSUS-01`

## E857 publication

Commit `c8fd4cc49b69ea8dae26590e2b280ccb1e0bed01` (`docs: record E857 all-ref attribution census`) was authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. The exact-range attribution gate, effective identity check, committed-tree audit, and commit diff check passed. The push to `origin main` succeeded; a subsequent fetch verified `HEAD == origin/main == FETCH_HEAD == c8fd4cc49b69ea8dae26590e2b280ccb1e0bed01`. Private command logs are under `local/validation/orch-20261027/e857/`.

## Fresh post-push all-ref scan

The scanner is the corrected version of the E857 checker-pattern-matched scanner. Independent replay confirmed its output against separately enumerated refs and the repository checker's exact `ASSISTANT_RE`, `TRAILER_RE`, `CREDIT_KEYS`, `SIGNATURE_RE`, and `ROBOT_RE` patterns. Its read-only command was:

```sh
python3 -I local/validation/orch-20261027/e858/attribution/scan_all_attribution_roots_e858.py > local/validation/orch-20261027/e858/attribution/run.log 2>&1
```

It uses the repository checker's assistant alias, credit-key, signature, and robot patterns. The new report records local `HEAD`, fetched `origin/main`, and live advertised GitHub `main` at the E857 commit.

- 37 named non-stash refs; 21 advertised GitHub refs; 21 unique non-stash tips; 2,470 reachable commits.
- Zero Claude/Anthropic author/committer identity hits, zero Claude/Anthropic credit-line hits, and zero assistant credit lines across the named and advertised non-stash scopes.
- The same two historical non-Claude `orchestrator@local` commits remain, accounting for four author/committer identity headers. They remain documented and preserved.
- Stash scan: 21 reflog tips / 826 commits, 70 Claude/Anthropic credit lines, zero Claude/Anthropic identities, and one distinct `orchestrator@local` commit identity (two headers). `refs/stash` remained `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.

Private artifact SHA-256 values:

- Scanner: `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`
- Report: `5e816922fbbc8d687a31bc37924a440a5fdc4e855bc5e840def33606d0419174`
- Log: `db895c12e258f04a2fea5ef0d7713a18e75a167b8b39fc128d9c06e39b5d4a7d`

### Evidence handling note

After pushing E857, an attempted `--help` query against the scanner unexpectedly ran it because the script does not parse command-line options. That invocation replaced the private E857 `report.json` with the post-push report. The E857 report SHA-256 remains recorded in the E857 checkpoint and the independent reviewer confirmed its pre-push scope/counts/hash before replacement. The fresh post-push report, scanner, and log are now preserved under the distinct E858 private directory. No tracked file, named ref, advertised ref, reflog, or stash changed as a result of that invocation.

## Repository gates

E857's configure/build, CTest (431/431), lift checkers (516/0 skipped/0 failed), Trail (9,877 nodes, deep layer unbuilt), and project-index (31,992 folders) gates passed before the E857 publication. E858 is a documentation-only closeout with no source or build-system changes. Its required gates also passed:

| Gate | Result | Private log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED | `e858-cmake-configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | PASSED; Ninja reported no work | `e858-cmake-build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 431/431 | `e858-ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 516 passed / 0 skipped / 0 failed | `e858-lift-checkers.log` |
| `trail build --check` | PASSED, graph current at 9,877 nodes; deep layer unbuilt | `e858-trail.log` |
| `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` | PASSED, 31,994 folders | `e858-project-index-generate.log`, `e858-project-index-check.log` |

The first project-index check reported stale `local/indexes/folders.json`. The index was regenerated after the test/checker sweeps; the sequential follow-up check passed at 31,994 folders. The private index is ignored and not staged.

Final precommit checks passed: staged path audit 6/6, full tracked-tree audit 3,075/3,075, attribution check against `origin/main..HEAD`, effective owner identity, project-index check at 31,994 folders, and staged/worktree diff checks. Their logs are `e858-audit-staged.log`, `e858-audit-all.log`, `e858-attribution-precommit.log`, `e858-identity-precommit.log`, `e858-project-index-check.log`, `e858-staged-diff-check.log`, and `e858-diff-check.log` under the private E858 validation directory. The exact-range audit for the new E858 commit, push, and fetch verification remain pending.

## Observation and limits

- **Observation:** the currently named non-stash refs and live advertised GitHub refs reach 2,470 commits with zero Claude/Anthropic identity or credit hits under the repository's full attribution rules.
- **Owner boundary:** 70 Claude-attributed credit lines remain in the private stash, unchanged per the user's choice.
- **Limit:** unreachable Git objects are outside the named/advertised/stash-reflog census. The census does not guarantee GitHub's contributor graph cache has refreshed. No game boot, authentic audio, real save/reload, Metal, or forbidden-runtime gate advanced.
- **Files changed:** this closeout, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and generated `docs/FOLDERS.md`. All raw scan/gate data is private under ignored `local/validation/orch-20261027/e858/`.
- **Next exact action:** finish staged/full path, owner identity, attribution, and diff checks; commit the six allowlisted docs paths as Johnny Wang; exact-range check, push `origin main`, and fetch-verify.
- **Task status / attempts:** the E857 publication and E858 fresh census are verified; the E858 docs closeout is in progress. No attribution hypotheses failed; one scanner coverage correction. The overwritten raw E857 report is an evidence retention limitation, with prior hash and independent review retained.
