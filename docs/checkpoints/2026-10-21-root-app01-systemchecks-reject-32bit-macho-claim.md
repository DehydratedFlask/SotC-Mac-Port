# APP-01 SystemChecks 32-bit Mach-O refusal — claim checkpoint

- Local date/time: 2026-10-21 23:27 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, child of `APP-01`.
- Status: `ACTIVE`; 0 failed implementation tries. No implementation/test file changes have been made.
- Specification: `docs/TASKS.md` section `APP-01 — native Mac launcher`; `docs/PLATFORM.md` §1 requires
  an ARM64 Mach-O game core. The focused task only closes the known 32-bit-magic acceptance path.
- Scope: update `launcher/Sources/LauncherCore/SystemChecks.swift` to reject 32-bit Mach-O magic values
  in the lightweight readiness probe, and add `launcher/Tests/LauncherCoreTests/SystemChecksArchitectureTests.swift`
  with a synthetic executable i386 Mach-O header that must classify `.notBuilt`. Preserve current
  64-bit and universal magic behavior. Do not claim full ARM64 validation: the probe will still not
  inspect CPU type for 64-bit files or parse FAT slices.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-systemchecks-arch --filter AssetValidatorTests` — exit 0, 11 tests passed. Log
  `local/validation/app01-systemchecks-arch/baseline.log`, SHA256
  `f1cef398f1013d2114ddec8afec668a9751bcb8962c097319ca54f5f41ce79bc`.
- Baseline source SHA256: `launcher/Sources/LauncherCore/SystemChecks.swift`
  `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`; existing regression suite
  `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`
  `4fb1bd9d512cff6f2bf96bc8130422343825462705e0def26c727f2fa286b192`.
- Observation: `SystemChecks.isMachOExecutable(at:)` reads four magic bytes and accepts
  `0xFEEDFACE` / `0xCEFAEDFE` alongside 64-bit and universal magics. `nativeCore(executable:)` returns
  `.ready` when the file is executable and this helper returns true. Current status is from live source
  and the synthetic test baseline, not a real core or runtime readiness claim.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, two prior completion checkpoints' next-action
-  lines, generated `docs/FOLDERS.md`, and this checkpoint. Implementation and test files remain
  unchanged. Pre-existing `native/api/CMakeLists.txt` and `native/api/host/` changes remain untouched
  and unstaged.
- `python3 scripts/project_index.py` exited 0 with 24,069 folders indexed; final
  `python3 scripts/project_index.py --check` exited 0 and reported current. The first check exited 1
  because its output file had been created after index generation; after precreating the remaining
  validation logs, the index was rebuilt and the final check passed. Logs `project-index.log` and
  `project-index-check.log` SHA256 `dffbd1538d12df0acff7deb7e09ce8c1a1359d884b71022b51b0a02165663891` and
  `ec6b6843bb6af979604b7c5e26d68ccc0759932703529c9cc2264a34d5f121c1`.
- `git diff --cached --check` exited 0 (empty-output SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Staged path audit exited 0
  for 6 allowed paths (SHA256 `c8a9dc01f50942efdc7b274c6d4b85d2241b72b1e40e792d18d98537d9b8d9ad`); full audit
  exited 0 for 1,680 allowed paths (SHA256 `17afa14daa50a2815dd86b0b19b171ef9a637f3adee7a809ded0ffc4d77d6b18`).
- Pre-commit `git fetch origin main` exited 0 and `git rev-list --left-right --count HEAD...origin/main`
  returned `0 0`. Claim commit `daa92c26f8972cadbe9f97e39669577b4c8daf62` was pushed; post-push fetch
  exited 0 and divergence remained `0 0`. Push/fetch/divergence/status logs are in
  `local/validation/app01-systemchecks-arch/`. Post-push status shows only user-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/`.
- Remaining risks: rejecting 32-bit magic alone does not prove ARM64. A 64-bit x86 Mach-O may still
  pass, and FAT architecture slices are not examined. The native core remains a stub.
- Next exact action: add a test for an executable synthetic i386 Mach-O header returning `.notBuilt`,
  reject 32-bit magic in `SystemChecks.swift`, and run the focused new suite plus `AssetValidatorTests`.
