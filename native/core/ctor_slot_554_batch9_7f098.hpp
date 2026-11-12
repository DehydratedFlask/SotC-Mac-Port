// CPU-01-STARTUP-SLOT-554-BATCH-09: native model of the 36-object body
// [0x7f098,0x8160c) = 0x2574 B in slot 554. Objects 0x2eb6538..0x2eb8218 = 0x1ce0 B:
// six 0xc0 bools, twenty-eight 0xd0 bounded floats and two 0xd0 wrappers. Slot 554 remains
// PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 6 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * The 28 floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, class vtable 0x1594318,
//     atexit(0xad30), and write 0x1594318 at +0xc0; bounds are pinned per object.
//   * Two wrappers, each 0x190 B outer + 0x160 B inner:
//       - r_showRenderSettings 0x6da20 / 0x6dbb0, inner vptr 0x15964a8, final 0x15970a0,
//         tag 0x1597070, atexit(0x54ee0), page+0x2ec, bounds 0..2.
//       - r_showRenderSettingsValues 0x6ddb0 / 0x6df40, inner vptr 0x1596520, final 0x1597138,
//         tag 0x1597108, atexit(0x54f20), page+0x2f0, bounds 0..1.
//
// Byte accounting: bool 162/30, float 171/37, wrapper 195/13;
// 6*162 + 28*171 + 2*195 = 6150 written and 1242 holes of 0x1ce0.
// Body measurement: 1660 instructions, 178 direct calls, 30 indirect calls (28 float functor
// destroys + 2 wrapper view destroys), 506 mov-family stores (110 object + 396 stack), no
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

inline constexpr std::uint64_t kSlot554B9Rva = 0x7f098;
inline constexpr std::size_t kSlot554B9BodySize = 0x2574;
inline constexpr std::uint64_t kSlot554B9ObjectBaseRva = 0x2eb6538;
inline constexpr std::uint64_t kSlot554B9ObjectEndRva = 0x2eb8218;
inline constexpr std::uint64_t kSlot554B9PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B9ObjectCount = 36;
inline constexpr std::size_t kSlot554B9BlockSize = 0x1ce0;
inline constexpr std::size_t kSlot554B9Written = 6150;
inline constexpr std::size_t kSlot554B9Holes = 1242;
inline constexpr std::size_t kSlot554B9InstructionCount = 1660;
inline constexpr std::size_t kSlot554B9DirectCallCount = 178;
inline constexpr std::size_t kSlot554B9IndirectCallCount = 30;
inline constexpr std::size_t kSlot554B9StoreCount = 506;
inline constexpr std::size_t kSlot554B9ObjectStoreCount = 110;
inline constexpr std::size_t kSlot554B9StackStoreCount = 396;
inline constexpr std::size_t kSlot554B9BoolCount = 6;
inline constexpr std::size_t kSlot554B9BoolWritten = 162;
inline constexpr std::size_t kSlot554B9BoolHoles = 30;
inline constexpr std::size_t kSlot554B9FloatCount = 28;
inline constexpr std::size_t kSlot554B9FloatWritten = 171;
inline constexpr std::size_t kSlot554B9FloatHoles = 37;
inline constexpr std::size_t kSlot554B9WrapperCount = 2;
inline constexpr std::size_t kSlot554B9WrapperWritten = 195;
inline constexpr std::size_t kSlot554B9WrapperHoles = 13;
static_assert(kSlot554B9BoolCount + kSlot554B9FloatCount + kSlot554B9WrapperCount ==
              kSlot554B9ObjectCount);
static_assert(kSlot554B9BoolWritten + kSlot554B9BoolHoles == 0xc0);
static_assert(kSlot554B9FloatWritten + kSlot554B9FloatHoles == 0xd0);
static_assert(kSlot554B9WrapperWritten + kSlot554B9WrapperHoles == 0xd0);
static_assert(kSlot554B9BoolCount * 0xc0 +
                  (kSlot554B9FloatCount + kSlot554B9WrapperCount) * 0xd0 == kSlot554B9BlockSize);
static_assert(kSlot554B9BoolCount * kSlot554B9BoolWritten +
                  kSlot554B9FloatCount * kSlot554B9FloatWritten +
                  kSlot554B9WrapperCount * kSlot554B9WrapperWritten == kSlot554B9Written);
static_assert(kSlot554B9BoolCount * kSlot554B9BoolHoles +
                  kSlot554B9FloatCount * kSlot554B9FloatHoles +
                  kSlot554B9WrapperCount * kSlot554B9WrapperHoles == kSlot554B9Holes);
static_assert(kSlot554B9Written + kSlot554B9Holes == kSlot554B9BlockSize);
static_assert(kSlot554B9StoreCount == kSlot554B9ObjectStoreCount + kSlot554B9StackStoreCount);
static_assert(kSlot554B9BodySize == 0x2574);

inline constexpr std::uint64_t kSlot554B9TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B9TableIndex = 554;
inline constexpr std::uint64_t kSlot554B9RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B9ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B9AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B9DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B9BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B9BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B9BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B9BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B9BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B9FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B9FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B9FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B9FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B9FloatDtorRva = 0xad30;
inline constexpr std::size_t kSlot554B9WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B9InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperRva = 0x6da20;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperInnerRva = 0x6dbb0;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperInnerVptr = 0x15964a8;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperFinalVptr = 0x15970a0;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperTagVptr = 0x1597070;
inline constexpr std::uint64_t kSlot554B9ShowSettingsWrapperDtorRva = 0x54ee0;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperRva = 0x6ddb0;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperInnerRva = 0x6df40;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperInnerVptr = 0x1596520;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperFinalVptr = 0x1597138;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperTagVptr = 0x1597108;
inline constexpr std::uint64_t kSlot554B9ShowSettingsValuesWrapperDtorRva = 0x54f20;
inline constexpr std::uint64_t kSlot554B9CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B9StackChkFailRva = 0xf8f3a0;

enum class Slot554B9Kind : std::uint8_t { bool_var, float_var, wrapper };

struct Slot554B9Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B9Kind kind;
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

inline constexpr Slot554B9Spec kSlot554B9Specs[kSlot554B9ObjectCount] = {
    {"r_acesHdrToeDenominator", 23, 0x2eb6538, 0xd0, Slot554B9Kind::float_var, 0x2e0, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesHdrMaximum", 16, 0x2eb6608, 0xd0, Slot554B9Kind::float_var, 0x2e4, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrOverallStrength", 24, 0x2eb66d8, 0xd0, Slot554B9Kind::float_var, 0x2b8, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrShoulderStrength", 25, 0x2eb67a8, 0xd0, Slot554B9Kind::float_var, 0x2bc, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrShoulderBroadAngle", 27, 0x2eb6878, 0xd0, Slot554B9Kind::float_var, 0x2c0, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrToeToSholderAngle", 26, 0x2eb6948, 0xd0, Slot554B9Kind::float_var, 0x2c4, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrToeDenominator", 23, 0x2eb6a18, 0xd0, Slot554B9Kind::float_var, 0x2c8, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0},
    {"r_acesSdrMaximum", 16, 0x2eb6ae8, 0xd0, Slot554B9Kind::float_var, 0x2cc, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0},
    {"r_isArtDirectionMode", 20, 0x2eb6bb8, 0xc0, Slot554B9Kind::bool_var, 0x2e8, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_ignoreOriginalRenderSettings", 30, 0x2eb6c78, 0xc0, Slot554B9Kind::bool_var, 0x2e9, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_ignoreDisplayMode", 19, 0x2eb6d38, 0xc0, Slot554B9Kind::bool_var, 0x2ea, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_showRenderSettings", 20, 0x2eb6df8, 0xd0, Slot554B9Kind::wrapper, 0x2ec, 0, 2, 0x6da20, 0x6dbb0, 0x15964a8, 0x15970a0, 0x1597070, 0x54ee0},
    {"r_showRenderSettingsValues", 26, 0x2eb6ec8, 0xd0, Slot554B9Kind::wrapper, 0x2f0, 0, 1, 0x6ddb0, 0x6df40, 0x1596520, 0x1597138, 0x1597108, 0x54f20},
    {"r_colorGradeOverrideEnable", 26, 0x2eb6f98, 0xc0, Slot554B9Kind::bool_var, 0x304, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_highlightFactorHDR", 20, 0x2eb7058, 0xd0, Slot554B9Kind::float_var, 0x308, 0x0, 0x40a00000, 0, 0, 0, 0, 0, 0},
    {"r_colorFilterR", 14, 0x2eb7128, 0xd0, Slot554B9Kind::float_var, 0x30c, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_colorFilterG", 14, 0x2eb71f8, 0xd0, Slot554B9Kind::float_var, 0x310, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_colorFilterB", 14, 0x2eb72c8, 0xd0, Slot554B9Kind::float_var, 0x314, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_saturation", 12, 0x2eb7398, 0xd0, Slot554B9Kind::float_var, 0x318, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_exposureBias", 14, 0x2eb7468, 0xd0, Slot554B9Kind::float_var, 0x31c, 0xc2000000, 0x42000000, 0, 0, 0, 0, 0, 0},
    {"r_contrast", 10, 0x2eb7538, 0xd0, Slot554B9Kind::float_var, 0x320, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_gamma", 7, 0x2eb7608, 0xd0, Slot554B9Kind::float_var, 0x324, 0x0, 0x40800000, 0, 0, 0, 0, 0, 0},
    {"r_shadowColorR", 14, 0x2eb76d8, 0xd0, Slot554B9Kind::float_var, 0x328, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_shadowColorG", 14, 0x2eb77a8, 0xd0, Slot554B9Kind::float_var, 0x32c, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_shadowColorB", 14, 0x2eb7878, 0xd0, Slot554B9Kind::float_var, 0x330, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_shadowOffset", 14, 0x2eb7948, 0xd0, Slot554B9Kind::float_var, 0x334, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_midtoneColorR", 15, 0x2eb7a18, 0xd0, Slot554B9Kind::float_var, 0x338, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_midtoneColorG", 15, 0x2eb7ae8, 0xd0, Slot554B9Kind::float_var, 0x33c, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_midtoneColorB", 15, 0x2eb7bb8, 0xd0, Slot554B9Kind::float_var, 0x340, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_midtoneOffset", 15, 0x2eb7c88, 0xd0, Slot554B9Kind::float_var, 0x344, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_highlightColorR", 17, 0x2eb7d58, 0xd0, Slot554B9Kind::float_var, 0x348, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_highlightColorG", 17, 0x2eb7e28, 0xd0, Slot554B9Kind::float_var, 0x34c, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_highlightColorB", 17, 0x2eb7ef8, 0xd0, Slot554B9Kind::float_var, 0x350, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0},
    {"r_highlightOffset", 17, 0x2eb7fc8, 0xd0, Slot554B9Kind::float_var, 0x354, 0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"r_bloomEnable", 13, 0x2eb8098, 0xc0, Slot554B9Kind::bool_var, 0x358, 0, 0, 0, 0, 0, 0, 0, 0},
    {"r_bloomFireflyFilter", 20, 0x2eb8158, 0xc0, Slot554B9Kind::bool_var, 0x359, 0, 0, 0, 0, 0, 0, 0, 0},
};

struct alignas(8) Slot554Batch9Objects {
    std::array<std::uint8_t, kSlot554B9BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch9Objects) == kSlot554B9BlockSize);

struct Slot554Batch9Result;
using Slot554B9RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B9AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B9ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B9WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                      ConCmdFunctor& object) noexcept;
using Slot554B9ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B9Hooks {
    Slot554B9RegisterHook register_var = nullptr;
    Slot554B9AtexitHook atexit = nullptr;
    Slot554B9ReleaseHook release_view = nullptr;
    Slot554B9WrapperHook wrapper_call = nullptr;       // per-wrapper outer RVA
    Slot554B9ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B9Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch9Result {
    Slot554B9Stage blocked = Slot554B9Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch9_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch9_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B9Hooks default_slot554_batch9_hooks() noexcept;
[[nodiscard]] Slot554Batch9Result init_slot554_batch9(
    Slot554Batch9Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B9Hooks& hooks) noexcept;

}  // namespace sotc
