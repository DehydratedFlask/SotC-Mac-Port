# Slot 309 helper targets and post-`7068f06a` attribution scan

Local date/time: 2026-10-27 02:08 CDT (America/Chicago). Integrator: `/root`.

Tasks: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.

## Slot 309 static call maps (E754)

The input was the pinned private ELF `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Three independent static-only lanes used pinned bytes, existing saved-project exports, stack/vptr dataflow, and RELA rows. Their private reports are:

- `local/validation/orch-20261027/slot309/worker-a4f4-contracts/checkpoint.md`, SHA-256 `7f46a3d19700538f9ca4ab4b631b79659f74477b3ff665f1d95825667a9ccdd3`.
- `local/validation/orch-20261027/slot309/worker-a4fec-contracts/REPORT.md`, SHA-256 `2779a33a3586bec3b178a0f7602148b302fb31828237f44341754edb673e205f`.
- `local/validation/orch-20261027/slot309/worker-a502-contracts/REPORT.md`, SHA-256 `46aa2b6a1406e33e10e852878544a4a71799bff542d633b73a5b4e43cd24515a`.
- `local/validation/orch-20261027/slot309/worker-20af0-contract/checkpoint.md`, SHA-256 `452904ac7c90666cef4c6061e7eb5e9cf952e7cc6efddec280223de0c97809a7`.

For the observed slot-309 caller and stack objects, all 11 selected indirect sites across `0xa4f4d0 -> 0xa4f690` map to `0xa4f470`, `0xa4f490`, or `0xa4f4c0`; the six sites listed unresolved in E751 are now resolved by tracing temporary vptr stores and table slots. This proof of the first `0xa4f490` target is limited to the observed direct caller at `0xa50720`. The table entry for `0xa4f4b0` is not used on this path.

All 11 selected indirect sites across `0xa4fec0 -> 0xa50080` map to `0xa4fe60`, `0xa4fe80`, or `0xa4feb0`. The `0xa4fea0` table entry is unused on this path. The one-byte `RET` target `0xa4feb0` has no established return value. The target bodies show copies and vptr stores, but the source-level object or ownership policy remains unknown.

The independent audit confirms E751's selected targets in `0xa50280 -> 0xa50440` for the observed path. It corrects one evidence description: nested call `0xa50526` receives its `0x15d1cc8` table through outer helper `0xa50280`'s `local_a0` at `[RBP-0x98]`, not directly from the root's `local_80`; the selected target remains `0xa50240`. Root cleanup at `0xa50c73` maps to `0xa50270`. The local temporary at `[RBP-0x38]` is used but has no explicit cleanup call in the two helper bodies, so its lifetime/ownership is still unresolved.

### Base helper `0x20af0`

The `0x20af0` static report records a 90-byte body with no calls or indirect transfers. It writes temporary vptr `0x1594d90`, zeroes object byte `+0x18`, copies two 32-byte records from its second and third arguments to object offsets `+0x20` and `+0x40`, and conditionally performs a locked increment through each source record's qword at `+0x18`. In both observed slot-309 helper paths, the root call arguments supply name/description records with zero at record `+0x18`, so both increments are skipped; their observed fourth argument is `1`. These byte/dataflow facts do not identify the records' type or ownership policy. The nested helper overwrites this temporary vptr and the outer helper later restores its own; a separate root call at `0xa50846` was not generalized from these paths.

### Registration boundary

An additional private static report covers the `0xa4fec0` call to `0x20d60`: `local/validation/orch-20261027/slot309/worker-20d60-contract/REPORT.md`, SHA-256 `1e351f933a1cd52687fb13b1a7441700e273465df22c3316ed3d38d72541f408`.

An independent callee audit is `local/validation/orch-20261027/slot309/worker-20d80-contract/REPORT.md`, SHA-256 `379eb8e78e4cb1248cdf88957943ed786a0653ba2b7ba3cb59fce511e11d7980`.

On the observed path, base helper `0x20af0` writes zero to object byte `+0x18`; `0x20d60` tests that byte, sets it to one, loads global pointer `0x02dfcbc8`, and tail-jumps to `0x20d80`. The cell `0x02dfcbc8` is in the ELF's memory-only BSS tail, has no relocation target, and has a zero-filled initial image; its runtime value at this call is unknown.

The static `0x20d80` body identifies `bpe::ConCmdManager::Register` from literal strings and has two paths. With a null manager pointer it initializes/uses a guarded global sentinel and links the primary object's `+8`/`+0x10` fields into an intrusive list. With a non-null pointer it locks manager `+0x58`, searches the tree rooted at manager `+0x18` by case-insensitive comparison of the 32-byte key at primary `+0x20`, and on a missing key calls `0x211d0`; it stores the primary pointer in the returned map value slot and unlocks. The equal-key path skips insertion and the primary-pointer store, then unlocks. The inspected `0x211d0` body allocates a 0x48-byte tree node, copies the 0x20-byte key representation to node `+0x20..+0x3f`, conditionally increments the key's `+0x18` control pointer, calls `0x42d30` for tree insertion/fixup, increments manager `+0x28`, and returns node `+0x40`. No store to the primary vptr is visible in these functions; the tree path retains a raw primary pointer as its value, while the list path writes intrusive links into the primary object. These observations support registry insertion of an existing object; they do not prove primary-object ownership transfer or teardown behavior. The literal `UnRegister` exists nearby, but its body and manager destructor have not yet been characterized. `0x20d60` has no rollback path after setting the byte.

Parent task remains **ACTIVE**, G1 remains **PARTIAL**, and slot 309 is not registered. These static call maps do not establish runtime reachability, object lifetime, teardown ownership, a native-lift boundary, or game behavior. No game code was executed. No source or tests were changed. Semantic hypotheses failed: **0**; prior analyzer timeouts and tooling/address errors remain counted as documented in E751. The later static audit's `readelf` lookup failed with exit 127 because the command is unavailable on this host; the repository ELF parser supplied the segment/relocation evidence, so this was a tooling miss and not a failed semantic hypothesis.

## Remote attribution scan (E755)

After pushing `7068f06a33c069420445b9c40279beb6bba71fc3`, `git fetch origin --prune` and the remote-ref scanner exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-7068f06a.json
```

The scan parsed **9 fetched GitHub branch heads**, **0 remote tag refs**, and **2,376 unique reachable commits**. It found zero Claude/Anthropic author or committer identities and zero co-author/generated-by credit-line matches. `origin/main` was `7068f06a33c069420445b9c40279beb6bba71fc3`; its author and committer are both `Johnny Wang <dehydratedflask@users.noreply.github.com>`. Private report SHA-256: `c11d04d9a046827bbc48a05db9edfd77bb6343ee08a5f822563fb1f1c6f1be0d`.

This verifies fetched public refs through `7068f06a`. The separate local `--all` check still flags only historical `f6604a86e60e` `orchestrator@local`, not Claude/Anthropic. The checkpoint commit itself must pass the owner-only unpublished-range gate before push.

## Repository validation

The fixture/checker gates below do not establish game boot, audible game content, real save/reload, or Metal acceptance.

| Command | Result | Private log |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0; configured | `local/validation/orch-20261027/slot309-checkpoint/e754/cmake-configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; no work required | `local/validation/orch-20261027/slot309-checkpoint/e754/cmake-build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `local/validation/orch-20261027/slot309-checkpoint/e754/ctest.log`, SHA-256 `04e6d9d4ebacbda26da4de256fa931f4b8a595d78f7bfb8a892cfe953b3a2aa7` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `local/validation/orch-20261027/slot309-checkpoint/e754/lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep layer not built, 9,875 nodes pending | `local/validation/orch-20261027/slot309-checkpoint/e754/trail-build-check.log`, SHA-256 `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f` |
| `python3 scripts/project_index.py` | exit 0; 33,087 folders indexed; retained only checkpoint count 804 to 805 | `local/validation/orch-20261027/slot309-checkpoint/e754/project-index.log`, SHA-256 `00cf43b47812f3f3b7f778eb04decdaad0e7088ff8bd8802c6a890e095af1b5a` |
| `python3 -I scripts/audit_publish_paths.py` | exit 0; **7 staged paths** passed | `local/validation/orch-20261027/slot309-checkpoint/e754/path-audit-staged.log`, SHA-256 `62991720e2dfb127fa79e25a6a2a7ca94a8e03d1f9fca77aecb17cab3ec89bff` |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; **3,001 paths** passed | `local/validation/orch-20261027/slot309-checkpoint/e754/path-audit-all.log`, SHA-256 `af3b6d9c27c429aed6e08d902c0456e5240440166c1adcee713be6c3ad65d929` |
| `python3 -I scripts/check_commit_attribution.py` | exit 0; unpublished range clean | `local/validation/orch-20261027/slot309-checkpoint/e754/attribution-unpublished.log`, SHA-256 `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --all` | exit 1 only for historical `f6604a86e60e` author/committer `orchestrator@local`; no Claude/Anthropic match | `local/validation/orch-20261027/slot309-checkpoint/e754/attribution-all.log`, SHA-256 `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |

**Next exact action:** trace writers of `0x02dfcbc8`, `ConCmdManager::UnRegister`, and manager teardown; check the separate `0x20af0` call at `0xa50846` and the `[RBP-0x38]` temporary. Keep G1 PARTIAL and do not implement/register slot 309 on a guessed ownership contract.
