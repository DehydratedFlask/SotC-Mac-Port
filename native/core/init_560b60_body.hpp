// Native 0x560b60 body: one-time 0x136f0-byte collision/render descriptor init.
//
// Original: 0x560b60..0x560e83, 803 B, sha256
// 9f67f04fe33b637bf34ad25a4912f245a5f6904c484f55370833bb1d5679fa56.
//
// The body contains 20 direct calls: 0x11bc0 (descriptor lookup), 0x17070/0x17090
// (allocation), 0x170b0/0x170c0 (release), 0x3ca330 (three stack descriptor objects),
// 0x273890 (callback registration), 0x14c0f0 (format/output), 0xf590 x3 (shared release),
// 0x56b540 (probe), 0x95580 (conditional follow-up) and 0x56efb0 (final initializer), plus
// the 0xf8f3a0 stack-check trap (pinned, never called). Its directly observable stores are:
//   [0x2f08910..0x2f08920) <- descriptor[0..0x18)
//   [0x2f08960]             <- the new object
//   [0x2efe2d0/8/e0]        <- 0x561320 / 0x561480 / 0x561570
//   [0x2f08928]             <- int from 0x14c0f0
//   [0x2f08930..0x2f08950)  <- 32 bytes from the 0x14c0f0 output block
// and the object header/list ranges +0x00/+0x08/+0x10/+0x20/+0x30/+0x8020/+0x8028/
// +0x10040..+0x10090/+0x100a0/+0x100b0/+0x136c0..+0x136d8.
//
// The native body reproduces those stores and branch structure. The 13 unmodelled helpers
// are named hooks; a missing or refusing hook produces a named blocked stage. The runtime
// supplies bounded host contracts for them and reports the resulting object/buffers. This is
// a body lift with explicit host seams, not a claim that the libc/resource helpers or their
// internal effects are native.

#pragma once

#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit560b60BodyRva = 0x560b60;
inline constexpr std::size_t kInit560b60BodySize = 0x323;  // 803 B
inline constexpr std::uint64_t kInit560b60BodyEndRva = 0x560e83;
inline constexpr std::size_t kInit560b60ObjectBytes = 0x136f0;
inline constexpr std::size_t kInit560b60Buffer0Bytes = 0x2000;
inline constexpr std::size_t kInit560b60Buffer1Bytes = 0x400;
inline constexpr std::size_t kInit560b60Buffer2Bytes = 0x200;
inline constexpr std::size_t kInit560b60DescriptorBytes = 0x18;
inline constexpr std::size_t kInit560b60FormatBytes = 0x28;
inline constexpr std::size_t kInit560b60Global30Bytes = 0x20;
inline constexpr std::size_t kInit560b60Global50Bytes = 0x20;

// Pinned globals and constants.
inline constexpr std::uint64_t kInit560b60DescriptorRva = 0x101ce8a;
inline constexpr std::uint64_t kInit560b60FormatStringRva = 0x101ce94;
inline constexpr std::uint64_t kInit560b60FormatStringBytes = 0x31;
inline constexpr std::uint64_t kInit560b60Global10Rva = 0x2f08910;
inline constexpr std::uint64_t kInit560b60Global20Rva = 0x2f08920;
inline constexpr std::uint64_t kInit560b60Global28Rva = 0x2f08928;
inline constexpr std::uint64_t kInit560b60Global30Rva = 0x2f08930;
inline constexpr std::uint64_t kInit560b60Global50Rva = 0x2f08950;
inline constexpr std::uint64_t kInit560b60Global60Rva = 0x2f08960;
inline constexpr std::uint64_t kInit560b60Callback0Rva = 0x2efe2d0;
inline constexpr std::uint64_t kInit560b60Callback1Rva = 0x2efe2d8;
inline constexpr std::uint64_t kInit560b60Callback2Rva = 0x2efe2e0;
inline constexpr std::uint64_t kInit560b60Callback0 = 0x561320;
inline constexpr std::uint64_t kInit560b60Callback1 = 0x561480;
inline constexpr std::uint64_t kInit560b60Callback2 = 0x561570;
inline constexpr std::uint64_t kInit560b60StackValue0 = 0x560580;
inline constexpr std::uint64_t kInit560b60StackValue1 = 0x5605f0;
inline constexpr std::uint64_t kInit560b60StackValue2 = 0x560600;
inline constexpr std::uint64_t kInit560b60Register0 = 0x560e90;
inline constexpr std::uint64_t kInit560b60Register1 = 0x5612c0;
inline constexpr std::uint64_t kInit560b60Register2 = 0x5612e0;

struct Init560b60Hooks {
    // Host-owned descriptor returned by 0x11bc0. The original reads 0x18 B from the return.
    const void* descriptor = nullptr;
    // Host-owned output/object storage. The body allocates through the two 0x17070/0x17090
    // contracts and writes into these exact ranges.
    std::uint8_t* object = nullptr;       // 0x136f0
    std::uint8_t* buffer_2000 = nullptr;  // 0x2000
    std::uint8_t* buffer_0400 = nullptr;  // 0x400
    std::uint8_t* buffer_0200 = nullptr;  // 0x200
    // Globals at 0x2f08910..0x2f08960. The caller owns the backing storage.
    std::uint8_t* global_10 = nullptr;    // 16 B
    std::uint64_t* global_20 = nullptr;   // 8 B
    std::uint32_t* global_28 = nullptr;   // 4 B
    std::uint8_t* global_30 = nullptr;    // 32 B
    std::uint8_t* global_50 = nullptr;    // 32 B
    void** previous = nullptr;            // 0x2f08960
    std::uintptr_t* callbacks = nullptr;  // [3] at 0x2efe2d0/8/e0
    std::uint8_t* format_out = nullptr;   // 0x28, int at +0, 32-byte block at +8

    InitPostCallOutcome (*call_11bc0)(const void* descriptor) = nullptr;
    InitPostCallOutcome (*alloc_17070)(std::size_t size, void** out) = nullptr;
    InitPostCallOutcome (*alloc_17090)(std::size_t size, void** out) = nullptr;
    InitPostCallOutcome (*free_170b0)(void* pointer) = nullptr;
    InitPostCallOutcome (*free_170c0)(void* pointer) = nullptr;
    InitPostCallOutcome (*call_3ca330)(void* a, void* b, void* c) = nullptr;
    InitPostCallOutcome (*call_273890)(std::uintptr_t a, std::uintptr_t b,
                                       std::uintptr_t c) = nullptr;
    InitPostCallOutcome (*call_14c0f0)(std::uint8_t* out, const std::uint8_t* input) = nullptr;
    InitPostCallOutcome (*release_f590)(void* pointer) = nullptr;
    InitPostCallOutcome (*call_56b540)(void* pointer, bool* take_branch) = nullptr;
    InitPostCallOutcome (*call_95580)(void* pointer) = nullptr;
    InitPostCallOutcome (*call_56efb0)() = nullptr;
};

struct Init560b60Result {
    bool ok = false;
    const char* blocked = nullptr;
    void* object = nullptr;
    void* buffer_2000 = nullptr;
    void* buffer_0400 = nullptr;
    void* buffer_0200 = nullptr;
    int alloc_calls = 0;
    int free_calls = 0;
    int calls_total = 0;
    int calls_95580 = 0;
    bool previous_cleanup = false;
    std::uint32_t global_28 = 0;
    std::uintptr_t callbacks[3] = {0, 0, 0};
    InitPostCallLog call_log{};
};

[[nodiscard]] Init560b60Result run_init_560b60_body(
    const Init560b60Hooks& hooks) noexcept;

}  // namespace sotc
