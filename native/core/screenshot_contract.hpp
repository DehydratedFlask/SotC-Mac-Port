// Native seam for the bug-report screenshot (`0x961a0` encoder, `0x1bdfbb..0x1be0a2` report block).
//
// What is native here (E597):
//   - the decision table: no renderer -> synthetic `Command` entry; capture failure -> synthetic;
//     capture ok -> `"SendFile"` best-effort push, then list-B `("Screenshot","bug_screenshot.png")`
//     and NO synthetic entry;
//   - the attachment name literal `bug_screenshot.png` (`0xfb3df0`), which is neither the
//     `"screenshot"` property value nor the `"bug_screenshot"` command text;
//   - the `frame_num` pass-through (the original reads `*(i32*)(engine+0x70)`).
//
// What stays a hook (never faked):
//   - surface creation/wait (`0xa2aa0`, `0x91090`), the `scePngEnc*` encode itself, the socket
//     transport (`*(0x2ea8978)`/`0x4deb0`/`0x4b1d0`) and the console `/app0/` sink (`0x30730`).
//   - `engine+0x70`'s writer was not found; the caller supplies the counter.
//
// `attach_screenshot` does not build a report tree: it returns the decision the report path applies
// (synthetic entry vs list-B attachment). `snapshot_bug_data` remains the tree composer.
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

inline constexpr std::uint64_t kScreenshotEncoder961a0Rva = 0x961A0;
inline constexpr std::size_t kScreenshotEncoder961a0Size = 1027;
inline constexpr std::uint64_t kScreenshotSinkCellRva = 0x2EA8978;
inline constexpr std::uint64_t kScreenshotSendBlobBuilderRva = 0x4B1D0;
inline constexpr std::uint64_t kScreenshotCaptureRva = 0xA2AA0;
inline constexpr std::uint64_t kScreenshotWaitRva = 0x91090;
inline constexpr const char kScreenshotAttachmentName[] = "bug_screenshot.png";
inline constexpr const char kScreenshotPropertyName[] = "screenshot";
inline constexpr const char kScreenshotCommandValue[] = "bug_screenshot";

// Renderer output for one frame. `rgba8` is tightly packed unless `stride` says otherwise.
struct FrameImage {
    const std::uint8_t* rgba8 = nullptr;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
    std::uint32_t frame_num = 0;
};

enum class ScreenshotStatus : std::uint8_t {
    ok = 0,
    no_surface = 1,    // 0x961a0 returns 1 before a surface exists
    encode_failed = 2, // 0x961a0 returns 1 after a surface existed
};

struct ScreenshotResult {
    ScreenshotStatus status = ScreenshotStatus::no_surface;
    std::vector<std::uint8_t> png;
};

struct ScreenshotHooks {
    // `renderer_present` is the `renderer != nullptr` check at the report block. When false the
    // capture hook is never called.
    bool renderer_present = false;
    // 0xa2aa0 + 0x91090 + surface+0xc4: create the surface, wait, and return the encoded PNG.
    std::function<ScreenshotResult(const FrameImage&)> capture_frame;
    // `*(0x2ea8978)` -> 0x4deb0 -> 0x4b1d0. Best-effort: a false result is reported, not raised.
    std::function<bool(std::string_view name, const std::vector<std::uint8_t>& png)> send_file;
    // Optional console path (`0x30730`/`0x30570`); not used by the report block.
    std::function<bool(std::string_view app0_path, const std::vector<std::uint8_t>& png)>
        write_app0_file;
};

struct ScreenshotAttachment {
    bool attached = false;          // the report block's flag at 0x1be09c
    bool synthetic_command = true;  // "Command"/name="screenshot"/value="bug_screenshot"
    bool sent_to_sink = false;      // 0x4b1d0 was reached and accepted
    bool capture_called = false;    // renderer_present && capture_frame supplied
    ScreenshotStatus status = ScreenshotStatus::no_surface;
    std::string name = kScreenshotAttachmentName;
    std::uint32_t frame_num = 0;
};

// Decision table from E597 §6.1. A successful capture with a non-empty PNG is `attached`; anything
// else leaves the synthetic command entry in place. The sink result never changes the attachment.
[[nodiscard]] ScreenshotAttachment attach_screenshot(const ScreenshotHooks& hooks,
                                                     const FrameImage& image) noexcept;

// Console-only path: writes the PNG to the normalized `/app0/...` sink. Separate from the report
// block, which never touches the filesystem.
[[nodiscard]] bool console_screenshot_write_app0(const ScreenshotHooks& hooks,
                                                 std::string_view app0_path,
                                                 const FrameImage& image) noexcept;

}  // namespace sotc
