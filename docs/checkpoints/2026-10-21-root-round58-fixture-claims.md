# Checkpoint: round 58 fixture claims

- Local date/time: 2026-10-21 13:20 CDT (America/Chicago).
- Agent ID: `/root` (integrator and task claimant).
- Claimed task IDs, owners, and one-test scope:
  - `APP-01-LIFECYCLE-UNEXPECTED-EXIT-EXACT-COPY-FIXTURE-01`, owner `/root/launcher_review`, file
    `launcher/Tests/LauncherCoreTests/LauncherStatusCopyTests.swift`: assert the exact unexpected
    status copy and full recovery hint for status 9. Focused command:
    `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherStatusCopyTests`.
  - `DATA-02-OVERLAP-MAP-DUPLICATE-ARCHIVE-IDENTIFIERS-FIXTURE-01`, owner
    `/root/review_host_proto`, file `scripts/test_archive_overlap_map.py`: duplicate IDs in a
    synthetic `compare_member_maps` call raise exactly `archive identifiers must be unique`.
    Focused command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`.
  - `SRV-01-REGION-LIST-EMPTY-HEAD-INSERT-FIXTURE-01`, owner `/root/slot23_raw_xrefs`, file
    `native/tests/test_region_list.cpp`: inserting into a null head establishes the new head, null
    next, arena previous/owner, and allocation count 1. Focused commands:
    `cmake --build local/build/native --target test_region_list` and
    `ctest --test-dir local/build/native -R '^region_list$'`.
- Claim-time source/test identities:
  - `launcher/Sources/LauncherCore/LauncherStatusCopy.swift` SHA256
    `63dd6b3b27eb370310a9cc84b0ed82066530173df080f1b6269169fa815067c1`;
    `launcher/Tests/LauncherCoreTests/LauncherStatusCopyTests.swift` SHA256
    `fcb5e3b7667b235b0664064ec88828828eaf0230b82c580334634dd13da3663b`.
  - `scripts/audit_archive_overlap_map.py` SHA256
    `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`;
    `scripts/test_archive_overlap_map.py` SHA256
    `b6bd314fbae2ef9cd1e2f0d3a887c282550b110ba1afb582912cba06789720ca`.
  - `native/core/region_list.cpp` SHA256
    `a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda`;
    `native/tests/test_region_list.cpp` SHA256
    `b37576694ca3fe505aad8b0efa7db79cbafecaea07376a366f62e2f724afcee4`.
- Registry review: the three proposed task IDs are absent from `docs/TASKS.md` and `docs/EVIDENCE.md`.
  Existing parent tasks are `APP-01-LIFECYCLE-UX-01` (`FIXTURE_VERIFIED`),
  `DATA-02-ARCHIVE-OVERLAP-MAP-01` (`ARTIFACT_VERIFIED`), and `SRV-01` (active project scope).
  Existing fixtures cover clean-exit copy, archive-ID/member-map key mismatch, and nonempty-list head
  insertion or endpoint unlinking; none pins the claimed edge case.
- Claim-time state: `main` and `origin/main` both resolve to `21b0ac4`. Source and test files are
  clean. The existing user-owned modification `native/api/CMakeLists.txt` and untracked
  `native/api/host/` are excluded and untouched.
- Observations: each production branch is already implemented; these claims add focused assertions
  only. Inference limit: all are pure/synthetic fixtures; none establishes live launcher process,
  real archive, allocator runtime, or game behavior.
- Verification status: implementation and tests are `NOT RUN`; 0 failed tries for each task.
- `python3 scripts/project_index.py` exited 0 and regenerated `docs/FOLDERS.md` for 14,401 folders
  (343 checkpoint files and 93 entries under `logs`); log `logs/root-round58-claim-project-index.log`,
  SHA256 `1fce09b08a087681b67949e46317690178e639d219ac20fbf6999c6e81c8e6f9`. `trail build --check`
  exited 0 with the wiring graph in sync at 3,551 nodes / 4,790 edges; log
  `logs/root-round58-claim-trail-check.log`, SHA256
  `1d6a8b7368552c48cb7630dfe051bb32e14cd65ebc74330bcb42255d15f9027f`. The optional deep concept
  layer remains unbuilt. `git diff --check` exited 0.
- The staged path audit accepted all 4 staged files; log `logs/root-round58-claim-audit-staged.log`,
  SHA256 `41386acd7ce72037de0a3d7dcfc4167adc0185ffe83c4cf256500f9d1e39dbe5`. The full path audit
  accepted 1,610 paths; log `logs/root-round58-claim-audit-all.log`, SHA256
  `585781946f40998853916242d1376e8a33dcdb4fbc014b1882dcaa7201dbec8e`. Cached and working diff
  checks both exited 0.
- Claim publication: commit `a7c6e67` (`docs: claim round 58 synthetic edge fixtures`) was pushed to
  `origin/main`; the dispatch checkpoint records exact owners and focused commands.
- Exact next action: publish the dispatch checkpoint and handoff update, then send implementation
  work to the three owners.
