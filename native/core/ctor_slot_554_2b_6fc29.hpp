// CPU-01-STARTUP-SLOT-554-BATCH-02B: native model of the eight-object prefix of the remaining
// slot-554 body. Object 0 is the 0xd0-byte `XPR Occluder Debug Mode` wrapper at 0x2eaa3d0, built by
// the 397 B wrapper 0x6abb0 (inner 0x6ad40); objects 1..7 are the seven 0xc0-byte bools at
// 0x2eaa4a0..0x2eaa9e0. This prefix is [0x6fc29,0x7036d) = 0x744 B. The rest of the body
// [0x7036d,0x718bb) is NOT claimed here and stays the next open unit.
//
// The wrapper is the same 17-member family as 0x6af60 but is not interchangeable: its inner mid
// vptr (table 0x1595e80, address point 0x1595e90), seed/caller-view tag 0x15968b9, outer final
// table 0x15968d8 (address point 0x15968e8) and inner size 0x160 are its own immediates. The model
// stores those identity tags and never executes an original RVA. Object 0 writes 195 bytes and
// leaves 13 holes (base +0x19..+0x1b, +0x71..+0x73, +0x79..+0x7f); each bool writes 162 bytes and
// leaves 30 holes exactly as batch 2a. Batch totals: 1329 written / 223 holes of 0x610. The body is
// 328 decoded instructions: 39 direct calls (0x6abb0 x1, 0x20af0 x7, 0xf590 x16, 0x20d60 x7,
// 0xf8f390 x8), one indirect `call [rax+0x18]` view destroy, no conditional branch and no canary;
// 63 direct stores build the seven bools and 54 stack stores prepare the constructor arguments. No
// boot, audio, Metal or save claim is made.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_functor_6af60.hpp"
#include "ctor_slot_554_2a_6f793.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554B2BRva = 0x6fc29;
inline constexpr std::size_t kSlot554B2BBodySize = 0x744;
inline constexpr std::uint64_t kSlot554B2BObjectBaseRva = 0x2eaa3d0;
inline constexpr std::uint64_t kSlot554B2BPageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B2BObjectCount = 8;
inline constexpr std::size_t kSlot554B2BBoolCount = 7;
inline constexpr std::size_t kSlot554B2BFunctorSize = 0xd0;
inline constexpr std::size_t kSlot554B2BBoolSize = 0xc0;
inline constexpr std::size_t kSlot554B2BBlockSize = 0x610;
static_assert(kSlot554B2BFunctorSize + kSlot554B2BBoolCount * kSlot554B2BBoolSize ==
              kSlot554B2BBlockSize);
inline constexpr std::size_t kSlot554B2BWritten = 1329;
inline constexpr std::size_t kSlot554B2BHoles = 223;
inline constexpr std::size_t kSlot554B2BInstructionCount = 328;
inline constexpr std::size_t kSlot554B2BDirectCallCount = 39;
inline constexpr std::size_t kSlot554B2BIndirectCallCount = 1;
inline constexpr std::size_t kSlot554B2BObjectStoreCount = 63;
inline constexpr std::size_t kSlot554B2BStackStoreCount = 54;
inline constexpr std::size_t kSlot554B2BFunctorWritten = 195;
inline constexpr std::size_t kSlot554B2BFunctorHoles = 13;
inline constexpr std::size_t kSlot554B2BBoolWritten = 162;
inline constexpr std::size_t kSlot554B2BBoolHoles = 30;
static_assert(kSlot554B2BFunctorWritten + kSlot554B2BFunctorHoles == kSlot554B2BFunctorSize);
static_assert(kSlot554B2BBoolWritten + kSlot554B2BBoolHoles == kSlot554B2BBoolSize);
static_assert(kSlot554B2BFunctorWritten + kSlot554B2BBoolCount * kSlot554B2BBoolWritten ==
              kSlot554B2BWritten);
static_assert(kSlot554B2BFunctorHoles + kSlot554B2BBoolCount * kSlot554B2BBoolHoles ==
              kSlot554B2BHoles);
static_assert(kSlot554B2BWritten + kSlot554B2BHoles == kSlot554B2BBlockSize);

inline constexpr std::uint64_t kSlot554B2BTableWordRva = 0x2dd5f90;   // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B2BTableIndex = 554;
inline constexpr std::uint64_t kSlot554B2BWrapperRva = 0x6abb0;
inline constexpr std::size_t kSlot554B2BWrapperSize = 0x18d;
inline constexpr std::uint64_t kSlot554B2BInnerRva = 0x6ad40;
inline constexpr std::size_t kSlot554B2BInnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B2BInnerMidVptr = 0x1595e90;   // 0x1595e80 + 0x10
inline constexpr std::uint64_t kSlot554B2BFinalVptr = 0x15968e8;      // 0x15968d8 + 0x10
inline constexpr std::uint64_t kSlot554B2BTagVptr = 0x15968b9;
inline constexpr std::uint64_t kSlot554B2BViewVptr = 0x15968b9;
inline constexpr std::uint64_t kSlot554B2BRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B2BAtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B2BDtorRva = 0x54b60;
inline constexpr std::uint64_t kSlot554B2BBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B2BReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B2BCanaryCell = 0x1593968;
inline constexpr std::uint64_t kSlot554B2BStackChkFailRva = 0xf8f3a0;
inline constexpr std::uint32_t kSlot554B2BOccluderLow = 0;
inline constexpr std::uint32_t kSlot554B2BOccluderHigh = 2;
inline constexpr std::uint16_t kSlot554B2BOccluderPageOffset = 0x18;

struct Slot554B2BSpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t page_cell_offset;
};

inline constexpr Slot554B2BSpec kSlot554B2BFunctor = {
    "XPR Occluder Debug Mode", 23, 0x2eaa3d0, 0x18,
};

inline constexpr Slot554B2BSpec kSlot554B2BBools[kSlot554B2BBoolCount] = {
    {"enableNonPower2DepthOccluders", 29, 0x2eaa4a0, 0x1c},
    {"dumpXPRInfoStats", 16, 0x2eaa560, 0x1d},
    {"useSunShadowIndices", 19, 0x2eaa620, 0x1e},
    {"doIndexBufferCulling", 20, 0x2eaa6e0, 0x1f},
    {"doSunShadowIndexBufferCulling", 29, 0x2eaa7a0, 0x20},
    {"enableSmallPrimitiveIndexBufferCulling", 38, 0x2eaa860, 0x21},
    {"forceUseLastIndexBufferCullingResults", 37, 0x2eaa920, 0x22},
};

struct alignas(8) Slot554Batch2BObjects {
    std::array<std::uint8_t, kSlot554B2BBlockSize> objects{};
};
static_assert(sizeof(Slot554Batch2BObjects) == kSlot554B2BBlockSize);

struct Slot554Batch2BResult;
using Slot554B2BRegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B2BAtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                     ConCmdRegistry& registry) noexcept;
using Slot554B2BReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B2BTransferHook = bool (*)(void* context, ConCmdFunctor& object,
                                        unsigned stage) noexcept;
using Slot554B2BViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B2BHooks {
    Slot554B2BRegisterHook register_var = nullptr;
    Slot554B2BAtexitHook atexit = nullptr;
    Slot554B2BReleaseHook release_view = nullptr;
    Slot554B2BTransferHook transfer = nullptr;         // five type-erased wrapper calls
    Slot554B2BViewDestroyHook destroy_view = nullptr;  // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B2BStage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch2BResult {
    Slot554B2BStage blocked = Slot554B2BStage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t transfers = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch2b_object_written(std::uint32_t index,
                                                   std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch2b_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B2BHooks default_slot554_batch2b_hooks() noexcept;
[[nodiscard]] Slot554Batch2BResult init_slot554_batch2b(
    Slot554Batch2BObjects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B2BHooks& hooks) noexcept;

}  // namespace sotc
