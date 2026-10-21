# Native checksum integration and loader static evidence checkpoint

- Time/agent:2026-10-20 21:48 CDT, `/root`, single integrator; base e54d4fc.
- Tasks:SAV-01-NATIVE-CHECKSUM-01 FIXTURE_VERIFIED; SAV-01-LOADER-01 STATIC_ANALYSIS_VERIFIED for bounded static coverage; parent SAV-01 remains ACTIVE/incomplete.
- Changed files:native/CMakeLists.txt,native/README.md,HANDOFF.md,docs/TASKS.md,docs/EVIDENCE.md,INSTALL_LOG.md,this checkpoint,docs/FOLDERS.md. Four checksum files were already checkpointed at85fbf7d and unchanged. No user inputs/reference changes.

## Checksum observations

Root read all four files and the retained helper/caller evidence. Luna independent review found no actionable issue and independently derived every known-answer literal. The implementation zero-extends raw bytes, XORs then multiplies in uint32_t (defined modulo2^32), returns explicit seed on empty input, and does no uppercasing. Nonnegative Save/Load dword-length domain only; original signed-negative shortcut is outside span API. Root added core/save_checksum.cpp to sotc_core and registered test_save_checksum in CTest; README maps source/domain/test command.

Commands from repo root:

```sh
python3 -I tools/lift/check_f161f50.py
cmake -S native -B local/build/native -DSOTC_SANITIZE=ON
cmake --build local/build/native -j 8
ctest --test-dir local/build/native --output-on-failure
file local/build/native/test_save_checksum
```

All exit0. CTest223/223 pass in20.46s, including save_checksum. Architecture Mach-O64 arm64; sanitizer cache ON. Existing CMake4.4.4, Apple clang21.0.0(clang-2100.3.34.2), target arm64-apple-darwin27.0.0. No install. Source checker binds ELF d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3 and 38-byte helper digest0e10cab6ef5646275365017d20e8baaf90f26bc0ac33edeb88cb97c1d105c981. Deliberately different local input passed via `--elf local/checkpoints/root-20261020-checksum-wrong-input.bin` exited1 with exact `FAIL: source ELF identity mismatch` before ELF parsing, as expected; harness exit0. Wrong-input file is synthetic, private; no source mutated.0 failed production implementation tries; worker deliberate prime mutant remains documented. This primitive is not a serializer or real-save proof.

## Loader observations and limits

Root read full verifier/report, verified28/28 manifest entries using `shasum -a 256 -c local/re/evidence/sav01-loader-01/SHA256SUMS` (exit0), then independently replayed the original checker without changing it. Exact wrapper command uses `python3 -B -I -` and:

```python
import runpy
from pathlib import Path
namespace=runpy.run_path('local/re/evidence/sav01-loader-01/verify_sav01_loader.py',run_name='root_independent_replay')
namespace['main'].__globals__['OUT_PATH']=Path('local/checkpoints/root-20261020-loader-verification.json')
namespace['main']()
```

Only output destination is redirected to preserve worker evidence. Exit0; root JSON byte-identical to worker JSON, SHA64a6dac7ca8bd78958c6d03c912b2d145eeece9536f13973e88b17093ce30548. Four body ranges,45 selected byte/canonical-row mappings,6/117 exact direct calls,14 generic-reader calls. Five bounded mutations reject integrity/capture identity; they are not five behavior tests. Report SHAe52318997e1717e92a672bb555be5b19c763abd3d19927aa32b583067d3f8666, manifest SHAdc240cf614961710e637f3eeca8fa781d8ad700b65e44f189eff2b8c679b5109. Checker and claims remain pinned to the original hashes in the stop checkpoint. Two failed schema/claims checker tries plus successful try3 remain counted; root replay passed, no new failure.

Bounded recovered observations: q1 scans hash-keyed12-byte record headers, markerBEEF, length+8/tag+6/payload+12; q2 performs object writes before later tagD/expected-length validation and reads the matched blob. Raw blob15576B and containing record15596B remain distinct. No field-name/rollback/atomicity inference is established. q2 unresolved targets5435fc/543611 and3 LLVM-only rows are retained. Hash normalization is outside this task; prior name-hash analysis is separate. Root acceptance is the byte-bound static map, not complete loader behavior/native serialization/compatibility. Native progress schema, required object/reference lifetimes, persistence and real relaunch-load are still missing.

## Next bounded startup dependency

Claim CPU-01-MUTEX-CONTRACT-01, worker `/root/aud02_waitloop_resume_audit`, owning only local/validation/cpu01-mutex-contract-01/. Characterize installed Darwin ARM64 pthread recursive mutex plus priority-inherit configuration using local SDK/current primary sources and one minimal runnable native probe. Preserve exact setter/getter/init/lock/unlock/destroy error codes and tool/environment logs. Test host API only; no guest/original-code execution. No public service implementation yet. Unsupported configuration must remain an explicit blocker, no silent fallback.0 failed task tries at claim. Root owns shared docs/Git; report before exit.

CPU-01-CTOR10-OBJECT-INIT-01 remains ACTIVE; sole provider owner is its CPU worker, doing the one authorized clean cleanup query. No additional provider permit. Slot10 still blocks startup; before any registration preserve stop at the actual next unrecovered original list slot. No leaf batch assigned.

Evidence logs listed by hash in `local/checkpoints/root-20261020-checksum-loader-manifest.json`, SHAa4565d3d7e9553d5aaab75d73be010816bc2ccc5376036d15ed00dd37ca4ebfa. Graft build refreshed14 WIP nodes:2002 wiring nodes; fnv1a32 is queryable. `graft check` reports OK, deep layer unbuilt; reference-source scope is still missing, so graph sync is not full reference coverage. G0 PARTIAL; G1-G6 NOT RUN. Next:publish this checkpoint/claim, release mutex worker, finish/replay callback evidence and choose a native ownership implementation only after those contracts are established.
