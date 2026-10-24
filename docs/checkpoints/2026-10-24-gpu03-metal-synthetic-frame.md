# Checkpoint: GPU-03-METAL-SYNTHETIC-FRAME-01 — direct-Metal fixture verified

- Time / timezone: 2026-10-24 15:53 CDT
- Agent and integrator: Codex `/root`; root owns the HANDOFF integration.
- Task / gate / status: `GPU-03-METAL-SYNTHETIC-FRAME-01` — `FIXTURE_VERIFIED`. Parent
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains `ACTIVE`; G3 remains `NOT RUN`.
- Prerequisite evidence / input and source hashes: E627 pins the selected default-pair MSL to
  SHA-256 `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e`. E629 records only
  `cube_1m`/`cone` identity and probe counts; E630 and E631 cover the parser fixture and typed op API.
- Owned / changed files: `native/CMakeLists.txt`, `native/platform/metal_capture.{hpp,mm}`,
  `native/platform/metal_asset_frame.{hpp,mm}`, `native/tests/test_metal_capture.cpp`,
  `native/tests/test_metal_asset_frame.cpp`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `docs/FOLDERS.md`, `INSTALL_LOG.md`, `HANDOFF.md`, and this checkpoint.
- Hypothesis and expected result: the typed pass/viewport/depth/pipeline/vertex/constants/index/draw
  subset can execute directly through Metal for a host-supplied U16 indexed mesh, read back a
  deterministic image, and match an independent CPU projection for the synthetic fixture. Invalid
  image dimensions and byte-count overflow should fail visibly before ImageIO uses the input span.
- Exact commands, CWD `/Volumes/Extra/SoC Mac Native`:
  - `cmake -S native -B local/build/orch-r22`
  - `cmake --build local/build/orch-r22 -j 12`
  - `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`
  - `ctest --test-dir local/build/orch-r22 -V -R '^metal_asset_frame$'`
  - `ctest --test-dir local/build/orch-r22 -R '^(metal_capture|metal_asset_frame)$' -j 2 --output-on-failure`
  - `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`
  - `python3 scripts/project_index.py`
  - `trail build --check` (initial stale report), `trail build`, `trail build --check` (OK), and
    `trail grep 'MetalAssetFrameRenderer'`
- Exit / observed result: configure/build exit 0; focused Metal CTest 2/2; full CTest 391/391;
  lift checkers 476 passed, 0 skipped, 0 failed. The verbose test reports `Metal API Validation
  Enabled` and `Metal GPU Validation Enabled`, with no validation fault/error diagnostics. The
  private MSL SHA is the E627 digest above. The 256x256 PNG is 6,395 bytes, SHA-256
  `35039e0fe8a9700ccfbe8bb66181426c16ea41849677633cea81ac99c0d21a94`. CPU projection reports
  33,856 covered pixels, y-down silhouette errors 0, y-down per-pixel draw/depth-class errors 0,
  y-up per-pixel errors 3,424, and exact CPU/GPU projected bounds. Repeated RGBA and PNG match;
  changing a vertex, index, or shader constant changes the PNG hash. The PNG encoder rejects zero
  dimensions, overflowing dimensions, and mismatched input bytes.
- Log, capture and REA evidence paths/IDs: verbose CTest and full validation logs are under
  `local/orchestrator/gpu03-r31/metal-frame/`; the PNG is
  `local/validation/gpu03-r31/synthetic-asset-frame.png`. No REA/Ghidra analysis was performed.
- Observation versus inference / coverage limitations: this is synthetic geometry only (a hand-built
  quad and triangle), with an explicit harness normal `(0.5, 0.5, 1, 0)` and the hash-pinned default
  MSL. The test does not call `parse_cmsh_mesh`, read real `cube_1m`/`cone` bytes, or call
  `attach_screenshot`; output is a direct test PNG and has no engine frame number. It is not an
  asset-derived game frame or retail binding proof. NRM0 meaning and retail shader/resource ABI are
  still UNVERIFIED. No G3 or gameplay behavior is claimed.
- Cumulative failed tries and distinct hypotheses: 1 successful fixture implementation; 0 failed
  implementation hypotheses. Read-only review found a PNG dimension-overflow gap; the encoder now
  checks it and its focused tests pass.
- Remaining risks / blockers: user choice remains pending on whether this slice may import real
  `.cmsh` bytes beyond E629 hash/count evidence, whether to decode NRM0 or retain the explicit fixture
  normal, and whether screenshot proof should use `attach_screenshot` with a harness sequence label or
  direct PNG output. Real asset parse/draw, the screenshot seam, retail bindings, cold boot, game audio,
  real save/reload and all final product gates remain NOT RUN.
- Publication preflight: staged path audit passed 14 paths and full path audit passed 2,746 paths;
  pre-commit attribution passed; the active branch fetch showed 0 ahead / 0 behind. Repeat the audits
  after this HANDOFF update, then commit and push the allowlisted files on the active branch.
- Next exact action after publication: follow the user's pending asset/normal/screenshot choices before
  attempting the L1 real-mesh frame.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, INSTALL_LOG,
  `docs/INDEX.md` and generated `docs/FOLDERS.md` updated. `python3 scripts/project_index.py` indexed
  9,038 folders. `trail build --check` reports 8,292 wiring nodes in sync; `trail grep
  'MetalAssetFrameRenderer'` finds the new API in the platform header and test. The deep meaning layer
  is not built and is not needed for this wiring check. Trail 0.23.0 was reused with no upgrade.
