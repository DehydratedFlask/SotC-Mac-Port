// CPU-01-STARTUP-SLOT-554-BATCH-05: native model of the 36-object body
// [0x75d22,0x781c5) = 0x24a3 B in slot 554. Objects 0x2eaf2a0..0x2eb0f10 = 0x1c70 B:
// 13 0xc0 bool variables, 22 0xd0 bounded floats and one 0xd0 wrapper.
// Slot 554 remains PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 13 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * The 22 floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, atexit(0xad30), and
//     write 0x1594318 at +0xc0; bounds are pinned per object below.
//   * showBakedLightingType: 0x6c4c0 outer wrapper (0x190 B) calling the 0x6c650 inner
//     (0x160 B); inner vptr 0x15961d8 (0x15961c8 + 0x10), final vptr 0x1596d10 (table
//     0x1596d00 + 0x10), tag 0x1596ce0, atexit(0x54d60), page+0x178, bounds 0..4.
//
// Byte accounting: bool 162 written / 30 holes, float 171 / 37, wrapper 195 / 13;
// batch 6063 written / 1217 holes of 0x1c70. Body measurement: 1628 instructions, 179 direct
// calls, 23 indirect calls (22 functor destroys plus the wrapper view destroy), 516 mov-family
// stores (161 object + 355 stack), no conditional branch. The wrapper call and view destroy are
// host hooks; no original RVA is executed.
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

inline constexpr std::uint64_t kSlot554B5Rva = 0x75d22;
inline constexpr std::size_t kSlot554B5BodySize = 0x24a3;
inline constexpr std::uint64_t kSlot554B5ObjectBaseRva = 0x2eaf2a0;
inline constexpr std::uint64_t kSlot554B5ObjectEndRva = 0x2eb0f10;
inline constexpr std::uint64_t kSlot554B5PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B5ObjectCount = 36;
inline constexpr std::size_t kSlot554B5BlockSize = 0x1c70;
inline constexpr std::size_t kSlot554B5Written = 6063;
inline constexpr std::size_t kSlot554B5Holes = 1217;
inline constexpr std::size_t kSlot554B5InstructionCount = 1628;
inline constexpr std::size_t kSlot554B5DirectCallCount = 179;
inline constexpr std::size_t kSlot554B5IndirectCallCount = 23;
inline constexpr std::size_t kSlot554B5StoreCount = 516;
inline constexpr std::size_t kSlot554B5ObjectStoreCount = 161;
inline constexpr std::size_t kSlot554B5StackStoreCount = 355;
inline constexpr std::size_t kSlot554B5BoolCount = 13;
inline constexpr std::size_t kSlot554B5BoolWritten = 162;
inline constexpr std::size_t kSlot554B5BoolHoles = 30;
inline constexpr std::size_t kSlot554B5FloatCount = 22;
inline constexpr std::size_t kSlot554B5FloatWritten = 171;
inline constexpr std::size_t kSlot554B5FloatHoles = 37;
inline constexpr std::size_t kSlot554B5WrapperCount = 1;
inline constexpr std::size_t kSlot554B5WrapperWritten = 195;
inline constexpr std::size_t kSlot554B5WrapperHoles = 13;
static_assert(kSlot554B5BoolCount + kSlot554B5FloatCount + kSlot554B5WrapperCount ==
              kSlot554B5ObjectCount);
static_assert(kSlot554B5BoolWritten + kSlot554B5BoolHoles == 0xc0);
static_assert(kSlot554B5FloatWritten + kSlot554B5FloatHoles == 0xd0);
static_assert(kSlot554B5WrapperWritten + kSlot554B5WrapperHoles == 0xd0);
static_assert(kSlot554B5BoolCount * 0xc0 +
                  (kSlot554B5FloatCount + kSlot554B5WrapperCount) * 0xd0 ==
              kSlot554B5BlockSize);
static_assert(kSlot554B5BoolCount * kSlot554B5BoolWritten +
                  kSlot554B5FloatCount * kSlot554B5FloatWritten +
                  kSlot554B5WrapperCount * kSlot554B5WrapperWritten == kSlot554B5Written);
static_assert(kSlot554B5BoolCount * kSlot554B5BoolHoles +
                  kSlot554B5FloatCount * kSlot554B5FloatHoles +
                  kSlot554B5WrapperCount * kSlot554B5WrapperHoles == kSlot554B5Holes);
static_assert(kSlot554B5Written + kSlot554B5Holes == kSlot554B5BlockSize);
static_assert(kSlot554B5StoreCount == kSlot554B5ObjectStoreCount + kSlot554B5StackStoreCount);

inline constexpr std::uint64_t kSlot554B5TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B5TableIndex = 554;
inline constexpr std::uint64_t kSlot554B5RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B5ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B5AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B5DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B5BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B5BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B5BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B5BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B5BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B5FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B5FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B5FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B5FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B5FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B5WrapperRva = 0x6c4c0;
inline constexpr std::size_t kSlot554B5WrapperSize = 0x190;
inline constexpr std::uint64_t kSlot554B5InnerRva = 0x6c650;
inline constexpr std::size_t kSlot554B5InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B5InnerVptr = 0x15961d8;      // table 0x15961c8 + 0x10
inline constexpr std::uint64_t kSlot554B5WrapperFinalVptr = 0x1596d10;  // table 0x1596d00 + 0x10
inline constexpr std::uint64_t kSlot554B5WrapperTagVptr = 0x1596ce0;
inline constexpr std::uint64_t kSlot554B5WrapperDtorRva = 0x54d60;
inline constexpr std::uint64_t kSlot554B5CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B5StackChkFailRva = 0xf8f3a0;

enum class Slot554B5Kind : std::uint8_t { bool_var, float_var, wrapper };

struct Slot554B5Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B5Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;  // 0 unless kind == wrapper
};

inline constexpr Slot554B5Spec kSlot554B5Specs[kSlot554B5ObjectCount] = {
    {"r_enableIBL", 11, 0x2eaf2a0, 0xc0, Slot554B5Kind::bool_var, 0x17c, 0x0, 0x0, 0},
    {"r_enableIBLScreenSpaceReflect", 29, 0x2eaf360, 0xc0, Slot554B5Kind::bool_var, 0x17d, 0x0,
     0x0, 0},
    {"r_enableIBLDebug", 16, 0x2eaf420, 0xc0, Slot554B5Kind::bool_var, 0x17e, 0x0, 0x0, 0},
    {"r_enableIBLStats", 16, 0x2eaf4e0, 0xc0, Slot554B5Kind::bool_var, 0x183, 0x0, 0x0, 0},
    {"PauseLighting", 13, 0x2eaf5a0, 0xc0, Slot554B5Kind::bool_var, 0x170, 0x0, 0x0, 0},
    {"DebugLighting", 13, 0x2eaf660, 0xc0, Slot554B5Kind::bool_var, 0x171, 0x0, 0x0, 0},
    {"r_useBakeOnlyInIBL", 18, 0x2eaf720, 0xc0, Slot554B5Kind::bool_var, 0x172, 0x0, 0x0, 0},
    {"showBakedLightingType", 21, 0x2eaf7e0, 0xd0, Slot554B5Kind::wrapper, 0x178, 0x0, 0x4,
     kSlot554B5WrapperRva},
    {"r_bakedAODistance", 17, 0x2eaf8b0, 0xd0, Slot554B5Kind::float_var, 0x16c, 0x3dcccccd,
     0x45800000, 0},
    {"r_force2SidedDepth", 18, 0x2eaf980, 0xc0, Slot554B5Kind::bool_var, 0x19a, 0x0, 0x0, 0},
    {"r_dynamicProbeAsync", 19, 0x2eafa40, 0xc0, Slot554B5Kind::bool_var, 0x166, 0x0, 0x0, 0},
    {"r_forceDynamicProbeDirty", 24, 0x2eafb00, 0xc0, Slot554B5Kind::bool_var, 0x167, 0x0, 0x0,
     0},
    {"r_displayDynamicProbeCount", 26, 0x2eafbc0, 0xc0, Slot554B5Kind::bool_var, 0x168, 0x0, 0x0,
     0},
    {"r_metalnessDebug", 16, 0x2eafc80, 0xd0, Slot554B5Kind::float_var, 0xa0, 0x0, 0x3f800000, 0},
    {"r_metalnessDebugBlend", 21, 0x2eafd50, 0xd0, Slot554B5Kind::float_var, 0xa4, 0x0,
     0x3f800000, 0},
    {"r_roughnessDebug", 16, 0x2eafe20, 0xd0, Slot554B5Kind::float_var, 0xa8, 0x0, 0x3f800000, 0},
    {"r_roughnessDebugBlend", 21, 0x2eafef0, 0xd0, Slot554B5Kind::float_var, 0xac, 0x0,
     0x3f800000, 0},
    {"r_albedoDebugR", 14, 0x2eaffc0, 0xd0, Slot554B5Kind::float_var, 0xb0, 0x0, 0x3f800000, 0},
    {"r_albedoDebugG", 14, 0x2eb0090, 0xd0, Slot554B5Kind::float_var, 0xb4, 0x0, 0x3f800000, 0},
    {"r_albedoDebugB", 14, 0x2eb0160, 0xd0, Slot554B5Kind::float_var, 0xb8, 0x0, 0x3f800000, 0},
    {"r_albedoDebugBlend", 18, 0x2eb0230, 0xd0, Slot554B5Kind::float_var, 0xbc, 0x0, 0x3f800000,
     0},
    {"r_diffuseScale", 14, 0x2eb0300, 0xd0, Slot554B5Kind::float_var, 0xc0, 0x0, 0x42c80000, 0},
    {"r_subsurfaceScale", 17, 0x2eb03d0, 0xd0, Slot554B5Kind::float_var, 0xc4, 0x0, 0x42c80000,
     0},
    {"r_specularScale", 15, 0x2eb04a0, 0xd0, Slot554B5Kind::float_var, 0xc8, 0x0, 0x42c80000, 0},
    {"r_IBLSpecularScale", 18, 0x2eb0570, 0xd0, Slot554B5Kind::float_var, 0xcc, 0x0, 0x42c80000,
     0},
    {"r_enableSkyOverride", 19, 0x2eb0640, 0xc0, Slot554B5Kind::bool_var, 0x120, 0x0, 0x0, 0},
    {"r_skyOverrideIntensity", 22, 0x2eb0700, 0xd0, Slot554B5Kind::float_var, 0x124, 0x0,
     0x447a0000, 0},
    {"r_skyIntensityScale", 19, 0x2eb07d0, 0xd0, Slot554B5Kind::float_var, 0x128, 0x0,
     0x447a0000, 0},
    {"r_skyDynamicParam", 17, 0x2eb08a0, 0xd0, Slot554B5Kind::float_var, 0x12c, 0x0, 0x3f800000,
     0},
    {"r_skyRotateY", 12, 0x2eb0970, 0xd0, Slot554B5Kind::float_var, 0x130, 0x0, 0x43b40000, 0},
    {"r_sunIntensity", 14, 0x2eb0a40, 0xd0, Slot554B5Kind::float_var, 0xe4, 0x0, 0x447a0000, 0},
    {"r_SunCascade0FadeScale", 22, 0x2eb0b10, 0xd0, Slot554B5Kind::float_var, 0xd4, 0x3f800000,
     0x42c80000, 0},
    {"r_SunCascade1FadeScale", 22, 0x2eb0be0, 0xd0, Slot554B5Kind::float_var, 0xd8, 0x3f800000,
     0x42c80000, 0},
    {"r_SunCascade2FadeScale", 22, 0x2eb0cb0, 0xd0, Slot554B5Kind::float_var, 0xdc, 0x3f800000,
     0x42c80000, 0},
    {"r_SunCascade3FadeScale", 22, 0x2eb0d80, 0xd0, Slot554B5Kind::float_var, 0xe0, 0x3f800000,
     0x42c80000, 0},
    {"r_enableCascadeShadows", 22, 0x2eb0e50, 0xc0, Slot554B5Kind::bool_var, 0xe9, 0x0, 0x0, 0},
};

struct alignas(8) Slot554Batch5Objects {
    std::array<std::uint8_t, kSlot554B5BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch5Objects) == kSlot554B5BlockSize);

struct Slot554Batch5Result;
using Slot554B5RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B5AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B5ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B5WrapperHook = bool (*)(void* context, ConCmdFunctor& object) noexcept;
using Slot554B5ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B5Hooks {
    Slot554B5RegisterHook register_var = nullptr;
    Slot554B5AtexitHook atexit = nullptr;
    Slot554B5ReleaseHook release_view = nullptr;
    Slot554B5WrapperHook wrapper_call = nullptr;       // 0x6c4c0
    Slot554B5ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B5Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch5Result {
    Slot554B5Stage blocked = Slot554B5Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch5_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch5_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B5Hooks default_slot554_batch5_hooks() noexcept;
[[nodiscard]] Slot554Batch5Result init_slot554_batch5(
    Slot554Batch5Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B5Hooks& hooks) noexcept;

}  // namespace sotc
