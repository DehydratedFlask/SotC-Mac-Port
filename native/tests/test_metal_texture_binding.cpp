#include "../platform/metal_asset_frame.hpp"
#include "../platform/metal_capture.hpp"
#include "../platform/cmsh_mesh.hpp"
#include "asset_png_fixture.hpp"

#include <CommonCrypto/CommonDigest.h>

#include <array>
#include <algorithm>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>
#include <vector>

namespace {

constexpr std::uint32_t kWidth = 64;
constexpr std::uint32_t kHeight = 64;
constexpr std::string_view kPinnedLogoPsMslSha256 =
    "071a4ebe11177086c178119fafae04aae99a1a147ad8b8d1e92ee0f17abbfa71";
constexpr std::string_view kPinnedLogoPngSha256 =
    "b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5";

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

constexpr std::string_view kGradientSampleMsl = R"metal(
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
    out.uv = value.position.xy * value.material.xy + 0.5f;
    return out;
}

fragment float4 asset_fragment(RasterValue in [[stage_in]],
                               texture2d<float> source [[texture(0)]],
                               sampler source_sampler [[sampler(0)]]) {
    const float2 gradient_x = dfdx(in.uv);
    const float2 gradient_y = dfdy(in.uv);
    const float4 implicit_sample = source.sample(source_sampler, in.uv);
    const float4 explicit_sample = source.sample(source_sampler, in.uv,
                                                 gradient2d(gradient_x, gradient_y));
    return float4(abs(gradient_x.x) * 64.0f, abs(gradient_y.y) * 64.0f,
                  implicit_sample.r, explicit_sample.r);
}
)metal";

constexpr std::string_view kMipGradientSampleMsl = R"metal(
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
    out.uv = value.position.xy * value.material.xy + 0.5f;
    return out;
}

fragment float4 asset_fragment(RasterValue in [[stage_in]],
                               texture2d<float> source [[texture(0)]],
                               sampler source_sampler [[sampler(0)]]) {
    const float2 gradient_x = dfdx(in.uv);
    const float2 gradient_y = dfdy(in.uv);
    const float4 implicit_sample = source.sample(source_sampler, in.uv);
    const float4 explicit_sample = source.sample(source_sampler, in.uv,
                                                 gradient2d(gradient_x, gradient_y));
    return float4(implicit_sample.r, explicit_sample.r, abs(gradient_x.x) * 16.0f, 1.0f);
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
        if (rgba[offset + channel] != expected[channel]) {
            std::fprintf(stderr,
                         "PIXEL mismatch (%u,%u) channel %zu: got %u expected %u\n",
                         x, y, channel, static_cast<unsigned>(rgba[offset + channel]),
                         static_cast<unsigned>(expected[channel]));
            return false;
        }
    }
    return true;
}

[[nodiscard]] std::array<std::uint8_t, 4> gradient_oracle(float uv_scale,
                                                         std::uint8_t constant_texel_red) {
    // For this synthetic contract, uv = position * uv_scale + 0.5 and one pixel spans
    // 2 / framebuffer-width clip units. The shader multiplies each derivative by that width.
    const float encoded_gradient = 2.0F * uv_scale;
    const auto gradient_byte = static_cast<std::uint8_t>(encoded_gradient * 255.0F + 0.5F);
    return {gradient_byte, gradient_byte, constant_texel_red, constant_texel_red};
}

constexpr std::array<std::array<std::uint8_t, 4>, 7> kMipColors = {{{0, 0, 0, 255},
                                                                     {255, 0, 0, 255},
                                                                     {0, 255, 0, 255},
                                                                     {0, 0, 255, 255},
                                                                     {255, 255, 0, 255},
                                                                     {255, 0, 255, 255},
                                                                     {0, 255, 255, 255}}};

[[nodiscard]] std::size_t mip_level_for_scale(float uv_scale) {
    // The affine harness UV is clip.xy * uv_scale + 0.5. For a 64x64
    // target/texture, each pixel spans 2/64 clip units and therefore 2*scale
    // texels. The nearest mip level is round(log2(rho)); test scales are exact
    // powers of two to avoid a hardware tie-breaking boundary.
    const float footprint = 2.0F * std::abs(uv_scale);
    const float lod = std::log2(footprint);
    return static_cast<std::size_t>(std::clamp(static_cast<int>(std::lround(lod)), 0,
                                               static_cast<int>(kMipColors.size() - 1)));
}

[[nodiscard]] std::array<std::uint8_t, 4> mip_color_for_scale(float uv_scale,
                                                              int mutated_level = -1) {
    auto color = kMipColors[mip_level_for_scale(uv_scale)];
    if (static_cast<int>(mip_level_for_scale(uv_scale)) == mutated_level) {
        color = {0, 0, 255, 255};
    }
    return color;
}

[[nodiscard]] std::array<std::uint8_t, 4> logo_multiply_oracle(
    float uv_scale, std::array<float, 4> color_scale, int mutated_level = -1) {
    const std::array<std::uint8_t, 4> sample = mip_color_for_scale(uv_scale, mutated_level);
    std::array<std::uint8_t, 4> result{};
    for (std::size_t channel = 0; channel < result.size(); ++channel) {
        // The synthetic inputs use binary fractions exactly representable by fp16, so RTZ leaves
        // each shader product unchanged before the observed nearest-byte RGBA8 conversion.
        const float product = (static_cast<float>(sample[channel]) / 255.0F) *
                              color_scale[channel];
        result[channel] = static_cast<std::uint8_t>(
            std::clamp(product, 0.0F, 1.0F) * 255.0F + 0.5F);
    }
    return result;
}

struct LogoTextureProbe {
    std::uint32_t output_x;
    std::uint32_t output_y;
    std::uint32_t texture_x;
    std::uint32_t texture_y;
};

struct LogoTextureUv {
    float scale_x;
    float scale_y;
    float u;
    float v;
};

[[nodiscard]] LogoTextureUv logo_texture_uv_for_probe(const LogoTextureProbe& probe,
                                                       std::uint32_t texture_width,
                                                       std::uint32_t texture_height) {
    const float clip_x =
        2.0F * (static_cast<float>(probe.output_x) + 0.5F) / static_cast<float>(kWidth) - 1.0F;
    const float clip_y =
        1.0F - 2.0F * (static_cast<float>(probe.output_y) + 0.5F) / static_cast<float>(kHeight);
    const float target_u = (static_cast<float>(probe.texture_x) + 0.5F) /
                           static_cast<float>(texture_width);
    const float target_v = (static_cast<float>(probe.texture_y) + 0.5F) /
                           static_cast<float>(texture_height);
    const float scale_x = (target_u - 0.5F) / clip_x;
    const float scale_y = (target_v - 0.5F) / clip_y;
    const float uv_x = clip_x * scale_x + 0.5F;
    const float uv_y = clip_y * scale_y + 0.5F;
    return {scale_x, scale_y, uv_x, uv_y};
}

[[nodiscard]] std::uint16_t float_to_half_rtz(float value) {
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
    const std::uint32_t sign = (bits >> 16U) & 0x8000U;
    const std::uint32_t exponent = (bits >> 23U) & 0xffU;
    const std::uint32_t mantissa = bits & 0x7fffffU;
    if (exponent == 0xffU) {
        return static_cast<std::uint16_t>(sign | (mantissa != 0 ? 0x7e00U : 0x7c00U));
    }
    const int unbiased_exponent = static_cast<int>(exponent) - 127;
    if (unbiased_exponent > 15) return static_cast<std::uint16_t>(sign | 0x7bffU);
    if (unbiased_exponent < -24) return static_cast<std::uint16_t>(sign);
    if (unbiased_exponent >= -14) {
        return static_cast<std::uint16_t>(sign |
                                          (static_cast<std::uint32_t>(unbiased_exponent + 15)
                                           << 10U) |
                                          (mantissa >> 13U));
    }
    return static_cast<std::uint16_t>(sign |
                                      ((mantissa | 0x800000U) >>
                                       static_cast<unsigned>(-unbiased_exponent - 1)));
}

[[nodiscard]] float half_to_float(std::uint16_t half) {
    const std::uint32_t sign = (static_cast<std::uint32_t>(half & 0x8000U)) << 16U;
    const std::uint32_t exponent = (half >> 10U) & 0x1fU;
    const std::uint32_t mantissa = half & 0x03ffU;
    if (exponent == 0x1fU) {
        return std::bit_cast<float>(sign | 0x7f800000U | (mantissa << 13U));
    }
    if (exponent == 0U) {
        const float magnitude = static_cast<float>(mantissa) * 0x1p-24F;
        return (sign == 0U) ? magnitude : -magnitude;
    }
    return std::bit_cast<float>(sign | ((exponent + 112U) << 23U) | (mantissa << 13U));
}

[[nodiscard]] std::array<std::uint8_t, 4> logo_png_pixel_oracle(
    std::span<const std::uint8_t> rgba8, std::uint32_t texture_width,
    std::uint32_t texture_height, const LogoTextureUv& uv, std::uint32_t* sampled_x = nullptr,
    std::uint32_t* sampled_y = nullptr) {
    const float bounded_u = std::clamp(uv.u, 0.0F, 1.0F);
    const float bounded_v = std::clamp(uv.v, 0.0F, 1.0F);
    const std::uint32_t x = std::min(static_cast<std::uint32_t>(bounded_u * texture_width),
                                     texture_width - 1U);
    const std::uint32_t y = std::min(static_cast<std::uint32_t>(bounded_v * texture_height),
                                     texture_height - 1U);
    if (sampled_x != nullptr) *sampled_x = x;
    if (sampled_y != nullptr) *sampled_y = y;
    std::array<std::uint8_t, 4> result{};
    const std::size_t offset =
        (static_cast<std::size_t>(y) * texture_width + x) * result.size();
    for (std::size_t channel = 0; channel < result.size(); ++channel) {
        const float sampled = static_cast<float>(rgba8[offset + channel]) / 255.0F;
        const float fp16_rtz = half_to_float(float_to_half_rtz(sampled));
        const float unorm = std::clamp(fp16_rtz, 0.0F, 1.0F) * 255.0F;
        result[channel] = static_cast<std::uint8_t>(unorm + 0.5F);
    }
    return result;
}

[[nodiscard]] std::array<std::uint8_t, 4> mip_oracle(float uv_scale,
                                                     int mutated_level = -1) {
    const float gradient_channel = std::clamp((2.0F / 64.0F) * std::abs(uv_scale) * 16.0F,
                                              0.0F, 1.0F);
    // The observed Metal RGBA8 render-target conversion truncates this diagnostic value: 0.5
    // reads back as 127. The shader-sampled mip colors are 0/1 and remain exact.
    const auto gradient_byte = static_cast<std::uint8_t>(gradient_channel * 255.0F);
    const auto color = mip_color_for_scale(uv_scale, mutated_level);
    return {color[0], color[0], gradient_byte, 255};
}

struct MipTextureStorage {
    std::array<std::vector<std::uint8_t>, 7> pixel_bytes;
    std::array<sotc::AssetTextureMipLevel, 6> mip_levels;

    [[nodiscard]] sotc::AssetTexture2D view() const noexcept {
        return {64, 64, std::span<const std::uint8_t>(pixel_bytes[0]),
                std::span<const sotc::AssetTextureMipLevel>(mip_levels)};
    }
};

[[nodiscard]] MipTextureStorage make_mip_texture(int mutated_level = -1) {
    constexpr std::array<std::uint32_t, 7> kDimensions = {64, 32, 16, 8, 4, 2, 1};
    MipTextureStorage result;
    for (std::size_t level = 0; level < kDimensions.size(); ++level) {
        const std::uint32_t dimension = kDimensions[level];
        auto color = kMipColors[level];
        if (static_cast<int>(level) == mutated_level) color = {0, 0, 255, 255};
        std::vector<std::uint8_t>& pixels = result.pixel_bytes[level];
        pixels.resize(static_cast<std::size_t>(dimension) * dimension * 4);
        for (std::size_t offset = 0; offset < pixels.size(); offset += 4) {
            std::copy(color.begin(), color.end(), pixels.begin() +
                      static_cast<std::ptrdiff_t>(offset));
        }
        if (level > 0) {
            result.mip_levels[level - 1] = {
                dimension, dimension, std::span<const std::uint8_t>(pixels)};
        }
    }
    return result;
}

[[nodiscard]] bool read_text_file(const char* path, std::string& contents) {
    if (path == nullptr || path[0] == '\0') return false;
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    contents.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return input.good() || input.eof();
}

[[nodiscard]] bool is_strict_path_child(const std::filesystem::path& child,
                                       const std::filesystem::path& parent) {
    auto child_part = child.begin();
    for (auto parent_part = parent.begin(); parent_part != parent.end();
         ++parent_part, ++child_part) {
        if (child_part == child.end() || *child_part != *parent_part) return false;
    }
    return child_part != child.end();
}

[[nodiscard]] bool canonical_local_path_under_repo(
    const std::filesystem::path& repo_root_input,
    const std::filesystem::path& candidate_input,
    std::filesystem::path& canonical_candidate) {
    std::error_code error;
    const std::filesystem::path repo_root =
        std::filesystem::weakly_canonical(repo_root_input, error);
    if (error) return false;
    const std::filesystem::path intended_local_root = repo_root / "local";
    const std::filesystem::file_status local_status =
        std::filesystem::symlink_status(intended_local_root, error);
    if (error && error != std::errc::no_such_file_or_directory) return false;
    if (!error && std::filesystem::is_symlink(local_status)) return false;
    error.clear();
    const std::filesystem::path local_root =
        std::filesystem::weakly_canonical(intended_local_root, error);
    if (error || local_root != intended_local_root) return false;
    const std::filesystem::path candidate =
        std::filesystem::weakly_canonical(candidate_input, error);
    if (error || !is_strict_path_child(candidate, local_root)) return false;
    canonical_candidate = candidate;
    return true;
}

[[nodiscard]] bool canonical_local_path(const char* path,
                                        std::filesystem::path& canonical_candidate) {
    if (path == nullptr || path[0] == '\0') return false;
    std::error_code error;
    const std::filesystem::path repo_root = std::filesystem::weakly_canonical(".", error);
    if (error) return false;
    return canonical_local_path_under_repo(repo_root, path, canonical_candidate);
}

[[nodiscard]] bool is_local_path(const char* path) {
    std::filesystem::path ignored;
    return canonical_local_path(path, ignored);
}

[[nodiscard]] bool local_paths_are_distinct(std::initializer_list<const char*> paths) {
    std::vector<std::filesystem::path> canonical_paths;
    canonical_paths.reserve(paths.size());
    for (const char* path : paths) {
        std::filesystem::path canonical;
        if (!canonical_local_path(path, canonical)) return false;
        for (const std::filesystem::path& previous : canonical_paths) {
            if (canonical == previous) return false;
            std::error_code error;
            const bool canonical_exists = std::filesystem::exists(canonical, error);
            if (error) return false;
            const bool previous_exists = std::filesystem::exists(previous, error);
            if (error) return false;
            if (canonical_exists && previous_exists &&
                std::filesystem::equivalent(canonical, previous, error)) {
                return false;
            }
            if (error) return false;
        }
        canonical_paths.push_back(std::move(canonical));
    }
    return !canonical_paths.empty();
}

[[nodiscard]] bool rejects_in_repo_local_root_symlink() {
    std::error_code error;
    const std::filesystem::path repo_root = std::filesystem::weakly_canonical(".", error);
    if (error) return false;
    const std::filesystem::path fixture_root =
        repo_root / "local" /
        (".gpu03-root-symlink-" + std::to_string(
             std::chrono::steady_clock::now().time_since_epoch().count()));
    if (std::filesystem::exists(fixture_root, error) || error ||
        !is_local_path((fixture_root / "probe").c_str())) {
        return false;
    }
    std::filesystem::create_directories(fixture_root / "native", error);
    if (error) return false;
    std::filesystem::create_directory_symlink("native", fixture_root / "local", error);
    if (error) {
        std::filesystem::remove_all(fixture_root, error);
        return false;
    }
    std::filesystem::path ignored;
    const bool rejected = !canonical_local_path_under_repo(
        fixture_root, fixture_root / "local" / "probe.png", ignored);
    std::filesystem::remove_all(fixture_root, error);
    return rejected && !error;
}

[[nodiscard]] bool read_binary_file(const char* path, std::vector<std::uint8_t>& contents) {
    if (!is_local_path(path)) return false;
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    contents.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return input.good() || input.eof();
}

[[nodiscard]] std::string sha256_hex(std::span<const std::uint8_t> bytes) {
    std::array<unsigned char, CC_SHA256_DIGEST_LENGTH> digest{};
    if (CC_SHA256(bytes.data(), static_cast<CC_LONG>(bytes.size()), digest.data()) == nullptr) {
        return {};
    }
    constexpr char kHex[] = "0123456789abcdef";
    std::string hex;
    hex.reserve(digest.size() * 2U);
    for (unsigned char byte : digest) {
        hex.push_back(kHex[byte >> 4U]);
        hex.push_back(kHex[byte & 0x0fU]);
    }
    return hex;
}

[[nodiscard]] bool write_png_file(const char* path, const std::vector<std::uint8_t>& png) {
    if (!is_local_path(path) || png.empty()) return false;
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char*>(png.data()),
                 static_cast<std::streamsize>(png.size()));
    return output.good();
}

[[nodiscard]] bool load_pinned_cmsh(const char* path, std::size_t expected_size,
                                    std::string_view expected_sha256,
                                    std::size_t expected_positions,
                                    std::size_t expected_indices,
                                    std::string_view label, sotc::CmshMesh& mesh) {
    std::vector<std::uint8_t> raw;
    const bool read = read_binary_file(path, raw);
    check(std::string(label) + " raw .cmsh input is read under local/", read);
    if (!read) return false;

    const std::string digest = sha256_hex(raw);
    const bool size_ok = raw.size() == expected_size;
    const bool digest_ok = digest == expected_sha256;
    check(std::string(label) + " exact raw byte count", size_ok);
    check(std::string(label) + " exact raw SHA-256", digest_ok);
    if (!size_ok || !digest_ok) return false;

    const std::span<const std::byte> bytes(
        reinterpret_cast<const std::byte*>(raw.data()), raw.size());
    const sotc::CmshParseResult parsed_result = sotc::parse_cmsh_mesh(bytes);
    const auto* parsed = std::get_if<sotc::CmshMesh>(&parsed_result);
    check(std::string(label) + " parses as POS0/U16", parsed != nullptr);
    if (parsed == nullptr) {
        const auto* parse_error = std::get_if<sotc::CmshParseError>(&parsed_result);
        std::fprintf(stderr, "FAIL %.*s parse: %s at byte %zu\n",
                     static_cast<int>(label.size()), label.data(),
                     parse_error == nullptr ? "unknown error"
                                            : sotc::cmsh_error_name(parse_error->code),
                     parse_error == nullptr ? 0 : parse_error->byte_offset);
        return false;
    }

    mesh = *parsed;
    const bool counts_ok = mesh.positions.size() == expected_positions &&
                           mesh.indices.size() == expected_indices &&
                           mesh.indices.size() % 3U == 0U;
    check(std::string(label) + " parsed position/index counts match the pinned fixture",
          counts_ok);
    const bool indices_in_range = std::all_of(
        mesh.indices.begin(), mesh.indices.end(), [&mesh](std::uint16_t index) {
            return index < mesh.positions.size();
        });
    check(std::string(label) + " parsed indices address the pinned positions",
          indices_in_range);
    std::printf("logo mesh input %.*s: bytes=%zu sha256=%s positions=%zu indices=%zu\n",
                static_cast<int>(label.size()), label.data(), raw.size(), digest.c_str(),
                mesh.positions.size(), mesh.indices.size());
    return counts_ok && indices_in_range;
}

[[nodiscard]] bool cmsh_position_bounds(const sotc::CmshMesh& mesh, float& min_x,
                                        float& max_x, float& min_y, float& max_y) {
    if (mesh.positions.empty()) return false;
    min_x = max_x = mesh.positions.front().x;
    min_y = max_y = mesh.positions.front().y;
    for (const sotc::CmshPosition& position : mesh.positions) {
        if (!std::isfinite(position.x) || !std::isfinite(position.y) ||
            !std::isfinite(position.z)) {
            return false;
        }
        min_x = std::min(min_x, position.x);
        max_x = std::max(max_x, position.x);
        min_y = std::min(min_y, position.y);
        max_y = std::max(max_y, position.y);
    }
    return max_x > min_x && max_y > min_y;
}

[[nodiscard]] bool transform_cmsh_positions_to_clip(
    const sotc::CmshMesh& mesh, float clip_center_x,
    std::vector<sotc::CmshPosition>& clip_positions) {
    float min_x = 0.0F;
    float max_x = 0.0F;
    float min_y = 0.0F;
    float max_y = 0.0F;
    if (!cmsh_position_bounds(mesh, min_x, max_x, min_y, max_y)) return false;

    constexpr float kMaximumClipSpan = 0.58F;
    const float source_span = std::max(max_x - min_x, max_y - min_y);
    const float scale = kMaximumClipSpan / source_span;
    const float center_x = (min_x + max_x) * 0.5F;
    const float center_y = (min_y + max_y) * 0.5F;
    clip_positions.clear();
    clip_positions.reserve(mesh.positions.size());
    for (const sotc::CmshPosition& position : mesh.positions) {
        clip_positions.push_back({(position.x - center_x) * scale + clip_center_x,
                                  (position.y - center_y) * scale, 0.25F});
    }
    return std::all_of(clip_positions.begin(), clip_positions.end(),
                       [](const sotc::CmshPosition& position) {
                           return std::isfinite(position.x) && std::isfinite(position.y) &&
                                  position.x > -1.0F && position.x < 1.0F &&
                                  position.y > -1.0F && position.y < 1.0F;
                       });
}

struct MeshLogoProbe {
    std::uint32_t output_x{};
    std::uint32_t output_y{};
    std::uint32_t texture_x{};
    std::uint32_t texture_y{};
    float u{};
    float v{};
    float minimum_barycentric{};
};

[[nodiscard]] bool find_mesh_logo_probe(
    const std::vector<sotc::CmshPosition>& clip_positions,
    std::span<const std::uint16_t> indices, float material_x, float material_y,
    std::span<const std::uint8_t> texture, std::uint32_t texture_width,
    std::uint32_t texture_height, MeshLogoProbe& probe) {
    if (indices.empty() || indices.size() % 3U != 0U || texture_width == 0U ||
        texture_height == 0U) {
        return false;
    }

    bool found = false;
    float best_score = -1.0F;
    for (std::size_t index_offset = 0; index_offset < indices.size(); index_offset += 3U) {
        const std::uint16_t i0 = indices[index_offset];
        const std::uint16_t i1 = indices[index_offset + 1U];
        const std::uint16_t i2 = indices[index_offset + 2U];
        if (i0 >= clip_positions.size() || i1 >= clip_positions.size() ||
            i2 >= clip_positions.size()) {
            return false;
        }
        const sotc::CmshPosition& p0 = clip_positions[i0];
        const sotc::CmshPosition& p1 = clip_positions[i1];
        const sotc::CmshPosition& p2 = clip_positions[i2];
        const float x0 = (p0.x * 0.5F + 0.5F) * static_cast<float>(kWidth);
        const float y0 = (0.5F - p0.y * 0.5F) * static_cast<float>(kHeight);
        const float x1 = (p1.x * 0.5F + 0.5F) * static_cast<float>(kWidth);
        const float y1 = (0.5F - p1.y * 0.5F) * static_cast<float>(kHeight);
        const float x2 = (p2.x * 0.5F + 0.5F) * static_cast<float>(kWidth);
        const float y2 = (0.5F - p2.y * 0.5F) * static_cast<float>(kHeight);
        const float denominator = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
        if (std::abs(denominator) < 0.0001F) continue;

        const float u0 = p0.x * material_x + 0.5F;
        const float v0 = p0.y * material_y + 0.5F;
        const float u1 = p1.x * material_x + 0.5F;
        const float v1 = p1.y * material_y + 0.5F;
        const float u2 = p2.x * material_x + 0.5F;
        const float v2 = p2.y * material_y + 0.5F;

        for (std::uint32_t y = 0; y < kHeight; ++y) {
            for (std::uint32_t x = 0; x < kWidth; ++x) {
                const float px = static_cast<float>(x) + 0.5F;
                const float py = static_cast<float>(y) + 0.5F;
                const float w0 = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) /
                                 denominator;
                const float w1 = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) /
                                 denominator;
                const float w2 = 1.0F - w0 - w1;
                const float minimum_barycentric = std::min({w0, w1, w2});
                // Stay well inside a real indexed triangle, away from edge-rule ambiguity.
                if (minimum_barycentric < 0.16F) continue;

                const float u = w0 * u0 + w1 * u1 + w2 * u2;
                const float v = w0 * v0 + w1 * v1 + w2 * v2;
                if (u <= 0.0F || u >= 1.0F || v <= 0.0F || v >= 1.0F) continue;
                const float texel_u = u * static_cast<float>(texture_width);
                const float texel_v = v * static_cast<float>(texture_height);
                const float fraction_u = texel_u - std::floor(texel_u);
                const float fraction_v = texel_v - std::floor(texel_v);
                const float texel_margin = std::min(
                    {fraction_u, 1.0F - fraction_u, fraction_v, 1.0F - fraction_v});
                // Avoid nearest-sampler texel boundaries so interpolation roundoff cannot change
                // which source texel the independent CPU oracle selects.
                if (texel_margin < 0.16F) continue;

                const LogoTextureUv uv{material_x, material_y, u, v};
                std::uint32_t texture_x = 0;
                std::uint32_t texture_y = 0;
                const std::array<std::uint8_t, 4> expected = logo_png_pixel_oracle(
                    texture, texture_width, texture_height, uv, &texture_x, &texture_y);
                if (expected == std::array<std::uint8_t, 4>{0, 0, 0, 255}) continue;

                const float score = minimum_barycentric + texel_margin * 0.25F;
                if (!found || score > best_score) {
                    found = true;
                    best_score = score;
                    probe = {x, y, texture_x, texture_y, u, v, minimum_barycentric};
                }
            }
        }
    }
    return found;
}

[[nodiscard]] bool run_cmsh_logo_fixture(
    sotc::MetalAssetFrameRenderer& renderer, const char* cube_path, const char* cone_path,
    const char* output_png_path, const char* mutation_png_path,
    std::span<const std::uint8_t> logo_png_rgba8, std::uint32_t texture_width,
    std::uint32_t texture_height) {
    constexpr std::string_view kCubeSha256 =
        "44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7";
    constexpr std::string_view kConeSha256 =
        "5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c";
    bool passed = true;
    const auto record = [&passed](std::string_view name, bool result) {
        check(name, result);
        passed = passed && result;
    };

    sotc::CmshMesh cube;
    sotc::CmshMesh cone;
    const bool cube_ok = load_pinned_cmsh(cube_path, 850U, kCubeSha256, 24U, 36U,
                                          "cube_1m.cmsh", cube);
    const bool cone_ok = load_pinned_cmsh(cone_path, 1462U, kConeSha256, 42U, 114U,
                                          "cone.cmsh", cone);
    record("both exact-hash raw mesh inputs parse with pinned counts", cube_ok && cone_ok);
    if (!cube_ok || !cone_ok) return false;

    std::vector<sotc::CmshPosition> cube_clip_positions;
    std::vector<sotc::CmshPosition> cone_clip_positions;
    record("cube positions receive a test-only clip-space transform",
           transform_cmsh_positions_to_clip(cube, -0.47F, cube_clip_positions));
    record("cone positions receive a test-only clip-space transform",
           transform_cmsh_positions_to_clip(cone, 0.47F, cone_clip_positions));
    if (cube_clip_positions.size() != cube.positions.size() ||
        cone_clip_positions.size() != cone.positions.size()) {
        return false;
    }

    constexpr float kHarnessUvScale = 0.25F;
    constexpr float kHarnessDepth = 0.25F;
    const sotc::AssetSamplerInput sampler{
        sotc::AssetTextureFilter::nearest, sotc::AssetTextureFilter::nearest,
        sotc::AssetTextureAddressMode::clamp_to_edge,
        sotc::AssetTextureAddressMode::clamp_to_edge};
    sotc::DefaultShaderCase cube_shader = sotc::make_default_shader_case();
    cube_shader.material = {kHarnessUvScale, kHarnessUvScale, 0.0F, 1.0F};
    cube_shader.encoded_normal = {0.5F, 0.5F, 1.0F, 0.0F};
    cube_shader.param0_p0 = {1.0F, 1.0F, 1.0F, 1.0F};
    sotc::DefaultShaderCase cone_shader = cube_shader;
    cone_shader.clip_row2[3] = kHarnessDepth;
    cube_shader.clip_row2[3] = kHarnessDepth;

    sotc::AssetMeshDraw cube_draw{cube_clip_positions, cube.indices, cube_shader};
    sotc::AssetMeshDraw cone_draw{cone_clip_positions, cone.indices, cone_shader};
    cube_draw.fragment_texture0 =
        sotc::AssetTexture2D{texture_width, texture_height, logo_png_rgba8};
    cone_draw.fragment_texture0 =
        sotc::AssetTexture2D{texture_width, texture_height, logo_png_rgba8};
    cube_draw.fragment_sampler0 = sampler;
    cone_draw.fragment_sampler0 = sampler;
    std::vector<sotc::AssetMeshDraw> draws{cube_draw, cone_draw};

    MeshLogoProbe cube_probe;
    MeshLogoProbe cone_probe;
    const bool cube_probe_found = find_mesh_logo_probe(
        cube_clip_positions, cube.indices, kHarnessUvScale, kHarnessUvScale,
        logo_png_rgba8, texture_width, texture_height, cube_probe);
    const bool cone_probe_found = find_mesh_logo_probe(
        cone_clip_positions, cone.indices, kHarnessUvScale, kHarnessUvScale,
        logo_png_rgba8, texture_width, texture_height, cone_probe);
    record("cube oracle probe is an interior pixel covered by an indexed mesh triangle",
           cube_probe_found);
    record("cone oracle probe is an interior pixel covered by an indexed mesh triangle",
           cone_probe_found);
    if (!cube_probe_found || !cone_probe_found) return false;

    sotc::MetalAssetFrameResult baseline = renderer.render(draws);
    record("combined cube/cone logo-PS Metal frame renders", baseline.ok);
    if (!baseline.ok) {
        std::fprintf(stderr, "FAIL combined logo mesh Metal render: %s\n",
                     baseline.error.c_str());
        return false;
    }
    const bool baseline_shape_ok =
        baseline.width == kWidth && baseline.height == kHeight && baseline.stride == kWidth * 4U &&
        baseline.rgba8.size() == static_cast<std::size_t>(baseline.stride) * kHeight &&
        !baseline.png.empty();
    record("combined logo mesh frame has a complete 64x64 RGBA8 readback and PNG",
           baseline_shape_ok);
    if (!baseline_shape_ok) return false;

    const auto check_probe = [&](std::string_view label, const MeshLogoProbe& probe,
                                 std::span<const std::uint8_t> texture,
                                 const sotc::MetalAssetFrameResult& frame) {
        const LogoTextureUv uv{kHarnessUvScale, kHarnessUvScale, probe.u, probe.v};
        const auto expected = logo_png_pixel_oracle(texture, texture_width, texture_height, uv);
        std::string check_name(label);
        check_name += " covered texel matches independent nearest/fp16-RTZ CPU oracle";
        record(check_name, is_color(frame.rgba8, probe.output_x, probe.output_y, expected));
    };
    check_probe("cube", cube_probe, logo_png_rgba8, baseline);
    check_probe("cone", cone_probe, logo_png_rgba8, baseline);

    std::printf("mesh-proven logo probes: cube output=(%u,%u) texel=(%u,%u) "
                "barycentric-min=%.3f; cone output=(%u,%u) texel=(%u,%u) "
                "barycentric-min=%.3f\n",
                cube_probe.output_x, cube_probe.output_y, cube_probe.texture_x,
                cube_probe.texture_y, static_cast<double>(cube_probe.minimum_barycentric),
                cone_probe.output_x, cone_probe.output_y, cone_probe.texture_x,
                cone_probe.texture_y, static_cast<double>(cone_probe.minimum_barycentric));

    const sotc::MetalAssetFrameResult repeated = renderer.render(draws);
    record("combined logo mesh frame repeats", repeated.ok);
    if (repeated.ok) {
        record("combined logo mesh RGBA8 and PNG are byte deterministic",
               repeated.rgba8 == baseline.rgba8 && repeated.png == baseline.png);
    }

    std::vector<std::uint8_t> mutated_texture(logo_png_rgba8.begin(), logo_png_rgba8.end());
    const std::size_t mutated_texel_offset =
        (static_cast<std::size_t>(cube_probe.texture_y) * texture_width +
         cube_probe.texture_x) *
        4U;
    mutated_texture[mutated_texel_offset] =
        mutated_texture[mutated_texel_offset] == 255U ? 0U : 255U;
    sotc::AssetTexture2D mutated_texture_view{
        texture_width, texture_height, std::span<const std::uint8_t>(mutated_texture)};
    std::vector<sotc::AssetMeshDraw> mutated_draws = draws;
    mutated_draws[0].fragment_texture0 = mutated_texture_view;
    mutated_draws[1].fragment_texture0 = mutated_texture_view;
    const sotc::MetalAssetFrameResult mutated = renderer.render(mutated_draws);
    record("mutated source logo texel renders through both parsed meshes", mutated.ok);
    if (mutated.ok) {
        check_probe("mutated cube", cube_probe, mutated_texture, mutated);
        check_probe("mutated cone", cone_probe, mutated_texture, mutated);
        const std::size_t output_offset = pixel_offset(cube_probe.output_x, cube_probe.output_y);
        const bool selected_pixel_changed = !std::equal(
            baseline.rgba8.begin() + static_cast<std::ptrdiff_t>(output_offset),
            baseline.rgba8.begin() + static_cast<std::ptrdiff_t>(output_offset + 4U),
            mutated.rgba8.begin() + static_cast<std::ptrdiff_t>(output_offset));
        record("source texel mutation changes a CPU-proven mesh-covered readback pixel",
               selected_pixel_changed);
        record("source texel mutation changes the combined output PNG", mutated.png != baseline.png);

        record("combined mesh baseline PNG writes under distinct local evidence",
               write_png_file(output_png_path, baseline.png));
        record("combined mesh mutation PNG writes under distinct local evidence",
               write_png_file(mutation_png_path, mutated.png));
    }

    std::puts("logo PS composite scope: test-only clip-space transform and synthetic XY-to-UV "
              "mapping; normals fixed to harness value; TEX0/NRM0 NOT decoded; retail UV, "
              "bindings, and orientation NOT VERIFIED; AMD WQM/helper-lane equivalence NOT "
              "VERIFIED; game-driven frame, G3, boot, game audio, and save/reload NOT VERIFIED.");
    return passed;
}

}  // namespace

int main() {
    static_assert(sizeof(sotc::DefaultShaderCase) == 288);
    static_assert(offsetof(sotc::DefaultShaderCase, param1_p0) == 224);

    std::error_code symlink_error;
    const std::filesystem::path escape_link =
        std::filesystem::path("local") /
        (".gpu03-path-guard-" + std::to_string(
             std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory_symlink("..", escape_link, symlink_error);
    const bool symlink_created = !symlink_error;
    const bool symlink_escape_rejected =
        symlink_created && !is_local_path((escape_link / "guard-probe.png").c_str());
    if (symlink_created) std::filesystem::remove(escape_link, symlink_error);
    check("local output guard rejects symlinks escaping local", symlink_created &&
              symlink_escape_rejected && !symlink_error);
    check("local output guard rejects local-root symlinks into tracked paths",
          rejects_in_repo_local_root_symlink());

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
          !truncated.ok && truncated.error.find("must contain exactly 8 RGBA8 bytes") !=
                                std::string::npos);

    sotc::AssetMeshDraw invalid_sampler_draw = draw;
    invalid_sampler_draw.fragment_sampler0->min_filter =
        static_cast<sotc::AssetTextureFilter>(255);
    sotc::MetalAssetFrameResult invalid_sampler = renderer->render(std::span(&invalid_sampler_draw, 1));
    check("unknown sampler filter fails visibly",
          !invalid_sampler.ok &&
              invalid_sampler.error.find("unsupported filter or address mode") != std::string::npos);

    const std::array<std::uint8_t, 4> constant_pixels = {64, 128, 192, 255};
    sotc::DefaultShaderCase gradient_shader = sotc::make_default_shader_case();
    gradient_shader.material = {0.5F, 0.5F, 0.0F, 1.0F};
    sotc::AssetMeshDraw gradient_draw{positions, indices, gradient_shader};
    gradient_draw.fragment_texture0 = sotc::AssetTexture2D{1, 1, constant_pixels};
    gradient_draw.fragment_sampler0 = sotc::AssetSamplerInput{
        sotc::AssetTextureFilter::linear, sotc::AssetTextureFilter::linear,
        sotc::AssetTextureAddressMode::clamp_to_edge,
        sotc::AssetTextureAddressMode::clamp_to_edge};

    std::string gradient_error;
    std::unique_ptr<sotc::MetalAssetFrameRenderer> gradient_renderer =
        sotc::MetalAssetFrameRenderer::create(config, kGradientSampleMsl, gradient_error);
    if (gradient_renderer == nullptr) {
        std::fprintf(stderr, "FAIL derivative MSL pipeline creation: %s\n",
                     gradient_error.c_str());
        return 1;
    }

    sotc::MetalAssetFrameResult gradient_frame = gradient_renderer->render(std::span(&gradient_draw, 1));
    check("fragment derivative fixture renders", gradient_frame.ok);
    if (!gradient_frame.ok) {
        std::fprintf(stderr, "FAIL derivative render: %s\n", gradient_frame.error.c_str());
        return 1;
    }
    const auto gradient_expected = gradient_oracle(0.5F, constant_pixels[0]);
    check("CPU oracle matches interior derivatives and implicit/explicit sample pair",
          is_color(gradient_frame.rgba8, 32, 32, gradient_expected));
    check("CPU oracle matches helper-lane derivatives and samples at primitive boundary",
          is_color(gradient_frame.rgba8, 3, 32, gradient_expected));

    sotc::AssetMeshDraw changed_gradient_draw = gradient_draw;
    changed_gradient_draw.shader.material[0] = 0.375F;
    changed_gradient_draw.shader.material[1] = 0.375F;
    sotc::MetalAssetFrameResult changed_gradient =
        gradient_renderer->render(std::span(&changed_gradient_draw, 1));
    check("mutated fragment gradient renders", changed_gradient.ok);
    if (changed_gradient.ok) {
        const auto changed_expected = gradient_oracle(0.375F, constant_pixels[0]);
        check("CPU oracle observes input-gradient mutation",
              is_color(changed_gradient.rgba8, 32, 32, changed_expected));
        check("input-gradient mutation changes readback", changed_gradient.rgba8 != gradient_frame.rgba8);
    }

    const std::array<std::uint8_t, 4> changed_constant_pixels = {65, 128, 192, 255};
    sotc::AssetMeshDraw changed_texel_draw = gradient_draw;
    changed_texel_draw.fragment_texture0 = sotc::AssetTexture2D{1, 1, changed_constant_pixels};
    sotc::MetalAssetFrameResult changed_texel =
        gradient_renderer->render(std::span(&changed_texel_draw, 1));
    check("mutated derivative-sample texel renders", changed_texel.ok);
    if (changed_texel.ok) {
        check("CPU oracle observes texel mutation",
              is_color(changed_texel.rgba8, 32, 32,
                       gradient_oracle(0.5F, changed_constant_pixels[0])));
        check("texel mutation changes readback", changed_texel.rgba8 != gradient_frame.rgba8);
    }

    const MipTextureStorage mip_texture = make_mip_texture();
    sotc::AssetMeshDraw mip_draw{positions, indices, gradient_shader};
    mip_draw.shader.material = {1.0F, 1.0F, 0.0F, 1.0F};
    mip_draw.fragment_texture0 = mip_texture.view();
    sotc::AssetSamplerInput mip_sampler{
        sotc::AssetTextureFilter::linear, sotc::AssetTextureFilter::linear,
        sotc::AssetTextureAddressMode::clamp_to_edge,
        sotc::AssetTextureAddressMode::clamp_to_edge};
    mip_sampler.mip_filter = sotc::AssetTextureMipFilter::nearest;
    mip_sampler.lod_min_clamp = 0.0F;
    mip_sampler.lod_max_clamp = 6.0F;
    mip_draw.fragment_sampler0 = mip_sampler;

    std::string mip_error;
    std::unique_ptr<sotc::MetalAssetFrameRenderer> mip_renderer =
        sotc::MetalAssetFrameRenderer::create(config, kMipGradientSampleMsl, mip_error);
    if (mip_renderer == nullptr) {
        std::fprintf(stderr, "FAIL mip derivative MSL pipeline creation: %s\n", mip_error.c_str());
        return 1;
    }
    sotc::MetalAssetFrameResult mip_frame = mip_renderer->render(std::span(&mip_draw, 1));
    check("mip-sensitive implicit/explicit sample fixture renders", mip_frame.ok);
    if (!mip_frame.ok) {
        std::fprintf(stderr, "FAIL mip derivative render: %s\n", mip_frame.error.c_str());
        return 1;
    }
    check("CPU oracle matches derivative-selected mip at interior pixel",
          is_color(mip_frame.rgba8, 32, 32, mip_oracle(1.0F)));
    check("CPU oracle matches derivative-selected mip at primitive boundary",
          is_color(mip_frame.rgba8, 3, 32, mip_oracle(1.0F)));

    sotc::AssetMeshDraw second_mip_draw = mip_draw;
    second_mip_draw.shader.material[0] = 2.0F;
    second_mip_draw.shader.material[1] = 2.0F;
    sotc::MetalAssetFrameResult second_mip_frame =
        mip_renderer->render(std::span(&second_mip_draw, 1));
    check("larger affine UV gradient renders", second_mip_frame.ok);
    if (second_mip_frame.ok) {
        check("CPU oracle observes implicit LOD transition to mip 2",
              is_color(second_mip_frame.rgba8, 32, 32, mip_oracle(2.0F)) &&
                  is_color(second_mip_frame.rgba8, 3, 32, mip_oracle(2.0F)));
        check("UV scale mutation changes mip-sensitive readback",
              second_mip_frame.rgba8 != mip_frame.rgba8);
    }

    const MipTextureStorage mutated_mip_texture = make_mip_texture(1);
    sotc::AssetMeshDraw mutated_mip_draw = mip_draw;
    mutated_mip_draw.fragment_texture0 = mutated_mip_texture.view();
    sotc::MetalAssetFrameResult mutated_mip_frame =
        mip_renderer->render(std::span(&mutated_mip_draw, 1));
    check("mutated mip level renders", mutated_mip_frame.ok);
    if (mutated_mip_frame.ok) {
        check("CPU oracle observes selected-mip texture mutation",
              is_color(mutated_mip_frame.rgba8, 32, 32, mip_oracle(1.0F, 1)));
        check("selected-mip texture mutation changes readback",
              mutated_mip_frame.rgba8 != mip_frame.rgba8);
    }

    const std::array<sotc::AssetTextureMipLevel, 1> wrong_dimension_levels = {{
        {31, 32, std::span<const std::uint8_t>(mip_texture.pixel_bytes[1])}}};
    sotc::AssetMeshDraw wrong_mip_dimensions_draw = mip_draw;
    wrong_mip_dimensions_draw.fragment_texture0 = sotc::AssetTexture2D{
        64, 64, std::span<const std::uint8_t>(mip_texture.pixel_bytes[0]),
        std::span<const sotc::AssetTextureMipLevel>(wrong_dimension_levels)};
    const sotc::MetalAssetFrameResult wrong_mip_dimensions =
        mip_renderer->render(std::span(&wrong_mip_dimensions_draw, 1));
    check("noncanonical mip dimensions fail visibly",
          !wrong_mip_dimensions.ok &&
              wrong_mip_dimensions.error.find("dimensions must be 32x32") != std::string::npos);

    std::array<std::uint8_t, 32 * 32 * 4 - 1> truncated_mip_bytes{};
    const std::array<sotc::AssetTextureMipLevel, 1> truncated_mip_levels = {{
        {32, 32, std::span<const std::uint8_t>(truncated_mip_bytes)}}};
    sotc::AssetMeshDraw truncated_mip_draw = mip_draw;
    truncated_mip_draw.fragment_texture0 = sotc::AssetTexture2D{
        64, 64, std::span<const std::uint8_t>(mip_texture.pixel_bytes[0]),
        std::span<const sotc::AssetTextureMipLevel>(truncated_mip_levels)};
    const sotc::MetalAssetFrameResult truncated_mip =
        mip_renderer->render(std::span(&truncated_mip_draw, 1));
    check("truncated supplied mip bytes fail visibly",
          !truncated_mip.ok && truncated_mip.error.find("mip level 1 must contain exactly") !=
                                    std::string::npos);

    sotc::AssetMeshDraw invalid_lod_range_draw = mip_draw;
    invalid_lod_range_draw.fragment_sampler0->lod_min_clamp = 2.0F;
    invalid_lod_range_draw.fragment_sampler0->lod_max_clamp = 1.0F;
    const sotc::MetalAssetFrameResult invalid_lod_range =
        mip_renderer->render(std::span(&invalid_lod_range_draw, 1));
    check("unordered sampler LOD clamps fail visibly",
          !invalid_lod_range.ok &&
              invalid_lod_range.error.find("LOD clamp range") != std::string::npos);

    const char* logo_msl_path = std::getenv("SOTC_LOGO_PS_MSL");
    const char* logo_texture_png_path = std::getenv("SOTC_LOGO_PS_TEXTURE_PNG");
    const char* logo_ps_synthetic_png_path = std::getenv("SOTC_LOGO_PS_PNG");
    const char* baseline_texture_png_path =
        std::getenv("SOTC_LOGO_PS_TEXTURE_OUTPUT_PNG");
    const char* mutation_texture_png_path =
        std::getenv("SOTC_LOGO_PS_TEXTURE_MUTATION_PNG");
    const char* logo_cube_cmsh_path = std::getenv("SOTC_LOGO_PS_CUBE_CMSH");
    const char* logo_cone_cmsh_path = std::getenv("SOTC_LOGO_PS_CONE_CMSH");
    const char* logo_mesh_output_png_path = std::getenv("SOTC_LOGO_PS_MESH_OUTPUT_PNG");
    const char* logo_mesh_mutation_png_path =
        std::getenv("SOTC_LOGO_PS_MESH_MUTATION_PNG");
    const auto path_is_set = [](const char* path) { return path != nullptr && path[0] != '\0'; };
    const bool logo_texture_png_requested = path_is_set(logo_texture_png_path) ||
                                            path_is_set(baseline_texture_png_path) ||
                                            path_is_set(mutation_texture_png_path);
    const bool logo_mesh_fixture_requested =
        path_is_set(logo_cube_cmsh_path) || path_is_set(logo_cone_cmsh_path) ||
        path_is_set(logo_mesh_output_png_path) || path_is_set(logo_mesh_mutation_png_path);
    bool logo_texture_output_paths_valid = true;
    bool logo_texture_fixture_config_valid = false;
    bool logo_mesh_fixture_config_valid = false;
    if (logo_mesh_fixture_requested) {
        const bool complete_path_set =
            path_is_set(logo_msl_path) && path_is_set(logo_texture_png_path) &&
            path_is_set(baseline_texture_png_path) && path_is_set(mutation_texture_png_path) &&
            path_is_set(logo_cube_cmsh_path) && path_is_set(logo_cone_cmsh_path) &&
            path_is_set(logo_mesh_output_png_path) &&
            path_is_set(logo_mesh_mutation_png_path);
        if (complete_path_set) {
            if (path_is_set(logo_ps_synthetic_png_path)) {
                logo_texture_output_paths_valid = local_paths_are_distinct(
                    {logo_msl_path, logo_texture_png_path, baseline_texture_png_path,
                     mutation_texture_png_path, logo_ps_synthetic_png_path,
                     logo_cube_cmsh_path, logo_cone_cmsh_path, logo_mesh_output_png_path,
                     logo_mesh_mutation_png_path});
            } else {
                logo_texture_output_paths_valid = local_paths_are_distinct(
                    {logo_msl_path, logo_texture_png_path, baseline_texture_png_path,
                     mutation_texture_png_path, logo_cube_cmsh_path, logo_cone_cmsh_path,
                     logo_mesh_output_png_path, logo_mesh_mutation_png_path});
            }
        } else {
            logo_texture_output_paths_valid = false;
        }
        logo_mesh_fixture_config_valid = complete_path_set && logo_texture_output_paths_valid;
        logo_texture_fixture_config_valid = logo_mesh_fixture_config_valid;
        check("composite mesh fixture has a complete canonical distinct local path set",
              logo_mesh_fixture_config_valid);
        if (!logo_mesh_fixture_config_valid) {
            std::puts("logo PS composite fixture: NOT RUN (partial/invalid path configuration; "
                      "no private asset reads or PNG writes)");
        }
        std::puts("logo PS composite limits: test-only clip-space transform and synthetic XY-to-UV "
                  "mapping; normals fixed to harness value; TEX0/NRM0 NOT decoded; retail UV, "
                  "bindings, and orientation NOT VERIFIED; AMD WQM/helper-lane equivalence NOT "
                  "VERIFIED; game-driven frame, G3, boot, game audio, and save/reload NOT VERIFIED.");
    } else if (logo_texture_png_requested) {
        const bool complete_path_set = path_is_set(logo_msl_path) &&
                                       path_is_set(logo_texture_png_path) &&
                                       path_is_set(baseline_texture_png_path) &&
                                       path_is_set(mutation_texture_png_path);
        if (complete_path_set) {
            if (path_is_set(logo_ps_synthetic_png_path)) {
                logo_texture_output_paths_valid = local_paths_are_distinct(
                    {logo_msl_path, logo_texture_png_path, baseline_texture_png_path,
                     mutation_texture_png_path, logo_ps_synthetic_png_path});
            } else {
                logo_texture_output_paths_valid = local_paths_are_distinct(
                    {logo_msl_path, logo_texture_png_path, baseline_texture_png_path,
                     mutation_texture_png_path});
            }
        } else {
            logo_texture_output_paths_valid = false;
        }
        logo_texture_fixture_config_valid = complete_path_set && logo_texture_output_paths_valid;
        check("logo texture replay has a complete canonical distinct local path set",
              logo_texture_fixture_config_valid);
        if (!logo_texture_fixture_config_valid) {
            std::puts("logo PS asset replay: NOT RUN (partial/invalid path configuration; "
                      "no private asset reads or PNG writes)");
        }
    } else if (logo_msl_path != nullptr && logo_msl_path[0] != '\0') {
        if (logo_ps_synthetic_png_path != nullptr && logo_ps_synthetic_png_path[0] != '\0') {
            logo_texture_output_paths_valid =
                local_paths_are_distinct({logo_msl_path, logo_ps_synthetic_png_path});
        } else {
            logo_texture_output_paths_valid = is_local_path(logo_msl_path);
        }
        check("logo MSL and synthetic PNG paths are distinct local files",
              logo_texture_output_paths_valid);
    }
    bool logo_texture_png_ran = false;
    bool logo_mesh_fixture_passed = false;
    if (logo_mesh_fixture_requested && !logo_mesh_fixture_config_valid) {
        // Keep a partial composite configuration fail-visible while avoiding every optional
        // private input read and output write in the logo fixture block below.
    } else if (logo_texture_png_requested && !logo_texture_fixture_config_valid) {
        // Any partial asset input/output request fails visibly without reading or writing files.
    } else if (logo_msl_path == nullptr || logo_msl_path[0] == '\0') {
        if (logo_texture_png_requested) {
            check("logo PNG replay requires the private pinned logo PS MSL", false);
        } else {
            std::puts("logo PS MSL replay: NOT RUN (set SOTC_LOGO_PS_MSL to the private generated source)");
        }
    } else {
        std::string logo_msl_source;
        const bool logo_msl_read = logo_texture_output_paths_valid &&
                                   read_text_file(logo_msl_path, logo_msl_source);
        check("private generated logo PS MSL can be read", logo_msl_read);
        const auto logo_msl_bytes = std::span(
            reinterpret_cast<const std::uint8_t*>(logo_msl_source.data()), logo_msl_source.size());
        const bool logo_msl_pinned =
            logo_msl_read && sha256_hex(logo_msl_bytes) == kPinnedLogoPsMslSha256;
        check("logo PS MSL matches the pinned generated shader digest", logo_msl_pinned);
        if (logo_msl_pinned) {
            std::string logo_error;
            std::unique_ptr<sotc::MetalAssetFrameRenderer> logo_renderer =
                sotc::MetalAssetFrameRenderer::create(config, logo_msl_source, logo_error);
            check("generated pinned logo PS MSL compiles as a fragment pipeline",
                  logo_renderer != nullptr);
            if (logo_renderer != nullptr) {
                auto make_logo_draw = [&](float uv_scale, const MipTextureStorage& texture,
                                          std::array<float, 4> color_scale = {1.0F, 1.0F,
                                                                              1.0F, 1.0F}) {
                    sotc::DefaultShaderCase logo_shader = sotc::make_default_shader_case();
                    logo_shader.material = {uv_scale, uv_scale, 0.0F, 1.0F};
                    logo_shader.param0_p0 = color_scale;
                    sotc::AssetMeshDraw logo_draw{positions, indices, logo_shader};
                    logo_draw.fragment_texture0 = texture.view();
                    logo_draw.fragment_sampler0 = mip_sampler;
                    return logo_draw;
                };
                sotc::AssetMeshDraw logo_mip1_draw = make_logo_draw(1.0F, mip_texture);
                sotc::MetalAssetFrameResult logo_mip1 =
                    logo_renderer->render(std::span(&logo_mip1_draw, 1));
                check("generated logo PS implicit sample renders mip 1", logo_mip1.ok);
                if (logo_mip1.ok) {
                    check("logo PS CPU oracle matches mip 1 at center and boundary",
                          is_color(logo_mip1.rgba8, 32, 32, mip_color_for_scale(1.0F)) &&
                              is_color(logo_mip1.rgba8, 3, 32, mip_color_for_scale(1.0F)));
                    const char* png_path = logo_ps_synthetic_png_path;
                    if (png_path != nullptr && png_path[0] != '\0') {
                        if (!logo_texture_png_requested || logo_texture_output_paths_valid) {
                            check("generated logo PS PNG is written directly under local evidence",
                                  write_png_file(png_path, logo_mip1.png));
                        }
                    }
                }

                const std::array<float, 4> color_scale = {0.5F, 0.75F, 0.25F, 0.875F};
                sotc::AssetMeshDraw logo_scaled_mip1_draw =
                    make_logo_draw(1.0F, mip_texture, color_scale);
                sotc::MetalAssetFrameResult logo_scaled_mip1 =
                    logo_renderer->render(std::span(&logo_scaled_mip1_draw, 1));
                check("logo PS channel multiplies and fp16 RTZ export render", logo_scaled_mip1.ok);
                if (logo_scaled_mip1.ok) {
                    check("logo PS CPU oracle matches channel multiply and RTZ export",
                          is_color(logo_scaled_mip1.rgba8, 32, 32,
                                   logo_multiply_oracle(1.0F, color_scale)));
                }

                sotc::AssetMeshDraw logo_mip2_draw = make_logo_draw(2.0F, mip_texture);
                sotc::MetalAssetFrameResult logo_mip2 =
                    logo_renderer->render(std::span(&logo_mip2_draw, 1));
                check("generated logo PS implicit sample renders mip 2", logo_mip2.ok);
                if (logo_mip2.ok) {
                    check("logo PS CPU oracle observes UV-gradient LOD transition",
                          is_color(logo_mip2.rgba8, 32, 32, mip_color_for_scale(2.0F)) &&
                              is_color(logo_mip2.rgba8, 3, 32, mip_color_for_scale(2.0F)));
                    check("logo PS UV mutation changes mip-sensitive readback",
                          logo_mip2.rgba8 != logo_mip1.rgba8);
                }

                sotc::AssetMeshDraw logo_scaled_mip2_draw =
                    make_logo_draw(2.0F, mip_texture, color_scale);
                sotc::MetalAssetFrameResult logo_scaled_mip2 =
                    logo_renderer->render(std::span(&logo_scaled_mip2_draw, 1));
                check("logo PS channel multiply renders for mip 2", logo_scaled_mip2.ok);
                if (logo_scaled_mip2.ok) {
                    check("logo PS CPU oracle matches green-channel multiply at mip 2",
                          is_color(logo_scaled_mip2.rgba8, 32, 32,
                                   logo_multiply_oracle(2.0F, color_scale)));
                }

                sotc::AssetMeshDraw logo_mutated_draw = make_logo_draw(1.0F, mutated_mip_texture);
                sotc::MetalAssetFrameResult logo_mutated =
                    logo_renderer->render(std::span(&logo_mutated_draw, 1));
                check("generated logo PS reads a mutated selected mip", logo_mutated.ok);
                if (logo_mutated.ok) {
                    check("logo PS CPU oracle matches mutated mip at center",
                          is_color(logo_mutated.rgba8, 32, 32, mip_color_for_scale(1.0F, 1)));
                    check("mutating the selected mip changes generated shader readback",
                          logo_mutated.rgba8 != logo_mip1.rgba8);
                }

                sotc::AssetMeshDraw logo_scaled_mutated_draw =
                    make_logo_draw(1.0F, mutated_mip_texture, color_scale);
                sotc::MetalAssetFrameResult logo_scaled_mutated =
                    logo_renderer->render(std::span(&logo_scaled_mutated_draw, 1));
                check("logo PS channel multiply renders for mutated blue mip",
                      logo_scaled_mutated.ok);
                if (logo_scaled_mutated.ok) {
                    check("logo PS CPU oracle matches blue-channel multiply after RTZ export",
                          is_color(logo_scaled_mutated.rgba8, 32, 32,
                                   logo_multiply_oracle(1.0F, color_scale, 1)));
                }

                if (!logo_texture_png_requested) {
                    std::puts("logo PS asset PNG replay: NOT RUN (set SOTC_LOGO_PS_TEXTURE_PNG to the private imported PNG)");
                } else {
                    std::vector<std::uint8_t> png_bytes;
                    const bool png_read = read_binary_file(logo_texture_png_path, png_bytes);
                    check("logo PS PNG fixture is read from local evidence", png_read);
                    check("logo PS PNG fixture has the pinned 24,636-byte size",
                          png_read && png_bytes.size() == 24'636U);
                    const bool logo_png_pinned =
                        png_read && png_bytes.size() == 24'636U &&
                        sha256_hex(png_bytes) == kPinnedLogoPngSha256;
                    check("logo PS PNG fixture matches the pinned source SHA-256",
                          logo_png_pinned);
                    if (logo_png_pinned) {
                        constexpr std::uint32_t kLogoPngWidth = 228;
                        constexpr std::uint32_t kLogoPngHeight = 128;
                        std::vector<std::uint8_t> logo_png_rgba8;
                        std::string png_error;
                        const bool decoded = sotc::test::decode_asset_png_rgba8(
                            png_bytes, kLogoPngWidth, kLogoPngHeight, logo_png_rgba8, png_error);
                        check("ImageIO decodes pinned logo PNG to 228x128 RGBA8", decoded);
                        if (!decoded) {
                            std::fprintf(stderr, "FAIL logo PNG decode: %s\n", png_error.c_str());
                        } else {
                            check("decoded logo PNG has tightly packed RGBA8 bytes",
                                  logo_png_rgba8.size() ==
                                      static_cast<std::size_t>(kLogoPngWidth) * kLogoPngHeight * 4U);
                            const sotc::AssetSamplerInput asset_sampler{
                                sotc::AssetTextureFilter::nearest,
                                sotc::AssetTextureFilter::nearest,
                                sotc::AssetTextureAddressMode::clamp_to_edge,
                                sotc::AssetTextureAddressMode::clamp_to_edge};
                            constexpr std::array<LogoTextureProbe, 3> kLogoPngProbes = {{
                                {16, 20, 37, 29}, {46, 14, 171, 83}, {20, 46, 196, 101}}};
                            if (baseline_texture_png_path == nullptr ||
                                baseline_texture_png_path[0] == '\0' ||
                                mutation_texture_png_path == nullptr ||
                                mutation_texture_png_path[0] == '\0') {
                                check("logo PNG replay supplies direct baseline and mutation PNG output paths",
                                      false);
                            }

                            for (std::size_t probe_index = 0;
                                 probe_index < kLogoPngProbes.size(); ++probe_index) {
                                const LogoTextureProbe& probe = kLogoPngProbes[probe_index];
                                const LogoTextureUv uv = logo_texture_uv_for_probe(
                                    probe, kLogoPngWidth, kLogoPngHeight);
                                std::uint32_t oracle_x = 0;
                                std::uint32_t oracle_y = 0;
                                const auto expected = logo_png_pixel_oracle(
                                    logo_png_rgba8, kLogoPngWidth, kLogoPngHeight, uv,
                                    &oracle_x, &oracle_y);
                                check("synthetic UV lands on requested source texel center",
                                      oracle_x == probe.texture_x && oracle_y == probe.texture_y);

                                sotc::DefaultShaderCase asset_shader =
                                    sotc::make_default_shader_case();
                                asset_shader.material = {uv.scale_x, uv.scale_y, 0.0F, 1.0F};
                                asset_shader.param0_p0 = {1.0F, 1.0F, 1.0F, 1.0F};
                                sotc::AssetMeshDraw asset_draw{positions, indices, asset_shader};
                                asset_draw.fragment_texture0 = sotc::AssetTexture2D{
                                    kLogoPngWidth, kLogoPngHeight, logo_png_rgba8};
                                asset_draw.fragment_sampler0 = asset_sampler;
                                const sotc::MetalAssetFrameResult asset_frame =
                                    logo_renderer->render(std::span(&asset_draw, 1));
                                check("pinned logo PS samples the local PNG asset through slot 0",
                                      asset_frame.ok);
                                if (!asset_frame.ok) {
                                    std::fprintf(stderr, "FAIL logo PNG Metal render: %s\n",
                                                 asset_frame.error.c_str());
                                    continue;
                                }
                                logo_texture_png_ran = true;
                                check("logo PNG texel-center output matches fp16-RTZ CPU oracle",
                                      is_color(asset_frame.rgba8, probe.output_x, probe.output_y,
                                               expected));

                                if (probe_index == 0) {
                                    const sotc::MetalAssetFrameResult asset_repeat =
                                        logo_renderer->render(std::span(&asset_draw, 1));
                                    check("logo PNG fixture repeat renders", asset_repeat.ok);
                                    if (asset_repeat.ok) {
                                        check("logo PNG asset frame is byte deterministic",
                                              asset_repeat.rgba8 == asset_frame.rgba8 &&
                                                  asset_repeat.png == asset_frame.png);
                                    }
                                    if (logo_texture_output_paths_valid) {
                                        check("logo PNG baseline is written directly under local evidence",
                                              write_png_file(baseline_texture_png_path,
                                                             asset_frame.png));
                                    }

                                    std::vector<std::uint8_t> mutated_texture = logo_png_rgba8;
                                    const std::size_t texel_offset =
                                        (static_cast<std::size_t>(probe.texture_y) *
                                             kLogoPngWidth +
                                         probe.texture_x) *
                                        4U;
                                    mutated_texture[texel_offset] =
                                        mutated_texture[texel_offset] == 255U ? 0U : 255U;
                                    sotc::AssetMeshDraw mutated_asset_draw = asset_draw;
                                    mutated_asset_draw.fragment_texture0 = sotc::AssetTexture2D{
                                        kLogoPngWidth, kLogoPngHeight, mutated_texture};
                                    const sotc::MetalAssetFrameResult mutated_asset_frame =
                                        logo_renderer->render(std::span(&mutated_asset_draw, 1));
                                    check("logo PNG texel mutation renders", mutated_asset_frame.ok);
                                    if (mutated_asset_frame.ok) {
                                        const auto mutated_expected = logo_png_pixel_oracle(
                                            mutated_texture, kLogoPngWidth, kLogoPngHeight, uv);
                                        check("mutated logo PNG texel matches CPU oracle",
                                              is_color(mutated_asset_frame.rgba8, probe.output_x,
                                                       probe.output_y, mutated_expected));
                                        const std::size_t output_offset =
                                            pixel_offset(probe.output_x, probe.output_y);
                                        check("logo PNG texel mutation changes selected readback",
                                              !std::equal(
                                                  asset_frame.rgba8.begin() +
                                                      static_cast<std::ptrdiff_t>(output_offset),
                                                  asset_frame.rgba8.begin() +
                                                      static_cast<std::ptrdiff_t>(output_offset + 4U),
                                                  mutated_asset_frame.rgba8.begin() +
                                                      static_cast<std::ptrdiff_t>(output_offset)));
                                        check("logo PNG texel mutation changes output PNG bytes",
                                              mutated_asset_frame.png != asset_frame.png);
                                        if (logo_texture_output_paths_valid) {
                                            check("mutated logo PNG is written directly under local evidence",
                                                  write_png_file(mutation_texture_png_path,
                                                                 mutated_asset_frame.png));
                                        }
                                    }
                                }
                            }
                            if (logo_mesh_fixture_requested &&
                                logo_mesh_fixture_config_valid &&
                                logo_png_rgba8.size() ==
                                    static_cast<std::size_t>(kLogoPngWidth) *
                                        kLogoPngHeight * 4U) {
                                logo_mesh_fixture_passed = run_cmsh_logo_fixture(
                                    *logo_renderer, logo_cube_cmsh_path, logo_cone_cmsh_path,
                                    logo_mesh_output_png_path, logo_mesh_mutation_png_path,
                                    logo_png_rgba8, kLogoPngWidth, kLogoPngHeight);
                                std::printf("logo PS composite fixture: %s (asset-derived harness)\n",
                                            logo_mesh_fixture_passed ? "PASSED" : "FAILED");
                            }
                        }
                    }
                }
            } else {
                std::fprintf(stderr, "FAIL generated logo PS MSL pipeline: %s\n",
                             logo_error.c_str());
            }
        }
    }

    if (failures != 0) {
        std::fprintf(stderr, "metal_texture_binding checks: FAILED (%d)\n", failures);
        return 1;
    }
    std::printf("metal_texture_binding: synthetic RGBA8 sample produced %zu PNG bytes\n",
                baseline.png.size());
    if (logo_texture_png_ran) {
        std::puts("metal_texture_binding checks: PASSED (local PNG fixture through synthetic ABI; no retail binding claim)");
    } else {
        std::puts("metal_texture_binding checks: PASSED (synthetic texture; no game asset claim)");
    }
    return 0;
}
