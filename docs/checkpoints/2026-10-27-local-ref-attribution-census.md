# E793 — all-ref Claude attribution census

- **Date/time:** 2026-10-27 10:07 CDT.
- **Agent / integrator:** `/root`.
- **Task:** `PUB-01-LOCAL-REF-ATTRIBUTION-CENSUS-01`, a read-only completion pass for the local-ref boundary of `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- **Starting state:** clean `main` at `f1ca39c12f41c7b411e997c421427000cc852951`, matching `origin/main`. Prior E792 evidence scanned fetched GitHub refs; this pass included local and Codex-managed refs.

## Exact scan and result

Command:

```sh
python3 -I local/validation/orch-20261027/local-ref-attribution/scan_local_refs.py && shasum -a 256 local/validation/orch-20261027/local-ref-attribution/scan_local_refs.py local/validation/orch-20261027/local-ref-attribution/claude-only-scan.json
```

Exit 0. The scanner reused `scripts/check_commit_attribution.py`'s commit parser and
`message_problems` policy, and added direct author/committer identity matching for Claude or
Anthropic. It parsed all **25 named refs other than `refs/stash`**, including local and remote
branches and the five `refs/codex/turn-diffs/*` refs, covering **2,404 unique commits**. It found
zero Claude/Anthropic identities and zero assistant credit/signature matches. A separate scan of
the stash ancestry found **768 commits** with **70** Claude-attributed commit messages. The user
explicitly chose to leave private stashes unchanged, and no ref was rewritten.

Private evidence under `local/validation/orch-20261027/local-ref-attribution/`:

- Scanner SHA-256: `337b5ead53884f8d455fd3c268bb51654a43a754b87884a1546a1681228af374`.
- JSON report SHA-256: `2d2c80f19d83e8bc4d5c1032fbfefed2501599396db1bb5d18480db20b56c5ef`.

## Publication verification

- `cmake -S native -B local/build/orch-r22`: **PASSED**, exit 0.
- `cmake --build local/build/orch-r22 -j 12`: **PASSED**, exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: **PASSED**, **431/431**.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **PASSED**, **516 passed / 0 skipped / 0 failed**.
- `trail build --check`: **PASSED**, 9,876 nodes / 13,469 edges. The deep meaning layer remains unbuilt.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: **PASSED**, 33,137 folders. The generated `launcher/.build` inventory changed during CTest; it was excluded from publication by restoring its prior rows and updating only the durable checkpoint count from 824 to 825.
- `python3 -I scripts/audit_publish_paths.py`: **PASSED**, 7 staged paths. `python3 -I scripts/audit_publish_paths.py --all`: **PASSED**, 3,021 tracked paths.
- `python3 -I scripts/check_commit_attribution.py`, `--identity`, `git diff --cached --check`, and `git diff --check`: **PASSED**.
- Private gate logs: `local/validation/orch-20261027/local-ref-attribution/gates/`; manifest SHA-256 `28c3dc7567a7bdbd1a3789a6d93e5db634add7bb6142aec23f47c123143dda7e`.

## Scope limits and disposition

The scan covers refs currently named by Git; it does not search unreachable commit objects.
The 70 stash matches are outside the requested rewrite scope by explicit user choice. The existing
all-history gate's unrelated `orchestrator@local` result is not a Claude/Anthropic attribution and
is not changed here. A first broad substring-only probe incorrectly classified explanatory prose
that discusses Claude as attribution; it was discarded and replaced with the repository's precise
credit/signature rules. The optional `CGSession -s` lock-state probe was unavailable on this host
(`command not found`); no UI was opened or changed. No source tree, game file, Ghidra database, or
stash content was modified.

No native-port acceptance gate advanced. CPU-01 slot 309 still lacks authoritative payload
ownership and invocation-order evidence; the VoiceOver task remains recorded as waiting for manual
unlock; AUD-02 still has no released provider query. No new source task was claimed.

**Task status:** `ARTIFACT_VERIFIED`; 0 failed hypotheses. **Next exact action:** commit only the
allowlisted documentation under the owner identity, push `origin main`, fetch, and verify the new
commit and public-ref attribution scan.
