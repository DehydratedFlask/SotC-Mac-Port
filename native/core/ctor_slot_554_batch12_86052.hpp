// CPU-01-STARTUP-SLOT-554-BATCH-12: native model of the 36-object body
// [0x86052,0x8849d) = 0x244b B in slot 554. Objects 0x2ebbbc8..0x2ebd7f8 = 0x1c30 B:
// seventeen 0xc0 bools, seventeen 0xd0 bounded floats on 0xc300, two 0xd0 unbounded wrappers.
// Slot 554 remains PARTIAL because batch 13 is still open.
//
// Object families (all pins are original identity tags, never executed):
//   * bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr 0x1594348,
//     functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its page byte.
//   * floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, class vtable 0x1594318,
//     atexit(0xad30); bounds are stored as full dwords at +0x6c/+0x74.
//   * two unbounded wrappers:
//       r_furStats  outer 0x6e800 / inner 0x6ebe0 (0x190 + 0x160 B), inner vptr 0x1597478,
//         final 0x15972d0, tag 0x1597348 (getter/setter 0x6e990/0x6e9b0), atexit(0x54fe0),
//         page+0x45c, no bound immediates (0/0).
//       r_furDebugMode outer 0x6e9f0 / inner 0x6ede0 (0x190 + 0x160 B), inner vptr 0x1597510,
//         final 0x1597368, tag 0x15973e0 (getter/setter 0x6eb80/0x6eba0), atexit(0x55020),
//         page+0x464, no bound immediates (0/0).
//
// Byte accounting: bool 162/30, float 171/37, wrapper 195/13;
// 17*162 + 17*171 + 2*195 = 6051 written and 17*30 + 17*37 + 2*13 = 1165 holes of 0x1c30.
// Body measurement: 1613 instructions, 180 direct calls, 19 indirect calls
// (`call *0x18`), 518 stores (191 object + 327 stack/other), no branch. Store counting is
// strict: every mov-family memory write plus every `vmovups` write; object stores are writes through
// the block's object register, with no offset cutoff. The inherited 199 calls / 148 stores are wrong;
// the measured values are 180 direct + 19 indirect and 518 strict stores. Wrapper transfers and view
// destroys are host hooks; no original RVA is executed.
//
// The inherited body end 0x8849d is measured (it holds batch 13's `leaq` of 0x2ebd7f8); the
// inherited last object start 0x2ebd728 now has its measured block end 0x2ebd7f8. The name table ends
// at 0xf9ca33, where the next unit's first name (`r_furWaterResist`) begins; the next body starts at
// 0x8849d and its second object is `0x2ebd8c8` at 0x885ab.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_functor_6af60.hpp"
#include "concmd_vars.hpp"
#include "ctor_slot_554_6efe0.hpp"
#include "ctor_slot_554_2a_6f793.hpp"
#include "ctor_slot_554_2b_6fc29.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554B12Rva = 0x86052;
inline constexpr std::size_t kSlot554B12BodySize = 0x244b;
inline constexpr std::uint64_t kSlot554B12ObjectBaseRva = 0x2ebbbc8;
inline constexpr std::uint64_t kSlot554B12ObjectEndRva = 0x2ebd7f8;
inline constexpr std::uint64_t kSlot554B12PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B12ObjectCount = 36;
inline constexpr std::size_t kSlot554B12BlockSize = 0x1c30;
inline constexpr std::size_t kSlot554B12Written = 6051;
inline constexpr std::size_t kSlot554B12Holes = 1165;
inline constexpr std::size_t kSlot554B12InstructionCount = 1613;
inline constexpr std::size_t kSlot554B12DirectCallCount = 180;
inline constexpr std::size_t kSlot554B12IndirectCallCount = 19;
inline constexpr std::size_t kSlot554B12StoreCount = 518;
inline constexpr std::size_t kSlot554B12ObjectStoreCount = 191;
inline constexpr std::size_t kSlot554B12StackStoreCount = 327;
inline constexpr std::size_t kSlot554B12BoolCount = 17;
inline constexpr std::size_t kSlot554B12BoolWritten = 162;
inline constexpr std::size_t kSlot554B12BoolHoles = 30;
inline constexpr std::size_t kSlot554B12FloatCount = 17;
inline constexpr std::size_t kSlot554B12FloatWritten = 171;
inline constexpr std::size_t kSlot554B12FloatHoles = 37;
inline constexpr std::size_t kSlot554B12DwordCount = 0;
inline constexpr std::size_t kSlot554B12DwordWritten = 0;
inline constexpr std::size_t kSlot554B12DwordHoles = 0;
inline constexpr std::size_t kSlot554B12WrapperCount = 2;
inline constexpr std::size_t kSlot554B12WrapperWritten = 195;
inline constexpr std::size_t kSlot554B12WrapperHoles = 13;
static_assert(kSlot554B12BoolCount + kSlot554B12FloatCount + kSlot554B12DwordCount +
                  kSlot554B12WrapperCount == kSlot554B12ObjectCount);
static_assert(kSlot554B12BoolWritten + kSlot554B12BoolHoles == 0xc0);
static_assert(kSlot554B12FloatWritten + kSlot554B12FloatHoles == 0xd0);
static_assert(kSlot554B12DwordWritten + kSlot554B12DwordHoles == 0);
static_assert(kSlot554B12WrapperWritten + kSlot554B12WrapperHoles == 0xd0);
static_assert(kSlot554B12BoolCount * 0xc0 +
                  (kSlot554B12ObjectCount - kSlot554B12BoolCount) * 0xd0 == kSlot554B12BlockSize);
static_assert(kSlot554B12BoolCount * kSlot554B12BoolWritten +
                  kSlot554B12FloatCount * kSlot554B12FloatWritten +
                  kSlot554B12DwordCount * kSlot554B12DwordWritten +
                  kSlot554B12WrapperCount * kSlot554B12WrapperWritten == kSlot554B12Written);
static_assert(kSlot554B12BoolCount * kSlot554B12BoolHoles +
                  kSlot554B12FloatCount * kSlot554B12FloatHoles +
                  kSlot554B12DwordCount * kSlot554B12DwordHoles +
                  kSlot554B12WrapperCount * kSlot554B12WrapperHoles == kSlot554B12Holes);
static_assert(kSlot554B12Written + kSlot554B12Holes == kSlot554B12BlockSize);
static_assert(kSlot554B12StoreCount == kSlot554B12ObjectStoreCount + kSlot554B12StackStoreCount);
static_assert(kSlot554B12ObjectEndRva - kSlot554B12ObjectBaseRva == kSlot554B12BlockSize);

inline constexpr std::uint64_t kSlot554B12TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B12TableIndex = 554;
inline constexpr std::uint64_t kSlot554B12RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B12ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B12AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B12DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B12BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B12BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B12BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B12BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B12BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B12FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B12FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B12FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B12FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B12FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B12DwordFinalVptr = 0x1594c08;
inline constexpr std::uint64_t kSlot554B12DwordArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B12DwordClassVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot554B12DwordCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot554B12DwordDtorRva = 0xad70;
inline constexpr std::size_t kSlot554B12WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B12InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B12FurStatsOuterRva = 0x6e800;
inline constexpr std::uint64_t kSlot554B12FurStatsInnerRva = 0x6ebe0;
inline constexpr std::uint64_t kSlot554B12FurStatsGetRva = 0x6e990;
inline constexpr std::uint64_t kSlot554B12FurStatsSetRva = 0x6e9b0;
inline constexpr std::uint64_t kSlot554B12FurStatsInnerVptr = 0x1597478;
inline constexpr std::uint64_t kSlot554B12FurStatsFinalVptr = 0x15972d0;
inline constexpr std::uint64_t kSlot554B12FurStatsTagVptr = 0x1597348;
inline constexpr std::uint64_t kSlot554B12FurStatsDtorRva = 0x54fe0;
inline constexpr std::uint64_t kSlot554B12FurDebugOuterRva = 0x6e9f0;
inline constexpr std::uint64_t kSlot554B12FurDebugInnerRva = 0x6ede0;
inline constexpr std::uint64_t kSlot554B12FurDebugGetRva = 0x6eb80;
inline constexpr std::uint64_t kSlot554B12FurDebugSetRva = 0x6eba0;
inline constexpr std::uint64_t kSlot554B12FurDebugInnerVptr = 0x1597510;
inline constexpr std::uint64_t kSlot554B12FurDebugFinalVptr = 0x1597368;
inline constexpr std::uint64_t kSlot554B12FurDebugTagVptr = 0x15973e0;
inline constexpr std::uint64_t kSlot554B12FurDebugDtorRva = 0x55020;
inline constexpr std::uint64_t kSlot554B12CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B12StackChkFailRva = 0xf8f3a0;

enum class Slot554B12Kind : std::uint8_t { bool_var, float_var, dword_var, wrapper };

struct Slot554B12Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B12Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;
    std::uint64_t inner_rva;
    std::uint64_t inner_vptr;
    std::uint64_t final_vptr;
    std::uint64_t tag_vptr;
    std::uint64_t dtor_rva;
};

inline constexpr Slot554B12Spec kSlot554B12Specs[kSlot554B12ObjectCount] = {
    {"r_pixelSizeOverrideEnable", 25, 0x2ebbbc8, 0xc0, Slot554B12Kind::bool_var, 0x434, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_pixelSizeOverride", 19, 0x2ebbc88, 0xd0, Slot554B12Kind::float_var, 0x438, 0x0, 0x44870000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ModelFadeOutOverrideEnable", 28, 0x2ebbd58, 0xc0, Slot554B12Kind::bool_var, 0x43c, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_ModelFadeOutStartOverride", 27, 0x2ebbe18, 0xd0, Slot554B12Kind::float_var, 0x440, 0x0, 0x44870000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ModelFadeOutEndOverride", 25, 0x2ebbee8, 0xd0, Slot554B12Kind::float_var, 0x444, 0x0, 0x44870000, 0, 0, 0, 0, 0, 0xad30},
    {"r_enableLodFadeOut", 18, 0x2ebbfb8, 0xc0, Slot554B12Kind::bool_var, 0x450, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_enablInstanceDistanceFading", 29, 0x2ebc078, 0xc0, Slot554B12Kind::bool_var, 0x448, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_fadeOutInstanceOverDistance", 29, 0x2ebc138, 0xd0, Slot554B12Kind::float_var, 0x44c, 0x0, 0x43480000, 0, 0, 0, 0, 0, 0xad30},
    {"r_skirtEnable", 13, 0x2ebc208, 0xc0, Slot554B12Kind::bool_var, 0xa90, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_EnableTwoPassSkirtRendering", 29, 0x2ebc2c8, 0xc0, Slot554B12Kind::bool_var, 0xa91, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_projectedDecalEnable", 22, 0x2ebc388, 0xc0, Slot554B12Kind::bool_var, 0xa92, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_SkirtDepthBias", 16, 0x2ebc448, 0xd0, Slot554B12Kind::float_var, 0xa94, 0xc7c35000, 0x47c35000, 0, 0, 0, 0, 0, 0xad30},
    {"r_SkirtDepthScale", 17, 0x2ebc518, 0xd0, Slot554B12Kind::float_var, 0xa98, 0xc7c35000, 0x47c35000, 0, 0, 0, 0, 0, 0xad30},
    {"r_foliageDisplacementEnable", 27, 0x2ebc5e8, 0xc0, Slot554B12Kind::bool_var, 0xac0, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_foliageDisplacementShowDisplacers", 35, 0x2ebc6a8, 0xc0, Slot554B12Kind::bool_var, 0xac1, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_foliageDisplacementMaxDistance", 32, 0x2ebc768, 0xd0, Slot554B12Kind::float_var, 0xac4, 0x3f800000, 0x43000000, 0, 0, 0, 0, 0, 0xad30},
    {"r_overrideWindDirection", 23, 0x2ebc838, 0xc0, Slot554B12Kind::bool_var, 0xa9c, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"wind_dir_x", 10, 0x2ebc8f8, 0xd0, Slot554B12Kind::float_var, 0xaa0, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"wind_dir_y", 10, 0x2ebc9c8, 0xd0, Slot554B12Kind::float_var, 0xaa4, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"wind_dir_z", 10, 0x2ebca98, 0xd0, Slot554B12Kind::float_var, 0xaa8, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"wind_intensity", 14, 0x2ebcb68, 0xd0, Slot554B12Kind::float_var, 0xaac, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"wind_speed", 10, 0x2ebcc38, 0xd0, Slot554B12Kind::float_var, 0xab0, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"gust_intensity", 14, 0x2ebcd08, 0xd0, Slot554B12Kind::float_var, 0xab4, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"gust_speed", 10, 0x2ebcdd8, 0xd0, Slot554B12Kind::float_var, 0xab8, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"r_maxWindDisplacementForCulling", 31, 0x2ebcea8, 0xd0, Slot554B12Kind::float_var, 0xabc, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_furEnable", 11, 0x2ebcf78, 0xc0, Slot554B12Kind::bool_var, 0x454, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furEnableUndercoat", 20, 0x2ebd038, 0xc0, Slot554B12Kind::bool_var, 0x455, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furEnableOvercoat", 19, 0x2ebd0f8, 0xc0, Slot554B12Kind::bool_var, 0x456, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furEnableOcclusion", 20, 0x2ebd1b8, 0xc0, Slot554B12Kind::bool_var, 0x457, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furShowPlayerCapsule", 22, 0x2ebd278, 0xc0, Slot554B12Kind::bool_var, 0x458, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furStats", 10, 0x2ebd338, 0xd0, Slot554B12Kind::wrapper, 0x45c, 0x0, 0x0, 0x6e800, 0x6ebe0, 0x1597478, 0x15972d0, 0x1597348, 0x54fe0},
    {"r_furInteractiveDebug", 21, 0x2ebd408, 0xc0, Slot554B12Kind::bool_var, 0x460, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furSimulateWhilePaused", 24, 0x2ebd4c8, 0xc0, Slot554B12Kind::bool_var, 0x461, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_furDebugMode", 14, 0x2ebd588, 0xd0, Slot554B12Kind::wrapper, 0x464, 0x0, 0x0, 0x6e9f0, 0x6ede0, 0x1597510, 0x1597368, 0x15973e0, 0x55020},
    {"r_furWetness", 12, 0x2ebd658, 0xd0, Slot554B12Kind::float_var, 0x468, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_furAirResist", 14, 0x2ebd728, 0xd0, Slot554B12Kind::float_var, 0x470, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
};

struct alignas(8) Slot554Batch12Objects {
    std::array<std::uint8_t, kSlot554B12BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch12Objects) == kSlot554B12BlockSize);

using Slot554B12RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B12AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                     ConCmdRegistry& registry) noexcept;
using Slot554B12ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B12WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                       ConCmdFunctor& object) noexcept;
using Slot554B12ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B12Hooks {
    Slot554B12RegisterHook register_var = nullptr;
    Slot554B12AtexitHook atexit = nullptr;
    Slot554B12ReleaseHook release_view = nullptr;
    Slot554B12WrapperHook wrapper_call = nullptr;
    Slot554B12ViewDestroyHook destroy_view = nullptr;
    void* context = nullptr;
};

enum class Slot554B12Stage : std::uint8_t { none, page, ctor, registration, view_release, atexit };

struct Slot554Batch12Result {
    Slot554B12Stage blocked = Slot554B12Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch12_object_written(std::uint32_t index,
                                                  std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch12_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B12Hooks default_slot554_batch12_hooks() noexcept;
[[nodiscard]] Slot554Batch12Result init_slot554_batch12(
    Slot554Batch12Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B12Hooks& hooks) noexcept;

}  // namespace sotc
