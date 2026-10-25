// Test 0xa473b0 capture/streaming enable-state setter against the CaptureServices contract.
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/capture_a473b0.hpp"

#define CHECK(c)                                                            \
    do {                                                                    \
        if (!(c)) {                                                         \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); \
            std::exit(1);                                                   \
        }                                                                   \
    } while (0)

using namespace sotc;

namespace {

// Records every contract dispatch in order, so the test can assert call order + args.
struct RecordingCaptureServices final : CaptureServices {
    struct Event {
        int kind = 0;             // 0=live_streaming 1=screenshot_enable 2=screenshot_disable
                                  // 3=video_set_info
        bool enable = false;      // for kind 0
        std::uint32_t type = 0;   // for kind 3
        std::uint32_t value = 0;  // for kind 3 (dereferenced 4-byte value)
        std::size_t size = 0;     // for kind 3
    };
    std::vector<Event> events;

    CaptureServiceStatus live_status = CaptureServiceStatus::ok;
    CaptureServiceStatus screenshot_enable_status = CaptureServiceStatus::ok;
    CaptureServiceStatus screenshot_disable_status = CaptureServiceStatus::ok;
    CaptureServiceStatus video_status = CaptureServiceStatus::ok;

    // Optional: mutate the stack-guard buffer during a contract call to prove the
    // canary check surfaces a mismatch (would be __stack_chk_fail in the original).
    std::uint8_t* guard_buf = nullptr;
    CaptureServiceStatus game_live_streaming_enable(bool e) override {
        if (guard_buf != nullptr) {
            guard_buf[0] ^= 0xff;  // simulate stack cookie corruption observed post-call
        }
        events.push_back({0, e, 0, 0, 0});
        return live_status;
    }
    CaptureServiceStatus screenshot_enable() override {
        events.push_back({1, false, 0, 0, 0});
        return screenshot_enable_status;
    }
    CaptureServiceStatus screenshot_disable() override {
        events.push_back({2, false, 0, 0, 0});
        return screenshot_disable_status;
    }
    CaptureServiceStatus video_recording_set_info(std::uint32_t type, const void* value,
                                                  std::size_t size) override {
        std::uint32_t v = 0;
        std::memcpy(&v, value, size < sizeof(v) ? size : sizeof(v));
        events.push_back({3, false, type, v, size});
        return video_status;
    }
};

void test_call_order_and_args() {
    // enable path
    {
        RecordingCaptureServices svc;
        const auto r = capture_a473b0_set_enabled(true, svc);
        CHECK(r.step == 5);
        CHECK(r.ok);
        CHECK(!r.any_unsupported);
        CHECK(r.enable);
        CHECK(r.screenshot_enable_taken);
        CHECK(r.video_value_0d == 0);    // enable -> state dword 0
        CHECK(r.video_value_a007 == 0);  // always 0

        CHECK(svc.events.size() == 4);
        // order: live-streaming, screenshot enable, video 0x0d, video 0xa007
        CHECK(svc.events[0].kind == 0 && svc.events[0].enable);
        CHECK(svc.events[1].kind == 1);  // screenshot_enable
        CHECK(svc.events[2].kind == 3 && svc.events[2].type == 0x0d && svc.events[2].value == 0 &&
              svc.events[2].size == 4);
        CHECK(svc.events[3].kind == 3 && svc.events[3].type == 0xa007 &&
              svc.events[3].value == 0 && svc.events[3].size == 4);

        CHECK(r.live_streaming == CaptureServiceStatus::ok);
        CHECK(r.screenshot == CaptureServiceStatus::ok);
        CHECK(r.video_0d == CaptureServiceStatus::ok);
        CHECK(r.video_a007 == CaptureServiceStatus::ok);
    }

    // disable path
    {
        RecordingCaptureServices svc;
        const auto r = capture_a473b0_set_enabled(false, svc);
        CHECK(r.step == 5);
        CHECK(r.ok);
        CHECK(!r.enable);
        CHECK(!r.screenshot_enable_taken);
        CHECK(r.video_value_0d == 1);    // disable -> state dword 1
        CHECK(r.video_value_a007 == 0);

        CHECK(svc.events.size() == 4);
        CHECK(svc.events[0].kind == 0 && !svc.events[0].enable);
        CHECK(svc.events[1].kind == 2);  // screenshot_disable
        CHECK(svc.events[2].kind == 3 && svc.events[2].type == 0x0d && svc.events[2].value == 1);
        CHECK(svc.events[3].kind == 3 && svc.events[3].type == 0xa007 &&
              svc.events[3].value == 0);
    }
}

void test_state_transition() {
    // enable -> disable flips the state dword and the screenshot branch.
    RecordingCaptureServices svc;
    const auto r_enable = capture_a473b0_set_enabled(true, svc);
    const auto r_disable = capture_a473b0_set_enabled(false, svc);
    CHECK(r_enable.video_value_0d == 0);
    CHECK(r_disable.video_value_0d == 1);
    CHECK(r_enable.screenshot_enable_taken);
    CHECK(!r_disable.screenshot_enable_taken);
    // the 0xa007 value is invariant
    CHECK(r_enable.video_value_a007 == 0 && r_disable.video_value_a007 == 0);
}

void test_unsupported_visibility() {
    // all-unsupported platform: control flow still runs but absence is visible.
    UnsupportedCaptureServices unsupported;
    const auto r = capture_a473b0_set_enabled(true, unsupported);
    CHECK(r.step == 5);
    CHECK(!r.ok);
    CHECK(r.any_unsupported);
    CHECK(r.live_streaming == CaptureServiceStatus::unsupported);
    CHECK(r.screenshot == CaptureServiceStatus::unsupported);
    CHECK(r.video_0d == CaptureServiceStatus::unsupported);
    CHECK(r.video_a007 == CaptureServiceStatus::unsupported);

    // partially-unsupported: exactly one missing service still fails ok and is visible.
    RecordingCaptureServices partial;
    partial.screenshot_enable_status = CaptureServiceStatus::unsupported;
    partial.screenshot_disable_status = CaptureServiceStatus::unsupported;
    const auto p = capture_a473b0_set_enabled(true, partial);
    CHECK(p.step == 5);
    CHECK(!p.ok);
    CHECK(p.any_unsupported);
    CHECK(p.screenshot == CaptureServiceStatus::unsupported);
    CHECK(p.live_streaming == CaptureServiceStatus::ok);
    CHECK(p.video_0d == CaptureServiceStatus::ok);

    // disabled is a legitimate "present but off" status, not a failure.
    RecordingCaptureServices off;
    off.live_status = CaptureServiceStatus::disabled;
    off.screenshot_enable_status = CaptureServiceStatus::disabled;
    off.video_status = CaptureServiceStatus::disabled;
    const auto d = capture_a473b0_set_enabled(true, off);
    CHECK(d.step == 5);
    CHECK(!d.any_unsupported);
    CHECK(d.ok);
    CHECK(d.live_streaming == CaptureServiceStatus::disabled);
}

void test_idempotence() {
    // The setter is stateless: two identical calls dispatch the same sequence and resolve
    // to the same result (no internal counter or latch alters behaviour).
    RecordingCaptureServices svc;
    const auto first = capture_a473b0_set_enabled(false, svc);
    const auto second = capture_a473b0_set_enabled(false, svc);
    CHECK(first.step == second.step);
    CHECK(first.ok == second.ok);
    CHECK(first.any_unsupported == second.any_unsupported);
    CHECK(first.video_value_0d == second.video_value_0d);
    CHECK(first.video_value_a007 == second.video_value_a007);
    CHECK(first.live_streaming == second.live_streaming);
    CHECK(first.screenshot == second.screenshot);
    CHECK(svc.events.size() == 8);  // 4 per call, identical args
    for (int i = 0; i < 4; ++i) {
        CHECK(svc.events[i].kind == svc.events[i + 4].kind);
    }
}

void test_stack_chk() {
    // guard supplied and unchanged -> modelled, no mismatch.
    {
        std::uint64_t guard = 0x0102030405060708ULL;
        const void* cell = &guard;
        RecordingCaptureServices svc;
        const auto r = capture_a473b0_set_enabled(true, svc, &cell);
        CHECK(r.stack_chk_modelled);
        CHECK(!r.stack_chk_mismatch);
        CHECK(r.ok);
    }
    // guard mutated during a contract call -> mismatch surfaced, ok fails (would abort).
    {
        alignas(8) std::uint8_t guard[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        const void* cell = &guard[0];
        RecordingCaptureServices svc;
        svc.guard_buf = guard;  // game_live_streaming_enable flips guard[0]
        const auto r = capture_a473b0_set_enabled(true, svc, &cell);
        CHECK(r.stack_chk_modelled);
        CHECK(r.stack_chk_mismatch);
        CHECK(!r.ok);
    }
    // no guard cell -> not modelled (explicit), but the service flow still resolves.
    {
        RecordingCaptureServices svc;
        const auto r = capture_a473b0_set_enabled(true, svc);
        CHECK(!r.stack_chk_modelled);
        CHECK(!r.stack_chk_mismatch);
        CHECK(r.ok);
    }
}

}  // namespace

int main() {
    test_call_order_and_args();
    test_state_transition();
    test_unsupported_visibility();
    test_idempotence();
    test_stack_chk();
    std::puts("OK test_capture_a473b0");
    return 0;
}