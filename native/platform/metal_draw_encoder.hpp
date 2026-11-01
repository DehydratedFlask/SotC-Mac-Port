// Direct-Metal encoder for the typed draw list produced by `pm4::traverse`.
//
// What this is: the seam between the PS4 command processor (whoever supplies the opcode policy) and
// Metal. It takes `pm4::DrawCommand` values, starts one offscreen render pass, and issues one
// **indexed** draw per typed draw through an `MTLRenderCommandEncoder`, then reads the target back.
//
// What this is NOT: game rendering. The geometry is synthetic (a horizontal band per draw) and the
// colour is derived from the draw kind and its count, so the fixture can prove that the encoder
// issued exactly the requested draws in order. There is no scene, asset or GNM state here, and G3
// stays NOT RUN.
//
// No Vulkan, no MoltenVK, no OpenGL: `MTLCreateSystemDefaultDevice`, `newLibraryWithSource`,
// `drawIndexedPrimitives` and `getBytes` only.
//
// Fixture contract (asserted by `tests/test_metal_draw_encoder.cpp`):
//   n draws are laid out as n equal-height bands, top to bottom, in list order.
//   band colour = kind colour scaled by (count == 0 ? 0.25 : 1.0)
//     index_auto        (1.0, 0.0, 0.0)
//     index             (0.0, 1.0, 0.0)
//     index_offset      (0.0, 0.0, 1.0)
//     index_indirect    (1.0, 1.0, 0.0)
//     dispatch          (1.0, 0.0, 1.0)
//     dispatch_indirect (0.0, 1.0, 1.0)
//   outside every band the clear colour is black.
#pragma once

#include "pm4_indirect.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace sotc {

struct MetalDrawEncoderConfig {
    std::uint32_t width = 256;
    std::uint32_t height = 128;
};

struct MetalDrawEncodeResult {
    bool ok = false;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;  // always `width * 4`
    std::uint32_t draws_requested = 0;
    std::uint32_t draws_encoded = 0;
    std::uint32_t draws_skipped = 0;  // DrawKind::none
    std::vector<std::uint8_t> rgba8;  // tightly packed RGBA, alpha last
    std::string error;
};

// Exact error text when the host exposes no Metal device; fixtures treat only this as SKIP.
inline constexpr char kMetalDrawEncoderNoDeviceError[] =
    "no Metal device available in this environment";

[[nodiscard]] MetalDrawEncodeResult metal_encode_draw_list(const pm4::DrawCommand* draws,
                                                           std::size_t count,
                                                           MetalDrawEncoderConfig config = {});

// FNV-1a over the RGBA readback, for cheap equality checks in fixtures.
[[nodiscard]] std::uint64_t metal_draw_digest(const std::vector<std::uint8_t>& rgba8) noexcept;

}  // namespace sotc
