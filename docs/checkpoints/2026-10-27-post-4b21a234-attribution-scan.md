# Attribution scan after `4b21a234`

- Local date/time: 2026-10-27 03:22 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task: `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- Status: **ARTIFACT_VERIFIED** for fetched public refs through `4b21a234f127852910cf5252672ae6a5c6f8be09`; local stash scope remains **ACTIVE** pending owner direction.
- Prior checkpoint `4b21a234f127852910cf5252672ae6a5c6f8be09` was authored and committed by Johnny Wang `<dehydratedflask@users.noreply.github.com>`.

## Public-ref result (E767)

`git fetch origin --prune` exited 0. This scan command exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-4b21a234.json
```

It parsed 9 fetched branch heads, 0 remote tag refs, and 2,384 unique reachable commits, including
`origin/main == 4b21a234f127852910cf5252672ae6a5c6f8be09`. It found zero Claude/Anthropic author or
committer identities and zero credit-line matches. Private report:
`local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-4b21a234.json`,
SHA-256 `4d5efe94dbf76069267d16800a936f6e2e158dfe29870fde67ddb60fc1622c28`.

The E762 scan of all locally stored refs found 70 attribution matches reachable only from
`refs/stash`; those private recovery refs remain unchanged pending the owner's scope decision.
`check_commit_attribution.py --all` still flags only the unrelated historical non-Claude
`f6604a86e60e` `orchestrator@local` commit on `main`.

## Verification

Repository validation logs are under `local/validation/orch-20261027/slot309-checkpoint/e767/`:

| Command | Result | Log SHA-256 |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 | `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; no work required | `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `d019b0bc1dccb4cac53dfeebce1c52e72470b7025b072719636a8d2817d73a74` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8e7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep layer not built, 9,875 nodes pending | `faf3792f3bd222b9d413eefb67ccad8b2d488029cc2ffbe5c38cb8c65e5c723f` |
| `python3 scripts/project_index.py` | exit 0; 33,107 folders; only durable `docs/checkpoints` count 812→813 retained in FOLDERS | `a1dc2b55ed7503ba661aca56544db1543874530e47062c5062f036e17b41cb17` |
| `python3 -I scripts/audit_publish_paths.py` | exit 0; **7 staged paths** passed | `5ded73fbdbf806fdc82879dc5aaa1c5e2e3fbe9c8f7835332fa4470357facebb` |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; **3,009 paths** passed | `ba03c7f2bfcaabc4f1c03d315c29faeead8fca44dfdbb0c3031f30e2d388b755` |
| `python3 -I scripts/check_commit_attribution.py` | exit 0 before commit; unpublished range clean | `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --identity` | exit 0; owner identity | `888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559` |
| `python3 -I scripts/check_commit_attribution.py --all` | exit 1 only for historical non-Claude `f6604a86e60e` `orchestrator@local` | `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |
| `git diff --cached --check` | exit 0 | no output |

Staged/all path audits and attribution checks are run after finalizing this docs set. No game
execution, boot/audio/save/load, Metal, or forbidden-runtime claim is made by this scan.

## Next exact action

Continue slot 309 at the exact table identities documented in the E765 checkpoint and preserve the
unresolved registration branch unless new evidence proves ordering. Resolve the local-stash scope
question before modifying `refs/stash`. Keep publishing allowlisted checkpoint docs under the owner
identity, and refresh fetched public refs after each push.
