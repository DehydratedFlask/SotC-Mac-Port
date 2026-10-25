#include "screenshot_contract.hpp"

namespace sotc {

ScreenshotAttachment attach_screenshot(const ScreenshotHooks& hooks,
                                       const FrameImage& image) noexcept {
    ScreenshotAttachment out{};
    out.frame_num = image.frame_num;

    // Rule 1: `renderer == nullptr` -> no capture at all, synthetic Command entry.
    if (!hooks.renderer_present || !hooks.capture_frame) {
        return out;
    }

    // Rules 2/3: the encoder's own status decides attachment vs the synthetic entry.
    out.capture_called = true;
    ScreenshotResult captured = hooks.capture_frame(image);
    out.status = captured.status;
    if (captured.status != ScreenshotStatus::ok || captured.png.empty()) {
        out.status = captured.status == ScreenshotStatus::ok ? ScreenshotStatus::encode_failed
                                                             : captured.status;
        return out;
    }

    // Rule 3: the blob is pushed to the best-effort socket sink (0x4b1d0), then list B gets the
    // "Screenshot" attachment and the synthetic entry is suppressed.
    if (hooks.send_file) {
        out.sent_to_sink = hooks.send_file(kScreenshotAttachmentName, captured.png);
    }
    // Rule 4: `0x4b1d0`'s result is discarded; no invented error path.
    out.attached = true;
    out.synthetic_command = false;
    return out;
}

bool console_screenshot_write_app0(const ScreenshotHooks& hooks, std::string_view app0_path,
                                   const FrameImage& image) noexcept {
    if (!hooks.renderer_present || !hooks.capture_frame || !hooks.write_app0_file ||
        app0_path.empty()) {
        return false;
    }
    ScreenshotResult captured = hooks.capture_frame(image);
    if (captured.status != ScreenshotStatus::ok || captured.png.empty()) {
        return false;
    }
    return hooks.write_app0_file(app0_path, captured.png);
}

}  // namespace sotc
