# GFX7 bounded instruction inventory checkpoint

- Local time: 2026-10-24 01:56 CDT. Worker: `/root/shader_decode`.
- Task: `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01`, inventory subtask.
- Owned public files: `tools/dev/gcn_inventory.py`, `tools/dev/test_gcn_inventory.py`, this checkpoint.
- Status: **STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED** for instruction boundaries and opcode
  identification only. Metal conversion, Metal numerical equivalence and G3: **NOT RUN**.

The original stdlib-only decoder implements a deliberately bounded set of 29 opcode/encoding
pairs across ten GFX7 encoding formats. It checks exact widths, literal words, reserved fields,
operand restrictions, and a single final `s_endpgm`; it never skips a dword to regain alignment.
Unsupported instructions fail visibly. Its input is an exact private program span. Generated
listings, disassembly and derivative shaders must remain private; no game bytes or asset-specific
instruction list are embedded in the public tool or tests.

| Private selected program | Declared / consumed bytes | Instructions | Unknowns | Boundary result |
|---|---:|---:|---:|---|
| default VS | 272 / 272 | 54 | 0 | final `s_endpgm`, exact end |
| default PS | 240 / 240 | 55 | 0 | final `s_endpgm`, exact end |
| optional logo VS | 140 / 140 | 25 | 0 | final `s_endpgm`, exact end |
| optional logo PS | 152 / 152 | 34 | 0 | final `s_endpgm`, exact end |

The default pair has no decoded whole-quad, EXEC-mask manipulation, LDS/direct-LDS or implicit
texture-derivative requirement. This is a static subset observation, not semantic equivalence.
The optional logo PS **does** use whole-quad mode, EXEC-mask save/restore and implicit texture
sampling derivatives. The older encoding-class-only claim did not exclude those operations.
Logo conversion remains outside the default-only next slice; no wave runtime is introduced.

Validation commands, all from the workspace root:

```sh
python3 -I -W error tools/dev/test_gcn_inventory.py
python3 -I -W error local/orchestrator/r30/verify-gcn-inventory.py
```

Results: **23/23** synthetic tests; **1,000** seeded independent field vectors; **804/804** private
truncated prefixes and **168/168** private targeted opcode/reserved-field mutations rejected.
The separate synthetic oracle concatenates MSB-first ISA fields; the decoder extracts little-endian
fields. Private reference-table verification checks **28/28** opcode/form values plus the separately
specified EXP format against the read-only source pin `70bb43859cd7ff181a6d409540b757866184d60d`.
This is an encoding oracle and reference cross-check, not a vendor disassembler or execution oracle.

One verification-harness experiment failed: setting VOP2's opcode to all ones changes its prefix
into VOP1, and one mutation was a valid supported instruction. The retained failure log is
`local/orchestrator/r30/gcn-corpus-verification-v1-failed.log`. The revised mutation uses unsupported
VOP2 opcode 61, preserving the encoding. No decoder behavior or acceptance test was weakened.
Implementation attempts: 1 successful decoder implementation; no failed shader-lowering hypothesis.
No dependencies were downloaded or installed.

Encoding source: [AMD Sea Islands ISA revision 1.3, December 2013](https://docs.amd.com/v/u/en-US/sea-islands-instruction-set-architecture_0),
chapters 6, 7, 11 and 13. Opcode numbers were only cross-checked against the pinned reference;
no GPL implementation was copied. The manual has inconsistent SMRD/VOP3 bit-range typography:
non-overlapping field widths are verified by the independent concatenation fixtures and reference
field extraction. Exact rules, source URLs, private hashes, operand/control evidence and full
command results are in `local/orchestrator/r30/shader-decode-report.md` and
`local/fixtures/gpu-r30/{manifest,verification}.json`.

Next exact action: integrator records this mandatory pre-generation checkpoint in `HANDOFF.md`,
then continues the owner-selected default-only conversion slice. Define an explicit import-time
resource ABI, preserve FP16 round-toward-zero and scalar-memory dataflow, emit ordinary MSL
expressions, and compare with a separately written matrix/lighting oracle before any harness
frame claim. Unknown resource binding or default opcode semantics must stop conversion visibly.
The real engine's live binding path remains unverified. The source graph did not index the pinned
reference scope; trail located the public probe API, then live reference files were checked read-only.

## Default-pair harness follow-up — 2026-10-24 14:55 CDT

After integrator acceptance of the inventory checkpoint (E625), the owner-selected default-only
slice now has an offline SSA expression emitter, a separate mathematical oracle, and a standalone
ARM64 Metal test executable. New public tools: `gcn_to_msl.py`, `test_gcn_to_msl.py`,
`default_shader_oracle.py`, and `metal_shader_verify.mm`, all under `tools/dev/`.
No native runtime, CMake, reference source or shared document was edited by this worker.

**FIXTURE_VERIFIED / ARTIFACT_VERIFIED, accepted by integrator as E627:** the selected private default
spans produce ordinary MSL expressions. There is no shader bytecode/register dispatcher in the
result. Missing resource cells, wrong digests, unknown formats/interpolation/FP profiles and
in-repository public output paths fail visibly. Game-derived MSL, ABI manifests and outputs stay
under `local/fixtures/gpu-r30/`. Optional logo WQM remains unsupported.

The explicit **harness-only** ABI supplies float3 streams, matrix rows, camera/material values and
interpolation coefficients. The tested numeric profile is nearest-even binary32 with explicit
binary32 subnormal flushing and binary16 round-toward-zero. Original runtime descriptor formats,
retail FP state, packed mesh normals and SPI interpolation controls remain **unverified**. Native
raster interpolation is only an explicit harness contract. These results establish neither those
retail bindings nor a real mesh import, engine frame or G3; **G3 stays NOT RUN**.

Validation on Apple M4 Pro:

- Public synthetic tests: **23/23 inventory + 11/11 lowering/oracle**.
- Independently written matrix/interpolation/lighting oracle: **2,141 cases** (2,048 seeded random,
  90 edge cases, 3 raster cases), **34,256 output components** agree; maximum finite absolute
  difference **0**, with nonfinite class/sign checks. **2,131 finite alpha results** match exactly.
- Predeclared bounds: transform outputs `2e-5 * max(1, abs(expected))`; RGB at most one binary16
  step for reciprocal-square-root/quantization effects; finite alpha exact. Edge cases include
  NaN, infinities, signed zero, binary32 subnormals and half-conversion boundaries.
- Metal.framework source compilation and compute/offscreen pipelines completed with
  `MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1`, `fastMathEnabled=NO`; no validation fault reported.
- Independent CPU projection of the supplied triangle: **4,608 covered pixels**, no pixel outside
  bounds, no missing strict-interior pixel, and no color mismatch (observed max difference 0).
  This triangle is synthetic input for the recovered shader, not a scene/game proof.
- Vertex and generated-shader-constant mutations change output; invalid MSL and truncated input
  fail visibly. **8/8** missing/wrong ABI and publication-boundary cases reject conversion.

The installed `xcrun metal` launcher fails because its optional Metal Toolchain component is
absent. That failed environment check is retained; no download was performed. The installed
Metal.framework compiler succeeded. The original standalone harness was compiled with Apple clang
21.0.0 (`clang-2100.3.34.2`), links only system frameworks/libraries, and is a thin arm64 Mach-O.

Exact recipes, digests, commands and results:
`local/orchestrator/r30/shader-decode-report.md`, `default-metal-checkpoint.md`,
`default-{metal-execution,oracle-comparison,mutations-raster,binding-rejections}.log`;
`local/fixtures/gpu-r30/{default-lowering,oracle-comparison,mutation-and-raster-verification}.json`.
No dependency installed; private tool executable removal boundary is `local/fixtures/gpu-r30/`.
Routine first-run Python typo/test-diagnostic corrections are retained in the private report;
no unsuccessful shader semantic hypothesis was hidden or replaced by a fallback.

Integrator independently reviewed/replayed the emitter and oracle, regenerated byte-identical
MSL, rebuilt/reran Metal and accepted this bounded result as E627. Full round-30 checks are
recorded separately as E628. Next exact action: integrator publishes allowlisted source only. A subsequent graphics task must recover actual mesh/descriptor/interpolation contracts
before asserting a real asset frame or promoting G3.

Rounding source clarification from review: the GFX7 basis is Sea Islands ISA 1.3 section 12.10,
printed page 12-124 (PDF index 221), which defines `v_mad_f32` as IEEE multiplication then addition;
section 12.7 page 12-59 (PDF index 156) defines `v_mac_f32` with destination as accumulator.
Both paths are exercised by the private default-pair run. A later AMD GCN3 1.1 section 13.3,
page 13-19 (PDF index 302), explicitly maps MAC internally to MAD; that is corroboration from
GFX8 and is **not** this decoder's GFX7 encoding source. Primary LLVM AMDGPU FMAD/MAC handling
at commit `65d651bc8fd6d83ae4989d93dc89cb287231afd2` also corroborates separate-operation FTZ
arithmetic. Exact URLs are retained in the private report. Retail MODE state remains unverified.

Root acceptance acknowledgment: 2026-10-24, E627 in HANDOFF/TASKS/EVIDENCE, with independent
review and replay recorded in `docs/checkpoints/2026-10-24-round30-integration.md`. Worker verified
the live HANDOFF/EVIDENCE record before closing; no further source change after acceptance.
