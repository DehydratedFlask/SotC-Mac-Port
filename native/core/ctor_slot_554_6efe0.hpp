// CPU-01-STARTUP-SLOT-554-BATCH-01: native model of constructor-table index 554 / RVA 0x6efe0.
//
// Batch 1 covers the page header `0x2ea8ee8..0x2ea99b0` (0xac8 B, built by the straight-line
// initialiser 0x53fc0) and the first eight console variables `0x2ea99b0..0x2eaa010` (0x660 B):
// two unbounded ints, four unbounded floats and two bools. The body prefix is
// [0x6efe0,0x6f793), ending at the `leaq` of the ninth object (`0x2eaa010`). The whole slot is
// 112,184 B, so this module is deliberately PARTIAL and the startup row remains partially lifted.
//
// Offline only. No original RVA is loaded, relocated or executed. The page image is reproduced by
// the exact 187 stores of 0x53fc0 (`kSlot554PageStores`), and each object is built through the
// already-lifted constructor family (0x3a980 / 0x6aa00 / 0x20af0), followed by the stores the slot
// body itself performs (final vptr, +0x68 value, 0x20d60, both 0xf590 releases, atexit).
//
// Object map (batch-relative, verified against the body's `leaq` sequence):
//   +0x0000  page        0xac8  (187 stores: 2113 written, 647 holes)
//   +0x0ac8  int        0x0d0  backgroundColor   (page+0x24, value 0, dtor 0xad70)
//   +0x0b98  int        0x0d0  gpuMarkerLevel    (page+0x28, value 1, dtor 0xad70)
//   +0x0c68  float      0x0d0  globalRenderVarX  (page+0x2c, 0.0f, dtor 0xad30)
//   +0x0d38  float      0x0d0  globalRenderVarY  (page+0x30, 0.0f)
//   +0x0e08  float      0x0d0  globalRenderVarZ  (page+0x34, 0.0f)
//   +0x0ed8  float      0x0d0  globalRenderVarW  (page+0x38, 0.0f)
//   +0x0fa8  bool       0x0c0  profileEnableGpu (page+0x10, value 0, dtor 0xacf0)
//   +0x1068  bool       0x0c0  groupCulling      (page+0x11, value 1, dtor 0xacf0)
//   +0x1128  end
//
// Native byte map: 3427 written / 965 holes over the 4392-byte batch. The object holes are the
// exact bytes no constructor or body store touches: base-list link +0x08..+0x18, +0x19..+0x1c,
// unengaged bound payload tails, functor inline buffers and the ranged aux gap. `slot554_written`
// is the oracle used by the fixture's sentinel sweep.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot554Rva = 0x6efe0;
inline constexpr std::size_t kSlot554BodySize = 112184;             // FDE [0x6efe0, 0x8a618)
inline constexpr std::uint64_t kSlot554Batch1EndRva = 0x6f793;      // first instruction of object 9
inline constexpr std::size_t kSlot554Batch1BodyBytes = 0x7b3;
inline constexpr std::uint64_t kSlot554PageRva = 0x2ea8ee8;
inline constexpr std::size_t kSlot554PageSize = 0xac8;
inline constexpr std::uint64_t kSlot554BlockRva = kSlot554PageRva;
inline constexpr std::size_t kSlot554BlockSize = 0x1128;            // page + 8 objects
inline constexpr std::size_t kSlot554ObjectOffset = 0xac8;
inline constexpr std::size_t kSlot554VariableCount = 8;
inline constexpr std::size_t kSlot554PageStoreCount = 187;
inline constexpr std::size_t kSlot554PageWritten = 2113;
inline constexpr std::size_t kSlot554PageHoles = 647;
inline constexpr std::size_t kSlot554RangedSize = 0xd0;
inline constexpr std::size_t kSlot554BoolSize = 0xc0;
inline constexpr std::size_t kSlot554RangedWritten = 165;
inline constexpr std::size_t kSlot554RangedHoles = 43;
inline constexpr std::size_t kSlot554BoolWritten = 162;
inline constexpr std::size_t kSlot554BoolHoles = 30;
inline constexpr std::size_t kSlot554BlockWritten = 3427;
inline constexpr std::size_t kSlot554BlockHoles = 965;
static_assert(kSlot554PageWritten + kSlot554PageHoles == kSlot554PageSize);
static_assert(6 * kSlot554RangedWritten + 2 * kSlot554BoolWritten == 1314);
static_assert(6 * kSlot554RangedHoles + 2 * kSlot554BoolHoles == 318);
static_assert(kSlot554PageWritten + 1314 == kSlot554BlockWritten);
static_assert(kSlot554PageHoles + 318 == kSlot554BlockHoles);
static_assert(kSlot554ObjectOffset + 6 * kSlot554RangedSize + 2 * kSlot554BoolSize == kSlot554BlockSize);

inline constexpr std::uint64_t kSlot554TableWordRva = 0x2dd5f90;    // 0x2dd70e0 - 554*8
inline constexpr std::uint32_t kSlot554TableIndex = 554;
inline constexpr std::uint64_t kSlot554PageInitRva = 0x53fc0;
inline constexpr std::size_t kSlot554PageInitSize = 0xb9e;         // [0x53fc0,0x54b5e)
inline constexpr std::uint64_t kSlot554EmptyTextRva = 0xf9a8d3;

inline constexpr std::uint64_t kSlot554IntCtorRva = 0x3a980;
inline constexpr std::uint64_t kSlot554FloatCtorRva = 0x6aa00;
inline constexpr std::uint64_t kSlot554BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot554RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot554ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot554AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot554DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot554IntDtorRva = 0xad70;
inline constexpr std::uint64_t kSlot554FloatDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot554BoolDtorRva = 0xacf0;

inline constexpr std::uint64_t kSlot554IntFinalVptr = 0x1594438;   // 0x1594428 + 0x10
inline constexpr std::uint64_t kSlot554FloatFinalVptr = 0x15943c0; // 0x15943b0 + 0x10
inline constexpr std::uint64_t kSlot554BoolMidVptr = 0x15940b0;    // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot554BoolFinalVptr = 0x1594348;  // 0x1594338 + 0x10
inline constexpr std::uint64_t kSlot554BoolFnVtbl = 0x1596898;
inline constexpr std::uint64_t kSlot554IntArgFnVtbl = 0x1596858;
inline constexpr std::uint64_t kSlot554FloatArgFnVtbl = 0x1596878;

enum class Slot554Kind : std::uint8_t { int_var, float_var, bool_var };

struct Slot554VarSpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t object_rva;
    std::uint64_t page_cell_offset;
    Slot554Kind kind;
    std::uint16_t size;
    std::uint64_t final_vptr;
    std::uint64_t dtor_rva;
};

inline constexpr Slot554VarSpec kSlot554Vars[kSlot554VariableCount] = {
    {"backgroundColor", 15, 0x2ea99b0, 0x24, Slot554Kind::int_var, 0xd0, kSlot554IntFinalVptr, kSlot554IntDtorRva},
    {"gpuMarkerLevel", 14, 0x2ea9a80, 0x28, Slot554Kind::int_var, 0xd0, kSlot554IntFinalVptr, kSlot554IntDtorRva},
    {"globalRenderVarX", 16, 0x2ea9b50, 0x2c, Slot554Kind::float_var, 0xd0, kSlot554FloatFinalVptr, kSlot554FloatDtorRva},
    {"globalRenderVarY", 16, 0x2ea9c20, 0x30, Slot554Kind::float_var, 0xd0, kSlot554FloatFinalVptr, kSlot554FloatDtorRva},
    {"globalRenderVarZ", 16, 0x2ea9cf0, 0x34, Slot554Kind::float_var, 0xd0, kSlot554FloatFinalVptr, kSlot554FloatDtorRva},
    {"globalRenderVarW", 16, 0x2ea9dc0, 0x38, Slot554Kind::float_var, 0xd0, kSlot554FloatFinalVptr, kSlot554FloatDtorRva},
    {"profileEnableGpu", 16, 0x2ea9e90, 0x10, Slot554Kind::bool_var, 0xc0, kSlot554BoolFinalVptr, kSlot554BoolDtorRva},
    {"groupCulling", 12, 0x2ea9f50, 0x11, Slot554Kind::bool_var, 0xc0, kSlot554BoolFinalVptr, kSlot554BoolDtorRva},
};

struct alignas(8) Slot554Objects {
    std::array<std::uint8_t, kSlot554PageSize> page{};
    std::array<std::uint8_t, 6 * kSlot554RangedSize + 2 * kSlot554BoolSize> objects{};
};
static_assert(sizeof(Slot554Objects) == kSlot554BlockSize);

struct Slot554Result;
using Slot554PageInitHook = bool (*)(std::uint8_t* page, std::size_t size) noexcept;
using Slot554RegisterHook = bool (*)(ConCmdBase& base, ConCmdRegistry& registry) noexcept;
using Slot554AtexitHook = bool (*)(std::uint64_t dtor, const void* object, ConCmdRegistry& registry) noexcept;
using Slot554ReleaseHook = bool (*)(const SharedStr& view) noexcept;
using Slot554DestroyHook = bool (*)(std::uint64_t arg_fn_vtbl) noexcept;

struct Slot554Hooks {
    Slot554PageInitHook page_init = nullptr;
    Slot554RegisterHook register_var = nullptr;
    Slot554AtexitHook atexit = nullptr;
    Slot554ReleaseHook release_view = nullptr;
    Slot554DestroyHook destroy_arg = nullptr;
};

enum class Slot554Stage : std::uint8_t {
    none, page_init, ctor, arg_destroy, final_vptr, registration, view_release, atexit, done
};

struct Slot554Result {
    Slot554Stage blocked = Slot554Stage::none;
    bool ok = false;
    bool page_ok = false;
    std::uint32_t built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    std::uint32_t views_inert = 0;
    std::uint32_t functors_destroyed = 0;
    std::uint32_t blocked_index = 0;
};

[[nodiscard]] bool slot554_page_written(std::size_t offset) noexcept;
[[nodiscard]] bool slot554_object_written(std::uint32_t index, std::size_t offset) noexcept;
[[nodiscard]] bool slot554_written(std::size_t offset) noexcept;
void slot554_init_page(std::uint8_t* page, std::size_t size) noexcept;
[[nodiscard]] Slot554Hooks default_slot554_hooks() noexcept;
[[nodiscard]] Slot554Result init_slot554_batch1(Slot554Objects& objects, ConCmdRegistry& registry,
                                                const Slot554Hooks& hooks) noexcept;

}  // namespace sotc
