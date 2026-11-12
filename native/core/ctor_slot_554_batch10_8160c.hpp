// CPU-01-STARTUP-SLOT-554-BATCH-10: native model of the 36-object body
// [0x8160c,0x83ad1) = 0x24c5 B in slot 554. Objects 0x2eb8218..0x2eb9ee8 = 0x1cd0 B:
// seven 0xc0 bools, twenty-two 0xd0 bounded floats, four 0xd0 unbounded floats, two 0xd0
// unbounded ints and one 0xd0 wrapper. Slot 554 remains PARTIAL because later batches are open.
//
// Object families (all pins are original identity tags, never executed):
//   * bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr 0x1594348,
//     functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its page byte.
//   * twenty-two bounded floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, class vtable
//     0x1594318, atexit(0xad30); bounds are stored as full dwords at +0x6c/+0x74.
//   * four unbounded floats use 0x6aa00 (final vptr 0x15943c0, class 0x1594318, atexit 0xad30);
//     two unbounded ints use 0x3a980 (final vptr 0x1594438, class 0x15944a0, atexit 0xad70).
//   * one wrapper 0x6e140 / 0x6e2d0, 0x190 B outer + 0x160 B inner: inner vptr 0x1596598,
//     final 0x15971d0, tag 0x15971a0, atexit(0x54f60), page+0x4d0, bounds 0..2.
//
// Byte accounting: bool 162/30, bounded float 171/37, unbounded float 165/43, int 165/43,
// wrapper 195/13; 7*162 + 22*171 + 4*165 + 2*165 + 195 = 6081 written and 1295 holes of 0x1cd0.
// Body measurement: 1641 instructions, 179 direct calls, 29 indirect calls (`call *0x18`), 497
// strict stores (119 object + 378 stack; mov-family + all `vmovups`), no branch. Wrapper transfers and view destroys are
// host hooks; no original RVA is executed.
//
// The inherited object-row end 0x2eb9e18 was the last object's start; the measured block end is
// 0x2eb9ee8, which is also the body's end (0x83ad1 is batch 11's first `leaq`, so there is no pad).
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

inline constexpr std::uint64_t kSlot554B10Rva = 0x8160c;
inline constexpr std::size_t kSlot554B10BodySize = 0x24c5;
inline constexpr std::uint64_t kSlot554B10ObjectBaseRva = 0x2eb8218;
inline constexpr std::uint64_t kSlot554B10ObjectEndRva = 0x2eb9ee8;
inline constexpr std::uint64_t kSlot554B10PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B10ObjectCount = 36;
inline constexpr std::size_t kSlot554B10BlockSize = 0x1cd0;
inline constexpr std::size_t kSlot554B10Written = 6081;
inline constexpr std::size_t kSlot554B10Holes = 1295;
inline constexpr std::size_t kSlot554B10InstructionCount = 1641;
inline constexpr std::size_t kSlot554B10DirectCallCount = 179;
inline constexpr std::size_t kSlot554B10IndirectCallCount = 29;
inline constexpr std::size_t kSlot554B10StoreCount = 497;
inline constexpr std::size_t kSlot554B10ObjectStoreCount = 119;
inline constexpr std::size_t kSlot554B10StackStoreCount = 378;
inline constexpr std::size_t kSlot554B10BoolCount = 7;
inline constexpr std::size_t kSlot554B10BoolWritten = 162;
inline constexpr std::size_t kSlot554B10BoolHoles = 30;
inline constexpr std::size_t kSlot554B10FloatCount = 22;
inline constexpr std::size_t kSlot554B10FloatWritten = 171;
inline constexpr std::size_t kSlot554B10FloatHoles = 37;
inline constexpr std::size_t kSlot554B10UnboundedFloatCount = 4;
inline constexpr std::size_t kSlot554B10UnboundedFloatWritten = 165;
inline constexpr std::size_t kSlot554B10UnboundedFloatHoles = 43;
inline constexpr std::size_t kSlot554B10IntCount = 2;
inline constexpr std::size_t kSlot554B10IntWritten = 165;
inline constexpr std::size_t kSlot554B10IntHoles = 43;
inline constexpr std::size_t kSlot554B10WrapperCount = 1;
inline constexpr std::size_t kSlot554B10WrapperWritten = 195;
inline constexpr std::size_t kSlot554B10WrapperHoles = 13;
static_assert(kSlot554B10BoolCount + kSlot554B10FloatCount + kSlot554B10UnboundedFloatCount +
                  kSlot554B10IntCount + kSlot554B10WrapperCount == kSlot554B10ObjectCount);
static_assert(kSlot554B10BoolWritten + kSlot554B10BoolHoles == 0xc0);
static_assert(kSlot554B10FloatWritten + kSlot554B10FloatHoles == 0xd0);
static_assert(kSlot554B10UnboundedFloatWritten + kSlot554B10UnboundedFloatHoles == 0xd0);
static_assert(kSlot554B10IntWritten + kSlot554B10IntHoles == 0xd0);
static_assert(kSlot554B10WrapperWritten + kSlot554B10WrapperHoles == 0xd0);
static_assert(kSlot554B10BoolCount * 0xc0 +
                  (kSlot554B10ObjectCount - kSlot554B10BoolCount) * 0xd0 == kSlot554B10BlockSize);
static_assert(kSlot554B10BoolCount * kSlot554B10BoolWritten +
                  kSlot554B10FloatCount * kSlot554B10FloatWritten +
                  kSlot554B10UnboundedFloatCount * kSlot554B10UnboundedFloatWritten +
                  kSlot554B10IntCount * kSlot554B10IntWritten +
                  kSlot554B10WrapperCount * kSlot554B10WrapperWritten == kSlot554B10Written);
static_assert(kSlot554B10BoolCount * kSlot554B10BoolHoles +
                  kSlot554B10FloatCount * kSlot554B10FloatHoles +
                  kSlot554B10UnboundedFloatCount * kSlot554B10UnboundedFloatHoles +
                  kSlot554B10IntCount * kSlot554B10IntHoles +
                  kSlot554B10WrapperCount * kSlot554B10WrapperHoles == kSlot554B10Holes);
static_assert(kSlot554B10Written + kSlot554B10Holes == kSlot554B10BlockSize);
static_assert(kSlot554B10StoreCount == kSlot554B10ObjectStoreCount + kSlot554B10StackStoreCount);
static_assert(kSlot554B10ObjectEndRva - kSlot554B10ObjectBaseRva == kSlot554B10BlockSize);

inline constexpr std::uint64_t kSlot554B10TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B10TableIndex = 554;
inline constexpr std::uint64_t kSlot554B10RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B10ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B10AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B10DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B10BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B10BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B10BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B10BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B10BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B10FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B10FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B10FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B10FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B10FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B10UnboundedFloatCtorRva = 0x6aa00;
inline constexpr std::uint64_t kSlot554B10IntFinalVptr = 0x1594438;
inline constexpr std::uint64_t kSlot554B10IntArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B10IntClassVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot554B10IntCtorRva = 0x3a980;
inline constexpr std::uint64_t kSlot554B10IntDtorRva = 0xad70;
inline constexpr std::size_t kSlot554B10WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B10InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B10WrapperRva = 0x6e140;
inline constexpr std::uint64_t kSlot554B10WrapperInnerRva = 0x6e2d0;
inline constexpr std::uint64_t kSlot554B10WrapperInnerVptr = 0x1596598;
inline constexpr std::uint64_t kSlot554B10WrapperFinalVptr = 0x15971d0;
inline constexpr std::uint64_t kSlot554B10WrapperTagVptr = 0x15971a0;
inline constexpr std::uint64_t kSlot554B10WrapperDtorRva = 0x54f60;
inline constexpr std::uint64_t kSlot554B10CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B10StackChkFailRva = 0xf8f3a0;

enum class Slot554B10Kind : std::uint8_t {
    bool_var, float_var, unbounded_float_var, int_var, wrapper
};

struct Slot554B10Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B10Kind kind;
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

inline constexpr Slot554B10Spec kSlot554B10Specs[kSlot554B10ObjectCount] = {
    {"r_bloomEnableFakeBloom", 22, 0x2eb8218, 0xc0, Slot554B10Kind::bool_var, 0x35a, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_bloomThreshold", 16, 0x2eb82d8, 0xd0, Slot554B10Kind::float_var, 0x35c, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomStrength", 15, 0x2eb83a8, 0xd0, Slot554B10Kind::float_var, 0x360, 0x0, 0x41a00000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomSaturation", 17, 0x2eb8478, 0xd0, Slot554B10Kind::float_var, 0x364, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomFalloff", 14, 0x2eb8548, 0xd0, Slot554B10Kind::float_var, 0x368, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomHDRThreshold", 19, 0x2eb8618, 0xd0, Slot554B10Kind::float_var, 0x36c, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomHDRStrength", 18, 0x2eb86e8, 0xd0, Slot554B10Kind::float_var, 0x370, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomHDRSaturation", 20, 0x2eb87b8, 0xd0, Slot554B10Kind::float_var, 0x374, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0xad30},
    {"r_bloomHDRFalloff", 17, 0x2eb8888, 0xd0, Slot554B10Kind::float_var, 0x378, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0xad30},
    {"r_vignetteEnable", 16, 0x2eb8958, 0xc0, Slot554B10Kind::bool_var, 0x37c, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_vignetteOverrideEnable", 24, 0x2eb8a18, 0xc0, Slot554B10Kind::bool_var, 0x37d, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_vignetteFadeStart", 19, 0x2eb8ad8, 0xd0, Slot554B10Kind::float_var, 0x380, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_vignetteFadeEnd", 17, 0x2eb8ba8, 0xd0, Slot554B10Kind::float_var, 0x384, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_vignetteScale", 15, 0x2eb8c78, 0xd0, Slot554B10Kind::float_var, 0x388, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_vignetteIntensity", 19, 0x2eb8d48, 0xd0, Slot554B10Kind::float_var, 0x38c, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofEnable", 11, 0x2eb8e18, 0xc0, Slot554B10Kind::bool_var, 0x410, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_dofOverrideEnable", 19, 0x2eb8ed8, 0xc0, Slot554B10Kind::bool_var, 0x411, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_dofShowValues", 15, 0x2eb8f98, 0xc0, Slot554B10Kind::bool_var, 0x412, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_dofNearAmount", 15, 0x2eb9058, 0xd0, Slot554B10Kind::float_var, 0x414, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofNearStart", 14, 0x2eb9128, 0xd0, Slot554B10Kind::unbounded_float_var, 0x418, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofNearRange", 14, 0x2eb91f8, 0xd0, Slot554B10Kind::unbounded_float_var, 0x41c, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofFarAmount", 14, 0x2eb92c8, 0xd0, Slot554B10Kind::float_var, 0x420, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofFarStart", 13, 0x2eb9398, 0xd0, Slot554B10Kind::unbounded_float_var, 0x424, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad30},
    {"r_dofFarRange", 13, 0x2eb9468, 0xd0, Slot554B10Kind::unbounded_float_var, 0x428, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad30},
    {"r_fullscreenBlurEnabled", 23, 0x2eb9538, 0xd0, Slot554B10Kind::wrapper, 0x4d0, 0x0, 0x2, 0x6e140, 0x6e2d0, 0x1596598, 0x15971d0, 0x15971a0, 0x54f60},
    {"r_fullscreenBlurEnableOptimizations", 35, 0x2eb9608, 0xc0, Slot554B10Kind::bool_var, 0x4d5, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_fullscreenBlurBlendWeight", 27, 0x2eb96c8, 0xd0, Slot554B10Kind::float_var, 0x4d8, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_fullscreenBlurNumPasses", 25, 0x2eb9798, 0xd0, Slot554B10Kind::int_var, 0x4dc, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad70},
    {"r_fullscreenBlurNumPasses4k", 27, 0x2eb9868, 0xd0, Slot554B10Kind::int_var, 0x4e0, 0x0, 0x0, 0, 0, 0, 0, 0, 0xad70},
    {"r_ui_HdrScale", 13, 0x2eb9938, 0xd0, Slot554B10Kind::float_var, 0x4e4, 0x0, 0x40400000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrWhitePointPos", 21, 0x2eb9a08, 0xd0, Slot554B10Kind::float_var, 0x4e8, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrWhitePointMin", 21, 0x2eb9ad8, 0xd0, Slot554B10Kind::float_var, 0x4ec, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrWhitePointMiddle", 24, 0x2eb9ba8, 0xd0, Slot554B10Kind::float_var, 0x4f0, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrWhitePointMax", 21, 0x2eb9c78, 0xd0, Slot554B10Kind::float_var, 0x4f4, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrPreviewSubtractNits", 27, 0x2eb9d48, 0xd0, Slot554B10Kind::float_var, 0x4f8, 0x0, 0x461c4000, 0, 0, 0, 0, 0, 0xad30},
    {"r_ui_HdrPreviewSubtractPercent", 30, 0x2eb9e18, 0xd0, Slot554B10Kind::float_var, 0x4fc, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
};

struct alignas(8) Slot554Batch10Objects {
    std::array<std::uint8_t, kSlot554B10BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch10Objects) == kSlot554B10BlockSize);

using Slot554B10RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B10AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                     ConCmdRegistry& registry) noexcept;
using Slot554B10ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B10WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                       ConCmdFunctor& object) noexcept;
using Slot554B10ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B10Hooks {
    Slot554B10RegisterHook register_var = nullptr;
    Slot554B10AtexitHook atexit = nullptr;
    Slot554B10ReleaseHook release_view = nullptr;
    Slot554B10WrapperHook wrapper_call = nullptr;
    Slot554B10ViewDestroyHook destroy_view = nullptr;
    void* context = nullptr;
};

enum class Slot554B10Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit
};

struct Slot554Batch10Result {
    Slot554B10Stage blocked = Slot554B10Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch10_object_written(std::uint32_t index,
                                                  std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch10_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B10Hooks default_slot554_batch10_hooks() noexcept;
[[nodiscard]] Slot554Batch10Result init_slot554_batch10(
    Slot554Batch10Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B10Hooks& hooks) noexcept;

}  // namespace sotc
