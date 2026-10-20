# Native Apple Silicon platform contract

Status: DESIGN ONLY. No game implementation, boot, audio, saving, or performance is certified here.
Baseline: 2026-10-19, America/Chicago; coordinating agent recorded M4 Pro, 12 CPU cores,
48 GB unified memory, macOS 27.0.1, Xcode 27.1. Recheck before using this as a test baseline.
Read [AGENTS.md](../AGENTS.md), [ROADMAP.md](../ROADMAP.md), [HANDOFF.md](../HANDOFF.md),
[validation gates](VALIDATION.md), and [task registry](TASKS.md) before implementation.

## 1. Required product boundary

- Ship an ARM64 Mach-O game core and a SwiftUI/AppKit macOS launcher.
- Submit rendering directly through Metal. MetalKit/CAMetalLayer is the presentation boundary.
- Boot the actual remake, play its actual sound, and save/reload meaningful progress.
- The final app must not open or load eboot.bin, libSce*.sprx, other Sony modules, or firmware.
- No Rosetta, Wine, CrossOver, QEMU, Unicorn, guest CPU interpreter/JIT, guest-register dispatcher,
  x86 executable loading, Vulkan/MoltenVK runtime, or PS4 execution environment.
- No runtime PM4 packet processor or GCN instruction processor, even if compiled to ARM64.
- Static lifting is acceptable only when it produces ordinary native functions and data.
- A guest-state machine retained beneath an AOT label fails this contract.
- Original executable sections must not be embedded or relabeled as asset blobs.
- Local offline executable analysis is authorized; no additional game upload is required.
- Original assets remain read-only. The documented runtime import must accept asset data only.
- Tool presence, a native launcher, a triangle, a test tone, or a mock save proves no game milestone.

## 2. Architecture and conversion boundary

1. Private source evidence: supplied PKGs, extracted files, hashes, REA/Ghidra observations.
2. Offline tools: container extraction, code/data analysis, semantic recovery, shader conversion,
   texture conversion, and generation of provenance-bearing native asset manifests.
3. Build inputs: reviewed native source/functions, typed data, Metal libraries, and asset metadata.
4. Runtime: native game logic, native platform services, native renderer, native audio, native saves.
5. User interface: SwiftUI/AppKit lifecycle and diagnostics surrounding the real native game session.

The native core must call typed render operations; it must not regenerate console command streams
and then execute those streams in a runtime compatibility processor.
Where original logic emits packets dynamically, recover the intent and replace those call sites.
An offline frame converter is a research fixture, not a substitute for interactive game logic.
Keep host-native resource identifiers distinct from original addresses and analysis labels.
Metal's normal GPU compilation of Metal IR is platform shader compilation, not a guest CPU runtime.
Apple describes the Metal IR/library lifecycle in [A01].
No offline converter, Ghidra installation, extracted executable, or Sony module may be required
after the final app and its documented asset import have been prepared.

## 3. Game-core recovery and CPU contract

Clang target selection presupposes valid source/IR and target-compatible libraries [A02].
Neither changing a target triple nor exporting decompiler pseudocode proves correct recovery.
Recover startup registrations, global initialization, asset initialization, the update loop,
scene transitions, animation, physics, input processing, rendering calls, sound events, and saves.
Track indirect calls and function-pointer tables; unresolved reachable targets block the gate.
Recover explicit types, ownership, lifetimes, and failure behavior before replacing an import.
Preserve source evidence addresses as documentation, not callable runtime function pointers.
Replace original initializer arrays with native registrations and verify initialization order.
Every required function must have an implementation and a bounded failure mode.
An unimplemented function must fail visibly; silently returning success is forbidden.
An optional unused feature may be omitted only with recorded reachability and behavioral evidence.
Use native C/C++ atomics, locks, condition variables, and scheduling with documented ownership.
Audit x86 ordering assumptions: Apple Silicon uses a weaker memory-ordering model [A03].
Audit SSE/AVX operations individually for integer, rounding, NaN, saturation, and shuffle semantics.
Use fixed-width asset fields and checked offsets instead of copying host C++ objects to disk.
Assert ABI structure sizes/offsets; keep parsed data separate from native object layouts.
Apple ARM64 calling conventions and C++ rules differ from x86 and generic ARM64 cases [A04].
Do not bridge callbacks by casting incompatible function signatures or variadic declarations.
Keep exceptions within their language/module boundary; do not unwind through Swift/C ABI callbacks.
Query page size and relevant device capabilities rather than inheriting console constants.

## 4. Native platform services

Define service interfaces by observed game requirements, not by a list of fabricated successful stubs.
- Files: immutable game assets, checked paths, bounds/short-read errors, async completion semantics.
- Time: monotonic native clocks, explicit time units, deterministic simulation tick ownership.
- Jobs: native workers, dependency completion, cancellation, thread-local storage, orderly shutdown.
- Memory: native allocation/alignment, checked arena bounds, lifetime and address-translation rules.
- Input: Game Controller plus keyboard/mouse, focus transitions, remapping, device hot-plug.
- User/settings: native profile identifiers and persisted configuration without Sony service loading.
- Audio: documented event/bank behavior feeding the native audio subsystem in section 7.
- Saving: documented game-state serialization and transactional storage in section 8.
- Diagnostics: structured native errors with task/fixture identifiers, no secrets or large game dumps.

Place a narrow versioned C or Objective-C++ bridge between the native core and Swift.
Use opaque handles and plain validated configuration values; do not expose C++ object layout to Swift.
Specify start, stop, pause/resume, configuration, diagnostics, and session-finished notification.
UI callbacks must hop to MainActor; game loading, decoding, and simulation must not block it.
Repeated start/stop must release callbacks, workers, audio nodes, resources, and session handles.
Required-service coverage belongs in the task registry and real-game trace, not only in a header.

## 5. Metal resource and synchronization contract

Choose and record one initial API baseline and supported OS/GPU floor before implementing bindings.
Metal 3 and Metal 4 have different synchronization/residency contracts; do not mix assumptions.
MTL4CommandQueue ignores hazardTrackingMode; its conflicting accesses need explicit ordering [A05].
For earlier queues, automatic tracking has limits, especially indirect/argument-buffer accesses.
Declare indirect resources with the appropriate residency/access methods before their use [A06].
Tracked useResource declarations can inform hazards; untracked resources need fences/events.
Define a resource-range map with checked offsets, alias relationships, generation, and lifetime.
Original CPU/GPU addresses are evidence identifiers, not interchangeable with Metal GPU addresses.
Shared memory does not remove producer/consumer synchronization or lifetime requirements.
Document CPU writes, uploads, draw/compute reads, render-target writes, readback, and alias reuse.
Retain buffers/textures until all consumers finish; cap frames in flight and handle command failure.
Use private GPU storage where appropriate, with explicit upload/readback paths; document choices.
Convert console tiled/packed asset layouts offline into verified Metal-compatible data.
Metal channel swizzle rearranges component values; it does not decode console memory tiling [A07].
Test every encountered texture format, block size, mip layout, array/cube layout, and depth encoding.
Query compression/format capabilities rather than assuming all Apple GPUs accept every format [A08].
Preserve render-target load/store intent; tile-based rendering exposes incorrect discard assumptions.
Audit depth/stencil, blend state, sampling, viewport/scissor, index formats, and coordinate conventions.
Track each render pass's reads/writes, residency declarations, barriers, and frame-lifetime ownership.
CPU/GPU label and completion behavior must come from native completion mechanisms, not packet polling.

## 6. Shader conversion and wave64 correctness

The offline front end must decode each encountered shader into verified stage-specific semantics.
Apple Metal Shader Converter accepts DXIL; it is not a PS4 GCN decoder or arbitrary SPIR-V input [A09].
Select an actual GCN-to-native-Metal path only after source/fixture coverage supports it.
No runtime GCN parser, instruction interpreter, or guest instruction dispatcher is permitted.
Record shader digest, stage, entry point, resource ABI, used instructions, and converter revision.
Unknown reachable instructions or unsupported stage semantics must fail conversion explicitly.
Never replace an unknown operation with zero, a skipped write, or a black output to pass a milestone.
Query SIMD width; a console wave64 is not automatically one Metal SIMD group [A10].
Prove exec-mask width, lane IDs, scalar/vector state, ballot, shuffle, reductions, and divergence.
Test lanes 0/31/32/63, cross-half values, sparse masks, inactive lanes, and divergent loop exits.
Two independent SIMD groups do not preserve cross-half wave operations without a proved lowering.
A synchronized 64-thread compute prototype is not proof for vertex or fragment shader behavior.
Vertex/fragment stages cannot generally use arbitrary cross-group compute barriers.
Document stage-specific active/helper-lane and derivative semantics; unresolved cases are blockers.
Barriers must have correct participation, scope, and memory flags; divergent barriers may deadlock.
Compare integer results exactly and define justified floating-point tolerances before running tests.
Preserve pass-to-pass position invariance when the rendering technique requires it.
Cache keys include input digest, translator version, options, resource ABI, and Metal language version.
Device-specific binary caches also identify compatible GPU/OS/toolchain conditions.
Retain generated shader source/debug information privately for Xcode captures and reproductions.

## 7. Native audio design

Recover audio bank/event metadata and identify actual codecs before selecting any decoder.
Record packet framing, configuration bytes, channel order, rate, loop points, seek state, and duration.
Do not infer codec support from an extension or assume that a Sony audio import is PCM.
Use AudioToolbox decoder capability queries and a real conversion fixture for supported formats [A11].
For unsupported formats, provide a reviewed ARM64-native decoder with provenance and license records.
Decoding and bank I/O run outside the real-time output callback.
Recommended initial pipeline: bank/event parser -> worker decoding -> bounded PCM queues/mixer
-> AVAudioSourceNode/AVAudioEngine or a documented Core Audio output unit.
AVAudioSourceNode supplies PCM and only supports specified PCM format conversions [A12].
The real-time callback must not allocate, read files, initialize codecs, dispatch work, block on locks,
or perform ordinary logging; preallocate data and measure underruns/deadlines [A13].
Mix voices, gains, panning/spatial intent, bus effects, music transitions, and game pause coherently.
Recover required NGS/audio behavior rather than replacing it with silence or an unrelated soundtrack.
Coordinate audio event timestamps with the native simulation and output clock.
Handle device/sample-rate changes, disconnect, sleep/wake, underrun recovery, and shutdown.
Verify audible game events as well as decoded PCM correctness; a tone passes neither requirement.
No libSce audio module or external firmware may be opened by the finished audio path.
Do not adopt extra real-time worker/workgroup complexity unless the measured pipeline requires it.

## 8. Native save design

Save semantic game state at a stable simulation boundary, then serialize off the real-time paths.
Use a native Application Support directory under the chosen app identifier [A14].
Keep saves separate from immutable assets, converter outputs, caches, and the application bundle.
Use explicit schema/version, fixed-width fields, length checks, checksum, and profile/slot identity.
Do not serialize host pointers, raw C++ objects, padding, or synchronization primitives.
Define complete checkpoint/progression state from recovered behavior; a file-write mock is insufficient.
Implement transactional replacement and retain a recoverable previous valid generation.
Write a complete temporary item on the destination volume; validate before replacement [A15].
Foundation atomic writes use an auxiliary file, but do not alone prove power-loss durability [A16].
If promising sudden-power-loss durability, specify and test the additional flush/recovery protocol.
Apple's archived fsync reference explains the distinction from stronger device flushing [A17].
Define first-save, overwrite, simultaneous requests, cancellation, and interruption behavior.
Reject unsupported/corrupt saves cleanly and recover a valid backup without overwriting evidence.
Persist game settings separately where their lifecycle differs from progress.
Original-console-save compatibility is separate from required native save/reload support.
Only process termination and successful restore of meaningful progress can close the real-save gate.

## 9. Swift-native launcher and lifecycle

Use SwiftUI for launcher/settings and AppKit for native window/lifecycle integration.
Embed MTKView via NSViewRepresentable, or document an equivalent native AppKit presentation [A18].
Keep one explicit game-session owner with states validating, ready, starting, running, stopping, error.
Asset selection validates the documented asset-only manifest and explains actionable missing data.
Start calls the real native core; no original executable path may be passed as a runtime entry point.
Support menu-bar Settings/Quit, keyboard navigation, VoiceOver labels, and Light/Dark appearances.
Keep controls responsive while validation/loading runs; expose progress and safe cancellation.
Use native resize/fullscreen behavior and update drawable size for backing scale/display changes [A19].
Handle minimized/occluded windows, focus loss, display hot-plug, close/reopen, and orderly termination.
Use Game Controller profiles/notifications; test controller and keyboard/mouse transitions [A20].
Do not intercept standard Mac shortcuts as gameplay input while launcher controls have focus.
Save UI preferences without conflating them with game progression.
Finder launch must work with documented assets and no shell-specific environment.
Missing implementation must show a truthful error; a launcher-only build remains a launcher-only build.

## 10. Agent checkpoints and acceptance

Follow G0-G6 in [VALIDATION.md](VALIDATION.md); initial implementation status is NOT RUN.
Separate observed evidence, inferred design, and unverified expectations in every checkpoint.
After three unsuccessful distinct hypotheses for one task, mark NEEDS_PRINCIPAL across agents.
Preserve the smallest reproducer and follow the escalation template; no automatic fourth try.
The integrator records worker checkpoints in HANDOFF.md and updates task/evidence/install records.
Every new/moved project file must be indexed before handoff; private game evidence stays private.
No final completion or FPS claim precedes real boot/audio/save and forbidden-runtime absence.

## Primary references

[A01]: https://developer.apple.com/documentation/metal/metal-libraries
[A02]: https://clang.llvm.org/docs/CrossCompilation.html
[A03]: https://developer.apple.com/documentation/apple-silicon/addressing-architectural-differences-in-your-macos-code
[A04]: https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms
[A05]: https://developer.apple.com/documentation/metal/resource-synchronization
[A06]: https://developer.apple.com/documentation/metal/tracking-the-resource-residency-of-argument-buffers
[A07]: https://developer.apple.com/documentation/metal/mtltextureswizzlechannels
[A08]: https://developer.apple.com/documentation/metal/mtldevice/supportsbctexturecompression
[A09]: https://developer.apple.com/metal/shader-converter/
[A10]: https://developer.apple.com/documentation/metal/creating-threads-and-threadgroups
[A11]: https://developer.apple.com/documentation/audiotoolbox/kaudioformatproperty_decodeformatids
[A12]: https://developer.apple.com/documentation/avfaudio/avaudiosourcenode/init(format:renderblock:)
[A13]: https://developer.apple.com/videos/play/wwdc2019/510/
[A14]: https://developer.apple.com/documentation/foundation/url/applicationsupportdirectory
[A15]: https://developer.apple.com/documentation/foundation/filemanager/replaceitemat(_:withitemat:backupitemname:options:)
[A16]: https://developer.apple.com/documentation/foundation/nsdata/writingoptions/atomic
[A17]: https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fsync.2.html
[A18]: https://developer.apple.com/documentation/swiftui/nsviewrepresentable
[A19]: https://developer.apple.com/documentation/metal/managing-your-game-window-for-metal-in-macos
[A20]: https://developer.apple.com/documentation/gamecontroller
