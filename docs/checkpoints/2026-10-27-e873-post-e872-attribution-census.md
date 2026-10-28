# E873 — Post-E872 attribution census and APP-01 accessibility follow-up

Date: 2026-10-27 (local, America/Chicago)
Agent: `/root`
Tasks: `PUB-01-POST-E872-ADVERTISED-REF-CENSUS-01`, `APP-01-VOICEOVER-READINESS-02`
Status: `ARTIFACT_VERIFIED` for the read-only attribution census; APP-01 remains `PARTIAL`. Publication is pending.

## Attribution census

The measured tip is E872 `ecd9d98cd174c02aad45fdb190f69a70a1f31209`, matching local `HEAD`, `origin/main`, and live advertised `main` at scan time. The scanner covered 36 named non-stash refs and all 21 refs advertised by GitHub (9 branches, 12 pull-request refs, 0 tags). These comprise 15 distinct advertised tips and 20 unique combined tips, reaching 2,485 commits; all 2,485 commit objects were parsed.

The scan found zero Claude/Anthropic author or committer identity hits, zero Claude/Anthropic credit-line hits, zero Claude-attributed commits, and zero assistant credit lines. Four assistant-alias identity headers remain on two historical non-Claude `orchestrator@local` commits: `f6604a86e60e7fd18cb15548362b8f134e433f5d` and `d614808bbdf732b5a3ebcf26b6d4c2a38f8d93b2`. GitHub's live contributors endpoint lists only `DehydratedFlask`.

The stash scan covered 21 reflog tips and 826 commits, finding 70 Claude credit lines. `refs/stash` remained `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` before and after. The user directed that private stashes remain unchanged; no ref, reflog, source, game input, or dependency was modified.

An independent read-only reviewer matched the 21 live `(ref, SHA)` pairs exactly, independently traversed and parsed all 2,485 commits, matched the attribution counts and the four assistant-alias identity headers, and recomputed the unchanged stash counts. The E873 scanner copy is byte-identical to the E870 scanner (`cmp -s` exited 0).

The scan covers the current named local refs, live advertised GitHub refs, and current stash reflog tips. It does not cover unreachable objects outside those roots. The 2,485-commit count describes E872 before this checkpoint commit; the checkpoint commit is checked separately before publication.

Private evidence is under `local/validation/orch-20261027/e873/attribution/`:

- Scanner SHA-256: `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`.
- `report.json` SHA-256: `b68e36a23e3f206705c7823021bc028b8198c5ce93f7f8f1caad20759dffceda`.
- `census.log` SHA-256: `d92303a516c459cf520add2434d087395c9d29f6703f8a10c99baa540670b0e6`.
- `contributors.log` SHA-256: `c26494de7a2b2ed933b58e8e5f3d580c5b2d8658b9f42eceeaae171b0004eeb9`.
- `remote-refs.log` SHA-256: `28bec1c83543573de3248b8825854169eedac805287b0b555090ee51b3474228`.
- Exact-range attribution log SHA-256: `e684c0d69ffd59f85e35e94e13bd2dc091eca98ade0cd2eaa114ca8812667d70`.

Commands and results:

| Command | Result |
|---|---|
| `git ls-remote --refs origin` | Exit 0; 21 live refs (9 branches, 12 PR refs, 0 tags) |
| `python3 local/validation/orch-20261027/e873/attribution/scan_all_attribution_roots_e873.py` | Exit 0; 2,485 reachable commits parsed; zero Claude/Anthropic identities/credits |
| `gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'` | Exit 0; only `DehydratedFlask` |
| `python3 -I scripts/check_commit_attribution.py --range 3ed9e49e42e3c971151eb2f51d6a41a3c4f336bf..HEAD` | Exit 0; exact range E870-E872 clean |
| Independent live-ref and stash replay by `/root/census_review_e873` | Matched ref inventory, reachable/parsed counts, attribution results, stash state, and scanner-copy checksum; no ref or file edits |

## APP-01 VoiceOver follow-up

The user authorized temporarily enabling VoiceOver and restoring it off. System Settings reported it off before the check, on during the check, and off afterward. VoiceOver Utility showed “Show caption panel” enabled (Value 1); the preference was not changed. Full Keyboard Access remained off, and the VoiceOver Utility window was closed afterward.

On the workspace launcher bundle, `Control+Option+Right` did not change the accessibility tree's reported focus. `Tab` moved the visible keyboard focus to `Choose Folder…`. The launcher-window screenshot showed no caption text, but it was not a desktop capture; no speech was captured. Therefore VoiceOver traversal, spoken output, and caption output remain `NOT VERIFIED`. No folder was chosen, Play was not activated, and no launcher preference, source, or game input changed. G4 remains `PARTIAL`.

Private observation note: `local/validation/orch-20261027/e873/voiceover/observation.txt`, SHA-256 `8a93c120b5ffbd9c1127ba17620b86a67dc3424fed9cd50b1ac0c68aa4e5f792`. The screenshot was visible during CUA review but was not persisted.

## Repository gates and publication

| Gate | Result / evidence |
|---|---|
| `cmake -S native -B local/build/orch-r22` | Exit 0; `local/validation/orch-20261027/e873/attribution/cmake-configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | Exit 0; Ninja reported no work; `cmake-build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | Exit 0; 431/431 passed; `ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | Exit 0; 516 passed / 0 skipped / 0 failed; `lift.log` |
| `trail build --check` | Exit 0; wiring graph OK at 9,877 nodes; deep meaning layer remains unbuilt; `trail.log` |
| `python3 scripts/project_index.py` then `python3 scripts/project_index.py --check` | Both exit 0; 32,017 folders indexed and current; `project-index.log`, `project-index-check.log` |
| `python3 -I scripts/audit_publish_paths.py` | Exit 0; 6 staged paths passed; `audit-staged.log` |
| `python3 -I scripts/audit_publish_paths.py --all` | Exit 0; 3,090 paths passed; `audit-full.log` |
| `git diff --cached --check` | Final run exit 0; `diff-check.log`. Initial run found three trailing spaces in the new metadata lines; they were removed before the final passing run. |
| `python3 -I scripts/check_commit_attribution.py` | Exit 0 before commit; `attribution-precommit.log`. Exact range for E873 will be checked after commit. |
| Commit / push / fetch / live-ref verification | Pending; this checkpoint records the pre-publication gates. |

Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No tests were added, no source or game input changed, no dependencies were acquired, and `INSTALL_LOG.md` is unchanged. The all-path audit also traversed ignored/private paths without staging or publishing them.

Remaining risks: the attribution census does not cover unreachable objects outside its roots; the user-directed stash remains untouched with 70 Claude credit lines; VoiceOver traversal and spoken/caption output remain unverified; G4 remains `PARTIAL`. The task-board scan `rg -n '\| READY' docs/TASKS.md` exited 1 with no matches.

Only allowlisted documentation paths are in scope. No tests were added; no source files, game inputs, private stash data, refs, or dependencies were changed. G1-G4 are unchanged; G4 remains `PARTIAL`. No product task is currently `READY`.
