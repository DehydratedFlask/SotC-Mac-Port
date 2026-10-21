# Round 75 fixture claims

- Date/time: 2026-10-21 17:25 CDT (America/Chicago)
- Agent/integrator: `/root`
- Starting revision: `af4931b8cde4767702f8c3b2ec7634aa6c3ef820` (`HEAD == origin/main`, divergence 0/0)
- Task status: three fixture tasks claimed, implementation not dispatched yet; each has 0 attempts and 0 failed tries.

## Claims and boundaries

| Task | Owner and exclusive file | Claimed assertion | Baseline |
|---|---|---|---|
| `AUD-01-CLASSIFIER-ZLIB-TRIVIAL-OUTPUT-REJECT-FIXTURE-01` | `/root/review_host_proto`; `scripts/test_audio_headers.py` | Synthetic `zlib.compress(b'X')` at offset zero is rejected as trivial: `{"ok": false, "out_len": 1, "rejected_trivial": true}`. No production classifier edit, codec identity, or playback claim. | 24 tests pass; log `local/validation/root-round75-claims/audio-baseline.log`, SHA-256 `5ff0e141575529018c47d4110194351a8373646850427f96c761e6ef3f435544`. Test SHA-256 `1cf3b2c86ebcc8bbacc98c001bfe63f9310cfd2ff6d179b26c5dfe63eb524beb`; unchanged production classifier SHA-256 `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`. |
| `APP-01-SYSTEM-CHECKS-CORE-SCRIPT-STUB-EXACT-COPY-FIXTURE-01` | `/root/launcher_review`; `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift` | Assert the exact `.notBuilt` text for a temporary script/stub: `NOT BUILT: SoCGameCore is not a Mach-O executable (refusing scripts/stubs). Play disabled until a native SoCGameCore exists`. No production launcher edit or runtime claim. | 11 `AssetValidatorTests` pass; log `local/validation/root-round75-claims/launcher-baseline.log`, SHA-256 `2f04c6e04dc58f35b0f24748e365cdf76255c8420d15c2adb5c76a9dceecc0f9`. Test SHA-256 `f10fce129f002573b34b2e99a8c3fb1fba32d84d08472ed12f9dce34a63c41e1`; unchanged production `SystemChecks.swift` SHA-256 `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`. |
| `SRV-01-HOST-MAP-ROUNDED-NONPOWER-ALIGNMENT-FIXTURE-01` | `/root/round68_native_scout`; `native/tests/test_host_map.cpp` | Assert `reserve_alignment(0x300000u) == 0x300000u` and `map_anonymous_aligned(0x1000u, 0x300000u)` returns empty. This is the synthetic non-power-of-two early guard only; no successful map or allocator/runtime claim. | `cmake --build local/build/native --target test_host_map` exits 0; `ctest --test-dir local/build/native -R '^host_map$' --output-on-failure` passes 1/1. Build log SHA-256 `c6a4b5ca13ce84a970f60a6231a0c43038e207bf62d80ce0e8e2926e5a0a94d4`; CTest log SHA-256 `51505c2fe454268e72b39931b438a25eca933900c86723aba8bfcb4df7ed72dd`. Test SHA-256 `e87631c3043b8d2bb9b6dda7c5c2b2a253b6fa5bff8a13d70f7d8dc0d786e2af`; unchanged `host_map.cpp` SHA-256 `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`; `host_map.hpp` SHA-256 `283c3b8f0c98ccb48cb7567e96ba4fd54c98babc1ea60b942f2cc2276c30bbe`; `allocator_region.hpp` SHA-256 `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a`. |

## Commands and results

- Audio baseline: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` — exit 0, 24 tests passed.
- Launcher baseline: `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests` — exit 0, 11 tests in one suite passed on `arm64e-apple-macos14.0`.
- Native baseline: `cmake --build local/build/native --target test_host_map` — exit 0; then `ctest --test-dir local/build/native -R '^host_map$' --output-on-failure` — exit 0, 1/1 passed.

## Observations, inference, risks

- Observed: the baseline suites/builds pass before any fixture edits. The source and test hashes above identify the claimed pre-change files. The local Trail query places the non-power-of-two check immediately after `reserve_alignment()` and before `mmap`.
- Inference: the proposed `0x300000` value survives the 64 KiB and 1 MiB rounding steps and reaches the early `is_pow2` rejection; this should be checked with the assertions, not presented as an actual mapping experiment.
- No implementation failure has occurred. A baseline pass does not yet prove the proposed assertion fails before implementation; workers must record that result or state why it is already covered.
- Production files must remain unchanged. The two pre-existing user-owned paths `native/api/CMakeLists.txt` (modified) and `native/api/host/` (untracked) are outside every claim and must remain untouched and unstaged.
- G0 remains PARTIAL; G1–G6 remain NOT RUN. No audio, launcher runtime, mapping integration, or game behavior is established.

## Claim files and next action

- Claim files: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and the generated `docs/FOLDERS.md` index.
- Current attempt counts: all three 0 implementation attempts, 0 failed tries.
- Next exact action: run `python3 scripts/project_index.py` and its `--check`, run `trail build --check`, stage only the claim documents, run staged and `--all` publication audits, commit/push the claim, fetch and verify `HEAD == origin/main`, record the publication state, then dispatch the single-file implementation tasks.
