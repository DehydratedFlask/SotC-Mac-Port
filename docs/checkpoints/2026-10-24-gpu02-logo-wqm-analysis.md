# GPU-02 logo pixel shader WQM analysis

- Local time: 2026-10-24 16:13 CDT. Integrator: Codex `/root`.
- Task: `GPU-02-GCN-LOGO-WQM-ANALYSIS-01` — **STATIC_ANALYSIS_VERIFIED** for one private byte
  span; one successful analysis replay, zero failed semantic hypotheses. No MSL lowering or renderer
  integration was attempted.
- Input identity: E625's `local/fixtures/gpu-r30/logo.csdr`, 1,916 bytes,
  SHA-256 `21cde037542277bbac24fdb6d1e211f40dbb2187534ce9c31c7c59fad6b68d64`; logo PS at file
  offset `0x4d8` (1240), length `0x98` (152), SHA-256
  `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`. Existing inventory JSON
  SHA-256: `99bec189146f19d0248fb85759c252847411ea6adb06704b11aad4cfb194ab68`.
- Exact replay command: `python3 -I local/orchestrator/gpu02-logo-wqm-analysis/replay.py` — exit 0.
  It sliced the existing pinned local container in memory, regenerated the complete inventory with
  `tools/dev/gcn_inventory.py`, and matched the stored JSON after JSON serialization normalization.
  Result summary: 34/34 instructions, 152/152 bytes consumed, zero unknowns, exact final
  `s_endpgm`, and zero branch instructions. Private, payload-free replay report:
  `local/orchestrator/gpu02-logo-wqm-analysis/report.json`, SHA-256
  `91c52e19625afbe4bbf9b7918ca5a1c64ad72104671d3ae09d84000d70e1558a`.
- One command-level comparison initially exited 1 at `assert fresh == stored`: Python tuples in the
  in-memory `requirements` values differ in type from JSON arrays, despite equivalent contents. The
  diagnosis is recorded at `local/orchestrator/gpu02-logo-wqm-analysis/replay-first-compare-failure.txt`.
  The replay was corrected to compare JSON-normalized values; it then exited 0. This was a report
  serialization mismatch, not a shader-semantic failure.

## Observed instruction behavior

- At relative code offset `0x28`, `s_mov_b64 s[106:107], exec` copies the incoming 64-bit EXEC mask
  into the architectural VCC aliases. At `0x2c`, `s_wqm_b64 exec, exec` replaces EXEC with
  whole-quad-expanded bits. AMD's Sea Islands ISA says each group of four bits expands to all four
  when any one bit is set. The manual maps SGPR 106/107 to VCC low/high and 126/127 to EXEC low/high.
- Straight-line interpolation, the MIMG sample, color interpolation, component multiplies and
  float32-to-float16 packing all occur after WQM. There are no decoded branches and no EXEC writes
  between `0x2c` and the restore at `0x88`. At `0x88`, `s_mov_b64 exec, s[106:107]` restores the
  saved input mask before `exp` at `0x8c` exports compressed MRT0 with `DONE=1` and `VM=1`.
  AMD's export rules require the valid mask in EXEC for a final pixel color export.
- Before the sample, `VINTRP` P1/P2 writes attribute-1 channels to `v2` and `v3`. At `0x44`,
  `image_sample` opcode `0x20` uses `vaddr=2`, `vdata=2`, `dmask=0xf`, resource SGPR base 0 and
  sampler SGPR base 8. Its flags are `unorm=glc=da=r128=tfe=lwe=slc=0`. By the MIMG field rules,
  all four selected components are returned in consecutive VGPRs beginning at `v2`.
- Scalar loads first read four SGPRs into `s4:s7` from base `s0:s1` at encoded offset 6, then load
  sampler `s8:s11` from base `s6:s7` at encoded offset 4, then load the eight-SGPR image resource
  into `s0:s7` from base `s4:s5` at encoded offset 0. These observations establish dataflow and
  descriptor register ranges, not live descriptor values.
- The sampled components are multiplied by interpolated attr0 channels and packed for MRT0. The
  attr0/attr1 semantic names, resource dimension, texture format, tiling, sampler state, coordinate
  normalization, and engine-to-Metal binding ABI are not established by this shader span.
- The decoder marks the base `IMAGE_SAMPLE` as requiring implicit quad derivatives. The ISA lists
  `_D` and `_CD` as user-derivative variants and `_L` as user LOD; this byte stream uses base opcode
  `IMAGE_SAMPLE`, not those variants. Describing the exact hardware derivative/LOD calculation or
  claiming it matches Metal remains an inference/unknown pending a fragment-pipeline implementation.

Authoritative encoding/semantics source: [AMD Sea Islands ISA, revision 1.3](https://docs.amd.com/v/u/en-US/sea-islands-instruction-set-architecture_0),
especially §§8.2.1, 8.2.3–8.2.6, 12.3, 12.16 and 13.8. No executable or asset member was re-extracted,
and no PSARC was opened. No public game bytes, derived shader code, bindings, or screenshots were
added.

## Gates and next action

- **Observation:** the selected logo pixel program has explicit save/WQM/restore around one texture
  sample and a final masked color export. **Inference:** in native Metal, hardware fragment helper
  lanes and derivative sampling may correspond to the shader's quad expansion; equivalence is not
  proven by this static analysis.
- No tests/builds were run. G3 remains **PARTIAL**, not PASSED; real asset bindings and a real game
  frame remain unverified.
- Files changed for this checkpoint: `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/METAL_FRAME_PATH.md`,
  `docs/INDEX.md`, `docs/FOLDERS.md` (generated), `HANDOFF.md`, and this checkpoint. Private replay
  source/logs/results stay under `local/orchestrator/gpu02-logo-wqm-analysis/`.
- Next exact action: design an ordinary Metal fragment implementation for the logo sample that uses
  the native texture/sampler interface and Metal's fragment derivative behavior; first reconcile
  its texture-coordinate, descriptor, format, and filtering inputs with the already pinned logo
  material/PNG evidence. Keep unsupported bindings fail-visible. Do not label a synthetic frame as
  real-game output or advance G3 beyond PARTIAL.
- Status: `GPU-02-GCN-LOGO-WQM-ANALYSIS-01` complete as **STATIC_ANALYSIS_VERIFIED**; dependent logo
  MSL conversion and G3 asset binding remain open. Failed semantic hypotheses: 0.
