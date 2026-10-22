# APP-01 SystemChecks 32-bit Mach-O refusal — completion checkpoint

- Local date/time: 2026-10-21 23:32 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, child of `APP-01`.
- Status: `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed implementation tries.
- Files changed: `launcher/Sources/LauncherCore/SystemChecks.swift`,
  `launcher/Tests/LauncherCoreTests/SystemChecksArchitectureTests.swift`, `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. The prior
  claim is in [the claim checkpoint](2026-10-21-root-app01-systemchecks-reject-32bit-macho-claim.md).
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-systemchecks-arch --filter AssetValidatorTests` — exit 0, 11 tests passed. Log
  `local/validation/app01-systemchecks-arch/baseline.log`, SHA256
  `f1cef398f1013d2114ddec8afec668a9751bcb8962c097319ca54f5f41ce79bc`.
- Focused command: `swift test --package-path launcher --scratch-path
  local/build/app01-systemchecks-arch --filter SystemChecksArchitectureTests` — exit 0, 1 test passed.
  Log `new-suite.log`, SHA256
  `46e838cae8f093d13111249fca6d0e8874ac78fd6a34fc6ac3823081516f35cf`.
- Regression command: `swift test --package-path launcher --scratch-path
  local/build/app01-systemchecks-arch --filter AssetValidatorTests` — exit 0, 11 tests passed. Log
  `asset-validator-replay.log`, SHA256
  `e2a68ae69dc4ce6e78a4847b8c4403a962df50e5469ef475f0ae99ceae35af6b`.
- Final source SHA256 `738addf6bcb0c1a1dae10351509310711f16f8a1ede514bb4849be94a48f437b`; final test SHA256
  `b1a39c1de3e7b7e43d41ef265bfe3b9dcf7bf4f42509b33d22160f10a05027d2`. Baseline `SystemChecks.swift`
  SHA256 was `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`.
- Observation: `isMachOExecutable(at:)` no longer accepts 32-bit `MH_MAGIC` or `MH_CIGAM` values; it
  retains the existing 64-bit and universal magic cases. The synthetic executable header identifies
  CPU type `CPU_TYPE_I386`, subtype `CPU_SUBTYPE_I386_ALL`, and file type `MH_EXECUTE`; the test
  asserts `SystemChecks.nativeCore` returns `.notBuilt`. This covers a lightweight header gate only.
- `git diff --check` exited 0. `python3 scripts/project_index.py` exited 0 with 24,073 folders
  indexed; the immediate `python3 scripts/project_index.py --check` exited 0 and reported the index
  current. Logs are `project-index.log` SHA256
  `8df23ac2e5f22996a0d4c7b38553fdeaa4b8d6dfc5099d80abddeb9bac6586b9` and
  `project-index-check.log` SHA256
  `afd91770f9394dae6212812682c1a5ffdac4fba272610e7cf85dc827707990cd`.
- Initial `trail build --check` exited 1 because the new test file was
  absent from the graph. `trail build` exited 0 with 3,607 nodes, 4,928 edges, and 947 cards, parsing
  2 of 1,240 files; final `trail build --check` exited 0 with `graph check: OK`. The deep layer remains
  unbuilt. `trail grep "rejectsExecutable32BitI386MachOHeader"` returned the new test in
  `SystemChecksArchitectureTests.swift`, proving the new source file is queryable. Graph logs are
  `trail-check-initial.log` SHA256 `bc7c687092572ad1398c9433464fda924325a3266cb713c7d1ce68b5bbb3e56d`,
  `trail-build.log` SHA256 `ee887ed563868f2e2b99ce524c3891cf3c29ed755ea810e9f2ab44f55beba28d`, and
  `trail-check-final.log` SHA256 `430fa0d7734a5d00180eeae46569c5d4a2aebf9d5f312fd6ca3fc6d28221204c`.
- Before publication, `git diff --cached --check` exited 0; the staged path audit accepted exactly
  7 allowlisted files, and `python3 -I scripts/audit_publish_paths.py --all` accepted 1,682 paths.
  The status log showed only these seven intended staged files plus the pre-existing user-owned
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/`. Audit log SHA256 values:
  diff check `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`, staged audit
  `d4be8c3a602264eea07c747ac8bd5d1bdc2a0a66ec18044c04c31420e918055c`, full audit
  `d3add3597f2d5f6af2d673f43a3f09baa5c5847d321be02d0e4f60b9949237ba`.
- Remaining risks: 64-bit non-ARM64 Mach-O files and universal slices may still pass the current
  magic-only probe. No executable was launched, and no actual core, game readiness, or G1 evidence is
  established. The native core remains a stub.
- Next exact action: select and claim another independent bounded task outside PSARC and the current
  launcher architecture files; keep the unanswered PSARC negative-limit choice pending.
