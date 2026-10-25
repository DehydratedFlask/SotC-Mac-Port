// 0xa473b0 — capture / streaming enable-state setter (leaf, 134 B).
//
// OBSERVED (byte-verified against local/re/eboot.elf sha256 d0723afb…):
//   effectively `void(/*this unused*/, bool enable)`: the first arg (rdi) is never read;
//   only the second arg (sil -> ebx) is consumed. From Init batch D it is reached as
//   `mov rdi,[0x32f79a8]; xor esi,esi; call 0xa473b0`, i.e. capture_enable(false).
//
//   prologue  push rbp / mov rbp,rsp / push r14 / push rbx / sub rsp,0x10
//   0xa473bb  mov r14,[rip+0xbb7156]        -> r14 = __stack_chk_guard (cell 0x15fe518,
//              an R_X86_64_64 slot that the loader fills with libkernel OBJECT f7uOxY9mM1U)
//   0xa473c8  mov rax,[r14]; mov [rbp-0x18],rax        save canary on the frame
//   0xa473c2  movzx ebx,sil ; 0xa473c6 mov edi,ebx     enable bool
//   0xa473cf  call 0xf91380        sceGameLiveStreamingEnableLiveStreaming(enable)
//   0xa473d4  test bl,bl ; 0xa473d6 je 0xa473e6
//      enable path:  0xa473d8 call 0xf913a0  sceScreenShotEnable()
//                    0xa473dd mov [rbp-0x1c],0        (state dword = 0)
//      disable path: 0xa473e6 call 0xf91390  sceScreenShotDisable()
//                    0xa473eb mov [rbp-0x1c],1        (state dword = 1)
//   0xa473f2  lea rsi,[rbp-0x1c]; mov edi,0x0d; mov edx,4
//   0xa47400  call 0xf913b0        sceVideoRecordingSetInfo(0x0d, &state, 4)
//   0xa47405  lea rsi,[rbp-0x1c]; mov edi,0xa007; mov edx,4
//   0xa47413  mov [rbp-0x1c],0     (state dword reset to 0)
//   0xa4741a  call 0xf913b0        sceVideoRecordingSetInfo(0xa007, &state, 4)
//   0xa4741f  mov rax,[r14]; 0xa47422 cmp rax,[rbp-0x18]; 0xa47426 jne fail
//   epilogue (add rsp,0x10 / pop x3 / ret)
//   0xa47431  call 0xf8f3a0        __stack_chk_fail  (only on canary mismatch; aborts)
//
// Import call inventory (call site -> thunk RVA -> NID -> symbol -> library):
//   0xa473cf  0xf91380  dWM80AX39o4  sceGameLiveStreamingEnableLiveStreaming  libSceGameLiveStreaming
//   0xa473d8  0xf913a0  2xxUtuC-RzE  sceScreenShotEnable                      libSceScreenShot
//   0xa473e6  0xf91390  tIYf0W5VTi8  sceScreenShotDisable                     libSceScreenShot
//   0xa47400  0xf913b0  Fc8qxlKINYQ  sceVideoRecordingSetInfo                 libSceVideoRecording
//   0xa4741a  0xf913b0  Fc8qxlKINYQ  sceVideoRecordingSetInfo                 libSceVideoRecording
//   0xa47431  0xf8f3a0  Ou3iL1abvng  __stack_chk_fail                        libkernel
//   (guard)   (0x15fe518) f7uOxY9mM1U __stack_chk_guard                      libkernel
//
// State dword written by this leaf: the frame local [rbp-0x1c] = 0 (enable) or 1
// (disable) for the 0x0d set-info, reset to 0 for the 0xa007 set-info. It is the only
// state this function stores (no BSS/.data store inside the body).
//
// NOT modelled:
//   - the `this` argument (rdi): optimised out in the original and never read.
//   - the Sony service bodies: the imports are contracts, not implementations.
//   - the __stack_chk_guard host lookup: supplied as a pointer cell (see param).
//   - the meaning of set-info type ids 0x0d / 0xa007 (opaque Sony enum values).
//   - __stack_chk_fail is surfaced as a result flag, never an actual abort.
//
// G1 PARTIAL — native service-contract lift only; not run against the game.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCaptureA473b0Rva = 0xa473b0;
inline constexpr std::size_t kCaptureA473b0Size = 134;
inline constexpr std::uint64_t kCaptureA473b0EndRva = 0xa47436;
inline constexpr std::uint64_t kCaptureA473b0GuardCellRva = 0x15fe518;

// Observed import thunk addresses (for the ELF checker + header audit).
inline constexpr std::uint64_t kCaptureLiveStreamingThunk = 0xf91380;
inline constexpr std::uint64_t kCaptureScreenShotEnableThunk = 0xf913a0;
inline constexpr std::uint64_t kCaptureScreenShotDisableThunk = 0xf91390;
inline constexpr std::uint64_t kCaptureVideoRecordingThunk = 0xf913b0;
inline constexpr std::uint64_t kCaptureStackChkFailThunk = 0xf8f3a0;

// Observed opaque set-info type ids passed to sceVideoRecordingSetInfo.
inline constexpr std::uint32_t kCaptureVideoInfoTypeD = 0x0d;
inline constexpr std::uint32_t kCaptureVideoInfoTypeA007 = 0xa007;

// Explicit status for every platform-service contract. A service is never *silently*
// treated as success: an absent/unsupported service must show up in the result.
enum class CaptureServiceStatus : std::uint8_t {
    ok = 0,           // service present and accepted the call
    disabled = 1,     // service present but reports disabled/deferred
    unsupported = 2,  // service missing / not supplied on this host
};

// Per-Sony-service contract. Implementations return an explicit status and (in a real
// port) perform the underlying effect; the lift only dispatches to them in the observed
// order and records their status. There is no Sony module name anywhere in this native
// contract — the symbols are identified by NID only, here as documentation.
class CaptureServices {
public:
    virtual ~CaptureServices() = default;
    virtual CaptureServiceStatus game_live_streaming_enable(bool enable) = 0;
    virtual CaptureServiceStatus screenshot_enable() = 0;
    virtual CaptureServiceStatus screenshot_disable() = 0;
    virtual CaptureServiceStatus video_recording_set_info(std::uint32_t type, const void* value,
                                                          std::size_t size) = 0;
};

// Default platform: every service unsupported. Using this still exercises the lifted
// control flow but reports `any_unsupported`, so absence is visible, not a fake success.
class UnsupportedCaptureServices final : public CaptureServices {
public:
    CaptureServiceStatus game_live_streaming_enable(bool) override {
        return CaptureServiceStatus::unsupported;
    }
    CaptureServiceStatus screenshot_enable() override {
        return CaptureServiceStatus::unsupported;
    }
    CaptureServiceStatus screenshot_disable() override {
        return CaptureServiceStatus::unsupported;
    }
    CaptureServiceStatus video_recording_set_info(std::uint32_t, const void*,
                                                  std::size_t) override {
        return CaptureServiceStatus::unsupported;
    }
};

struct CaptureA473b0Result {
    bool ok = false;    // full control flow + no unsupported service + no canary mismatch
    bool enable = false;
    int step = 0;       // 0..5; 5 = all call sites reached in the observed order

    CaptureServiceStatus live_streaming = CaptureServiceStatus::unsupported;
    CaptureServiceStatus screenshot = CaptureServiceStatus::unsupported;  // enable xor disable
    CaptureServiceStatus video_0d = CaptureServiceStatus::unsupported;
    CaptureServiceStatus video_a007 = CaptureServiceStatus::unsupported;

    std::uint32_t video_value_0d = 0;    // 0 (enable) or 1 (disable)
    std::uint32_t video_value_a007 = 0;  // always 0

    bool screenshot_enable_taken = false;  // true on the enable path, false on disable
    bool any_unsupported = true;           // at least one service reported unsupported

    bool stack_chk_modelled = false;  // a guard cell was supplied and checked
    bool stack_chk_mismatch = false;  // guard changed across the call (would be __stack_chk_fail)
};

[[nodiscard]] CaptureA473b0Result capture_a473b0_set_enabled(
    bool enable, CaptureServices& services,
    const void* const* stack_chk_guard_cell = nullptr) noexcept;

}  // namespace sotc