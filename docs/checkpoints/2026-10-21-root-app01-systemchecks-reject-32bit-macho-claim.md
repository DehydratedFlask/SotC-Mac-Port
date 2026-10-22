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
  lines, and this checkpoint. Implementation and test files remain unchanged. Pre-existing
  `native/api/CMakeLists.txt` and `native/api/host/` changes remain untouched and unstaged.
- Remaining risks: rejecting 32-bit magic alone does not prove ARM64. A 64-bit x86 Mach-O may still
  pass, and FAT architecture slices are not examined. The native core remains a stub.
- Next exact action: add a test for an executable synthetic i386 Mach-O header returning `.notBuilt`,
  reject 32-bit magic in `SystemChecks.swift`, and run the focused new suite plus `AssetValidatorTests`.
