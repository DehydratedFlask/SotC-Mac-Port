# Checkpoint — direct-Metal offscreen capture provider

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; orchestrator verification
- Task: `GPU-01-METAL-CAPTURE-PROVIDER-01`
- Gate: **FIXTURE_VERIFIED** for the Metal path; **G3 NOT RUN** (no game frame)

## What exists

`native/platform/metal_capture.{hpp,mm}` implements a direct-Metal offscreen provider: default
device -> command queue -> MSL library compiled from an embedded source string -> render pipeline
-> `BGRA8Unorm` render target -> full-screen triangle strip -> `getBytes` readback -> BGRA->RGBA
repack (tight stride) -> PNG through ImageIO. No window, no AppKit, no GUI session, and no
MoltenVK/Vulkan/OpenGL.

Pattern contract: `rgba8 = (x & 0xff, y & 0xff, 64 + (frame & 63), 255)` for every pixel, so alpha
is always 255 and only the blue channel tracks `frame_num`.

## Measured evidence (Apple M4 Pro, macOS 27.0.1)

- 640x360, `rgba8 = 921,600 B` (stride 2560), PNG 5,258 B, sha256
  `e6a64e939f5a400764fcf3d24e47ca320bb4fe6bcec8201297a13981badc5cab` (identical across two
  processes). `file`: 640x360, 8-bit/color RGBA. IHDR 640/360/8/6; IEND at the exact end.
- Frame 1 samples: `(0,0)=000041ff`, `(17,5)=110541ff`, `(255,0)=ff0041ff`, `(0,255)=00ff41ff`,
  `(320,180)=40b441ff`, `(639,359)=7f6741ff`.
- Frame 2 differs on exactly 230,400 bytes, all blue, +1 only; repeating frame 1 is byte-identical.
  A second size (7x5, stride 28) reads back correctly.
- `MTL_DEBUG_LAYER=1` run is clean (96 checks, no validation errors).

## Wiring

`test_metal_capture` builds `ScreenshotHooks{renderer_present=true, capture_frame=...}` and drives
the existing `attach_screenshot`: the attachment is `attached=true`, `synthetic_command=false`,
name `bug_screenshot.png`, and `frame_num` passes through. The GPU encoder remained a hook until
now; the provider supplies a native PNG for this synthetic frame.

## Tests and limits

- `test_metal_capture`: 96 checks (98 with the opt-in dump env). No device -> explicit
  `SKIP: no Metal device available in this environment`, exit 77, registered as
  `SKIP_RETURN_CODE`; any render/readback/PNG failure fails.
- Mutations: PNG colour type 6->2 and the pixel blue base 64->65 both fail; restored green.
- **Limits:** the provider renders its own MSL pattern, not a game frame. There is no scene, asset,
  colossus, depth, MSAA or CAMetalLayer, and `frame_num` is caller-supplied (`engine+0x70`'s writer
  is still unknown). **G3 remains NOT RUN.**
