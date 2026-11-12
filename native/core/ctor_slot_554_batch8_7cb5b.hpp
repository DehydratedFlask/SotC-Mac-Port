// CPU-01-STARTUP-SLOT-554-BATCH-08: native model of the 36-object body
// [0x7cb5b,0x7f098) = 0x253d B in slot 554. Objects 0x2eb4868..0x2eb6538 = 0x1cd0 B:
// seven 0xc0 bools, twenty-six 0xd0 bounded floats and three 0xd0 wrappers. Slot 554 remains
// PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 7 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * The 26 floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, class vtable 0x1594318,
//     atexit(0xad30), and write 0x1594318 at +0xc0; bounds are pinned per object.
//   * Three wrappers, each 0x190 B outer + 0x160 B inner:
//       - r_waterDebugDraw 0x6c850 / 0x6c9e0, inner vptr 0x1596250, final 0x1596da8,
//         tag 0x1596d78, atexit(0x54da0), page+0x270, bounds 0..5.
//       - r_tonemapMethodSDR/HDR 0x6d690 / 0x6d820, inner vptr 0x1596430, final 0x1597008,
//         tag 0x1596fd8, atexit(0x54ea0), page+0x290 / page+0x294, bounds 0..5.
//
// Byte accounting: bool 162/30, float 171/37, wrapper 195/13;
// 7*162 + 26*171 + 3*195 = 6165 written and 1211 holes of 0x1cd0.
// Body measurement: 1659 instructions, 177 direct calls, 29 indirect calls (26 float functor
// destroys + 3 wrapper view destroys), 505 mov-family stores (115 object + 390 stack), no
// branch of any kind. Wrapper transfers and view destroys are host hooks; no original RVA is
// executed.
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

inline constexpr std::uint64_t kSlot554B8Rva = 0x7cb5b;
inline constexpr std::size_t kSlot554B8BodySize = 0x253d;
inline constexpr std::uint64_t kSlot554B8ObjectBaseRva = 0x2eb4868;
inline constexpr std::uint64_t kSlot554B8ObjectEndRva = 0x2eb6538;
inline constexpr std::uint64_t kSlot554B8PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B8ObjectCount = 36;
inline constexpr std::size_t kSlot554B8BlockSize = 0x1cd0;
inline constexpr std::size_t kSlot554B8Written = 6165;
inline constexpr std::size_t kSlot554B8Holes = 1211;
inline constexpr std::size_t kSlot554B8InstructionCount = 1659;
inline constexpr std::size_t kSlot554B8DirectCallCount = 177;
inline constexpr std::size_t kSlot554B8IndirectCallCount = 29;
inline constexpr std::size_t kSlot554B8StoreCount = 505;
inline constexpr std::size_t kSlot554B8ObjectStoreCount = 115;
inline constexpr std::size_t kSlot554B8StackStoreCount = 390;
inline constexpr std::size_t kSlot554B8BoolCount = 7;
inline constexpr std::size_t kSlot554B8BoolWritten = 162;
inline constexpr std::size_t kSlot554B8BoolHoles = 30;
inline constexpr std::size_t kSlot554B8FloatCount = 26;
inline constexpr std::size_t kSlot554B8FloatWritten = 171;
inline constexpr std::size_t kSlot554B8FloatHoles = 37;
inline constexpr std::size_t kSlot554B8WrapperCount = 3;
inline constexpr std::size_t kSlot554B8WrapperWritten = 195;
inline constexpr std::size_t kSlot554B8WrapperHoles = 13;
static_assert(kSlot554B8BoolCount + kSlot554B8FloatCount + kSlot554B8WrapperCount ==
              kSlot554B8ObjectCount);
static_assert(kSlot554B8BoolWritten + kSlot554B8BoolHoles == 0xc0);
static_assert(kSlot554B8FloatWritten + kSlot554B8FloatHoles == 0xd0);
static_assert(kSlot554B8WrapperWritten + kSlot554B8WrapperHoles == 0xd0);
static_assert(kSlot554B8BoolCount * 0xc0 +
                  (kSlot554B8FloatCount + kSlot554B8WrapperCount) * 0xd0 == kSlot554B8BlockSize);
static_assert(kSlot554B8BoolCount * kSlot554B8BoolWritten +
                  kSlot554B8FloatCount * kSlot554B8FloatWritten +
                  kSlot554B8WrapperCount * kSlot554B8WrapperWritten == kSlot554B8Written);
static_assert(kSlot554B8BoolCount * kSlot554B8BoolHoles +
                  kSlot554B8FloatCount * kSlot554B8FloatHoles +
                  kSlot554B8WrapperCount * kSlot554B8WrapperHoles == kSlot554B8Holes);
static_assert(kSlot554B8Written + kSlot554B8Holes == kSlot554B8BlockSize);
static_assert(kSlot554B8StoreCount == kSlot554B8ObjectStoreCount + kSlot554B8StackStoreCount);
static_assert(kSlot554B8BodySize == 0x253d);

inline constexpr std::uint64_t kSlot554B8TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B8TableIndex = 554;
inline constexpr std::uint64_t kSlot554B8RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B8ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B8AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B8DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B8BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B8BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B8BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B8BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B8BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B8FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B8FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B8FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B8FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B8FloatDtorRva = 0xad30;
inline constexpr std::size_t kSlot554B8WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B8InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B8WaterWrapperRva = 0x6c850;
inline constexpr std::uint64_t kSlot554B8WaterWrapperInnerRva = 0x6c9e0;
inline constexpr std::uint64_t kSlot554B8WaterWrapperInnerVptr = 0x1596250;
inline constexpr std::uint64_t kSlot554B8WaterWrapperFinalVptr = 0x1596da8;
inline constexpr std::uint64_t kSlot554B8WaterWrapperTagVptr = 0x1596d78;
inline constexpr std::uint64_t kSlot554B8WaterWrapperDtorRva = 0x54da0;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperRva = 0x6d690;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperInnerRva = 0x6d820;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperInnerVptr = 0x1596430;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperFinalVptr = 0x1597008;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperTagVptr = 0x1596fd8;
inline constexpr std::uint64_t kSlot554B8TonemapWrapperDtorRva = 0x54ea0;
inline constexpr std::uint64_t kSlot554B8CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B8StackChkFailRva = 0xf8f3a0;

enum class Slot554B8Kind : std::uint8_t { bool_var, float_var, wrapper };

struct Slot554B8Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B8Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;  // 0 unless kind == wrapper
    std::uint64_t inner_rva;
    std::uint64_t inner_vptr;
    std::uint64_t final_vptr;
    std::uint64_t tag_vptr;
    std::uint64_t dtor_rva;
};

inline constexpr Slot554B8Spec kSlot554B8Specs[kSlot554B8ObjectCount] = {
    {"r_waterShowStats", 16, 0x2eb4868, 0xc0, Slot554B8Kind::bool_var, 0x248, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_waterReset", 12, 0x2eb4928, 0xc0, Slot554B8Kind::bool_var, 0x249, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_waterDebugDraw", 16, 0x2eb49e8, 0xd0, Slot554B8Kind::wrapper, 0x270, 0, 5,
     0x6c850, 0x6c9e0, 0x1596250, 0x1596da8, 0x1596d78, 0x54da0},
    {"r_waterWaveSpeed", 16, 0x2eb4ab8, 0xd0, Slot554B8Kind::float_var, 0x24c, 0, 0x42c80000, 0, 0, 0, 0, 0, 0},
    {"r_waterDampening", 16, 0x2eb4b88, 0xd0, Slot554B8Kind::float_var, 0x250, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterFoamDecay", 16, 0x2eb4c58, 0xd0, Slot554B8Kind::float_var, 0x25c, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterHighwaterDecay", 21, 0x2eb4d28, 0xd0, Slot554B8Kind::float_var, 0x260, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterHighwaterDecayLimiter", 28, 0x2eb4df8, 0xd0, Slot554B8Kind::float_var, 0x264, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterSlopeLimit", 17, 0x2eb4ec8, 0xd0, Slot554B8Kind::float_var, 0x254, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterVolumePreservation", 25, 0x2eb4f98, 0xd0, Slot554B8Kind::float_var, 0x258, 0, 0x41200000, 0, 0, 0, 0, 0, 0},
    {"r_waterEnableDebugWetAmmount", 28, 0x2eb5068, 0xc0, Slot554B8Kind::bool_var, 0x24a, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_waterDebugWetAmmount", 22, 0x2eb5128, 0xd0, Slot554B8Kind::float_var, 0x274, 0, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_waterMicroSimVirtualOffset", 28, 0x2eb51f8, 0xd0, Slot554B8Kind::float_var, 0x268, 0, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_waterMicroSimBumpHeight", 25, 0x2eb52c8, 0xd0, Slot554B8Kind::float_var, 0x26c, 0, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_tessellationEnable", 20, 0x2eb5398, 0xc0, Slot554B8Kind::bool_var, 0x278, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_tessellationEnableAsyncCompute", 32, 0x2eb5458, 0xc0, Slot554B8Kind::bool_var, 0x279, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_tessellationTransitionBand", 28, 0x2eb5518, 0xd0, Slot554B8Kind::float_var, 0x27c, 0, 0x42c80000, 0, 0, 0, 0, 0, 0},
    {"r_tessellationRange4x", 21, 0x2eb55e8, 0xd0, Slot554B8Kind::float_var, 0x280, 0, 0x447a0000, 0, 0, 0, 0, 0, 0},
    {"r_tessellationRange16x", 22, 0x2eb56b8, 0xd0, Slot554B8Kind::float_var, 0x284, 0, 0x447a0000, 0, 0, 0, 0, 0, 0},
    {"r_tessellationRange64x", 22, 0x2eb5788, 0xd0, Slot554B8Kind::float_var, 0x288, 0, 0x447a0000, 0, 0, 0, 0, 0, 0},
    {"r_exposureEnable", 16, 0x2eb5858, 0xc0, Slot554B8Kind::bool_var, 0x28c, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_exposureInstant", 17, 0x2eb5918, 0xc0, Slot554B8Kind::bool_var, 0x28d, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_tonemapMethodSDR", 18, 0x2eb59d8, 0xd0, Slot554B8Kind::wrapper, 0x290, 0, 5,
     0x6d690, 0x6d820, 0x1596430, 0x1597008, 0x1596fd8, 0x54ea0},
    {"r_tonemapMethodHDR", 18, 0x2eb5aa8, 0xd0, Slot554B8Kind::wrapper, 0x294, 0, 5,
     0x6d690, 0x6d820, 0x1596430, 0x1597008, 0x1596fd8, 0x54ea0},
    {"r_hableShoulderStrength", 23, 0x2eb5b78, 0xd0, Slot554B8Kind::float_var, 0x298, 0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_hableLinearStrength", 21, 0x2eb5c48, 0xd0, Slot554B8Kind::float_var, 0x29c, 0, 0x40a00000, 0, 0, 0, 0, 0, 0},
    {"r_hableLinearAngle", 18, 0x2eb5d18, 0xd0, Slot554B8Kind::float_var, 0x2a0, 0, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_hableToeStrength", 18, 0x2eb5de8, 0xd0, Slot554B8Kind::float_var, 0x2a4, 0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_hableToeNumerator", 19, 0x2eb5eb8, 0xd0, Slot554B8Kind::float_var, 0x2a8, 0, 0x3f000000, 0, 0, 0, 0, 0, 0},
    {"r_hableToeDenominator", 21, 0x2eb5f88, 0xd0, Slot554B8Kind::float_var, 0x2ac, 0, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_hableExposureBias", 19, 0x2eb6058, 0xd0, Slot554B8Kind::float_var, 0x2b0, 0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_hableWhitePoint", 17, 0x2eb6128, 0xd0, Slot554B8Kind::float_var, 0x2b4, 0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesHdrOverallStrength", 24, 0x2eb61f8, 0xd0, Slot554B8Kind::float_var, 0x2d0, 0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesHdrShoulderStrength", 25, 0x2eb62c8, 0xd0, Slot554B8Kind::float_var, 0x2d4, 0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesHdrShoulderBroadAngle", 27, 0x2eb6398, 0xd0, Slot554B8Kind::float_var, 0x2d8, 0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesHdrToeToSholderAngle", 26, 0x2eb6468, 0xd0, Slot554B8Kind::float_var, 0x2dc, 0, 0x41a00000, 0, 0, 0, 0, 0, 0},
};

struct alignas(8) Slot554Batch8Objects {
    std::array<std::uint8_t, kSlot554B8BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch8Objects) == kSlot554B8BlockSize);

struct Slot554Batch8Result;
using Slot554B8RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B8AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B8ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B8WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                      ConCmdFunctor& object) noexcept;
using Slot554B8ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B8Hooks {
    Slot554B8RegisterHook register_var = nullptr;
    Slot554B8AtexitHook atexit = nullptr;
    Slot554B8ReleaseHook release_view = nullptr;
    Slot554B8WrapperHook wrapper_call = nullptr;       // per-wrapper outer RVA
    Slot554B8ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B8Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch8Result {
    Slot554B8Stage blocked = Slot554B8Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch8_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch8_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B8Hooks default_slot554_batch8_hooks() noexcept;
[[nodiscard]] Slot554Batch8Result init_slot554_batch8(
    Slot554Batch8Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B8Hooks& hooks) noexcept;

}  // namespace sotc
