#pragma once

#include "cmsh_mesh.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

using ShaderFloat4 = std::array<float, 4>;

// Import-time test ABI for tools/dev/gcn_to_msl.py's current default pair. Field order and stride
// match the generated MSL ShaderCase exactly; this is not the retail vertex-fetch ABI.
struct alignas(16) DefaultShaderCase {
    ShaderFloat4 position{};
    ShaderFloat4 encoded_normal{};
    ShaderFloat4 object_row0{};
    ShaderFloat4 object_row1{};
    ShaderFloat4 object_row2{};
    ShaderFloat4 clip_row0{};
    ShaderFloat4 clip_row1{};
    ShaderFloat4 clip_row2{};
    ShaderFloat4 clip_row3{};
    ShaderFloat4 camera{};
    ShaderFloat4 material{};
    ShaderFloat4 param0_p0{};
    ShaderFloat4 param0_p10{};
    ShaderFloat4 param0_p20{};
    ShaderFloat4 param1_p0{};
    ShaderFloat4 param1_p10{};
    ShaderFloat4 param1_p20{};
    ShaderFloat4 barycentric{};
};

static_assert(sizeof(DefaultShaderCase) == 288);
static_assert(alignof(DefaultShaderCase) == 16);
static_assert(offsetof(DefaultShaderCase, encoded_normal) == 16);
static_assert(offsetof(DefaultShaderCase, camera) == 144);
static_assert(offsetof(DefaultShaderCase, barycentric) == 272);

[[nodiscard]] DefaultShaderCase make_default_shader_case() noexcept;

enum class AssetTextureFilter : std::uint8_t { nearest, linear };
enum class AssetTextureAddressMode : std::uint8_t { clamp_to_edge, repeat, mirror_repeat };
enum class AssetTextureMipFilter : std::uint8_t { not_mipmapped, nearest, linear };

// One explicitly supplied RGBA8Unorm 2D mip level after the base level. Dimensions must follow
// the ordinary halving chain from AssetTexture2D's base level.
struct AssetTextureMipLevel {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::span<const std::uint8_t> rgba8;
};

// Explicit import-time harness texture input: one RGBA8Unorm 2D base level and optional supplied
// mips. It does not describe a retail texture format or recovered game binding.
struct AssetTexture2D {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::span<const std::uint8_t> rgba8;
    // Optional levels 1..N; empty preserves the existing one-level aggregate initializer behavior.
    std::span<const AssetTextureMipLevel> mip_levels{};
};

// Explicit harness sampler settings. Defaults preserve one-level sampling; when mip filtering is
// enabled, the broad default LOD range leaves implicit fragment LOD selection available.
struct AssetSamplerInput {
    AssetTextureFilter min_filter = AssetTextureFilter::nearest;
    AssetTextureFilter mag_filter = AssetTextureFilter::nearest;
    AssetTextureAddressMode address_u = AssetTextureAddressMode::clamp_to_edge;
    AssetTextureAddressMode address_v = AssetTextureAddressMode::clamp_to_edge;
    AssetTextureMipFilter mip_filter = AssetTextureMipFilter::not_mipmapped;
    float lod_min_clamp = 0.0F;
    float lod_max_clamp = std::numeric_limits<float>::max();
};

struct AssetMeshDraw {
    std::span<const CmshPosition> positions;
    std::span<const std::uint16_t> indices;
    DefaultShaderCase shader;
    std::optional<AssetTexture2D> fragment_texture0 = std::nullopt;
    std::optional<AssetSamplerInput> fragment_sampler0 = std::nullopt;
};

struct MetalAssetFrameConfig {
    std::uint32_t width = 256;
    std::uint32_t height = 256;
    // Declares the caller-supplied MSL resource ABI. This fixture currently supports one paired
    // fragment texture/sampler binding at slot 0 when enabled.
    bool fragment_texture_sampler_slot0 = false;
};

struct MetalAssetFrameResult {
    bool ok = false;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
    std::vector<std::uint8_t> rgba8;
    std::vector<std::uint8_t> png;
    std::string error;
};

class MetalAssetFrameRenderer {
public:
    MetalAssetFrameRenderer(const MetalAssetFrameRenderer&) = delete;
    MetalAssetFrameRenderer& operator=(const MetalAssetFrameRenderer&) = delete;
    virtual ~MetalAssetFrameRenderer();

    // Compiles only the supplied ordinary MSL source; generated code stays outside the repository.
    [[nodiscard]] static std::unique_ptr<MetalAssetFrameRenderer> create(
        const MetalAssetFrameConfig& config, std::string_view msl_source, std::string& error);

    // Executes one offscreen typed-op list and returns RGBA8 readback plus ImageIO PNG bytes.
    // `shader` provides both the per-vertex harness template and fragment constants for this draw.
    [[nodiscard]] virtual MetalAssetFrameResult render(std::span<const AssetMeshDraw> draws) = 0;

protected:
    MetalAssetFrameRenderer() = default;
};

}  // namespace sotc
