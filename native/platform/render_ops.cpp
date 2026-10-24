#include "render_ops.hpp"

#include <cmath>
#include <limits>
#include <utility>

namespace sotc {
namespace {

[[nodiscard]] RenderOpValidation failure(std::size_t operation_index, std::string message) {
    return RenderOpValidation{false, operation_index, std::move(message)};
}

[[nodiscard]] bool finite_color(const std::array<float, 4>& color) noexcept {
    for (float channel : color) {
        if (!std::isfinite(channel)) return false;
    }
    return true;
}

}  // namespace

const char* render_op_name(UnsupportedRenderOpKind kind) noexcept {
    switch (kind) {
        case UnsupportedRenderOpKind::set_scissor: return "SetScissor";
        case UnsupportedRenderOpKind::set_blend_state: return "SetBlendState";
        case UnsupportedRenderOpKind::set_raster_state: return "SetRasterState";
        case UnsupportedRenderOpKind::set_stencil_state: return "SetStencilState";
        case UnsupportedRenderOpKind::bind_texture: return "BindTexture";
        case UnsupportedRenderOpKind::bind_sampler: return "BindSampler";
        case UnsupportedRenderOpKind::draw_index_auto: return "DrawIndexAuto";
        case UnsupportedRenderOpKind::dispatch: return "Dispatch";
        case UnsupportedRenderOpKind::present: return "Present";
    }
    return "UnknownUnsupportedRenderOp";
}

RenderOpValidation validate_render_ops(std::span<const RenderOp> operations) {
    if (operations.empty()) return failure(0, "render operation list is empty");

    bool in_pass = false;
    bool has_viewport = false;
    bool has_depth_state = false;
    bool has_pipeline = false;
    bool has_vertex_stream = false;
    bool has_fragment_constants = false;
    bool has_index_stream = false;

    for (std::size_t index = 0; index < operations.size(); ++index) {
        const RenderOp& operation = operations[index];
        if (const auto* unsupported = std::get_if<UnsupportedRenderOp>(&operation)) {
            return failure(index, std::string(render_op_name(unsupported->kind)) +
                                      " is not implemented by this backend");
        }

        if (const auto* begin = std::get_if<BeginPass>(&operation)) {
            if (in_pass) return failure(index, "nested BeginPass is unsupported");
            if (begin->color_target == kInvalidRenderResource ||
                begin->depth_target == kInvalidRenderResource) {
                return failure(index, "BeginPass requires nonzero color and depth handles");
            }
            if (begin->color_target == begin->depth_target) {
                return failure(index, "color and depth targets must use distinct handles");
            }
            if (!finite_color(begin->clear_color) || !std::isfinite(begin->clear_depth) ||
                begin->clear_depth < 0.0F || begin->clear_depth > 1.0F) {
                return failure(index, "BeginPass clear values are invalid");
            }
            in_pass = true;
            has_viewport = false;
            has_depth_state = false;
            has_pipeline = false;
            has_vertex_stream = false;
            has_fragment_constants = false;
            has_index_stream = false;
            continue;
        }

        if (std::holds_alternative<EndPass>(operation)) {
            if (!in_pass) return failure(index, "EndPass has no matching BeginPass");
            in_pass = false;
            continue;
        }

        if (!in_pass) return failure(index, "render operation occurs outside a pass");

        if (const auto* viewport = std::get_if<SetViewport>(&operation)) {
            if (!std::isfinite(viewport->x) || !std::isfinite(viewport->y) ||
                !std::isfinite(viewport->width) || !std::isfinite(viewport->height) ||
                !std::isfinite(viewport->min_depth) || !std::isfinite(viewport->max_depth) ||
                viewport->width <= 0.0F || viewport->height <= 0.0F ||
                viewport->min_depth < 0.0F || viewport->max_depth > 1.0F ||
                viewport->min_depth > viewport->max_depth) {
                return failure(index, "SetViewport values are invalid");
            }
            has_viewport = true;
            continue;
        }

        if (const auto* depth = std::get_if<SetDepthState>(&operation)) {
            if (static_cast<std::uint8_t>(depth->compare) >
                static_cast<std::uint8_t>(DepthCompare::always)) {
                return failure(index, "SetDepthState compare function is invalid");
            }
            has_depth_state = true;
            continue;
        }

        if (const auto* pipeline = std::get_if<SetPipeline>(&operation)) {
            if (pipeline->pipeline == kInvalidRenderResource) {
                return failure(index, "SetPipeline requires a nonzero host handle");
            }
            has_pipeline = true;
            continue;
        }

        if (const auto* vertex = std::get_if<BindVertexStream>(&operation)) {
            if (vertex->buffer == kInvalidRenderResource || vertex->stride == 0 ||
                vertex->offset > std::numeric_limits<std::size_t>::max() - vertex->stride) {
                return failure(index, "BindVertexStream buffer, offset, or stride is invalid");
            }
            has_vertex_stream = true;
            continue;
        }

        if (const auto* constants = std::get_if<BindConstants>(&operation)) {
            if (constants->buffer == kInvalidRenderResource || constants->size == 0 ||
                constants->offset > std::numeric_limits<std::size_t>::max() - constants->size) {
                return failure(index, "BindConstants buffer, offset, or size is invalid");
            }
            if (static_cast<std::uint8_t>(constants->stage) >
                static_cast<std::uint8_t>(RenderStage::fragment)) {
                return failure(index, "BindConstants stage is invalid");
            }
            if (constants->stage == RenderStage::fragment) has_fragment_constants = true;
            continue;
        }

        if (const auto* indices = std::get_if<BindIndexStream>(&operation)) {
            if (indices->buffer == kInvalidRenderResource || indices->offset % 2 != 0) {
                return failure(index, "BindIndexStream requires a nonzero U16-aligned buffer span");
            }
            has_index_stream = true;
            continue;
        }

        if (const auto* draw = std::get_if<DrawIndexed>(&operation)) {
            const std::size_t max_first_index =
                std::numeric_limits<std::size_t>::max() / sizeof(std::uint16_t);
            if (draw->index_count == 0 || draw->index_count % 3 != 0 ||
                draw->first_index > max_first_index ||
                static_cast<std::size_t>(draw->index_count) > max_first_index - draw->first_index) {
                return failure(index, "DrawIndexed count or first index is invalid");
            }
            if (!has_viewport || !has_depth_state || !has_pipeline || !has_vertex_stream ||
                !has_fragment_constants || !has_index_stream) {
                return failure(index, "DrawIndexed is missing viewport, depth, pipeline, or buffer state");
            }
            continue;
        }

        return failure(index, "unknown render operation");
    }

    if (in_pass) return failure(operations.size(), "render operation list has an unclosed pass");
    return RenderOpValidation{true, operations.size(), {}};
}

}  // namespace sotc
