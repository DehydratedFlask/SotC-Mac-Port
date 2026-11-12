// CPU-01-STARTUP-SLOT-554-BATCH-03: native model of the 36-object body
// [0x718bb,0x739e8) = 0x1b30 B in slot 554. Objects 0x2eabb80..0x2ead6b0 = 0x1b30 B:
// 33 0xc0 bool variables, two 0xd0 bounded-float variables and one 0xd0 wrapper object.
// Slot 554 remains PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 33 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * particleTimeScale / particleWaterFogScale: 0xc300 bounded floats with arg-fn table
//     0x1596878, final vptr 0x15943c0 and atexit(0xad30); bounds 0..2.0f and 0..10.0f.
//   * showParticle_Stats: 0x6ba10 outer wrapper (0x18c, sha 869eb0b6…) calling the 0x6bba0
//     inner (0x160, sha 132494cf…); inner vptr 0x1596070, final vptr 0x1596b48 (table
//     0x1596b38 + 0x10), tag 0x1596b18, atexit(0x54ca0), page + 0x8c, bounds 0..3.
//
// Byte accounting: bool 162 written / 30 holes, float 171 / 37, wrapper 195 / 13; batch
// 5883 written / 1077 holes of 0x1b30. Body measurement: 1494 instructions, 179 direct calls,
// three indirect `call [reg+0x18]` functor destroys, 303 object stores + 169 stack stores and no
// conditional branch. Wrapper calls and view destroys are host hooks; no original RVA is executed.
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

inline constexpr std::uint64_t kSlot554B3Rva = 0x718bb;
inline constexpr std::size_t kSlot554B3BodySize = 0x1b30;
inline constexpr std::uint64_t kSlot554B3ObjectBaseRva = 0x2eabb80;
inline constexpr std::uint64_t kSlot554B3ObjectEndRva = 0x2ead6b0;
inline constexpr std::uint64_t kSlot554B3PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B3ObjectCount = 36;
inline constexpr std::size_t kSlot554B3BlockSize = 0x1b30;
inline constexpr std::size_t kSlot554B3Written = 5883;
inline constexpr std::size_t kSlot554B3Holes = 1077;
inline constexpr std::size_t kSlot554B3InstructionCount = 1494;
inline constexpr std::size_t kSlot554B3DirectCallCount = 179;
inline constexpr std::size_t kSlot554B3IndirectCallCount = 3;
inline constexpr std::size_t kSlot554B3StoreCount = 472;
inline constexpr std::size_t kSlot554B3ObjectStoreCount = 303;
inline constexpr std::size_t kSlot554B3StackStoreCount = 169;
inline constexpr std::size_t kSlot554B3BoolCount = 33;
inline constexpr std::size_t kSlot554B3BoolWritten = 162;
inline constexpr std::size_t kSlot554B3BoolHoles = 30;
inline constexpr std::size_t kSlot554B3FloatCount = 2;
inline constexpr std::size_t kSlot554B3FloatWritten = 171;
inline constexpr std::size_t kSlot554B3FloatHoles = 37;
inline constexpr std::size_t kSlot554B3WrapperCount = 1;
inline constexpr std::size_t kSlot554B3WrapperWritten = 195;
inline constexpr std::size_t kSlot554B3WrapperHoles = 13;
static_assert(kSlot554B3BoolCount + kSlot554B3FloatCount + kSlot554B3WrapperCount ==
              kSlot554B3ObjectCount);
static_assert(kSlot554B3BoolWritten + kSlot554B3BoolHoles == 0xc0);
static_assert(kSlot554B3FloatWritten + kSlot554B3FloatHoles == 0xd0);
static_assert(kSlot554B3WrapperWritten + kSlot554B3WrapperHoles == 0xd0);
static_assert(kSlot554B3BoolCount * 0xc0 + kSlot554B3FloatCount * 0xd0 +
                  kSlot554B3WrapperCount * 0xd0 == kSlot554B3BlockSize);
static_assert(kSlot554B3BoolCount * kSlot554B3BoolWritten +
                  kSlot554B3FloatCount * kSlot554B3FloatWritten +
                  kSlot554B3WrapperWritten == kSlot554B3Written);
static_assert(kSlot554B3BoolCount * kSlot554B3BoolHoles +
                  kSlot554B3FloatCount * kSlot554B3FloatHoles +
                  kSlot554B3WrapperHoles == kSlot554B3Holes);
static_assert(kSlot554B3Written + kSlot554B3Holes == kSlot554B3BlockSize);

inline constexpr std::uint64_t kSlot554B3TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B3TableIndex = 554;
inline constexpr std::uint64_t kSlot554B3RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B3ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B3AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B3DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B3BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B3BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B3BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B3BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B3BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B3FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B3FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B3FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B3FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B3WrapperRva = 0x6ba10;
inline constexpr std::size_t kSlot554B3WrapperSize = 0x18c;
inline constexpr std::uint64_t kSlot554B3InnerRva = 0x6bba0;
inline constexpr std::size_t kSlot554B3InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B3InnerVptr = 0x1596070;      // table 0x1596060 + 0x10
inline constexpr std::uint64_t kSlot554B3WrapperFinalVptr = 0x1596b48;  // table 0x1596b38 + 0x10
inline constexpr std::uint64_t kSlot554B3WrapperTagVptr = 0x1596b18;
inline constexpr std::uint64_t kSlot554B3WrapperDtorRva = 0x54ca0;
inline constexpr std::uint64_t kSlot554B3CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B3StackChkFailRva = 0xf8f3a0;

enum class Slot554B3Kind : std::uint8_t { bool_var, float_var, wrapper };

struct Slot554B3Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B3Kind kind;
    std::uint16_t page_cell_offset;
    std::uint32_t low;
    std::uint32_t high;
};

inline constexpr Slot554B3Spec kSlot554B3Specs[kSlot554B3ObjectCount] = {
    {"showWireframe", 13, 0x2eabb80, 0xc0, Slot554B3Kind::bool_var, 0xa8c, 0, 0},
    {"showParticle", 12, 0x2eabc40, 0xc0, Slot554B3Kind::bool_var, 0x54, 0, 0},
    {"showParticleModels", 18, 0x2eabd00, 0xc0, Slot554B3Kind::bool_var, 0x61, 0, 0},
    {"showParticleGroup_Normal", 24, 0x2eabdc0, 0xc0, Slot554B3Kind::bool_var, 0x55, 0, 0},
    {"showParticleGroup_UltraLowRes", 29, 0x2eabe80, 0xc0, Slot554B3Kind::bool_var, 0x56, 0, 0},
    {"showParticleGroup_BelowWater", 28, 0x2eabf40, 0xc0, Slot554B3Kind::bool_var, 0x57, 0, 0},
    {"showParticleGroup_Distortion", 28, 0x2eac000, 0xc0, Slot554B3Kind::bool_var, 0x5a, 0, 0},
    {"showParticleGroup_Fog", 21, 0x2eac0c0, 0xc0, Slot554B3Kind::bool_var, 0x5b, 0, 0},
    {"showParticleGroup_WaterDisplace", 31, 0x2eac180, 0xc0, Slot554B3Kind::bool_var, 0x5c, 0, 0},
    {"showParticleGroup_WaterDisplaceMicro", 36, 0x2eac240, 0xc0, Slot554B3Kind::bool_var, 0x5d, 0, 0},
    {"showParticleGroup_GroundDisplace", 32, 0x2eac300, 0xc0, Slot554B3Kind::bool_var, 0x5e, 0, 0},
    {"showParticleGroup_RainDisplace", 30, 0x2eac3c0, 0xc0, Slot554B3Kind::bool_var, 0x5f, 0, 0},
    {"showParticleGroup_ScreenOverlay", 31, 0x2eac480, 0xc0, Slot554B3Kind::bool_var, 0x60, 0, 0},
    {"showParticle_Wireframe", 22, 0x2eac540, 0xc0, Slot554B3Kind::bool_var, 0x62, 0, 0},
    {"showParticle_Errors", 19, 0x2eac600, 0xc0, Slot554B3Kind::bool_var, 0x66, 0, 0},
    {"showParticle_Name", 17, 0x2eac6c0, 0xc0, Slot554B3Kind::bool_var, 0x67, 0, 0},
    {"showParticle_InstanceIdx", 24, 0x2eac780, 0xc0, Slot554B3Kind::bool_var, 0x68, 0, 0},
    {"showParticle_Overdraw", 21, 0x2eac840, 0xc0, Slot554B3Kind::bool_var, 0x63, 0, 0},
    {"showParticle_Lighting", 21, 0x2eac900, 0xc0, Slot554B3Kind::bool_var, 0x64, 0, 0},
    {"showParticle_Displacement", 25, 0x2eac9c0, 0xc0, Slot554B3Kind::bool_var, 0x65, 0, 0},
    {"showParticle_Transform", 22, 0x2eaca80, 0xc0, Slot554B3Kind::bool_var, 0x69, 0, 0},
    {"showParticle_Count", 18, 0x2eacb40, 0xc0, Slot554B3Kind::bool_var, 0x6a, 0, 0},
    {"showParticle_Range", 18, 0x2eacc00, 0xc0, Slot554B3Kind::bool_var, 0x6b, 0, 0},
    {"showParticle_Age", 16, 0x2eaccc0, 0xc0, Slot554B3Kind::bool_var, 0x6c, 0, 0},
    {"showParticle_Lod", 16, 0x2eacd80, 0xc0, Slot554B3Kind::bool_var, 0x6d, 0, 0},
    {"showParticle_Loop", 17, 0x2eace40, 0xc0, Slot554B3Kind::bool_var, 0x6e, 0, 0},
    {"showParticle_ShadowBlend", 24, 0x2eacf00, 0xc0, Slot554B3Kind::bool_var, 0x6f, 0, 0},
    {"showParticle_Stats", 18, 0x2eacfc0, 0xd0, Slot554B3Kind::wrapper, 0x8c, 0, 3},
    {"showParticle_BasicStats", 23, 0x2ead090, 0xc0, Slot554B3Kind::bool_var, 0x90, 0, 0},
    {"filterParticle_ActiveLod", 24, 0x2ead150, 0xc0, Slot554B3Kind::bool_var, 0x70, 0, 0},
    {"filterParticle_Ancient", 22, 0x2ead210, 0xc0, Slot554B3Kind::bool_var, 0x71, 0, 0},
    {"filterParticle_AncientInactive", 30, 0x2ead2d0, 0xc0, Slot554B3Kind::bool_var, 0x72, 0, 0},
    {"filterParticle_EmitFromSkinnedMesh", 34, 0x2ead390, 0xc0, Slot554B3Kind::bool_var, 0x73, 0, 0},
    {"particleTimeScale", 17, 0x2ead450, 0xd0, Slot554B3Kind::float_var, 0x74, 0, 0x40000000},
    {"particleWaterFogScale", 21, 0x2ead520, 0xd0, Slot554B3Kind::float_var, 0x78, 0, 0x41200000},
    {"particleEnablePausing", 21, 0x2ead5f0, 0xc0, Slot554B3Kind::bool_var, 0x7c, 0, 0},
};

struct alignas(8) Slot554Batch3Objects {
    std::array<std::uint8_t, kSlot554B3BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch3Objects) == kSlot554B3BlockSize);

struct Slot554Batch3Result;
using Slot554B3RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B3AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B3ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B3WrapperHook = bool (*)(void* context, ConCmdFunctor& object) noexcept;
using Slot554B3ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B3Hooks {
    Slot554B3RegisterHook register_var = nullptr;
    Slot554B3AtexitHook atexit = nullptr;
    Slot554B3ReleaseHook release_view = nullptr;
    Slot554B3WrapperHook wrapper_call = nullptr;       // single 0x6ba10 call
    Slot554B3ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B3Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch3Result {
    Slot554B3Stage blocked = Slot554B3Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch3_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch3_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B3Hooks default_slot554_batch3_hooks() noexcept;
[[nodiscard]] Slot554Batch3Result init_slot554_batch3(
    Slot554Batch3Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B3Hooks& hooks) noexcept;

}  // namespace sotc
