#include "../platform/metal_draw_encoder.hpp"
#include "../platform/pm4_indirect.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::MetalDrawEncodeResult;
using sotc::pm4::DrawKind;
using sotc::pm4::GuestReader;
using sotc::pm4::OpcodeRule;
using sotc::pm4::Policy;
using sotc::pm4::traverse;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

constexpr std::uint32_t command(std::uint8_t opcode, std::uint32_t payload_dwords) {
    return (3u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) |
           ((static_cast<std::uint32_t>(opcode) & 0xFFu) << 8);
}

constexpr std::uint8_t kDrawAuto = 0x2D;
constexpr std::uint8_t kDrawIndex = 0x2E;
constexpr std::uint8_t kDrawOffset = 0x2F;
constexpr std::uint8_t kDrawIndirect = 0x30;
constexpr std::uint8_t kDispatch = 0x31;

constexpr OpcodeRule kRules[] = {
    {kDrawAuto, DrawKind::index_auto, 0, false, 0xFF, 0xFF},
    {kDrawIndex, DrawKind::index, 0, false, 0xFF, 0xFF},
    {kDrawOffset, DrawKind::index_offset, 0, false, 0xFF, 0xFF},
    {kDrawIndirect, DrawKind::index_indirect, 0, false, 0xFF, 0xFF},
    {kDispatch, DrawKind::dispatch, 0, false, 0xFF, 0xFF},
};
constexpr Policy kPolicy{kRules, sizeof(kRules) / sizeof(kRules[0])};

struct EmptyReader {
    static bool read(void*, std::uint64_t, std::uint32_t*, std::size_t) { return false; }
};

std::vector<sotc::pm4::DrawCommand> typed_draws() {
    // Two draws at the top level; a nested buffer adds two more after the first indirect slot.
    const std::uint32_t root[] = {command(kDrawAuto, 1), 7,
                                  command(kDrawIndex, 1), 9,
                                  command(kDrawOffset, 1), 0,
                                  command(kDispatch, 1), 4};
    const auto result = traverse(root, 8, 0x1000, kPolicy, GuestReader{&EmptyReader::read, nullptr});
    return result.draws;
}

std::uint8_t channel_at(const MetalDrawEncodeResult& result, std::uint32_t x, std::uint32_t y,
                        int channel) {
    const std::size_t offset = (static_cast<std::size_t>(y) * result.stride) + x * 4 +
                               static_cast<std::size_t>(channel);
    return result.rgba8[offset];
}

bool near(std::uint8_t value, int expected, int tolerance = 3) {
    return value >= expected - tolerance && value <= expected + tolerance;
}

void test_typed_draw_list_from_stream() {
    const auto draws = typed_draws();
    check("stream draws", draws.size() == 4);
    check("stream kinds", draws[0].kind == DrawKind::index_auto &&
                              draws[1].kind == DrawKind::index &&
                              draws[2].kind == DrawKind::index_offset &&
                              draws[3].kind == DrawKind::dispatch);
    check("stream counts", draws[0].count == 7 && draws[1].count == 9 && draws[2].count == 0 &&
                               draws[3].count == 4);
}

void test_encode_bands() {
    const auto draws = typed_draws();
    sotc::MetalDrawEncoderConfig config{};
    config.width = 64;
    config.height = 64;
    const MetalDrawEncodeResult result = sotc::metal_encode_draw_list(draws.data(), draws.size(),
                                                                      config);
    if (!result.ok) {
        check("no device skip", result.error == sotc::kMetalDrawEncoderNoDeviceError);
        if (result.error == sotc::kMetalDrawEncoderNoDeviceError) {
            std::printf("SKIP test_metal_draw_encoder: %s\n", result.error.c_str());
            std::exit(77);
        }
        std::fprintf(stderr, "encode error: %s\n", result.error.c_str());
        return;
    }

    check("encode stats", result.draws_requested == 4 && result.draws_encoded == 4 &&
                              result.draws_skipped == 0);
    check("encode size", result.width == 64 && result.height == 64 && result.stride == 256);
    check("encode pixels", result.rgba8.size() == 64u * 64u * 4u);

    // Bands are 16 rows each; sample the centre of every band.
    struct Probe {
        std::uint32_t y;
        int red;
        int green;
        int blue;
    };
    const Probe probes[] = {{8, 255, 0, 0}, {24, 0, 255, 0}, {40, 0, 0, 64}, {56, 255, 0, 255}};
    for (const Probe& probe : probes) {
        check("band red", near(channel_at(result, 32, probe.y, 0), probe.red));
        check("band green", near(channel_at(result, 32, probe.y, 1), probe.green));
        check("band blue", near(channel_at(result, 32, probe.y, 2), probe.blue));
        check("band alpha", channel_at(result, 32, probe.y, 3) == 255);
    }
}

void test_clear_only() {
    sotc::MetalDrawEncoderConfig config{};
    config.width = 32;
    config.height = 32;
    const MetalDrawEncodeResult result = sotc::metal_encode_draw_list(nullptr, 0, config);
    if (!result.ok) {
        check("clear skip", result.error == sotc::kMetalDrawEncoderNoDeviceError);
        return;
    }
    check("clear stats", result.draws_requested == 0 && result.draws_encoded == 0);
    bool all_black = true;
    for (std::size_t i = 0; i < result.rgba8.size(); i += 4) {
        if (result.rgba8[i] != 0 || result.rgba8[i + 1] != 0 || result.rgba8[i + 2] != 0 ||
            result.rgba8[i + 3] != 255) {
            all_black = false;
            break;
        }
    }
    check("clear black", all_black);
}

void test_skip_none_kind() {
    std::vector<sotc::pm4::DrawCommand> draws(2);
    draws[0].kind = DrawKind::none;
    draws[1].kind = DrawKind::index;
    draws[1].count = 3;
    sotc::MetalDrawEncoderConfig config{};
    config.width = 32;
    config.height = 32;
    const MetalDrawEncodeResult result = sotc::metal_encode_draw_list(draws.data(), draws.size(),
                                                                      config);
    if (!result.ok) {
        return;
    }
    check("skip stats", result.draws_requested == 2 && result.draws_encoded == 1 &&
                            result.draws_skipped == 1);
}

void test_rejects_bad_input() {
    sotc::MetalDrawEncoderConfig config{};
    const MetalDrawEncodeResult null_list = sotc::metal_encode_draw_list(nullptr, 3, config);
    check("null list rejected", !null_list.ok && !null_list.error.empty());
    config.width = 0;
    const MetalDrawEncodeResult zero = sotc::metal_encode_draw_list(nullptr, 0, config);
    check("zero target rejected", !zero.ok && !zero.error.empty());
}

// `--probe`: encode the canonical stream and print one line per band centre so an independent
// Python oracle can compare colours and statistics.
int probe_mode() {
    const auto draws = typed_draws();
    sotc::MetalDrawEncoderConfig config{};
    config.width = 64;
    config.height = 64;
    const MetalDrawEncodeResult result = sotc::metal_encode_draw_list(draws.data(), draws.size(),
                                                                      config);
    if (!result.ok) {
        std::printf("error %s\n", result.error.c_str());
        return result.error == sotc::kMetalDrawEncoderNoDeviceError ? 77 : 1;
    }
    std::printf("stats requested=%u encoded=%u skipped=%u\n", result.draws_requested,
                result.draws_encoded, result.draws_skipped);
    const std::uint32_t band_height = config.height / static_cast<std::uint32_t>(draws.size());
    for (std::size_t index = 0; index < draws.size(); ++index) {
        const std::uint32_t y = band_height * static_cast<std::uint32_t>(index) + band_height / 2;
        std::printf("band %zu kind=%s count=%u rgba=%u,%u,%u,%u\n", index,
                    sotc::pm4::draw_kind_name(draws[index].kind), draws[index].count,
                    channel_at(result, 32, y, 0), channel_at(result, 32, y, 1),
                    channel_at(result, 32, y, 2), channel_at(result, 32, y, 3));
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string_view(argv[1]) == "--probe") {
        return probe_mode();
    }
    test_typed_draw_list_from_stream();
    test_rejects_bad_input();
    test_encode_bands();
    test_clear_only();
    test_skip_none_kind();

    if (failures == 0) {
        std::printf("OK test_metal_draw_encoder\n");
        return 0;
    }
    std::fprintf(stderr, "test_metal_draw_encoder: %d failure(s)\n", failures);
    return 1;
}
