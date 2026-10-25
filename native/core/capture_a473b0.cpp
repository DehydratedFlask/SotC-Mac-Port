#include "capture_a473b0.hpp"

#include <cstring>

namespace sotc {

CaptureA473b0Result capture_a473b0_set_enabled(bool enable, CaptureServices& services,
                                               const void* const* stack_chk_guard_cell) noexcept {
    CaptureA473b0Result out{};
    out.enable = enable;

    // canary save: r14 = __stack_chk_guard; rax = *(r14) -> [rbp-0x18].
    std::uint64_t canary_before = 0;
    const void* guard_ptr = (stack_chk_guard_cell != nullptr) ? *stack_chk_guard_cell : nullptr;
    out.stack_chk_modelled = (guard_ptr != nullptr);
    if (out.stack_chk_modelled) {
        std::memcpy(&canary_before, guard_ptr, sizeof(canary_before));
    }

    // 0xa473cf sceGameLiveStreamingEnableLiveStreaming(enable) — always first.
    out.live_streaming = services.game_live_streaming_enable(enable);
    out.step = 1;

    // 0xa473d6 branch on the enable bit: select screenshot enable vs disable, and the
    // 0x0d set-info value (0 when enabling, 1 when disabling) written to [rbp-0x1c].
    if (enable) {
        out.screenshot = services.screenshot_enable();
        out.screenshot_enable_taken = true;
        out.video_value_0d = 0;
    } else {
        out.screenshot = services.screenshot_disable();
        out.screenshot_enable_taken = false;
        out.video_value_0d = 1;
    }
    out.step = 2;

    // 0xa47400 sceVideoRecordingSetInfo(0x0d, &state, 4).
    out.video_0d =
        services.video_recording_set_info(kCaptureVideoInfoTypeD, &out.video_value_0d, 4);
    out.step = 3;

    // 0xa47413 state = 0; 0xa4741a sceVideoRecordingSetInfo(0xa007, &state, 4).
    out.video_value_a007 = 0;
    out.video_a007 =
        services.video_recording_set_info(kCaptureVideoInfoTypeA007, &out.video_value_a007, 4);
    out.step = 4;

    // canary compare: 0xa4741f mov rax,[r14]; cmp rax,[rbp-0x18]; jne __stack_chk_fail.
    if (out.stack_chk_modelled) {
        std::uint64_t canary_after = 0;
        std::memcpy(&canary_after, guard_ptr, sizeof(canary_after));
        out.stack_chk_mismatch = (canary_after != canary_before);
    }
    out.step = 5;

    out.any_unsupported = (out.live_streaming == CaptureServiceStatus::unsupported) ||
                          (out.screenshot == CaptureServiceStatus::unsupported) ||
                          (out.video_0d == CaptureServiceStatus::unsupported) ||
                          (out.video_a007 == CaptureServiceStatus::unsupported);

    out.ok = (out.step == 5) && !out.any_unsupported && !out.stack_chk_mismatch;
    return out;
}

}  // namespace sotc