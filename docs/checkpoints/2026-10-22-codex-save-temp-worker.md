# Atomic-save temporary ownership — worker checkpoint

2026-10-22 22:12 CDT (America/Chicago). Integrator /root; worker /root/save_temp_implementer.
Task SAV-01-ATOMIC-WRITER-TEMP-ISOLATION-01: worker implementation verified; independent
review and integration pending. Attempt 1 successful; 0 failed production hypotheses.

Changed only native/core/save_stream.cpp, save_stream.hpp, test_save_stream_file.cpp.
The user chose unique exclusive same-directory temporary files. The implementation uses
exclusive open with random suffixes and preserves 0644 subject to umask. Stale symlink/hardlink
aliases and unrelated stale items remain intact. Existing downstream write/flush/rename logic
is preserved. No concurrent-writer or power-loss durability claim.

Exact commands (managed worktree): cmake -S native -B local/build/task1-save-temp-20261022
-G Ninja -DCMAKE_BUILD_TYPE=Debug -DSOTC_SANITIZE=ON; cmake --build that directory
--target test_save_stream_file -j 8; ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1
ctest --test-dir that directory -R '^save_stream_file$' --output-on-failure. Configure/build
exited 0; focused CTest passed 1/1. New regressions first failed the old implementation;
restoring the old shared-temp creation block in a private mutant builds but fails the new
referent-preservation assertion. Exact variant/mutant compiler argv are in the report.
The first detect_leaks=1 invocation aborts because LeakSanitizer is unsupported; ASan+UBSan
passed with detect_leaks=0. This is an environment limitation, not a failed product hypothesis.

Private managed evidence directory local/validation/task1-save-temp-20261022/.
Worker report .superpowers/sdd/2026-10-22-codex-parallel-foundations-plan/task-1-report.md,
SHA256 78456de453f2046d304447301316f349ab3ff664b325b110ac73bc56e91f211d.
Green CTest log SHA256 4d252032d92f1e19b41c55203107d94dd76ded08c033893cd8ad3c6e0c75670e;
manifest SHA256 46c1c71e49f03fcc2fff3a2fd88925db45d6d7ef7da2baafe2123e10d4ac793b.
Final cpp/header/test SHA256 e9fbd4a7b8f5af8018382e4e46426f46ce0eac6144715d7f1e0081063bb5e24f /
61a05dde7cbc9da42a6c0eae3e64feac0eda3d3c5293fc924aa3debcf0a6f257 /
654bf7a6d4e59c9a273556eb8440ad9881291cf54a97ce5468dce964d6a6309d.

Observation: synthetic filesystem regressions pass on arm64 under ASan/UBSan and the old
implementation is rejected. Inference is bounded to the helper contract. Real game saves,
crash recovery, deterministic concurrent writers and injected sync/close failures remain unverified.
Next exact action: independent spec/quality review of the task diff, then integration acceptance.
