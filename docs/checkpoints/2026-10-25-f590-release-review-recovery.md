# F590 shared-string release review and evidence recovery

## 2026-10-25 10:53 CDT — independent review and fresh validation

- Integrator: Codex `/root`; reviewer: `/root/review_f590_release`.
- Task: `CPU-01-SHARED-STR-RELEASE-F590-01`. Review subtask is closed for source/code correctness and a fresh fixture/byte-oracle replay. The full Task 2 acceptance remains **ACTIVE** because its five-mutant rejection criterion is not independently reproducible.
- Review scope: typed external deallocator, null-control no-op, sequentially consistent decrement and prior-count handling, data release before control reload/release, untouched fields, wrap cases, callback mutation, concurrency cases and byte-checker pinning. No code-level finding was reported; no source, CMake, test or checker file changed in this review.
- The checkpoint-named `local/validation/task2-release/mutants.json`, `local/validation/task2-release/manifest.json` and private `task-2-report.md` are absent. The five prior mutation cases were **NOT RUN**; the missing list was not reconstructed and no replacement mutants were invented.

### Fresh fixture and byte-oracle verification

- Pinned input: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Configure: `cmake -S native -B local/build/task2-release-recovery-20261025 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DSOTC_SANITIZE=ON` — exit 0.
- Build: `cmake --build local/build/task2-release-recovery-20261025 --target test_shared_str_release_f590 -j6` — exit 0, 503 Ninja steps; generated flags include `-fsanitize=address,undefined` and CMakeCache sets `SOTC_SANITIZE=ON`.
- Focused CTest: `ASAN_OPTIONS=detect_leaks=0 ctest --test-dir local/build/task2-release-recovery-20261025 -R '^shared_str_release_f590$' --output-on-failure` — exit 0, 1/1.
- Byte oracle: `ASAN_OPTIONS=detect_leaks=0 python3 -I tools/lift/check_ff590.py --elf '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf' --native '/Volumes/Extra/SoC Mac Native/local/build/task2-release-recovery-20261025/test_shared_str_release_f590'` — exit 0; 73-byte helper, 10-byte wrapper, ten vectors under two register poisons, callback trace/reload and wrap behavior match the pinned ELF. Allocator `0x12af0` remains external.
- All 15 entries in `local/validation/task2-release-recovery-20261025/artifact-hashes.sha256` verified with `shasum -a 256 -c` from the repository root. Logs, command list, result and mutation limitation remain private under ignored `local/validation/task2-release-recovery-20261025/`.

### Observation, inference and limits

- Observation: source review found no correctness issue; the fresh sanitizer fixture and pinned byte checker pass.
- Inference/unknown: field mapping remains inferred; allocator behavior is not modeled; the missing five-mutant run means the full Task 2 acceptance is not met. No runtime or G1 claim follows.
- Exact next action: leave the task ACTIVE until the original private mutation list/report is recovered or a separately reassessed mutation plan is recorded. Do not silently treat this recovery replay as the missing five mutations.
- Attempts: one successful implementation attempt, zero failed semantic hypotheses; this evidence-recovery review introduced no implementation attempt.
