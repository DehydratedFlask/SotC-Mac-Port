// CPU-01-STARTUP-SLOT-554-BATCH-13: native model of the final batch in slot 554.
// Body [0x8849d,0x8a618) = 0x217b B. Measured shape: 31 block variables at
// 0x2ebd7f8..0x2ebf0d0 (0x18d8 B) plus a separate 0xd0 `option_unavailable` command at
// 0x2ebf0d0 (32 starts, 0x19a8 total stride). The dummy is not a block variable. It is a
// 0xa0 ConCmdCommand shape with 0x30 trailing holes; its vtable slot 2 is register 0x20d60 and
// its atexit target is 0x4b0. All vtable values are identity tags and are never executed.
//
// Families: 7 bool, 14 c300 bounded float, 3 page-cell c580 dword, 3 object-owned d8 c580
// dword, 4 wrappers. The wrappers call 0x6ebe0/0x6ede0 directly and install final vtables
// 0x1597410/0x15974a8 (tag_c0 0x1597478/0x1597510, dtor 0x55060/0x550a0).
//
// Body measurement: 1,501 instructions, 161 direct + 25 indirect calls, 457 stores
// (120 object = 115 block + 5 dummy; 337 stack), two branches. Direct call histogram:
// f590 64, f8f390 32, 20d60 31, c300 14, 20af0 8, c580 6, 6ebe0 2, 6ede0 2, f2f0 1,
// f8f3a0 1. The 6 c580 calls and 32 atexit calls reconcile with the six c580 starts and 32 starts.
//
// Byte maps: bool 162/30, c300 171/37, c580 page 171/37, c580 owned 175/41, wrapper 195/13,
// command 149/59. Block 5346/1014; total 5495/1073 of 0x19a8. Slot 554 is fully lifted when
// this unit and all prior batches are green.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_functor_6af60.hpp"
#include "concmd_vars.hpp"
#include "fconcmd_object_slot306.hpp"
#include "ctor_slot_554_2a_6f793.hpp"
#include "ctor_slot_554_2b_6fc29.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554B13Rva = 0x8849d;
inline constexpr std::size_t kSlot554B13BodySize = 0x217b;
inline constexpr std::uint64_t kSlot554B13ObjectBaseRva = 0x2ebd7f8;
inline constexpr std::uint64_t kSlot554B13ObjectEndRva = 0x2ebf0d0;
inline constexpr std::uint64_t kSlot554B13DummyRva = 0x2ebf0d0;
inline constexpr std::size_t kSlot554B13ObjectCount = 32;
inline constexpr std::size_t kSlot554B13BlockCount = 31;
inline constexpr std::size_t kSlot554B13BlockSize = 0x18d8;
inline constexpr std::size_t kSlot554B13DummySize = 0xd0;
inline constexpr std::size_t kSlot554B13TotalSize = 0x19a8;
inline constexpr std::size_t kSlot554B13Written = 5495;
inline constexpr std::size_t kSlot554B13Holes = 1073;
inline constexpr std::size_t kSlot554B13InstructionCount = 1501;
inline constexpr std::size_t kSlot554B13DirectCallCount = 161;
inline constexpr std::size_t kSlot554B13IndirectCallCount = 25;
inline constexpr std::size_t kSlot554B13StoreCount = 457;
inline constexpr std::size_t kSlot554B13ObjectStoreCount = 120;
inline constexpr std::size_t kSlot554B13StackStoreCount = 337;
inline constexpr std::size_t kSlot554B13BoolCount = 7;
inline constexpr std::size_t kSlot554B13BoolWritten = 162;
inline constexpr std::size_t kSlot554B13BoolHoles = 30;
inline constexpr std::size_t kSlot554B13FloatCount = 14;
inline constexpr std::size_t kSlot554B13FloatWritten = 171;
inline constexpr std::size_t kSlot554B13FloatHoles = 37;
inline constexpr std::size_t kSlot554B13DwordPageCount = 3;
inline constexpr std::size_t kSlot554B13DwordPageWritten = 171;
inline constexpr std::size_t kSlot554B13DwordPageHoles = 37;
inline constexpr std::size_t kSlot554B13DwordOwnedCount = 3;
inline constexpr std::size_t kSlot554B13DwordOwnedWritten = 175;
inline constexpr std::size_t kSlot554B13DwordOwnedHoles = 41;
inline constexpr std::size_t kSlot554B13WrapperCount = 4;
inline constexpr std::size_t kSlot554B13WrapperWritten = 195;
inline constexpr std::size_t kSlot554B13WrapperHoles = 13;
inline constexpr std::size_t kSlot554B13CommandCount = 1;
inline constexpr std::size_t kSlot554B13DummyWritten = 149;
inline constexpr std::size_t kSlot554B13DummyHoles = 59;
static_assert(kSlot554B13BoolCount + kSlot554B13FloatCount + kSlot554B13DwordPageCount +
                  kSlot554B13DwordOwnedCount + kSlot554B13WrapperCount == kSlot554B13BlockCount);
static_assert(kSlot554B13BlockCount + kSlot554B13CommandCount == kSlot554B13ObjectCount);
static_assert(kSlot554B13BlockSize + kSlot554B13DummySize == kSlot554B13TotalSize);
static_assert(kSlot554B13ObjectStoreCount == 115 + 5);
static_assert(kSlot554B13StoreCount == kSlot554B13ObjectStoreCount + kSlot554B13StackStoreCount);
static_assert(kSlot554B13Written + kSlot554B13Holes == kSlot554B13TotalSize);
static_assert(7 * kSlot554B13BoolWritten + 14 * kSlot554B13FloatWritten +
                  3 * kSlot554B13DwordPageWritten + 3 * kSlot554B13DwordOwnedWritten +
                  4 * kSlot554B13WrapperWritten == 5346);
static_assert(7 * kSlot554B13BoolHoles + 14 * kSlot554B13FloatHoles +
                  3 * kSlot554B13DwordPageHoles + 3 * kSlot554B13DwordOwnedHoles +
                  4 * kSlot554B13WrapperHoles == 1014);
static_assert(5346 + kSlot554B13DummyWritten == kSlot554B13Written);
static_assert(1014 + kSlot554B13DummyHoles == kSlot554B13Holes);

inline constexpr std::uint64_t kSlot554B13TableWordRva = 0x2dd5f90;
inline constexpr std::uint32_t kSlot554B13TableIndex = 554;
inline constexpr std::uint64_t kSlot554B13RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B13ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B13AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B13DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B13BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B13BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B13BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B13BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B13BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B13FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B13FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B13FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B13FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B13FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B13DwordFinalVptr = 0x1594c08;
inline constexpr std::uint64_t kSlot554B13DwordArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B13DwordClassVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot554B13DwordCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot554B13DwordDtorRva = 0xad70;
inline constexpr std::uint64_t kSlot554B13HdrCtorRva = 0x6ebe0;
inline constexpr std::uint64_t kSlot554B13HdrFinalVptr = 0x1597410;
inline constexpr std::uint64_t kSlot554B13HdrTagVptr = 0x1597478;
inline constexpr std::uint64_t kSlot554B13HdrDtorRva = 0x55060;
inline constexpr std::uint64_t kSlot554B13FourKCtorRva = 0x6ede0;
inline constexpr std::uint64_t kSlot554B13FourKFinalVptr = 0x15974a8;
inline constexpr std::uint64_t kSlot554B13FourKTagVptr = 0x1597510;
inline constexpr std::uint64_t kSlot554B13FourKDtorRva = 0x550a0;
inline constexpr std::uint64_t kSlot554B13DummyVptr = 0x1594dd0;
inline constexpr std::uint64_t kSlot554B13DummyHandlerRva = 0x550e0;
inline constexpr std::uint64_t kSlot554B13DummyDtorRva = 0x4b0;
inline constexpr std::uint32_t kSlot554B13DummyFlags = 2;
inline constexpr std::size_t kSlot554B13DummyNameLength = 18;
inline constexpr std::size_t kSlot554B13DummyDescriptionLength = 0x39;

enum class Slot554B13Kind : std::uint8_t { bool_var, float_var, dword_page, dword_owned, wrapper };

struct Slot554B13Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B13Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;
    std::uint64_t final_vptr;
    std::uint64_t tag_vptr;
    std::uint64_t dtor_rva;
    std::uint32_t owned_value;
};

inline constexpr Slot554B13Spec kSlot554B13Specs[kSlot554B13BlockCount] = {
    {"r_furWaterResist", 16, 0x2ebd7f8, 0xd0, Slot554B13Kind::float_var, 0x474, 0x0, 0x42c80000,0,0,0,0xad30,0},
    {"r_furSpringConstant", 19, 0x2ebd8c8, 0xd0, Slot554B13Kind::float_var, 0x46c, 0x0, 0x42c80000,0,0,0,0xad30,0},
    {"r_furSpeedForMaxTurbulence", 26, 0x2ebd998, 0xd0, Slot554B13Kind::float_var, 0x478, 0x0, 0x42c80000,0,0,0,0xad30,0},
    {"r_furMinTurbulenceFreq", 22, 0x2ebda68, 0xd0, Slot554B13Kind::float_var, 0x47c, 0x0, 0x41f00000,0,0,0,0xad30,0},
    {"r_furMaxTurbulenceFreq", 22, 0x2ebdb38, 0xd0, Slot554B13Kind::float_var, 0x480, 0x0, 0x41f00000,0,0,0,0xad30,0},
    {"r_furMotionX", 12, 0x2ebdc08, 0xd0, Slot554B13Kind::float_var, 0x484, 0xc1200000, 0x41200000,0,0,0,0xad30,0},
    {"r_furMotionY", 12, 0x2ebdcd8, 0xd0, Slot554B13Kind::float_var, 0x488, 0xc1200000, 0x41200000,0,0,0,0xad30,0},
    {"r_furMotionZ", 12, 0x2ebdda8, 0xd0, Slot554B13Kind::float_var, 0x48c, 0xc1200000, 0x41200000,0,0,0,0xad30,0},
    {"r_enableVirtualGeometry", 23, 0x2ebde78, 0xc0, Slot554B13Kind::bool_var, 0x490, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_showVirtualGeometryStats", 26, 0x2ebdf38, 0xc0, Slot554B13Kind::bool_var, 0x491, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_showVirtualGeometrySoundEmitters", 34, 0x2ebdff8, 0xc0, Slot554B13Kind::bool_var, 0x492, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_debugVirtualGeometrySoundEmitters", 35, 0x2ebe0b8, 0xc0, Slot554B13Kind::bool_var, 0x493, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_virtualGeometryFadeMultiplier", 31, 0x2ebe178, 0xd0, Slot554B13Kind::float_var, 0x494, 0x0, 0x3f800000,0,0,0,0xad30,0},
    {"r_virtualGeometryScaleMultiplier", 32, 0x2ebe248, 0xd0, Slot554B13Kind::float_var, 0x498, 0x0, 0x41200000,0,0,0,0xad30,0},
    {"r_virtualGeometryNumShadowCascades", 34, 0x2ebe318, 0xd0, Slot554B13Kind::dword_page, 0x49c, 0x0, 0x4,0,0,0,0xad70,0},
    {"r_virtualGeometryNumShadowCascades_Cinematic", 44, 0x2ebe3e8, 0xd8, Slot554B13Kind::dword_owned, 0x0, 0x0, 0x4,0x1594c08,0,0,0xad70,3},
    {"r_virtualGeometryNumShadowCascades_BasePS4", 42, 0x2ebe4c0, 0xd8, Slot554B13Kind::dword_owned, 0x0, 0x0, 0x4,0x1594c08,0,0,0xad70,2},
    {"r_virtualGeometryNumShadowCascades_Perf", 39, 0x2ebe598, 0xd8, Slot554B13Kind::dword_owned, 0x0, 0x0, 0x4,0x1594c08,0,0,0xad70,2},
    {"r_tempBool0", 11, 0x2ebe670, 0xc0, Slot554B13Kind::bool_var, 0x4a0, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_tempBool1", 11, 0x2ebe730, 0xc0, Slot554B13Kind::bool_var, 0x4a1, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_tempInt0", 10, 0x2ebe7f0, 0xd0, Slot554B13Kind::dword_page, 0x4a4, 0x0, 0xa,0,0,0,0xad70,0},
    {"r_tempInt1", 10, 0x2ebe8c0, 0xd0, Slot554B13Kind::dword_page, 0x4a8, 0x0, 0xa,0,0,0,0xad70,0},
    {"r_tempFloat0", 12, 0x2ebe990, 0xd0, Slot554B13Kind::float_var, 0x4ac, 0x0, 0x41200000,0,0,0,0xad30,0},
    {"r_tempFloat1", 12, 0x2ebea60, 0xd0, Slot554B13Kind::float_var, 0x4b0, 0x0, 0x447a0000,0,0,0,0xad30,0},
    {"r_WantsHDR", 10, 0x2ebeb30, 0xd0, Slot554B13Kind::wrapper, 0x4b4, 0x0, 0x2,0x6ebe0,0x1597410,0x1597478,0x55060,0},
    {"r_Wants4K", 9, 0x2ebec00, 0xd0, Slot554B13Kind::wrapper, 0x4b8, 0x0, 0x3,0x6ede0,0x15974a8,0x1597510,0x550a0,0},
    {"r_WantsHDR_Game", 15, 0x2ebecd0, 0xd0, Slot554B13Kind::wrapper, 0x4bc, 0x0, 0x2,0x6ebe0,0x1597410,0x1597478,0x55060,0},
    {"r_Wants4K_Game", 14, 0x2ebeda0, 0xd0, Slot554B13Kind::wrapper, 0x4c0, 0x0, 0x3,0x6ede0,0x15974a8,0x1597510,0x550a0,0},
    {"r_BT709GammaCurve", 17, 0x2ebee70, 0xc0, Slot554B13Kind::bool_var, 0x4c4, 0x0, 0x0,0,0,0,0xacf0,0},
    {"r_hardwareGamma", 15, 0x2ebef30, 0xd0, Slot554B13Kind::float_var, 0x4c8, 0x3dcccccd, 0x40000000,0,0,0,0xad30,0},
    {"r_hardwareGamma_HDR", 19, 0x2ebf000, 0xd0, Slot554B13Kind::float_var, 0x4cc, 0x3dcccccd, 0x40000000,0,0,0,0xad30,0},
};

struct alignas(8) Slot554Batch13Objects {
    std::array<std::uint8_t, kSlot554B13BlockSize> block{};
    std::array<std::uint8_t, kSlot554B13DummySize> dummy{};
};
static_assert(sizeof(Slot554Batch13Objects) == kSlot554B13TotalSize);

using Slot554B13RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B13AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                     ConCmdRegistry& registry) noexcept;
using Slot554B13ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B13WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                       ConCmdFunctor& object) noexcept;
using Slot554B13ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B13Hooks {
    Slot554B13RegisterHook register_var = nullptr;
    Slot554B13AtexitHook atexit = nullptr;
    Slot554B13ReleaseHook release_view = nullptr;
    Slot554B13WrapperHook wrapper_call = nullptr;
    Slot554B13ViewDestroyHook destroy_view = nullptr;
    void* context = nullptr;
};

enum class Slot554B13Stage : std::uint8_t { none, page, ctor, registration, view_release, atexit };

struct Slot554Batch13Result {
    Slot554B13Stage blocked = Slot554B13Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch13_object_written(std::uint32_t index, std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch13_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B13Hooks default_slot554_batch13_hooks() noexcept;
[[nodiscard]] Slot554Batch13Result init_slot554_batch13(
    Slot554Batch13Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B13Hooks& hooks) noexcept;

}  // namespace sotc
