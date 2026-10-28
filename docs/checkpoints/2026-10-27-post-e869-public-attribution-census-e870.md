# E870 — post-E869 public attribution census

- Local date/time: 2026-10-27 22:53 CDT (America/Chicago)
- Agent: `/root`
- Task: `PUB-01-POST-E869-ADVERTISED-REF-CENSUS-01`
- Status: `ARTIFACT_VERIFIED` for the advertised-ref census measured at E869 HEAD; the E870 documentation commit is separately range-checked and published below.
- Attempt count: 0 failed census hypotheses.

## Scope and method

The check refreshed the Claude/Anthropic census after E869, covered every currently advertised GitHub ref, checked the GitHub contributors endpoint, and read the private stash roots without changing them. The preserved E867 scanner was inspected and copied byte-identically to a new private evidence directory. It uses read-only Git inventory and object commands and writes only within `local/validation/orch-20261027/e870/attribution/`.

## Census result at E869 HEAD

`git ls-remote --refs origin` returned 21 advertised refs: 9 branches, 12 pull-request heads, and no tags. These have 15 unique tips. The current named non-stash local/remote-tracking/Codex turn-diff ref inventory contains 36 refs; combined with advertised refs it contains 20 unique tips. The advertised refs reach 2,482 unique commits and all 2,482 commit objects were parsed.

The scan found zero Claude/Anthropic author or committer identities, zero Claude/Anthropic credit lines, zero matching Claude-attributed commits, and zero assistant credit lines. Four assistant-alias identity headers remain on two previously documented, non-Claude `orchestrator@local` commits: `f6604a86e60e7fd18cb15548362b8f134e433f5d` and `d614808bbdf732b5a3ebcf26b6d4c2a38f8d93b2`. The live GitHub `/contributors` endpoint listed only `DehydratedFlask`.

The separate stash scan covered 21 reflog tips / 826 reachable commits / 70 Claude credit lines. `refs/stash` was `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` both before and after. No stash ref or reflog was modified, per the user's explicit choice to leave private stashes unchanged.

## Commands and private evidence

- `git ls-remote --refs origin`: exit 0; 21 advertised refs / 15 unique tips.
- `python3 local/validation/orch-20261027/e870/attribution/scan_all_attribution_roots_e870.py`: exit 0; scanner SHA-256 `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`.
- `gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'`: exit 0; only `DehydratedFlask`.
- `python3 -I scripts/check_commit_attribution.py --range fc97d6fd55e5ff02a08ccc47ecb08f5592cf6db5..HEAD`: exit 0; E867, E868 and E869 are all by Johnny Wang.

Private artifacts: `local/validation/orch-20261027/e870/attribution/report.json` (SHA-256 `dab5fcffca101c1c033690222cad8e27bdf9372d56dad4611fc0c2a013792282`), `census.log` (`d84606033664a4ff4721633eee56bcafbf034e595eb5983ad7dcd34046fd4718`), `contributors.log` (`c26494de7a2b2ed933b58e8e5f3d580c5b2d8658b9f42eceeaae171b0004eeb9`), and `remote-refs.log` (`7286ec4f06a00d5849803cda82017e5495c4d79d2085f0aeae2d532050e53564`). `SHA256SUMS.txt` contains the private manifest. Two independent read-only agents confirmed the ref inventory and census results.

## Scope limits and product status

The scan covers current named non-stash refs, live advertised GitHub refs, and current stash reflog tips only. Unreachable Git objects outside those roots are not covered. The measurement is at E869 HEAD; the later E870 documentation commit is checked separately by the exact-range attribution gate and fetch/live-ref verification, so it is not counted in the 2,482-commit measurement. No branch rewrite, merge, source edit, game input, stash mutation, or dependency change occurred.

This attribution closeout does not change product evidence. G1–G4/game acceptance remain as previously recorded; G4 is `PARTIAL`. VoiceOver focus traversal and spoken output remain `NOT VERIFIED`.

## Publication gates

| Gate | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED, exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | PASSED, exit 0; Ninja no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 431/431, exit 0 |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 516 passed / 0 skipped / 0 failed, exit 0 |
| `trail build --check` | PASSED, exit 0; wiring graph 9,877/current; deep layer unbuilt |
| `python3 scripts/project_index.py` then `python3 scripts/project_index.py --check` | PASSED, 32,014 folders indexed/current |
| `python3 -I scripts/audit_publish_paths.py` (staged) and `python3 -I scripts/audit_publish_paths.py --all` | PASSED, 7 staged paths / 3,087 full paths |
| `git diff --cached --check` | PASSED, exit 0 |
| `python3 -I scripts/check_commit_attribution.py` before commit | PASSED, empty `origin/main..HEAD` range |
| Exact-range attribution, `git push origin main`, fetch/live-ref match, final contributor endpoint | PASSED: commit `c8d7fb5c1774451ca7847b7a5887d9e7e36cd714`, author/committer Johnny Wang; exact-range check passed on `origin/main..HEAD`; push to `origin main` passed; fetch matched `HEAD`, `origin/main`, `FETCH_HEAD`, and live `refs/heads/main`; contributors endpoint listed only `DehydratedFlask` |

Exact outputs and hashes are in `local/validation/orch-20261027/e870/attribution/`; no private evidence is staged.

## Files and next action

Allowlisted documentation changes: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and the E869/E870 checkpoints. No source or original game input changed.

E870 publication completed: commit `c8d7fb5c1774451ca7847b7a5887d9e7e36cd714` was exact-range checked, pushed to `origin main`, and fetch/live-ref verified. The separate publication closeout and no-READY triage are in [E871](2026-10-27-e870-publication-closeout-e871.md). The private stash remains unchanged.
