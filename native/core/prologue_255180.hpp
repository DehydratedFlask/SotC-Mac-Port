// Base-Initialize prologue `0x255180` (448 B).
//
// Early fragment (OBS 0x25519b..0x25525f) — E408:
//   alloc 0xa30 → ctor 0x264e50 (hook) → install cell 0x2dfd4b8
//   call 0x11e70 / 0x3df40 (hooks; default no-op)
//   alloc 0x40 → init_flag_object_335a0 → install cell 0x2ea4718
//   apply 0x335d0 (hook) → xchg_store_u32_357f0(flag, 1) → tls 0x35770 (hook)
//
// Tail fragment (OBS 0x255264..0x2552cd) — E409+:
//   alloc 0x98a90 → ctor 0x3be050 (lifted / or override hook) → self+0x3b58
//     (prev: dtor 0x3be390 hook + delete_)
//   alloc 0x40 → ctor 0x1ab550 (lifted E416 / or override) → install self+0x3b40
//     (prev: vslot+0x08 hook only — OBS has no delete_ on this path)
//
// NOT modelled: debug gates after 0x2552cd (0xba40 / stack-chk / vcalls);
// PLT bodies 0xf901a0 / 0xf901c0; ensure migrate; non-null list.
// PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include "allocator_front.hpp"
#include "base_initialize_255130.hpp"
#include "ctor_40_1ab550.hpp"
#include "ctor_98a90_3be050.hpp"
#include "init_flag_object_335a0.hpp"
#include "xchg_store_u32_357f0.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kPrologue255180Rva = 0x255180;
inline constexpr std::size_t kPrologue255180Size = 448;
inline constexpr std::size_t kPrologueA30Bytes = 0xa30;
inline constexpr std::uint64_t kPrologueA30CellRva = 0x2dfd4b8;

inline constexpr std::size_t kPrologue98a90Bytes = 0x98a90;
inline constexpr std::size_t kPrologueAppSlot3b58 = 0x3b58;
inline constexpr std::size_t kPrologueAppSlot3b40 = 0x3b40;
inline constexpr std::size_t kPrologueAppMinBytes = kPrologueAppSlot3b58 + sizeof(void*);
inline constexpr std::size_t kPrologueTail40Bytes = 0x40;

// OBS stack config blob fed to 0x335d0 (ymm from 0xfc3220 + trailing qwords).
struct PrologueFlagConfig255180 {
    std::array<std::uint64_t, 4> ymm_words{{0x800, 0x400, 0x80, 0x20}};  // LE qwords @ 0xfc3220
    std::uint64_t qword_4000 = 0x4000;
    std::uint64_t qword_10000 = 0x10000;
    std::uint64_t qword_pack = 0xfffffffe0000001eULL;
};

struct Prologue255180Hooks {
    void (*ctor_a30_264e50)(void* obj) = nullptr;           // required (early)
    void (*dtor_vslot08)(void* obj) = nullptr;              // destroy previous early cells
    void (*call_11e70)() = nullptr;                         // null → no-op (OBS null-gate)
    void (*call_3df40)() = nullptr;                         // null → no-op
    void (*apply_335d0)(void* flag_obj, const PrologueFlagConfig255180& cfg) = nullptr;
    void (*tls_35770)(void* flag_obj, std::uint8_t v) = nullptr;  // null → no-op
};

struct Prologue255180TailHooks {
    // If non-null, called instead of lifted init_98a90_3be050 (E409 stub path).
    void (*ctor_98a90_3be050)(void* obj) = nullptr;
    // When ctor_98a90_3be050 is null and use_lifted_ctor_98a90 is true, run E410 header.
    bool use_lifted_ctor_98a90 = false;
    Ctor98a90_3be050Hooks ctor_98a90_detail{};
    void (*dtor_98a90_3be390)(void* obj) = nullptr;  // previous self+0x3b58
    // If non-null, replaces lifted init_ctor_40_1ab550.
    void (*ctor_40_1ab550)(void* obj) = nullptr;
    Ctor40_1ab550Hooks ctor_40_detail{};
    void (*dtor_vslot08_3b40)(void* obj) = nullptr;  // previous self+0x3b40 (no delete_)
};

struct Prologue255180Result {
    bool ok = false;
    void* a30_obj = nullptr;
    void* flag_obj = nullptr;
    std::uint32_t flag_word_20 = 0;  // expect 1 after success
};

struct Prologue255180TailResult {
    bool ok = false;
    void* obj_98a90 = nullptr;
    void* obj_40 = nullptr;
};

// Process-wide cell modelling original qword @ 0x2dfd4b8.
[[nodiscard]] void*& prologue_a30_cell_2dfd4b8() noexcept;

[[nodiscard]] Prologue255180Result run_prologue_255180_early(
    AllocatorFront& alloc, const Prologue255180Hooks& hooks);

// Tail via tip AllocatorFront into app object slots. Requires self with at least
// kPrologueAppMinBytes. On failure leaves slots unchanged when possible.
[[nodiscard]] Prologue255180TailResult run_prologue_255180_tail(
    AllocatorFront& alloc, void* self, const Prologue255180TailHooks& hooks);

}  // namespace sotc
