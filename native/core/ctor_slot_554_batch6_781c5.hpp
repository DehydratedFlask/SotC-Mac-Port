// CPU-01-STARTUP-SLOT-554-BATCH-06: native model of the 36-object body
// [0x781c5,0x7a68c) = 0x24c7 B in slot 554. Objects 0x2eb0f10..0x2eb2ba0 = 0x1c90 B:
// 11 0xc0 bool variables, 22 0xd0 bounded floats and three 0xd0 wrappers.
// Slot 554 remains PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 11 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * The 22 floats use 0xc300: final vptr 0x15943c0 (the hoisted 0x10(%rsp) slot), arg-fn
//     0x1596878, atexit(0xad30), and write 0x1594318 at +0xc0; bounds are pinned per object.
//   * Three wrappers, each 0x190 B outer + 0x160 B inner:
//       r_rainDebugDraw   0x6c850 / 0x6c9e0, inner vptr 0x1596250, final 0x1596da8,
//                         tag 0x1596d78, atexit(0x54da0), page+0x194, bounds 0..5;
//       AO_Mode           0x6cbe0 / 0x6cd70, inner vptr 0x15962b8, final 0x1596e30,
//                         tag 0x1596e10, atexit(0x54de0), page+0x19c, bounds 0..3;
//       BakedAOMode       0x6cf70 / 0x6d100, inner vptr 0x1596330, final 0x1596ec8,
//                         tag 0x1596ea8, atexit(0x54e20), page+0x1a0, bounds 0..1.
//
// Byte accounting: bool 162 written / 30 holes, float 171 / 37, wrapper 195 / 13;
// batch 6129 written / 1183 holes of 0x1c90. Body measurement: 1636 instructions, 177 direct
// calls, 25 indirect calls (22 functor destroys plus the three wrapper view destroys), 509
// mov-family stores (143 object + 366 stack), no conditional branch. The wrapper calls and view
// destroys are host hooks; no original RVA is executed.
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

inline constexpr std::uint64_t kSlot554B6Rva = 0x781c5;
inline constexpr std::size_t kSlot554B6BodySize = 0x24c7;
inline constexpr std::uint64_t kSlot554B6ObjectBaseRva = 0x2eb0f10;
inline constexpr std::uint64_t kSlot554B6ObjectEndRva = 0x2eb2ba0;
inline constexpr std::uint64_t kSlot554B6PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B6ObjectCount = 36;
inline constexpr std::size_t kSlot554B6BlockSize = 0x1c90;
inline constexpr std::size_t kSlot554B6Written = 6129;
inline constexpr std::size_t kSlot554B6Holes = 1183;
inline constexpr std::size_t kSlot554B6InstructionCount = 1636;
inline constexpr std::size_t kSlot554B6DirectCallCount = 177;
inline constexpr std::size_t kSlot554B6IndirectCallCount = 25;
inline constexpr std::size_t kSlot554B6StoreCount = 509;
inline constexpr std::size_t kSlot554B6ObjectStoreCount = 143;
inline constexpr std::size_t kSlot554B6StackStoreCount = 366;
inline constexpr std::size_t kSlot554B6BoolCount = 11;
inline constexpr std::size_t kSlot554B6BoolWritten = 162;
inline constexpr std::size_t kSlot554B6BoolHoles = 30;
inline constexpr std::size_t kSlot554B6FloatCount = 22;
inline constexpr std::size_t kSlot554B6FloatWritten = 171;
inline constexpr std::size_t kSlot554B6FloatHoles = 37;
inline constexpr std::size_t kSlot554B6WrapperCount = 3;
inline constexpr std::size_t kSlot554B6WrapperWritten = 195;
inline constexpr std::size_t kSlot554B6WrapperHoles = 13;
static_assert(kSlot554B6BoolCount + kSlot554B6FloatCount + kSlot554B6WrapperCount ==
              kSlot554B6ObjectCount);
static_assert(kSlot554B6BoolWritten + kSlot554B6BoolHoles == 0xc0);
static_assert(kSlot554B6FloatWritten + kSlot554B6FloatHoles == 0xd0);
static_assert(kSlot554B6WrapperWritten + kSlot554B6WrapperHoles == 0xd0);
static_assert(kSlot554B6BoolCount * 0xc0 +
                  (kSlot554B6FloatCount + kSlot554B6WrapperCount) * 0xd0 ==
              kSlot554B6BlockSize);
static_assert(kSlot554B6BoolCount * kSlot554B6BoolWritten +
                  kSlot554B6FloatCount * kSlot554B6FloatWritten +
                  kSlot554B6WrapperCount * kSlot554B6WrapperWritten == kSlot554B6Written);
static_assert(kSlot554B6BoolCount * kSlot554B6BoolHoles +
                  kSlot554B6FloatCount * kSlot554B6FloatHoles +
                  kSlot554B6WrapperCount * kSlot554B6WrapperHoles == kSlot554B6Holes);
static_assert(kSlot554B6Written + kSlot554B6Holes == kSlot554B6BlockSize);
static_assert(kSlot554B6StoreCount == kSlot554B6ObjectStoreCount + kSlot554B6StackStoreCount);

inline constexpr std::uint64_t kSlot554B6TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B6TableIndex = 554;
inline constexpr std::uint64_t kSlot554B6RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B6ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B6AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B6DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B6BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B6BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B6BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B6BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B6BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B6FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B6FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B6FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B6FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B6FloatDtorRva = 0xad30;
inline constexpr std::size_t kSlot554B6WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B6InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B6Wrapper1Rva = 0x6c850;
inline constexpr std::uint64_t kSlot554B6Wrapper1InnerRva = 0x6c9e0;
inline constexpr std::uint64_t kSlot554B6Wrapper1InnerVptr = 0x1596250;
inline constexpr std::uint64_t kSlot554B6Wrapper1FinalVptr = 0x1596da8;
inline constexpr std::uint64_t kSlot554B6Wrapper1TagVptr = 0x1596d78;
inline constexpr std::uint64_t kSlot554B6Wrapper1DtorRva = 0x54da0;
inline constexpr std::uint64_t kSlot554B6Wrapper2Rva = 0x6cbe0;
inline constexpr std::uint64_t kSlot554B6Wrapper2InnerRva = 0x6cd70;
inline constexpr std::uint64_t kSlot554B6Wrapper2InnerVptr = 0x15962b8;
inline constexpr std::uint64_t kSlot554B6Wrapper2FinalVptr = 0x1596e30;
inline constexpr std::uint64_t kSlot554B6Wrapper2TagVptr = 0x1596e10;
inline constexpr std::uint64_t kSlot554B6Wrapper2DtorRva = 0x54de0;
inline constexpr std::uint64_t kSlot554B6Wrapper3Rva = 0x6cf70;
inline constexpr std::uint64_t kSlot554B6Wrapper3InnerRva = 0x6d100;
inline constexpr std::uint64_t kSlot554B6Wrapper3InnerVptr = 0x1596330;
inline constexpr std::uint64_t kSlot554B6Wrapper3FinalVptr = 0x1596ec8;
inline constexpr std::uint64_t kSlot554B6Wrapper3TagVptr = 0x1596ea8;
inline constexpr std::uint64_t kSlot554B6Wrapper3DtorRva = 0x54e20;
inline constexpr std::uint64_t kSlot554B6CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B6StackChkFailRva = 0xf8f3a0;

enum class Slot554B6Kind : std::uint8_t { bool_var, float_var, wrapper };

struct Slot554B6Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B6Kind kind;
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

inline constexpr Slot554B6Spec kSlot554B6Specs[kSlot554B6ObjectCount] = {
    {"r_ShowMaxShadowFOVSoFar", 23, 0x2eb0f10, 0xc0, Slot554B6Kind::bool_var, 0xe8, 0x0, 0x0,
     0, 0, 0, 0, 0, 0},
    {"r_EnableOptimizedFrustumCulling", 31, 0x2eb0fd0, 0xc0, Slot554B6Kind::bool_var, 0xea, 0x0,
     0x0, 0, 0, 0, 0, 0, 0},
    {"r_EnableShadowFadeout", 21, 0x2eb1090, 0xc0, Slot554B6Kind::bool_var, 0xeb, 0x0, 0x0, 0, 0,
     0, 0, 0, 0},
    {"r_EnableShadowTemporalFilter", 28, 0x2eb1150, 0xc0, Slot554B6Kind::bool_var, 0xec, 0x0, 0x0,
     0, 0, 0, 0, 0, 0},
    {"r_ShadowTemporalFilterPixelOffset", 33, 0x2eb1210, 0xd0, Slot554B6Kind::float_var, 0xf0,
     0x0, 0x42c80000, 0, 0, 0, 0, 0, 0},
    {"sunShadowPrintDebugInfo", 23, 0x2eb12e0, 0xc0, Slot554B6Kind::bool_var, 0xf4, 0x0, 0x0, 0,
     0, 0, 0, 0, 0},
    {"sunShadowNew", 12, 0x2eb13a0, 0xc0, Slot554B6Kind::bool_var, 0xf5, 0x0, 0x0, 0, 0, 0, 0, 0,
     0},
    {"sunShadowEnableHighRes", 22, 0x2eb1460, 0xc0, Slot554B6Kind::bool_var, 0xf6, 0x0, 0x0, 0, 0,
     0, 0, 0, 0},
    {"sunShadowDebugDrawCascades", 26, 0x2eb1520, 0xc0, Slot554B6Kind::bool_var, 0xf7, 0x0, 0x0,
     0, 0, 0, 0, 0, 0},
    {"sunShadowViewportFOV", 20, 0x2eb15e0, 0xd0, Slot554B6Kind::float_var, 0xf8, 0x3c23d70a,
     0x43340000, 0, 0, 0, 0, 0, 0},
    {"sunShadowMaxDistance", 20, 0x2eb16b0, 0xd0, Slot554B6Kind::float_var, 0xfc, 0x0, 0x447a0000,
     0, 0, 0, 0, 0, 0},
    {"sunShadowCascadeDistance0", 25, 0x2eb1780, 0xd0, Slot554B6Kind::float_var, 0x100, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"sunShadowCascadeDistance1", 25, 0x2eb1850, 0xd0, Slot554B6Kind::float_var, 0x104, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"sunShadowCascadeDistance2", 25, 0x2eb1920, 0xd0, Slot554B6Kind::float_var, 0x108, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"sunShadowCascadeDistance3", 25, 0x2eb19f0, 0xd0, Slot554B6Kind::float_var, 0x10c, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"sunShadowCascadeDistance4", 25, 0x2eb1ac0, 0xd0, Slot554B6Kind::float_var, 0x110, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"sunShadowLightDistance", 22, 0x2eb1b90, 0xd0, Slot554B6Kind::float_var, 0x114, 0x0,
     0x447a0000, 0, 0, 0, 0, 0, 0},
    {"r_EnableRainBlockers", 20, 0x2eb1c60, 0xc0, Slot554B6Kind::bool_var, 0x190, 0x0, 0x0, 0, 0,
     0, 0, 0, 0},
    {"r_rainDebugDraw", 15, 0x2eb1d20, 0xd0, Slot554B6Kind::wrapper, 0x194, 0x0, 0x5,
     kSlot554B6Wrapper1Rva, kSlot554B6Wrapper1InnerRva, kSlot554B6Wrapper1InnerVptr,
     kSlot554B6Wrapper1FinalVptr, kSlot554B6Wrapper1TagVptr, kSlot554B6Wrapper1DtorRva},
    {"AO_Mode", 7, 0x2eb1df0, 0xd0, Slot554B6Kind::wrapper, 0x19c, 0x0, 0x3,
     kSlot554B6Wrapper2Rva, kSlot554B6Wrapper2InnerRva, kSlot554B6Wrapper2InnerVptr,
     kSlot554B6Wrapper2FinalVptr, kSlot554B6Wrapper2TagVptr, kSlot554B6Wrapper2DtorRva},
    {"BakedAOMode", 11, 0x2eb1ec0, 0xd0, Slot554B6Kind::wrapper, 0x1a0, 0x0, 0x1,
     kSlot554B6Wrapper3Rva, kSlot554B6Wrapper3InnerRva, kSlot554B6Wrapper3InnerVptr,
     kSlot554B6Wrapper3FinalVptr, kSlot554B6Wrapper3TagVptr, kSlot554B6Wrapper3DtorRva},
    {"Ssao_DoTemporalBlur", 19, 0x2eb1f90, 0xc0, Slot554B6Kind::bool_var, 0x1a4, 0x0, 0x0, 0, 0,
     0, 0, 0, 0},
    {"Ssao_UseWorldNormals", 20, 0x2eb2050, 0xc0, Slot554B6Kind::bool_var, 0x1a5, 0x0, 0x0, 0, 0,
     0, 0, 0, 0},
    {"Ssao_FadeDistanceScale", 22, 0x2eb2110, 0xd0, Slot554B6Kind::float_var, 0x1a8, 0x0,
     0x42c80000, 0, 0, 0, 0, 0, 0},
    {"Ssao_SampleDistanceScale", 24, 0x2eb21e0, 0xd0, Slot554B6Kind::float_var, 0x1ac, 0x0,
     0x42c80000, 0, 0, 0, 0, 0, 0},
    {"Ssao_IntensityExp", 17, 0x2eb22b0, 0xd0, Slot554B6Kind::float_var, 0x1b0, 0x0, 0x42c80000,
     0, 0, 0, 0, 0, 0},
    {"Hbao_Intensity", 14, 0x2eb2380, 0xd0, Slot554B6Kind::float_var, 0x1b4, 0x0, 0x41200000, 0,
     0, 0, 0, 0, 0},
    {"Hbao_Falloff", 12, 0x2eb2450, 0xd0, Slot554B6Kind::float_var, 0x1b8, 0x0, 0x41200000, 0, 0,
     0, 0, 0, 0},
    {"Hbao_AngleTolerance", 19, 0x2eb2520, 0xd0, Slot554B6Kind::float_var, 0x1bc, 0x0, 0x3f800000,
     0, 0, 0, 0, 0, 0},
    {"Hbao_Radius", 11, 0x2eb25f0, 0xd0, Slot554B6Kind::float_var, 0x1c0, 0x0, 0x41200000, 0, 0,
     0, 0, 0, 0},
    {"Hbao_RadiusScreenMin", 20, 0x2eb26c0, 0xd0, Slot554B6Kind::float_var, 0x1c4, 0x0,
     0x3f800000, 0, 0, 0, 0, 0, 0},
    {"Hbao_RadiusScreenMax", 20, 0x2eb2790, 0xd0, Slot554B6Kind::float_var, 0x1c8, 0x0,
     0x3f800000, 0, 0, 0, 0, 0, 0},
    {"Hbao_MipFactor", 14, 0x2eb2860, 0xd0, Slot554B6Kind::float_var, 0x1cc, 0x0, 0x447a0000, 0,
     0, 0, 0, 0, 0},
    {"Hbao_MaxAOLinearDepth", 21, 0x2eb2930, 0xd0, Slot554B6Kind::float_var, 0x1d0, 0x0,
     0x459c4000, 0, 0, 0, 0, 0, 0},
    {"BakedAO_NearDistance", 20, 0x2eb2a00, 0xd0, Slot554B6Kind::float_var, 0x1d4, 0x0,
     0x44fa0000, 0, 0, 0, 0, 0, 0},
    {"BakedAO_FadeRange", 17, 0x2eb2ad0, 0xd0, Slot554B6Kind::float_var, 0x1d8, 0x3f800000,
     0x447a0000, 0, 0, 0, 0, 0, 0},
};

struct alignas(8) Slot554Batch6Objects {
    std::array<std::uint8_t, kSlot554B6BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch6Objects) == kSlot554B6BlockSize);

struct Slot554Batch6Result;
using Slot554B6RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B6AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B6ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B6WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                      ConCmdFunctor& object) noexcept;
using Slot554B6ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B6Hooks {
    Slot554B6RegisterHook register_var = nullptr;
    Slot554B6AtexitHook atexit = nullptr;
    Slot554B6ReleaseHook release_view = nullptr;
    Slot554B6WrapperHook wrapper_call = nullptr;       // per-wrapper outer RVA
    Slot554B6ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B6Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch6Result {
    Slot554B6Stage blocked = Slot554B6Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch6_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch6_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B6Hooks default_slot554_batch6_hooks() noexcept;
[[nodiscard]] Slot554Batch6Result init_slot554_batch6(
    Slot554Batch6Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B6Hooks& hooks) noexcept;

}  // namespace sotc
