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

## 2026-10-25 11:11 CDT — follow-up search for the five prior mutations

- Read-only worker `/root/f590_mutation_recovery` searched the named recovery paths and tracked F590
  code. Both `local/validation/task2-release/mutants.json` and
  `local/validation/task2-release/manifest.json` are missing; `find local -type f -iname
  'task-2-report.md' -print` returned no path. The search also found no alternate exact manifest or
  report under `local/`.
- Exact successful commands:
  - `for candidate in local/validation/task2-release/mutants.json local/validation/task2-release/manifest.json; do if test -e "$candidate"; then printf 'PRESENT %s\n' "$candidate"; else printf 'MISSING %s\n' "$candidate"; fi; done; find local -type f -iname 'task-2-report.md' -print` — exit 0; both files MISSING, report absent.
  - `find local -type f \( -iname 'mutants.json' -o -iname 'task-2-report.md' -o -path '*/task2-release/manifest.json' \) -print` — exit 0; no matches.
  - `git grep -n -i -E 'mutant|mutation|enum class Mutation|constexpr Case cases' -- native/tests/test_shared_str_release_f590.cpp native/tests/test_shared_release_f590.cpp tools/lift/check_fshared_release_f590.py tools/lift/check_ff590.py` — exit 0; tracked F590 code has only callback mutations `none`, `clear_ctl`, `replace_ctl`, ten state vectors, and no definitions of the five prior source mutants.
- One initial zsh search wrapper used `path` as its loop variable, colliding with zsh's special PATH array; it exited 127 after reporting the two missing files. The corrected `candidate` loop above exited 0. No tests were run and no replacement mutants were created. This does not change the implementation attempt count or satisfy the missing mutation criterion.
- Exact hashes from the read-only worker: `mutation-limitation.txt` SHA-256
  `96473e934ea193d4f2be15f640e6613851ab59f344c9557fead64aa9195b3eee`, current F590 test source
  `7e6e1dac8d523fa1137e94b0f9f1a2752a0d961af08d2a5bc9ebde723575fce1`, checker
  `e1046b2f27133929e03c160406c934486cc10cf71dbbcaf727c3d9e86d8d9121`. No files were edited.
- Status remains ACTIVE; next action is still to recover the original cases or obtain an explicit
  principal reassessment with precisely specified replacements. No runtime/G1 claim follows.
