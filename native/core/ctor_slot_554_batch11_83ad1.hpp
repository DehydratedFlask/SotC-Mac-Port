// CPU-01-STARTUP-SLOT-554-BATCH-11: native model of the 36-object body
// [0x83ad1,0x86052) = 0x2581 B in slot 554. Objects 0x2eb9ee8..0x2ebbbc8 = 0x1ce0 B:
// six 0xc0 bools, twenty-six 0xd0 bounded floats, three 0xd0 bounded dwords and one wrapper.
// Slot 554 remains PARTIAL because batches 12 and 13 are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr 0x1594348,
//     functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its page byte.
//   * floats use 0xc300: final vptr 0x15943c0, arg-fn 0x1596878, class vtable 0x1594318,
//     atexit(0xad30); bounds are stored as full dwords at +0x6c/+0x74.
//   * dwords use 0xc580 with the page cell as value_ref: final vptr 0x1594c08, arg-fn
//     0x1596858, class vtable 0x15944a0, atexit(0xad70). This is the batch-7 c580 shape
//     without the object-hosted +0xd0 cell.
//   * one wrapper 0x6e4d0 / 0x6e660, 0x190 B outer + 0x160 B inner: inner vptr 0x1596600,
//     final 0x1597258, tag 0x1597238 (final = tag + 0x20), atexit(0x54fa0), page+0x3a8,
//     bounds 0..1.
//
// Byte accounting: bool 162/30, float 171/37, dword 171/37, wrapper 195/13;
// 6*162 + 26*171 + 3*171 + 195 = 6126 written and 1266 holes of 0x1ce0.
// Body measurement: 1670 instructions, 179 direct calls, 30 indirect calls (`call *0x18`),
// 508 stores (112 object + 396 stack/other), no branch. Store counting is strict:
// every mov-family memory write plus every `vmovups` write; object stores are writes through the
// block's object register. Under that rule batch 10 measures 497, not its landed 481 (16 stack
// writes short), so this unit records the measured counts instead of propagating the undercount. Wrapper transfers and view destroys are host hooks; no original RVA is executed.
//
// The inherited body end 0x86052 is measured (it holds batch 12's `leaq` of 0x2ebbbc8); the
// inherited last object start 0x2ebbaf8 now has its measured block end 0x2ebbbc8.
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

inline constexpr std::uint64_t kSlot554B11Rva = 0x83ad1;
inline constexpr std::size_t kSlot554B11BodySize = 0x2581;
inline constexpr std::uint64_t kSlot554B11ObjectBaseRva = 0x2eb9ee8;
inline constexpr std::uint64_t kSlot554B11ObjectEndRva = 0x2ebbbc8;
inline constexpr std::uint64_t kSlot554B11PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B11ObjectCount = 36;
inline constexpr std::size_t kSlot554B11BlockSize = 0x1ce0;
inline constexpr std::size_t kSlot554B11Written = 6126;
inline constexpr std::size_t kSlot554B11Holes = 1266;
inline constexpr std::size_t kSlot554B11InstructionCount = 1670;
inline constexpr std::size_t kSlot554B11DirectCallCount = 179;
inline constexpr std::size_t kSlot554B11IndirectCallCount = 30;
inline constexpr std::size_t kSlot554B11StoreCount = 508;
inline constexpr std::size_t kSlot554B11ObjectStoreCount = 112;
inline constexpr std::size_t kSlot554B11StackStoreCount = 396;
inline constexpr std::size_t kSlot554B11BoolCount = 6;
inline constexpr std::size_t kSlot554B11BoolWritten = 162;
inline constexpr std::size_t kSlot554B11BoolHoles = 30;
inline constexpr std::size_t kSlot554B11FloatCount = 26;
inline constexpr std::size_t kSlot554B11FloatWritten = 171;
inline constexpr std::size_t kSlot554B11FloatHoles = 37;
inline constexpr std::size_t kSlot554B11DwordCount = 3;
inline constexpr std::size_t kSlot554B11DwordWritten = 171;
inline constexpr std::size_t kSlot554B11DwordHoles = 37;
inline constexpr std::size_t kSlot554B11WrapperCount = 1;
inline constexpr std::size_t kSlot554B11WrapperWritten = 195;
inline constexpr std::size_t kSlot554B11WrapperHoles = 13;
static_assert(kSlot554B11BoolCount + kSlot554B11FloatCount + kSlot554B11DwordCount +
                  kSlot554B11WrapperCount == kSlot554B11ObjectCount);
static_assert(kSlot554B11BoolWritten + kSlot554B11BoolHoles == 0xc0);
static_assert(kSlot554B11FloatWritten + kSlot554B11FloatHoles == 0xd0);
static_assert(kSlot554B11DwordWritten + kSlot554B11DwordHoles == 0xd0);
static_assert(kSlot554B11WrapperWritten + kSlot554B11WrapperHoles == 0xd0);
static_assert(kSlot554B11BoolCount * 0xc0 +
                  (kSlot554B11ObjectCount - kSlot554B11BoolCount) * 0xd0 == kSlot554B11BlockSize);
static_assert(kSlot554B11BoolCount * kSlot554B11BoolWritten +
                  kSlot554B11FloatCount * kSlot554B11FloatWritten +
                  kSlot554B11DwordCount * kSlot554B11DwordWritten +
                  kSlot554B11WrapperCount * kSlot554B11WrapperWritten == kSlot554B11Written);
static_assert(kSlot554B11BoolCount * kSlot554B11BoolHoles +
                  kSlot554B11FloatCount * kSlot554B11FloatHoles +
                  kSlot554B11DwordCount * kSlot554B11DwordHoles +
                  kSlot554B11WrapperCount * kSlot554B11WrapperHoles == kSlot554B11Holes);
static_assert(kSlot554B11Written + kSlot554B11Holes == kSlot554B11BlockSize);
static_assert(kSlot554B11StoreCount == kSlot554B11ObjectStoreCount + kSlot554B11StackStoreCount);
static_assert(kSlot554B11ObjectEndRva - kSlot554B11ObjectBaseRva == kSlot554B11BlockSize);

inline constexpr std::uint64_t kSlot554B11TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B11TableIndex = 554;
inline constexpr std::uint64_t kSlot554B11RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B11ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B11AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B11DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B11BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B11BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B11BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B11BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B11BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B11FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B11FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B11FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B11FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B11FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B11DwordFinalVptr = 0x1594c08;
inline constexpr std::uint64_t kSlot554B11DwordArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B11DwordClassVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot554B11DwordCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot554B11DwordDtorRva = 0xad70;
inline constexpr std::size_t kSlot554B11WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B11InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B11WrapperRva = 0x6e4d0;
inline constexpr std::uint64_t kSlot554B11WrapperInnerRva = 0x6e660;
inline constexpr std::uint64_t kSlot554B11WrapperInnerVptr = 0x1596600;
inline constexpr std::uint64_t kSlot554B11WrapperFinalVptr = 0x1597258;
inline constexpr std::uint64_t kSlot554B11WrapperTagVptr = 0x1597238;
inline constexpr std::uint64_t kSlot554B11WrapperDtorRva = 0x54fa0;
inline constexpr std::uint64_t kSlot554B11CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B11StackChkFailRva = 0xf8f3a0;

enum class Slot554B11Kind : std::uint8_t { bool_var, float_var, dword_var, wrapper };

struct Slot554B11Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B11Kind kind;
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

inline constexpr Slot554B11Spec kSlot554B11Specs[kSlot554B11ObjectCount] = {
    {"r_ui_HdrConsistentUIWhitePoint", 30, 0x2eb9ee8, 0xc0, Slot554B11Kind::bool_var, 0x500, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_motionBlurEnable", 18, 0x2eb9fa8, 0xc0, Slot554B11Kind::bool_var, 0x390, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_motionBlurAmount", 18, 0x2eba068, 0xd0, Slot554B11Kind::float_var, 0x394, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_motionBlurExposureMin", 23, 0x2eba138, 0xd0, Slot554B11Kind::float_var, 0x398, 0x0, 0x3d888889, 0, 0, 0, 0, 0, 0xad30},
    {"r_motionBlurExposureMax", 23, 0x2eba208, 0xd0, Slot554B11Kind::float_var, 0x39c, 0x0, 0x3d888889, 0, 0, 0, 0, 0, 0xad30},
    {"r_motionBlurUnderwaterVignette", 30, 0x2eba2d8, 0xd0, Slot554B11Kind::float_var, 0x3a0, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_motionBlurUnderwaterVignetteRadius", 36, 0x2eba3a8, 0xd0, Slot554B11Kind::float_var, 0x3a4, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaMethod", 10, 0x2eba478, 0xd0, Slot554B11Kind::wrapper, 0x3a8, 0x0, 0x1, 0x6e4d0, 0x6e660, 0x1596600, 0x1597258, 0x1597238, 0x54fa0},
    {"r_aaUnclampedBlendFactor", 24, 0x2eba548, 0xd0, Slot554B11Kind::float_var, 0x3ac, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaEnableTemporalProjectMatrixJitter", 37, 0x2eba618, 0xc0, Slot554B11Kind::bool_var, 0x3b0, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_aaEnableTemporalDitherBuffer", 30, 0x2eba6d8, 0xc0, Slot554B11Kind::bool_var, 0x3b1, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"r_aaForceJitterIndex", 20, 0x2eba798, 0xd0, Slot554B11Kind::dword_var, 0x3b4, 0xffffffff, 0x64, 0, 0, 0, 0, 0, 0xad70},
    {"r_aaSharpenStrengthStatic", 25, 0x2eba868, 0xd0, Slot554B11Kind::float_var, 0x3b8, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaSharpenStrengthMotion", 25, 0x2eba938, 0xd0, Slot554B11Kind::float_var, 0x3bc, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaSharpenContrastSensitivity", 30, 0x2ebaa08, 0xd0, Slot554B11Kind::float_var, 0x3c0, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaSharpenMotionFactor", 23, 0x2ebaad8, 0xd0, Slot554B11Kind::float_var, 0x3c4, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaHistorySharpeningMotionMin", 30, 0x2ebaba8, 0xd0, Slot554B11Kind::float_var, 0x3c8, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaHistorySharpeningMotionMax", 30, 0x2ebac78, 0xd0, Slot554B11Kind::float_var, 0x3cc, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaHistorySharpeningStrength", 29, 0x2ebad48, 0xd0, Slot554B11Kind::float_var, 0x3d0, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaCurrFrameAmount", 19, 0x2ebae18, 0xd0, Slot554B11Kind::float_var, 0x3d4, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaCurrFrameMotionAmount", 25, 0x2ebaee8, 0xd0, Slot554B11Kind::float_var, 0x3d8, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaCurrFrameMotionFactor", 25, 0x2ebafb8, 0xd0, Slot554B11Kind::float_var, 0x3dc, 0x0, 0x3f800000, 0, 0, 0, 0, 0, 0xad30},
    {"r_aaDeflickerClampFalloff", 25, 0x2ebb088, 0xd0, Slot554B11Kind::float_var, 0x3e0, 0x0, 0x40000000, 0, 0, 0, 0, 0, 0xad30},
    {"sRender_AA_DeflickerLumaSensitivity", 35, 0x2ebb158, 0xd0, Slot554B11Kind::float_var, 0x3e4, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_enable", 17, 0x2ebb228, 0xc0, Slot554B11Kind::bool_var, 0x3e8, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"distortion_waveSpatialFrequency1", 32, 0x2ebb2e8, 0xd0, Slot554B11Kind::float_var, 0x3ec, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_waveTemporalFrequency1", 33, 0x2ebb3b8, 0xd0, Slot554B11Kind::float_var, 0x3f0, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_waveAmplitude1", 25, 0x2ebb488, 0xd0, Slot554B11Kind::float_var, 0x3f4, 0x0, 0x3c23d70a, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_waveSpatialFrequency2", 32, 0x2ebb558, 0xd0, Slot554B11Kind::float_var, 0x3f8, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_waveTemporalFrequency2", 33, 0x2ebb628, 0xd0, Slot554B11Kind::float_var, 0x3fc, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"distortion_waveAmplitude2", 25, 0x2ebb6f8, 0xd0, Slot554B11Kind::float_var, 0x400, 0x0, 0x3c23d70a, 0, 0, 0, 0, 0, 0xad30},
    {"filmGrain_enable", 16, 0x2ebb7c8, 0xc0, Slot554B11Kind::bool_var, 0x404, 0x0, 0x0, 0, 0, 0, 0, 0, 0xacf0},
    {"filmGrain_scale", 15, 0x2ebb888, 0xd0, Slot554B11Kind::float_var, 0x408, 0x0, 0x42c80000, 0, 0, 0, 0, 0, 0xad30},
    {"filmGrain_amount", 16, 0x2ebb958, 0xd0, Slot554B11Kind::float_var, 0x40c, 0x0, 0x41200000, 0, 0, 0, 0, 0, 0xad30},
    {"r_forceViewportHeightForPixelSizeCalc", 37, 0x2ebba28, 0xd0, Slot554B11Kind::dword_var, 0x42c, 0xffffffff, 0x870, 0, 0, 0, 0, 0, 0xad70},
    {"r_forceLod", 10, 0x2ebbaf8, 0xd0, Slot554B11Kind::dword_var, 0x430, 0xffffffff, 0xa, 0, 0, 0, 0, 0, 0xad70},
};

struct alignas(8) Slot554Batch11Objects {
    std::array<std::uint8_t, kSlot554B11BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch11Objects) == kSlot554B11BlockSize);

using Slot554B11RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B11AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                     ConCmdRegistry& registry) noexcept;
using Slot554B11ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B11WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                       ConCmdFunctor& object) noexcept;
using Slot554B11ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B11Hooks {
    Slot554B11RegisterHook register_var = nullptr;
    Slot554B11AtexitHook atexit = nullptr;
    Slot554B11ReleaseHook release_view = nullptr;
    Slot554B11WrapperHook wrapper_call = nullptr;
    Slot554B11ViewDestroyHook destroy_view = nullptr;
    void* context = nullptr;
};

enum class Slot554B11Stage : std::uint8_t { none, page, ctor, registration, view_release, atexit };

struct Slot554Batch11Result {
    Slot554B11Stage blocked = Slot554B11Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch11_object_written(std::uint32_t index,
                                                  std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch11_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B11Hooks default_slot554_batch11_hooks() noexcept;
[[nodiscard]] Slot554Batch11Result init_slot554_batch11(
    Slot554Batch11Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B11Hooks& hooks) noexcept;

}  // namespace sotc
