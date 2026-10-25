#include "../platform/metal_asset_frame.hpp"
#include "../platform/metal_capture.hpp"

#include <CommonCrypto/CommonDigest.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {

constexpr std::uint32_t kWidth = 256;
constexpr std::uint32_t kHeight = 256;
constexpr std::string_view kDefaultPairMslSha256 =
    "5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e";

struct Point2 {
    double x = 0.0;
    double y = 0.0;
};

struct Point3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct Projection {
    double min_x = 0.0;
    double max_x = 0.0;
    double min_y = 0.0;
    double max_y = 0.0;
    double depth = 0.0;
};

struct CpuMask {
    std::vector<std::uint8_t> covered;
    std::vector<double> depth;
    std::vector<int> front_draw;
    std::vector<int> front_triangle;
    std::size_t covered_count = 0;
};

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

[[nodiscard]] std::string sha256_hex(std::span<const std::uint8_t> bytes) {
    std::array<unsigned char, CC_SHA256_DIGEST_LENGTH> digest{};
    if (bytes.size() > std::numeric_limits<CC_LONG>::max()) return {};
    CC_SHA256(bytes.data(), static_cast<CC_LONG>(bytes.size()), digest.data());
    constexpr char hex[] = "0123456789abcdef";
    std::string output;
    output.reserve(digest.size() * 2);
    for (unsigned char byte : digest) {
        output.push_back(hex[byte >> 4]);
        output.push_back(hex[byte & 0x0f]);
    }
    return output;
}

[[nodiscard]] bool load_text(const char* path, std::string& contents) {
    if (path == nullptr || *path == '\0') return false;
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    contents.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return !contents.empty();
}

[[nodiscard]] bool path_is_under_local(const char* path) {
    if (path == nullptr || *path == '\0') return false;
    std::error_code error;
    const std::filesystem::path local_root = std::filesystem::weakly_canonical("local", error);
    if (error) return false;
    const std::filesystem::path candidate = std::filesystem::weakly_canonical(path, error);
    if (error) return false;
    const std::filesystem::path relative = candidate.lexically_relative(local_root);
    if (relative.empty() || relative == ".") return false;
    return *relative.begin() != "..";
}

[[nodiscard]] bool load_bytes(const char* path, std::vector<std::byte>& bytes) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    const std::vector<char> raw(std::istreambuf_iterator<char>(input), {});
    if (input.bad()) return false;
    bytes.resize(raw.size());
    for (std::size_t i = 0; i < raw.size(); ++i) {
        bytes[i] = static_cast<std::byte>(static_cast<unsigned char>(raw[i]));
    }
    return true;
}

[[nodiscard]] Point3 transform_position(const sotc::DefaultShaderCase& shader,
                                        const sotc::CmshPosition& position) {
    const double x = position.x;
    const double y = position.y;
    const double z = position.z;
    const double world[3] = {
        static_cast<double>(shader.object_row0[0]) * x +
            static_cast<double>(shader.object_row0[1]) * y +
            static_cast<double>(shader.object_row0[2]) * z + shader.object_row0[3],
        static_cast<double>(shader.object_row1[0]) * x +
            static_cast<double>(shader.object_row1[1]) * y +
            static_cast<double>(shader.object_row1[2]) * z + shader.object_row1[3],
        static_cast<double>(shader.object_row2[0]) * x +
            static_cast<double>(shader.object_row2[1]) * y +
            static_cast<double>(shader.object_row2[2]) * z + shader.object_row2[3],
    };
    const double clip[4] = {
        static_cast<double>(shader.clip_row0[0]) * world[0] +
            static_cast<double>(shader.clip_row0[1]) * world[1] +
            static_cast<double>(shader.clip_row0[2]) * world[2] + shader.clip_row0[3],
        static_cast<double>(shader.clip_row1[0]) * world[0] +
            static_cast<double>(shader.clip_row1[1]) * world[1] +
            static_cast<double>(shader.clip_row1[2]) * world[2] + shader.clip_row1[3],
        static_cast<double>(shader.clip_row2[0]) * world[0] +
            static_cast<double>(shader.clip_row2[1]) * world[1] +
            static_cast<double>(shader.clip_row2[2]) * world[2] + shader.clip_row2[3],
        static_cast<double>(shader.clip_row3[0]) * world[0] +
            static_cast<double>(shader.clip_row3[1]) * world[1] +
            static_cast<double>(shader.clip_row3[2]) * world[2] + shader.clip_row3[3],
    };
    if (clip[3] == 0.0) return {0.0, 0.0, 2.0};
    return {clip[0] / clip[3], clip[1] / clip[3], clip[2] / clip[3]};
}

[[nodiscard]] double edge(Point2 a, Point2 b, Point2 p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

[[nodiscard]] bool inside_triangle(Point2 a, Point2 b, Point2 c, Point2 p) {
    const double ab = edge(a, b, p);
    const double bc = edge(b, c, p);
    const double ca = edge(c, a, p);
    constexpr double epsilon = 1.0e-10;
    return (ab >= -epsilon && bc >= -epsilon && ca >= -epsilon) ||
           (ab <= epsilon && bc <= epsilon && ca <= epsilon);
}

[[nodiscard]] double interpolated_depth(Point2 a, Point2 b, Point2 c, Point2 p,
                                        double za, double zb, double zc) {
    const double area = edge(a, b, c);
    if (std::abs(area) < 1.0e-12) return 1.0;
    const double wa = edge(b, c, p) / area;
    const double wb = edge(c, a, p) / area;
    const double wc = edge(a, b, p) / area;
    return wa * za + wb * zb + wc * zc;
}

[[nodiscard]] CpuMask cpu_project(std::span<const sotc::AssetMeshDraw> draws, bool y_down) {
    const std::size_t pixel_count = static_cast<std::size_t>(kWidth) * kHeight;
    CpuMask mask{std::vector<std::uint8_t>(pixel_count, 0),
                 std::vector<double>(pixel_count, 1.0),
                 std::vector<int>(pixel_count, -1),
                 std::vector<int>(pixel_count, -1), 0};

    for (std::size_t draw_index = 0; draw_index < draws.size(); ++draw_index) {
        const auto& draw = draws[draw_index];
        for (std::size_t first = 0; first + 2 < draw.indices.size(); first += 3) {
            const sotc::CmshPosition& pa = draw.positions[draw.indices[first]];
            const sotc::CmshPosition& pb = draw.positions[draw.indices[first + 1]];
            const sotc::CmshPosition& pc = draw.positions[draw.indices[first + 2]];
            const Point3 a3 = transform_position(draw.shader, pa);
            const Point3 b3 = transform_position(draw.shader, pb);
            const Point3 c3 = transform_position(draw.shader, pc);
            const Point2 a{a3.x, a3.y};
            const Point2 b{b3.x, b3.y};
            const Point2 c{c3.x, c3.y};

            const double min_x = std::min({a.x, b.x, c.x});
            const double max_x = std::max({a.x, b.x, c.x});
            const double min_y = std::min({a.y, b.y, c.y});
            const double max_y = std::max({a.y, b.y, c.y});
            const int first_x = std::max(0, static_cast<int>(std::floor((min_x + 1.0) * 0.5 * kWidth)) - 1);
            const int last_x = std::min(static_cast<int>(kWidth) - 1,
                                        static_cast<int>(std::ceil((max_x + 1.0) * 0.5 * kWidth)) + 1);
            const double top_y = y_down ? -max_y : min_y;
            const double bottom_y = y_down ? -min_y : max_y;
            const int first_y = std::max(0, static_cast<int>(std::floor((top_y + 1.0) * 0.5 * kHeight)) - 1);
            const int last_y = std::min(static_cast<int>(kHeight) - 1,
                                        static_cast<int>(std::ceil((bottom_y + 1.0) * 0.5 * kHeight)) + 1);

            for (int y = first_y; y <= last_y; ++y) {
                for (int x = first_x; x <= last_x; ++x) {
                    const double ndc_x = 2.0 * (static_cast<double>(x) + 0.5) / kWidth - 1.0;
                    const double centered_y = 2.0 * (static_cast<double>(y) + 0.5) / kHeight - 1.0;
                    const double ndc_y = y_down ? -centered_y : centered_y;
                    const Point2 point{ndc_x, ndc_y};
                    if (!inside_triangle(a, b, c, point)) continue;

                    const double depth = interpolated_depth(a, b, c, point, a3.z, b3.z, c3.z);
                    if (depth < 0.0 || depth > 1.0) continue;
                    const std::size_t pixel = static_cast<std::size_t>(y) * kWidth +
                                              static_cast<std::size_t>(x);
                    if (mask.covered[pixel] == 0) ++mask.covered_count;
                    mask.covered[pixel] = 1;
                    if (depth < mask.depth[pixel]) {
                        mask.depth[pixel] = depth;
                        mask.front_draw[pixel] = static_cast<int>(draw_index);
                        mask.front_triangle[pixel] = static_cast<int>(first / 3);
                    }
                }
            }
        }
    }
    return mask;
}

void mask_matches(std::span<const std::uint8_t> rgba, const CpuMask& cpu,
                  std::size_t& differing_pixels) {
    differing_pixels = 0;
    for (std::size_t pixel = 0; pixel < cpu.covered.size(); ++pixel) {
        const std::size_t byte = pixel * 4;
        const bool actual = rgba[byte] != 0 || rgba[byte + 1] != 0 || rgba[byte + 2] != 0;
        if (actual != (cpu.covered[pixel] != 0)) ++differing_pixels;
    }
}

void fragment_classes_match(std::span<const std::uint8_t> rgba, const CpuMask& cpu,
                            std::size_t& differing_pixels) {
    differing_pixels = 0;
    for (std::size_t pixel = 0; pixel < cpu.front_draw.size(); ++pixel) {
        const std::size_t byte = pixel * 4;
        const std::uint8_t red = rgba[byte];
        const std::uint8_t green = rgba[byte + 1];
        const std::uint8_t blue = rgba[byte + 2];
        const int expected_draw = cpu.front_draw[pixel];
        const bool matches = expected_draw < 0
                                 ? red == 0 && green == 0 && blue == 0
                                 : expected_draw == 0
                                       ? red > green && red > 0
                                       : expected_draw == 1 && green > red && green > 0;
        if (!matches) ++differing_pixels;
    }
}

[[nodiscard]] bool write_png(const char* path, std::span<const std::uint8_t> png) {
    if (path == nullptr || *path == '\0') return false;
    const std::filesystem::path output(path);
    const std::filesystem::path absolute = std::filesystem::absolute(output);
    const std::filesystem::path local_root = std::filesystem::absolute("local");
    const std::string absolute_text = absolute.lexically_normal().string();
    const std::string local_text = local_root.lexically_normal().string();
    if (!absolute_text.starts_with(local_text + "/")) return false;
    std::filesystem::create_directories(absolute.parent_path());
    std::ofstream file(absolute, std::ios::binary | std::ios::trunc);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
    return file.good();
}

[[nodiscard]] std::vector<sotc::CmshPosition> make_back_quad() {
    return {{-0.72F, -0.72F, 0.15F}, {0.72F, -0.72F, 0.15F},
            {-0.72F, 0.72F, 0.15F}, {0.72F, 0.72F, 0.15F}};
}

[[nodiscard]] std::vector<sotc::CmshPosition> make_front_triangle() {
    return {{-0.38F, -0.31F, -0.10F}, {0.39F, -0.29F, -0.10F}, {0.04F, 0.42F, -0.10F}};
}

void append_u16(std::vector<std::byte>& bytes, std::uint16_t value) {
    bytes.push_back(static_cast<std::byte>(value & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 8) & 0xff));
}

void append_u32(std::vector<std::byte>& bytes, std::uint32_t value) {
    bytes.push_back(static_cast<std::byte>(value & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 8) & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 16) & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 24) & 0xff));
}

[[nodiscard]] std::vector<std::byte> make_synthetic_raw_cmsh(
    std::span<const sotc::CmshPosition> positions,
    std::span<const std::uint16_t> indices) {
    constexpr std::size_t kHeaderBytes = 0x4e;
    constexpr std::size_t kDescriptorBytes = 24;
    constexpr std::uint32_t kIndexMarker = 1;
    if (positions.empty() || indices.empty() || indices.size() % 3 != 0 ||
        positions.size() > std::numeric_limits<std::uint32_t>::max() / 12 ||
        indices.size() > std::numeric_limits<std::uint32_t>::max()) {
        return {};
    }

    std::vector<std::byte> bytes(kHeaderBytes, std::byte{0});
    for (char tag_byte : std::array<char, 4>{'0', 'S', 'O', 'P'}) {
        bytes.push_back(static_cast<std::byte>(tag_byte));
    }
    append_u16(bytes, 2);  // Synthetic POS0 float3 format used by the bounded parser fixture.
    append_u16(bytes, 2);  // Opaque descriptor flags.
    append_u32(bytes, 0);  // Stream offset relative to the computed data base.
    append_u32(bytes, 0);  // Opaque descriptor field.
    append_u32(bytes, static_cast<std::uint32_t>(positions.size() * 12));
    append_u32(bytes, 0);  // Opaque descriptor field.
    if (bytes.size() != kHeaderBytes + kDescriptorBytes) return {};
    bytes.insert(bytes.end(), 8, std::byte{0xff});

    for (const sotc::CmshPosition& position : positions) {
        append_u32(bytes, std::bit_cast<std::uint32_t>(position.x));
        append_u32(bytes, std::bit_cast<std::uint32_t>(position.y));
        append_u32(bytes, std::bit_cast<std::uint32_t>(position.z));
    }
    append_u32(bytes, kIndexMarker);
    append_u32(bytes, static_cast<std::uint32_t>(indices.size()));
    for (std::uint16_t index : indices) append_u16(bytes, index);
    return bytes;
}

[[nodiscard]] bool render_ok(sotc::MetalAssetFrameRenderer& renderer,
                             std::span<const sotc::AssetMeshDraw> draws,
                             sotc::MetalAssetFrameResult& result, std::string_view label) {
    result = renderer.render(draws);
    if (result.ok) return true;
    std::fprintf(stderr, "FAIL %.*s: %s\n", static_cast<int>(label.size()), label.data(),
                 result.error.c_str());
    return false;
}

struct MeshBounds {
    float min_x = std::numeric_limits<float>::infinity();
    float max_x = -std::numeric_limits<float>::infinity();
    float min_y = std::numeric_limits<float>::infinity();
    float max_y = -std::numeric_limits<float>::infinity();
    float min_z = std::numeric_limits<float>::infinity();
    float max_z = -std::numeric_limits<float>::infinity();
};

[[nodiscard]] bool mesh_bounds(const sotc::CmshMesh& mesh, MeshBounds& bounds) {
    if (mesh.positions.empty()) return false;
    for (const sotc::CmshPosition& position : mesh.positions) {
        bounds.min_x = std::min(bounds.min_x, position.x);
        bounds.max_x = std::max(bounds.max_x, position.x);
        bounds.min_y = std::min(bounds.min_y, position.y);
        bounds.max_y = std::max(bounds.max_y, position.y);
        bounds.min_z = std::min(bounds.min_z, position.z);
        bounds.max_z = std::max(bounds.max_z, position.z);
    }
    return std::isfinite(bounds.min_x) && std::isfinite(bounds.max_x) &&
           std::isfinite(bounds.min_y) && std::isfinite(bounds.max_y) &&
           std::isfinite(bounds.min_z) && std::isfinite(bounds.max_z) &&
           bounds.max_x > bounds.min_x && bounds.max_y > bounds.min_y;
}

[[nodiscard]] sotc::DefaultShaderCase asset_shader(const MeshBounds& bounds, float depth,
                                                   sotc::ShaderFloat4 material) {
    constexpr float kTargetSpan = 0.72F;
    const float x_span = bounds.max_x - bounds.min_x;
    const float y_span = bounds.max_y - bounds.min_y;
    const float xy_span = std::max(x_span, y_span);
    const float xy_scale = kTargetSpan / xy_span;
    const float z_span = bounds.max_z - bounds.min_z;
    const float z_scale = z_span > 0.0F ? 0.12F / z_span : 0.0F;
    const float center_x = (bounds.min_x + bounds.max_x) * 0.5F;
    const float center_y = (bounds.min_y + bounds.max_y) * 0.5F;
    const float center_z = (bounds.min_z + bounds.max_z) * 0.5F;

    sotc::DefaultShaderCase shader = sotc::make_default_shader_case();
    shader.encoded_normal = {0.5F, 0.5F, 1.0F, 0.0F};
    shader.object_row0 = {xy_scale, 0.0F, 0.0F, -center_x * xy_scale};
    shader.object_row1 = {0.0F, xy_scale, 0.0F, -center_y * xy_scale};
    shader.object_row2 = {0.0F, 0.0F, z_scale, -center_z * z_scale};
    shader.clip_row2[3] = depth;
    shader.material = material;
    return shader;
}

[[nodiscard]] bool parse_pinned_asset(const char* path, std::size_t expected_size,
                                      std::string_view expected_sha256,
                                      std::vector<std::byte>& raw, sotc::CmshMesh& mesh,
                                      std::string_view label) {
    check(std::string(label) + " input is inside local/", path_is_under_local(path));
    if (!path_is_under_local(path)) return false;
    check(std::string(label) + " bytes load", load_bytes(path, raw));
    if (raw.empty()) return false;
    const auto raw_u8 = std::span(reinterpret_cast<const std::uint8_t*>(raw.data()), raw.size());
    const std::string digest = sha256_hex(raw_u8);
    check(std::string(label) + " pinned byte count", raw.size() == expected_size);
    check(std::string(label) + " pinned SHA-256", digest == expected_sha256);
    if (raw.size() != expected_size || digest != expected_sha256) return false;

    const sotc::CmshParseResult result = sotc::parse_cmsh_mesh(raw);
    const auto* parsed = std::get_if<sotc::CmshMesh>(&result);
    if (parsed == nullptr) {
        const auto* parse_error = std::get_if<sotc::CmshParseError>(&result);
        std::fprintf(stderr, "FAIL %.*s parser: %s at byte %zu\n",
                     static_cast<int>(label.size()), label.data(),
                     parse_error == nullptr ? "unknown error"
                                            : sotc::cmsh_error_name(parse_error->code),
                     parse_error == nullptr ? 0 : parse_error->byte_offset);
        ++failures;
        return false;
    }
    mesh = *parsed;
    check(std::string(label) + " POS0/U16 mesh has triangles",
          !mesh.positions.empty() && !mesh.indices.empty() && mesh.indices.size() % 3 == 0);
    std::printf("asset-derived harness input %.*s: bytes=%zu sha256=%s positions=%zu indices=%zu\n",
                static_cast<int>(label.size()), label.data(), raw.size(), digest.c_str(),
                mesh.positions.size(), mesh.indices.size());
    return !mesh.positions.empty() && !mesh.indices.empty();
}

[[nodiscard]] bool run_asset_derived_frame(sotc::MetalAssetFrameRenderer& renderer,
                                           const char* cube_path, const char* cone_path,
                                           const char* png_path) {
    check("asset-derived inputs and PNG remain under local/",
          path_is_under_local(cube_path) && path_is_under_local(cone_path) &&
              path_is_under_local(png_path));
    if (!path_is_under_local(cube_path) || !path_is_under_local(cone_path) ||
        !path_is_under_local(png_path)) {
        return false;
    }

    constexpr std::string_view kCubeSha256 =
        "44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7";
    constexpr std::string_view kConeSha256 =
        "5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c";
    std::vector<std::byte> cube_bytes;
    std::vector<std::byte> cone_bytes;
    sotc::CmshMesh cube;
    sotc::CmshMesh cone;
    const bool cube_ok = parse_pinned_asset(cube_path, 850, kCubeSha256, cube_bytes, cube,
                                            "cube_1m.cmsh");
    const bool cone_ok = parse_pinned_asset(cone_path, 1462, kConeSha256, cone_bytes, cone,
                                            "cone.cmsh");
    if (!cube_ok || !cone_ok) return false;

    MeshBounds cube_bounds;
    MeshBounds cone_bounds;
    check("cube bounds are finite and nondegenerate", mesh_bounds(cube, cube_bounds));
    check("cone bounds are finite and nondegenerate", mesh_bounds(cone, cone_bounds));
    if (!mesh_bounds(cube, cube_bounds) || !mesh_bounds(cone, cone_bounds)) return false;

    const sotc::DefaultShaderCase cube_shader =
        asset_shader(cube_bounds, 0.74F, {1.0F, 0.0F, 0.0F, 1.0F});
    const sotc::DefaultShaderCase cone_shader =
        asset_shader(cone_bounds, 0.26F, {0.0F, 1.0F, 0.0F, 1.0F});
    std::vector<sotc::AssetMeshDraw> draws = {
        {cube.positions, cube.indices, cube_shader},
        {cone.positions, cone.indices, cone_shader},
    };

    sotc::MetalAssetFrameResult baseline;
    check("asset-derived harness Metal frame renders",
          render_ok(renderer, draws, baseline, "asset-derived harness"));
    if (!baseline.ok) return false;
    check("asset-derived frame has RGBA readback and PNG",
          baseline.width == kWidth && baseline.height == kHeight &&
              baseline.stride == kWidth * 4 &&
              baseline.rgba8.size() == static_cast<std::size_t>(baseline.stride) * kHeight &&
              !baseline.png.empty());
    if (baseline.png.empty() || !write_png(png_path, baseline.png)) {
        check("asset-derived PNG written under local/", false);
        return false;
    }

    const CpuMask cube_only = cpu_project(std::span(&draws[0], 1), true);
    const CpuMask cone_only = cpu_project(std::span(&draws[1], 1), true);
    const CpuMask expected = cpu_project(draws, true);
    std::size_t overlap_pixels = 0;
    for (std::size_t pixel = 0; pixel < cube_only.covered.size(); ++pixel) {
        if (cube_only.covered[pixel] != 0 && cone_only.covered[pixel] != 0) ++overlap_pixels;
    }
    check("asset silhouettes overlap enough to exercise depth order", overlap_pixels > 100);
    check("both parsed meshes cover pixels", cube_only.covered_count > 100 &&
                                                   cone_only.covered_count > 100);

    std::size_t silhouette_errors = 0;
    mask_matches(baseline.rgba8, expected, silhouette_errors);
    const std::size_t edge_allowance = (static_cast<std::size_t>(kWidth) + kHeight) * 2;
    check("asset frame covered-pixel count and CPU silhouette match",
          expected.covered_count > 1000 && silhouette_errors <= edge_allowance);
    std::size_t depth_errors = 0;
    fragment_classes_match(baseline.rgba8, expected, depth_errors);
    check("asset frame depth selects the nearer parsed cone in overlap",
          overlap_pixels > 100 && depth_errors == 0);

    int min_x = static_cast<int>(kWidth);
    int min_y = static_cast<int>(kHeight);
    int max_x = -1;
    int max_y = -1;
    int projected_min_x = static_cast<int>(kWidth);
    int projected_min_y = static_cast<int>(kHeight);
    int projected_max_x = -1;
    int projected_max_y = -1;
    for (std::size_t pixel = 0; pixel < expected.covered.size(); ++pixel) {
        if (expected.covered[pixel] == 0) continue;
        const int x = static_cast<int>(pixel % kWidth);
        const int y = static_cast<int>(pixel / kWidth);
        projected_min_x = std::min(projected_min_x, x);
        projected_max_x = std::max(projected_max_x, x);
        projected_min_y = std::min(projected_min_y, y);
        projected_max_y = std::max(projected_max_y, y);
    }
    for (std::uint32_t y = 0; y < kHeight; ++y) {
        for (std::uint32_t x = 0; x < kWidth; ++x) {
            const std::size_t pixel = (static_cast<std::size_t>(y) * kWidth + x) * 4;
            if (baseline.rgba8[pixel] == 0 && baseline.rgba8[pixel + 1] == 0 &&
                baseline.rgba8[pixel + 2] == 0) continue;
            min_x = std::min(min_x, static_cast<int>(x));
            max_x = std::max(max_x, static_cast<int>(x));
            min_y = std::min(min_y, static_cast<int>(y));
            max_y = std::max(max_y, static_cast<int>(y));
        }
    }
    check("asset-rendered pixels stay inside the CPU-projected bounds",
          min_x == projected_min_x && max_x == projected_max_x &&
              min_y == projected_min_y && max_y == projected_max_y);

    sotc::MetalAssetFrameResult repeated;
    const bool repeated_ok = render_ok(renderer, draws, repeated, "asset-derived repeat");
    check("asset-derived frame renders twice", repeated_ok);
    if (repeated_ok) {
        check("asset-derived RGBA and PNG are deterministic",
              repeated.rgba8 == baseline.rgba8 && repeated.png == baseline.png);
    }

    const std::string baseline_hash = sha256_hex(baseline.png);
    check("asset-derived PNG SHA-256 computed", !baseline_hash.empty());
    std::printf("asset-derived harness frame: PNG=%s bytes=%zu sha256=%s; covered=%zu; overlap=%zu; "
                "silhouette-errors=%zu; depth-errors=%zu\n",
                png_path, baseline.png.size(), baseline_hash.c_str(), expected.covered_count,
                overlap_pixels, silhouette_errors, depth_errors);

    std::vector<sotc::CmshPosition> moved_cone_positions = cone.positions;
    const std::size_t moved_vertex = cone.indices.front();
    moved_cone_positions[moved_vertex].x += (cone_bounds.max_x - cone_bounds.min_x) * 0.12F;
    std::vector<sotc::AssetMeshDraw> vertex_mutation = draws;
    vertex_mutation[1].positions = moved_cone_positions;
    sotc::MetalAssetFrameResult moved;
    check("asset vertex mutation renders",
          render_ok(renderer, vertex_mutation, moved, "asset vertex mutation"));
    if (moved.ok) check("asset vertex mutation changes PNG hash",
                        sha256_hex(moved.png) != baseline_hash);

    bool index_mutation_visible = false;
    std::size_t changed_index_slot = 0;
    std::size_t changed_draw_index = 0;
    std::uint16_t changed_index_from = 0;
    std::uint16_t changed_index_to = 0;
    std::size_t changed_cpu_pixels = 0;
    std::size_t cpu_mutation_candidates = 0;
    for (std::size_t draw_index = 0; draw_index < draws.size() && !index_mutation_visible;
         ++draw_index) {
        const sotc::CmshMesh& mesh = draw_index == 0 ? cube : cone;
        const sotc::DefaultShaderCase& shader = draws[draw_index].shader;
        std::vector<std::pair<std::size_t, std::size_t>> visible_triangles;
        visible_triangles.reserve(mesh.indices.size() / 3);
        for (std::size_t triangle = 0; triangle < mesh.indices.size() / 3; ++triangle) {
            std::size_t visible_pixels = 0;
            for (std::size_t pixel = 0; pixel < expected.front_draw.size(); ++pixel) {
                if (expected.front_draw[pixel] == static_cast<int>(draw_index) &&
                    expected.front_triangle[pixel] == static_cast<int>(triangle)) {
                    ++visible_pixels;
                }
            }
            if (visible_pixels != 0) visible_triangles.emplace_back(visible_pixels, triangle);
        }
        std::sort(visible_triangles.begin(), visible_triangles.end(),
                  [](const auto& left, const auto& right) { return left.first > right.first; });

        const std::size_t triangle_probe_count =
            std::min<std::size_t>(8, visible_triangles.size());
        for (std::size_t probe = 0; probe < triangle_probe_count && !index_mutation_visible;
             ++probe) {
            const std::size_t triangle = visible_triangles[probe].second;
            for (std::size_t corner = 0; corner < 3 && !index_mutation_visible; ++corner) {
                const std::size_t slot = triangle * 3 + corner;
                const std::uint16_t original = mesh.indices[slot];
                const Point3 source = transform_position(shader, mesh.positions[original]);
                std::vector<std::pair<double, std::uint16_t>> distant_vertices;
                distant_vertices.reserve(mesh.positions.size());
                for (std::size_t vertex = 0; vertex < mesh.positions.size() &&
                                             vertex <= std::numeric_limits<std::uint16_t>::max();
                     ++vertex) {
                    const auto index = static_cast<std::uint16_t>(vertex);
                    if (index == original || index == mesh.indices[triangle * 3 + (corner + 1) % 3] ||
                        index == mesh.indices[triangle * 3 + (corner + 2) % 3]) {
                        continue;
                    }
                    const Point3 projected = transform_position(shader, mesh.positions[vertex]);
                    const double dx = projected.x - source.x;
                    const double dy = projected.y - source.y;
                    distant_vertices.emplace_back(dx * dx + dy * dy, index);
                }
                std::sort(distant_vertices.begin(), distant_vertices.end(),
                          [](const auto& left, const auto& right) {
                              return left.first > right.first;
                          });
                const std::size_t candidate_count =
                    std::min<std::size_t>(4, distant_vertices.size());
                for (std::size_t candidate = 0; candidate < candidate_count &&
                                                !index_mutation_visible; ++candidate) {
                    const std::uint16_t replacement = distant_vertices[candidate].second;
                    std::vector<std::uint16_t> changed_indices = mesh.indices;
                    changed_indices[slot] = replacement;
                    std::vector<sotc::AssetMeshDraw> index_mutation = draws;
                    index_mutation[draw_index].indices = changed_indices;
                    const CpuMask candidate_projection = cpu_project(index_mutation, true);
                    std::size_t cpu_delta = 0;
                    for (std::size_t pixel = 0; pixel < expected.covered.size(); ++pixel) {
                        if (expected.covered[pixel] != candidate_projection.covered[pixel] ||
                            expected.front_draw[pixel] != candidate_projection.front_draw[pixel]) {
                            ++cpu_delta;
                        }
                    }
                    if (cpu_delta == 0) continue;
                    ++cpu_mutation_candidates;

                    sotc::MetalAssetFrameResult changed;
                    if (render_ok(renderer, index_mutation, changed, "asset index mutation")) {
                        index_mutation_visible = sha256_hex(changed.png) != baseline_hash;
                        if (index_mutation_visible) {
                            changed_draw_index = draw_index;
                            changed_index_slot = slot;
                            changed_index_from = original;
                            changed_index_to = replacement;
                            changed_cpu_pixels = cpu_delta;
                        }
                    }
                }
            }
        }
    }
    if (index_mutation_visible) {
        std::printf("asset index mutation: draw=%zu slot=%zu from=%u to=%u; CPU pixels changed=%zu\n",
                    changed_draw_index, changed_index_slot,
                    static_cast<unsigned>(changed_index_from), static_cast<unsigned>(changed_index_to),
                    changed_cpu_pixels);
    } else {
        std::printf("asset index mutation search: CPU-visible candidates=%zu\n",
                    cpu_mutation_candidates);
    }
    check("asset index mutation changes PNG hash", index_mutation_visible);

    std::vector<sotc::AssetMeshDraw> constant_mutation = draws;
    constant_mutation[1].shader.material[1] = 0.35F;
    sotc::MetalAssetFrameResult changed_constant;
    check("asset shader-constant mutation renders",
          render_ok(renderer, constant_mutation, changed_constant, "asset shader constant mutation"));
    if (changed_constant.ok) check("asset shader-constant mutation changes PNG hash",
                                   sha256_hex(changed_constant.png) != baseline_hash);

    return failures == 0;
}

}  // namespace

int main(int argc, char** argv) {
    const bool asset_derived_mode = argc == 5 && std::string_view(argv[1]) == "--asset-derived";
    if (argc != 1 && !asset_derived_mode) {
        std::fprintf(stderr, "usage: %s [--asset-derived CUBE_CMSH CONE_CMSH OUTPUT_PNG]\n", argv[0]);
        return 2;
    }
    std::string msl_source;
    const char* msl_path = std::getenv("SOTC_DEFAULT_PAIR_MSL");
    if (!load_text(msl_path, msl_source)) {
        std::fprintf(stderr, "SKIP: set SOTC_DEFAULT_PAIR_MSL to the private generated default-pair MSL\n");
        return 77;
    }
    const auto msl_bytes = std::span(
        reinterpret_cast<const std::uint8_t*>(msl_source.data()), msl_source.size());
    const std::string msl_hash = sha256_hex(msl_bytes);
    if (msl_hash != kDefaultPairMslSha256) {
        std::fprintf(stderr,
                     "FAIL private MSL SHA-256: got %s expected %.*s\n",
                     msl_hash.c_str(), static_cast<int>(kDefaultPairMslSha256.size()),
                     kDefaultPairMslSha256.data());
        return 1;
    }

    sotc::MetalAssetFrameConfig config{kWidth, kHeight};
    std::string error;
    std::unique_ptr<sotc::MetalAssetFrameRenderer> renderer =
        sotc::MetalAssetFrameRenderer::create(config, msl_source, error);
    if (renderer == nullptr) {
        if (error == sotc::kMetalCaptureNoDeviceError) {
            std::fprintf(stderr, "SKIP: %s\n", error.c_str());
            return 77;
        }
        std::fprintf(stderr, "FAIL MetalAssetFrameRenderer::create: %s\n", error.c_str());
        return 1;
    }

    if (asset_derived_mode) {
        if (!run_asset_derived_frame(*renderer, argv[2], argv[3], argv[4])) {
            std::fprintf(stderr, "asset-derived harness checks: FAILED (%d)\n", failures);
            return 1;
        }
        std::puts("asset-derived harness checks: PASSED (raw game meshes through native parser and direct Metal; no game frame)");
        return 0;
    }

    std::vector<sotc::CmshPosition> back_positions = make_back_quad();
    const std::vector<std::uint16_t> back_indices = {0, 1, 2, 2, 1, 3};
    std::vector<sotc::CmshPosition> front_positions = make_front_triangle();
    const std::vector<std::uint16_t> front_indices = {0, 1, 2};
    sotc::DefaultShaderCase back_shader = sotc::make_default_shader_case();
    back_shader.clip_row2[3] = 0.50F;
    back_shader.material = {1.0F, 0.0F, 0.0F, 1.0F};
    sotc::DefaultShaderCase front_shader = sotc::make_default_shader_case();
    front_shader.clip_row2[3] = 0.50F;
    front_shader.material = {0.0F, 1.0F, 0.0F, 1.0F};

    std::vector<sotc::AssetMeshDraw> draws = {
        {back_positions, back_indices, back_shader},
        {front_positions, front_indices, front_shader},
    };

    sotc::MetalAssetFrameResult baseline;
    check("baseline typed Metal frame renders",
          render_ok(*renderer, draws, baseline, "baseline asset-frame harness"));
    if (!baseline.ok) return 1;
    check("readback dimensions and RGBA stride", baseline.width == kWidth &&
              baseline.height == kHeight && baseline.stride == kWidth * 4 &&
              baseline.rgba8.size() == static_cast<std::size_t>(baseline.stride) * kHeight);
    check("ImageIO PNG produced", !baseline.png.empty());

    const CpuMask cpu_y_down = cpu_project(draws, true);
    const CpuMask cpu_y_up = cpu_project(draws, false);
    std::size_t errors_y_down = 0;
    std::size_t errors_y_up = 0;
    mask_matches(baseline.rgba8, cpu_y_down, errors_y_down);
    mask_matches(baseline.rgba8, cpu_y_up, errors_y_up);
    const std::size_t edge_allowance = (static_cast<std::size_t>(kWidth) + kHeight) * 2;
    const std::size_t best_mask_errors = std::min(errors_y_down, errors_y_up);
    check("CPU-projected silhouette and covered-pixel count",
          best_mask_errors <= edge_allowance && cpu_y_down.covered_count > 1000 &&
              cpu_y_up.covered_count > 1000);

    std::size_t class_errors_y_down = 0;
    std::size_t class_errors_y_up = 0;
    fragment_classes_match(baseline.rgba8, cpu_y_down, class_errors_y_down);
    fragment_classes_match(baseline.rgba8, cpu_y_up, class_errors_y_up);
    check("CPU projection predicts per-pixel nearer draw and triangle silhouette",
          class_errors_y_down == 0);

    std::size_t observed_pixels = 0;
    int min_x = static_cast<int>(kWidth);
    int min_y = static_cast<int>(kHeight);
    int max_x = -1;
    int max_y = -1;
    bool blue_seen = false;
    for (std::uint32_t y = 0; y < kHeight; ++y) {
        for (std::uint32_t x = 0; x < kWidth; ++x) {
            const std::size_t pixel = (static_cast<std::size_t>(y) * kWidth + x) * 4;
            const bool covered = baseline.rgba8[pixel] != 0 || baseline.rgba8[pixel + 1] != 0 ||
                                 baseline.rgba8[pixel + 2] != 0;
            if (!covered) continue;
            ++observed_pixels;
            min_x = std::min(min_x, static_cast<int>(x));
            max_x = std::max(max_x, static_cast<int>(x));
            min_y = std::min(min_y, static_cast<int>(y));
            max_y = std::max(max_y, static_cast<int>(y));
            blue_seen = blue_seen || baseline.rgba8[pixel + 2] != 0;
        }
    }

    const CpuMask& selected_cpu = cpu_y_down;
    int projected_min_x = static_cast<int>(kWidth);
    int projected_min_y = static_cast<int>(kHeight);
    int projected_max_x = -1;
    int projected_max_y = -1;
    for (std::size_t pixel = 0; pixel < selected_cpu.covered.size(); ++pixel) {
        if (selected_cpu.covered[pixel] == 0) continue;
        const int x = static_cast<int>(pixel % kWidth);
        const int y = static_cast<int>(pixel / kWidth);
        projected_min_x = std::min(projected_min_x, x);
        projected_max_x = std::max(projected_max_x, x);
        projected_min_y = std::min(projected_min_y, y);
        projected_max_y = std::max(projected_max_y, y);
    }
    check("covered pixel count follows CPU projection",
          observed_pixels > 1000 &&
              std::abs(static_cast<long long>(observed_pixels) -
                       static_cast<long long>(cpu_y_down.covered_count)) <=
                  static_cast<long long>(edge_allowance));
    check("rendered pixels stay inside CPU-projected bounds",
          min_x == projected_min_x && max_x == projected_max_x &&
              min_y == projected_min_y && max_y == projected_max_y);
    check("depth test selects the nearer green triangle at center",
          baseline.rgba8[(static_cast<std::size_t>(128) * kWidth + 128) * 4 + 1] > 0 &&
              baseline.rgba8[(static_cast<std::size_t>(128) * kWidth + 128) * 4 + 0] == 0);
    check("blue channel remains zero for red/green fixture materials", !blue_seen);

    sotc::MetalAssetFrameResult repeated;
    check("repeat frame renders", render_ok(*renderer, draws, repeated, "repeat frame"));
    if (repeated.ok) {
        check("repeat RGBA and PNG are deterministic",
              repeated.rgba8 == baseline.rgba8 && repeated.png == baseline.png);
    }

    std::string baseline_hash = sha256_hex(baseline.png);
    check("baseline PNG SHA-256 computed", !baseline_hash.empty());

    const std::vector<std::byte> synthetic_cmsh =
        make_synthetic_raw_cmsh(back_positions, back_indices);
    check("synthetic raw .cmsh bytes serialized", !synthetic_cmsh.empty());
    const sotc::CmshParseResult parsed_result = sotc::parse_cmsh_mesh(synthetic_cmsh);
    const auto* parsed_mesh = std::get_if<sotc::CmshMesh>(&parsed_result);
    check("synthetic raw .cmsh parsed", parsed_mesh != nullptr);
    if (parsed_mesh != nullptr) {
        bool parsed_geometry_matches = parsed_mesh->positions.size() == back_positions.size() &&
                                       parsed_mesh->indices == back_indices;
        if (parsed_geometry_matches) {
            for (std::size_t i = 0; i < back_positions.size(); ++i) {
                const sotc::CmshPosition& parsed = parsed_mesh->positions[i];
                const sotc::CmshPosition& expected = back_positions[i];
                parsed_geometry_matches = parsed_geometry_matches && parsed.x == expected.x &&
                                          parsed.y == expected.y && parsed.z == expected.z;
            }
        }
        check("parser returns the serialized positions and U16 indices", parsed_geometry_matches);

        const sotc::AssetMeshDraw hand_built_draw{back_positions, back_indices, back_shader};
        const sotc::AssetMeshDraw parser_draw{parsed_mesh->positions, parsed_mesh->indices,
                                              back_shader};
        sotc::MetalAssetFrameResult hand_built_result;
        sotc::MetalAssetFrameResult parser_result;
        const bool hand_built_rendered = render_ok(*renderer, std::span(&hand_built_draw, 1),
                                                   hand_built_result, "hand-built reference draw");
        const bool parser_rendered = render_ok(*renderer, std::span(&parser_draw, 1),
                                               parser_result, "synthetic parser-to-Metal draw");
        check("hand-built and parser-produced draws render", hand_built_rendered && parser_rendered);
        if (hand_built_rendered && parser_rendered) {
            check("synthetic parser-to-Metal RGBA and PNG match the direct draw",
                  parser_result.rgba8 == hand_built_result.rgba8 &&
                      parser_result.png == hand_built_result.png);
            std::printf("synthetic parser-to-Metal flow: bytes=%zu PNG bytes=%zu sha256=%s\n",
                        synthetic_cmsh.size(), parser_result.png.size(),
                        sha256_hex(parser_result.png).c_str());
        }
    }

    std::printf("metal_asset_frame: synthetic harness MSL sha256=%s; PNG bytes=%zu sha256=%s; "
                "coverage=%zu; CPU mask errors yDown=%zu yUp=%zu; per-pixel draw errors="
                "yDown:%zu yUp:%zu\n",
                msl_hash.c_str(),
                baseline.png.size(), baseline_hash.c_str(), observed_pixels,
                errors_y_down, errors_y_up, class_errors_y_down, class_errors_y_up);

    const char* dump_path = std::getenv("SOTC_ASSET_FRAME_DUMP");
    if (dump_path != nullptr && *dump_path != '\0') {
        check("private PNG artifact written under local/", write_png(dump_path, baseline.png));
    }

    std::vector<sotc::CmshPosition> moved_positions = back_positions;
    moved_positions[3].x -= 0.12F;
    std::vector<sotc::AssetMeshDraw> vertex_mutation = draws;
    vertex_mutation[0].positions = moved_positions;
    sotc::MetalAssetFrameResult moved;
    check("vertex mutation render", render_ok(*renderer, vertex_mutation, moved, "vertex mutation"));
    if (moved.ok) check("vertex mutation changes PNG hash", sha256_hex(moved.png) != baseline_hash);

    std::vector<std::uint16_t> changed_indices = back_indices;
    changed_indices[2] = 0;
    std::vector<sotc::AssetMeshDraw> index_mutation = draws;
    index_mutation[0].indices = changed_indices;
    sotc::MetalAssetFrameResult changed_index;
    check("index mutation render", render_ok(*renderer, index_mutation, changed_index, "index mutation"));
    if (changed_index.ok) check("index mutation changes PNG hash",
                                sha256_hex(changed_index.png) != baseline_hash);

    std::vector<sotc::AssetMeshDraw> constant_mutation = draws;
    constant_mutation[0].shader.material[0] = 0.65F;
    sotc::MetalAssetFrameResult changed_constant;
    check("shader constant mutation render",
          render_ok(*renderer, constant_mutation, changed_constant, "shader constant mutation"));
    if (changed_constant.ok) check("shader constant mutation changes PNG hash",
                                   sha256_hex(changed_constant.png) != baseline_hash);

    if (failures != 0) {
        std::fprintf(stderr, "metal_asset_frame checks: FAILED (%d)\n", failures);
        return 1;
    }
    std::puts("metal_asset_frame checks: PASSED (synthetic geometry; no game frame)");
    return 0;
}
