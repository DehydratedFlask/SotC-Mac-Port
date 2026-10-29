// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 419 (RVA 0x56fae0, 402 bytes; the
// last instruction is the tail `jmp 0x323d0`, so there is no epilogue). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 419. Every body fact below was read end to end from the pinned bytes
// and is byte-pinned by tools/lift/check_ctor_slot_419.py.
// NOTHING here executes, loads or calls an original RVA.
//
// Shape: five 0x28-byte records at 0x2f117b0 + i * 0x28 filled by immediate stores, then a TSC stamp qword at
// 0x2f11878 (directly after the table, 0x2f117b0 + 5 * 0x28) by tail-jumping to 0x323d0, which is
// core/store_tsc_323d0 (its PLT timebase hook is passed through; the default is the documented non-zero sentinel).
// Record layout (observation; field meaning NOT recovered): +0x00 float a, +0x04 float b, +0x08 float c,
// +0x0c byte f0c, +0x0d byte f0d, +0x10 dword 0, +0x18 qword path (a rodata `$/CP3Shadow/BP_SplashScreens/_ps4/
// <name>_%s%s.ctxr` template; borrowed `const char*` here), +0x20 dword -1. Never written: +0x0e..+0x0f, +0x14..
// +0x17, +0x24..+0x27. By the template names the table lists the splash screens (epilepsy, legal_jp, sce_presents,
// JAPANstudio, bluepoint); that reading is an INFERENCE from the strings, nothing here draws a splash screen.
// No atexit is registered.
#pragma once

#include <cstddef>
#include <cstdint>

#include "store_tsc_323d0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot419Rva = 0x56fae0;
inline constexpr std::size_t kSlot419Size = 402;
inline constexpr std::uint64_t kSlot419TableRva = 0x2f117b0;
inline constexpr std::uint64_t kSlot419TscRva = 0x2f11878;
inline constexpr std::size_t kSlot419RecordSize = 0x28;
inline constexpr std::size_t kSlot419RecordCount = 5;
inline constexpr char kSlot419Path0[] = "$/CP3Shadow/BP_SplashScreens/_ps4/epilepsy_%s%s.ctxr";
inline constexpr char kSlot419Path1[] = "$/CP3Shadow/BP_SplashScreens/_ps4/legal_jp_%s%s.ctxr";
inline constexpr char kSlot419Path2[] = "$/CP3Shadow/BP_SplashScreens/_ps4/sce_presents_%s%s.ctxr";
inline constexpr char kSlot419Path3[] = "$/CP3Shadow/BP_SplashScreens/_ps4/JAPANstudio_%s%s.ctxr";
inline constexpr char kSlot419Path4[] = "$/CP3Shadow/BP_SplashScreens/_ps4/bluepoint_%s%s.ctxr";

struct SplashRecord419 {
    std::uint32_t a;            // +0x00 float bits
    std::uint32_t b;            // +0x04 float bits
    std::uint32_t c;            // +0x08 float bits
    std::uint8_t f0c;           // +0x0c
    std::uint8_t f0d;           // +0x0d
    std::uint8_t gap0e[2];      // +0x0e never written
    std::uint32_t zero10;       // +0x10 stored 0
    std::uint8_t gap14[4];      // +0x14 never written
    const char* path;           // +0x18 borrowed rodata template (the original stores the rodata address)
    std::int32_t minus1;        // +0x20 stored 0xffffffff
    std::uint8_t gap24[4];      // +0x24 never written
};
static_assert(sizeof(SplashRecord419) == kSlot419RecordSize && offsetof(SplashRecord419, f0c) == 0x0c &&
              offsetof(SplashRecord419, zero10) == 0x10 && offsetof(SplashRecord419, path) == 0x18 &&
              offsetof(SplashRecord419, minus1) == 0x20);

struct Slot419Objects {
    SplashRecord419 records[kSlot419RecordCount];   // 0x2f117b0
    std::uint64_t tsc;                              // 0x2f11878 (stamp written by store_tsc_323d0)
};
static_assert(offsetof(Slot419Objects, tsc) == kSlot419RecordCount * kSlot419RecordSize);

struct Slot419Result {
    bool ok = true;     // plain stores plus the tail leaf; the leaf itself cannot fail
};

[[nodiscard]] Slot419Result init_slot419_56fae0(Slot419Objects& objects,
                                                const StoreTsc323d0Hooks& tsc_hooks = {}) noexcept;

}  // namespace sotc
