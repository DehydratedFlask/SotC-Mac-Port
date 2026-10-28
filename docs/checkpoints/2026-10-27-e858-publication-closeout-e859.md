# E859 — E858 publication closeout and post-push attribution census

**Date:** 2026-10-27 19:13 CDT
**Integrator:** `/root`
**Task ID:** `PUB-01-LOCAL-REF-ATTRIBUTION-CENSUS-01`

## E858 publication

Commit `63fe6d24ca667518e71c1326932815e989bbfe50` (`docs: close E857 attribution verification`) was authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. Its exact-range attribution, effective identity, committed-tree (3,075 paths), and diff checks passed. Push to `origin main` succeeded. Fetch verified `HEAD == origin/main == FETCH_HEAD == 63fe6d24ca667518e71c1326932815e989bbfe50`. Logs are private under `local/validation/orch-20261027/e858/`.

## Fresh live attribution census

The post-push scan uses the checker-pattern-matched scanner from E858. Its command was:

```sh
python3 -I local/validation/orch-20261027/e859/attribution/scan_all_attribution_roots_e859.py > local/validation/orch-20261027/e859/attribution/run.log 2>&1
```

The report records local `HEAD`, fetched `origin/main`, and live advertised GitHub `main` at the E858 commit. It covers:

- 37 named non-stash refs and all 21 live advertised GitHub refs; 21 unique tips and 2,471 reachable commits.
- Zero Claude/Anthropic author or committer identity hits, zero Claude/Anthropic credit-line hits, and zero assistant credit lines.
- Two documented non-Claude `orchestrator@local` commit objects, contributing four author/committer identity headers.
- A separate stash reflog scope of 21 tips / 826 commits / 70 Claude credit lines, with zero Claude identities and one separate `orchestrator@local` identity. The stash ref remained `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.

Independent replay passed: separately enumerated refs and all match records equal the report, and AST comparison confirmed the scanner's `ASSISTANT_RE`, `TRAILER_RE`, `CREDIT_KEYS`, `SIGNATURE_RE`, and `ROBOT_RE` match the repository checker.

Private artifact SHA-256 values:

- Scanner: `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`
- Report: `bb13c9e977ad74fd3016f2ee0d2464ec5f758770051a86ef4c311c8d7216d590`
- Log: `89dfc05bfca4030c6753b7380e732d58040ebd876950fb79bb47e0ae953459ff`

## Product-task scan

The latest CPU, launcher, and DATA/AUD/SAV/GPU scouts found no independent `READY` product task. UI validation still needs an unlocked Mac. Save/load needs the serializer and a user-owned save. AUD-02 needs a released query scope. GPU work remains dependency-gated. The existing CXML evidence remains `UNKNOWN`; its DTD-disabled rejection must not be retried with a different parser or DTD settings.

## Repository gates

The E859 closeout adds documentation only; source and build-system files remain unchanged. Required gates passed, with full logs under `local/validation/orch-20261027/e859/`:

| Gate | Result | Private log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED | `e859-cmake-configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | PASSED; Ninja reported no work | `e859-cmake-build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 431/431 | `e859-ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 516 passed / 0 skipped / 0 failed | `e859-lift-checkers.log` |
| `trail build --check` | PASSED, graph current at 9,877 nodes; deep layer unbuilt | `e859-trail.log` |
| `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` | PASSED, 31,996 folders | `e859-project-index-generate.log`, `e859-project-index-check.log` |

The first final index check reported `local/indexes/folders.json` stale after new private E859 audit log files had been created. Regeneration after those files existed and the sequential check passed at 31,996 folders. Final precommit publication checks also passed: staged paths 6/6, tracked paths 3,076/3,076, `origin/main..HEAD` attribution, effective owner identity, and staged/worktree diff checks. Logs are `e859-audit-staged.log`, `e859-audit-all.log`, `e859-attribution-precommit.log`, `e859-identity-precommit.log`, `e859-staged-diff-check.log`, and `e859-diff-check.log` under the private E859 directory.

## Observation and limits

- **Observation:** current named non-stash and live advertised GitHub refs reach 2,471 commits with zero Claude/Anthropic attribution and zero assistant credit lines under the repository's full checker patterns.
- **Owner boundary:** 70 Claude-attributed credit lines remain in the private stash, unchanged by the user's direction. The E857 raw pre-push report was overwritten by the scanner's unsupported `--help` invocation; its recorded SHA and independent review are retained, while post-push E858/E859 reports are safely preserved in fresh private directories.
- **Limit:** unreachable Git objects outside current named, advertised, and stash reflog roots were not scanned. This census does not guarantee GitHub's contributor graph cache has refreshed. No actual game boot, authentic audio, real save/reload, Metal, or forbidden-runtime gate advanced.
- **Files changed:** this checkpoint, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and generated `docs/FOLDERS.md`. Private scan and gate artifacts remain under ignored `local/validation/orch-20261027/e859/`.
- **Next exact action:** commit six allowlisted docs paths as Johnny Wang; exact-range check, push `origin main`, and fetch-verify.
- **Task status / attempts:** E859 attribution census `ARTIFACT_VERIFIED`; E859 documentation publication is in progress. Zero failed attribution hypotheses; one scanner-coverage correction. Raw E857 report overwrite remains documented as an evidence-retention limitation.
