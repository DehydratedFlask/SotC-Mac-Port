# Round 65 fixture completion

Checkpoint: 2026-10-21 14:58 CDT (America/Chicago)
Integrator: `/root` — Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`
Tasks: `APP-01-CORE-ABI-SAME-MAJOR-MINOR-ORDER-FIXTURE-01`,
`DATA-02-PSARC-TRAILING-NUL-TERMINATOR-FIXTURE-01`,
`SRV-01-ALLOCATOR-REGION-MAX-ENDPOINT-FIXTURE-01`

## Completed bounded work

Root reviewed the three delegated diffs. Each worker changed exactly its assigned existing test
file; no production file or build configuration changed. Root independently replayed all focused
checks. The task register marks each task `FIXTURE_VERIFIED`, with 0 failed tries and 1 passing
implementation attempt.

| Task | Changed file / test SHA-256 | Unchanged production SHA-256 | Worker result / log SHA-256 | Root command / result / log SHA-256 |
|---|---|---|---|---|
| `APP-01-CORE-ABI-SAME-MAJOR-MINOR-ORDER-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` — `f0447d854f89d56b7c165462d5a83680be676ffad8f5d9f949b0b372134b370f` | `launcher/Sources/LauncherCore/CoreModels.swift` — `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda` | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`; exit 0, 22 tests across 2 suites; `local/validation/app01-coreabi-same-major-minor-order/attempt-1.log` — `f7d0160156824a864b57196ca6405b9301706ef5e0d1d4605934e4fb1c589df4` | Same command; exit 0, 22 tests across 2 suites; `local/validation/root-round65-replay/app.log` — `dd17faa7dcf857ee2cc660ebd461892c3f671b1fe38722987b8aadf9732969a1` |
| `DATA-02-PSARC-TRAILING-NUL-TERMINATOR-FIXTURE-01` | `scripts/test_psarc.py` — `e873256364378ab3b98e8bc683b6441d418d68a03e662f5463bc6ef7341057fb` | `scripts/psarc.py` — `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641783a1e2c140485` | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; exit 0, 35 tests; `local/validation/data02-psarc-trailing-nul-terminator/attempt-1.log` — `ea57a1768c572b4d377004773000e3b70e0c490da94bd4dfc3ff6b13b006904b` | Same command; exit 0, 35 tests; `local/validation/root-round65-replay/data.log` — `f042c1379f422264ac93c5bac2b58a94a6bc19181c8b60216ef777be3a928497` |
| `SRV-01-ALLOCATOR-REGION-MAX-ENDPOINT-FIXTURE-01` | `native/tests/test_allocator_region.cpp` — `98b885a108810d7cd8152e0630e82326dce079f6288467caae0d15c2c639dbb4` | `native/core/allocator_region.cpp` — `7ea6e93c79f43dfbad5bb447008a39c77136633f3afe97dde493d57dd69c9da8`; `native/core/allocator_region.hpp` — `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a` | `cmake --build local/build/native --target test_allocator_region`; exit 0, log `local/validation/srv01-allocator-region-max-endpoint/build-attempt-1.log` — `012296bfcfc7dc5a3f8e3b0261ca37654d84d097e256233e37f39365cf71660f`. Then `ctest --test-dir local/build/native -R '^allocator_region$'`; exit 0, 1/1; `attempt-1.log` — `7a2ef4f24c10051c8bfe68ac0be285848c4e3b97f709af5d63e492dcf2763516` | Build command exit 0; `local/validation/root-round65-replay/srv-build.log` — `40d4c8c5d57a58d1b3e380e966efa8c1c6a87b1bcbb7f7f81bfb16abb4efc181`. CTest command exit 0, 1/1; `srv-ctest.log` — `db3f8595523db6fc5b31eb9aefa526d405f1831c9339c52335ac5a5fa830669c` |

## Observations, inferences, and limits

- APP observed: `CoreABIVersion(major: 1, minor: 2) < CoreABIVersion(major: 1, minor: 3)` passes.
  Inference: the pure Swift comparator orders the minor fields when major fields match. No live ABI
  exchange or game runtime was exercised.
- DATA observed: a synthetic manifest ending with one NUL is returned as `b'one.bin\0'`, while the
  parsed path list is `['one.bin']`. Inference: the existing parser drops the empty final segment.
  This does not establish that original game archives use this convention.
- SRV observed: a region starting at `UINT64_MAX - 15` with size 15 is accepted, and its endpoint
  equals `UINT64_MAX`. This is synthetic unsigned-boundary arithmetic, not actual allocation or
  mapping behavior.
- No failures occurred. Risks remain that fixture coverage is narrow and says nothing about a
  native game executable, cold boot, authentic audio, real save/reload, or Metal gameplay.
- G0 remains PARTIAL; G1–G6 remain NOT RUN. No dependency, installer, or `INSTALL_LOG.md` facts
  changed.

## Indexes and publish preparation

- `trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests` exited 0 and rebuilt the 3,562-node / 4,816-edge wiring graph; log `local/validation/root-round65-scout/trail-build.log`, SHA-256 `b0d9e14ee32072f639571b96004844b0eeaa385a6d44f827fd6c7dd686ded077`.
- `trail build --check` exited 0; the optional meaning layer remains unbuilt. Log `local/validation/root-round65-replay/trail-check.log`, SHA-256 `7272b1c7540b89c4719537381397fa04a6dea83c0062f4a37015dece476ff497`.
- `trail_find_all` queried the three new assertion spellings in one call; it found 5 hits in the three intended test files. The returned spans are `CoreClientTests.swift:L76-L83`, `test_psarc.py:L109-L112`, and `test_allocator_region.cpp:L8-L47`.
- `git diff --check -- launcher/Tests/LauncherCoreTests/CoreClientTests.swift scripts/test_psarc.py native/tests/test_allocator_region.cpp` exited 0 with no output.
- `python3 scripts/project_index.py` exited 0, indexed 14,450 folders, and wrote `local/indexes/folders.json`; log `local/validation/root-round65-scout/project-index-complete.log`, SHA-256 `f03b0b5c677ce9f43465e4232051095307dc8ca0a226f318a95e5f3e52aee897`.
- `python3 scripts/project_index.py --check` exited 0 and reported 14,450 folders current; log `local/validation/root-round65-scout/project-index-check-complete.log`, SHA-256 `7078a2abc8e35bce9eab451acbbeb16cbc0852fad0a5d5ec54be6f470b214048`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted the 9 staged allowlisted paths;
  log `local/validation/root-round65-scout/audit-staged-complete.log`, SHA-256
  `5b2f061e54feaa96865608681d1c45eb7a8e37fcc784d73359dda689c3212486`.
- `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,635 paths; log
  `local/validation/root-round65-scout/audit-all-complete.log`, SHA-256
  `bc15d1c4b386e9ea40630b7c198e2a7026cf89895c1878cafcfd78aac1bb3d51`.
- `git diff --cached --check` and `git diff --check` each exited 0 with empty output; logs
  `local/validation/root-round65-scout/diff-check-complete.log` and
  `working-diff-check-complete.log`, each SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `git fetch origin main` exited 0; log `local/validation/root-round65-scout/remote-ref-complete.log`,
  SHA-256 `58e4bb2ebd9bd713bf3332d4cad1251aeb0b51fb26a8b06f30b3aaa431df79b3`. Before the completion
  commit, `HEAD == origin/main == 756e9f4cd2ddfa2a2ff2cc39915f3f705651a3ef`.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` are present in
  the worktree, unstaged, and excluded from the 9-path publication.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched
  and excluded from publication.

## Task status and next action

All three tasks are `FIXTURE_VERIFIED`; attempts: 1 passing, 0 failed for each. Files changed:
the three test files, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint, the claim
checkpoint addendum, and generated `docs/FOLDERS.md` after indexing. The unrelated user-owned native
API paths remain unstaged and unpublished.

## Publication

- `git commit -m "test: cover Round65 launcher, archive, allocator fixtures"` exited 0 and created
  `061d8d530d4a84213372d03289e72535f9be6102`. Commit output:
  `local/validation/root-round65-scout/commit-complete.log`, SHA-256
  `b6927548505e8e5e4e72a9bb2259fc0ccbe81f79d23893cd60ced9a9b34901f1`.
- `git push origin main` exited 0; push log `local/validation/root-round65-scout/push-complete.log`,
  SHA-256 `ea878be0ba64a486cf372d984ddd1d26059d54d473ad45fd6f39875bee28fdff`.
- Post-push status recorded unstaged user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/`; `HEAD == origin/main == 061d8d530d4a84213372d03289e72535f9be6102`.
  State log `local/validation/root-round65-scout/state-complete.log`, SHA-256
  `9df6193665622ab645d8888d6249eb7ca18cb26a26da29414d8505cd569ad4a1`.
- Exact next action: review read-only Round66 scouts, independently verify a bounded candidate with
  Graft and live files, claim/publish it, then dispatch implementation only after publication.
