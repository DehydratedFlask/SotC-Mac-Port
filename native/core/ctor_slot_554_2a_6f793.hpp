// CPU-01-STARTUP-SLOT-554-BATCH-02A: native model of the five-bool prefix of slot 554's second
// body batch. Body [0x6f793,0x6fc29) = 0x496 B, objects 0x2eaa010..0x2eaa3d0 = 0x3c0 B.
//
// The plan estimated a uniform 36-object batch here. The ELF says otherwise: objects 0..4 are
// five 0xc0-byte bool variables, and object 5 switches to the 0xd0-byte 0x6abb0 ranged/functor
// family. Batch 2a is therefore the clean five-object prefix; batch 2b is the remaining 31 objects
// and stays open. The constructor-table word is 0x2dd70e0 - 554*8 = 0x2dd5f90 and holds 0x6efe0.
//
// Each object: 0x20af0 base, vptr 0x15940b0, value_ref -> page cell, zeroed +0x69/+0x70/+0xa0,
// functor tag 0x1596898, released description, final vptr 0x1594348, byte default from the page,
// 0x20d60 registration, released name, atexit 0xacf0. The byte mask is the batch-1 bool shape:
// 162 written / 30 holes per object, 810/150 for five. No original RVA is executed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554B2ARva = 0x6f793;
inline constexpr std::size_t kSlot554B2ABodySize = 0x496;
inline constexpr std::uint64_t kSlot554B2AObjectBaseRva = 0x2eaa010;
inline constexpr std::uint64_t kSlot554B2APageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B2AObjectCount = 5;
inline constexpr std::size_t kSlot554B2AObjectSize = 0xc0;
inline constexpr std::size_t kSlot554B2ABlockSize = 0x3c0;
static_assert(kSlot554B2AObjectCount * kSlot554B2AObjectSize == kSlot554B2ABlockSize);
inline constexpr std::size_t kSlot554B2AWritten = 810;
inline constexpr std::size_t kSlot554B2AHoles = 150;
inline constexpr std::size_t kSlot554B2AObjectWritten = 162;
inline constexpr std::size_t kSlot554B2AObjectHoles = 30;
static_assert(kSlot554B2AObjectWritten + kSlot554B2AObjectHoles == kSlot554B2AObjectSize);
static_assert(kSlot554B2AObjectCount * kSlot554B2AObjectWritten == kSlot554B2AWritten);
static_assert(kSlot554B2AObjectCount * kSlot554B2AObjectHoles == kSlot554B2AHoles);
static_assert(kSlot554B2AWritten + kSlot554B2AHoles == kSlot554B2ABlockSize);

inline constexpr std::uint64_t kSlot554B2ATableWordRva = 0x2dd5f90;   // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B2ATableIndex = 554;
inline constexpr std::uint64_t kSlot554B2AFinalVptr = 0x1594348;      // 0x1594338 + 0x10
inline constexpr std::uint64_t kSlot554B2AMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B2AFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B2ADtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B2ARegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B2AReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B2AAtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B2ADsoRva = 0x1604218;

struct Slot554B2ASpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t page_cell_offset;
};

inline constexpr Slot554B2ASpec kSlot554B2AVars[kSlot554B2AObjectCount] = {
    {"threadedCulling", 15, 0x2eaa010, 0x12},
    {"threadedRendering", 17, 0x2eaa0d0, 0x13},
    {"threadedSumbit", 14, 0x2eaa190, 0x14},
    {"enableAsyncCompute", 18, 0x2eaa250, 0x15},
    {"enableGPUCounters", 17, 0x2eaa310, 0x16},
};

struct Slot554B2ABool {
    ConCmdBase base;             // +0x00
    std::uint8_t* value_ref;     // +0x60
    std::uint8_t default_value;  // +0x68
    std::uint8_t zero69[4];      // +0x69..+0x6c
    std::uint8_t gap6d[3];       // +0x6d..+0x6f
    std::uint8_t zero70[0x20];   // +0x70..+0x8f
    std::uint8_t gap90[0x10];    // +0x90..+0x9f
    std::uint64_t fn_payload;    // +0xa0
    std::uint8_t gapa8[8];       // +0xa8..+0xaf
    std::uint64_t fn_vptr;       // +0xb0
    std::uint64_t fn_aux;        // +0xb8
};
static_assert(sizeof(Slot554B2ABool) == kSlot554B2AObjectSize);
static_assert(offsetof(Slot554B2ABool, value_ref) == 0x60);
static_assert(offsetof(Slot554B2ABool, default_value) == 0x68);
static_assert(offsetof(Slot554B2ABool, fn_vptr) == 0xb0);

struct alignas(8) Slot554Batch2AObjects {
    std::array<std::uint8_t, kSlot554B2ABlockSize> objects{};
};
static_assert(sizeof(Slot554Batch2AObjects) == kSlot554B2ABlockSize);

struct Slot554Batch2AResult;
using Slot554B2ARegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B2AAtexitHook = bool (*)(std::uint64_t dtor, const void* object, ConCmdRegistry& registry) noexcept;
using Slot554B2AReleaseHook = bool (*)(const SharedStr& view) noexcept;

struct Slot554B2AHooks {
    Slot554B2ARegisterHook register_var = nullptr;
    Slot554B2AAtexitHook atexit = nullptr;
    Slot554B2AReleaseHook release_view = nullptr;
};

enum class Slot554B2AStage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch2AResult {
    Slot554B2AStage blocked = Slot554B2AStage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch2a_object_written(std::uint32_t index, std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch2a_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B2AHooks default_slot554_batch2a_hooks() noexcept;
[[nodiscard]] Slot554Batch2AResult init_slot554_batch2a(Slot554Batch2AObjects& objects,
                                                         const std::uint8_t* page,
                                                         ConCmdRegistry& registry,
                                                         const Slot554B2AHooks& hooks) noexcept;

}  // namespace sotc
