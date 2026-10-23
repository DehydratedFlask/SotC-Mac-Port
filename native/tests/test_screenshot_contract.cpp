// Fixture for the E597 screenshot seam: the three reachable report states plus the console path.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "core/screenshot_contract.hpp"

using namespace sotc;

namespace {

int g_checks = 0;

#define CHECK(c)                                                                     \
    do {                                                                             \
        ++g_checks;                                                                  \
        if (!(c)) {                                                                  \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);          \
            std::exit(1);                                                            \
        }                                                                            \
    } while (0)

struct Sink {
    int calls = 0;
    std::string name;
    std::vector<std::uint8_t> payload;
    bool result = true;
};

std::vector<std::uint8_t> png_bytes() { return {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A}; }

FrameImage frame(std::uint32_t num) {
    static const std::uint8_t pixel[4] = {1, 2, 3, 4};
    FrameImage f{};
    f.rgba8 = pixel;
    f.width = 1920;
    f.height = 1080;
    f.stride = 1920 * 4;
    f.frame_num = num;
    return f;
}

}  // namespace

int main() {
    // 1. No renderer: no capture, synthetic Command, sink untouched.
    {
        Sink sink;
        int captures = 0;
        ScreenshotHooks hooks{};
        hooks.capture_frame = [&](const FrameImage&) {
            ++captures;
            return ScreenshotResult{ScreenshotStatus::ok, png_bytes()};
        };
        hooks.send_file = [&](std::string_view name, const std::vector<std::uint8_t>& png) {
            ++sink.calls;
            sink.name = std::string(name);
            sink.payload = png;
            return true;
        };
        const auto a = attach_screenshot(hooks, frame(7));
        CHECK(!a.attached);
        CHECK(a.synthetic_command);
        CHECK(!a.sent_to_sink);
        CHECK(!a.capture_called);
        CHECK(captures == 0);
        CHECK(sink.calls == 0);
        CHECK(a.name == "bug_screenshot.png");
        CHECK(a.frame_num == 7);
    }

    // 2. Renderer but no capture hook: still synthetic, no crash.
    {
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        const auto a = attach_screenshot(hooks, frame(8));
        CHECK(!a.attached);
        CHECK(a.synthetic_command);
        CHECK(!a.capture_called);
    }

    // 3. Capture reports NoSurface: synthetic, sink untouched.
    {
        Sink sink;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::no_surface, {}};
        };
        hooks.send_file = [&](std::string_view, const std::vector<std::uint8_t>&) {
            ++sink.calls;
            return true;
        };
        const auto a = attach_screenshot(hooks, frame(9));
        CHECK(!a.attached);
        CHECK(a.synthetic_command);
        CHECK(a.capture_called);
        CHECK(a.status == ScreenshotStatus::no_surface);
        CHECK(sink.calls == 0);
    }

    // 4. Capture reports EncodeFailed: synthetic, sink untouched.
    {
        Sink sink;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::encode_failed, {}};
        };
        hooks.send_file = [&](std::string_view, const std::vector<std::uint8_t>&) {
            ++sink.calls;
            return true;
        };
        const auto a = attach_screenshot(hooks, frame(10));
        CHECK(!a.attached);
        CHECK(a.synthetic_command);
        CHECK(a.status == ScreenshotStatus::encode_failed);
        CHECK(sink.calls == 0);
    }

    // 5. Ok + sink accepts: attached, no synthetic entry, one send with the exact name/payload.
    {
        Sink sink;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage& f) {
            CHECK(f.width == 1920);
            CHECK(f.height == 1080);
            CHECK(f.stride == 1920 * 4);
            return ScreenshotResult{ScreenshotStatus::ok, png_bytes()};
        };
        hooks.send_file = [&](std::string_view name, const std::vector<std::uint8_t>& png) {
            ++sink.calls;
            sink.name = std::string(name);
            sink.payload = png;
            return sink.result;
        };
        const auto a = attach_screenshot(hooks, frame(11));
        CHECK(a.attached);
        CHECK(!a.synthetic_command);
        CHECK(a.sent_to_sink);
        CHECK(a.capture_called);
        CHECK(a.status == ScreenshotStatus::ok);
        CHECK(a.name == "bug_screenshot.png");
        CHECK(a.frame_num == 11);
        CHECK(sink.calls == 1);
        CHECK(sink.name == "bug_screenshot.png");
        CHECK(sink.payload == png_bytes());

        // 6. Same but the sink rejects: the attachment still stands; the result is discarded.
        sink.result = false;
        const auto b = attach_screenshot(hooks, frame(12));
        CHECK(b.attached);
        CHECK(!b.synthetic_command);
        CHECK(!b.sent_to_sink);
    }

    // 7. Ok with an empty payload is treated as an encoder failure.
    {
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::ok, {}};
        };
        const auto a = attach_screenshot(hooks, frame(13));
        CHECK(!a.attached);
        CHECK(a.synthetic_command);
        CHECK(a.status == ScreenshotStatus::encode_failed);
    }

    // 8. Console path: separate /app0/ write, no report attachment.
    {
        int writes = 0;
        std::string path;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [&](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::ok, png_bytes()};
        };
        hooks.write_app0_file = [&](std::string_view p, const std::vector<std::uint8_t>& png) {
            ++writes;
            path = std::string(p);
            CHECK(png == png_bytes());
            return true;
        };
        CHECK(console_screenshot_write_app0(hooks, "/app0/screenshot.png", frame(14)));
        CHECK(writes == 1);
        CHECK(path == "/app0/screenshot.png");

        CHECK(!console_screenshot_write_app0(hooks, "", frame(14)));
        ScreenshotHooks no_write = hooks;
        no_write.write_app0_file = nullptr;
        CHECK(!console_screenshot_write_app0(no_write, "/app0/x.png", frame(14)));
        ScreenshotHooks no_renderer = hooks;
        no_renderer.renderer_present = false;
        CHECK(!console_screenshot_write_app0(no_renderer, "/app0/x.png", frame(14)));
        ScreenshotHooks bad_capture = hooks;
        bad_capture.capture_frame = [](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::encode_failed, {}};
        };
        CHECK(!console_screenshot_write_app0(bad_capture, "/app0/x.png", frame(14)));
        CHECK(writes == 1);
    }

    // 9. The report path never calls the console writer.
    {
        int app0_writes = 0;
        ScreenshotHooks hooks{};
        hooks.renderer_present = true;
        hooks.capture_frame = [](const FrameImage&) {
            return ScreenshotResult{ScreenshotStatus::ok, png_bytes()};
        };
        hooks.write_app0_file = [&](std::string_view, const std::vector<std::uint8_t>&) {
            ++app0_writes;
            return true;
        };
        const auto a = attach_screenshot(hooks, frame(15));
        CHECK(a.attached);
        CHECK(app0_writes == 0);
    }

    std::printf("test_screenshot_contract: %d checks passed\n", g_checks);
    return 0;
}
