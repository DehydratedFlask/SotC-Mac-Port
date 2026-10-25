#include "../platform/metal_asset_frame.hpp"
#include "../platform/metal_capture.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint32_t kWidth = 64;
constexpr std::uint32_t kHeight = 64;

constexpr std::string_view kTextureSampleMsl = R"metal(
#include <metal_stdlib>
using namespace metal;

struct ShaderCase {
    float4 position;
    float4 padding0[9];
    float4 material;
    float4 padding1[3];
    float4 param1_p0;
    float4 padding2[3];
};

struct RasterValue {
    float4 position [[position]];
    float2 uv;
};

vertex RasterValue asset_vertex(device const ShaderCase* inputs [[buffer(0)]],
                                uint vertex_id [[vertex_id]]) {
    RasterValue out;
    const ShaderCase value = inputs[vertex_id];
    out.position = float4(value.position.xy, value.position.z, 1.0f);
    out.uv = value.param1_p0.xy;
    return out;
}

fragment float4 asset_fragment(RasterValue in [[stage_in]],
                               constant ShaderCase& uniforms [[buffer(0)]],
                               texture2d<float> source [[texture(0)]],
                               sampler source_sampler [[sampler(0)]]) {
    return source.sample(source_sampler, in.uv) * uniforms.material;
}
)metal";

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

[[nodiscard]] std::size_t pixel_offset(std::uint32_t x, std::uint32_t y) {
    return (static_cast<std::size_t>(y) * kWidth + x) * 4;
}

[[nodiscard]] bool is_color(const std::vector<std::uint8_t>& rgba, std::uint32_t x,
                            std::uint32_t y, std::array<std::uint8_t, 4> expected) {
    if (x >= kWidth || y >= kHeight || rgba.size() != kWidth * kHeight * 4) return false;
    const std::size_t offset = pixel_offset(x, y);
    for (std::size_t channel = 0; channel < expected.size(); ++channel) {
        if (rgba[offset + channel] != expected[channel]) return false;
    }
    return true;
}

}  // namespace

int main() {
    static_assert(sizeof(sotc::DefaultShaderCase) == 288);
    static_assert(offsetof(sotc::DefaultShaderCase, param1_p0) == 224);

    const std::array<sotc::CmshPosition, 4> positions = {{{-0.9F, -0.9F, 0.25F},
                                                           {0.9F, -0.9F, 0.25F},
                                                           {-0.9F, 0.9F, 0.25F},
                                                           {0.9F, 0.9F, 0.25F}}};
    const std::array<std::uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};
    const std::array<std::uint8_t, 8> pixels = {20, 40, 60, 255, 80, 100, 120, 255};
    const std::array<std::uint8_t, 8> changed_pixels = {20, 40, 60, 255, 92, 100, 120, 255};

    sotc::DefaultShaderCase shader = sotc::make_default_shader_case();
    shader.param1_p0 = {0.5F, 0.5F, 0.0F, 0.0F};
    shader.material = {1.0F, 1.0F, 1.0F, 1.0F};
    sotc::AssetMeshDraw draw{positions, indices, shader};
    draw.fragment_texture0 = sotc::AssetTexture2D{2, 1, pixels};
    draw.fragment_sampler0 = sotc::AssetSamplerInput{
        sotc::AssetTextureFilter::linear, sotc::AssetTextureFilter::linear,
        sotc::AssetTextureAddressMode::clamp_to_edge,
        sotc::AssetTextureAddressMode::clamp_to_edge};

    sotc::MetalAssetFrameConfig config{kWidth, kHeight, true};
    std::string error;
    std::unique_ptr<sotc::MetalAssetFrameRenderer> renderer =
        sotc::MetalAssetFrameRenderer::create(config, kTextureSampleMsl, error);
    if (renderer == nullptr) {
        if (error == sotc::kMetalCaptureNoDeviceError) {
            std::printf("SKIP: %s\n", error.c_str());
            return 77;
        }
        std::fprintf(stderr, "FAIL texture MSL pipeline creation: %s\n", error.c_str());
        return 1;
    }

    sotc::MetalAssetFrameResult baseline = renderer->render(std::span(&draw, 1));
    check("synthetic fragment texture/sampler draw renders", baseline.ok);
    if (!baseline.ok) {
        std::fprintf(stderr, "FAIL render: %s\n", baseline.error.c_str());
        return 1;
    }
    check("linear 2x1 RGBA8 sample returns the expected midpoint",
          is_color(baseline.rgba8, 32, 32, {50, 70, 90, 255}));

    sotc::MetalAssetFrameResult repeated = renderer->render(std::span(&draw, 1));
    check("repeated texture sample renders", repeated.ok);
    if (repeated.ok) {
        check("repeated texture sample is byte deterministic",
              repeated.rgba8 == baseline.rgba8 && repeated.png == baseline.png);
    }

    sotc::AssetMeshDraw changed_draw = draw;
    changed_draw.fragment_texture0 = sotc::AssetTexture2D{2, 1, changed_pixels};
    sotc::MetalAssetFrameResult changed = renderer->render(std::span(&changed_draw, 1));
    check("texture input mutation renders", changed.ok);
    if (changed.ok) {
        check("texture input mutation changes the sampled color",
              is_color(changed.rgba8, 32, 32, {56, 70, 90, 255}));
        check("texture input mutation changes PNG bytes", changed.png != baseline.png);
    }

    sotc::AssetMeshDraw missing_binding{positions, indices, shader};
    sotc::MetalAssetFrameResult missing = renderer->render(std::span(&missing_binding, 1));
    check("required texture/sampler omission fails visibly",
          !missing.ok && missing.error.find("explicit slot-0 ABI") != std::string::npos);

    const std::array<std::uint8_t, 7> truncated_pixels = {20, 40, 60, 255, 80, 100, 120};
    sotc::AssetMeshDraw truncated_draw = draw;
    truncated_draw.fragment_texture0 = sotc::AssetTexture2D{2, 1, truncated_pixels};
    sotc::MetalAssetFrameResult truncated = renderer->render(std::span(&truncated_draw, 1));
    check("truncated RGBA8 input fails visibly",
          !truncated.ok && truncated.error.find("exact nonempty RGBA8") != std::string::npos);

    sotc::AssetMeshDraw invalid_sampler_draw = draw;
    invalid_sampler_draw.fragment_sampler0->min_filter =
        static_cast<sotc::AssetTextureFilter>(255);
    sotc::MetalAssetFrameResult invalid_sampler = renderer->render(std::span(&invalid_sampler_draw, 1));
    check("unknown sampler filter fails visibly",
          !invalid_sampler.ok &&
              invalid_sampler.error.find("unsupported filter or address mode") != std::string::npos);

    if (failures != 0) {
        std::fprintf(stderr, "metal_texture_binding checks: FAILED (%d)\n", failures);
        return 1;
    }
    std::printf("metal_texture_binding: synthetic RGBA8 sample produced %zu PNG bytes\n",
                baseline.png.size());
    std::puts("metal_texture_binding checks: PASSED (synthetic texture; no game asset claim)");
    return 0;
}
