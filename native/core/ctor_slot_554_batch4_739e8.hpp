// CPU-01-STARTUP-SLOT-554-BATCH-04: native model of the 36-object body
// [0x739e8,0x75d22) = 0x233a B in slot 554. Objects 0x2ead6b0..0x2eaf2a0 = 0x1bf0 B:
// 21 0xc0 bool variables, twelve 0xd0 bounded floats, one 0xd0 bounded dword and two 0xd0 wrappers.
// Slot 554 remains PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 21 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * particleNumShadowCascades: 0xc580 bounded-dword constructor (0x160 B), mid vptr
//     0x15941a0, final vptr 0x1594438 (prologue 0x1594428 + 0x10), class tag 0x15944a0,
//     arg-fn 0x1596858, atexit(0xad70); bounds 0..4, page+0x80.
//   * Twelve 0xc300 bounded floats with arg-fn 0x1596878, final vptr 0x15943c0 and
//     atexit(0xad30); bounds are pinned per object below.
//   * particleResolution: 0x6bda0 outer wrapper (0x18c, sha 677c9581…) calling the 0x6bf30
//     inner (0x15f, sha 151a853c…); inner vptr 0x15960e8, final vptr 0x1596be0 (table
//     0x1596bd0 + 0x10), tag 0x1596bb0, atexit(0x54ce0), page+0x84, bounds 0..2.
//   * r_showLightProbes: 0x6c130 outer wrapper (0x18c, sha 9216b8be…) calling the 0x6c2c0
//     inner (0x15f, sha f7841dbd…); inner vptr 0x1596160, final vptr 0x1596c78 (table
//     0x1596c68 + 0x10), tag 0x1596c48, atexit(0x54d20), page+0x174, bounds 0..4.
//
// Byte accounting: bool 162 written / 30 holes, float 171 / 37, bounded dword 171 / 37,
// wrapper 195 / 13; batch 6015 written / 1137 holes of 0x1bf0. Body measurement: 1568
// instructions, 178 direct calls, fifteen indirect calls (thirteen functor destroys plus two
// wrapper view destroys), 215 object stores + 307 stack stores, no conditional branch. The
// wrapper calls and view destroys are host hooks; no original RVA is executed.
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

inline constexpr std::uint64_t kSlot554B4Rva = 0x739e8;
inline constexpr std::size_t kSlot554B4BodySize = 0x233a;
inline constexpr std::uint64_t kSlot554B4ObjectBaseRva = 0x2ead6b0;
inline constexpr std::uint64_t kSlot554B4ObjectEndRva = 0x2eaf2a0;
inline constexpr std::uint64_t kSlot554B4PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B4ObjectCount = 36;
inline constexpr std::size_t kSlot554B4BlockSize = 0x1bf0;
inline constexpr std::size_t kSlot554B4Written = 6015;
inline constexpr std::size_t kSlot554B4Holes = 1137;
inline constexpr std::size_t kSlot554B4InstructionCount = 1568;
inline constexpr std::size_t kSlot554B4DirectCallCount = 178;
inline constexpr std::size_t kSlot554B4IndirectCallCount = 15;
inline constexpr std::size_t kSlot554B4StoreCount = 522;
inline constexpr std::size_t kSlot554B4ObjectStoreCount = 215;
inline constexpr std::size_t kSlot554B4StackStoreCount = 307;
inline constexpr std::size_t kSlot554B4BoolCount = 21;
inline constexpr std::size_t kSlot554B4BoolWritten = 162;
inline constexpr std::size_t kSlot554B4BoolHoles = 30;
inline constexpr std::size_t kSlot554B4FloatCount = 12;
inline constexpr std::size_t kSlot554B4FloatWritten = 171;
inline constexpr std::size_t kSlot554B4FloatHoles = 37;
inline constexpr std::size_t kSlot554B4IntCount = 1;
inline constexpr std::size_t kSlot554B4IntWritten = 171;
inline constexpr std::size_t kSlot554B4IntHoles = 37;
inline constexpr std::size_t kSlot554B4WrapperCount = 2;
inline constexpr std::size_t kSlot554B4WrapperWritten = 195;
inline constexpr std::size_t kSlot554B4WrapperHoles = 13;
static_assert(kSlot554B4BoolCount + kSlot554B4FloatCount + kSlot554B4IntCount +
                  kSlot554B4WrapperCount == kSlot554B4ObjectCount);
static_assert(kSlot554B4BoolWritten + kSlot554B4BoolHoles == 0xc0);
static_assert(kSlot554B4FloatWritten + kSlot554B4FloatHoles == 0xd0);
static_assert(kSlot554B4IntWritten + kSlot554B4IntHoles == 0xd0);
static_assert(kSlot554B4WrapperWritten + kSlot554B4WrapperHoles == 0xd0);
static_assert(kSlot554B4BoolCount * 0xc0 + (kSlot554B4FloatCount + kSlot554B4IntCount +
                                             kSlot554B4WrapperCount) * 0xd0 ==
              kSlot554B4BlockSize);
static_assert(kSlot554B4BoolCount * kSlot554B4BoolWritten +
                  kSlot554B4FloatCount * kSlot554B4FloatWritten +
                  kSlot554B4IntWritten + kSlot554B4WrapperCount * kSlot554B4WrapperWritten ==
              kSlot554B4Written);
static_assert(kSlot554B4BoolCount * kSlot554B4BoolHoles +
                  kSlot554B4FloatCount * kSlot554B4FloatHoles +
                  kSlot554B4IntHoles + kSlot554B4WrapperCount * kSlot554B4WrapperHoles ==
              kSlot554B4Holes);
static_assert(kSlot554B4Written + kSlot554B4Holes == kSlot554B4BlockSize);

inline constexpr std::uint64_t kSlot554B4TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B4TableIndex = 554;
inline constexpr std::uint64_t kSlot554B4RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B4ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B4AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B4DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B4BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B4BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B4BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B4BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B4BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B4FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B4FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B4FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B4FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B4IntFinalVptr = 0x1594438;   // 0x1594428 + 0x10
inline constexpr std::uint64_t kSlot554B4IntMidVptr = 0x15941a0;     // 0x1594190 + 0x10
inline constexpr std::uint64_t kSlot554B4IntClassVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot554B4IntArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B4IntCtorRva = 0xc580;
inline constexpr std::size_t kSlot554B4IntCtorSize = 0x160;
inline constexpr std::uint64_t kSlot554B4IntDtorRva = 0xad70;
inline constexpr std::uint64_t kSlot554B4WrapperARva = 0x6bda0;
inline constexpr std::size_t kSlot554B4WrapperASize = 0x18c;
inline constexpr std::uint64_t kSlot554B4InnerARva = 0x6bf30;
inline constexpr std::size_t kSlot554B4InnerASize = 0x15f;
inline constexpr std::uint64_t kSlot554B4InnerAVptr = 0x15960e8;      // table 0x15960d8 + 0x10
inline constexpr std::uint64_t kSlot554B4WrapperAFinalVptr = 0x1596be0;  // table 0x1596bd0 + 0x10
inline constexpr std::uint64_t kSlot554B4WrapperATagVptr = 0x1596bb0;
inline constexpr std::uint64_t kSlot554B4WrapperADtorRva = 0x54ce0;
inline constexpr std::uint64_t kSlot554B4WrapperBRva = 0x6c130;
inline constexpr std::size_t kSlot554B4WrapperBSize = 0x18c;
inline constexpr std::uint64_t kSlot554B4InnerBRva = 0x6c2c0;
inline constexpr std::size_t kSlot554B4InnerBSize = 0x15f;
inline constexpr std::uint64_t kSlot554B4InnerBVptr = 0x1596160;      // table 0x1596150 + 0x10
inline constexpr std::uint64_t kSlot554B4WrapperBFinalVptr = 0x1596c78;  // table 0x1596c68 + 0x10
inline constexpr std::uint64_t kSlot554B4WrapperBTagVptr = 0x1596c48;
inline constexpr std::uint64_t kSlot554B4WrapperBDtorRva = 0x54d20;
inline constexpr std::uint64_t kSlot554B4CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B4StackChkFailRva = 0xf8f3a0;

enum class Slot554B4Kind : std::uint8_t { bool_var, float_var, int_var, wrapper };

struct Slot554B4Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B4Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;  // 0 unless kind == wrapper
};

inline constexpr Slot554B4Spec kSlot554B4Specs[kSlot554B4ObjectCount] = {
    {"particleEnableLOD", 17, 0x2ead6b0, 0xc0, Slot554B4Kind::bool_var, 0x7e, 0, 0, 0},
    {"particleEnableAsyncCompute", 26, 0x2ead770, 0xc0, Slot554B4Kind::bool_var, 0x7d, 0, 0, 0},
    {"particleNumShadowCascades", 25, 0x2ead830, 0xd0, Slot554B4Kind::int_var, 0x80, 0, 4, 0},
    {"particleResolution", 18, 0x2ead900, 0xd0, Slot554B4Kind::wrapper, 0x84, 0, 2,
     kSlot554B4WrapperARva},
    {"particleLowResBudgetFactor", 26, 0x2ead9d0, 0xd0, Slot554B4Kind::float_var, 0x88, 0,
     0x3f800000, 0},
    {"r_enableLights", 14, 0x2eadaa0, 0xc0, Slot554B4Kind::bool_var, 0x98, 0, 0, 0},
    {"r_enableGobos", 13, 0x2eadb60, 0xc0, Slot554B4Kind::bool_var, 0x99, 0, 0, 0},
    {"r_enableCustomLightShapes", 25, 0x2eadc20, 0xc0, Slot554B4Kind::bool_var, 0x9a, 0, 0, 0},
    {"r_DirectLightingAOScale", 23, 0x2eadce0, 0xd0, Slot554B4Kind::float_var, 0x9c, 0,
     0x3f800000, 0},
    {"r_enableSpotShadows", 19, 0x2eaddb0, 0xc0, Slot554B4Kind::bool_var, 0x118, 0, 0, 0},
    {"r_enableHiResSpotShadows", 24, 0x2eade70, 0xc0, Slot554B4Kind::bool_var, 0x119, 0, 0, 0},
    {"r_ErrorIfCanNotDoHiResSpotShadows", 33, 0x2eadf30, 0xc0, Slot554B4Kind::bool_var, 0x11a, 0,
     0, 0},
    {"r_ErrorIfMoreThen4ShadowCastingSpotLights", 41, 0x2eadff0, 0xc0,
     Slot554B4Kind::bool_var, 0x11b, 0, 0, 0},
    {"r_SpotShadowDepthScaleJitter", 28, 0x2eae0b0, 0xd0, Slot554B4Kind::float_var, 0x11c,
     0xbf800000, 0x3f800000, 0},
    {"r_enableAmbientPass", 19, 0x2eae180, 0xc0, Slot554B4Kind::bool_var, 0x134, 0, 0, 0},
    {"r_enableAmbientShadowing", 24, 0x2eae240, 0xc0, Slot554B4Kind::bool_var, 0x135, 0, 0, 0},
    {"r_showAmbientOccluders", 22, 0x2eae300, 0xc0, Slot554B4Kind::bool_var, 0x136, 0, 0, 0},
    {"r_ambientShadowConeAngle", 24, 0x2eae3c0, 0xd0, Slot554B4Kind::float_var, 0x138,
     0x3f800000, 0x42b20000, 0},
    {"r_ambientShadowRadiusToFadeRange", 32, 0x2eae490, 0xd0, Slot554B4Kind::float_var, 0x140,
     0x40a00000, 0x42480000, 0},
    {"r_ambientShadowRadiusToFadeRangePow", 35, 0x2eae560, 0xd0, Slot554B4Kind::float_var, 0x13c,
     0x3dcccccd, 0x40800000, 0},
    {"r_ambientShadowMinAmbient", 25, 0x2eae630, 0xd0, Slot554B4Kind::float_var, 0x144, 0,
     0x3f800000, 0},
    {"r_ambientShadowMinDirectional", 29, 0x2eae700, 0xd0, Slot554B4Kind::float_var, 0x148, 0,
     0x3f800000, 0},
    {"r_ambientTintEnable", 19, 0x2eae7d0, 0xc0, Slot554B4Kind::bool_var, 0x14c, 0, 0, 0},
    {"r_ambientTintR", 14, 0x2eae890, 0xd0, Slot554B4Kind::float_var, 0x150, 0, 0x41200000, 0},
    {"r_ambientTintG", 14, 0x2eae960, 0xd0, Slot554B4Kind::float_var, 0x154, 0, 0x41200000, 0},
    {"r_ambientTintB", 14, 0x2eaea30, 0xd0, Slot554B4Kind::float_var, 0x158, 0, 0x41200000, 0},
    {"r_ambientTintBlend", 18, 0x2eaeb00, 0xd0, Slot554B4Kind::float_var, 0x15c, 0, 0x3f800000, 0},
    {"r_showLightProbes", 17, 0x2eaebd0, 0xd0, Slot554B4Kind::wrapper, 0x174, 0, 4,
     kSlot554B4WrapperBRva},
    {"r_showSunOnLightProbes", 22, 0x2eaeca0, 0xc0, Slot554B4Kind::bool_var, 0x162, 0, 0, 0},
    {"r_showLightProbeVolumes", 23, 0x2eaed60, 0xc0, Slot554B4Kind::bool_var, 0x163, 0, 0, 0},
    {"r_showLightProbeAO", 18, 0x2eaee20, 0xc0, Slot554B4Kind::bool_var, 0x161, 0, 0, 0},
    {"r_debugWanderProbe", 18, 0x2eaeee0, 0xc0, Slot554B4Kind::bool_var, 0x165, 0, 0, 0},
    {"r_debugDrawIBLLocal", 19, 0x2eaefa0, 0xc0, Slot554B4Kind::bool_var, 0x17f, 0, 0, 0},
    {"r_debugDrawIBLDistant", 21, 0x2eaf060, 0xc0, Slot554B4Kind::bool_var, 0x180, 0, 0, 0},
    {"r_debugDrawIBLProxy", 19, 0x2eaf120, 0xc0, Slot554B4Kind::bool_var, 0x181, 0, 0, 0},
    {"r_debugDrawSuperShinyMode", 25, 0x2eaf1e0, 0xc0, Slot554B4Kind::bool_var, 0x182, 0, 0, 0},
};

struct alignas(8) Slot554Batch4Objects {
    std::array<std::uint8_t, kSlot554B4BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch4Objects) == kSlot554B4BlockSize);

struct Slot554Batch4Result;
using Slot554B4RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B4AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B4ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B4WrapperHook = bool (*)(void* context, ConCmdFunctor& object) noexcept;
using Slot554B4ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B4Hooks {
    Slot554B4RegisterHook register_var = nullptr;
    Slot554B4AtexitHook atexit = nullptr;
    Slot554B4ReleaseHook release_view = nullptr;
    Slot554B4WrapperHook wrapper_call = nullptr;       // 0x6bda0 / 0x6c130
    Slot554B4ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B4Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch4Result {
    Slot554B4Stage blocked = Slot554B4Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch4_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch4_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B4Hooks default_slot554_batch4_hooks() noexcept;
[[nodiscard]] Slot554Batch4Result init_slot554_batch4(
    Slot554Batch4Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B4Hooks& hooks) noexcept;

}  // namespace sotc
