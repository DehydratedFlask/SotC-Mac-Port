# Slot 309 lifecycle, callback edges, and post-`f914bcb2` attribution scan

Local date/time: 2026-10-27 02:31 CDT (America/Chicago). Integrator: `/root`.

Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.

## Slot 309 lifecycle and callback characterization (E757)

All shipped-code observations below use the pinned local ELF `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, saved exports, and
read-only byte/RELA analysis. Eight private reports were independently hash-checked by the
integrator; each contains its exact commands, output hashes, and narrower evidence limits:

| Bounded lane | Private report SHA-256 | Result |
|---|---|---|
| `0x20d80` registration, unregister, manager cleanup | `local/validation/orch-20261027/slot309/worker-211d0-registry-contract/REPORT.md` — `e8611225a4305c50e4a7ddf11f453855437edab731c9932859fefd95fe7cc63b` | For inspected keyed-map paths, unregister removes the separately allocated key/node but does not release the mapped primary pointer; manager tree cleanup destroys keys/nodes, not map values. This supports a non-owning registry for those paths. |
| Primary object `0x32f7d30` | `local/validation/orch-20261027/slot309/worker-slot309-owner/REPORT.md` — `0dde68979ee875d7491efc129f9533164f58a8485cbeca50a6c54e10b6784e56` | This observed object is in the memory-only writable ELF tail, is initialized by slot 309, and is passed to `__cxa_atexit` with callback `0xa48240`. Its callback releases members and unregisters through `0x20b50`; it does not free this static-storage object. Do not generalize this owner result to other objects. |
| Third `0x20af0` caller at `0xa50846` | `local/validation/orch-20261027/slot309/worker-20af0-third-caller/checkpoint.md` — `b119c139a1064e4fbd9cd50a5d5fbf01421c5da94ec04f8182f59bd7e74b3296` | The two source-record control qwords are explicitly zero for this call, so `0x20af0`'s conditional control increments are skipped on this path. No source type or ownership is inferred. |
| `0xf180` and `0xf7b0` | `local/validation/orch-20261027/slot309/worker-f180-contract/REPORT.md` — `2b6ca3c5e403876ca84214a651be70a40ee94334b0f66d24e86b1dc202581fe5` | `0xf180` writes `0x2dd9f78` and zeroes 24 following bytes; it has no calls or branches. The report does not identify a vtable or SharedStr type. The callback's conditional release contract is not enough to infer ownership. |
| Twelve root `__cxa_atexit` registrations | `local/validation/orch-20261027/slot309/worker-atexit-inventory/checkpoint.md` — `eea7ea70bfd14f8f079b3044a500ffb82e969717e0f7f79f8a94cab9053d6980` | Pinned bytes and saved import metadata show 12 registrations and nine distinct callback addresses. Callback registration success and callback execution at process exit were not observed. One inner member edge was still open before the next lane below. |
| `0x26ea0` tail to `0x268b0` | `local/validation/orch-20261027/slot309/worker-268b0-tail-contract/REPORT.md` — `0258efeb19607894baadb0c1f319c0f51e634a6d071513c136d5bbf8d8534eb4` | All four root paths reach `0x268b0`; the root ignores its return. Its list-vs-manager registration side effects remain material. Runtime branch state and `0x27470` were open in this report. |
| `0x2dfd4b8` writers and `0x27470` | `local/validation/orch-20261027/slot309/worker-268b0-registry-branch/REPORT.md` — `f2b1d59e1f1848201500bf6be4f325bb68d1d7b36401d5226fa872cfe4582375` | `0x27470` mutates manager records and retains copied string controls. Global `0x2dfd4b8` has explicit zero initialization and a non-null writer via `0x531d60 -> 0x255130 -> 0x255180`; relative invocation order against slot 309 remains unresolved, so neither registration branch can be selected statically. |
| `0xa47d45` callback member target | `local/validation/orch-20261027/slot309/worker-a47d20-member-target/REPORT.md` — `de505977458f16e5e0f65ed984df289ae900f0ea49fdcd0bea819abc246deb2a` | For the constructed object at `0x32f79e8`, its `+0xc0` field points to table `0x15d1b78`; the `+0x18` slot resolves through type-8 RELA and pinned bytes to one-byte `RET` at `0xa4f4c0`. This resolves only that object's edge and says nothing about interior-object type or overall cleanup completeness. |

### Observations and limits

- The manager stores raw primary-object pointers as tree values and its inspected unregister and
  tree-destruction paths remove nodes/keys without releasing those values. The manager therefore is
  non-owning on the observed keyed-map paths. The slot-309 object at `0x32f7d30` is separately
  backed by writable zero-initialized global storage and has an exit callback; its cleanup path does
  not free the object itself.
- Four objects reach the tail at `0x268b0`. That code selects either a list registration path or a
  non-null manager path according to `*(0x2dfd4b8)`. ELF memory layout and explicit zeroing support
  an initially-null pointer, but the only direct non-null writer chain begins at `0x531d60`, whose
  dynamic invocation and ordering relative to slot 309 are unresolved. **Inference:** static evidence
  does not establish which branch executes for those four objects. A native model must retain both
  behaviors; the return value being ignored does not make registration a no-op.
- The root's 12 exit registrations are statically visible, but `__cxa_atexit` success, runtime
  execution order, and actual process-exit callbacks were not tested. The callback target at
  `0xa47d45` is resolved only for the `0x32f79e8` object. The `[RBP-0x38]` temporary and broader
  object/control ownership remain unresolved.
- No source or tests changed. No original game code was executed. No native-lift boundary is
  established; do not implement or register slot 309 based on this checkpoint. Parent task remains
  **ACTIVE**, G1 remains **PARTIAL**, failed semantic hypotheses remain **0**. The three prior
  provider startup timeouts and documented tooling/address errors are cumulative and were not
  reset by these read-only lanes.

### Repository validation

Run from the workspace root. Fixture/static checks do not demonstrate game boot, game audio,
save/reload, Metal, or forbidden-runtime absence.

| Command | Result | Private log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0; configured | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/cmake-configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; no work required | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/cmake-build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/ctest.log`, SHA-256 `8f9f58642f9782c5f6fc9ee3f89711fe7a9b43e74c5725d2dd9bef6ee90b9d1c` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep layer unbuilt with 9,875 nodes pending | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/trail-build-check.log`, SHA-256 `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f` |
| `python3 scripts/project_index.py` | exit 0; 33,096 folders indexed; retained only `docs/checkpoints` 805 to 806 | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/project-index.log`, SHA-256 `e8437e14c38f15f554b8d9cd09c68413cbdc8771a1e918738c29dcca7984fc2f` |
| `python3 -I scripts/audit_publish_paths.py` and `--all` | exit 0; **7 staged paths** and **3,002 all-paths** passed | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/path-audit-staged.log`, SHA-256 `bf06adcc3fe272dfcfa7e0ade627e2a93532c043483f765ed93ae3357e6f7774`; `path-audit-all.log`, SHA-256 `7152f1ac663219cff906ba0eb1aba4e7fff10965d5fcb27b2d9566a0afd20fb2` |
| `python3 -I scripts/check_commit_attribution.py` and `--all` | unpublished-range audit pending (run after commit); all-history exits 1 only for historical non-Claude `f6604a86e60e` `orchestrator@local` | `local/validation/orch-20261027/slot309-checkpoint/e756-e757/attribution-unpublished.log`, `attribution-all.log`, SHA-256 `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` for all-history |

**Next exact action:** resolve the dynamic invocation/order of `0x531d60` relative to slot 309,
check the remaining `[RBP-0x38]` cleanup contract from the pinned inputs, and preserve any unresolved
edge. Keep both registration branches in the model unless ordering evidence closes the branch.

One initial shell redirection exited 1 because the private log directory did not yet exist, so CMake
was not invoked; after creating the directory, the configure/build/test/checker sequence completed.
This was a command setup error, not a semantic or build failure.

## Remote attribution scan after `f914bcb2` (E756)

After `f914bcb26fb201e372dcc9c772b84baf6ad82eb4`, `git fetch origin --prune` and the remote-ref
scanner exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-f914bcb2.json
```

The scan parsed **9 fetched GitHub branch heads**, **0 remote tag refs**, and **2,377 unique
reachable commits**. It found zero Claude/Anthropic author or committer identities and zero
co-author/generated-by credit-line matches. `origin/main` was
`f914bcb26fb201e372dcc9c772b84baf6ad82eb4`, authored and committed by the owner. Private report
SHA-256: `cc271397fd2b71c3740f79711ebe75d4ec4fb07d927e9fb4220cdcc0ef0608be`.

This verifies fetched public refs through `f914bcb2`; the next owner-authored checkpoint must pass
the unpublished-range and staged/all path gates before push. The local `--all` attribution audit
still reports only historical non-Claude `f6604a86e60e` (`orchestrator@local`).

**Next exact action:** complete the bounded slot-309 static lanes above, then commit only the
allowlisted documentation paths under the owner identity and push `origin main`.
