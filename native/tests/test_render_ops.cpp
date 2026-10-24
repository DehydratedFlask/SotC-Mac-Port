#include "../platform/render_ops.hpp"

#include <cstdio>
#include <span>
#include <string_view>
#include <vector>

namespace {

using sotc::BeginPass;
using sotc::BindConstants;
using sotc::BindIndexStream;
using sotc::BindVertexStream;
using sotc::DrawIndexed;
using sotc::EndPass;
using sotc::RenderOp;
using sotc::SetDepthState;
using sotc::SetPipeline;
using sotc::SetViewport;
using sotc::UnsupportedRenderOp;
using sotc::UnsupportedRenderOpKind;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

[[nodiscard]] std::vector<RenderOp> valid_ops() {
    return {BeginPass{1, 2},
            SetViewport{0.0F, 0.0F, 64.0F, 64.0F, 0.0F, 1.0F},
            SetDepthState{},
            SetPipeline{3},
            BindVertexStream{4, 0, 0, 288},
            BindConstants{sotc::RenderStage::fragment, 4, 0, 0, 288},
            BindIndexStream{5, 0},
            DrawIndexed{6, 0},
            EndPass{}};
}

void check_error(std::string_view name, const std::vector<RenderOp>& operations,
                 std::string_view expected_fragment) {
    const auto result = sotc::validate_render_ops(operations);
    check(name, !result.ok && result.operation_index < operations.size() + 1 &&
                    result.error.find(expected_fragment) != std::string::npos);
}

}  // namespace

int main() {
    const std::vector<RenderOp> accepted = valid_ops();
    const auto result = sotc::validate_render_ops(accepted);
    check("valid indexed depth pass accepted", result.ok && result.error.empty());

    std::vector<RenderOp> missing_depth = valid_ops();
    missing_depth.erase(missing_depth.begin() + 2);
    check_error("draw requires depth state", missing_depth, "depth");

    std::vector<RenderOp> bad_viewport = valid_ops();
    bad_viewport[1] = SetViewport{0.0F, 0.0F, 0.0F, 64.0F, 0.0F, 1.0F};
    check_error("zero viewport width rejected", bad_viewport, "SetViewport");

    std::vector<RenderOp> bad_index_alignment = valid_ops();
    bad_index_alignment[6] = BindIndexStream{5, 1};
    check_error("U16 index offset alignment enforced", bad_index_alignment, "U16-aligned");

    std::vector<RenderOp> bad_triangle_count = valid_ops();
    bad_triangle_count[7] = DrawIndexed{4, 0};
    check_error("triangle count alignment enforced", bad_triangle_count, "DrawIndexed count");

    std::vector<RenderOp> nested_pass = valid_ops();
    nested_pass.insert(nested_pass.begin() + 1, BeginPass{1, 2});
    check_error("nested pass rejected", nested_pass, "nested BeginPass");

    std::vector<RenderOp> outside_pass = {SetViewport{0.0F, 0.0F, 64.0F, 64.0F, 0.0F, 1.0F}};
    check_error("state change outside pass rejected", outside_pass, "outside a pass");

    std::vector<RenderOp> unsupported = {BeginPass{1, 2},
                                         UnsupportedRenderOp{UnsupportedRenderOpKind::dispatch},
                                         EndPass{}};
    check_error("unsupported dispatch fails visibly", unsupported, "Dispatch is not implemented");

    std::vector<RenderOp> unclosed = {BeginPass{1, 2}};
    check_error("unclosed pass rejected", unclosed, "unclosed pass");

    if (failures != 0) {
        std::fprintf(stderr, "render_ops fixture checks: FAILED (%d)\n", failures);
        return 1;
    }
    std::puts("render_ops fixture checks: PASSED");
    return 0;
}
