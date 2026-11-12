// Native bodies for the Init `0x531d60` post-base callees that previously had
// call-site-only models:
//
//   `0x56dcb0` (132 B FDE) — global-object/descriptor setup followed by four calls.
//   `0x5c64c0` (291 B FDE) — AVX constant/BSS initialization, source copy and calls.
//   `0x7f7220` (54 B FDE)  — staffroll text selection.
//
// These are ordinary native functions. Every un-lifted callee is an explicit hook in the
// corresponding hook struct; a missing hook/input produces a named `blocked` stage instead of a
// fake success. Each hook also returns an `InitPostCallOutcome`, which the body records in its
// `call_log`; the runtime projects that log into `sotc.init_window.v5`. A blocked callee is an
// observed, named refusal with arguments and diagnostic; it is not a fabricated return.
#pragma once

#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

// ---------------------------------------------------------------------------------------------
// 0x56dcb0
// ---------------------------------------------------------------------------------------------
inline constexpr std::uint64_t kInit56dcb0BodyRva = 0x56dcb0;
inline constexpr std::size_t kInit56dcb0BodySize = 132;
inline constexpr std::uint64_t kInit56dcb0BodyEndRva = 0x56dd34;
inline constexpr std::uint64_t kInit56dcb0GlobalObjectRva = 0x32f8ff0;
inline constexpr std::uint64_t kInit56dcb0DataCellRva = 0x1604bf0;
inline constexpr std::uintptr_t kInit56dcb0DescriptorWord0 = 0x101e7ac;
inline constexpr std::uint64_t kInit56dcb0DescriptorWord1 = 0x030a;  // bytes 0a 03

struct Init56dcb0BodyHooks {
    void* object = nullptr;    // [0x32f8ff0]
    void* data_cell = nullptr; // 0x1604bf0, passed by address only
    InitPostCallOutcome (*call_a5cd90)(void* object, const void* descriptor) = nullptr;
    InitPostCallOutcome (*call_946880)(int value) = nullptr;
    InitPostCallOutcome (*call_a5d990)(void* object, void* data_cell) = nullptr;
    InitPostCallOutcome (*call_560b60)() = nullptr;
};

struct Init56dcb0BodyResult {
    bool ok = false;
    const char* blocked = nullptr;
    void* object = nullptr;
    void* data_cell = nullptr;
    std::uintptr_t descriptor_word0 = kInit56dcb0DescriptorWord0;
    std::uint64_t descriptor_word1 = kInit56dcb0DescriptorWord1;
    int calls_a5cd90 = 0;
    int calls_946880 = 0;
    int calls_a5d990 = 0;
    int calls_560b60 = 0;
    int calls_blocked = 0;
    int arg_946880[2] = {0, 0};
    int last_946880_value = 0;
    bool first_946880_arg_one = false;
    InitPostCallLog call_log{};
};

[[nodiscard]] Init56dcb0BodyResult run_init_56dcb0_body(
    const Init56dcb0BodyHooks& hooks) noexcept;

// ---------------------------------------------------------------------------------------------
// 0x5c64c0
// ---------------------------------------------------------------------------------------------
inline constexpr std::uint64_t kInit5c64c0BodyRva = 0x5c64c0;
inline constexpr std::size_t kInit5c64c0BodySize = 291;
inline constexpr std::uint64_t kInit5c64c0BodyEndRva = 0x5c65e3;
inline constexpr std::uint64_t kInit5c64c0GlobalOwnerRva = 0x2ed3670;
inline constexpr std::size_t kInit5c64c0SourceOffset = 0x80;
inline constexpr std::size_t kInit5c64c0SourceCopyOffset = 0x24a0;
inline constexpr std::size_t kInit5c64c0SourceCopyBytes = 0x4c;
inline constexpr std::size_t kInit5c64c0BssASize = 0x84;
inline constexpr std::size_t kInit5c64c0BssBSize = 0xc0;
inline constexpr std::size_t kInit5c64c0BssCSize = 0x4c;
inline constexpr std::uint64_t kInit5c64c0BssARva = 0x2fdaa60;
inline constexpr std::uint64_t kInit5c64c0BssBRva = 0x2fda300;
inline constexpr std::uint64_t kInit5c64c0BssCRva = 0x2fda3c0;
inline constexpr std::uint64_t kInit5c64c0CellRva = 0x2fecd68;

// The 0x5c91e0 body copies the initial .data block at 0x2dca510 into 0x2ff2b10 and rewrites
// four fields in the source block. The runtime owns both cells; the body is exact native code.
inline constexpr std::size_t kInit5c91e0SourceBytes = 0xa0;
inline constexpr std::size_t kInit5c91e0DestinationBytes = 0x28;

struct Init5c64c0BodyHooks {
    // Object at [0x2ed3670]; the body reads its +0x80 pointer and copies from +0x24a0.
    void* owner = nullptr;
    std::size_t source_size = 0;
    std::uint8_t* bss_a = nullptr;  // 0x2fdaa60, 0x84 B
    std::uint8_t* bss_b = nullptr;  // 0x2fda300, 0xc0 B
    std::uint8_t* bss_c = nullptr;  // 0x2fda3c0, 0x4c B
    std::uintptr_t* cell = nullptr; // 0x2fecd68
    InitPostCallOutcome (*call_5b9f90)() = nullptr;
    InitPostCallOutcome (*call_96f20)(std::uint32_t, std::uint32_t, std::uint32_t,
                                      std::uint32_t) = nullptr;
    InitPostCallOutcome (*call_5ddcc0)() = nullptr;
    InitPostCallOutcome (*call_5d57f0)() = nullptr;
    InitPostCallOutcome (*call_5d5410)() = nullptr;
    InitPostCallOutcome (*call_5d5760)() = nullptr;
    InitPostCallOutcome (*call_5d5700)() = nullptr;
    InitPostCallOutcome (*call_5c91e0)() = nullptr;
    InitPostCallOutcome (*tail_5cf130)() = nullptr;
};

struct Init5c64c0BodyResult {
    bool ok = false;
    const char* blocked = nullptr;
    void* source = nullptr;
    std::uintptr_t cell_value = 0;
    std::uint32_t bss_a_hash = 0;
    std::uint32_t bss_b_hash = 0;
    std::uint32_t bss_c_hash = 0;
    int calls_5b9f90 = 0;
    int calls_96f20 = 0;
    int calls_after = 0;
    int calls_blocked = 0;
    bool tail_called = false;
    InitPostCallLog call_log{};
};

[[nodiscard]] Init5c64c0BodyResult run_init_5c64c0_body(
    const Init5c64c0BodyHooks& hooks) noexcept;

// ---------------------------------------------------------------------------------------------
// 0x7f7220
// ---------------------------------------------------------------------------------------------
inline constexpr std::uint64_t kStaffroll7f7220Rva = 0x7f7220;
inline constexpr std::size_t kStaffroll7f7220Size = 54;
inline constexpr std::uint64_t kStaffroll7f7220EndRva = 0x7f7256;
inline constexpr std::uintptr_t kStaffrollShadowTextRva = 0x128b878;   // cp10_shadow_staffroll.txt
inline constexpr std::uintptr_t kStaffrollOriginalTextRva = 0x128b892; // staffroll_original_us.txt
inline constexpr std::uintptr_t kStaffrollRemasterTextRva = 0x128b8ac; // staffroll_remaster_us.txt

struct Staffroll7f7220Hooks {
    InitPostCallOutcome (*query_5d06f0)() = nullptr;
    InitPostCallOutcome (*set_7f6f90)(int mode, std::uintptr_t string_rva) = nullptr;
};

struct Staffroll7f7220Result {
    bool ok = false;
    const char* blocked = nullptr;
    bool query_result = false;
    int set_calls = 0;
    int calls_blocked = 0;
    int modes[2] = {0, 0};
    std::uintptr_t strings[2] = {0, 0};
    InitPostCallLog call_log{};
};

[[nodiscard]] Staffroll7f7220Result run_staffroll_7f7220(
    const Staffroll7f7220Hooks& hooks) noexcept;

}  // namespace sotc
