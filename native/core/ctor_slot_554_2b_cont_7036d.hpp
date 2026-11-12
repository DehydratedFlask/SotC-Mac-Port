// CPU-01-STARTUP-SLOT-554-BATCH-02B-CONT: native model of the remaining 23-object body
// [0x7036d,0x718bb) = 0x154e B in slot 554. Objects 0x2eaa9e0..0x2eabb80 = 0x11a0 B:
// six 0xd0 typed objects and seventeen 0xc0 bool objects. This closes batch 2b without a second
// prefix; slot 554 as a whole remains PARTIAL because later batches are still open.
//
// Object families (all pins are original identity tags, never executed):
//   * ShowRenderTarget / Show Ambient RenderTarget: 0x6af60 outer wrapper (inner 0x6b0f0),
//     final vptr 0x1596980, caller/tag 0x1596950, dtor 0x54ba0.
//   * ShaderDebugCollisionMode: 0x6b2f0 wrapper family entry (inner 0x6b480), final vptr
//     0x1596a18 (table 0x1596a08 + 0x10), tag 0x15969e8, dtor 0x54c20.
//   * ShaderDebugMode: 0x6b680 wrapper family entry (inner 0x6b810), final vptr 0x1596ab0
//     (table 0x1596aa0 + 0x10), tag 0x1596a80, dtor 0x54c60.
//   * RenderTargetDisplayMip: 0x3a980 unbounded int worker, final vptr 0x1594438, dtor 0xad70.
//   * miscDebugShaderFloat0: 0xc300 bounded float worker, final vptr 0x15943c0, dtor 0xad30.
//   * The seventeen 0xc0 bools use the batch-2a shape; each has its own page cell.
//
// Byte accounting: wrapper objects 195 written / 13 holes (the 0x6af60/0x6b2f0/0x6b680 family
// has the same observable ConCmdFunctor mask); the unbounded int is 165 / 43 (the batch-1 ranged
// shape) and the bounded float is 171 / 37 because 0xc300 stores both bounds as full dwords
// (+0x6c..+0x70, +0x74..+0x78) where the unbounded twin wrote one byte of each; bools 162 / 30.
// Batch total 3870 written / 642 holes of 0x11a0.
//
// Body measurement: 955 instructions, 111 direct calls + 6 indirect, 332 memory stores
// (157 object, 175 stack), no conditional branch. Wrapper transfers and view destroys are host
// hooks; the native model never calls an original RVA.
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

namespace sotc {

inline constexpr std::uint64_t kSlot554B2BContRva = 0x7036d;
inline constexpr std::size_t kSlot554B2BContBodySize = 0x154e;
inline constexpr std::uint64_t kSlot554B2BContObjectBaseRva = 0x2eaa9e0;
inline constexpr std::uint64_t kSlot554B2BContPageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554B2BContObjectCount = 23;
inline constexpr std::size_t kSlot554B2BContBlockSize = 0x11a0;
inline constexpr std::size_t kSlot554B2BContWritten = 3870;
inline constexpr std::size_t kSlot554B2BContHoles = 642;
inline constexpr std::size_t kSlot554B2BContInstructionCount = 955;
inline constexpr std::size_t kSlot554B2BContDirectCallCount = 111;
inline constexpr std::size_t kSlot554B2BContIndirectCallCount = 6;
inline constexpr std::size_t kSlot554B2BContStoreCount = 332;
inline constexpr std::size_t kSlot554B2BContObjectStoreCount = 157;
inline constexpr std::size_t kSlot554B2BContStackStoreCount = 175;
inline constexpr std::size_t kSlot554B2BContWrapperWritten = 195;
inline constexpr std::size_t kSlot554B2BContWrapperHoles = 13;
inline constexpr std::size_t kSlot554B2BContRangedWritten = 165;   // unbounded int shape
inline constexpr std::size_t kSlot554B2BContRangedHoles = 43;
inline constexpr std::size_t kSlot554B2BContFloatWritten = 171;    // bounded float: full dword bounds
inline constexpr std::size_t kSlot554B2BContFloatHoles = 37;
inline constexpr std::size_t kSlot554B2BContBoolWritten = 162;
inline constexpr std::size_t kSlot554B2BContBoolHoles = 30;
inline constexpr std::size_t kSlot554B2BContWrapperCount = 4;
inline constexpr std::size_t kSlot554B2BContRangedCount = 2;
inline constexpr std::size_t kSlot554B2BContBoolCount = 17;
static_assert(kSlot554B2BContWrapperCount + kSlot554B2BContRangedCount +
                  kSlot554B2BContBoolCount == kSlot554B2BContObjectCount);
static_assert(kSlot554B2BContWrapperWritten + kSlot554B2BContWrapperHoles == 0xd0);
static_assert(kSlot554B2BContRangedWritten + kSlot554B2BContRangedHoles == 0xd0);
static_assert(kSlot554B2BContFloatWritten + kSlot554B2BContFloatHoles == 0xd0);
static_assert(kSlot554B2BContBoolWritten + kSlot554B2BContBoolHoles == 0xc0);
static_assert(kSlot554B2BContWrapperCount * 0xd0 + kSlot554B2BContRangedCount * 0xd0 +
                  kSlot554B2BContBoolCount * 0xc0 == kSlot554B2BContBlockSize);
static_assert(kSlot554B2BContWrapperCount * kSlot554B2BContWrapperWritten +
                  kSlot554B2BContRangedWritten + kSlot554B2BContFloatWritten +
                  kSlot554B2BContBoolCount * kSlot554B2BContBoolWritten ==
              kSlot554B2BContWritten);
static_assert(kSlot554B2BContWrapperCount * kSlot554B2BContWrapperHoles +
                  kSlot554B2BContRangedHoles + kSlot554B2BContFloatHoles +
                  kSlot554B2BContBoolCount * kSlot554B2BContBoolHoles ==
              kSlot554B2BContHoles);
static_assert(kSlot554B2BContWritten + kSlot554B2BContHoles == kSlot554B2BContBlockSize);

inline constexpr std::uint64_t kSlot554B2BContTableWordRva = 0x2dd5f90;  // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554B2BContTableIndex = 554;
inline constexpr std::uint64_t kSlot554B2BContRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554B2BContReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554B2BContAtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554B2BContDsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554B2BContBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554B2BContBoolFinalVptr = 0x1594348;
inline constexpr std::uint64_t kSlot554B2BContBoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot554B2BContBoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554B2BContBoolDtorRva = 0xacf0;
inline constexpr std::uint64_t kSlot554B2BContIntFinalVptr = 0x1594438;
inline constexpr std::uint64_t kSlot554B2BContFloatFinalVptr = 0x15943c0;
inline constexpr std::uint64_t kSlot554B2BContIntArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554B2BContFloatArgFnVtbl = 0x1596878;
inline constexpr std::uint64_t kSlot554B2BContIntCtorRva = 0x3a980;
inline constexpr std::uint64_t kSlot554B2BContFloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot554B2BContIntDtorRva = 0xad70;
inline constexpr std::uint64_t kSlot554B2BContFloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554B2BContWrapper6af60Rva = 0x6af60;
inline constexpr std::uint64_t kSlot554B2BContWrapper6b2f0Rva = 0x6b2f0;
inline constexpr std::uint64_t kSlot554B2BContWrapper6b680Rva = 0x6b680;
inline constexpr std::uint64_t kSlot554B2BContInner6b0f0Rva = 0x6b0f0;
inline constexpr std::uint64_t kSlot554B2BContInner6b480Rva = 0x6b480;
inline constexpr std::uint64_t kSlot554B2BContInner6b810Rva = 0x6b810;
inline constexpr std::uint64_t kSlot554B2BContInner6af60Vptr = 0x1595f08;
inline constexpr std::uint64_t kSlot554B2BContInner6b2f0Vptr = 0x1595f80;
inline constexpr std::uint64_t kSlot554B2BContInner6b680Vptr = 0x1595ff8;
inline constexpr std::uint64_t kSlot554B2BContWrapper6af60FinalVptr = 0x1596980;
inline constexpr std::uint64_t kSlot554B2BContWrapper6b2f0FinalVptr = 0x1596a18;  // table + 0x10
inline constexpr std::uint64_t kSlot554B2BContWrapper6b680FinalVptr = 0x1596ab0;  // table + 0x10

enum class Slot554B2BContKind : std::uint8_t { wrapper, int_var, float_var, bool_var };

struct Slot554B2BContSpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint16_t size;
    std::uint16_t page_cell_offset;
    Slot554B2BContKind kind;
    std::uint64_t wrapper_rva;
    std::uint64_t inner_vptr;
    std::uint64_t final_vptr;
    std::uint64_t tag_vptr;
    std::uint64_t dtor_rva;
    std::uint32_t low;
    std::uint32_t high;
};

inline constexpr Slot554B2BContSpec kSlot554B2BContSpecs[kSlot554B2BContObjectCount] = {
    {"ShowRenderTarget", 16, 0x2eaa9e0, 0xd0, 0x3c, Slot554B2BContKind::wrapper,
     0x6af60, 0x1595f08, 0x1596980, 0x1596950, 0x54ba0, 0, 0x24},
    {"RenderTargetDisplayMip", 22, 0x2eaaab0, 0xd0, 0x40, Slot554B2BContKind::int_var,
     0x3a980, 0x0, 0x1594438, 0x1596858, 0xad70, 0, 0},
    {"Show Ambient RenderTarget", 25, 0x2eaab80, 0xd0, 0x3c, Slot554B2BContKind::wrapper,
     0x6af60, 0x1595f08, 0x1596980, 0x1596950, 0x54ba0, 0xf, 0xf},
    {"ShaderDebugCollisionMode", 24, 0x2eaac50, 0xd0, 0x48, Slot554B2BContKind::wrapper,
     0x6b2f0, 0x1595f80, 0x1596a18, 0x15969e8, 0x54c20, 0, 2},
    {"ShowCollisionWireframe", 22, 0x2eaad20, 0xc0, 0xa8d, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"OnlyRenderSpecialScreenFX", 25, 0x2eaade0, 0xc0, 0xa8e, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"ShaderDebugMode", 15, 0x2eaaea0, 0xd0, 0x44, Slot554B2BContKind::wrapper,
     0x6b680, 0x1595ff8, 0x1596ab0, 0x1596a80, 0x54c60, 0, 0x1c},
    {"ShowPostGradient", 16, 0x2eaaf70, 0xc0, 0xa8f, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"enableForceClear", 16, 0x2eab030, 0xc0, 0x0, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"enableRandomClearColor", 22, 0x2eab0f0, 0xc0, 0x1, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"enableOccluderRandomClearColor", 30, 0x2eab1b0, 0xc0, 0x2, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"miscDebugShaderFloat0", 21, 0x2eab270, 0xd0, 0xc, Slot554B2BContKind::float_var,
     0xc300, 0x0, 0x15943c0, 0x1596878, 0xad30, 0, 0x43480000},
    {"disableDynamicGeometry", 22, 0x2eab340, 0xc0, 0x3, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"renderOnlyDefaultMaterials", 26, 0x2eab400, 0xc0, 0x4, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"renderOnlySpotShadowGroupDefault", 32, 0x2eab4c0, 0xc0, 0x5, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"renderOnlySpotShadowGroupDynamic", 32, 0x2eab580, 0xc0, 0x6, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"renderOnlySpotShadowGroupCustomA", 32, 0x2eab640, 0xc0, 0x7, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"renderOnlySpotShadowGroupCustomB", 32, 0x2eab700, 0xc0, 0x8, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"disableAAInSpecialDebugModes", 28, 0x2eab7c0, 0xc0, 0x4c, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"drawJustOccluders", 17, 0x2eab880, 0xc0, 0x4d, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"drawJustShadowCasters", 21, 0x2eab940, 0xc0, 0x4e, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"drawJustRainBlockers", 20, 0x2eaba00, 0xc0, 0x4f, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
    {"drawJustParticleBlockers", 24, 0x2eabac0, 0xc0, 0x50, Slot554B2BContKind::bool_var,
     0x0, 0x0, 0x1594348, 0x1596898, 0xacf0, 0, 0},
};

struct alignas(8) Slot554B2BContObjects {
    std::array<std::uint8_t, kSlot554B2BContBlockSize> objects{};
};
static_assert(sizeof(Slot554B2BContObjects) == kSlot554B2BContBlockSize);

struct Slot554B2BContResult;
using Slot554B2BContRegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554B2BContAtexitHook = bool (*)(std::uint64_t dtor, const void* object,
                                         ConCmdRegistry& registry) noexcept;
using Slot554B2BContReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554B2BContTransferHook = bool (*)(void* context, ConCmdFunctor& object,
                                           unsigned stage) noexcept;
using Slot554B2BContViewDestroyHook = bool (*)(void* context,
                                               const ConCmdFunctorView& view) noexcept;

struct Slot554B2BContHooks {
    Slot554B2BContRegisterHook register_var = nullptr;
    Slot554B2BContAtexitHook atexit = nullptr;
    Slot554B2BContReleaseHook release_view = nullptr;
    Slot554B2BContTransferHook transfer = nullptr;
    Slot554B2BContViewDestroyHook destroy_view = nullptr;
    void* context = nullptr;
};

enum class Slot554B2BContStage : std::uint8_t {
    none, page, ctor, registration, view_release, atexit, done
};

struct Slot554B2BContResult {
    Slot554B2BContStage blocked = Slot554B2BContStage::none;
    bool ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t transfers = 0;
    std::uint32_t view_destroys = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_b2b_cont_object_written(std::uint32_t index,
                                                    std::size_t offset) noexcept;
[[nodiscard]] bool slot554_b2b_cont_written(std::size_t offset) noexcept;
[[nodiscard]] Slot554B2BContHooks default_slot554_b2b_cont_hooks() noexcept;
[[nodiscard]] Slot554B2BContResult init_slot554_b2b_cont(
    Slot554B2BContObjects& objects, const std::uint8_t* page, ConCmdRegistry& registry,
    const Slot554B2BContHooks& hooks) noexcept;

}  // namespace sotc
