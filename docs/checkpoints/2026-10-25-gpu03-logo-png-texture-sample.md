# GPU-03 logo PNG texture-sample fixture — 2026-10-25

**Time:** 05:16 CDT
**Integrator:** Codex `/root`
**Task:** `GPU-03-LOGO-PNG-TEXTURE-SAMPLE-01`
**Status:** `FIXTURE_VERIFIED`; zero shader-semantic hypotheses failed. One initial CTest run used the
build-directory working directory and failed the local-path guard before PNG decode or shader
sampling; the test was corrected to run from the repository root.

## Pinned inputs and replay

The exact PSARC path-MD5 entry for `misc/boot/_ps4/saveshadow_scee.ctxr` is entry 3738. Its payload
is 24,636 B, SHA-256
`b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5`; the test-only ImageIO helper
checks PNG dimensions/status and decodes tightly packed top-row-first RGBA8 with opaque alpha. The
generated logo pixel-shader MSL is pinned to SHA-256
`071a4ebe11177086c178119fafae04aae99a1a147ad8b8d1e92ee0f17abbfa71` before compilation. The source
PNG and MSL remain private under `local/`.

The Metal replay binds the decoded image as explicit slot-0 RGBA8 texture data with a synthetic
nearest/clamp sampler, synthetic quad positions and indices, and synthetic UV scales. Three
texel-center probes target source `(37,29)`, `(171,83)`, `(196,101)` and output `(16,20)`, `(46,14)`,
`(20,46)`. Each Metal readback matches an independent CPU oracle that models nearest sampling,
fp16 round-toward-zero, and RGBA8 conversion. The first probe is byte-deterministic across repeated
RGBA/PNG renders. Flipping its source red channel to the opposite endpoint changes the output pixel
and PNG.

The 64x64 RGBA baseline PNG SHA-256 is
`edd3672bca32b00f99d923cece72b885bf8b21f813eb591d3a432fde9d078ae0`; the mutated output is
`171077945500013fcabe28906a9e5d63a851445d25785a6e2f68255aba96b524`. The private input, outputs,
and logs are under `local/validation/gpu03-logo-png-texture-20261025/`.

The local artifact guard resolves existing symlinks and requires a real `repo/local` root, then
checks canonical input and output paths are distinct before any PNG writes. Regression checks cover
a nested symlink escaping `local/` and a fake repository whose `local` entry points to a tracked
directory. MSL reads and PNG writes are gated on that path preflight.

## Verification

- `cmake -S native -B local/build/orch-r22` — PASSED.
- `cmake --build local/build/orch-r22 -j 12` — PASSED.
- Full CTest with the private MSL, imported PNG, and separate local PNG output paths — PASSED,
  397/397; Metal API/GPU validation enabled, no reported faults. Log: `final4-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — PASSED, 481 passed,
  0 skipped, 0 failed. Log: `final4-lift-checkers.log`.
- `python3 scripts/project_index.py` — PASSED, 19,590 folders. Log: `final4-project-index.log`.
- `trail build` / `trail build --check` — PASSED, 8,539 nodes / 11,548 edges; the helper is queryable.
  Logs: `final4-trail-build.log`, `final4-trail-check.log`.
- `python3 -I scripts/audit_publish_paths.py` / `--all` — PASSED, 10 staged paths / 2,791 total
  paths. `python3 -I scripts/check_commit_attribution.py` — PASSED for `origin/main..HEAD` before
  the task commit.
- Focused target build and Metal CTest — PASSED; log: `review-fix3-metal-ctest.log`.
- Independent read-only code review — no remaining actionable concerns.

## Limits

This is fixture evidence for the exact pinned shader and image under the synthetic ABI. Retail
descriptor, UV/orientation mapping, AMD WQM/helper-lane equivalence, a game-driven frame, and G3 are
unverified. It does not establish boot, actual game audio, or save/reload.
