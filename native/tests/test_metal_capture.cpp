// Fixture for the direct-Metal offscreen capture provider (GPU-01-METAL-CAPTURE-PROVIDER-01).
//
// Scope: this proves the provider renders its own deterministic MSL pattern into an offscreen
// Metal texture, reads it back as tight RGBA8, PNG-encodes it, and can be wired into
// `attach_screenshot`. It proves nothing about game frames, scenes, assets or colossus rendering;
// G3 remains NOT RUN.
//
// If the host has no Metal device the test prints `SKIP: <reason>` and returns 77 (registered as
// SKIP_RETURN_CODE in native/CMakeLists.txt). Every other provider failure fails the test.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "core/screenshot_contract.hpp"
#include "platform/metal_capture.hpp"

using namespace sotc;

namespace {

int g_checks = 0;

#define CHECK(condition)                                                              \
    do {                                                                              \
        ++g_checks;                                                                   \
        if (!(condition)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #condition);  \
            std::exit(1);                                                             \
        }                                                                             \
    } while (0)

struct Sample {
    std::uint32_t x;
    std::uint32_t y;
};

// Corners plus interior points. 639/359 exercise the 8-bit channel wrap (639 & 0xff == 127).
constexpr Sample kSamples[] = {
    {0, 0}, {17, 5}, {255, 0}, {0, 255}, {320, 180}, {639, 359},
};

// Independent statement of the header's pattern contract.
std::array<std::uint8_t, 4> expected_pixel(std::uint32_t x, std::uint32_t y,
                                           std::uint32_t frame_num) {
    return {static_cast<std::uint8_t>(x & 0xFFu), static_cast<std::uint8_t>(y & 0xFFu),
            static_cast<std::uint8_t>(64u + (frame_num & 0x3Fu)), 255};
}

std::size_t pixel_index(const MetalCaptureResult& result, std::uint32_t x, std::uint32_t y) {
    return (static_cast<std::size_t>(y) * result.width + x) * 4;
}

void check_pixel(const MetalCaptureResult& result, std::uint32_t x, std::uint32_t y) {
    ++g_checks;
    const std::size_t index = pixel_index(result, x, y);
    if (index + 4 > result.rgba8.size()) {
        std::fprintf(stderr, "FAIL pixel (%u,%u) outside %zu-byte buffer\n", x, y,
                     result.rgba8.size());
        std::exit(1);
    }
    const auto expected = expected_pixel(x, y, result.frame_num);
    for (std::size_t channel = 0; channel < 4; ++channel) {
        if (result.rgba8[index + channel] != expected[channel]) {
            std::fprintf(stderr,
                         "FAIL pixel (%u,%u) channel %zu: got %02x expected %02x (frame %u)\n", x,
                         y, channel, result.rgba8[index + channel], expected[channel],
                         result.frame_num);
            std::exit(1);
        }
    }
}

void check_alpha_opaque(const MetalCaptureResult& result) {
    ++g_checks;
    for (std::size_t index = 3; index < result.rgba8.size(); index += 4) {
        if (result.rgba8[index] != 255) {
            std::fprintf(stderr, "FAIL alpha at byte %zu is %u, expected 255\n", index,
                         result.rgba8[index]);
            std::exit(1);
        }
    }
}

void print_pixel(std::FILE* out, const MetalCaptureResult& result, std::uint32_t x,
                 std::uint32_t y) {
    const std::size_t index = pixel_index(result, x, y);
    std::fprintf(out, " (%u,%u)=%02x%02x%02x%02x", x, y, result.rgba8[index],
                 result.rgba8[index + 1], result.rgba8[index + 2], result.rgba8[index + 3]);
}

struct PngInfo {
    bool signature = false;
    bool first_chunk_ihdr = false;
    bool iend = false;
    bool ends_at_iend = false;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint8_t bit_depth = 0;
    std::uint8_t colour_type = 0;
    std::uint8_t compression = 0;
    std::uint8_t filter = 0;
    std::uint8_t interlace = 0;
    std::size_t idat_bytes = 0;
};

std::uint32_t read_be32(const std::uint8_t* bytes) {
    return (static_cast<std::uint32_t>(bytes[0]) << 24) |
           (static_cast<std::uint32_t>(bytes[1]) << 16) |
           (static_cast<std::uint32_t>(bytes[2]) << 8) | static_cast<std::uint32_t>(bytes[3]);
}

// Decode-free PNG structure walk: signature, first chunk IHDR (13 bytes), IDAT presence, IEND at
// the exact end of the buffer. CRCs are not recomputed here; the test only asserts the documented
// header fields and closure.
bool parse_png(const std::vector<std::uint8_t>& png, PngInfo& info) {
    static constexpr std::uint8_t kSignature[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    if (png.size() < sizeof(kSignature) + 12 + 13 + 12) {
        return false;
    }
    info.signature = std::memcmp(png.data(), kSignature, sizeof(kSignature)) == 0;
    if (!info.signature) {
        return false;
    }

    std::size_t offset = sizeof(kSignature);
    bool first = true;
    while (offset + 12 <= png.size()) {
        const std::uint32_t length = read_be32(png.data() + offset);
        const std::uint8_t* type = png.data() + offset + 4;
        if (offset + 12 + static_cast<std::size_t>(length) > png.size()) {
            return false;
        }
        const bool is_ihdr = std::memcmp(type, "IHDR", 4) == 0;
        const bool is_idat = std::memcmp(type, "IDAT", 4) == 0;
        const bool is_iend = std::memcmp(type, "IEND", 4) == 0;
        if (first) {
            info.first_chunk_ihdr = is_ihdr;
            first = false;
            if (!is_ihdr || length != 13) {
                return false;
            }
            const std::uint8_t* data = png.data() + offset + 8;
            info.width = read_be32(data);
            info.height = read_be32(data + 4);
            info.bit_depth = data[8];
            info.colour_type = data[9];
            info.compression = data[10];
            info.filter = data[11];
            info.interlace = data[12];
        }
        if (is_idat) {
            info.idat_bytes += length;
        }
        offset += 12 + static_cast<std::size_t>(length);
        if (is_iend) {
            info.iend = true;
            info.ends_at_iend = offset == png.size();
            break;
        }
    }
    return info.signature && info.first_chunk_ihdr && info.iend && info.ends_at_iend;
}

void check_png(const std::vector<std::uint8_t>& png, std::uint32_t width, std::uint32_t height) {
    PngInfo info;
    CHECK(parse_png(png, info));
    CHECK(info.signature);
    CHECK(info.first_chunk_ihdr);
    CHECK(info.width == width);
    CHECK(info.height == height);
    CHECK(info.bit_depth == 8);
    CHECK(info.colour_type == 6);  // 8-bit RGBA
    CHECK(info.compression == 0);
    CHECK(info.filter == 0);
    CHECK(info.interlace == 0);
    CHECK(info.iend);
    CHECK(info.ends_at_iend);
    CHECK(info.idat_bytes > 0);
}

void check_result_shape(const MetalCaptureResult& result, std::uint32_t width,
                        std::uint32_t height, std::uint32_t frame_num) {
    CHECK(result.ok);
    CHECK(result.error.empty());
    CHECK(result.width == width);
    CHECK(result.height == height);
    CHECK(result.stride == width * 4);
    CHECK(result.frame_num == frame_num);
    CHECK(result.rgba8.size() == static_cast<std::size_t>(result.stride) * result.height);
    CHECK(!result.png.empty());
}

void dump_png_if_requested(const std::vector<std::uint8_t>& png) {
    const char* path = std::getenv("SOTC_METAL_CAPTURE_DUMP");
    if (path == nullptr || *path == '\0') {
        return;
    }
    std::FILE* file = std::fopen(path, "wb");
    if (file == nullptr) {
        std::fprintf(stderr, "FAIL cannot open SOTC_METAL_CAPTURE_DUMP path %s\n", path);
        std::exit(1);
    }
    const std::size_t written = std::fwrite(png.data(), 1, png.size(), file);
    const int closed = std::fclose(file);
    CHECK(written == png.size());
    CHECK(closed == 0);
    std::printf("metal_capture: dumped %zu PNG bytes to %s\n", png.size(), path);
}

}  // namespace

int main() {
    // 1. Invalid sizes are hard errors, never a skip.
    {
        std::string error;
        auto invalid = MetalCapture::create(MetalCaptureConfig{0, 0}, error);
        CHECK(!invalid);
        CHECK(error == "invalid capture size");
    }

    // 2. Provider creation. Only a missing Metal device may skip.
    MetalCaptureConfig config;
    std::string error;
    std::unique_ptr<MetalCapture> capture = MetalCapture::create(config, error);
    if (!capture) {
        if (error == kMetalCaptureNoDeviceError) {
            std::printf("SKIP: %s\n", error.c_str());
            return 77;
        }
        std::fprintf(stderr, "FAIL MetalCapture::create: %s\n", error.c_str());
        return 1;
    }
    CHECK(error.empty());

    // 3. Frame 1: shape, deterministic pattern, opaque alpha, PNG header.
    MetalCaptureResult first = capture->capture(1);
    if (!first.ok) {
        std::fprintf(stderr, "FAIL MetalCapture::capture(1): %s\n", first.error.c_str());
        return 1;
    }
    check_result_shape(first, config.width, config.height, 1);
    for (const Sample& sample : kSamples) {
        check_pixel(first, sample.x, sample.y);
    }
    check_alpha_opaque(first);
    check_png(first.png, config.width, config.height);
    dump_png_if_requested(first.png);

    std::printf("metal_capture: frame 1 rgba8=%zu bytes png=%zu bytes samples:", first.rgba8.size(),
                first.png.size());
    for (const Sample& sample : kSamples) {
        print_pixel(stdout, first, sample.x, sample.y);
    }
    std::printf("\n");

    // 4. Frame 2: every pixel differs in blue only, by exactly +1.
    MetalCaptureResult second = capture->capture(2);
    if (!second.ok) {
        std::fprintf(stderr, "FAIL MetalCapture::capture(2): %s\n", second.error.c_str());
        return 1;
    }
    check_result_shape(second, config.width, config.height, 2);
    CHECK(second.rgba8.size() == first.rgba8.size());
    CHECK(second.png != first.png);
    {
        std::size_t changed = 0;
        bool only_blue_plus_one = true;
        for (std::size_t index = 0; index < first.rgba8.size(); ++index) {
            if (first.rgba8[index] == second.rgba8[index]) {
                continue;
            }
            ++changed;
            if (index % 4 != 2 || second.rgba8[index] != first.rgba8[index] + 1) {
                only_blue_plus_one = false;
            }
        }
        CHECK(only_blue_plus_one);
        CHECK(changed == static_cast<std::size_t>(config.width) * config.height);
    }
    check_pixel(second, 0, 0);
    check_pixel(second, 639, 359);

    // 5. Repeating frame 1 is byte-identical (RGBA and encoded PNG).
    MetalCaptureResult repeated = capture->capture(1);
    if (!repeated.ok) {
        std::fprintf(stderr, "FAIL MetalCapture::capture(1) repeat: %s\n", repeated.error.c_str());
        return 1;
    }
    CHECK(repeated.rgba8 == first.rgba8);
    CHECK(repeated.png == first.png);

    // 6. Non-default size: tight stride (7 * 4 == 28) and the same pattern.
    {
        MetalCaptureConfig small_config{7, 5};
        std::string small_error;
        std::unique_ptr<MetalCapture> small = MetalCapture::create(small_config, small_error);
        CHECK(small != nullptr);
        CHECK(small_error.empty());
        MetalCaptureResult tiny = small->capture(0);
        if (!tiny.ok) {
            std::fprintf(stderr, "FAIL 7x5 capture: %s\n", tiny.error.c_str());
            return 1;
        }
        check_result_shape(tiny, 7, 5, 0);
        CHECK(tiny.rgba8.size() == 7u * 5u * 4u);
        check_pixel(tiny, 0, 0);
        check_pixel(tiny, 3, 2);
        check_pixel(tiny, 6, 4);
        check_alpha_opaque(tiny);
        check_png(tiny.png, 7, 5);
    }

    // 7. Seam wiring: capture_frame -> attach_screenshot yields the real provider PNG, the exact
    //    attachment name and the frame_num from FrameImage.
    {
        std::vector<std::uint8_t> seam_png;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage& image) {
            MetalCaptureResult captured = capture->capture(image.frame_num);
            seam_png = captured.png;
            ScreenshotResult result;
            result.status = captured.ok ? ScreenshotStatus::ok : ScreenshotStatus::encode_failed;
            result.png = std::move(captured.png);
            return result;
        };

        FrameImage image{};
        image.width = config.width;
        image.height = config.height;
        image.stride = config.width * 4;
        image.frame_num = 42;
        const ScreenshotAttachment attachment = attach_screenshot(hooks, image);
        CHECK(attachment.attached);
        CHECK(!attachment.synthetic_command);
        CHECK(!attachment.sent_to_sink);
        CHECK(attachment.capture_called);
        CHECK(attachment.status == ScreenshotStatus::ok);
        CHECK(attachment.name == "bug_screenshot.png");
        CHECK(attachment.frame_num == 42);
        check_png(seam_png, config.width, config.height);

        MetalCaptureResult direct = capture->capture(42);
        if (!direct.ok) {
            std::fprintf(stderr, "FAIL MetalCapture::capture(42): %s\n", direct.error.c_str());
            return 1;
        }
        CHECK(seam_png == direct.png);
    }

    std::printf("test_metal_capture: %d checks passed\n", g_checks);
    return 0;
}
