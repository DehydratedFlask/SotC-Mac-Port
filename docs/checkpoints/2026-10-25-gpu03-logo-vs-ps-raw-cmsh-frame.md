# Logo VS/PS pair on raw CMSH meshes

## 2026-10-25 10:17 CDT — task claim

- Integrator: Codex `/root`; branch `main`, base `0e93d14b`.
- Task: `GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01`, **ACTIVE**, attempt 0.
- Scope: combine the E669 closed synthetic `logo-pair` MSL with the previously parsed raw
  `cube_1m.cmsh` and `cone.cmsh` POS0/U16 geometry and the E656 pinned `saveshadow_scee.png` test
  texture. Keep all game bytes and generated outputs under ignored `local/`. Apply a test-only
  position transform into disjoint visible `[0,1]` XY regions; in E669, the transformed position
  supplies both clip XY and correlated interpolated UV. Use fixed harness normals and synthetic
  shader constants.
- Hash/count gates: composed MSL 4,094 B, SHA-256
  `37e09aef0838dff77c8f151f288ed8cb2cdf7db26e202b40bb9f6f27a34dbd56`; cube 850 B, SHA-256
  `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`, 24 positions/36 indices;
  cone 1,462 B, SHA-256
  `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`, 42 positions/114 indices;
  PNG 24,636 B, SHA-256
  `b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5` (228x128 RGB before the
  documented test-only RGBA8 conversion).
- Acceptance: fail closed on partial paths, path escapes, aliases, wrong bytes, parse/count/index
  errors; compare selected interior covered pixels on both meshes against an independent CPU oracle
  for the E636 clip transform and E646 nearest/clamp sample + fp16-RTZ output; require repeat RGBA/PNG
  determinism; mutate a sampled source texel and a mesh position and require their selected output
  pixels/PNGs to change; write private baseline and mutation PNGs directly; focused Metal CTest must
  run without device skip under Metal API/GPU validation. Then run repository gates from `AGENTS.md`.
- Owner decisions already in force: use the synthetic fixture first, fixed harness normals, direct
  private PNG output, and the documented raw-asset import; defer the `0x8C` decoder. Do not infer
  TEX0 or NRM0 semantics and do not use `attach_screenshot` or invent an engine frame number.
- Implementation lane: `/root/review_logo_pair_metal` owns only
  `native/tests/test_metal_texture_binding.cpp`; root owns docs, independent verification, and Git.
- No code, parser, renderer API, dependency, or test change has been made for this card yet. Test
  status: **NOT RUN**. G3 remains **PARTIAL**; retail resource/stream ABI, AMD WQM/helper-lane
  equivalence, game-driven rendering, boot, audio, and save/reload are not established.
- Exact next action: implement the opt-in raw-mesh/pinned-PNG composed-pair case with canonical
  distinct path checks, run the focused `metal_texture_binding` CTest with its exact local inputs,
  and report outputs/hashes under `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/`.
