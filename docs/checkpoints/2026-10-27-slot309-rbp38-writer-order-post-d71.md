# Slot 309 temporary cleanup boundary, writer-order audit, and post-`d71c3ccf` scan

Local date/time: 2026-10-27 02:48 CDT (America/Chicago). Integrator: `/root`.

Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.

## Slot 309 static follow-ups (E760)

Inputs use the pinned local ELF `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, the constructor export,
and prior saved disassembly/RELA evidence. The independent private reports were hash-checked by the
integrator and retain exact commands, outputs, and narrower caveats:

| Bounded lane | Private report SHA-256 | Result |
|---|---|---|
| `[RBP-0x38]` in `0xa50280 -> 0xa50440` | `local/validation/orch-20261027/slot309/worker-rbp38-lifetime/REPORT.md` — `2ec171890c965301976c0f192aafd0e1bcab001c03ad481c7d9d20583c964f4c` | Each helper frame initializes the local record through `0xa50220`, uses it as a copy source, and returns without an explicit release/destructor call on the record. The root's `0xf590` cleanups target different caller-frame locals. The record's type, copied control qword ownership, and whether cleanup is required remain unknown; do not infer triviality. |
| `0x531d60` direct caller and constructor-order boundary | `local/validation/orch-20261027/slot309/worker-531d60-order/checkpoint.md` — `9b87ba1575477607dd07bec42b7ff258487415258e3b5ec6706d3876c32a7d84`; evidence manifest `5a466f4a49f3eeac2723fef484c36381435b97535a4caaabdecf93cf791d78a3` | Existing full disassembly has no direct caller to `0x531d60`; constructor metadata has 573 entries, slot 309 is `0xa505e0`, and `0x531d60` is absent. A type-8 RELATIVE slot at `0x15c8cf8` points to `0x531d60`, but no code reference or ordered consumer is identified. Writer invocation and order remain unresolved. |
| Independent pinned-byte review of `0x531d60` | `local/validation/orch-20261027/slot309/worker-531d60-order-review/REPORT.md` — `45144fd42e68766cf5ebf8c6a797875c08760a1cb7c9ab7b07f99a894e33408b` | Finds zero direct `E8 rel32` calls and two address-taking qwords: file offset `0x15cccf8` / vaddr `0x15c8cf8` in a read-only custom range, and file offset `0x2e716a0` in custom metadata. Neither location identifies a consumer. Slot `0x2fcf0` is at constructor index 565 and the extracted walk is reverse-order, but because `0x531d60` is not in the list this does not place it relative to slot 309. Existing Ghidra references are outgoing-only; no incoming-xref export is present. |

### Conclusion and limits

- The three lanes independently preserve the same boundary. `0x531d60` is address-taken, but no
  direct caller, indirect consumer, or invocation time is identified. The extracted constructor
  walk's conditional reverse-order interpretation does not decide whether its writer chain runs
  before slot 309. Therefore `*(0x2dfd4b8)` at `0x268b0` remains unknown; preserve both list and
  manager registration branches.
- Neither helper explicitly destroys the `[RBP-0x38]` record before returning, but its copied
  `+0x10` control qword's source-level type and ownership remain unknown. The observed lack of a
  release is not evidence that no release is required.
- Parent task remains **ACTIVE**, G1 remains **PARTIAL**, and no native-lift boundary is established.
  Do not register slot 309. Failed semantic hypotheses remain **0**. Each new lane was one bounded
  successful static attempt; earlier provider timeouts and tooling errors remain cumulative.
  No game executable was run, no source/tests changed, and no runtime behavior is claimed.

### Next exact action

Use an exclusive read-only saved-project query for incoming references to `0x531d60`, inspect both
address-taking locations' consumers, and seek the source/type or destructor contract for the
`[RBP-0x38]` record's copied control qword. If those edges remain absent, retain both registration
hooks and keep slot 309 unregistered.

## Remote attribution scan after `d71c3ccf` (E759)

After publishing owner-authored commit `d71c3ccff1dd2eced3aac42aa61d1b949cbfd357`,
`git fetch origin --prune` and the remote-ref scanner exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-d71c3ccf.json
```

The scan parsed **9 fetched GitHub branch heads**, **0 remote tag refs**, and **2,379 unique
reachable commits**. It found zero Claude/Anthropic author or committer identities and zero
co-author/generated-by credit-line matches. `HEAD == origin/main` at
`d71c3ccff1dd2eced3aac42aa61d1b949cbfd357`; the working tree was clean at scan time. The new
commit's author and committer are both the owner identity.

Private report: `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-d71c3ccf.json`,
SHA-256 `65975090a3d9b746cde68f61a5e08d45d3ef580dec23c298a051c8919006790b`.

Status: **ARTIFACT_VERIFIED** for fetched public refs through this head. The local all-history
audit still identifies only historical non-Claude `f6604a86e60e` (`orchestrator@local`).

## Repository verification

Fixture and static-analysis checks do not establish game boot, game audio, real save/reload, Metal,
or forbidden-runtime absence.

| Command | Result | Private log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0; configured | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/cmake-configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; no work required | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/cmake-build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/ctest.log`, SHA-256 `d2953ccb7195e3bdac554a8062417d5d0e510143301d728da010b7ef2ea0d347` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep layer unbuilt with 9,875 nodes pending | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/trail-build-check.log`, SHA-256 `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f` |
| `python3 scripts/project_index.py` | exit 0; 33,101 folders indexed; retained only `docs/checkpoints` count 807 to 808 | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/project-index.log`, SHA-256 `1bc3cef24ecb7ba0f9bdb72bc178dd77e84afb9f11ce33983edff9bc965878d7` |
| `python3 -I scripts/audit_publish_paths.py` and `--all` | exit 0; **7 staged paths** and **3,004 all-paths** passed | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/path-audit-staged.log`, SHA-256 `7e034991909ba590a0c2db5ddaa3aa760a088828e442659705148bb8e2a26a40`; `path-audit-all.log`, SHA-256 `16b79e6fc41699dc41ab52e275b34a27bf78ecbef6503e0c99a79bd3f51a5243` |
| `python3 -I scripts/check_commit_attribution.py` and `--all` | unpublished audit pending after commit; all-history exits 1 only for historical non-Claude `f6604a86e60e` `orchestrator@local` | `local/validation/orch-20261027/slot309-checkpoint/e759-e760/attribution-unpublished.log`, `attribution-all.log`, SHA-256 `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` for all-history |

**Next exact action:** finish repository validation and path/attribution gates; commit only the
allowlisted docs under the owner identity, push `origin main`, then refresh the public-ref scan.
