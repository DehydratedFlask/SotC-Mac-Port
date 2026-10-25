# Metal first-frame path: research, design, and bounded harness work

Status: E627 verifies the selected default shader pair against an independent oracle; E632/E633 verify
synthetic direct-Metal geometry and parser flow; E649 verifies the pinned raw cube/cone POS0/U16
members through the native parser and default-pair Metal harness; E634 characterizes the logo PS
WQM/sample path; E635 verifies synthetic texture binding; E656 verifies the pinned logo PS/PNG on
synthetic geometry; E658 combines the raw mesh, image, and logo PS under a synthetic UV adapter; and
E669 composes the separately pinned logo vertex and pixel shaders in one Metal pipeline. The
`GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01` fixture exercises that composed pair on those same raw meshes
and PNG with a synthetic position-to-clip/UV transform. The first attempt failed to find safe CPU
interior probes before pipeline execution; attempts two and three passed baseline and texture checks
but failed the one-sided position-probe search. A principal reassessment authorized one symmetric
coverage-loss probe after an independent review found a camera-scaled UV oracle defect and missing
mutated-frame shape validation (E675). The fourth focused CTest passed with Metal API/GPU validation
and direct private PNGs (E677); independent review approved the bounded fixture (E678); repository
configure/build, CTest 397/397, lift checkers 481/0/0, project index and Trail wiring checks passed
(E679). Precommit staged/all-path and attribution audits passed; E681 records publication to `main`.
Retail bindings remain
unknown, and no game frame is established. **G3 is PARTIAL, not PASSED**, under the synthetic-scene rule in
`docs/VALIDATION.md`.
Read [AGENTS.md](../AGENTS.md), [ROADMAP.md](../ROADMAP.md), [HANDOFF.md](../HANDOFF.md),
[PLATFORM.md](PLATFORM.md) and [VALIDATION.md](VALIDATION.md) (gate G3) first. This file adds no
runtime behaviour; its only code is the read-only probe `tools/dev/gpu_asset_probe.py`.

## 0. Labels, identity, method

- **OBS** = observed in a tool result (address, byte, digest, count) that can be re-run. **INF** =
  inference from OBS plus naming/structure. **UNK** = not established; do not promote it silently.
- Executable: `eboot.bin` sha256 `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`
  (65,314,052 B); reconstructed ELF `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Image base is 0, so every
  address below is an RVA equal to the Ghidra address. The first LOAD segment maps file offset
  `vaddr + 0x4000`; a raw-offset string scan must subtract `0x4000` before it is used as an address.
- Tools: Ghidra 12.1.4 headless on the persistent project `local/ghidra/persistent-20261020`, opened
  `-process eboot.elf -noanalysis -readOnly` (no import, no re-analysis, no writes). Ghidra knows
  **38,845** functions; the call graph used here has **direct call/jump edges only**. Indirect calls,
  vtables and thread/job entry tables are not resolved, so "not reached" below means "not reached by
  direct edges".
- Homebrew LLVM 23.1.3 (`llvm-mc`) cannot disassemble GFX6/GFX7 (`disassembly not yet supported for
  subtarget`), so no off-the-shelf PS4 GCN disassembler is available. Instruction counts below come from
  an encoding-bit classifier, not a disassembler.
- Private evidence (game-derived, not for publication): `local/re/evidence/metal-frame-path-r29/`
  (Ghidra scripts, call graph, decompiler dumps, census, `recipe.txt`, `SHA256SUMS`).
- Reference source (`reference/`, read only, never fetched) is cited as a map of GCN/PM4 semantics, not as
  code to copy: the shadPS4-derived recompiler is GPL-2.0-or-later (see section 9).

**Unfinished in this pass (marked UNK in the text):** Oodle identification is inference only; the
`CompressionHeader` location, input-usage-slot decoding, `.cmat` constants, texture formats/tiling and the
job-table edges were not resolved; the Gnm-band layer names are inferred from decompiler shape. The
logo VS/PS Metal composition and raw-CMSH composed-frame fixture are fixture-verified within their
synthetic ABI; the latter required a three-try escalation and a principal-authorized fourth attempt.
Other future cards remain proposals.

## 1. Headline findings

1. **The repo's PSARC path/entry pairing is wrong for every archive (section 2).** The TOC value is
   MD5 of the path; entries are sorted by it; the manifest is alphabetical. Per-path facts in
   `members.jsonl`, `audio-headers.json` and several E-claims are attached to the wrong files. This
   must be fixed before any asset conclusion is trusted.
2. The game's rendering is **BPE `Renderer/{Frontend,Backend}` over a statically linked Sony Gnm/Gnmx
   band** (`0x85c040..0x885f90`, 724 functions, 157,787 B) over **103 `libSceGnmDriver` imports**.
   PS4-specific code is concentrated in the `PS4CRenderContext` block `0x917c0..0x99720` (100
   functions, 30,902 B, 77 externally called) and `PS4CRenderBackend`/`PS4ComputeContext`.
3. The renderer is **compute-dominated**: 132 dispatch call sites (64 + 37 direct, 16 + 15 indirect
   variants on the MEC) against about 15 draw call sites. Virtual geometry, XPR static models, fur
   and particles are GPU-driven. A faithful frame needs compute translation; a *first* frame does not.
4. Draw/submit is driven from a **job-scheduled render thread** (`RenderThread::ThreadFunction`
   `0xbd6d0`, entered through a data table), so the static call graph cannot connect it to the
   startup slice.
5. The only direct renderer link from the already-native startup slice is **four constructor rows of
   `0x2569d0`** (rows 4, 5, 8, 9 of the 21-row driver in `owner_members.cpp`); all four ctor bodies
   are hooks today. Startup slot 298 (`shaderTypeToLoad`, `0xb0e250`) is the shader-variant selector.
6. Shader assets are decodable offline *without any decompressor* for about 44% of `.csdr` members: raw
   members carry `OrbShdr` GCN binaries that parse with a fully verified layout (14,967/14,971 raw
   members; section 4.3). The other ~56% (and ~95% of mesh/texture members) begin with a `0x8C`
   block header consistent with Oodle LZ and remain unsupported by the owner's defer decision (section 4.2).
7. A **fully raw asset chain exists**: `default.cmat` -> `standard_4030788106.csdr` (VS 272 B + PS 240 B,
   no texture sample) with `cube_1m.cmsh` / `cone.cmsh` (section 4.6). A textured variant uses the
   `cp10shadowlogo` pair (VS 140 B, PS 152 B with one `image_sample`) and a PNG-wrapped boot texture.
8. The smallest *honest* first frame is an **asset-derived harness frame** (section 6): real game shaders
   and geometry through a native typed-op layer into Metal, labelled as not game-driven. A frame produced
   by recovered game code is a later milestone that needs the engine.

## 2. F0: asset-index pairing defect (blocking correction)

OBS (all seven local archives, `python3 -I tools/dev/gpu_asset_probe.py pairing <archive>`):

| Archive | Members | MD5(path) matches a TOC entry | Positional pairing agrees | Entries sorted by TOC value | Manifest sorted |
|---|---:|---:|---:|---|---|
| cachedFiles | 40,213 | 40,213 | 1 | yes | yes |
| patch cachedfiles | 40,216 | 40,216 | 0 | yes | yes |
| pak0 | 6,918 | 6,918 | 0 | yes | yes |
| pak1 | 5,630 | 5,630 | 0 | yes | yes |
| pak2 | 1,313 | 1,313 | 0 | yes | yes |
| pak3 | 12,053 | 12,053 | 1 | yes | yes |
| pakN | 64,108 | 64,108 | 1 | yes | yes |

- `scripts/psarc.py` `paths()` returns the alphabetical manifest, and every consumer pairs manifest line
  `k` with `entries[k + 1]`. Because entries are in MD5 order, **that pairing is wrong for essentially every
  member**. The correct pairing is `entry = entries[index_of(md5(path_utf8))]` (a standard PSARC lookup;
  the engine's own loader hashes the path the same way, INF).
- Independent corroboration (OBS, pak0/pak1/pakN, correct pairing): every raw `.csdr` member contains an
  `OrbShdr` binary (positional pairing: roughly half contained none); raw `.cclm` members begin `LOCS`;
  raw `.cprt` members begin `00000000`; `cachedFiles` `.cmdl` members begin `fbad9897` (24,364/24,540).
  `default.cmat` names the shader file that actually exists and is a shader (section 4.6).
- Corollary: base and patch `cachedFiles` share all 40,213 paths with **equal TOC value (it is a function
  of the path) and equal declared size** under the correct pairing (OBS). The ASSETS.md statements that
  35,734 shared paths differ in "TOC digest" and 35,512 in declared size describe positional artifacts.
- Affected (must be re-labelled or regenerated; none of this lane's conclusions depend on them): the
  per-path columns of `local/manifests/assets/*.members.jsonl` (`size`, `offset`, `block`, `md5` are the
  neighbour entry's), the overlay report (E75/E77/E123/E126/E239 comparisons), `audio-headers.json` and
  the AUD-01 family table (the `xff`/`hd-bd` attribution), `audio_bank_header.py`, `docs/GPU01_PREP.md`
  private JSONL (extension and path counts are unaffected; sizes/md5s per path are not), and any native
  fixture that was seeded from a positional member. Extension counts, path lists and archive identity
  digests are unaffected.
- Proposed fix is card 1 (section 7). Do not run audio or asset claims that depend on a path until it lands.

## 3. The game's graphics stack (observation)

### 3.1 Layers

| Layer | Addresses | Evidence |
|---|---|---|
| L5 SotC game graph managers | `BP_Renderer` `0x5b51e0`, `effectmanager` `0x5e4330`, `fseffect` `0x5e5ef0`, `modelmanager` `0x5eb1d0`, `refraction` `0x5f0940`, `spritemanager` `0x5f1670`, `texturemanager` `0x5f33c0`, `vrammanager` `0x5f5140`, `polygonmesh` `0x6ba490`; flow: `title` `0x7860a0`, `titlemenu` `0x787b60`, `opflow` `0x7ea310`, `gameflow` `0x7dfb70` | OBS: assert-string xrefs (`d:\dev\cp10\Main\Code\Game\CP10\Shadow\sources\...`) |
| L4 BPE renderer frontend | `CRenderer` `0x113960..0x117930`, `RenderThread` `0xbd6d0/0xc12a0/0xc5560`, `LightRendering` `0xae310`, virtual geometry `0xdadc0..0xe1610`, particles `0xfbb60..0x10eaa0`, `CMesh` `0x1264d0`, `CMeshRenderEntity` `0x1249a0`, static lighting `0x129690..0x136b30`, `TextureManager` `0x13d280`, fur `0x1848d0..0x1acdc0`, XPR `0x19aad0/0x1ae900/0x1b2390` | OBS: assert-string xrefs (`...BPE\Source\Renderer\...`) |
| L3 BPE PS4 backend | `PS4CRenderBackend` `0x8ef20..0x906d0`, `PS4CRenderContext` `0x917c0..0x99720`, `RenderTargetManager` `0x9aef0`, `PS4ComputeContext` `0x16e990`, `CRenderGlobals` `0x169be0` | OBS: assert xrefs; band edges INF |
| L2 Gnmx-like `GfxContext` | `0x85c040..0x862e80`; draw-state flush `0x85fc10` (3,994 B), `0x85f410` (2,042 B) | INF: decompiler shows dirty-flag shader/descriptor-table flush, `DrawCommandBuffer` at `+0xc090`, constant-update engine at `+0xc098` |
| L1 Gnm `DrawCommandBuffer`/`ComputeCommandBuffer` wrappers | `0x869290..0x885320` (101 functions that call a Gnm import); each checks space, calls the reserve-failed callback at `+0x18`, then a driver thunk | OBS: decompiler dump of `0x86a9d0`, `0x86b9e0`, `0x86c050` |
| L0 `libSceGnmDriver`, `libSceVideoOut`, `libScePngEnc` imports | GOT `0x16007e0..0x1600b10` (Gnm), `0x16005c0..0x1600660` (VideoOut), `0x16005a0..0x16005b8` (PngEnc) | OBS: `local/re/imports.json` |

The whole band `0x85c040..0x885f90` is ordinary x86-64 code inside `eboot.bin`: it is **not** a runtime the
product may load. PM4 words are produced by these functions and by the driver thunks. Under PLATFORM section
1 the native port replaces this band by typed operations (section 6); it is evidence for state semantics,
not a candidate for lifting.

### 3.2 Driver imports (OBS)

- `libSceGnmDriver`: 103 imports. Draw: `DrawIndex`, `DrawIndexAuto`, `DrawIndexOffset`, `DrawIndirect`,
  `DrawIndexIndirect`, `...IndirectCountMulti` (x2), `DrawIndexMultiInstanced`, `DrawOpaqueAuto`.
  Dispatch: `DispatchDirect`, `DispatchIndirect`, `DispatchIndirectOnMec`. Shaders: `Set/Update` for
  VS/PS/HS/LS/ES/GS, `SetEmbeddedVs/PsShader`, `SetCsShaderWithModifier`. Submit: `SubmitCommandBuffers`,
  `SubmitAndFlipCommandBuffers` (+ForWorkload), `SubmitDone`, `DingDong`. Resource registration:
  `RegisterOwner/Resource`, `Unregister*`. No `sceGnmDrawIndexInline` import exists.
- `libSceVideoOut`: 21 imports (`Open`, `RegisterBuffers`, `SubmitFlip`, `SetFlipRate`, `AddFlipEvent`,
  `WaitVblank`, `GetResolutionStatus`, ...). `libScePngEnc`: 4 imports, one caller (`0x961a0`, E597).

### 3.3 Draw and dispatch inventory (OBS, direct call edges)

| Gnm sink (wrapper) | Direct callers | Where the callers live |
|---|---|---|
| `DrawIndexAuto` (`0x86ac10`) | 2: `0x93690`, `0x94ec0` | `PS4CRenderContext` band |
| `DrawIndexOffset` (`0x86b040`) | 9: `0x94d00`, `0x94dd0`, `0x133550`, `0x133750`, `0x1338d0`, `0x133a50`, `0x133bd0`, `0x133e20`, `0x1a8c60` | backend; static-light probe cluster; XPR |
| `DrawIndirect` (`0x86b150`) | 1: `0x10add0` | particles |
| `DrawIndexIndirect` (`0x86b3d0`) | 3: `0xdb620`, `0x1a6d20`, `0x1a7290` | virtual geometry; XPR |
| `DispatchDirect` (`0x86b9e0`; MEC `0x883870`) | 64; 37 | render targets, lighting, VG, particles, fur, XPR, probe cache |
| `DispatchIndirect` (`0x86bac0`; MEC `0x883950`) | 16; 15 | VG, particles, fur, XPR |

The inline-index and predicated wrappers (`0x86a9d0`, `0x86aab0`, ...) have no callers. The two
`PS4CRenderContext` draw methods most likely to be the generic "draw N vertices / draw indexed" calls are
`0x94ec0` (234 B, 28 external callers, sets primitive type from a table at `0xf9f2e0`, calls
`0x85fc10`, then `DrawIndexAuto`) and `0x94d00`/`0x94dd0` (indexed). INF.

### 3.4 State API surface of `PS4CRenderContext` (INF from OBS)

- The ctor `0x917c0` (499 B) allocates a `0xcbb8`-byte `GfxContext` (`0x85c040`), initialises it
  (`0x862690`), and stores it at `[ctx+0]`. It is called only by `0x90470`
  (`CRenderBackend::PerFrameRenderContexts::RequestNewRenderContext`), which also calls
  `DrawInitDefaultHardwareState350` (`0x869290`).
- The methods are **state-caching wrappers**: a dirty-bit mask at `[ctx+8]` plus cached fields (for
  example `0x91e10` blend/colour-control, `0x919e0` per-slot values 0..4, `0x91cd0` raster/guard-band
  floats) compare a small state struct and call a Gnm setter only on change. That is the natural typed-op
  surface: a state object in, an op out. Of the block's 100 functions, 77 have callers outside the block.

### 3.5 Submission and presentation (OBS)

- Frame submit: `RenderThread::KickoffSubmitJob` (`0xc5560`, 1,128 B) -> `0xd07c0` -> `0x91f90` ->
  `0x863aa0` -> `0x863af0` (1,237 B) -> `sceGnmSubmitAndFlipCommandBuffers` wrapper `0x882ad0`.
  Non-flip `0x863310` -> `0x882ab0` has no direct caller (indirect).
- Display: the 0x88-byte VideoOut object constructor `0x95a90` (987 B, install row 8) calls `sceVideoOutOpen`
  and friends; `0x95760` registers buffers, names a thread `"RenderFlip"`, and a flip event queue. Display
  buffers are named `"DisplayBuffer"` and are 1920x1080 (`0x780 x 0x438`) or 3840x2160 (`0xf00 x 0x870`)
  depending on a platform probe (`0xf8fc46`); pixel-format words `0x88000000` or `0x88740000` are passed to
  `sceVideoOutSetBufferAttribute`. Screenshot: `0x961a0` (E597).
- Render-target setup lives in `ViewportRenderTargets` (`0x9aef0`) and the buffer allocator `0x97a50`
  (2,834 B, 17 external callers, `"DisplayBuffer"` is one name). Render targets and depth map naturally to
  GPU-private `MTLTexture`s; tiling is irrelevant for them (PLATFORM section 5 concerns textures).

### 3.6 Threads and unresolved edges

- `RenderThread::ThreadFunction` `0xbd6d0` is referenced only as data (`0x15fe778`);
  `ResourceFactory::ThreadFunc` `0x1464d0` only as data (`0x15feae0`); `CMesh::MeshFactory` `0x1264d0`
  (4,988 B) and `0x863310` have no static callers. These are job/factory entry tables: **UNK edges**
  that block any "reachable from main" claim by direct analysis.
- The render job tree below `0xc5560`/`0xc59d0`/`0xcaed0` fans out through the job system
  (`JobSystem.cpp`/`JobSystemScheduler.cpp` anchors `0x33400..0x3a380`); its job-declaration table has not
  been walked.

## 4. Shader and asset path (observation)

### 4.1 Shader file naming and variant selection

- Resolver `0xb0cc50` (624 B) with helper `0xb0cb80` (201 B) builds the shader file name from the resource
  name plus a suffix chosen by the `shaderTypeToLoad` value read through `*[0x3305d80]`. Enum strings at
  `0x12cfc56` name the four values `Debug`, `Shipping`, `PS4Neo_Debug`, `PS4Neo_Shipping`; the suffix table
  at `0x15d5828` holds `_shipping.csdr`, `_neo.csdr`, `_neoshipping.csdr`, with `.csdr` for the base. The
  resolver tries the preferred name, then falls back to the base name (state in `*param_2`).
- The ConVar constructor `0xb0e250` (startup slot 298) clears the value cell `0x3305df0` to 0 and
  registers the command. Whether retail sets a non-zero value (command line, platform probe) is **UNK**.
- OBS: all 60 sampled pak0 `materials/` `.csdr` members have a `_shipping` twin in pakN and are
  **byte-identical** to it; the chosen default pair is identical (sha256 below). The variant risk for the
  recommended fixtures is therefore small, but not zero for other families.

### 4.2 Resource load and the compressed-block layer

- `ResourceFactory::CreateResource` (`0x147160`, 813 B) reads the member, then, when the type descriptor
  flag `entry+0x70 & 8` and the global `0x1aa210` allow, takes the uncompressed size from a
  `CompressionHeader` (`0x1aa260`), allocates, and calls `Compression::Decompress` `0x1aa2a0` (235 B); it
  then wraps the bytes in a stream object (`0x4d470`) and calls the type factory build (`0x144880`).
- OBS (`0x1aa2a0`): the header u32 is `method << 29 | size` (`size` = low 29 bits). A jump table at
  `0xfadd4c` maps method 0 to a `memcpy` after a warning ("Extracting a non-compressed buffer, use
  Compression::GetUncompressedData() instead!"), 1 to `0x1b9130`, 2/3/4 and 5 to `0x89d3b0` (5 with
  `r8d = 0`, 2-4 with `r8d = 1`), and methods 6/7 return the size without a call.
- INF (strong): `0x89d3b0` is called with six register arguments and eight stack pushes (the last pushed
  value is the constant 3), 14 arguments in total, with the decoder scratch pointer/size in two globals.
  That matches the arity and argument roles of Oodle's `OodleLZ_Decompress` (final `threadPhase` =
  `Unthreaded`). Oodle is therefore probably statically linked; this is not verified.
- OBS: members whose first two bytes are `8c 0a` or `8c 06` (and `cc xx` variants) carry **no** u32 of the
  `method << 29 | size` form at offset 0, and after the 2-byte prefix the next 8 bytes are literal payload
  bytes (as an Oodle block's initial copy would be). INF: this is the Oodle LZ block header (magic nibble
  `0xC`; `0x0a`/`0x06` decoder-type codes; `0xCC` = block stored uncompressed). **UNK**: where the
  `CompressionHeader` lives for such a member, and which engine method value corresponds to which prefix.
  Owner decision E665: defer implementing/importing a decoder; keep compressed members outside current
  raw-asset tasks.
- Census (OBS, correct pairing, first-bytes classification; "8C" = `8c 0a`/`8c 06`):

| Archive/ext | Members | 8C | `cc` | PNG | Other (raw or tagged) |
|---|---:|---:|---:|---:|---:|
| pak0 `.csdr` | 3,309 | 2,270 | 0 | 0 | 1,039 |
| pak1 `.csdr` | 2,512 | 1,442 | 0 | 0 | 1,070 |
| pakN `.csdr` | 28,470 | 15,608 | 0 | 0 | 12,862 |
| pak0/pak1/pakN `.cmsh` | 1,806 / 2,320 / 16,750 | 1,582 / 1,997 / 14,492 | 1 / 0 / 1 | 0 | 223 / 323 / 2,257 |
| pak0/pak1/pakN `.ctxr` | 1,378 / 515 / 5,135 | 1,369 / 515 / 4,731 | 3 / 0 / 322 | 0 / 0 / 13 | 6 / 0 / 69 |
| pakN `.ctxc` | 9,288 | 8,444 | 266 | 0 | 578 |
| pak0/pak1/pakN `.cptx` | 60 / 34 / 103 | all | 0 | 0 | 0 |
| cachedFiles `.csdr` / `.cmsh` | 101 / 1,136 | 0 / 1 | 0 | 0 | 101 / 1,135 |
| cachedFiles `.cmdl` | 24,540 | 176 | 0 | 0 | 24,364 begin `fbad9897` |

`.cmat` members do not use a magic: the first dword is hash-like and unique per file (INF).

### 4.3 The raw `.csdr` container (verified)

OBS (`tools/dev/gpu_asset_probe.py csdr <archive>`; pak0, pak1, pakN, cachedFiles, patch cachedfiles):

- Layout: `u32 blob_count`, a container header, then per shader a program that starts with the dword
  `0xBEEB03FF` (`s_mov_b32 vcc_hi, imm`) followed by `imm`, `BinaryInfo.length` bytes of GCN code ending in
  `s_endpgm` (`0xBF810000`), padding, then the 28-byte `OrbShdr` `BinaryInfo`.
- The rule `BinaryInfo offset == token offset + (imm + 1) * 8` and "exactly one token candidate per
  `BinaryInfo`" and "`code[length - 4] == s_endpgm`" and "blob count == header u32" hold for **14,967 of
  14,971** raw members in pak0/pak1/pakN (1,039 + 1,070 + 12,858 passed, plus 101 + 101 in the two
  `cachedFiles` archives). The 4 exceptions are two geometry-shader files and their `_shipping` twins whose
  header says 1 while holding 2 `OrbShdr` blobs (GS plus a second program). The `BinaryInfo` fields match
  the reference `AmdGpu::BinaryInfo` (`regs_shader.h:16`): `OrbShdr`, version 7 on every blob, a
  type nibble, `source_type` 1 on every blob, a 24-bit length, a hash and a CRC32.
- Type codes, by co-occurring file names (INF from OBS): **0 = PS, 1 = VS, 2 = GS, 4 = CS, 7 = HS, 8 = DS**
  (`ps_main_*`, `vs_main_*`, `cs_*`, `gs_main_*`, `hs_main_*`, `ds_main_*` in `cachedFiles`). Material
  passes are `(VS, PS)` pairs: pak0 748, pak1 532, pakN 5,614. Particle emitters and engine compute shaders
  are single CS blobs: pak0 291, pak1 538, pakN 7,065. pakN also has PS-only 118, VS-only 53, GS 6.
- A usable fixed field: the BinaryInfo `shader_hash` also appears inside the container header (for example
  at `+0xf4` of a 1,248-byte compute shader). The input-usage table
  (`chunk_usage_base_offset_in_dw`, `num_input_usage_slots`) is **not decoded**; the reference recompiler
  does not parse it either (it derives bindings from live PM4 user data), so the native importer must define
  its own resource ABI (card 3).

### 4.4 Other formats (OBS unless marked)

- `.cmsh` raw simple layout (`00 02 00 00 00 01 00 00 ...`): bounding box floats, a material name
  (`default`), a descriptor table of 24-byte records tagged `0SOP`/`0MRN`/`0NAT`/`0XET` (fourcc is the
  reversed ASCII `POS0`/`NRM0`/`TAN0`/`TEX0`), an 8-byte `0xFF` sentinel, then the streams, then a
  `u32 marker, u32 count, u16 indices[count]` tail and a `HSEM` ("MESH") chunk. Verified by the probe on
  `cube_1m.cmsh` (24 vertices, positions all `+-0.5`, 36 indices, max index 23) and `cone.cmsh` (42
  vertices, radii 0 and 1, 114 indices). Normal, tangent and UV streams are 4 bytes per vertex; their
  encodings are **UNK**. Other `.cmsh` variants (`00050000`, tagged, 8C) are not decoded.
- `.cmat` (e.g. `default.cmat`, 171 B): a header, the literal path
  `$/materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr`, a few constants
  (two `1.0f` values) and a trailing `STAM` ("MATS") tag. The constant layout is **UNK**. `logo.cmat`
  (`materials/shadow/ui/_ps4/`, 172 B) names `.../cp10shadowlogo/b22796d2/_ps4/standard_3728175326.csdr`.
- `.cmdl` in `cachedFiles` (24,364 of 24,540) begin `fb ad 98 97`: a property-bag serialisation naming
  meshes/materials by `$/.../****/...cmat|cmsh` paths (the `****` placeholder is the platform tag of E582).
- `.ctxr`/`.ctxc`/`.cptx`: nearly all 8C-prefixed, so their payload layout and PS4 tiling are **UNK**. Two
  exceptions are usable without a decoder: **13 pakN `.ctxr` members are plain PNG files**
  (`misc/boot/_ps4/saveshadow_*.ctxr`, 23,089..110,875 B; `saveshadow_scee.ctxr` is 24,636 B, 228x128, 8-bit
  RGB, sha256 `b4b13523...00f5`), and a few raw tagged `.ctxr` start `0b000000`/`00000000`.

### 4.5 Where the shaders sit in the corpus

`.csdr` total 34,392 unique paths (34,291 in pak0/pak1/pakN + 101 in `cachedFiles`); about 56% of the pak
members are 8C-prefixed. Raw ones cover `materials/!cookedshaders/version57/...` (1,280 in pak0/pak1) and
`particles/shadow/...` (829), plus all of `coredata/enginesupport/shaders/`. The `.cslt` shader libraries
(`engine-shaders`, `particle-shaders`, `lightprobe-shaders`, 3 members in pakN, 9,163 B total) are not decoded.

### 4.6 The all-raw candidate chain (ARTIFACT_VERIFIED digests)

| Role | Archive path | Bytes | sha256 |
|---|---|---:|---|
| Material | cachedFiles `coredata/enginesupport/materials/_ps4/default.cmat` | 171 | `0c2233c1aab20cfc1da914de96cfd1cbd0a20e37964024d54e10e36b260c6d29` |
| Shader pair | pak0 `materials/!cookedshaders/version57/simple/0f1c3bf0/_ps4/standard_4030788106.csdr` (identical `_shipping` twin in pakN) | 1,988 | `c4214e4ee0852a962ce56e0cab0befbe3169698cb354bc62afa7eab4d1f3f356` |
| Mesh | cachedFiles `coredata/enginesupport/models/_cmn/cube_1m.cmsh` | 850 | `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7` |
| Mesh | pak0 `coredata/enginesupport/models/_cmn/cone.cmsh` | 1,462 | `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c` |
| Textured shader pair | pak0 `materials/!cookedshaders/version57/cp10shadowlogo/b22796d2/_ps4/standard_3728175326.csdr` | 1,916 | `21cde037542277bbac24fdb6d1e211f40dbb2187534ce9c31c7c59fad6b68d64` |
| PNG texture | pakN `misc/boot/_ps4/saveshadow_scee.ctxr` | 24,636 | `b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5` |

Program blobs (code offset in the `.csdr` file, length, `BinaryInfo` offset, sha256 prefix of the code bytes):

| Pair | Stage | Code @ | Length | Info @ | Code sha256 (first 16 hex) |
|---|---|---:|---:|---:|---|
| default `standard_4030788106` | VS | 564 | 272 | 972 | `5a4f11f29a8d46a3` |
| default | PS | 1368 | 240 | 1744 | `4af7cb6973d4743a` |
| logo `standard_3728175326` | VS | 564 | 140 | 844 | `430f9adbb07d387c` |
| logo | PS | 1240 | 152 | 1672 | `4309caada7f114ec` |

`GPU-02-RAW-MATERIAL-SELECTOR-01` (claim E682, publication E683) has ARTIFACT_VERIFIED exact raw
member identity, selector literal occurrence and CSDR spans (E684). The standalone verifier/tests and
task-specific replay of the existing MSL/oracle baseline remain NOT RUN. The `.cmat` constants/layout
and engine consumption remain UNKNOWN; this task does not decode `0x8C`, mesh/texture streams, or
claim a game-driven frame.

Encoding-class inventory (approximate; a classifier by encoding bits, with literal-dword handling, not a
disassembler): default VS `SMRD 8, MUBUF 2, VOP2 17, VOP1 8, VOP3 7, EXP 3, SOPP 7, SOP1 2`; default PS
`SMRD 8, VINTRP 12, VOP2 22, VOP1 1, VOP3 2, EXP 1, SOPP 6, SOP1 3` (no `MIMG`, no `DS`); logo VS
`SMRD 4, MUBUF 3, VOP2 2, VOP1 2, VOP3 2, EXP 3`; logo PS `SMRD 3, VINTRP 12, MIMG 1, VOP2 6, SOP1 6, EXP 1`.
None of the four uses `DS`, LDS or `v_readlane`-class operations in this classification, so a per-lane
translation is plausible (a cross-lane use would have to be excluded instruction by instruction, card 3).

### 4.7 What is still not decoded (UNK)

E634 now characterizes the logo PS's explicit EXEC save, whole-quad expansion, texture-sample interval,
mask restore, and descriptor-register dataflow. The live image/sampler values and their mapping to
Metal resources remain UNK; no logo MSL or textured render has been generated.

- The `0x8C` block layer (section 4.2) and everything behind it: most meshes, all non-PNG textures,
  most shaders, `.ctxc` streaming chunks, `.cani`/`.cclm`/`.cgpr` bodies.
- The PS4 texture tiling mode per `.ctxr`, mip layout, format codes (PLATFORM section 5).
- `BinaryInfo` input-usage slots and how the game binds V#/T#/S#/constant buffers (user-data layout per shader).
- Normal/tangent/UV stream encodings, skinning streams, and the `.cmat` constant layout.
- How a pass chooses among `depth`/`shadow`/`gbuffer*`/`standard` shaders (the pass name is the file stem
  prefix; the numeric suffix is unexplained).

## 5. Reachability from the native startup slice

- OBS (direct edges, Ghidra graph): from `0xf120` (entry) 632 functions; from `0x536730` 619; from
  `0x531d60` (Init) 903; from `0x25a8d0` 217; from `0x255130` 284; from `0x2569d0` 1,137; from `0x533100`
  1,147. **None of the renderer sinks is reached from `0xf120`, `0x536730`, `0x531d60`, `0x25a8d0` or
  `0x255130`** (draw/dispatch/submit wrappers, `RenderThread`, `ResourceFactory::ThreadFunc`, `CMesh`,
  `Decompress`, the shader resolver). `0x2569d0` is the only root that reaches renderer code directly.
- OBS: install rows of `0x2569d0` (`native/core/owner_members.cpp`, E570/E579 driver, all ctor bodies are
  hooks) whose ctor is called only from `0x2569d0`:

| Row | Ctor | Object size / global | Role (INF) |
|---|---|---|---|
| 1 | `0x144ca0` | `0x350` -> `0x2ee2d00` | resource factory-side object (the `vslot_40_533100_gfx_2ee2d00` name is a misnomer: that cell carries the resource platform tag `+0x40` of E589) |
| 3 | `0x14db00` | `0x138` -> `0x2ee2cf8` | `ResourcePool` (E593/E594, native model exists) |
| 4 | `0x08f6f0` (2,350 B) | `0xAA0`, self-registered cell `0x2eec4d8` | PS4 render backend (band of `PS4CRenderBackend.cpp`) |
| 5 | `0x13a9a0` (4,245 B) | `0x206050` -> `0x2ee2688` | texture/surface pool (near `TextureManager.cpp`) |
| 8 | `0x095a90` (987 B) | `0x88`, self-registered cell `0x2eec4e0` | VideoOut/display object (calls `sceVideoOut*`) |
| 9 | `0x112db0` (2,140 B); dtor `0x113690` (710 B) | `0x2203F0` -> `0x2edf140` | `CRenderer` (global passed to rows 17/18) |

  Closure sizes by direct edges (upper bound, they share allocator/string helpers): VideoOut ctor 414
  functions / 113,202 B; backend ctor 458 / 129,913 B; `CRenderer` ctor 497 / 160,883 B.
- OBS: startup slot 298 (`shaderTypeToLoad`, `0xb0e250`) is the ConVar behind the variant selector of 4.1.
  The native port is currently extending that area (concmd slots 298/299/301 are in the working tree).
- Conclusion: nothing renderer-side is executed by the native slice today. Rows 4/5/8/9 are the first
  places a native renderer object would be constructed once the engine exists; they are not needed for an
  asset-derived harness frame.

## 6. Design: the smallest honest path

### 6.1 Honesty ladder (what each level may claim)

| Level | Content | May claim | May not claim |
|---|---|---|---|
| L0 (today) | synthetic MSL pattern, E610 | provider/readback/PNG mechanics | any game content |
| L1 asset-derived harness | real `.csdr` programs, real `.cmsh`/PNG data, native typed ops, Metal | "a Metal frame whose shaders and geometry are game assets, produced by a documented import path"; evidence for G3 items "CPU upload -> shader read" and "render-target write -> readback" | that the game drew it, boot, scene, FPS, G3 PASS |
| L2 recovered-code-driven | a recovered `CRenderContext`/`CRenderer` path emits the ops | draw submission semantics of one recovered pass | gameplay, boot, audio |
| L3 in-engine frame | resource system + render thread + scene produce the frame | G5 evidence candidates | anything without cold boot and forbidden-runtime absence |

The recommended first frame is **L1**. Its log, file name and `HANDOFF` entry must say "asset-derived
harness" and the VALIDATION G3 row stays PARTIAL at best.

### 6.2 The seam

- Do not lift L0-L2. Replace them at the `PS4CRenderContext` surface (3.4): a native `RenderContext`
  exposing typed operations equal in granularity to the observed state structs and draw methods
  (`BeginPass`, `SetViewport/Scissor`, `SetDepth/Blend/Raster/StencilState`, `SetPipeline(vs, ps)`,
  `BindVertexStream`, `BindConstants`, `BindTexture/Sampler`, `DrawIndexAuto`, `DrawIndexed`, `Dispatch`,
  `EndPass`, `Present`). No PM4 words and no Gnm command buffer exist in the native runtime.
- The harness builds an op list from decoded assets; recovered code will later build the same list. Keep
  host resource handles distinct from original addresses (PLATFORM section 2).
- API baseline for the first slice: keep the classic `MTLCommandQueue` path already used by
  `metal_capture.mm` (automatic hazard tracking, `newLibraryWithSource`). The Metal 3/4 decision of PLATFORM
  section 5 stays OPEN and must be recorded before argument-buffer or indirect-draw work.

### 6.3 Where conversion runs

- GCN to MSL is **import-time** (asset import into Application Support), never in the shipped runtime and
  never a GCN interpreter. Generated MSL, metallibs and decoded geometry are derivative of game assets: keep
  them under `local/` or the user's cache, never in the repository. Cache keys per PLATFORM section 6
  (input digest, translator version, options, resource ABI, Metal language version).
- Unknown opcodes, unsupported encodings and unparsed usage slots fail conversion visibly (no zeroed
  output). Wave64: the four candidate programs show no cross-lane classes; each must still be checked
  instruction by instruction before the per-lane mapping is accepted.
- A test-only oracle (a second, independent evaluator in `tools/`, never linked into the runtime) is
  acceptable as the "independent check" of VALIDATION; it must not share code with the emitter.

### 6.4 Constraints and non-goals

- No dependence on `eboot.bin` at run time. The addresses in this file are evidence for state semantics,
  not callable targets.
- Do not copy GPL-2.0-or-later shadPS4 recompiler code into the tree without an owner decision on
  licensing; the AMD GCN ISA manual and the observed bytes are the clean-room basis (section 9).
- Compute-heavy passes (virtual geometry, XPR, fur, particles, probe cache) are out of scope for L1.
- The harness must not pretend to be the engine: no fake `frame_num`, no invented scene, no synthetic
  fallback when decoding fails (fail visibly; the existing provider's synthetic pattern must not be the
  fallback).

### 6.5 Synthetic texture/sampler binding slice

E635 implements the first fragment resource binding slice in the typed RenderOp executor. A caller
declares the MSL ABI in `MetalAssetFrameConfig` and supplies an RGBA8Unorm 2D base level plus optional
explicitly supplied mip levels and an explicit sampler for each draw. This fixture binds fragment
texture and sampler slot 0 with host-assigned handles; its sampler exposes nearest/linear
minification and magnification, optional nearest/linear mip filtering, and clamp, repeat, and
mirror-repeat U/V addressing. Invalid or missing descriptors fail before issuing the draw. Defaults
preserve E635's single-level path.

The E635 self-contained synthetic MSL fixture proves resource upload/binding, filtering, deterministic
readback, and fail-visible input checks. E645 adds explicit mip upload and verifies a synthetic
implicit/explicit gradient sample selects distinct levels against a CPU oracle. E646 specializes
the exact digest-pinned logo PS for a synthetic slot-0 texture/sampler and coordinate ABI, then checks
the generated fragment path against mip colors, per-channel multiplication, fp16 RTZ values, and a
CPU readback oracle. The profile emits a fixed template for that one pinned shader; it is not a
general per-instruction GCN translator. These fixtures do not prove AMD WQM/Metal helper-lane equivalence,
establishes retail texture/attribute bindings, or reads a game texture. G3 remains PARTIAL.

### 6.6 Logo vertex formatted-fetch lowering

`GPU-02-GCN-LOGO-VS-FETCH-01` extends the offline per-lane lowerer to the pinned logo vertex
program's `buffer_load_format_xy` and `buffer_load_format_xyzw` fetches. MUBUF descriptor identity,
per-vertex indexing, fetch width, and destination-register writes must match an explicit harness ABI;
format mismatches and unbound streams fail conversion. Tests exercise synthetic float2/float3/float4
stream widths, then replay the pinned private logo-VS span with synthetic stream fields, an independent
CPU oracle, and the Metal harness. The ABI is a test input and does not recover retail descriptor or
attribute semantics. E636 verified 1,024 Metal cases and 12,288 exact output components against a
separate CPU oracle. This vertex-fetch slice does not establish retail descriptors or attribute
semantics, and it does not provide real mesh sampling. The bounded E646 pixel fixture is documented
below. G3 remains PARTIAL.

### 6.7 Pinned logo pixel image sample

`GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` accepts only the private 152-byte E634 shader slice with its
exact SHA-256 and a closed-world synthetic slot-0 ABI. The decoder checks the pinned instruction
count, WQM save/enable/restore anchors, the single base `IMAGE_SAMPLE` fields and descriptor ranges,
compressed target-0 export, end instruction, and absence of branches or intervening EXEC writes.
The digest-gated profile emits a fixed PS-only MSL specialization; it validates and executes the
actual private byte slice but does not claim a reusable instruction-to-MSL lowering for arbitrary
programs or prove the retail descriptor/interpolation meaning.

The Metal fixture selects red mip 1 or green mip 2 from implicit UV derivatives, verifies center and
primitive-boundary samples, observes a selected-level mutation to blue, and checks channel
multiplication plus the half-RTZ export using binary-fraction inputs. `SOTC_LOGO_PS_MSL` enables the
private generated-MSL replay; `SOTC_LOGO_PS_PNG` writes the direct 64x64 harness image under `local/`.
E646 passed the focused test and full CTest with Metal API/GPU validation enabled and no faults.
This is fixture evidence for the exact shader and PNG under synthetic geometry and resource bindings.
WQM/helper-lane equivalence, retail bindings, game-driven rendering, and G3 remain unverified.

### 6.8 Raw mesh plus logo PNG composite fixture

`GPU-03-LOGO-PNG-CMSH-ASSET-FRAME-01` combines the exact-hash local `cube_1m.cmsh` and `cone.cmsh`
POS0/U16 subsets already parsed and drawn in E649 with the E656 digest-pinned logo PS MSL and
`saveshadow_scee.ctxr` PNG. The test-side harness normalizes positions into clip space and derives
synthetic UVs from transformed XY through the pinned adapter's `position.xy * material.xy + 0.5`
mapping. It binds one explicit RGBA8 texture/sampler at slot 0 and compares selected covered texels
with the independent CPU sampler/fp16-RTZ oracle, including deterministic repeat and a visible
texture mutation. Direct PNG evidence stays under `local/`.

E658 passes this combined fixture with Metal API and GPU validation enabled. It parses the pinned
850-byte cube (24 positions/36 indices) and 1,462-byte cone (42/114) raw members. The CPU coverage
search selected interior readback pixels `(20,30)` and `(43,27)` from actual mesh indices; their
synthetic UVs selected source texels `(93,65)` and `(134,68)`. Both matched the independent oracle,
the repeat frame was byte-identical, and the source-texel mutation changed the readback and PNG.
Private 64x64 PNG hashes are `5aa812cfd4db7056c7cb7c9193a7edc445e5e1ede23fa110b83580f667959154`
and `a6e72b7044326b63619e7e64d4ef9ebc70e3421d61a14643e1d6c313bbb7c57c`. The focused CTest passed
1/1, full CTest 397/397, and lift checks 481/0/0. An output-only partial texture configuration
fails visibly before any optional asset read or PNG write.

This is an asset-derived harness combination, not recovered material mapping: TEX0 and NRM0 remain
opaque; normals and UV projection are harness inputs. It does not establish retail texture or vertex
descriptors, orientation, WQM/helper-lane equivalence, a game-driven frame, or G3 completion.

### 6.9 TEX0 descriptor format-6 evidence task

The hash-bound local cube and cone members contain four 24-byte descriptors at file offsets
`0x4e..0xae`. Their `TEX0` records begin at `0x96`, carry observed `format_u16 = 6` and
`flags_u16 = 2`, and describe stream sizes 96 B and 168 B. Using the parser's observed base/offset
arithmetic, their byte ranges are `[0x296,0x2f6)` (cube) and `[0x416,0x4be)` (cone), with SHA-256
`c97732092653f3fd2e76d7bf72a99f3e00ce3fcef3afdb2d363e185ae2b91065` and
`fd126498e069530ca39144521d4dbae76d85a1a0044aeafc15eb5ad8ca75b7b5`. These sizes happen to be
four bytes per parsed POS0 vertex (24 and 42 vertices respectively), but that ratio does not
establish the component type, order, normalization, coordinate orientation, or renderer binding.
The parser and private survey leave TEX0 payload bytes opaque; see E660 and the [format-6 evidence
checkpoint](checkpoints/2026-10-25-gpu03-cmsh-tex0-format-6.md).

`GPU-03-CMSH-TEX0-FORMAT-6-EVIDENCE-01` is closed **UNKNOWN**. Read-only Ghidra evidence on the
hash-bound plaintext-segment ELF (`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`,
derived from SELF `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`) traced six
`.cmsh` asset-path references into `FUN_00143f50` (`0x143f50`, 832 B). That function registers paths
through repeated helper calls; it does not decode the TEX0 stream. The pinned PC reference checkout
contains no `.cmsh`/TEX0 implementation. The Ghidra profile, Evidence IDs, body/reference coverage,
import warnings and missing-SELF-segment limitation are recorded in E662 and the [TEX0 evidence
checkpoint](checkpoints/2026-10-25-gpu03-cmsh-tex0-format-6.md).

No byte type, component order, normalization, orientation, or renderer binding is established.
Keep TEX0 opaque and keep the E658 synthetic XY-to-UV mapping unchanged. Do not feed candidate
decodings into the verified fixture based on the observed four-byte-per-position size ratio.

### 6.10 Composed logo vertex/pixel Metal fixture

`GPU-03-LOGO-VS-PS-METAL-PIPELINE-01` composes the pinned E636 logo VS (140 B,
SHA-256 `430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c`) with the pinned E646
logo PS (152 B, SHA-256 `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`) through a
closed-world `logo-pair` profile. The generated MSL must use the E636 vertex stage and E646 fragment
stage together; the E646 vertex adapter is not substituted for the pinned vertex shader.

The fixture ABI is explicitly synthetic: `logo_clip` reads the per-vertex position, `logo_param0`
reads a draw-level unit object row, and `logo_param1` aliases the per-vertex position so the PS gets
varying UVs correlated with clip-space construction. A four-vertex indexed quad uses position corners
in `[0,1]²`, camera XY `(1,1)`, and object row `(1,1,1,1)`. A 64x64 asymmetric RGBA8 texture and
nearest/clamp slot-0 sampler feed a CPU-selected interior-pixel oracle. Repeat output must be
deterministic; a position or texel mutation must change the selected readback; wrong shader hashes,
unsupported ABI fields, and missing resource bindings must fail closed. Private MSL, logs, and PNGs
stay under `local/`.

This is a synthetic stage-composition check. It does not infer retail stream aliasing, TEX0/NRM0
semantics, WQM/helper-lane equivalence, game-driven rendering, or G3 completion. E664 is the task
claim. E667 records the focused implementation result: 23/23 lowerer tests and the opt-in direct-Metal
quad passed with Metal API/GPU validation enabled. The closed profile also canonicalizes and confines
all five ABI/input/output paths to `local/` and rejects path collisions. E669 records the full
repository gates and closes this bounded task as **FIXTURE_VERIFIED**; E670 records publication on
`main`. No game-frame or G3 claim.

## 7. Ranked task cards

| # | ID | Scope | Exit check | Depends on |
|---|---|---|---|---|
| 1 | `DATA-02-PSARC-PAIRING-FIX-01` | Replace positional manifest/entry pairing in `scripts/psarc.py` consumers (`members.jsonl` writer, overlay, `classify_audio_headers.py`, `audio_bank_header.py`) with `MD5(path)` lookup, reject missing/duplicate TOC values, extend `scripts/test_psarc.py` with a synthetic archive whose manifest is alphabetical and entries are MD5-sorted, regenerate `local/manifests/assets/*`, and re-label every dependent E-claim. | `gpu_asset_probe.py pairing` shows `md5_hits == members` and `entries_sorted_by_md5` for all 7 archives; raw `.csdr` members have 0 failures except the 4 documented GS cases; regenerated manifests agree with the probe on size/offset for 100 random members; the list of withdrawn/relabelled claims is in HANDOFF/EVIDENCE. | none (hours; precondition for 2-5) |
| 2 | `GPU-01-CSDR-READER-01` | Native importer-side C++ reader for the raw `.csdr` container (4.3): count, token, code span, `BinaryInfo`, type codes, hash/CRC, explicit GS count rule, fail-visible handling of 8C members; emit a private manifest row per shader. | 14,967/14,971 raw members parse; the 4 GS members take the documented path or fail with a named error; bit-flip/length/count/token mutations fail; ASan/UBSan clean; manifest digest recorded; ELF bytes never read at run time. | 1 |
| 3 | `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01` | Instruction-level decode of the four programs in 4.6 with an own table-driven GFX7 decoder (public ISA as basis), define the resource ABI (user-data SGPR table, `buffer_load_format` vertex fetch, `v_interp`, `exp` targets, `image_sample`), emit MSL at import time, unsupported opcodes fail conversion, add a separate offline oracle. | zero unknown opcodes and exact instruction boundaries in all four programs; generated MSL compiles; Metal output equals the oracle (exact for integer/move paths, documented tolerance for float) on at least 1,000 vectors including edge values; opcode coverage table recorded; assertion that no cross-lane instruction is present. | 2 |
| 4 | `GPU-03-RENDEROPS-ASSET-FRAME-01` | Typed op list (6.2) and a Metal backend extending `metal_capture`; parse the identified local raw `cube_1m` and `cone` `.cmsh` bytes into vertex/index buffers; use per-vertex harness normal `[0.5, 0.5, 1.0]`; draw both meshes offscreen and write a private PNG directly (the owner selected this instead of `attach_screenshot`; do not invent an engine frame number); optional logo pair + PNG texture as the textured follow-up. | deterministic PNG hash recorded; independent assertions (covered-pixel count and silhouette from a CPU projection of the decoded positions, depth order of two meshes, no pixel outside the projected bounds); `MTL_DEBUG_LAYER=1` and Metal shader validation report zero faults; mutations of a vertex, an index or a shader constant change the hash; artifact and log carry the label "asset-derived harness". G3 stays PARTIAL at best. | 2, 3 |
| 4a | `GPU-03-LOGO-VS-PS-METAL-PIPELINE-01` | Hash-gated E636/E646 logo VS/PS pair and synthetic position-to-UV alias in a closed ABI; render an indexed quad with an asymmetric slot-0 RGBA8 texture through the existing direct-Metal frame API. | Generated MSL uses the pinned vertex and fragment stages together; Metal API/GPU validation passes; selected samples match an independent nearest/clamp and fp16-RTZ CPU oracle; repeat is deterministic; position and texel mutations are visible; unsupported hashes/ABI fail closed. No retail binding or game-frame claim. | E636, E646, E635 |
| 5 | `ASSET-FMT-02-BLOCK-DECODER-DECISION-01` | Owner decision on the provenance of the `0x8C` block decoder before choosing implementation. | Decision recorded in E665: defer the decoder and restrict active milestones to already supported raw members; no decompressor implementation or import path is in scope. | 1 (compressed textures and most meshes/shaders remain unsupported; not raw tasks 2-4) |

Not ranked (later, needs the engine): lift install rows 8/4/5/9 (`0x95a90`, `0x8f6f0`, `0x13a9a0`,
`0x112db0`) with a Metal presenter and native backend hooks (CPU-04 territory), then port one real
`PS4CRenderContext` method sequence (L2).

## 8. Recommended next slice and exact first action

The default pair, logo vertex fetch, and logo pixel sampling are already closed by E627, E636, and E646.
The current next slice is `GPU-03-LOGO-VS-PS-METAL-PIPELINE-01`: compose the exact pinned stages through
the closed synthetic ABI in section 6.10. Its claim is recorded in E664.

Exact next action: extract the 140-byte and 152-byte program spans from their documented local `.csdr`
inputs, verify the E636/E646 SHA-256 pins, then implement the bounded `logo-pair` profile without
changing the existing `logo-ps` or default-pair outputs. If the exact pins or synthetic ABI anchors fail,
stop and record the observed mismatch before adding a new lowering case.

## 9. Unresolved items and risks

- **F0 (section 2) invalidates path-keyed asset facts** until card 1 lands. Highest-impact item.
- **Oodle provenance (INF).** The block decoder is probably proprietary RAD code linked into the game.
  The owner chose to defer it (E665); continue only with supported raw members (about 44% of `.csdr`,
  13 PNG textures, and a minority of meshes). A future decoder/import decision needs a new task.
- **No GFX7 disassembler on this machine** (LLVM 23.1.3 rejects the subtarget). The decoder must be written
  and validated against the ISA manual and the reference tables; there is no vendor oracle.
- **GPL.** The reference recompiler is GPL-2.0-or-later. Adapting it needs the attribution and licensing
  decision AGENTS.md requires; a clean-room decoder avoids the question.
- **Variant risk.** The retail value of `shaderTypeToLoad` is UNK; `_shipping` twins matched for 60/60
  sampled files and for the chosen pair, but `_neo*` variants were not compared.
- **Binding model.** How PSSL user data maps to V#/T#/S# per shader is not decoded; the first pair avoids
  textures (default) or has one `image_sample` (logo). A wrong ABI shows as plausible-looking wrong pixels, so
  card 4's CPU-side assertions are mandatory.
- **Compute dominance.** About 9 in 10 GPU call sites are dispatches (132 vs about 15 draws). A frame that looks like the game needs
  compute with LDS/wave64 semantics (GPU-02 proper); L1 deliberately avoids it and must not be presented as
  progress on that front.
- **Static call graph incompleteness.** 38,845 functions, direct edges only; thread/job entries and vtable
  calls are unresolved, so "unreachable from the slice" is an observation about direct edges.
- **Texture tiling and formats** (PS4 tiled layouts, BC formats, mips) are untouched; PLATFORM section 5
  requires asymmetric-pattern conversion tests before any real texture counts.
- **Shadow of the Colossus remake specifics** (water, WQM, readback timing) in `reference/` are noted for
  later; they are not used here.

## 10. Evidence and reproduction

- Probe (read-only, prints counts/digests only): `python3 -I tools/dev/gpu_asset_probe.py --self-test`;
  `... pairing <psarc>`; `... csdr <psarc> [--path P]`; `... cmsh <psarc> --path P`. Observed results:
  pak0 `csdr` 3,309 members (2,270 8C, 1,039 raw, 0 raw failures; 748 VS+PS, 291 CS); pak1 2,512 (1,442
  8C, 1,070 raw, 0 failures); pakN 28,470 (15,608 8C, 12,862 raw, 4 documented GS failures).
- Ghidra: scripts and outputs in `local/re/evidence/metal-frame-path-r29/` (`recipe.txt`, `SHA256SUMS`).
  Key outputs: `out/gnm_xrefs.txt` (import -> thunk -> callers), `out/region.txt` (Gnm band), `out/cg.txt`
  (direct call graph, 38,845 functions), `out/filerefs.txt` and `out/funcrefs.txt` (assert-string to
  function maps), `out/decomp_*.txt` (decompiler dumps of the functions named above).
- Function byte digests (sha256, first 12 hex, sizes from Ghidra): `0x95a90` 987 B `587ec0febfd6`;
  `0x8f6f0` 2,350 B `d19f46c65e6f`; `0x112db0` 2,140 B `c2ec6212730b`; `0x13a9a0` 4,245 B
  `1df9f8f2217c`; `0x113690` 710 B `e317680fa8b0`; `0x917c0` 499 B `e51eb7aa01f6`; `0x90470` 509 B
  `8fe385db4a26`; `0x94ec0` 234 B `ca06331cca06`; `0x94d00` 204 B `a155f6b29352`; `0x86ac10` 111 B
  `2f57b4f29c75`; `0x86b9e0` 125 B `71fe2dd11ad5`; `0x863af0` 1,237 B `0dd4d6d416a7`; `0xbd6d0` 686 B
  `ccce38586f47`; `0xc5560` 1,128 B `1e08125f8d81`; `0xb0cc50` 624 B `8f7682a8da64`; `0xb0e250` 331 B
  `39d29b11d646`; `0x1aa2a0` 235 B `f8a7e6d1f086`; `0x147160` 813 B `d142f19bc595`; `0x1264d0` 4,988 B
  `a47cba7da7bc`.
- Pairing check, stdlib only: for each manifest path `p`, `hashlib.md5(p.encode()).hexdigest()` equals
  exactly one entry's TOC value; compare its index with `k + 1`.
- The design and fixture evidence here do not establish a game frame. Current G3 is PARTIAL, not
  PASSED; G1 remains PARTIAL as recorded in HANDOFF.
