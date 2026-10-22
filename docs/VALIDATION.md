# Validation gates and evidence protocol

Current checkpoint: G0 PARTIAL; G1 PARTIAL (see E404–E487 —
`docs/checkpoints/2026-10-22-g1-partial-evidence-pack.md`,
`docs/checkpoints/2026-10-22-appmain-init-registry-slice.md`,
`docs/checkpoints/2026-10-22-init-531d60-trivial-leaves.md`,
`docs/checkpoints/2026-10-22-base-initialize-255130.md`,
`docs/checkpoints/2026-10-22-prologue-255180-early.md`,
`docs/checkpoints/2026-10-22-prologue-255180-tail.md`,
`docs/checkpoints/2026-10-22-ctor-98a90-3be050.md`,
`docs/checkpoints/2026-10-22-ctor-98a90-3be050-bufgrow.md`, and
`docs/checkpoints/2026-10-22-ctor-98a90-3be050-listwalk.md`, and
`docs/checkpoints/2026-10-22-store-tsc-323d0.md`, and
`docs/checkpoints/2026-10-22-ensure-3c2f70.md`, and
`docs/checkpoints/2026-10-22-init-sub-3be930.md`, and
`docs/checkpoints/2026-10-22-ctor-40-1ab550.md`, and
`docs/checkpoints/2026-10-22-prologue-gates-2552d4.md`, and
`docs/checkpoints/2026-10-22-vslot-30-534130.md`, and
`docs/checkpoints/2026-10-22-mid-255340.md`,
`docs/checkpoints/2026-10-22-singleton-f08-531d84.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-header.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-post.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-miss.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-join.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-gfx.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadow.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadow-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowskels.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-skels-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowanims.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-anims-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowbanks.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-banks-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowstreams.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-streams-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowrandomevents.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-randomevents-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-shadowbossmusic.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-bossmusic-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-cp3characters.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-cp3characters-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache-attach.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-tail-140d90.md`,
`docs/checkpoints/2026-10-22-vslot-40-533100-stackchk.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-499f80.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-56dcb0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-5c64c0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-1290.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-56db70.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-5b3d00.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-5b3cf0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-avx-bss.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-7f7220.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-56fad0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-f90310.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-ymm-bss.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-ymm-intercell.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-xmm-seed.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-ymm-reload.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-26800-f590.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-sbj.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-sboj.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-52eb50.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-lea-vmovq.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ca0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ca0-u2.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-u2.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ca0-u3.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-u3.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ca0-u4.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-u4.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ca0-u5.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-u5.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ed0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-532ed0-u2.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0-u2.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-17070.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-544fe0.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-cell-2eff450.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-17070-u2.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-53fd10.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-cell-2eff448.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-vcall38.md`,
`docs/checkpoints/2026-10-22-init-531d60-post-base-lea-32f4fe0.md`);
G2-G6 NOT RUN. This is a plan, not a game execution report. G1 is not PASSED.
Artifact/tool/source evidence is recorded in EVIDENCE.md; full asset/linkage schemas remain open.
Baseline supplied by the coordinating agent on 2026-10-19, America/Chicago: M4 Pro, 12 CPU cores,
48 GB unified memory, macOS 27.0.1, Xcode 27.1. Read-only machine/toolchain capture on 2026-10-20
00:34 CDT matched those values and additionally recorded arm64, macOS build 26A434, Xcode build
27A9275, Metal compiler path, and macOS SDK 27.0. All six candidate baseline commands exited 0
with empty stderr; exact per-command outputs, durations, and statuses are private under
`local/validation/g0-env-baseline-20261020/` (capture JSON SHA-256
`cea524d435eeef15e02d506744969dab872990e4812d6621d6b256f042e475b0`; evidence E50). Re-record
actual versions for every evidence run. This is toolchain inventory only, not a Metal compile or G0
pass.
Follow [AGENTS.md](../AGENTS.md), [PLATFORM.md](PLATFORM.md), [ROADMAP.md](../ROADMAP.md),
[task registry](TASKS.md), [evidence registry](EVIDENCE.md), and [HANDOFF.md](../HANDOFF.md).

## Status and promotion rules

| Gate | Deliverable | Current status |
| --- | --- | --- |
| G0 | Local inventory, provenance, toolchain, indexing | PARTIAL |
| G1 | Pure native CPU/game-core feasibility | PARTIAL (startup harness + Init-hash + prologue early+tail+gates+lifted 0x3be050+0x1ab550→0xf08 singleton→base with lifted vslot 0x534130+mid 0x255340+0x533100 header+post/1fc80→registry; unresolved edges in E404–E422; not PASSED) |
| G2 | Native services, audio fixtures, save fixtures | NOT RUN |
| G3 | Direct Metal resources, shader semantics, rendering | NOT RUN |
| G4 | Native Swift/macOS launcher and lifecycle | NOT RUN |
| G5 | Actual game boot, actual sound, real save/reload | NOT RUN |
| G6 | Final packaging, native-only audit, stability/performance | NOT RUN |

Use only NOT RUN, FAILED, PARTIAL, or PASSED for gate execution.
Implemented is a development state, not a substitute for a passed acceptance gate.
A passing subfixture permits PARTIAL with the remaining failures listed.
Source inspection, compiler success, synthetic fixtures, and real-game verification stay distinct.
Never promote a gate because an upstream README, previous agent, screenshot, or index says it works.
A game window, triangle, tone, static movie, asset viewer, or arbitrary file write cannot pass G5.
Run independent fixtures early, but close dependent gates only after their prerequisites pass.
No default-success fallback, disabled assertion, muted error, or weaker test may close a blocked gate.

## Evidence record for every run

Record local date/time/timezone, agent/task ID, gate ID, attempt number, hypothesis, and input hashes.
Record source pins, native revision/diff identity, compiler/SDK/tool versions, and configuration.
Record exact command, working directory, relevant nonsecret environment, exit status, and duration.
Save logs and private captures beneath a unique local/validation/<run-id>/ directory.
Describe expected versus observed behavior; label inference and unverified behavior separately.
For binary claims, include artifact hash, image base, addresses, tool identity, and REA evidence IDs.
For fixtures, include deterministic input/expected-output hashes and tolerance rationale.
For visual/audio claims, preserve reproducible inputs, capture settings, timestamps, and observations.
Link evidence in docs/EVIDENCE.md without publishing game code, executable dumps, or personal data.
Record changed files, remaining risks, next exact action, task status, and cumulative failed tries.
Update HANDOFF.md at each checkpoint through the single integrator; confirm the record was added.
After the third unsuccessful distinct hypothesis, mark NEEDS_PRINCIPAL and preserve the reproducer.
A new agent, larger timeout, repeated command, or model change does not reset the attempt count.

## G0 — inventory, provenance, and reproducible environment

Entry: original user inputs remain read-only and analysis/extraction paths are documented.
- Hash the supplied base/update/DLC inputs and record sizes without duplicating them into source.
- Record effective game revision, extraction recipe, tool revision, outputs, and failure boundaries.
- Identify eboot/module formats after extraction; do not interpret a PKG rejection as executable analysis.
- Inventory executable sections, asset containers, audio banks, shaders, and save-related evidence.
- Record both reference repositories' exact commits and the relevant source paths used by the plan.
- Record all installs/downloads/builds, versions, locations, commands, purpose, and removal boundary.
- Verify ARM64 toolchain availability and selected SDK; do not install duplicate tools blindly.
- Ensure each project folder has an index entry or documented private/generated exclusion.
- Refresh root documentation indexing and check Graft's appropriate source scopes before handoff.
Pass: a fresh agent can locate inputs, tools, source pins, evidence, task ownership, and next action.
Fail: unknown executable revision, missing provenance, modified original inputs, or unrecoverable evidence.
Evidence: manifests, extraction logs, toolchain record, index output, and scope/coverage notes.

Candidate read-only baseline commands; execute separately and retain their actual outputs:
```sh
uname -m
sw_vers
sysctl machdep.cpu.brand_string hw.ncpu hw.memsize
xcodebuild -version
xcrun --find metal
xcrun --sdk macosx --show-sdk-version
```
These commands have not been executed as this document's acceptance run.

## G1 — pure native CPU feasibility

Entry: G0 executable identity and reproducible offline analysis evidence exist.
- Demonstrate a real recovered startup path compiled into ARM64 Mach-O native functions.
- Identify initialization registrations, reachable game-core functions, and unresolved indirect calls.
- Prove recovered call signatures, struct layouts, callbacks, TLS, globals, and native ownership.
- Test original instruction/numeric edge semantics where portability is not automatic.
- Audit all x86 vector intrinsics, memory-ordering assumptions, guest addresses, and fixed page sizes.
- Prove that required platform imports have native replacements rather than fake successful returns.
- Inspect the runtime design for a guest CPU/register/flags dispatcher disguised as AOT output.
- Inspect for runtime PM4/GCN processors; replace their callers with native render operations.
- Verify no x86 executable/module loading, executable blobs, guest JIT, or translated helper process.
- Build a meaningful recovered subsystem fixture with fixed inputs and independently checked outputs.
Pass: ordinary native game-core functions run with recorded semantic coverage and no prohibited runtime.
Partial: a native harness or isolated recovered function works but the startup chain remains incomplete.
Fail: boot still jumps to original x86 entry, a guest-state runtime remains, or required behavior is stubbed.
Evidence: code map, disassembly/ABI audit, native build logs, coverage manifest, fixture outputs.
Compiler targeting alone is insufficient; consult Clang [B01] and Apple ABI/ordering guidance [B02/B03].

## G2 — native services, audio fixtures, and save fixtures

Entry: G1 establishes the native execution boundary; fixtures may be developed independently earlier.
- Exercise required file/time/job/memory/input services with explicit success and failure semantics.
- Verify asynchronous callbacks, cancellation, shutdown, and no main-thread service deadlock.
- Identify actual audio codec/container and verify framing, rate, channels, loops, seeking, and duration.
- Query platform decode support; unsupported formats require a verified native decoder [B04].
- Decode representative real assets to checked PCM; compare independent expected results/tolerances.
- Test actual event/bank selection and overlapping voices, not only a stand-alone file player.
- Measure audio callback deadlines/underruns under loading and rendering pressure.
- Confirm no allocation, file I/O, codec setup, blocking lock, dispatch, or ordinary log in callbacks [B05].
- Test audio-device disconnect/reconnect, sample-rate changes, pause/resume, sleep/wake, and teardown.
- Serialize meaningful recovered game-state fields with schema, checksums, slot/profile, and bounds.
- Exercise first save, overwrite, repeated save, concurrent requests, and schema migration/rejection.
- Inject failure before write, during write, before replacement, and after replacement.
- Test full disk, unwritable directory, corruption/truncation, missing profile, and valid-backup recovery.
- Terminate the fixture process and reload the committed state in a new process.
Pass: native services behave correctly; real audio assets decode/play; transactional state survives restart.
Partial: fixtures pass but their connection to actual game execution is unproven.
Fail: silence fallback, random PCM, mock JSON unrelated to recovered state, or destructive failed writes.
Evidence: import coverage, codec/PCM fixtures, audio counters, save/recovery matrix and retained logs.
Use same-volume replacement [B06]; atomic writing [B07] does not alone prove power-loss durability.

## G3 — direct Metal correctness

Entry: native render operations and offline shader/asset conversion boundaries are specified.
- Verify no Vulkan/MoltenVK, runtime PM4 interpreter, or runtime GCN interpreter is loaded or required.
- Document Metal API/OS/GPU baseline, feature queries, resource binding ABI, and cache identities.
- Validate tiled texture conversion with asymmetric patterns, mips, non-power-of-two sizes, and arrays.
- Verify compressed formats, channel ordering, depth/stencil, blend/raster states, viewport, and sampling.
- Prove CPU upload -> shader read; render-target write -> sample; compute -> indirect draw; GPU -> CPU.
- Stress aliasing/free-reallocate, multiple frames in flight, failed commands, and deferred destruction.
- Verify residency declarations and producer/consumer ordering under the selected Metal API version.
- Metal 4 ignores hazardTrackingMode; argument-buffer resources need explicit declarations [B08/B09].
- Test wave64 masks/lanes 0/31/32/63, cross-half operations, divergence, and inactive/helper-lane cases.
- Check each vertex/fragment/compute lowering separately; compute group success proves no other stage.
- Unknown reachable shader instructions or unsupported stages fail conversion rather than rendering zero.
- Run API and Shader Validation with zero reported faults; retain full logs and a representative capture.
- Repeat visual correctness without instrumentation; invalid-read zero filling can hide errors [B10].
Pass: real converted resources/shaders and native render operations produce independently checked output.
Partial: synthetic scenes pass but required real-game shaders/passes remain unsupported or unverified.
Fail: dropped unknown operations, out-of-bounds access, missing residency, nondeterministic hazards.
Evidence: converter coverage, fixtures, pass dependency map, validation logs, private GPU frame capture.

## G4 — Swift-native launcher and macOS lifecycle

Entry: launcher can start/stop a meaningful native subsystem without calling an original executable.
- Verify SwiftUI/AppKit UI and native MetalKit/CAMetalLayer presentation rather than a web wrapper.
- Test asset-only selection/validation, actionable errors, progress, cancellation, and repeated sessions.
- Test Finder launch and native menu-bar Settings/Quit without a terminal environment.
- Check keyboard traversal, default/cancel actions, VoiceOver labels, and Light/Dark appearance.
- Exercise resizing, Retina scale, fullscreen, external display change, minimize/restore, and focus.
- Test controller connection/disconnection/remapping and keyboard/mouse transitions.
- Verify stop/close/quit releases session callbacks, workers, GPU resources, and audio without a hang.
Pass: native UI/lifecycle passes its matrix and starts the actual available native subsystem honestly.
Partial: a polished launcher exists but meaningful game-core start remains unimplemented.
Evidence: build identity, UI/lifecycle results, process architecture, logs and private captures.
Apple references: SwiftUI/AppKit [B11], Metal game windows [B12], Game Controller [B13].

## G5 — actual game boot, actual sound, actual save/reload

Entry: G1-G4 pass for the required path; document any explicitly remaining unrelated coverage gaps.
- Stage documented assets while excluding eboot.bin, all libSce*.sprx, Sony modules, and firmware.
- Cold-start the final native executable; record successful real engine initialization and scene loading.
- Reach the actual title/menu, start a new game, and reach a genuinely controllable playable scene.
- Record input that changes player/camera/world state; corroborate rendered output and simulation logs.
- Trigger identifiable menu, movement, environment, music/dialogue, and overlapping game sound events.
- Verify audible output corresponds to the current scene/actions and does not use a prerecorded surrogate.
- Reach a meaningful progress/checkpoint change, save, terminate the process, and launch again.
- Load the saved slot and verify the recovered progression/player/settings state against the prior state.
- Repeat a scene transition and save overwrite; record any untested gameplay/content coverage.
- Inspect loaded native images/processes and filesystem access evidence for forbidden dependencies.
Pass: all observed behavior above works through the actual native game, with reproducible evidence.
Partial: title only, gameplay without audio, audio without event correctness, or saves without relaunch.
Fail: original executable/module dependency, guest runtime, movie/harness substitute, fake success.
Evidence: continuous boot/input/audio/save/relaunch sequence, structured logs, save checks, runtime audit.
Screenshots alone cannot show architecture, sound, interaction, or persistence.

## G6 — packaging, native-only audit, stability, and performance

Entry: G5 passes; reproducible app packaging and asset import exist.
- Enumerate all shipped Mach-O binaries, frameworks, plug-ins, codecs, and launched helpers.
- Verify ARM64 availability and actual ARM64 execution; inspect all linked/loaded dependencies.
- Audit source and behavior for prohibited processors; ARM64 file headers alone do not prove absence.
- Test the packaged app from Finder with only documented assets and without offline analysis tools.
- Repeat boot/audio/save/load after a clean launch and cold cache; retain app/asset/revision identity.
- Check signing, bundle resources, relative paths, diagnostics, failure recovery, and save-location stability.
- Run representative traversal/combat/loading/save scenarios; record crashes, leaks, audio underruns.
- Profile CPU/GPU time, frame-interval p50/p95/p99, memory, stalls, loading, and shader compilation.
- Record resolution/settings/scene, display refresh, power/thermal state, and cold/warm cache conditions.
- Collect correctness with validation, then performance without validation; never compare mixed modes.
- Use HUD, Metal Debugger, and Instruments for different questions, not as substitute correctness proof.
- Compare upstream performance only on a matched scenario with hardware/context disclosed.
Pass: packaged native app passes the full matrix and performance claims match preserved measurements.
Partial: developer build passes but packaging, stability, or measurement coverage remains incomplete.
No FPS target, parity, release date, or full-content completion is certified by this planning document.

## Candidate runtime diagnostics — NOT RUN

Substitute a verified future application path and executable name; no such native app is asserted.
Configure the documented asset import first. Do not invent undocumented game command-line flags.
```sh
game_app="/absolute/path/to/future-native.app"
game_bin="$game_app/Contents/MacOS/actual-executable-name"
test -x "$game_bin"
file "$game_bin"
lipo -archs "$game_bin"
otool -L "$game_bin"
env MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1 MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1 MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1 "$game_bin"
env MTL_HUD_ENABLED=1 MTL_HUD_LOG_ENABLED=1 MTL_HUD_LOG_SHADER_ENABLED=1 "$game_bin"
xcrun xctrace list templates
```
Run each command separately; save the actual exit status and complete logs under the run directory.
Repeat architecture/dependency checks for every bundled native component, not just the main binary.
HUD/logging names and validation variables are documented by Apple [B10/B14/B15].
Use Xcode scheme Diagnostics for API/Shader Validation and capture a representative Metal workload.
Validation can instrument shaders and alter timing; do not report these runs as release performance.
If captures fail or a profiling feature is unavailable on the host, record the limitation truthfully.
No future candidate command becomes evidence until an agent executes it against a verified artifact.

## Primary references

[B01]: https://clang.llvm.org/docs/CrossCompilation.html
[B02]: https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms
[B03]: https://developer.apple.com/documentation/apple-silicon/addressing-architectural-differences-in-your-macos-code
[B04]: https://developer.apple.com/documentation/audiotoolbox/kaudioformatproperty_decodeformatids
[B05]: https://developer.apple.com/videos/play/wwdc2019/510/
[B06]: https://developer.apple.com/documentation/foundation/filemanager/replaceitemat(_:withitemat:backupitemname:options:)
[B07]: https://developer.apple.com/documentation/foundation/nsdata/writingoptions/atomic
[B08]: https://developer.apple.com/documentation/metal/resource-synchronization
[B09]: https://developer.apple.com/documentation/metal/tracking-the-resource-residency-of-argument-buffers
[B10]: https://developer.apple.com/documentation/xcode/validating-your-apps-metal-shader-usage
[B11]: https://developer.apple.com/documentation/swiftui/nsviewrepresentable
[B12]: https://developer.apple.com/documentation/metal/managing-your-game-window-for-metal-in-macos
[B13]: https://developer.apple.com/documentation/gamecontroller
[B14]: https://developer.apple.com/documentation/xcode/validating-your-apps-metal-api-usage
[B15]: https://developer.apple.com/documentation/xcode/monitoring-your-metal-apps-graphics-performance
