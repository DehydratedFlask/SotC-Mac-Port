#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace sotc {

// Host-assigned IDs only; these are never guest GPU addresses.
using RenderResourceHandle = std::uint32_t;
inline constexpr RenderResourceHandle kInvalidRenderResource = 0;

enum class RenderStage : std::uint8_t { vertex, fragment };
enum class DepthCompare : std::uint8_t { never, less, less_equal, greater, always };
enum class UnsupportedRenderOpKind : std::uint8_t {
    set_scissor,
    set_blend_state,
    set_raster_state,
    set_stencil_state,
    bind_texture,
    bind_sampler,
    draw_index_auto,
    dispatch,
    present,
};

struct BeginPass {
    RenderResourceHandle color_target = kInvalidRenderResource;
    RenderResourceHandle depth_target = kInvalidRenderResource;
    std::array<float, 4> clear_color{0.0F, 0.0F, 0.0F, 1.0F};
    float clear_depth = 1.0F;
};

struct EndPass {};

struct SetViewport {
    float x = 0.0F;
    float y = 0.0F;
    float width = 0.0F;
    float height = 0.0F;
    float min_depth = 0.0F;
    float max_depth = 1.0F;
};

struct SetDepthState {
    bool enabled = true;
    bool write_enabled = true;
    DepthCompare compare = DepthCompare::less;
};

struct SetPipeline {
    RenderResourceHandle pipeline = kInvalidRenderResource;
};

struct BindVertexStream {
    RenderResourceHandle buffer = kInvalidRenderResource;
    std::uint32_t slot = 0;
    std::size_t offset = 0;
    std::uint32_t stride = 0;
};

struct BindConstants {
    RenderStage stage = RenderStage::vertex;
    RenderResourceHandle buffer = kInvalidRenderResource;
    std::uint32_t slot = 0;
    std::size_t offset = 0;
    std::size_t size = 0;
};

struct BindIndexStream {
    RenderResourceHandle buffer = kInvalidRenderResource;
    std::size_t offset = 0;
};

struct DrawIndexed {
    std::uint32_t index_count = 0;
    std::size_t first_index = 0;
};

struct UnsupportedRenderOp {
    UnsupportedRenderOpKind kind{};
};

using RenderOp = std::variant<BeginPass, EndPass, SetViewport, SetDepthState, SetPipeline,
                              BindVertexStream, BindConstants, BindIndexStream, DrawIndexed,
                              UnsupportedRenderOp>;

struct RenderOpValidation {
    bool ok = false;
    std::size_t operation_index = 0;
    std::string error;
};

[[nodiscard]] const char* render_op_name(UnsupportedRenderOpKind kind) noexcept;
[[nodiscard]] RenderOpValidation validate_render_ops(std::span<const RenderOp> operations);

}  // namespace sotc
