// Direct-Metal offscreen RGBA capture + PNG provider (GPU-01-METAL-CAPTURE-PROVIDER-01).
//
// What this is: infrastructure evidence for the native Metal path. It renders one deterministic
// full-screen pattern into an offscreen `MTLTexture`, reads the render target back as tightly
// packed RGBA8, and PNG-encodes it with ImageIO. No window, no AppKit, no GUI session, and no
// MoltenVK/Vulkan/OpenGL: `MTLCreateSystemDefaultDevice` and `newLibraryWithSource` only.
//
// What this is NOT: game rendering. The pixel source is an embedded MSL pattern, not a lifted
// game frame. There is no scene, asset, colossus or game state here; G3 stays NOT RUN.
//
// Pattern contract (asserted independently by `tests/test_metal_capture.cpp`):
//   for every pixel `(x, y)` of a `width x height` target and frame number `f`,
//     rgba8 = (x & 0xff, y & 0xff, 64 + (f & 0x3f), 255)
//   so the four fixed corners are (0,0), (255,0), (0,255), (255,255) in the red/green channels at
//   every frame, alpha is always 255, and only the blue channel tracks `frame_num` (frames 1 and 2
//   differ by +1 in blue on every pixel and nowhere else).
//
// The render target is `MTLPixelFormatBGRA8Unorm`; the provider converts the BGRA readback into
// RGBA order, so `rgba8` is always RGBA (alpha last) and `stride == width * 4`.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace sotc {

struct MetalCaptureConfig {
    std::uint32_t width = 640;
    std::uint32_t height = 360;
};

struct MetalCaptureResult {
    bool ok = false;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;  // always `width * 4`; `rgba8` is tightly packed
    std::uint32_t frame_num = 0;
    std::vector<std::uint8_t> rgba8;  // width * height * 4 bytes, RGBA order, alpha last
    std::vector<std::uint8_t> png;    // valid PNG: 8-bit RGBA (IHDR colour type 6)
    std::string error;
};

// Exact error text used when the host exposes no Metal device. The fixture treats only this case
// as SKIP (return code 77); a shader/pipeline/render/readback/PNG failure must fail.
inline constexpr char kMetalCaptureNoDeviceError[] = "no Metal device available in this environment";

class MetalCapture {
public:
    MetalCapture(const MetalCapture&) = delete;
    MetalCapture& operator=(const MetalCapture&) = delete;
    virtual ~MetalCapture();

    // Builds the device, command queue, MSL library and render pipeline for `config`. Returns null
    // and fills `error` when any of those fail (including "invalid capture size"); `error` is
    // cleared on success.
    [[nodiscard]] static std::unique_ptr<MetalCapture> create(const MetalCaptureConfig& config,
                                                              std::string& error);

    // Renders exactly one offscreen frame and returns the RGBA8 readback plus PNG encoding.
    // `ok == false` with a non-empty `error` on any render/readback/encode failure.
    [[nodiscard]] virtual MetalCaptureResult capture(std::uint32_t frame_num) = 0;

protected:
    MetalCapture() = default;
};

}  // namespace sotc
