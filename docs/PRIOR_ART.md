# Prior art: Apple-Silicon ports of PS4 games (assessed 2026-11-01)

This note records what three public Mac ports actually do, which of their tools and designs fit
this repository's non-negotiables, and what we will therefore build. Sources were read from raw
files at pinned commits; **we did not build or run any of the three**, so their performance and
verification numbers are *their* claims. Anything not confirmed by reading the published tree is
marked **UNVERIFIED**. The independent deep-dive with file-by-file detail is in
`local/validation/prior-art/REPORT.md` (private).

| Project | Pinned commit | Branch | Licence | Language | Stars |
|---|---|---|---|---|---|
| [bmy/bbport-mac](https://github.com/bmy/bbport-mac) | `a69f56a` | `macos-native-0.6` | GPL-2.0-or-later | C++ | 50 |
| [Wooloomooloo2/bloodborne_mac_port](https://github.com/Wooloomooloo2/bloodborne_mac_port) | `50274ee` | `master` | GPL-2.0-or-later | C++ / ObjC++ | 251 |
| [truwa/nathan-drake-collection-macos](https://github.com/truwa/nathan-drake-collection-macos) | `550cca4` | `main` | **none** (all rights reserved) | Swift | 32 |

## 1. What each project does

### bmy/bbport-mac

A macOS fork of `deadinside28/bloodborne_pc`.

- **Execution model:** the game's own x86-64 `eboot` runs under **Rosetta 2**; rendering runs in a
  separate native arm64 process. A Vulkan runtime (KosmicKrisp) is still bundled (`README.md`,
  `tools/macos/`, ICD files), so it does **not** satisfy our no-Vulkan rule either.
- **Own GPU code:** `gpu/cp/` is their own PM4 decoder (cursor, register file, `Sink`, shadow
  self-test), and `gpu/shader/` their own GCN → IR → SPIR-V translator. Their claim: the shadow
  self-test matched ~800k command buffers / ~200M packets with 0 divergences, and the shader
  translator produced byte-identical SPIR-V over the game corpus (their measurement, **UNVERIFIED
  here**). This is the best-shaped module boundary in the set for a command processor.
- **CPU lifting (documented, not shipped):** `docs/CPU_RECOMPILATION_ANALYSIS.ru.md` describes a
  static-recompilation prototype — plaintext SELF → verified ELF → unwind (FDE) ranges + control
  flow graph → selected integer functions → **LLVM IR → x86-64 and AArch64 objects**, with no
  decoder/interpreter/JIT at execution time. Reported there: 162,959 FDE ranges, 9,358,972 decoded
  instructions, 407,756 direct calls, 158,095 indirect calls, 15,073 indirect jumps, 22
  undecodable addresses, 5,635 FDE-escaping branches; 9,613 of 26,651 candidates accepted by the
  strict lifter; a **140-function** prototype matched the original x86-64 code in 308,420
  comparisons and AArch64-under-QEMU in 46,340 comparisons, 0 divergences each.
  **The `recompiler/` source is absent from every published branch**, so the pipeline is
  documented but not runnable from the repository — treat it as a method, not a tool.
- **Stated limits of that method:** integer-only contract (six integer arguments, RAX result, no
  memory, calls or branches); memory/stack/SSE-AVX/BMI/FP, flags, MXCSR/NaN, LOCK/TSO, exception
  landing pads, switch tables and a guest-address → AOT-entry map are **not** done. It also states
  `libc.prx` (751,908 RX bytes) and `libSceFios2.prx` (309,956 RX bytes) must be translated or
  replaced for arm64.
- `docs/TRANSLATOR_ARCHITECTURE.ru.md` lists what they vendored from shadPS4 (about 116k lines) and
  their plan to replace it module by module with their own code.

**Verdict for us:** Rosetta + Vulkan execution is rejected, but the **PM4 decoder module boundary
and its shadow self-test** are the pattern to copy, and the FDE/CFG lifting checklist is exactly
the scale-up step our byte-checked unit lifting still owes.

### Wooloomooloo2/bloodborne_mac_port

- **Execution model:** original x86-64 executable under **Rosetta 2**, PS4 system libraries
  replaced by a C **HLE runtime** in `src/` (kernel, memory, threads, mutex/semaphore, file, save
  data, audio/NGS2/AJM, pad, content, RTC, net), GPU work in a separate native arm64 process
  sharing guest memory.
- **GPU:** `gpu/shadps4/video_core` + `gpu/shadps4/shader_recompiler`, rendering through
  **`renderer_metal/` (60 files)** — a direct Metal path. Their `docs/MACOS_PORT.md` states
  MoltenVK and the Vulkan loader were removed in 0.5.3; the renderer encodes Metal itself.
  `docs/DESIGN_METAL_BACKEND.md` explains the design: own the frame's command encoding, keep
  resource management initially, SPIR-V → MSL through SPIRV-Cross, pipelines compiled on background
  threads and kept in an `MTLBinaryArchive`, direct per-draw buffer/texture binding. Reported ~190
  render passes / ~409 encoders per frame before the change and 1,087 shader modules compiling in
  2.4 s (their measurement).
  `renderer_metal/` is a mixed GPL-2.0 codebase: `mtl_device/encoder/draw/present/scale/geometry/
  tessellation` credited to Mark Hall, `mtl_shader/pso_cache/vk_to_mtl/mtl_spirv_*` to Huax-1839.
  The API surface it needs from the engine: guest memory at fixed addresses + device-address
  buffers, a deferred `Record(Encoder&)` command list produced by PM4 parsing, SPIR-V + a
  `GraphicsPipelineKey`, buffer/texture caches, an `MTLSharedEvent` tick, and a `CAMetalLayer`
  present target.
- **HLE documentation:** `docs/HLE_CLASSIFICATION.md` classifies all **327** HLE functions the GPU
  library registers (254 GnmDriver, 30 VideoOut, 17 event queue, 26 AvPlayer) and states only
  **77** are imported by that game — a ready-made behavioural map of the PS4 services a port must
  replace.

**Verdict for us:** rejected as an execution model (Rosetta + guest executable), but its **direct
Metal command path**, **pipeline archive + background shader compilation**, **render-pass
coalescing**, geometry/tessellation emulation and **HLE service map** are the closest public
blueprint for our M2/M3 work. Everything there is GPL-2.0-or-later.

### truwa/nathan-drake-collection-macos

A **Swift launcher** around a third-party Windows PC port, bundling that port plus KosmicKrisp and
requiring Rosetta 2 (with a legacy CrossOver/Wine path in `tls_adapter.c`, `patch_probe.py`). No
CPU or GPU translation of its own, and **no licence file at all**, so its code is legally
unusable. Its value is packaging: Developer ID signing, `notarytool` + `stapler`, UDZO DMG,
game-folder/update-folder selection, per-user support folder, credits/notices file.

**Verdict for us:** execution model rejected; **packaging and launcher UX patterns** are the best
public chain in the set, but must be re-implemented — no code may be copied.

## 2. Execution-model comparison

| | This repo (target) | bbport-mac | bloodborne_mac_port | nathan-drake-collection-macos |
|---|---|---|---|---|
| Game CPU code | Native arm64 functions produced by byte-checked lifting | x86-64 under Rosetta 2 | x86-64 under Rosetta 2 | x86-64 under Rosetta 2 (via PC port) |
| PS4 libraries | Replaced by native services; never executed | HLE runtime | HLE runtime (C) | HLE inside PC port |
| GPU | Direct Metal, our own command encoding | Vulkan on KosmicKrisp (Metal underneath) | Direct Metal (`renderer_metal/`) | KosmicKrisp |
| Vulkan/MoltenVK at run time | **Forbidden** | Yes (bundled ICD) | No (removed in 0.5.3) | Yes |
| Guest CPU at run time | **Forbidden** | Yes (Rosetta) | Yes (Rosetta) | Yes (Rosetta) |
| Licence | none yet — decision pending | GPL-2.0-or-later | GPL-2.0-or-later | all rights reserved |

## 3. Reuse verdicts

`ADOPT` = use directly (licence permitting), `PORT` = re-implement the design in our code,
`REFERENCE` = read for semantics only, `REJECT` = incompatible.

| Component | Project | Path | Licence | Verdict | Why |
|---|---|---|---|---|---|
| Own PM4 decoder (cursor + register file + `Sink`) + shadow self-test | bbport-mac | `gpu/cp/cp_decoder.{h,cpp}`, `cp_pm4.h`, `command_processor.cpp`, `pm4_selftest.*` | GPL-2.0-or-later | PORT | Best module boundary in the set; copy the shadow-check gate (~800k buffers / 0 divergences is their claim) |
| GCN → IR → SPIR-V translator | bbport-mac | `gpu/shader/**` | GPL-2.0-or-later | REFERENCE | Most complete GCN semantics reference; their byte-identical corpus test is the model for ours |
| Static x86-64 → LLVM IR → arm64 lifting method | bbport-mac | `docs/CPU_RECOMPILATION_ANALYSIS.ru.md` (no code published) | GPL-2.0-or-later (doc) | PORT | Same goal as ours; their contract/limits are the checklist we still owe |
| Metal device/encoder/order/timeline layer | bloodborne_mac_port | `renderer_metal/mtl_device.*`, `mtl_encoder.*`, `mtl_order.*`, `mtl_timeline.*` | GPL-2.0-or-later | PORT | Only working direct-Metal command path for PS4 streams; small explicit API |
| Metal draw/present/scale + geometry/tessellation emulation | bloodborne_mac_port | `renderer_metal/mtl_draw*.mm`, `mtl_present.*`, `mtl_scale.*`, `mtl_geometry.*`, `mtl_tessellation.*` | GPL-2.0-or-later | PORT | Solves Metal's gaps (no geometry shaders, rect lists, tessellation) that our title will also hit |
| SPIR-V → MSL + PSO cache design | bloodborne_mac_port | `renderer_metal/{mtl_shader,mtl_pso_cache,vk_to_mtl,mtl_spirv_*}` | GPL-2.0-or-later | PORT | Pipeline archive + background compile; pure Metal API, no translation runtime |
| SPIRV-Cross (upstream) | khronos | `third_party` | Apache-2.0 | REFERENCE | Offline SPIR-V → MSL reference implementation; vendoring is a separate licence-compatible decision |
| PS4 HLE classification (327 entries, 77 imported) | bloodborne_mac_port | `docs/HLE_CLASSIFICATION.md` | GPL-2.0-or-later (doc) | REFERENCE | Behavioural spec for the services we must implement natively |
| HLE runtime service semantics | bloodborne_mac_port | `src/runtime_{kernel,memory,thread,mutex,file,savedata,audio,pad,content,rtc}.c` | GPL-2.0-or-later | REFERENCE | Documents PS4 service behaviour; our implementations must be our own |
| MetalFX / FSR 3.1 Metal back end | bloodborne_mac_port | `gpu/shim/bbport_metalfx.mm`, `gpu/shim/ffx_metal/` | GPL-2.0-or-later / MIT (FFX) | REFERENCE | Needed only once we have a native frame to upscale |
| Signed, notarised DMG + game-folder picker | nathan-drake-collection-macos | `build_app.sh`, `launch.py`, `macos/` | **none** | PORT (design only, no code) | Best packaging chain; must be re-implemented |
| Rosetta 2 / guest executable execution | both Bloodborne ports | — | — | REJECT | Forbidden by this repo's target |
| KosmicKrisp / MoltenVK runtime | bbport-mac, nathan-drake | — | MIT | REJECT | Vulkan runtime is explicitly out of scope |

## 4. Direct answers

1. **Is there a published component that turns PS4 x86-64 code into ordinary native functions
   without a guest CPU at run time?** **No.** bbport-mac documents such a pipeline and its
   verification numbers (308,420 x86-64 and 46,340 arm64-under-QEMU comparisons, 0 divergences),
   but the `recompiler/` source is not published, and it covers only 140 memory-free integer leaf
   functions. Our own `native/` byte-checked lifts are the only running instance in either
   project.
2. **Is there a direct-Metal renderer for PS4 command streams we could adopt or port?** **Yes, to
   port** (not adopt): `bloodborne_mac_port/gpu/shadps4/video_core/renderer_metal/` (60 files). It
   is GPL-2.0-or-later, so vendoring forces this repository to GPL-2.0-or-later; re-implementing
   the design does not.
3. **Is there an offline GCN → MSL tool we could run over a shader corpus?** **No single tool.**
   Both projects do GCN → SPIR-V (shadPS4-derived `shader_recompiler` or bbport's own
   `gpu/shader/`), then SPIR-V → MSL through SPIRV-Cross. Offline runners exist
   (`bbport-mac/tests/test_shader_corpus.cpp`, `bloodborne_mac_port`'s `bbgpu-host --msl-check`),
   but neither is packaged as a standalone tool.
4. **What does adopting GPL-2.0 code imply?** The whole distributed work must be GPL-2.0-or-later
   with complete corresponding source; no relicensing. **This repository currently has no
   `LICENSE` file at all**, which means all rights reserved and is incompatible with vendoring
   either port. Reading for semantics and re-implementing the design carries no such obligation.

## 5. What this changes in our plan

1. **Keep our execution model.** The two mature ports confirm that the GPU and service boundary —
   not the CPU fetch loop — dominates their engineering, but they buy that with Rosetta, which we
   may not ship. Our byte-checked lifting stays the differentiator and the only compliant route.
2. **Add a scale-up step to M1.** Move from per-unit lifts to whole-program CFG + a guest-address →
   native-entry map (`CPU-01-AOT-CFG-SCALE-01`), following the published FDE/CFG method and its
   stated gaps. No code reuse needed.
3. **Make M3 concrete with a shadow-checked command processor.** `GPU-01-PM4-TRANSLATOR-01` now has
   its first slice in-tree (`native/platform/pm4_decoder.*` + `test_pm4_decoder` +
   `tools/lift/check_pm4_decoder.py`): bounds-checked PM4 framing cross-checked against an
   independent Python oracle. Next slices: register-file/state tracking, typed draws, then Metal
   encoders with render-pass coalescing and an `MTLBinaryArchive` (`GPU-01-MSL-OFFLINE-01`).
4. **Reuse the HLE map as a specification.** `docs/HLE_CLASSIFICATION.md` tells us which PS4
   services a real game actually imports; use it as a checklist for our native service layer
   (`PLAT-01-SERVICE-MAP-01`), not as code.
5. **Copy the packaging, not the runtime.** Signed/notarised DMG with an explicit asset-import flow
   (`APP-01-NOTARIZED-DMG-01`).
6. **Licence decided (2026-11-01):** the owner chose **GPL-2.0-or-later** for this repository
   ([LICENSE](../LICENSE)), so GPL-2.0-or-later prior-art code may now be ported or vendored with
   attribution. `docs/ATTRIBUTION.md` records the rules (original headers, `NOTICES.md` entry,
   pinned upstream commit, no unlicensed code). Design-only re-implementation remains the default
   for anything we can write cleanly ourselves.
