# Pinned reference-source audit

Audit date: 2026-10-19, America/Chicago. Agent: `reference_audit`.
Purpose: evidence and bounded task recommendations for the ARM64/Metal roadmap.
This document reports source inspection, not game execution or completed binary recovery.

## Scope, identities, and evidence labels

- **VERIFIED SOURCE**: behavior visible in the pinned local source; it may still be untested.
- **UPSTREAM CLAIM**: a README, changelog, or benchmark report; not reproduced on this Mac.
- **INFERENCE / PROPOSED**: an engineering consequence or proposed implementation, not observed behavior.
- **UNKNOWN**: evidence is missing; later agents must not silently promote it to verified.
- SoTC: `reference/ShadowOfTheColossus-PC`, commit `70bb43859cd7ff181a6d409540b757866184d60d`.
- Bloodborne: `reference/bloodborne_pc`, commit `de2e38af055f09f819946951075c6bb119e62ae6`.
- Additional NGS2 check: `reference/shadPS4`, pin `0ebea15c24253b60f514fc37a348dabc8d0bcb90`.
- Source pins were checked locally. Reference checkouts were not modified or fetched by this audit.
- No dependencies were installed, game launched, runtime tested, or shared REA binary session used.
- Package extraction and Ghidra evidence belong to the principal's investigation and evidence ledger.

## Required interpretation

The final product needs ordinary ARM64 Mach-O game code, direct Metal, a Swift native launcher,
real boot, real game audio, and persistent saves. Original executable modules are offline evidence.
The final runtime cannot load `eboot.bin`, `boot*.bin`, `.prx`, `.sprx`, or firmware, or retain a
guest CPU/register-dispatch environment. See root `AGENTS.md` for the exact project contract.
An ARM64 launcher, window, triangle, tone, silent menu, or mock save does not satisfy this contract.

## Source navigation

All anchors below are relative to the named pinned checkout; links resolve to immutable upstream revisions.

| Area | Primary source anchors | What the next agent should inspect |
|---|---|---|
| CPU boot | [SoTC `src/probe.c:1137-1156`][s-probe-entry] | Linked executable initializers and direct original x86-64 entry call |
| SELF/ELF | [SoTC `scripts/prepare.py:26-85`][s-prepare] | Plaintext SELF reconstruction, x86 ELF validation, explicit rejection conditions |
| Module linking | [SoTC `scripts/link_modules.py:21-24`][s-link-modules] | Default libc/Fios modules and additional NGS2/ULT modules |
| Low pointers | [SoTC `src/probe.c:55-83`][s-low-image] | 31-bit static/heap-relative pointers; image reservation below 2 GiB |
| Memory | [SoTC `src/runtime_memory.c:1-44`][s-memory] | Aliased backing, 40-bit GPU addresses, 16 KiB pages, address arenas |
| Thread TLS | [SoTC `src/runtime_thread.c:1-108`][s-thread] | FS/GS rewriting, Linux `arch_prctl`, guest TCB construction |
| Fibers | [SoTC `src/runtime_fiber.c:29-172`][s-fiber] | x86 context/assembly, floating-point state, migration/TLS warning |
| NGS2 audio | [SoTC `src/runtime_sotc.c:188-258`][s-ngs2] | Silent fallback, zeroed render buffers, placeholder Audio3D behavior |
| PCM/AJM | [SoTC `src/runtime_audio.c:151-217`][s-audio], [SoTC `src/runtime_ajm.c:1-12`][s-ajm] | Output pacing/channel mapping versus actual ATRAC9 decoding |
| Saves | [SoTC `src/runtime_savedata.c:188-251`][s-save] | Create/mount/unmount, host directories and service error behavior |
| Renderer boundary | [SoTC `gpu/bbgpu.h:1-49`][s-bbgpu] | Existing GNM/VideoOut/Vulkan-facing bridge and overlay/input coupling |
| Shader semantics | [SoTC `gpu/shadps4/shader_recompiler/backend/spirv/emit_spirv.cpp:469-490`][s-spirv] | Physical storage addresses and 64-bit atomics |
| Build | [SoTC `gpu/CMakeLists.txt:43-89`][s-cmake], [SoTC `build.sh:75-100`][s-build] | Current dependency/build graph and Linux linker assumptions |
| Bloodborne architecture | [Bloodborne `README.md:4-8`][b-readme] | Original x86-64 code execution plus Vulkan graphics |
| Performance caveats | [SoTC `docs/sotc/CAMINHO-B.md:11-38`][s-perf-caveat] | Bloodborne optimizations that broke SoTC boot, streaming, and readbacks |

## CPU boot and native game-code recovery

**VERIFIED SOURCE:** `src/probe.c:1137-1141` calls linked module initializer addresses.
`src/probe.c:1145-1156` then enters the original x86-64 executable directly.
`src/probe.c:930-977` reads BBPROBE containers and validates module/TLS metadata.
`scripts/link_modules.py:94-107,221-240` reads `eboot.bin` and writes combined executable bytes.
Renaming that output to a cache or asset does not remove its executable dependency.

`gpu/sotc/cpu_patches.cpp:539-544,908-921,2352-2361` uses Xbyak to patch x86 instructions.
Its `TryPatchAot` name denotes ahead-of-time x86 patching, not an ARM64 native game compiler.
**INFERENCE:** Metal-only work cannot make this CPU boot path work on Apple Silicon.

Proposed bounded tasks:
1. Record package/executable identity, effective update version, hashes, segments, imports, relocations, TLS, and unwind tables.
2. Recover the dependency graph from executable entry through constructors, job system, assets, menu, scene, and player spawn.
3. Give each recovered function its original address, callers/callees, data layouts, native symbol, test, and uncertainty.
4. Resolve indirect calls, callbacks, virtual tables, exceptions, globals, and middleware boundaries explicitly.
5. Replace used libc/Fios contracts with native implementations; the old loader also depends on these executable modules.
6. Compile recovered implementations into ordinary ARM64 functions; no guest register-machine fallback is acceptable.

**Gate:** cold native entry reaches actual game menu and controllable world, with module/file tracing
showing no original executable load. Source compilation alone is not boot evidence.
**UNKNOWN:** this audit established no game function coverage or extracted-executable correctness.

### Format-correct SELF helper

`scripts/prepare.py:40-85` accepts ELF or reconstructs already plaintext SELF segments.
It recognizes SELF magic `4f153d1d`, rejects encrypted/compressed segments at `68-69`, blocked
layouts at `74-75`, and unavailable required segments at `78-84`; this helper performs no decryption.
`parse_elf` at `26-37` validates little-endian x86-64 ELF, not ARM64 input.
Use it only after hashing the extracted file; save reconstruction hashes and rejection output.

## Packed pointers and memory

**VERIFIED SOURCE:** `src/probe.c:55-83` documents 31-bit pointers into static data, with bit 31
selecting a heap-relative form; Windows reserves the executable below 2 GiB.
`src/runtime_memory.c:1-8,38-44` separately describes 40-bit GPU addresses and 16 KiB pages.
`src/runtime_thread.c:287-296` notes packed stack addresses and low stack allocation.
`src/runtime_memory.c:253-269,704-727` uses Linux memfd/shared aliases and fixed-address mappings.

Proposed tasks:
1. Inventory tagged static pointers, heap-relative pointers, stack references, GPU descriptors, and ordinary native pointers separately.
2. Specify decoding helpers: tag bits, signedness, base, width, null semantics, bounds, alignment, and overflow.
3. Prefer recovered offsets/handles decoded to native pointers over dependence on reproducing Windows address placement.
4. Prove any indispensable low-address mapping on the actual signed ARM64 Mach-O before adopting that design.
5. Define native backing/alias/resource ownership and CPU/Metal visibility; unified memory alone does not order access.

**Gate:** fixtures cover both pointer-tag branches, null/boundary/overflow cases, aliased writes,
descriptor decoding, and CPU-to-GPU/GPU-to-CPU resource lifetimes. Linux mapping code is not a Darwin implementation.

## TLS, fibers, callbacks, and ABI

**VERIFIED SOURCE:** `src/runtime.h:18` selects the x86 System V ABI.
`src/runtime_thread.c:1-7,88-108` relies on FS/GS rewriting, Linux `arch_prctl`, and a guest TCB.
`src/runtime_fiber.c:29-34,87-172` saves/restores x86 registers, x87 control state, and MXCSR.
`src/runtime_fiber.c:74-79` warns that resumed fibers migrate across host threads and TLS must be reread.

Proposed tasks:
1. Preserve required fiber state/API semantics, but replace its private context with ARM64 state or a suitable native facility.
2. Keep task/fiber state distinct from worker TLS; migration must observe destination worker TLS without losing task state.
3. Specify native stack alignment, callee-saved/SIMD registers, floating-point control, callbacks, unwind, and cancellation.
4. Convert every executable callback/function pointer to a native target; do not cast old addresses into calls.
5. Adapt pthread naming/attributes/priorities for Darwin instead of sending macOS through the current Linux branch.
6. Reuse suitable existing Apple/ARM64 branches in vendored common utilities instead of rewriting them blindly.

**Gate:** create/run/yield/resume/return/finalize, migration, TLS isolation, callback nesting,
stack guards, shutdown, and floating-point state survive stress before the full job graph is attempted.

## Native Metal renderer

**VERIFIED SOURCE:** `gpu/bbgpu.h:1-49` joins renderer imports, fault/page tracking, fibers,
save/text overlays, and input. `gpu/shim/bbgpu.cpp:275-304` creates its SDL window on a private thread.
`gpu/shadps4/video_core/renderer_vulkan/vk_platform.cpp:106-116` creates a Vulkan Metal surface
around `CAMetalLayer`; that isolated Apple branch is not a native Metal renderer.
`shader_recompiler/backend/spirv/spirv_emit_context.cpp:71-79` uses physical storage addresses for DMA.
`shader_recompiler/backend/spirv/emit_spirv.cpp:469-490` adds physical-storage and int64 atomic capabilities.
`video_core/renderer_vulkan/vk_pipeline_cache.cpp:296-350` records wave, floating-point, and atomic requirements.
The last three paths are relative to `gpu/shadps4/`.

Proposed tasks:
1. Use GCN/resource/texture semantics and existing fixes as offline reconstruction evidence.
2. Implement production resources, pipelines, presentation, and commands with direct Metal APIs.
3. Keep any PM4/GCN conversion confined to documented offline tooling; retaining the PS4 GPU execution environment is not a silent scope substitution.
4. Generate identified Metal shader/material manifests offline and replace game-facing GNM calls with the native renderer boundary.
5. Test wave64/EXEC/WQM/helper lanes, LDS/barriers, resource indexing, vertex fetch, interpolation, packed formats, atomics, and tiling.
6. Progress through presentation, textures/depth, representative material, scene, compute/skinning, shadows, vegetation, water, postprocessing, and UI.
7. Integrate native main-thread window lifecycle; do not copy the existing SDL worker-thread arrangement unchanged.

**Gate:** Metal captures show native commands/resources; loaded-image inspection excludes Vulkan/MoltenVK.
Image and synchronization fixtures cover logos/menu/world/characters/shadows/streaming/water.
**UNKNOWN:** shader coverage and feature suitability on this Mac remain to be measured.

## Real audio without Sony modules

**VERIFIED SOURCE:** `src/probe.c:136-142` prefers linked `sceNgs2*`/`sceUlt*` exports over host stubs.
`src/runtime_sotc.c:188-209` explicitly renders silence; `469-478` maps VoiceControl and Audio3D attributes to success-only placeholders.
`src/runtime_audio.c:151-217` implements actual PCM output pacing, channel mapping, volume, queueing, and statistics.
`src/runtime_ajm.c:1-12,185-231` implements ATRAC9 decoding through LibAtrac9; its own comment says MP3/AAC instances stop.
The pinned shadPS4 [NGS2 implementation][sh-ngs2] also has STUBBED render at `249-258` and success-only VoiceControl at `310-314`.
**INFERENCE:** neither a device-output backend nor the available ATRAC9 decoder supplies the missing NGS2 mixer.

Proposed tasks:
1. Recover used NGS2 imports/call sites, rack IDs, parameter commands, voice graph, callbacks, and sound-bank readers.
2. Identify actual containers/codecs from extracted assets, not filenames alone; preserve sample identities and offsets.
3. Implement required handle/state lifecycle, command parsing, decoding, cursors/loops, resampling, gain/pan, buses, effects, and completion.
4. Determine the ULT contracts still needed after native recovery and implement those natively rather than shipping its module.
5. Reuse proven PCM/AJM semantics where applicable and implement the used Audio3D behavior instead of accepting no-op attributes.
6. Capture nonzero PCM and device output evidence; unsupported required commands must fail visibly, not return silent success.

**Gate:** audible menu/music/voice/ambience/footsteps/combat and positional behavior, transitions,
loops, pause/resume, teardown, device changes, and a sustained run, with no firmware module available.
**UNKNOWN:** actual SoTC codec, command, rack, and effect coverage requires executable/asset investigation.

## Saving and Swift launcher

**VERIFIED SOURCE:** `src/runtime_savedata.c:1-6` separates save files, metadata, and SaveDataMemory.
Mount/create/unmount are at `188-251`; SaveDataMemory persistence is at `360-400`.
Its ordinary file writes are not proof of crash-safe transactions or complete game serialization recovery.
The inherited Bloodborne-specific sound flag hack at `73-105` checks Bloodborne title IDs; it is not a SoTC audio fix.
`src/runtime_services.c:229-266` supplies the save/load/delete list through the graphics overlay.
The current launcher is Python/Tkinter (`launcher/sotc_launcher_win.py`), not Swift native.

Proposed tasks: preserve save-service layouts/error contracts; recover game serialization/checksums;
use native Application Support storage; implement safe commit/backup behavior; import by copying originals;
test read-only, disk-full, interrupted write, corrupted metadata, and failed replacement cases.
Build SwiftUI/AppKit asset selection, diagnostics, settings, save management, launch/stop, and readable logs.
Launch the actual ARM64 game directly; Python preparation or an x86 helper cannot remain a game-runtime dependency.
**Gate:** new game -> real save point -> save -> quit process -> relaunch -> load -> verified progress.
Repeat after abnormal termination and verify that a previously valid save remains usable.

## Build, dependency, and licensing boundaries

**VERIFIED SOURCE:** `gpu/CMakeLists.txt:43-89,131-166` references Vulkan, fmt, Boost, robin-map,
VMA, xxhash, SDL3, X11 on non-Windows, miniz, Zydis, glslang, sirit, FSR-Vulkan, and FFmpeg.
`build.sh:75-100` supplies Linux-oriented loader linkage (`-no-pie`, ELF rpaths) and runtime sources.
Existing non-Windows Linux APIs and x86 assembly mean this is not a ready native macOS build.
All six SoTC submodules were uninitialized when inspected; exact pins are in `git submodule status`/`git ls-tree HEAD`.
This audit did not fetch them, run a project build, or verify a complete third-party license set.

- SoTC `README.md:204` declares GPL-2.0-or-later; root `LICENSE` contains GPL v2 text.
- SoTC `gpu/VENDOR.txt:1-5` records vendored shadPS4 revision/provenance and GPL-2.0-or-later.
- SoTC `gpu/third_party/sirit/LICENSE.txt:1-24` contains BSD-style three-clause terms, read locally.
- LibAtrac9/FSR/ImGui MIT descriptions occur in comments; actual pinned submodule licenses were absent and remain unverified.
- Bloodborne [network provenance][b-net-license] records a GPL-3.0-or-later subtree at `gpu/bbnet/SHADNET-PORT.md:20-28`.
- Networking is not required for this initial SoTC scope; do not assign a blanket license to copied Bloodborne material.

Record every installed/downloaded/built dependency in `INSTALL_LOG.md`: version/hash, source,
command, path, architecture, purpose, runtime versus analysis-only use, verified license, and removal boundary.
These are observed license/provenance facts, not legal conclusions or a completed distribution review.

## Performance: claims, limits, and reusable method

**UPSTREAM CLAIM:** SoTC [CHANGELOG performance report][s-perf] uses i9-13900K + RTX 2060 SUPER,
60 FPS title demo: average 37-45 -> ~67 FPS, heavy section ~17 -> 29-34 FPS (`127-148`).
Bloodborne [preparation A/B][b-perf-prep] reports ~74 versus ~57.7 FPS on 16 threads (`144-150`).
Its [pipeline A/B][b-perf-pipe] reports 96.1 versus 80.6 FPS; four-core/eight-thread case 83.8 versus 71.0 (`372-377`).
Another [Bloodborne benchmark setup][b-perf-config] is RX 7800 XT/RADV, four cores/eight threads,
1280x720 output but 640x360 scene (`docs/CHANGES_2026-10-03.md:3-5`); its `99-103` report of 209 FPS is not Mac/native-1080p evidence.

**UPSTREAM CORRECTNESS REPORT:** SoTC `docs/sotc/CAMINHO-B.md:11-20` records 50-75% intermittent
black boots from PendingFenceWaits, stale/invalid shader state from draw preparation, and hangs from relaxed readbacks.
It requires precise readbacks (`BB_READBACKS=2`) and advises against copying Bloodborne's two-thread pipeline unchanged (`33-38`).
Its in-place-memory discussion identifies the original dma-buf approach as Linux/AMD-specific (`55-59`).
These reports are strong reasons for validation gates, not current runtime measurements from this audit.

**PROPOSED:** reuse profiling and controlled A/B methods; reimplement useful ideas for Metal only after correctness.
Record hardware/build/game hash, fixed scene/route, scene/output resolution, frame cap, thermal state,
cold/warm shader state, CPU/GPU timings, and frame-time percentiles. No Mac FPS promise is supported yet.

## Agent checkpoints and escalation

At every checkpoint send the integrator task ID, timestamp/agent, files, exact commands/results,
evidence paths, observations/inferences, unresolved gaps, next action, status, and cumulative attempts.
The integrator owns shared HANDOFF/task/evidence/index updates while workers own distinct assigned files.
After the third distinct failed attempt, use root `AGENTS.md` status `NEEDS_PRINCIPAL`, preserve all
three hypotheses/results and smallest reproducer, and stop that task until principal reassessment.
Do not replace a blocked boot/audio/save/native requirement with a stub or weaker acceptance gate.
Index this document through the project index; preserve these source pins when future references change.

## Immutable upstream source links

[s-probe-entry]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/probe.c#L1137-L1156
[s-prepare]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/scripts/prepare.py#L26-L85
[s-link-modules]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/scripts/link_modules.py#L21-L24
[s-low-image]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/probe.c#L55-L83
[s-memory]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_memory.c#L1-L44
[s-thread]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_thread.c#L1-L108
[s-fiber]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_fiber.c#L29-L172
[s-ngs2]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_sotc.c#L188-L258
[s-audio]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_audio.c#L151-L217
[s-ajm]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_ajm.c#L1-L12
[s-save]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/src/runtime_savedata.c#L188-L251
[s-bbgpu]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/gpu/bbgpu.h#L1-L49
[s-spirv]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/gpu/shadps4/shader_recompiler/backend/spirv/emit_spirv.cpp#L469-L490
[s-cmake]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/gpu/CMakeLists.txt#L43-L89
[s-build]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/build.sh#L75-L100
[b-readme]: https://github.com/Supermedo/bloodborne_pc/blob/de2e38af055f09f819946951075c6bb119e62ae6/README.md#L4-L8
[s-perf-caveat]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/docs/sotc/CAMINHO-B.md#L11-L38
[sh-ngs2]: https://github.com/shadps4-emu/shadPS4/blob/0ebea15c24253b60f514fc37a348dabc8d0bcb90/src/core/libraries/ngs2/ngs2.cpp#L249-L314
[b-net-license]: https://github.com/Supermedo/bloodborne_pc/blob/de2e38af055f09f819946951075c6bb119e62ae6/gpu/bbnet/SHADNET-PORT.md#L20-L28
[s-perf]: https://github.com/AjncSystem/ShadowOfTheColossus-PC/blob/70bb43859cd7ff181a6d409540b757866184d60d/CHANGELOG.md#L127-L148
[b-perf-prep]: https://github.com/Supermedo/bloodborne_pc/blob/de2e38af055f09f819946951075c6bb119e62ae6/docs/parallel_gpu.md#L132-L150
[b-perf-pipe]: https://github.com/Supermedo/bloodborne_pc/blob/de2e38af055f09f819946951075c6bb119e62ae6/docs/parallel_gpu.md#L372-L377
[b-perf-config]: https://github.com/Supermedo/bloodborne_pc/blob/de2e38af055f09f819946951075c6bb119e62ae6/docs/CHANGES_2026-10-03.md#L3-L5
