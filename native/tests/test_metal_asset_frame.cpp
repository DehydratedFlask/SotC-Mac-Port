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

}  // namespace

int main() {
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
