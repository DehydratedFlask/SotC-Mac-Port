# Startup slot 309 helper dispatch: three custom paths

Date: 2026-10-27 01:34 CDT. Integrator: `/root`. Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.

This checkpoint records three completed read-only helper lanes. Parent task stays ACTIVE; G1 stays PARTIAL. These results resolve selected static call targets for the observed slot-309 stack objects. Other function-pointer calls and object lifetime/cleanup contracts remain unresolved; no native-lift boundary is claimed.

## Input and method

Pinned ELF `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Ghidra 12.1.4, saved project `local/ghidra/persistent-20261020/SotC`, image base 0. All Ghidra opens used `-process eboot.elf -noanalysis -readOnly`; no game code was executed.

### `0xa4f4d0 -> 0xa4f690`

The saved-project nested-helper export exited 0:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript ExportHelpers.java 0xa4f690 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/nested.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/nested-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/nested.stdout.log' 2>&1
```

One raw-context command first used an invalid colon-joined `-scriptPath` and exited 1 before opening the project (`Script not found: DumpCallContext.java`). The corrected read-only query exited 0:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4' -postScript DumpCallContext.java 0xa4f5a1 0xa4f5b2 0xa4f5c8 0xa4f5d4 0xa4f5f3 0xa4f63a 0xa4f776 0xa4f787 0xa4f79d 0xa4f7a9 0xa4f7c8 data:0x15d1b78 data:0x15d1b90 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/contexts.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/contexts-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4f4/contexts.stdout.log' 2>&1
```

Pinned-ELF target spans were inspected read-only:

```sh
objdump -d --start-address=0xa4f460 --stop-address=0xa4f4d0 local/re/eboot.elf > local/validation/orch-20261027/slot309/worker-a4f4/target-spans.log 2>&1
```

It exited 0 with an Apple LLVM section-overlap warning; inspected target bytes agreed with the saved-project Ghidra bytes.

### `0xa50280 -> 0xa50440`

The saved-project read-only export exited 0 with no lock/error:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript ExportHelpers.java 0xa50280 0xa50440 0xa50220 0x20d60 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a502/helpers.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a502/helpers-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a502/helpers.stdout.log' 2>&1
```

The separate pinned-ELF table-byte parser exited 0:

```sh
python3 -I local/validation/orch-20261027/slot309/worker-a502/inspect_vtables.py > local/validation/orch-20261027/slot309/worker-a502/vtable-bytes.json
```

Offline structural reports exited 0:

```sh
python3 -I tools/lift/analyze_slot_struct.py 0xa50240 --label slot309-a50280-vtable-plus-8
python3 -I tools/lift/analyze_slot_struct.py 0xa50260 --label slot309-a50280-vtable-plus-16
objdump -D -Mintel --start-address=0xa50240 --stop-address=0xa50269 local/re/eboot.elf
```

The `objdump` command warned that no section overlaps the requested range, then decoded the requested `PT_LOAD#0` addresses. The helper-analysis lanes changed no source and ran no tests; repository validation for this documentation checkpoint is summarized below.

## Observations

### `0xa4f4d0 -> 0xa4f690`

The root custom helper calls nested `0xa4f690` at `0xa4f627`. Nested body is `[0xa4f690,0xa4f7ee]`, 351 bytes / 89 reachable instructions. The helper constructs a stack-local vptr at `0x15d1b78`; its four qwords are `0xa4f470`, `0xa4f490`, `0xa4f4b0`, and `0xa4f4c0` at offsets `+0`, `+8`, `+0x10`, and `+0x18`.

Static target mapping for the observed slot-309 caller `0xa50720` and its nested call:

| Site | Static target | Evidence boundary |
|---|---:|---|
| root `0xa4f5a1`, `CALL [RCX+8]` | `0xa4f490` | Inferred from the observed caller's recovered stack block and vptr at offset `+0x40`; do not generalize to unseen callers. |
| root `0xa4f5b2`, `CALL [RAX]` | unresolved | Function-pointer value is not recovered. |
| root `0xa4f5c8`, `CALL [RAX]` | `0xa4f470` | Ghidra reference to vptr slot `0x15d1b78+0`. |
| root `0xa4f5d4`, `CALL [RAX]` | unresolved | Function-pointer value is not recovered. |
| root `0xa4f5f3`, `CALL [RAX+0x18]` | unresolved | Function-pointer value is not recovered. |
| root `0xa4f63a`, `CALL [RAX+0x18]` | `0xa4f4c0` | Ghidra reference to `0x15d1b78+0x18`; the callee span is one-byte `ret` (`c3`), which does not establish a useful return value. |
| nested `0xa4f776`, `CALL [RAX+8]` | `0xa4f490` | Inferred from the outer helper's local vptr at `param_7+0x40` on this observed path. |
| nested `0xa4f787`, `CALL [RAX]` | unresolved | Function-pointer value is not recovered. |
| nested `0xa4f79d`, `CALL [RAX]` | `0xa4f470` | Ghidra reference to vptr slot `0x15d1b78+0`. |
| nested `0xa4f7a9`, `CALL [RAX]` | unresolved | Function-pointer value is not recovered. |
| nested `0xa4f7c8`, `CALL [RAX+0x18]` | unresolved | Function-pointer value is not recovered. |

Observed target spans: `0xa4f470` and `0xa4f490` each contain 20 bytes ending in `ret`; `0xa4f4c0` is a one-byte `ret`. No semantic behavior was inferred from those bytes.

### `0xa50280 -> 0xa50440`

Ghidra body spans are `[0xa50280,0xa50432]` (435 bytes / 98 instructions) and `[0xa50440,0xa5059e]` (351 bytes / 89 instructions). Decompilation completed and the full pseudocode is present in `helpers.log` (SHA-256 `1faa94f86b412502c9bbabbc49b09830240a4b3ad6701a7adaae9af32c9ca1a9`). It supports stack/vptr construction and call ordering; ownership/lifetime and service semantics remain unproven.

The pinned table at `0x15d1cc8` has type-8/symbol-0 RELA slots `+0 -> 0xa50220`, `+8 -> 0xa50240`, `+0x10 -> 0xa50260`, and `+0x18 -> 0xa50270`. Ghidra maps `0xa50378` and `0xa5054d` to `0xa50220`; the table-byte parser confirms these targets. A second reference at `0x15d1ce0` maps `0xa503ea` to `0xa50270`; the pinned raw slot bytes are `70 02 a5 00 00 00 00 00`, with RELA offset `0x3db6840`. The existing root replay also records the matching slot and first target byte `c3`.

Static call mapping for the observed stack objects:

| Site(s) | Static target | Evidence boundary |
|---|---:|---|
| `0xa50351` (`[RCX+8]`) and analogous nested `0xa50526` | `0xa50240` | Inference from slot 309's recovered `local_80 = &PTR_FUN_015d1cc8` at `local_c0+0x40`, passed to this helper. |
| `0xa50362`, `0xa50378`, `0xa50384`, and nested `0xa50537`, `0xa5054d`, `0xa50559` (`[RAX]`) | `0xa50220` | Ghidra computes `0xa50378`/`0xa5054d`; other slot-zero calls are inferred after the preceding `0xa50240` vptr initialization on this path. |
| `0xa503a3`, `0xa503ea`, nested `0xa50578` (`[RAX+0x18]`) | `0xa50270` | `0xa503ea` has a direct Ghidra computed-call reference; others follow the same recovered table layout on this path. |

The `0xa50240` structural report is 20 bytes / 5 instructions and records the vptr write at destination `+0x20` and a copy from source `+0x10` to destination `+0x10`. `0xa50260` is 9 bytes / 3 instructions and copies source `+0x10` to destination `+0x10`; this table entry was not observed on the path. The second helper's direct calls include `0x20af0` and `0x20d60`; `0x20d60` tail-jumps to `0x20d80`; both custom helpers also call `0xf8f3a0` at stack-check sites. Ownership, borrowed-view, and cleanup semantics remain open.

### `0xa4fec0 -> 0xa50080`

The root export records `0xa50ad8` passing `local_c0` to `0xa4fec0`, with `local_80 = &PTR_FUN_015d1c30`. The outer body is `[0xa4fec0,0xa50072]`, 435 bytes / 98 instructions. It calls nested helper `0xa50080` at `0xa50017`, `0x20d60` at `0xa50047`, and stack-check thunk `0xf8f3a0` at `0xa5006e`.

The focused saved-project query for `0xa50080` reports body `[0xa50080,0xa501de]`, 351 bytes / 89 instructions. Its direct calls are `0x20af0` at `0xa500b6` and stack-check thunk `0xf8f3a0` at `0xa501da`. Ghidra reports `0xa5018d -> 0xa4fe60` through table `0x15d1c30`. Pinned ELF type-8/symbol-0 RELA records and raw qwords map table slots `0x15d1c30,+8,+0x10,+0x18` to `0xa4fe60`, `0xa4fe80`, `0xa4fea0`, and `0xa4feb0` respectively; the `+0x18` target is the one-byte `ret` already recorded in E750.

For this observed stack path, the outer helper's `local_a0` is its `param_7[8]` vptr, so the `+8` call at `0xa50166` maps statically to `0xa4fe80`. The caller's stack layout similarly maps outer call `0xa4ff91` to `0xa4fe80`. The remaining call map is:

| Path | Site | Static target | Evidence boundary |
|---|---:|---:|---|
| outer `0xa4fec0` | `0xa4ff91` (`[RAX+8]`) | `0xa4fe80` | Inferred from `0xa50ad8` caller's recovered stack slot and table bytes. |
| | `0xa4ffa2` (`[RAX]`) | unresolved | Temporary function-pointer value not recovered. |
| | `0xa4ffb8` (`[RAX]`) | `0xa4fe60` | Ghidra computed-call reference to table `0x15d1c30+0`. |
| | `0xa4ffc4` (`[RAX]`) | unresolved | Temporary function-pointer value not recovered. |
| | `0xa4ffe3` (`[RAX]`) | unresolved | Temporary function-pointer value not recovered. |
| | `0xa5002a` (`[RAX+0x18]`) | `0xa4feb0` | Ghidra computed-call reference to `0x15d1c30+0x18`; target is a one-byte `ret`, with return value unknown. |
| nested `0xa50080` | `0xa50166` (`[RAX+8]`) | `0xa4fe80` | Inferred from nested argument/outer `local_a0` stack fields and corroborated by table bytes. |
| | `0xa50177` (`[RAX]`) | unresolved | Returned temporary function-pointer value not recovered. |
| | `0xa5018d` (`[RAX]`) | `0xa4fe60` | Ghidra computed-call reference to `0x15d1c30+0`. |
| | `0xa50199` (`[RAX]`) | unresolved | Returned temporary function-pointer value not recovered. |
| | `0xa501b8` (`[RAX+0x18]`) | unresolved | Temporary object/vptr not recovered. |

The body/effects of `0xa4fe80` and `0xa4fea0` were not analyzed. Temporary-object calls, exact ownership/destructor behavior, and host lifetime boundaries remain open. The outer helper's `0x20d60` registration effects also remain subject to the existing native registration contract.

The initial `0xa4fec0` Ghidra attempt exited 1 with `LockException: Unable to lock project` before script execution. It was not retried unchanged; a coordinated, single-function read-only query for `0xa50080` later exited 0 and released the lock.

Failed lock-contention command:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec' -postScript ExportHelpers.java 0xa4fec0 0xa50080 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helpers.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helpers-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helpers.stdout.log' 2>&1
```

Successful focused command:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec' -postScript ExportHelpers.java 0xa50080 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helper-a50080.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helper-a50080-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/worker-a4fec/helper-a50080.stdout.log' 2>&1
```

Pinned table/RELA dump:

```sh
python3 -I local/validation/orch-20261027/slot309/worker-a4fec/dump_vtable.py > local/validation/orch-20261027/slot309/worker-a4fec/vtable-dump.log
```

The initial lock-failure logs are private: `helpers.log` SHA-256 `1e79a128e54f1139f144bd7e7a52cf05b1f247b812e20a9ef41c61cddf218834` and `helpers.stdout.log` SHA-256 `8d1513d50028b296b53af2516aa20ff1745dca977265b787e981aa16806b87ff`. Successful helper log SHA-256 `cfcd0d7432b64552ab7bff1ec4ae39beb791ff8a1c1937a374a8a67626c0824b`; script log SHA-256 `2f99089fd77d94e636fa423d288d2a3588207a28211bd017538515c5f11fb44c`; table dump SHA-256 `d1f02a1997e37d84c17d5f6a2ced4027d4dc8b5392b74faf2393060db6fb7f47`. The parser consumed the three-header ELF's RELRO view at file offset `0x1598000`, vaddr `0x1594000`, filesz `0x6d190`.

## Attempts, status, and next action

No semantic hypothesis failed. The earlier three provider startup timeouts and one corrected address-typo query remain counted. This work had one saved-project lock contention and one invalid script-path invocation; both failed before semantic analysis and were corrected/avoided. Parent task remains ACTIVE / G1 PARTIAL. Exact private evidence lives under `local/validation/orch-20261027/slot309/worker-a4f4/`, `worker-a4fec/`, and `worker-a502/`.

### Repository verification for this docs checkpoint

The following repository commands ran from the workspace root. These results verify the existing native fixtures and static checkers only; they do not establish a game boot, game audio, real save/reload, or Metal acceptance gate.

| Command | Result | Private log |
|---|---|---|
| `python3 scripts/project_index.py` | exit 0; 33,075 folders indexed | `local/validation/orch-20261027/slot309-checkpoint/e751/project-index.log`, SHA-256 `4dcb740d1df8d6a61850af2e996e5c598abfaba8f2b45ecde8a1b87dfbdba22b` |
| `cmake -S native -B local/build/orch-r22` | exit 0; configured | `.../cmake-configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; Ninja reported no work to do | `.../cmake-build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; **431/431 passed** | `.../ctest.log`, SHA-256 `8cb30c6dccd18d063c2c11edef5811f12136294068749ad2721b0f699adf38d0` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; **516 passed, 0 skipped, 0 failed** | `.../lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | exit 0; wiring graph in sync; deep meaning layer remains unbuilt with 9,875 pending nodes | `local/validation/orch-20261027/slot309-checkpoint/e751/trail-build-check.log`, SHA-256 `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f` |
| `python3 -I scripts/audit_publish_paths.py` | exit 0; **6 staged paths** passed | `.../path-audit-staged.log`, SHA-256 `d1b2dc8571cce09460ce9f08266d7b07982ab2af0669714cf3c50b8ea163f553` |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; **2,998 paths** passed | `.../path-audit-all.log`, SHA-256 `30d7729a48e9adda94e572cfd2a1eb02ad80f7b439296ed98d22abaabf30d0fe` |
| `python3 -I scripts/check_commit_attribution.py` | exit 0; unpublished commit range clean | `.../attribution-unpublished-precommit.log`, SHA-256 `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --all` | exit 1 only for historic `f6604a86e60e` author/committer `orchestrator <orchestrator@local>`; no Claude/Anthropic match reported | `.../attribution-all-precommit.log`, SHA-256 `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |

`python3 scripts/project_index.py` reported 33,075 folders; generated counts from ignored build/private validation directories were discarded, retaining only `docs/checkpoints` 801 to 802 in `docs/FOLDERS.md`. `git diff --cached --check` passed after removing one trailing blank line. `git fetch origin --prune` exited 0 and `main` had no ahead/behind commits before this checkpoint. The unpublished attribution gate passes; the all-ref gate continues to flag only the historic `orchestrator@local` commit, not Claude/Anthropic. The follow-up handoff records the owner-attributed commit, push, and a fresh scan of remote refs.

**Next exact action:** characterize the still-unresolved function-pointer callees and temporary-object lifetime paths across all three helpers; establish the minimum native boundary from direct-function contracts before considering implementation. Do not implement/register slot 309 while unresolved edges or lifetime contracts remain.
