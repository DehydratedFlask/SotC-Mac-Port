# Slot 309 table-consumer census and post-`32f507e0` scan

- Local time / timezone: 2026-10-27 03:19 CDT (America/Chicago).
- Integrator: `/root`; read-only analysis lane: `/root/slot309_table_consumer`.
- Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- Status: **STATIC_ANALYSIS_VERIFIED** for the bounded table-reference census; slot 309 remains ACTIVE/G1 PARTIAL. **ARTIFACT_VERIFIED** for fetched public refs through `32f507e0`; local stash scope remains pending.
- Changed paths in this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/ATTRIBUTION.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this file.

## Pointer-table consumer census (E765)

Input: pinned `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
Private report: `local/validation/orch-20261027/slot309/table-consumer/REPORT.md`, SHA-256
`5dbe15d0c13cf6108259a55dc9ad1b2fb4beb21e322329024fbef6ca85bd9e11`.

The bounded RIP-reference scan found two instruction-boundary-confirmed LEAs to table base
`0x15c8cc8`, at `0x536ac7` and `0x537f9a`. The constructor-like routine `0x536aa0` and cleanup
routine `0x537f90` set an object vptr to `0x15c8cd8`. Its inspected caller `0x536730` dispatches
vtable offsets `+0x10` and `+0x18`; it does not use the `+0x20` slot at `0x15c8cf8`, whose value is
`0x531d60`. The sole direct caller found for `0x536730` is `0xf120` at `0xf15a`, followed by a trap
path. No evidence connects these functions to constructor slot 309 or orders this table consumer
relative to slot 309. Preserve both `0x2dfd4b8` registration branches and keep slot 309 unregistered.

Exact private analysis commands are recorded in the report; all exited 0:

- `python3 -I tools/lift/xref_rip.py 0x15c8ce8 0x15c8cf0 0x15c8cf8 0x15c8d00 0x15c8d08 0x15c8d10 0x15c8d18 0x2e716a0` — all targets returned 0 refs.
- `python3 -I tools/lift/xref_rip.py 0x531d60 0x15c8ce0 0x15c8ce8 0x15c8cf0 0x15c8cf8 0x15c8d00 0x15c8d08 0x15c8d10 0x15c8d18 0x15c8d20 0x2e716a0` — all targets returned 0 refs.
- `python3 -I local/validation/orch-20261027/slot309/table-consumer/rip_window_scan.py > local/validation/orch-20261027/slot309/table-consumer/rip-window-scan.json` — six heuristic candidates reduced to two instruction-boundary-confirmed LEAs; bounded scan only.
- Disassemblies of `0x536730`, `0x536aa0`, `0x537f90`, and `0xf120` were produced with `objdump -D -Mintel --start-address=... --stop-address=... local/re/eboot.elf` and hashes are in the private report.

The direct-rip scanner and bounded executable-window scan do not cover metadata-driven dispatch,
indirect calls outside the examined path, or runtime execution. The worker completed one bounded
census with **0 failed semantic hypotheses**; an initial scratch-script path calculation was
corrected before the successful run. No game code was executed, no REA/Ghidra import was started,
and no shared source or docs were changed by the worker.

## Public-ref scan after `32f507e0` (E766)

After `git fetch origin --prune` (exit 0), this command exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-32f507e0.json
```

It parsed 9 fetched branch heads, 0 remote tag refs, and 2,383 unique reachable commits through
`origin/main == 32f507e033a6a374b03bf0109aefdfececfb4b42`. It found zero Claude/Anthropic author or
committer identities and zero credit-line matches. Private report SHA-256:
`5d8aabfd59f7cd1124b215903b219496bc6e7c3e5e52a3d660f7b556019eda93`.

E762's all-locally-stored-ref scan found 70 attribution matches reachable only from `refs/stash`;
those refs remain unchanged pending the owner's scope decision. `check_commit_attribution.py --all`
still identifies only the unrelated non-Claude `f6604a86e60e` `orchestrator@local` commit on `main`.

## Repository verification

Logs are under `local/validation/orch-20261027/slot309-checkpoint/e765-e766/`:

| Command | Result | Log SHA-256 |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 | `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; no work required | `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `db382f63b25f458d9231b6aeb455bf1781fcf3b4365b3f193a1835daf9bf0045` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8e7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep layer not built, 9,875 nodes pending | `faf3792f3bd222b9d413eefb67ccad8b2d488029cc2ffbe5c38cb8c65e5c723f` |
| `python3 scripts/project_index.py` | exit 0; 33,106 folders; only durable `docs/checkpoints` count 811→812 retained in FOLDERS | `789f28e1cf5aa3de2fcf10b08f525844d299c76fb909f2e9ebdc5c4dbe92c574` |
| `python3 -I scripts/audit_publish_paths.py` | exit 0; **7 staged paths** passed | `39b54d551ba2ba61ab544496366244a92ba0d049746e4c3cfad15261100370a5` |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; **3,008 paths** passed | `e0ebcfd5a39216210468494e69540bd1fb8da4e479f7800e6593c34bfb50c4dc` |
| `python3 -I scripts/check_commit_attribution.py` | exit 0 before commit; unpublished range clean | `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --identity` | exit 0; owner identity | `888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559` |
| `python3 -I scripts/check_commit_attribution.py --all` | exit 1 only for historical non-Claude `f6604a86e60e` `orchestrator@local` | `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |
| `git diff --cached --check` | exit 0 | no output |

No game boot, audio, real save/reload, Metal, or runtime-absence claim is made by this checkpoint.

## Next exact action

Check the saved slot-309 map for exact identities `0x536aa0`, `0x537f90`, `0x536730`, or `0xf120`.
If absent, keep the registration branch unresolved and seek an incoming-reference query only from an
already-open static-analysis session. Resolve the local-stash scope question before modifying
`refs/stash`.
