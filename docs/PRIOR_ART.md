# Prior art: Apple-Silicon ports of PS4 games (assessed 2026-11-01)

This note records what three public Mac ports actually do, which of their tools and designs fit
this repository's non-negotiables, and what we will therefore build. It is an assessment of
published sources (READMEs and design docs fetched 2026-11-01); we have not built or run any of
the three, and their performance numbers are **their claims, not our measurements**.

Sources (commit/branch recorded at fetch time):

| Project | Branch | Licence | Language | Stars |
|---|---|---|---|---|
| [bmy/bbport-mac](https://github.com/bmy/bbport-mac) | `macos-native-0.6` | GPL-2.0 | C++ | 50 |
| [Wooloomooloo2/bloodborne_mac_port](https://github.com/Wooloomooloo2/bloodborne_mac_port) | `master` | GPL-2.0 | C++ / ObjC++ | 251 |
| [truwa/nathan-drake-collection-macos](https://github.com/truwa/nathan-drake-collection-macos) | `main` | none stated in repo | Swift | 32 |

## 1. What each project does

### bmy/bbport-mac

A macOS fork of `deadinside28/bloodborne_pc` (a native re-implementation of the PS4 game's
services around the original executable).

- **Execution model:** the game's own x86-64 `eboot` runs under **Rosetta 2**; the renderer runs
  as a separate **native arm64** process (`README.md`, "Built for the Mac").
- **GPU:** bbport's Vulkan renderer on **KosmicKrisp** (Mesa's Vulkan-on-Metal), plus MetalFX and
  FSR 3.1. A native Metal command path is described in `docs/macos-native-gpu.md`.
- **CPU tooling (most relevant to us):** `docs/CPU_RECOMPILATION_ANALYSIS.ru.md` documents a
  static-recompilation prototype: plaintext SELF → verified ELF → unwind (FDE) ranges + control
  flow graph → selected integer functions → **LLVM IR → x86-64 and AArch64 objects**, with no
  decoder/interpreter/JIT at execution time. Measured there: 162,959 FDE ranges, 9,358,972
  decoded instructions, 407,756 direct calls, 158,095 indirect calls, 15,073 indirect jumps,
  235,000 relocations; the strict lifter accepted 9,613 of 26,651 candidate functions and the
  140-function prototype matched the original x86-64 code in **308,420 comparisons with 0
  mismatches**, and under QEMU for AArch64 in **46,340 comparisons with 0 mismatches**.
- **Stated limits:** the lifter contract is integer-only (six integer arguments, RAX result, no
  memory, calls or branches); memory, stack, SSE/AVX/BMI/FP, flags, MXCSR/NaN, LOCK/TSO, exception
  landing pads, switch tables and the guest-address → AOT-entry mapping are **not** done. It also
  states that `libc.prx` (751,908 RX bytes) and `libSceFios2.prx` (309,956 RX bytes) must be
  translated or replaced for arm64.
- `docs/TRANSLATOR_ARCHITECTURE.ru.md` lists what they vendored from shadPS4 (about 116k lines:
  `video_core/amdgpu` 8.3k, `renderer_vulkan` 28.6k, `buffer_cache` 7.4k, `texture_cache` 4.8k,
  `shader_recompiler` 38.6k, `core`+`common` 21.8k) and their plan to replace it module by module
  with their own `gpu/cp/` PM4 decoder, memory layer and renderer.

**Verdict for us:** the Rosetta execution model is rejected, but the **lifting methodology and its
verification harness are exactly the approach this repository already uses at unit scale**. Their
numbers give us the missing whole-program checklist (CFG closure, guest-address mapping,
runtime/PRX translation, ABI bridges).

### Wooloomooloo2/bloodborne_mac_port

- **Execution model:** original x86-64 executable under **Rosetta 2**, PS4 system libraries
  replaced by a C **HLE runtime** in `src/` (kernel, memory, threads, audio, input via SDL3, save
  data), GPU work in a separate native arm64 process sharing guest memory.
- **GPU:** `gpu/shadps4/video_core` + `gpu/shadps4/shader_recompiler` built as `libbbgpu.dylib`,
  rendering through **its own native Metal renderer** (`renderer_metal/`) since 0.5.0 — no Vulkan
  and no MoltenVK in the shipped app. `docs/DESIGN_METAL_BACKEND.md` explains why and how: own the
  frame's command encoding, keep resource management initially, SPIR-V → MSL through SPIRV-Cross,
  pipelines compiled on background threads and kept in an `MTLBinaryArchive`, direct per-draw
  buffer/texture binding instead of descriptor sets. It reports ~190 render passes and ~409
  encoders per frame before the change, and 1,087 shader modules compiling in 2.4 s on 8 threads.
- **HLE documentation:** `docs/HLE_CLASSIFICATION.md` classifies all **327** HLE functions the GPU
  library registers (254 GnmDriver, 30 VideoOut, 17 event queue, 26 AvPlayer) and states that only
  **77** of them are imported by that game. This is a ready-made behavioural map of the PS4
  services a port must replace.
- Other reusable designs: `docs/DESIGN_NATIVE_GPU_PROCESS.md` (cross-process shared guest memory),
  `gpu/shim/ffx_metal/` (FSR 3.1 on a Metal back end), `docs/DESIGN_INPUT.md`.

**Verdict for us:** rejected as an execution model (Rosetta + guest executable), but its **direct
Metal command path**, **pipeline archive + background shader compilation**, **render-pass
coalescing** and **HLE service map** are the closest public blueprint for milestones M2/M3 here.

### truwa/nathan-drake-collection-macos

A **Swift launcher** around someone else's Windows PC port, bundling that port plus KosmicKrisp
and requiring Rosetta 2. No CPU or GPU translation of its own. Its value is packaging:
Developer-ID signing, notarised DMG, game-folder/update-folder selection, per-user support folder,
and a credits/notices file.

**Verdict for us:** execution model rejected; **packaging and launcher UX patterns** are worth
copying (signed, notarised `.dmg`, explicit "no game files" flow, `~/Library/Application Support`
state directory).

## 2. Execution-model comparison

| | This repo (target) | bbport-mac | bloodborne_mac_port | nathan-drake-collection-macos |
|---|---|---|---|---|
| Game CPU code | Native arm64 functions produced by byte-checked lifting | x86-64 under Rosetta 2 | x86-64 under Rosetta 2 | x86-64 under Rosetta 2 (via PC port) |
| PS4 libraries | Replaced by native services; never executed | HLE runtime | HLE runtime (C) | HLE inside PC port |
| GPU | Direct Metal, our own command encoding | Vulkan on KosmicKrisp (Metal underneath) | Direct Metal (`renderer_metal/`) | KosmicKrisp |
| Vulkan/MoltenVK at run time | **Forbidden** | Yes (KosmicKrisp) | No | Yes (KosmicKrisp) |
| Guest CPU at run time | **Forbidden** | Yes (Rosetta) | Yes (Rosetta) | Yes (Rosetta) |

## 3. Reuse verdicts

`ADOPT` = use directly (licence permitting), `PORT` = re-implement the design in our code,
`REFERENCE` = read for semantics only, `REJECT` = incompatible with our constraints.

| Component | Project | Path | Licence | Verdict | Why |
|---|---|---|---|---|---|
| Static x86-64 → LLVM IR → arm64 lifting pipeline | bbport-mac | `docs/CPU_RECOMPILATION_ANALYSIS.ru.md`, `recompiler/` | GPL-2.0 | PORT | Same goal as ours (ordinary native functions, no guest CPU); their contract/limits are the checklist we still owe |
| Whole-program CFG recovery from FDE + call graph | bbport-mac | same doc | GPL-2.0 | PORT | We already recover bytes; FDE-based CFG closure is the missing scale-up step |
| Guest-address → native-entry mapping (vtables, callbacks, threads) | bbport-mac | same doc, "what is needed" §3 | GPL-2.0 | PORT | Our lift registry needs exactly this table |
| PS4 PM4/GNM command decoding | bloodborne_mac_port | `gpu/shadps4/video_core/amdgpu` | GPL-2.0 | REFERENCE (code needs licence decision) | Defines the command stream we must decode ourselves |
| Direct Metal renderer for PS4 draws | bloodborne_mac_port | `renderer_metal/`, `docs/DESIGN_METAL_BACKEND.md` | GPL-2.0 | PORT | The design (own encoders, pass coalescing, binary archive) fits our G2 ban on Vulkan |
| GCN shader recompiler (GCN → IR → SPIR-V → MSL) | bloodborne_mac_port | `gpu/shadps4/shader_recompiler`, SPIRV-Cross | GPL-2.0 / Apache-2.0 | REFERENCE / offline tool | We need an offline shader path; vendoring is a licence decision |
| Pipeline archive + background shader compile | bloodborne_mac_port | `renderer_metal/`, `docs/DESIGN_METAL_BACKEND.md` | GPL-2.0 | PORT | Removes first-run stutter; pure Metal API, no translation |
| PS4 HLE service classification (327 entries, 77 imported) | bloodborne_mac_port | `docs/HLE_CLASSIFICATION.md` | GPL-2.0 (doc) | REFERENCE | Behavioural spec for the services we must implement natively |
| MetalFX / FSR 3.1 Metal back end | bloodborne_mac_port | `gpu/shim/bbport_metalfx.mm`, `gpu/shim/ffx_metal/` | GPL-2.0 / MIT (FFX) | REFERENCE | Useful once we have a native frame to upscale |
| Native GPU process + shared memory | bloodborne_mac_port | `docs/DESIGN_NATIVE_GPU_PROCESS.md` | GPL-2.0 | REFERENCE | Only needed if we split processes; our core is already one native process |
| Signed, notarised DMG + game-folder picker | nathan-drake-collection-macos | `build_app.sh`, `launch.py`, `macos/` | none stated | PORT | Packaging checklist for G4 |
| Rosetta 2 / guest executable execution | both Bloodborne ports | — | — | REJECT | Forbidden by this repo's target |
| KosmicKrisp / MoltenVK runtime | bbport-mac, nathan-drake | — | MIT | REJECT | Vulkan runtime is explicitly out of scope |

## 4. Direct answers

1. **Is there a published component that turns PS4 x86-64 code into ordinary native functions
   without a guest CPU at run time?** Yes, as a **prototype only**: bbport-mac's `recompiler/`
   (documented in `docs/CPU_RECOMPILATION_ANALYSIS.ru.md`) emits x86-64 and AArch64 objects from
   LLVM IR and validates them against the original code (308,420 / 46,340 comparisons, 0
   mismatches under QEMU for arm64). It covers integer-only leaf functions; the game as a whole is
   explicitly not recompiled. No project ships a fully statically recompiled game.
2. **Is there a direct-Metal renderer for PS4 command streams we could adopt or port?** Yes:
   Wooloomooloo2's `renderer_metal/` (Metal encoders, pass structure, pipeline archive, MetalFX).
   It expects the engine to hand it decoded draws and resource bindings — i.e. the seam we must
   build in our `GPU-01` work.
3. **Is there an offline GCN → MSL tool we could run over a shader corpus?** The pieces exist
   (`gpu/shadps4/shader_recompiler` for GCN → SPIR-V, then SPIRV-Cross for SPIR-V → MSL), and that
   port reports compiling its whole cache that way, but it is not packaged as a standalone offline
   tool. Using it is a licence decision (GPL-2.0), and SPIRV-Cross itself is Apache-2.0.
4. **What does adopting GPL-2.0 code imply?** The derivative work must be distributed under
   GPL-2.0 (source available, same licence). Both Bloodborne ports and shadPS4 are GPL-2.0, so any
   vendoring of their renderer or shader compiler makes this repository GPL-2.0. Design-level
   reuse (reading the docs, re-implementing the architecture) carries no such obligation.

## 5. What this changes in our plan

1. **Keep our execution model.** The two successful ports confirm that the hard part is the
   GPU/service boundary, not the CPU fetch loop — but they buy that with Rosetta, which we may not
   ship. Our byte-checked lifting stays the differentiator and the only compliant route.
2. **Add a scale-up step to M1.** Move from per-unit lifts to a whole-program CFG + guest-address
   map (`docs/TASKS.md`: `CPU-01-AOT-CFG-SCALE-01`), following the FDE/CFG method and its stated
   gaps. No code reuse needed.
3. **Make M3 concrete.** Build our own decode → Metal encoder path with render-pass coalescing and
   an `MTLBinaryArchive`, and an offline GCN → MSL step, instead of continuing ad-hoc Metal
   fixtures (`GPU-01-PM4-TRANSLATOR-01`, `GPU-01-MSL-OFFLINE-01`).
4. **Reuse the HLE map as a specification.** `docs/HLE_CLASSIFICATION.md` tells us which PS4
   services a real game actually imports; use it as a checklist for our native service layer
   (`PLAT-01-SERVICE-MAP-01`), not as code.
5. **Copy the packaging, not the runtime.** Signed/notarised DMG with an explicit asset-import flow
   (`APP-01-NOTARIZED-DMG-01`).
6. **Licence decision first.** No GPL-2.0 code enters the tree until the owner records a licence
   choice (see `docs/ATTRIBUTION.md`).
