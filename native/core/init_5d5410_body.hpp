// Native bodies for the Init post-base `0x5d5410` chain.
//
//   `0x5d5410` (420 B, 0x5d5410..0x5d55b4) configures two 0x30-byte state objects at
//   0x30015c8 / 0x30015f0 through `0x8be60`, builds three 16-byte records through
//   `0x8f520`, and three 8-byte records through `0x8f1d0`, storing them at
//   0x3001618..0x300165f. The owner pointer is loaded from [0x2ed33f0] into rdi before
//   every call; both callees overwrite rdi with a local record pointer before their first
//   nested call, so the owner is a dead argument and is only recorded.
//
//   `0x8be60` (547 B, 0x8be60..0x8c083) is the object-format initializer. Its observed
//   path (esi=0x1c, edx=0x50, ecx=1, r8=2, r9=0, stack0=0) writes +0x18=2, +0x1c=0x1c,
//   +0x20=0x50 and calls the native format setters 0x8655d0 then 0x865360(0x10). The
//   allocation/copy branches are modelled too; 0x12820 and the 0x87f3e0 -> 0xf906d0
//   platform tail remain explicit host contracts.
//
//   `0x8f520` (269 B, 0x8f520..0x8f62d) and `0x8f1d0` (171 B, 0x8f1d0..0x8f27b) are pure
//   local bitfield builders over the native 0x87cxxx setters below. The compiler
//   stack-check call sites 0x8f628 / 0x8f276 (target 0xf8f3a0) are pinned by the checker
//   and are never called by these models.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit5d5410BodyRva = 0x5d5410;
inline constexpr std::size_t kInit5d5410BodySize = 0x1a4;
inline constexpr std::uint64_t kInit5d5410BodyEndRva = 0x5d55b4;
inline constexpr std::uint64_t kInit5d5410ObjectARva = 0x30015c8;
inline constexpr std::uint64_t kInit5d5410ObjectBRva = 0x30015f0;
inline constexpr std::uint64_t kInit5d5410GlobalOwnerRva = 0x2ed33f0;
inline constexpr std::uint64_t kInit5d5410StateRva = 0x3001618;
inline constexpr std::size_t kInit5d5410ObjectBytes = 0x30;
inline constexpr std::size_t kInit5d5410StateBytes = 0x48;

inline constexpr std::uint64_t kInit8be60BodyRva = 0x8be60;
inline constexpr std::size_t kInit8be60BodySize = 0x223;
inline constexpr std::uint64_t kInit8be60BodyEndRva = 0x8c083;
inline constexpr std::uint64_t kInit8f520BodyRva = 0x8f520;
inline constexpr std::size_t kInit8f520BodySize = 0x10d;
inline constexpr std::uint64_t kInit8f520BodyEndRva = 0x8f62d;
inline constexpr std::uint64_t kInit8f1d0BodyRva = 0x8f1d0;
inline constexpr std::size_t kInit8f1d0BodySize = 0xab;
inline constexpr std::uint64_t kInit8f1d0BodyEndRva = 0x8f27b;

// Compiler stack-check trap sites inside the two bitfield builders. These are pinned by
// the checker and are never called; the models have no stack protector.
inline constexpr std::uint64_t kInit8f520StackChkSiteRva = 0x8f628;
inline constexpr std::uint64_t kInit8f1d0StackChkSiteRva = 0x8f276;
inline constexpr std::uint64_t kInitStackChkTrapRva = 0xf8f3a0;
inline constexpr std::uint64_t kInit8be60AllocRva = 0x12820;
inline constexpr std::uint64_t kInit8be60PlatformTailRva = 0x87f3e0;
inline constexpr std::uint64_t kInit8be60PlatformTailImportSlotRva = 0x1600958;

struct Init8be60Hooks {
    // 0x12820(size, tag, alignment=0x10, flags=0). The observed 0x5d5410 path never
    // allocates because a4 bit1 is set; the unobserved allocation/copy branches use it.
    InitPostCallOutcome (*alloc_12820)(std::uint32_t size, std::uint32_t tag,
                                       std::uint32_t alignment,
                                       std::uint32_t flags) = nullptr;
    // 0x87f3e0 is a 5-byte jump thunk to 0xf906d0, which jumps through the import slot at
    // 0x1600958. Reached only when the first stack argument (stack0) is non-null.
    InitPostCallOutcome (*tail_87f3e0)(void* dst, std::uint32_t tag, std::uint64_t packed,
                                       std::uint32_t count, std::uint64_t arg5,
                                       std::uint32_t arg6, std::uint64_t arg7) = nullptr;
    // Values of the four tag cells read by the body.
    std::uint32_t tag_2dd9f84 = 0;
    std::uint32_t tag_15fe920 = 0;
    std::uint32_t tag_2ed1f54 = 0;
    std::uint32_t tag_2ed37a4 = 0;
};

struct Init8be60Result {
    bool ok = false;
    const char* blocked = nullptr;
    void* allocation = nullptr;
    int alloc_calls = 0;
    int copy_calls = 0;
    int tail_calls = 0;
    std::uint32_t format = 0;
    std::uint32_t object_word0 = 0;
    std::uint32_t object_word4 = 0;
    std::uint32_t object_word8 = 0;
    std::uint32_t object_word_c = 0;
};

// Exact native model of 0x8be60. `a1`=esi, `a2`=edx, `a3`=ecx, `a4`=r8d, `a5`=r9,
// `stack0` is the 64-bit first stack argument at [rbp+0x10].
[[nodiscard]] Init8be60Result run_init_8be60_body(
    void* object, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4,
    void* a5, std::uint64_t stack0, const Init8be60Hooks& hooks) noexcept;

struct Init8f520Result {
    std::uint64_t q0 = 0;  // dwords at rbp-0x40 (d0) and rbp-0x3c (d1), returned in rax
    std::uint64_t q1 = 0;  // the 8-byte record at rbp-0x38, returned in rdx
};

// Exact native model of 0x8f520. rdi is a dead owner argument (the body replaces rdi with
// the local record pointer before its first call). Register args are rsi/rdx/rcx/r8/r9;
// stack args are [rbp+0x10]..[rbp+0x40].
[[nodiscard]] Init8f520Result run_init_8f520_body(
    std::uint32_t rsi, std::uint32_t rdx, std::uint32_t rcx, std::uint32_t r8,
    std::uint32_t r9, std::uint32_t s0, std::uint32_t s1, std::uint32_t s2,
    std::uint32_t s3, std::uint32_t s4, std::uint32_t s5, std::uint32_t s6) noexcept;

// Exact native model of 0x8f1d0. rdi is dead. The returned rax holds the built 32-bit word
// in its low dword and r8d in its high dword.
[[nodiscard]] std::uint64_t run_init_8f1d0_body(std::uint32_t rsi, std::uint32_t rdx,
                                                std::uint32_t rcx,
                                                std::uint32_t r8) noexcept;

struct Init5d5410Hooks {
    void* object_a = nullptr;
    void* object_b = nullptr;
    std::uint64_t global_owner = 0;  // value loaded from 0x2ed33f0; callees ignore it
    std::uint8_t* state = nullptr;
    Init8be60Hooks be{};
};

struct Init5d5410Result {
    bool ok = false;
    const char* blocked = nullptr;
    int calls_8be60 = 0;
    int calls_8f520 = 0;
    int calls_8f1d0 = 0;
    int blocked_calls = 0;
    std::uint64_t owner = 0;
    Init8be60Result inner_a{};
    Init8be60Result inner_b{};
    std::array<std::uint8_t, kInit5d5410StateBytes> state{};
};

// Exact native model of 0x5d5410. Direct calls are the native 0x8be60/0x8f520/0x8f1d0
// bodies above; the only injected calls are the allocator and unobserved platform tail.
[[nodiscard]] Init5d5410Result run_init_5d5410_body(
    const Init5d5410Hooks& hooks) noexcept;

}  // namespace sotc
