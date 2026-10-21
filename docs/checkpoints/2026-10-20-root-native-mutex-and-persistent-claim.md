# Native mutex fixture verified; persistent offline analysis claimed

- Time/agent: 2026-10-20 22:05 CDT, `/root`, single integrator; base `02db1d4`.
- Task SRV-01-RECURSIVE-MUTEX-01: FIXTURE_VERIFIED. Root read the full implementation/test and integrated CMake/README. New files: native/core/recursive_mutex.hpp, native/core/recursive_mutex.cpp, native/tests/test_recursive_mutex.cpp. Native owner uses PTHREAD_MUTEX_RECURSIVE/PTHREAD_PRIO_INHERIT symbols, explicit init/operation/close errors, noncopyable/nonmovable storage, retained ownership after EBUSY destruction, and RAII destruction. Unreportable destructor or temporary-attribute cleanup error terminates visibly; lifecycle transitions require external synchronization. No fallback.
- The user chose StartupState RAII ownership in response to the pending lifetime question. This decision is settled; do not re-ask. Actual StartupState/slot10 wiring remains blocked on the independent descriptor publication contract, not on this preference.

## Verification and failed attempts

Root commands from repo root:

```sh
cmake -S native -B local/build/native -DSOTC_SANITIZE=ON
cmake --build local/build/native --target test_recursive_mutex test_startup_ctors test_save_checksum -j 8
ASAN_OPTIONS=halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ctest --test-dir local/build/native -R '^(recursive_mutex|startup_ctors|save_checksum)$' --output-on-failure
file local/build/native/test_recursive_mutex
```

All exit0;3/3 focused tests pass; native binary Mach-O arm64. Full223-test suite passed at previous checksum checkpoint; a new224-test full-suite run was NOT RUN here. Existing Apple clang21.0.0/macOS27.0.1/SDK27.0/CMake4.4.4 reused. No install/download. Worker standalone build includes -DNDEBUG and ASan+UBSan; explicit CHECK remains active. Test covers uninitialized errors, duplicate init, depth2/depth1 competing-thread exclusion, busy close preserving ability to unlock, acquisition after final unlock, clean close/reinit, and scope-owned cleanup. Root observed no sanitizer report in focused tests. Scheduler priority inheritance under contention remains NOT TESTED.

Two failed harness/configuration attempts remain counted: worker first sanitizer launch requested unsupported `detect_leaks=1` and exited134 before test execution; the same binary passed after only that option was removed. Initial scratch-mutant build lacked native/core include path and failed before link; one explicitly approved corrected command added it and compiled. The mutant clears initialized_ even on failed native destroy and was rejected at the following unlock check, exit1 as intended.0 demonstrated product implementation failures; cumulative failed setup/check attempts2. Original and corrected logs/commands retained under local/build/recursive-mutex-worker/. A third unsuccessful distinct attempt requires escalation; no automatic retry budget reset.

Source hashes: header f97aea6beb99aed0bc5348bb92dc70d059b965558d1eaef60c4c57600678d5df; cpp f61fb74be7c771b35203a9cf98d658fd3bba4d00c2c023e19404b37a4f18b6aa; test656bcccc6c9bb11566b9920e02a68b13032efbb4ff816ca3285fce60925ea6d9. Root/worker log identity manifest `local/checkpoints/root-20261020-native-mutex-manifest.json`, SHA `a07c97ebb33aee3d2b14e8705a98f6f279b647678bc1c1e8b34872cb7f787f77`. Graft refreshed new files and check reports sync; reference coverage is still missing.

## Existing CPU object evidence review

Worker cleanup/initializer dossier currently passes134 local assertions, but root requested two scoped review corrections before acceptance: qualify descriptor mapping as PT_LOAD-only (other headers require separate audit), and accurately cite prior E53 proof that all3 computed calls are skipped on the specific byte-constructed zero-local path. Other states/reachability remain unknown. Preserve pre-review artifacts; root has not replayed/accepted corrected dossier yet.2 documented failed targeted attempts are compiler-cache denial and checker root-path assumption. Worker additionally reports2 prior JSON-inspection errors with exact commands/tracebacks unavailable; provenance gap remains explicit and is not invented or silently deleted. These diagnostic reads are not accepted evidence or documented distinct recovery hypotheses. No more provider query assigned.

Descriptor worker retains2 failed inspection-command assumptions,0 rejected semantic hypotheses. It has located a uniquely file-backed PHDR#1 (community label PT_SCE_RELRO) and relocation-bound executable candidates; table/class boundary and native interface remain unresolved, report/replay pending. Do not infer a222-method class from a contiguous relocation run. No slot registration.

## RE-01-PERSISTENT-01 claim — root-owned

Own only new private local/ghidra/persistent-20261020/ and local/checkpoints/root-20261020-persistent-*; no original/reference/install modifications. Follow the existing REVERSE_ENGINEERING persistent recipe with installed Ghidra12.1.4/OpenJDK25.0.4.1, eight-GiB heap and max4 analysis CPUs. Destination did not exist at claim. Import exact pinned ELF once, retain project (no overwrite/delete),20-minute analysis timeout; retain logs and terminal status. Then reopen the saved program read-only with -noanalysis to verify durable access. This is a distinct default headless-analysis profile, not the REA profile or a runtime/coverage claim. Record warnings, unresolved/missing special mappings and actual program name.0 failed persistent-project tries at claim. No automatic retry if it fails; principal reassess and retain earlier REA timeout/cache history.

Exact import to execute only after claim publication:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -import '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf' -analysisTimeoutPerFile 1200 -max-cpu 4 -log '/Volumes/Extra/SoC Mac Native/local/checkpoints/root-20261020-persistent-import.log'
```

No decompilation/recovered-code completion follows merely from a project file or successful import. No Sony executable code is run. Next exact action: index/check/audit/commit/push this native checkpoint and analysis claim; release mutex worker after recording, run persistent import, complete CPU evidence review and descriptor recovery. G0 PARTIAL; G1-G6 NOT RUN.
