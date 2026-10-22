# Next-candidate checkpoint: SRV-01 uncounted head unlink (not claimed)

- Local date/time: 2026-10-22 07:25 CDT (America/Chicago). Integrator: /root (Codex desktop task 01a15eab-88d5-7a83-a23b-cc55dc002eb6). Candidate scout: /root/fixture_review_app_srv.
- No task claim was created, no row was added to TASKS, no implementation was dispatched, and no source file changed for this candidate.
- Candidate task: SRV-01-REGION-LIST-UNCOUNTED-HEAD-UNLINK-FIXTURE-01, parent SRV-01-REGION-LIST-UNCOUNTED-UNLINK-FIXTURE-01, target native/tests/test_region_list.cpp.
- Gap: existing coverage pins uncounted middle unlink and counted head unlink separately. One A-B-C case calling list_unlink(a) without a counter could assert B.prev == nullptr, B.next == C, C.prev == B, and A.next == A.
- Candidate scout reported that the implementation at native/core/region_list.cpp relinks the successor prev pointer and self-links the removed node. The focused target is test_region_list and CTest name region_list. The scout made no edits and ran no tests.
- Root query: graft ask "Find existing test coverage and the implementation behavior for uncounted head unlink in the native region list; identify the focused CTest target." --source. Root then read the live helper/test sections and confirmed the gap. Root live-source log SHA256 c8cc17867079ac4d75967f0b5d79d17499b642b7f253552b30df1fd5e7323932.
- Baseline HEAD 47d4a03259457dfc2a5b8eff46dcca5e6e3934b7. Test/helper/header/CMake SHA256 4f7e88ef466901894f1305cc390ef295157ba790370015e121181d436b4ce3f8 / a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda / 3f36189b27178b4ebea6135067e96c29d4839be9e255cddb6e51cd4b7f65fb69 / cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f. These paths were unchanged from HEAD.
- Root baseline commands, all exit 0:
  1. cmake -S native -B local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug
  2. cmake --build local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022 --target test_region_list
  3. ctest --test-dir local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022 --output-on-failure -R '^region_list$'
  Baseline CTest passed 1/1. Configure/build/CTest log SHA256 d4c73fd799d75d5dadbf6a50aeadcd78a07b305b5bf5c82e2b7cf0b1df9a129e / 926968de2bfcd129722bb1dca69d2c24812dd3ab7bf2942806f63583178f62cf / cb3db33aff1ceb2bfae4ddbe26cdd7a0e908ca5f0a7028aa5c22eae879d7ae77.
- Observation versus inference: this candidate covers only synthetic intrusive-list pointer behavior. It makes no allocator/runtime/game claim.
- Next exact action: next agent should reread the current task/spec, decide whether the candidate is still useful, then claim it in HANDOFF/TASKS and publish that claim before implementation. If another task is higher priority, leave this candidate unclaimed.
- Evidence logs: local/validation/srv01-region-list-uncounted-head-20261022/.
