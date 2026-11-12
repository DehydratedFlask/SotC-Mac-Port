// CPU-01-STARTUP-SLOT-554-BATCH-07: native model of the 36-object body
// [0x7a68c,0x7cb5b) = 0x24cf B in slot 554. Objects 0x2eb2ba0..0x2eb4868 = 0x1cc8 B:
// nine 0xc0 bool variables, nineteen 0xd0 bounded floats, four 0xd0 unbounded ints, three
// 0xd8 object-hosted bounded dwords and one 0xd0 wrapper. Slot 554 remains PARTIAL because
// later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * The 9 bools use the batch-2a shape: base 0x20af0, mid vptr 0x15940b0, final vptr
//     0x1594348, functor tag 0x1596898, register 0x20d60, atexit(0xacf0); each reads its own
//     page byte as the default.
//   * The 19 floats use 0xc300: final vptr 0x15943c0 (the hoisted 0x10(%rsp) slot), arg-fn
//     0x1596878, atexit(0xad30), and write 0x1594318 at +0xc0; bounds are pinned per object.
//   * The 4 ints use 0x3a980: final vptr 0x1594438, arg-fn 0x1596858, atexit(0xad70); each
//     reads its page dword as the default and has no bounds.
//   * The 3 dwords use 0xc580 with an **object-hosted** value cell: the constructor receives
//     `object + 0xd0`, not a page cell, so these three have no page byte to read and are 0xd8
//     (0xd0 + the 8-byte tail). Final vptr 0x1594c08 (table 0x1594bf8 + 0x10), arg-fn
//     0x1596858, atexit(0x18a60), bounds 0..0x10.
//   * One wrapper, 0x190 B outer + 0x160 B inner: fog_superSampleMode
//     0x6d300 / 0x6d490, inner vptr 0x15963a8, final 0x1596f70 (table 0x1596f60 + 0x10),
//     view/tag vtable 0x1596f40, atexit(0x54e60), page+0x1f8, bounds 0..2. The outer body has
//     no direct 0x20d60 call: it registers inside its own body, which is the wrapper_call hook.
//
// Byte accounting: bool 162/30, float 171/37, int 165/43, dword 175/41 (the 0xd0 dword mask
// plus the body's explicit 4-byte store at +0xd0), wrapper 195/13;
// 9*162 + 19*171 + 4*165 + 3*175 + 195 = 6087 written and 1281 holes of 0x1cc8.
// Body measurement: 1631 instructions, 179 direct calls, 27 indirect calls (19 float functor
// destroys + 4 int destroys + 3 dword destroys + one wrapper view destroy), 508 mov-family
// stores (137 object + 371 stack), no branch of any kind. Wrapper transfers and view destroys
// are host hooks; no original RVA is executed.
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
#include "helper_3a980_unbounded_int.hpp"
#include "helper_c580_ranged_int.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554B7Rva = 0x7a68c;
inline constexpr std::size_t kSlot554B7BodySize = 0x24cf;
inline constexpr std::uint64_t kSlot554B7ObjectBaseRva = 0x2eb2ba0;
inline constexpr std::uint64_t kSlot554B7ObjectEndRva = 0x2eb4868;
inline constexpr std::uint64_t kSlot554B7PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B7ObjectCount = 36;
inline constexpr std::size_t kSlot554B7BlockSize = 0x1cc8;
inline constexpr std::size_t kSlot554B7Written = 6087;
inline constexpr std::size_t kSlot554B7Holes = 1281;
inline constexpr std::size_t kSlot554B7InstructionCount = 1631;
inline constexpr std::size_t kSlot554B7DirectCallCount = 179;
inline constexpr std::size_t kSlot554B7IndirectCallCount = 27;
inline constexpr std::size_t kSlot554B7StoreCount = 508;
inline constexpr std::size_t kSlot554B7ObjectStoreCount = 137;
inline constexpr std::size_t kSlot554B7StackStoreCount = 371;
inline constexpr std::size_t kSlot554B7BoolCount = 9;
inline constexpr std::size_t kSlot554B7BoolWritten = 162;
inline constexpr std::size_t kSlot554B7BoolHoles = 30;
inline constexpr std::size_t kSlot554B7FloatCount = 19;
inline constexpr std::size_t kSlot554B7FloatWritten = 171;
inline constexpr std::size_t kSlot554B7FloatHoles = 37;
inline constexpr std::size_t kSlot554B7IntCount = 4;
inline constexpr std::size_t kSlot554B7IntWritten = 165;
inline constexpr std::size_t kSlot554B7IntHoles = 43;
inline constexpr std::size_t kSlot554B7DwordCount = 3;
inline constexpr std::size_t kSlot554B7DwordWritten = 175;
inline constexpr std::size_t kSlot554B7DwordHoles = 41;
inline constexpr std::size_t kSlot554B7WrapperCount = 1;
inline constexpr std::size_t kSlot554B7WrapperWritten = 195;
inline constexpr std::size_t kSlot554B7WrapperHoles = 13;
static_assert(kSlot554B7BoolCount + kSlot554B7FloatCount + kSlot554B7IntCount +
                  kSlot554B7DwordCount + kSlot554B7WrapperCount == kSlot554B7ObjectCount);
static_assert(kSlot554B7BoolWritten + kSlot554B7BoolHoles == 0xc0);
static_assert(kSlot554B7FloatWritten + kSlot554B7FloatHoles == 0xd0);
static_assert(kSlot554B7IntWritten + kSlot554B7IntHoles == 0xd0);
static_assert(kSlot554B7DwordWritten + kSlot554B7DwordHoles == 0xd8);
static_assert(kSlot554B7WrapperWritten + kSlot554B7WrapperHoles == 0xd0);
static_assert(kSlot554B7BoolCount * 0xc0 +
                  (kSlot554B7FloatCount + kSlot554B7IntCount + kSlot554B7WrapperCount) * 0xd0 +
                  kSlot554B7DwordCount * 0xd8 == kSlot554B7BlockSize);
static_assert(kSlot554B7BoolCount * kSlot554B7BoolWritten +
                  kSlot554B7FloatCount * kSlot554B7FloatWritten +
                  kSlot554B7IntCount * kSlot554B7IntWritten +
                  kSlot554B7DwordCount * kSlot554B7DwordWritten +
                  kSlot554B7WrapperCount * kSlot554B7WrapperWritten == kSlot554B7Written);
static_assert(kSlot554B7BoolCount * kSlot554B7BoolHoles +
                  kSlot554B7FloatCount * kSlot554B7FloatHoles +
                  kSlot554B7IntCount * kSlot554B7IntHoles +
                  kSlot554B7DwordCount * kSlot554B7DwordHoles +
                  kSlot554B7WrapperCount * kSlot554B7WrapperHoles == kSlot554B7Holes);
static_assert(kSlot554B7Written + kSlot554B7Holes == kSlot554B7BlockSize);
static_assert(kSlot554B7StoreCount == kSlot554B7ObjectStoreCount + kSlot554B7StackStoreCount);

inline constexpr std::uint64_t kSlot554B7TableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B7TableIndex = 554;
inline constexpr std::uint64_t kSlot554B7RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B7ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B7AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B7DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B7BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B7BoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B7BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B7BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B7BoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B7FloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B7FloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B7FloatClassVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot554B7FloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B7FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B7IntFinalVptr = 0x1594438;
inline constexpr std::uint64_t kSlot554B7IntArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B7IntCtorRva = 0x3a980;
inline constexpr std::uint64_t kSlot554B7IntDtorRva = 0xad70;
inline constexpr std::uint64_t kSlot554B7DwordVptrTable = 0x1594bf8;
inline constexpr std::uint64_t kSlot554B7DwordFinalVptr = 0x1594c08;  // table + 0x10
inline constexpr std::uint64_t kSlot554B7DwordCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot554B7DwordDtorRva = 0x18a60;
inline constexpr std::size_t kSlot554B7DwordCellOffset = 0xd0;
inline constexpr std::size_t kSlot554B7WrapperSize = 0x190;
inline constexpr std::size_t kSlot554B7InnerSize = 0x160;
inline constexpr std::uint64_t kSlot554B7WrapperRva = 0x6d300;
inline constexpr std::uint64_t kSlot554B7WrapperInnerRva = 0x6d490;
inline constexpr std::uint64_t kSlot554B7WrapperInnerVptr = 0x15963a8;
inline constexpr std::uint64_t kSlot554B7WrapperFinalVptr = 0x1596f70;
inline constexpr std::uint64_t kSlot554B7WrapperTagVptr = 0x1596f40;
inline constexpr std::uint64_t kSlot554B7WrapperDtorRva = 0x54e60;
inline constexpr std::uint64_t kSlot554B7CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot554B7StackChkFailRva = 0xf8f3a0;

enum class Slot554B7Kind : std::uint8_t { bool_var, float_var, int_var, dword_var, wrapper };

struct Slot554B7Spec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    Slot554B7Kind kind;
    std::uint16_t page_cell_offset;  // 0 for dword_var: its value cell is object-hosted
    std::uint32_t low;
    std::uint32_t high;
    std::uint64_t wrapper_rva;  // 0 unless kind == wrapper
    std::uint64_t inner_rva;
    std::uint64_t inner_vptr;
    std::uint64_t final_vptr;
    std::uint64_t tag_vptr;
    std::uint64_t dtor_rva;
};

inline constexpr Slot554B7Spec kSlot554B7Specs[kSlot554B7ObjectCount] = {
    {"samplerLodBias", 14, 0x2eb2ba0, 0xd0, Slot554B7Kind::float_var, 0x1dc, 0xc0800000,
     0x40800000, 0, 0, 0, 0, 0, 0},
    {"samplerAnisoThreshold", 21, 0x2eb2c70, 0xd0, Slot554B7Kind::int_var, 0x1e0, 0, 0, 0, 0, 0,
     0, 0, 0},
    {"linearSamplerAnisoLevel", 23, 0x2eb2d40, 0xd0, Slot554B7Kind::int_var, 0x1e4, 0, 0, 0, 0, 0,
     0, 0, 0},
    {"progSamplerAnisoLevel", 21, 0x2eb2e10, 0xd0, Slot554B7Kind::int_var, 0x1e8, 0, 0, 0, 0, 0, 0,
     0, 0},
    {"progSamplerAnisoLevel_Cinematic", 31, 0x2eb2ee0, 0xd8, Slot554B7Kind::dword_var, 0, 0,
     0x10, 0, 0, 0, 0, 0, 0},
    {"progSamplerAnisoLevel_BasePS4", 29, 0x2eb2fb8, 0xd8, Slot554B7Kind::dword_var, 0, 0, 0x10,
     0, 0, 0, 0, 0, 0},
    {"progSamplerAnisoLevel_Perf", 26, 0x2eb3090, 0xd8, Slot554B7Kind::dword_var, 0, 0, 0x10, 0,
     0, 0, 0, 0, 0},
    {"noAnisoSamplerAnisoLevel", 24, 0x2eb3168, 0xd0, Slot554B7Kind::int_var, 0x1ec, 0, 0, 0, 0,
     0, 0, 0, 0},
    {"fog_enable", 10, 0x2eb3238, 0xc0, Slot554B7Kind::bool_var, 0x1f0, 0, 0, 0, 0, 0, 0, 0, 0},
    {"fog_enableAsyncCompute", 22, 0x2eb32f8, 0xc0, Slot554B7Kind::bool_var, 0x1f1, 0, 0, 0, 0, 0,
     0, 0, 0},
    {"fog_enableDither", 16, 0x2eb33b8, 0xc0, Slot554B7Kind::bool_var, 0x1f2, 0, 0, 0, 0, 0, 0, 0,
     0},
    {"fog_enableParticles", 19, 0x2eb3478, 0xc0, Slot554B7Kind::bool_var, 0x1f3, 0, 0, 0, 0, 0, 0,
     0, 0},
    {"fog_apply", 9, 0x2eb3538, 0xc0, Slot554B7Kind::bool_var, 0x1f4, 0, 0, 0, 0, 0, 0, 0, 0},
    {"fog_superSampleMode", 19, 0x2eb35f8, 0xd0, Slot554B7Kind::wrapper, 0x1f8, 0, 2,
     kSlot554B7WrapperRva, kSlot554B7WrapperInnerRva, kSlot554B7WrapperInnerVptr,
     kSlot554B7WrapperFinalVptr, kSlot554B7WrapperTagVptr, kSlot554B7WrapperDtorRva},
    {"fog_smoothing", 13, 0x2eb36c8, 0xd0, Slot554B7Kind::float_var, 0x1fc, 0xbf800000, 0x3f800000,
     0, 0, 0, 0, 0, 0},
    {"fog_rangeFactor", 15, 0x2eb3798, 0xd0, Slot554B7Kind::float_var, 0x200, 0, 0x42c80000, 0, 0,
     0, 0, 0, 0},
    {"fog_rangePowFactor", 18, 0x2eb3868, 0xd0, Slot554B7Kind::float_var, 0x204, 0, 0x41200000, 0,
     0, 0, 0, 0, 0},
    {"fog_particleRangeFactor", 23, 0x2eb3938, 0xd0, Slot554B7Kind::float_var, 0x208, 0,
     0x42c80000, 0, 0, 0, 0, 0, 0},
    {"fog_particleRangePowFactor", 26, 0x2eb3a08, 0xd0, Slot554B7Kind::float_var, 0x20c, 0,
     0x41200000, 0, 0, 0, 0, 0, 0},
    {"fog_density", 11, 0x2eb3ad8, 0xd0, Slot554B7Kind::float_var, 0x210, 0, 0x447a0000, 0, 0, 0,
     0, 0, 0},
    {"fog_scatterDirectionality", 25, 0x2eb3ba8, 0xd0, Slot554B7Kind::float_var, 0x214, 0xbf800000,
     0x3f800000, 0, 0, 0, 0, 0, 0},
    {"fog_dirScatterDirectionality", 28, 0x2eb3c78, 0xd0, Slot554B7Kind::float_var, 0x218,
     0xbf800000, 0x3f800000, 0, 0, 0, 0, 0, 0},
    {"fog_occlusionFudgeScale", 23, 0x2eb3d48, 0xd0, Slot554B7Kind::float_var, 0x21c, 0x3f800000,
     0x3fa00000, 0, 0, 0, 0, 0, 0},
    {"fog_occlusionFudgeOffset", 24, 0x2eb3e18, 0xd0, Slot554B7Kind::float_var, 0x220, 0,
     0x41200000, 0, 0, 0, 0, 0, 0},
    {"fog_darkEffectRate", 18, 0x2eb3ee8, 0xd0, Slot554B7Kind::float_var, 0x224, 0, 0x3f800000, 0,
     0, 0, 0, 0, 0},
    {"fog_darkEffectClamp", 19, 0x2eb3fb8, 0xd0, Slot554B7Kind::float_var, 0x228, 0, 0x3f800000,
     0, 0, 0, 0, 0, 0},
    {"fog_noiseFrequency1", 19, 0x2eb4088, 0xd0, Slot554B7Kind::float_var, 0x234, 0, 0x3f800000,
     0, 0, 0, 0, 0, 0},
    {"fog_noiseFrequency2", 19, 0x2eb4158, 0xd0, Slot554B7Kind::float_var, 0x238, 0, 0x3f8147ae,
     0, 0, 0, 0, 0, 0},
    {"fog_noiseSpeed1", 15, 0x2eb4228, 0xd0, Slot554B7Kind::float_var, 0x23c, 0, 0x42c80000, 0, 0,
     0, 0, 0, 0},
    {"fog_noiseSpeed2", 15, 0x2eb42f8, 0xd0, Slot554B7Kind::float_var, 0x240, 0, 0x42c80000, 0, 0,
     0, 0, 0, 0},
    {"waterFog_range", 14, 0x2eb43c8, 0xd0, Slot554B7Kind::float_var, 0x22c, 0x41200000,
     0x459c4000, 0, 0, 0, 0, 0, 0},
    {"waterFog_rangePow", 17, 0x2eb4498, 0xd0, Slot554B7Kind::float_var, 0x230, 0x3dcccccd,
     0x40800000, 0, 0, 0, 0, 0, 0},
    {"r_waterEnableGeo", 16, 0x2eb4568, 0xc0, Slot554B7Kind::bool_var, 0x244, 0, 0, 0, 0, 0, 0, 0,
     0},
    {"r_waterEnableSim", 16, 0x2eb4628, 0xc0, Slot554B7Kind::bool_var, 0x245, 0, 0, 0, 0, 0, 0, 0,
     0},
    {"r_waterEnableAsyncCompute", 26, 0x2eb46e8, 0xc0, Slot554B7Kind::bool_var, 0x246, 0, 0, 0, 0,
     0, 0, 0, 0},
    {"r_waterEnableCameraSnap", 23, 0x2eb47a8, 0xc0, Slot554B7Kind::bool_var, 0x247, 0, 0, 0, 0, 0,
     0, 0, 0},
};

struct alignas(8) Slot554Batch7Objects {
    std::array<std::uint8_t, kSlot554B7BlockSize> objects{};
};
static_assert(sizeof(Slot554Batch7Objects) == kSlot554B7BlockSize);

struct Slot554Batch7Result;
using Slot554B7RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B7AtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                    ConCmdRegistry& registry) noexcept;
using Slot554B7ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B7WrapperHook = bool (*)(void* context, std::uint64_t wrapper_rva,
                                      ConCmdFunctor& object) noexcept;
using Slot554B7ViewDestroyHook = bool (*)(void* context, const ConCmdFunctorView& view) noexcept;

struct Slot554B7Hooks {
    Slot554B7RegisterHook register_var = nullptr;
    Slot554B7AtexitHook atexit = nullptr;
    Slot554B7ReleaseHook release_view = nullptr;
    Slot554B7WrapperHook wrapper_call = nullptr;       // per-wrapper outer RVA
    Slot554B7ViewDestroyHook destroy_view = nullptr;   // body's `call [view + 0x18]`
    void* context = nullptr;
};

enum class Slot554B7Stage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554Batch7Result {
    Slot554B7Stage blocked = Slot554B7Stage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t wrapper_calls = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_batch7_object_written(std::uint32_t index,
                                                 std::size_t offset) noexcept;
[[nodiscard]] bool slot554_batch7_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B7Hooks default_slot554_batch7_hooks() noexcept;
[[nodiscard]] Slot554Batch7Result init_slot554_batch7(
    Slot554Batch7Objects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B7Hooks& hooks) noexcept;

}  // namespace sotc
