# Startup slot 309: saved-project Ghidra coverage

## 2026-10-27 01:15 CDT — E748

Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.

This checkpoint completes a read-only Ghidra coverage subtask. The parent characterization remains **ACTIVE** because the downstream custom helper graph and lifetime behavior are unresolved. Startup gate G1 remains **PARTIAL**.

### Input and method

- Pinned local ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Reused the saved Ghidra 12.1.4 project `local/ghidra/persistent-20261020/SotC.gpr` read-only. The command used `-noanalysis -readOnly`; it did not import, reanalyze, or execute the ELF.
- The exported program reports image base `0`, language `x86:LE:64:default`, and executable path `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf`.
- Root export used `ExportSlot309.java` and exited 0. Ghidra found `FUN_00a505e0` with body `[0xa505e0, 0xa50d42]`, 1,891 body bytes, 353 instructions, 43 call instructions, one branch, and a completed decompilation.

### Observations

- The 43 root call instructions comprise 40 direct calls to ten unique targets (`0xf8f390`, `0xa4f4d0`, `0xf590`, `0x20af0`, `0x20d60`, `0xf180`, `0x26ea0`, `0xa4fec0`, `0xa50280`, `0xf8f3a0`) and three computed calls.
- The computed calls are `0xa5073c -> 0xa4f4c0`, `0xa50af4 -> 0xa4feb0`, and `0xa50c73 -> 0xa50270`. Ghidra identifies each destination as a one-byte function; direct instruction inspection reports `RET` with byte `c3` at each destination. The pinned-ELF RELA/raw-byte replay in the prior private dossier independently records the same targets and bytes. The narrow inference is that these three calls immediately return.
- The only root branch is `JNZ 0xa50d3e` at `0xa50d2d`. The success path returns; the failure path calls the `0xf8f3a0` thunk. The prior import evidence identifies that path as `__stack_chk_fail`; no claim about runtime execution follows.
- The decompiled body initializes save-related setting/command state and writes to multiple global addresses. These are static observations, not evidence that the original constructor ran or that a native port has equivalent behavior.
- The `0xf590` call target matches the repository's existing 73-byte shared release contract. `0xf180` is **not** identified here as `SharedStr`: its Ghidra pseudocode initializes a 0x20-byte object with `DAT_02dd9f78` and zeroed fields. The earlier private census label “SharedStr constructor” is withdrawn pending an exact type correlation; the project contract identifies `0xf1a0` as the native `SharedStr` constructor.

### Boundary and unresolved work

- Ghidra mapped `0x26ea0` to a 237-byte function. Its indirect calls at `0x26f4a` and `0x26f58` remain unresolved, and it tail-jumps to `0x268b0`.
- The three custom root helpers `0xa4f4d0`, `0xa4fec0`, and `0xa50280` share the same 435-byte shape. Each reaches a nested helper (`0xa4f690`, `0xa50080`, or `0xa50440`) and `0x20d60`; the prior private census records six first-hop and five nested unresolved indirect callsites per corresponding helper path. Their target values, object semantics, and lifetime effects are still open.
- The twelve root calls through `0xf8f390` are consistent with static destructor registration; a native implementation needs an explicit host-owned lifetime/RAII boundary. This is not an authorization to reproduce the original runtime environment.
- Therefore this evidence establishes static function coverage and the root call/branch map only. It does **not** establish a complete lift, startup registration, boot, audio, save/load, or Metal behavior. No native implementation or test was run for this subtask.

### Reproduction and evidence

All scripts and outputs below are private, ignored files under `local/validation/orch-20261027/slot309/`. The saved project was first reopened read-only (exit 0):

```sh
/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless \
  '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC \
  -process eboot.elf -noanalysis -readOnly \
  -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/reopen-readonly.log' \
  > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/reopen-readonly.stdout.log' 2>&1
```

The root export command exited 0:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G \
  /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless \
  '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC \
  -process eboot.elf -noanalysis -readOnly \
  -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' \
  -postScript ExportSlot309.java 0xa505e0 \
  -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/query.log' \
  -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/query-script.log'
```

Exit 0. The complete root inventory/decompilation is in `query.log` (SHA-256 `511c5dbc8c96b98b98cd3ec10116a7d0cbf6ab031c43fa4a422287da6088f0a8`). The root output is independently reviewed by `/root/slot309_coverage_review`; that read-only review found no basis to claim a lift and confirmed the unresolved helper boundary.

The remaining helper/callee/address exports each exited 0 and reused the same Ghidra project, Java environment, and flags as the root query:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript ExportHelpers.java 0xa4f4c0 0xa4feb0 0xa50270 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/helpers.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/helpers-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/helpers.stdout.log' 2>&1

env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript ExportHelpers.java 0xf8f390 0xa4f4d0 0xf590 0xa4f4c0 0x20af0 0x20d60 0xf180 0x26ea0 0xa4fec0 0xa4feb0 0xa50280 0xa50270 0xf8f3a0 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/callees.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/callees-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/callees.stdout.log' 2>&1

env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript InspectAddresses.java 0xa4f4c0 0xa4feb0 0xa50270 0xa4f470 0x15d1b78 0x15d1b90 0x15d1c30 0x15d1c48 0x15d1cc8 0x15d1ce0 0xf180 0xf1a0 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/addresses.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/addresses-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/addresses.stdout.log' 2>&1
```

`helpers-script.log` SHA-256 `412cb737bf4875a2234a0f1df9ea3de2de7c65d3535cb638a8750d2075c605e7`; `callees-script.log` SHA-256 `4e48c6724b81b0c9effdd99b86177c67cbee53fc526694d05d25353bdc088bbb`; `addresses-script.log` SHA-256 `2d2de427d695dfc41f34fba808a2acca1b4a1e5548240119e1ec75a31169ee3f` and records the three `RET c3` instructions and referenced RELRO bytes. Ghidra version is 12.1.4; Java is 25.0.4.1. No dependency was installed, so `INSTALL_LOG.md` is unchanged.

Repository bookkeeping: `python3 scripts/project_index.py` exited 0 and reported 33,064 folders. Generated counts for ignored launcher build/private validation directories were discarded; only `docs/checkpoints` was retained at 799 (previously 798). `trail build --check` exited 0; the wiring graph is in sync, with its deep meaning layer unbuilt and 9,875 nodes pending. Its private output log SHA-256 is `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f`. No code/test gate was run because this subtask changed no source.

### Status and next action

- Subtask: **STATIC_ANALYSIS_VERIFIED** for Ghidra root coverage, one branch, direct-call census, and the three root computed-call targets.
- Parent `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`: **ACTIVE**. Failed semantic hypotheses remain 0; three earlier REA/Ghidra provider startup timeouts remain recorded and were not retried. The read-only saved-project queries are a method change, not a reset of those failures.
- Exact next action: use the same saved project read-only to export pointer/RELA provenance for `0x26f4a` and `0x26f58`, then continue through the three custom helper/nested-helper paths. Keep unresolved dispatch and lifetime edges explicit; do not implement the slot until each effect has a verified native model or a fail-closed hook contract.
