# Startup slot 309: `0x26ea0` virtual-call resolution

## 2026-10-27 01:26 CDT — E750

Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.

This read-only static-analysis subtask resolves both virtual-call sites in `0x26ea0` for all four calls made by slot 309. The parent task remains **ACTIVE**: three custom helper paths still contain unresolved indirect calls and lifetime effects. Startup gate G1 remains **PARTIAL**.

### Evidence

- Input is the pinned `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. All queries reused Ghidra 12.1.4's saved project under `-noanalysis -readOnly`; no binary was executed and no project analysis was written.
- Ghidra decompilation of `0x26ea0` shows indirect calls through `param_4` vtable slots `+0x18` at `0x26f4a` and `+0x30` at `0x26f58`. Slot 309 calls this helper four times, passing `0x32f79e8`, `0x32f7af0`, `0x32f7d30`, and `0x32f7eb0` as `param_4`.
- The final call-time vptr and the corresponding Ghidra DATA references resolve as follows:

| Slot-309 callsite | `param_4` object | Call-time vptr | `+0x18` target | `+0x30` target |
|---|---:|---:|---|---|
| `0xa5096c` | `0x32f79e8` | `0x15d1ba8` | `0xa4d490`: `mov eax,1; ret` | `0xa4d760`: `mov eax,1; ret` |
| `0xa509f7` | `0x32f7af0` | `0x1594218` | `0xb890`: `mov eax,1; ret` | `0xb930`: `xor eax,eax; ret` |
| `0xa50b75` | `0x32f7d30` | `0x15d1c60` | `0xa4e190`: `mov eax,1; ret` | `0xa4e310`: `mov eax,1; ret` |
| `0xa50cf4` | `0x32f7eb0` | `0x15d1cf8` | `0xa4e830`: `mov eax,1; ret` | `0xa4e9b0`: `mov eax,1; ret` |

- For `0x32f7af0`, the root sets vptr `0x1594218` before calling the `0x20d60` registration wrapper. That wrapper reaches `0x20d80`. The decompilation of `0x20d80` writes fields beginning at `param_1+8` or `param_1+0x20` and does not write `param_1+0`; the call-time vptr therefore remains `0x1594218` in the analyzed path. The earlier table `0x15940b0` is overwritten before `0xa509f7` and is not the call-time vptr.
- Every `+0x18` method returns `1`; the `+0x30` methods return `1` for three objects and `0` for `0x32f7af0`. `0x26ea0` tests whether the first result equals `1`, then whether the second equals `3`; the latter condition is false in all four slot-309 cases. **Inference:** the conditional property adjustment is not taken on these four paths. This is an inference from static callsite/vptr/byte evidence, not runtime behavior.
- Independent read-only review by `/root/slot309_vtable_review` flagged the distinction between the temporary `0x15940b0` and call-time `0x1594218`; root then verified the final vptr, data references, method bytes, and the `0x20d80` write set in the saved project.

### Failed query and limits

An initial vtable-byte query used the mistyped address `0x15d940b0` instead of `0x15940b0`. The script stopped at an unmapped address after printing only the first three entries. The Ghidra log contains `MemoryAccessException`; the project was read-only and unchanged. This was an address transcription error, not a semantic hypothesis. The corrected queries completed successfully. Failed semantic hypotheses remain 0; the three earlier provider startup timeouts remain recorded.

The failed query's exact command was:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript InspectAddresses.java 0x15d1ba8 0x15d1bc0 0x15d1bd8 0x15d940b0 0x15d940c8 0x15d940e0 0x15d1c60 0x15d1c78 0x15d1c90 0x15d1cf8 0x15d1d10 0x15d1d28 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-slots.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-slots-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-slots.stdout.log' 2>&1
```

The query process returned exit 0, but `vtable-slots.log` contains a Ghidra `REPORT SCRIPT ERROR`; the result is **PARTIAL/FAILED** for that address list. The later corrected commands above exited 0.

This resolves `0x26ea0` only for the four objects constructed by slot 309. It does not close the custom helper/nested-helper indirect edges, certify all indirect callers of `0x26ea0`, or support a native lift or game-behavior claim.

### Reproduction

Ghidra version 12.1.4; Java 25.0.4.1. The following commands used the saved project, `-noanalysis -readOnly`, and private logs under `local/validation/orch-20261027/slot309/`.

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript InspectReferences.java 0x1594218 0x1594230 0x1594248 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable-refs.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable-refs-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable-refs.stdout.log' 2>&1

env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript DumpAddressSpans.java 0xa4d490 0xa4d760 0xa4e190 0xa4e310 0xa4e830 0xa4e9b0 0xb890 0xb930 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-target-spans.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-target-spans-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/vtable-target-spans.stdout.log' 2>&1

env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript ExportHelpers.java 0x20d80 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/concmd-register-helper.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/concmd-register-helper-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/concmd-register-helper.stdout.log' 2>&1

env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -scriptPath '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309' -postScript InspectAddresses.java 0x1594218 0x1594230 0x1594248 -log '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable.log' -scriptlog '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable-script.log' > '/Volumes/Extra/SoC Mac Native/local/validation/orch-20261027/slot309/final-vtable.stdout.log' 2>&1
```

The final vtable-reference log SHA-256 is `4a68c1f8362d90f2f545c370de7f174926e21b819b4abbbebf578f719b4d5513`; the method-span log SHA-256 is `1c6a3785a3a731e83e4d04bf2e5aff755b28c67f4f682064ff292bd9d9d2aa63`; the registration-helper log SHA-256 is `569a737199f6e926365a5ea0af755abcde05d1d4a733070fbe94ecde926603bb`. The latest remote attribution scan after `9b0ed4d3` parsed 9 heads, 0 tags, and 2,372 commits with no Claude/Anthropic matches; private report SHA-256 `db4db48c17c54db94704ac92e163c789336fe682782b293e1bec4f1de755cee2`.

Repository bookkeeping: `python3 scripts/project_index.py` exited 0 and reported 33,068 folders. Counts for ignored launcher build/private validation directories were discarded; `docs/checkpoints` increased from 800 to 801 in `docs/FOLDERS.md`. `trail build --check` exited 0; the wiring graph is in sync, while the deep meaning layer remains unbuilt with 9,875 pending nodes. Its private log SHA-256 is `00119eb6b8e66856a413c1ec07c3322f374159b225f828749d8d6f805ed4df5f`. No source/tests changed.

### Next action

Continue the parent task by separately resolving callsites and object/lifetime effects in `0xa4f4d0 -> 0xa4f690`, `0xa4fec0 -> 0xa50080`, and `0xa50280 -> 0xa50440`. Keep all remaining indirect edges explicit; no implementation or G1 change is authorized by this evidence.
